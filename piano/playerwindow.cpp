#include "playerwindow.h"
#include "channeldialog.h"
#include "ui_playerwindow.h"
#include "ledspanel.h"

PlayerWindow::PlayerWindow(QMidiOut* mainMidiOut, QWidget *parent)
    : QWidget(parent),
    ui(new Ui::PlayerWindow), midiOut(mainMidiOut)
{
    //m_channelActivity
    m_channelActivity = QVector<bool>(16, false);
    m_channelVolumes = QVector<int>(16, 100); // по умолчанию 100%
    m_preSoloVolumes = QVector<int>(16, 100); // По умолчанию 100%
    m_masterVolume = 100; //звук от ползунка плеера

    ui->setupUi(this);

    //visualizationPanel
    m_visualizationPanel = new LedsPanel(this, this);
    if (!m_visualizationPanel) {
        qCritical() << "Failed to create LedsPanel!";
    }

    // если visualizationWidget имеет layout
    if (!ui->visualizationWidget->layout()) {
        QVBoxLayout *layout = new QVBoxLayout(ui->visualizationWidget);
        ui->visualizationWidget->setLayout(layout);
        layout->setContentsMargins(2, 2, 2, 2);
    }

    //визуализация
    if (m_visualizationPanel) {
        ui->visualizationWidget->layout()->addWidget(m_visualizationPanel);
        m_visualizationPanel->setStyle("modern"); //дефолтный стиль
    }
    ui->visualizationWidget->setVisible(false); //по умолчанию визуализация скрыта

    connect(ui->homeButton, &QPushButton::clicked, this, &QWidget::close);

    // плейлист
    ui->Table->verticalHeader()->setVisible(false);
    ui->Table->setColumnCount(1);
    ui->Table->setHorizontalHeaderLabels(QStringList() << "File name");
    ui->Table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //двойной клик по файлу для игры
    connect(ui->Table, &QTableWidget::cellDoubleClicked, [this](int row, int column) {
        Q_UNUSED(column);
        if (row >= 0 && row < Paths.size()) {
            playFile(row);
        }
    });

    //фокус для обработки клавиатуры
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    //перемещение ползунка прогресса
    connect(ui->progressSlider, &QSlider::sliderPressed, [this]() {
        m_isSeeking = true;
        wasPlayingBeforeSeek = is_playing;

        // остановить таймер только если воспроизведение активно
        if (is_playing) playTimer.stop();
    });

    connect(ui->progressSlider, &QSlider::sliderMoved, [this](int value) {

        ui->progressSlider->setValue(value);// разрешаем перемотку даже на паузе
        // всегда обновляем время
        currentPlayPos = (value * totalDur) / 1000;
        ui->currentLabel->setText(QString::fromStdString(
            std::format("{:02d}:{:02d}",
                        (currentPlayPos/1000)/60,
                        (currentPlayPos/1000)%60)));
    });

    connect(ui->progressSlider, &QSlider::sliderReleased, [this]() {
        m_isSeeking = false;
        int position = ui->progressSlider->value();

        seekToPosition(position);//выполняем перемотку даже на паузе

        // возобновляем игру только если оно было активно
        if (wasPlayingBeforeSeek) playTimer.start();
        wasPlayingBeforeSeek = false;
    });

    // tooltip для слайдеров
    ui->volumeSlider->setToolTip("Master volume: 100%");
    ui->progressSlider->setToolTip("Playback position: 0%");

    connect(ui->volumeSlider, &QSlider::valueChanged, this, [this](int value) {
        ui->volumeSlider->setToolTip(QString("Master volume: %1%").arg(value));
    });

    connect(ui->progressSlider, &QSlider::valueChanged, this, [this](int value) {
        ui->progressSlider->setToolTip(QString("Playback position: %1%").arg(value / 10.0));
    });

    // меню выбора стиля
    QMenu* themeMenu = new QMenu(this);
    QActionGroup* themeGroup = new QActionGroup(this);

    auto addThemeAction = [&](const QString& name, Theme theme) {
        QAction* action = themeMenu->addAction(name);
        action->setCheckable(true);
        action->setChecked(currentTheme == theme);
        action->setData(static_cast<int>(theme));
        themeGroup->addAction(action);
    };

    addThemeAction("Modern", Theme::Modern);
    addThemeAction("Paper", Theme::Paper);
    addThemeAction("Water", Theme::Water);

    ui->themeButton->setMenu(themeMenu);
    connect(themeGroup, &QActionGroup::triggered, this, [this](QAction* action){
        currentTheme = static_cast<Theme>(action->data().toInt());
        updateThemeIcons();
        saveThemeSettings();
    });

    //шрифты по умолчанию
    QFont defaultFont("Bahnschrift", 18);
    ui->Table->setFont(defaultFont);
    ui->currentLabel->setFont(defaultFont);
    ui->totalLabel->setFont(defaultFont);

    //таймер для обновления прогресса воспроизведения
    playTimer.setTimerType(Qt::PreciseTimer);
    connect(&playTimer, &QTimer::timeout, this, &PlayerWindow::updatePlayProgress);

    //загрузка сохраненных настроек
    loadPlaybackMode();
    loadSortSettings();
    ui->volumeSlider->setValue(100);

    //громкости каналов
    m_channelVolumes = QVector<int>(16, 100);  // текущие громкости
    m_savedVolumes = QVector<int>(16, 100);    // сохраненные громкости
    m_channelMuted = QVector<bool>(16, false); // состояния mute
    m_masterVolume = 100;

    //состояния каналов
    channelStates = QVector<ChannelState>(16);
    for (int i = 0; i < 16; ++i) {
        channelStates[i].currentVolume = 100;
        channelStates[i].savedVolume = 100;
        channelStates[i].isMuted = false;
    }

    // Загружаем сохраненные настройки громкости каналов
    loadChannelSettings();
    if (midiOut && midiOut->isConnected()) {
        //начальные значения всех каналов
        for (int i = 0; i < 16; ++i) {
            midiOut->pitchWheel(i, 8192); // pitch bend (0-16383, центр - 8192 )
            HelperClass::sendControlChange(i, 1, 0); //modulation wheel
            HelperClass::sendControlChange(i, 7, 100); // громкости все на 100
            HelperClass::sendControlChange(i, 121, 0); // Все контроллеры выключены

        }
    }

    loadThemeSettings();
    updateButtonStates();
    loadPlaybackSpeedSettings();

    // кнопка ухода на главный экран
    connect(ui->homeButton, &QPushButton::clicked, this, [this]() {
        emit homeButtonClicked();
    });
}

PlayerWindow::~PlayerWindow()
{
    m_isDestructing = true;  //флаг для деструктора

    // остановка всех нот и сюрос MIDI-состояния
    resetAllNotes();
    resetMidiChannels();

    // остановка и отключение таймера
    playTimer.stop();
    disconnect(&playTimer, nullptr, this, nullptr);

    // Останавливаем MIDI воспроизведение
    if (midiOut && midiOut->isConnected()) {
        for (const auto& notePair : activeNotes) {
            HelperClass::sendNoteOff(notePair.second, notePair.first);
        }
        activeNotes.clear();
        midiOut->stopAll();
    }

    //очистка данных
    Files.clear();
    Paths.clear();
    Durs.clear();
    activeNotes.clear();
    channelInstruments.clear();
    m_channelActivity.fill(false); // снести активность каналов
    delete ui;
}

void PlayerWindow::closeEvent(QCloseEvent *event)
{
    // остановить все процессы перед закрытием
    stopPlay(true);
    resetAllNotes();

    emit homeButtonClicked();  //возврат на главный экран
    event->accept();
}

void PlayerWindow::contextMenuEvent(QContextMenuEvent *event)
{
    //элемент таблицы под курсором
    QTableWidgetItem *item = ui->Table->itemAt(ui->Table->viewport()->mapFromGlobal(event->globalPos()));
    if (!item) return;

    int row = ui->Table->row(item);
    if (row < 0 || row >= Paths.size()) return;

    //контекстное меню удаления
    QMenu menu(this);
    QAction *deleteAction = menu.addAction("Delete file");

    // отработка удаления файла
    connect(deleteAction, &QAction::triggered, this, [this, row](){
        if (QMessageBox::question(this, "Delete", "Delete selected composition from playlist?",
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
        {
            bool wasPlayingCurrent = (currentFileIndex == row);
            bool wasPlaying = is_playing;

            // удалить файл из всех контейнеров
            Files.erase(Files.begin() + row);
            Paths.removeAt(row);
            Durs.remove(row);

            //если удалялся текущий воспроизводимый файл
            if (wasPlayingCurrent) {
                stopPlay(true);
                if (!Paths.empty()) {
                    currentFileIndex = qMax(0, row - 1);
                    if (wasPlaying) {
                        playFile(currentFileIndex);
                    }
                } else {
                    currentFileIndex = -1;
                }
            }
            else if (currentFileIndex > row) {
                currentFileIndex--;
            }

            // обнова интерфейса
            refreshTable();
            updateButtonStates();
        }
    });

    menu.exec(event->globalPos());  //показать меню
}

void PlayerWindow::keyPressEvent(QKeyEvent *event)
{
    // пропускаем обработку если нет активного воспроизведения
    if (currentFileIndex < 0 || !is_playing) {
        QWidget::keyPressEvent(event);
        return;
    }

    //0-9 для быстрой перемотки
    if (event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9) {
        int percent = (event->key() - Qt::Key_0) * 10;
        if (percent == 0) percent = 100;  //0 = 100%
        int newPosition = (percent * 1000) / 100;

        playTimer.stop();
        seekToPosition(newPosition);
        playTimer.start();
        return;
    }

    //влево/вправо для перемотки
    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_Right) {
        int delta = (event->key() == Qt::Key_Left) ? -10000 : 10000;  // ±10 секунд
        int newPosition = currentPlayPos + delta;
        newPosition = qMax(0, qMin(newPosition, totalDur));

        int sliderPosition = (newPosition * 1000) / totalDur;
        sliderPosition = qMax(0, qMin(sliderPosition, 1000));

        playTimer.stop();
        seekToPosition(sliderPosition);
        playTimer.start();
        event->accept();
        return;
    }

    QWidget::keyPressEvent(event);
}

void PlayerWindow::on_uploadButton_clicked() {
    // диалог выбора файла
    QString filePath = QFileDialog::getOpenFileName(this, "Select MIDI file",
                        QDir::homePath(), "MIDI Files (*.mid *.midi)");
    QFile file(filePath);

    // всякие проверки
    if (!file.exists()) {
        QMessageBox::warning(this, "Error", "File doesn't exist");
        return;
    }
    if (file.size() > 20 * 1024 * 1024) {
        QMessageBox::warning(this, "Error", "File is too big (max 20 MB)");
        return;
    }
    if (Paths.contains(filePath)) {
        QMessageBox::information(this, "Info", "This file is already loaded");
        return;
    }

    // загрузка и проверка файла
    QMidiFile midiFile;
    if (!midiFile.load(filePath)) {
        QMessageBox::warning(this, "Error", "Couldn't load MIDI file");
        return;
    }

    // добавляем файл в контейнеры
    Files.emplace_back(std::make_shared<QMidiFile>());
    if (!Files.back()->load(filePath)) {
        Files.pop_back();
        return;
    }

    Durs.push_back(durInSec(Files.back().get()));
    Paths.push_back(filePath);

    // сортируем плейлист если нужно
    if (m_sortMode != SortByLoadOrder) {
        sortPlaylist();
    } else {
        refreshTable();  // обновляем таблицу если loadorder
    }

    updateButtonStates();  // обновляем состояние кнопок
}

void PlayerWindow::refreshTable()
{
    ui->Table->setRowCount(0);  // очищаем
    ui->Table->setColumnCount(1);
    ui->Table->setHorizontalHeaderLabels(QStringList() << "File name");

    QFont font = ui->Table->font();
    int columnWidth = ui->Table->columnWidth(0) - 20;  // ширина колонки с отступом

    // заполняем таблицу файлами из плейлиста
    for (int i = 0; i < Paths.size(); ++i) {
        int row = ui->Table->rowCount();
        ui->Table->insertRow(row);

        QFileInfo fileInfo(Paths[i]);
        QString fileName = fileInfo.fileName();
        // убираем расширение .mid/.midi из отображаемого имени
        if (fileName.endsWith(".mid", Qt::CaseInsensitive) || fileName.endsWith(".midi", Qt::CaseInsensitive)) {
            fileName = fileName.left(fileName.lastIndexOf('.'));
        }

        // урезанный текст с многоточием если не помещается
        QString displayText = getElidedText(fileName, font, columnWidth);

        QTableWidgetItem *nameItem = new QTableWidgetItem(displayText);
        nameItem->setData(Qt::UserRole, i); // сохр индекс!!! файла
        nameItem->setData(Qt::UserRole + 1, Paths[i]); // сохр полный путь

        //tooltip с информацией о файле
        QString durationText = QString::fromStdString(
            std::format("{:02d}:{:02d}",
                        static_cast<int>(Durs[i]) / 60,
                        static_cast<int>(Durs[i]) % 60));
        QString tooltip = QString("%1\nDuration: %2").arg(fileInfo.fileName()).arg(durationText);
        nameItem->setData(Qt::ToolTipRole, tooltip);

        nameItem->setFlags(nameItem->flags() ^ Qt::ItemIsEditable);  // нельзя редачить
        nameItem->setTextAlignment(Qt::AlignCenter);  // выравнивание по центру

        ui->Table->setItem(row, 0, nameItem);  // добавляем элемент в таблицу

        if (i == currentFileIndex) {
            ui->Table->selectRow(row);
            ui->Table->setCurrentCell(row, 0);
        }

    }


    updateNavButtonsState();  // обнова состояния кнопок
    updateThemeIcons();  // обнова иконок темы
}

void PlayerWindow::updateVolume(int volume)
{
    if (!midiOut || !midiOut->isConnected()) return;

    m_masterVolume = volume;  // сохр основн громкость
    int midiMasterVolume = volume * 1.27;  // конверт в диапазон для MIDI

    // громкость с учетом solo
    if (m_isSoloActive && m_soloChannel >= 0) {
        // только для solo канала громкость
        int individualVolume = m_channelVolumes[m_soloChannel];
        int finalVolume = (individualVolume * midiMasterVolume) / 100;
        HelperClass::sendControlChange(m_soloChannel, 7, finalVolume);
    } else {
        // для всех каналов громкость
        for (int channel = 0; channel < 16; ++channel) {
            int individualVolume = m_channelVolumes[channel];
            int finalVolume = (individualVolume * midiMasterVolume) / 100;
            HelperClass::sendControlChange(channel, 7, finalVolume);
        }
    }
}

void PlayerWindow::playFile(int row)
{
    // нет файлов или не норм индекс
    if (row < 0 || row >= Paths.size() || Files.empty()) {
        stopPlay(true);  // стоп игра если что-то не так
        return;
    }

    // сносим MIDI состояния перед началом новой игры
    resetMidiState();
    resetMidiChannels();
    resetAllNotes();

    stopPlay(false);  // стопаем текущее воспроизведение без сброса позиции
    currentFileIndex = row;  // установка текущего файла

    // ВЫДЕЛЕНИЕ ТЕКУЩЕГО ФАЙЛА В ТАБЛИЦЕ
    if (row >= 0 && row < ui->Table->rowCount()) {
        ui->Table->selectRow(row);  // Выделяем строку
        ui->Table->setCurrentCell(row, 0);  // Устанавливаем текущую ячейку
    }

    startPlay();  // запуск
    updateNavButtonsState();  // обнова кнопок
}

void PlayerWindow::updatePlayProgress()
{
    // условия для обновления
    if (currentFileIndex < 0 || !midiOut || !midiOut->isConnected() || m_isSeeking) return;

    auto& midiFile = Files[currentFileIndex];

    if (is_playing) {
        // обновляем текущую позицию с учетом скорости воспроизведения
        float speedMultiplier = playbackSpeedMultiplier();
        currentPlayPos += playTimer.interval() * speedMultiplier;
    }

    // Конвертируем время в тики для обработки MIDI-событий
    float currentTime = currentPlayPos / 1000.0f;
    qint32 currentTick = midiFile->tickFromTime(currentTime);

    // Обрабатываем события между пред и нынешним тиком
    if (is_playing) {
        const auto& events = midiFile->events();
        for (QMidiEvent* event : events) {
            if (event->tick() > m_previousTick && event->tick() <= currentTick) {
                processMidiEvent(event);  // Обрабатываем каждое MIDI-событие
            }
        }
        m_previousTick = currentTick;  // сохр последний обработанный тик
    }

    updateProgressUI();  // обнова

    // если трек завершен
    if (is_playing && currentPlayPos >= totalDur) {
        handlePlaybackFinished();  // обрабатываем завершение трека
    }
}

void PlayerWindow::processMidiEvent(QMidiEvent* event)
{
    if (!midiOut || !midiOut->isConnected() || !event) return;

    const int channel = event->voice();  //канал события
    const int note = event->note();      //нота
    const QPair<int, int> noteIdentifier(channel, note);  //идентификатор ноты

    switch (event->type()) {
    case QMidiEvent::NoteOn:
        if (event->velocity() > 0) {
            // начало звучания ноты
            if (!activeNotes.contains(noteIdentifier)) {
                midiOut->sendEvent(*event);  // Отпр событие
                activeNotes.insert(noteIdentifier);  // Добавляем в активные ноты
                updateChannelActivity(channel, true);  // Обновляем визуализацию
            }
        } else {
            // NoteOn с velocity=0 как NoteOff
            midiOut->sendEvent(*event);
            activeNotes.remove(noteIdentifier);
            updateChannelActivity(channel, hasActiveNotesOnChannel(channel));
        }
        break;

    case QMidiEvent::NoteOff:
        // Обрабатываем окончание звучания ноты
        midiOut->sendEvent(*event);
        activeNotes.remove(noteIdentifier);
        updateChannelActivity(channel, hasActiveNotesOnChannel(channel));
        break;

    case QMidiEvent::ProgramChange:
        // Обрабатываем смену инструмента
        midiOut->sendEvent(*event);
        channelInstruments[channel] = event->number();  // Сохр инструмент
        break;

    default: //все остальные MIDI-события
        midiOut->sendEvent(*event);
        break;
    }
}

bool PlayerWindow::hasActiveNotesOnChannel(int channel) const
{
    // Проход по всем активным нотам и есть ли в указанном канале
    for (const auto& notePair : activeNotes) {
        if (notePair.first == channel) {
            return true;  // Нашли актив ноту на канале
        }
    }
    return false;  // Активных нот на канале нет
}

void PlayerWindow::handlePlaybackFinished()
{
    // достигли ли конца файла
    if (currentPlayPos >= totalDur) {
        if (m_playbackMode == RepeatOne) {
            // Режим повтора одного файла значит перезапускаем текущий
            playFile(currentFileIndex);
        }
        else if (m_playbackMode == RepeatAll || currentFileIndex < Paths.size() - 1) {
            // Режим повтора всего плейлиста или переход к следующему файлу
            currentFileIndex = (currentFileIndex + 1) % Paths.size();
            playFile(currentFileIndex);
        } else {
            // Нет повторения и это последний файл то останавливаем воспроизведение
            stopPlay(true);
            ui->progressSlider->setEnabled(false);
        }
        updateNavButtonsState(); // Обновляем состояние кнопок навигации
    }
}

void PlayerWindow::savePlaybackMode()
{
    QSettings settings;
    settings.setValue("playbackMode", static_cast<int>(m_playbackMode));
}

void PlayerWindow::loadPlaybackMode()
{
    QSettings settings;
    m_playbackMode = static_cast<PlaybackMode>(
        settings.value("playbackMode", NoRepeat).toInt());
}

void PlayerWindow::shufflePlaylist()
{
    stopPlay(true);  // Останавливаем текущее воспроизведение

    if (Paths.empty()) return;

    // Сохраняем путь текущего файла для последующего поиска
    QString currentPath = (currentFileIndex >= 0) ? Paths[currentFileIndex] : "";

    // Создаем и перемешиваем индексы файлов
    std::vector<int> indexes(Paths.size());
    std::iota(indexes.begin(), indexes.end(), 0);
    std::shuffle(indexes.begin(), indexes.end(), std::mt19937(std::random_device()()));

    // Создаем временные списки для нового порядка
    QList<QString> newPaths;
    std::vector<std::shared_ptr<QMidiFile>> newFiles;
    QList<float> newDurs;

    // Заполняем списки в новом случайном порядке
    for (int i : indexes) {
        newPaths.append(Paths[i]);
        newFiles.push_back(Files[i]);
        newDurs.append(Durs[i]);
    }

    // Заменяем старые списки новыми
    Paths = newPaths;
    Files = std::move(newFiles); // move-семантика чтоб эффективно
    Durs = newDurs;

    // Обновляем индекс текущего файла после перемешивания
    currentFileIndex = currentPath.isEmpty() ? -1 : Paths.indexOf(currentPath);

    refreshTable();      // Обновляем отображение таблицы
    updateButtonStates(); // Обновляем состояние кнопок
}

void PlayerWindow::startPlay() {
    // Проверяем условия для начала воспроизведения
    if (m_isDestructing || currentFileIndex < 0 || currentFileIndex >= Files.size()) return;

    // ПРОВЕРКА И ВЫДЕЛЕНИЕ ПЕРЕД ЗАПУСКОМ
    if (currentFileIndex >= 0 && currentFileIndex < ui->Table->rowCount()) {
        ui->Table->selectRow(currentFileIndex);
        ui->Table->setCurrentCell(currentFileIndex, 0);
    }

    m_isSeeking = false;  // Сбрасываем флаг перемотки
    resetMidiChannels();  // Сбрасываем состояния MIDI-каналов
    resetAllNotes();      // Останавливаем все активные ноты

    // Сбрасываем состояния активности каналов
    m_channelActivity.fill(false);
    if (m_visualizationPanel && ui->visualizationWidget && ui->visualizationWidget->isVisible()) {
        m_visualizationPanel->updateChannels(m_channelActivity);
    }

    // Сбрасываем флаги состояния воспроизведения
    is_paused = false;
    m_pausePosition = 0;

    // Инициализащия параметров воспроизведения
    auto& midiFile = Files[currentFileIndex];
    totalDur = Durs[currentFileIndex] * 1000;  // Конвертируем в миллисекунды
    currentPlayPos = 0;
    lastProcessedTime = 0.0f;
    m_previousTick = 0;
    activeNotes.clear();

    // Находим макс тик для расчета общ времени
    m_totalTicks = 0;
    const auto& events = midiFile->events();
    for (QMidiEvent* event : events) {
        if (event->tick() > m_totalTicks) {
            m_totalTicks = event->tick();
        }
    }

    // Обрабатываем все события с нулевым тиком (инициализация инструментов)
    if (midiOut && midiOut->isConnected()) {
        const auto& events = midiFile->events();
        for (QMidiEvent* event : events) {
            if (event->tick() == 0)
                processMidiEvent(event);
        }
    }

    is_playing = true;  //флаг воспроизведения
    updatePlay();       // Обновляем UI

    // Запускаем таймер с учетом текущей скорости
    float speedMultiplier = playbackSpeedMultiplier();
    int timerInterval = 16 / speedMultiplier;
    playTimer.start(timerInterval);

    ui->progressSlider->setEnabled(true);

    updateThemeIcons();

    // Устанавливаем тексты длительности трека
    ui->totalLabel->setText(QString::fromStdString(
        std::format("{:02d}:{:02d}", (totalDur/1000)/60, (totalDur/1000)%60)));
        ui->currentLabel->setText("00:00");
}

void PlayerWindow::seekToPosition(int position)
{
    //условия для перемотки
    if (currentFileIndex < 0 || currentFileIndex >= Files.size() || !midiOut || !midiOut->isConnected()) return;

    // Сохраняем текущие индивид громкости всех каналов
    QVector<int> currentIndividualVolumes(16);
    for (int i = 0; i < 16; ++i) {
        currentIndividualVolumes[i] = m_channelVolumes[i];
    }

    // Сохр состояние solo для след восстановления
    bool wasSoloActive = m_isSoloActive;
    int savedSoloChannel = m_soloChannel;
    QVector<int> savedPreSoloVolumes = m_preSoloVolumes;

    // стоп все ноты если воспроизведение активно или есть активные ноты
    if (is_playing || !activeNotes.isEmpty())
        resetAllNotes();

    // Сброс pitch bend для всех каналов
    for (int ch = 0; ch < 16; ++ch)
        midiOut->pitchWheel(ch, 0x2000);

    // Устанавливаем новую позицию воспроизведения
    currentPlayPos = (position * totalDur) / 1000;
    float currentTime = currentPlayPos / 1000.0f;
    auto& midiFile = Files[currentFileIndex];
    m_previousTick = midiFile->tickFromTime(currentTime);

    // Восстанавливаем инструменты и контроллеры до новой позиции
    const auto& events = midiFile->events();
    for (QMidiEvent* event : events) {
        if (event->tick() <= m_previousTick) {
            if (event->type() == QMidiEvent::ProgramChange) {
                midiOut->sendEvent(*event);
                channelInstruments[event->voice()] = event->number();
            }
            else if (event->type() == QMidiEvent::ControlChange && event->number() != 7) {
                midiOut->sendEvent(*event);
            }
        }
    }

    // Восстанавливаем громкости каналов
    for (int channel = 0; channel < 16; ++channel) {
        m_channelVolumes[channel] = currentIndividualVolumes[channel];
        setChannelVolume(channel, currentIndividualVolumes[channel]);
    }

    // Восстанавливаем состояние solo если было активно
    if (wasSoloActive) {
        m_isSoloActive = true;
        m_soloChannel = savedSoloChannel;
        m_preSoloVolumes = savedPreSoloVolumes;

        for (int i = 0; i < 16; ++i) {
            if (i == m_soloChannel)
                setChannelVolume(i, m_preSoloVolumes[i]);
            else
                setChannelVolume(i, 0);
        }
    }

    //UI с новым временем
    ui->currentLabel->setText(QString::fromStdString(
        std::format("{:02d}:{:02d}", (currentPlayPos/1000)/60, (currentPlayPos/1000)%60)));

    lastProcessedTime = currentTime;
    ui->progressSlider->setValue(position);

    // Обновляем позицию паузы если игра была на паузе
    if (is_paused)
        m_pausePosition = currentPlayPos;
}

void PlayerWindow::pausePlay()
{
    // Если уже на паузе - возобновляем воспроизведение
    if (is_paused) {
        // Пересчитываем предыдущий тик используя правильный метод
        float currentTime = m_pausePosition / 1000.0f;
        auto& midiFile = Files[currentFileIndex];
        m_previousTick = midiFile->tickFromTime(currentTime);

        currentPlayPos = m_pausePosition;
        lastProcessedTime = currentTime;

        // Запускаем таймер с учетом текущей скорости
        float speedMultiplier = playbackSpeedMultiplier();
        playTimer.start(16 / speedMultiplier);
        is_playing = true;
        is_paused = false;

        updateThemeIcons();
        return;
    }

    if (!is_playing) return;// Если не играет - ничего не делаем

    playTimer.stop();
    m_pausePosition = currentPlayPos;
    is_paused = true;
    is_playing = false;

    // Останавливаем все активные ноты (чтобы не играли при открытии диалога)
    if (midiOut && midiOut->isConnected()) {
        //resetAllNotes();
        midiOut->stopAll();
    }

    updateThemeIcons();
    ui->progressSlider->setEnabled(true);
}

void PlayerWindow::stopPlay(bool resetPos)
{
    if (m_isDestructing) return;

    playTimer.stop();

    // СНИМАЕМ ВЫДЕЛЕНИЕ ПРИ ОСТАНОВКЕ
    ui->Table->clearSelection();
    ui->Table->setCurrentItem(nullptr);

    // Полный сброс всех нот и состояний
    resetAllNotes();
    for (int ch = 0; ch < 16; ++ch) {
        midiOut->pitchWheel(ch, 0x2000);
    }

    // Сброс состояний
    if (resetPos) {
        channelInstruments.clear();
        currentPlayPos = 0;
        totalDur = 0;
        ui->progressSlider->setValue(0);
        ui->totalLabel->setText("00:00");
        ui->currentLabel->setText("00:00");
        ui->progressSlider->setEnabled(false);
        m_previousTick = 0;
        lastProcessedTime = 0.0f;
    }

    is_playing = false;
    is_paused = false;
    updateButtonStates();
}

void PlayerWindow::updateButtonStates()
{
    bool hasFiles = !Files.empty();
    bool isFileSelected = (currentFileIndex >= 0); // Файл выбран

    ui->pauseButton->setEnabled(hasFiles);
    ui->stopButton->setEnabled(hasFiles);

    // активируем ползунок если есть файлы И файл выбран
    ui->progressSlider->setEnabled(hasFiles && isFileSelected);

    // Обновляем кнопки навигации с учетом режима воспроизведения
    ui->prevButton->setEnabled(hasFiles && (m_playbackMode == RepeatAll || m_playbackMode == RepeatOne || currentFileIndex > 0));
    ui->nextButton->setEnabled(hasFiles && (m_playbackMode == RepeatAll ||
        m_playbackMode == RepeatOne || (currentFileIndex < Paths.size() - 1 && currentFileIndex >= 0)));

    if (!hasFiles) {
        currentFileIndex = -1;
        is_playing = false;
        is_paused = false;
        ui->progressSlider->setValue(0);
        totalDur = 0;
        ui->totalLabel->setText("00:00");
        ui->currentLabel->setText("00:00");
        ui->progressSlider->setEnabled(false);
    }

    updateThemeIcons();
}

void PlayerWindow::updatePlay()
{
    QString iconName = is_playing ? "pause" : "play";
    ui->pauseButton->setIcon(QIcon(QString(":/img/%1/%2.png").arg(getCurrentThemeName()).arg(iconName)));
}

void PlayerWindow::on_homeButton_clicked(){
    // Made by Nadzeya Yarmolik ;)
    emit homeButtonClicked();
    this->hide();}

void PlayerWindow::on_pauseButton_clicked()
{
    if (Files.empty()) return;

    // Если не играет и не на паузе то начинаем воспроизведение
    if (!is_playing && !is_paused) {
        if (currentFileIndex == -1 && !Files.empty()) {
            // АВТОМАТИЧЕСКИ ВЫБИРАЕМ ПЕРВЫЙ ТРЕК если ничего не выбрано
            currentFileIndex = 0;
            ui->progressSlider->setEnabled(true);  // Активируем ползунок
            updateNavButtonsState();
        }
        startPlay();  // Запускаем воспроизведение
    }
    else pausePlay();  // Приостанавливаем или возобновляем

    //обновление иконки
    QString themeName = getCurrentThemeName();
    QString iconName = is_playing ? "pause" : "play";
    ui->pauseButton->setIcon(QIcon(QString(":/img/%1/%2.png").arg(themeName).arg(iconName)));

    setFocus();// Фокус обратно на окно для обработки клавы
}

void PlayerWindow::on_stopButton_clicked() {
    if (Files.empty()) return;

    if (is_playing) {
        // Если трек играет - перематываем на начало и продолжаем играть
        currentPlayPos = 0;
        seekToPosition(0);
        // Не меняем is_playing - продолжаем воспроизведение
    } else if (is_paused) {
        // Если трек на паузе - перематываем на начало и остаемся на паузе
        currentPlayPos = 0;
        seekToPosition(0);
        m_pausePosition = 0; // Обновляем позицию паузы
        // Остаемся в паузе is_paused = true
    } else {
        // Если трек остановлен то начинаем с начала
        if (currentFileIndex == -1 && !Files.empty()) {
            currentFileIndex = 0;
        }
        currentPlayPos = 0;
        seekToPosition(0);
        startPlay();  // Запускаем воспроизведение
    }

    updateButtonStates();
}

void PlayerWindow::on_volumeSlider_valueChanged(int value)
{
    if (is_mute && value > 0) is_mute = false;

    if (!is_mute) {
        updateVolume(value);  // новая громкость
        saved_volume = value; // Сохраняем значение
    }

    QString themeName = getCurrentThemeName();
    QString iconName = (value == 0 || is_mute) ? "mute" : "volume";
    ui->muteButton->setIcon(QIcon(QString(":/img/%1/%2.png").arg(themeName, iconName)));

}

float PlayerWindow::durInSec(QMidiFile* midiFile) const {
    if (!midiFile || midiFile->events().isEmpty())
        return 0.0f;

    // последний тик среди всех событий
    qint32 maxTick = 0;
    for (QMidiEvent* event : midiFile->events()) {
        if (event->tick() > maxTick) {
            maxTick = event->tick();
        }
    }
    return midiFile->timeFromTick(maxTick);  // тики в секунды
}

void PlayerWindow::on_muteButton_clicked()
{
    is_mute = !is_mute;  // Переключаем состояние

    if (is_mute) {
        saved_volume = ui->volumeSlider->value(); // Сохраняем текущее значение перед mute
        ui->volumeSlider->setValue(0); // Устанавливаем ползунок в 0
        updateVolume(0);  // Применяем нулевую громкость
    } else {
        ui->volumeSlider->setValue(saved_volume); // Восстанавливаем сохр значение
        updateVolume(saved_volume);  // Применяем сохр громкость
    }

    QString themeName = getCurrentThemeName();
    QString iconName = is_mute ? "mute" : "volume";
    ui->muteButton->setIcon(QIcon(QString(":/img/%1/%2.png").arg(themeName, iconName)));
}

void PlayerWindow::on_settingsButton_clicked()
{
    QMenu menu(this);

    // Меню режимов воспроизведения
    QMenu* playbackMenu = menu.addMenu("Playback mode");

    QAction* repeatAllAction = playbackMenu->addAction("Repeat playlist");
    repeatAllAction->setCheckable(true);
    repeatAllAction->setChecked(m_playbackMode == RepeatAll);

    QAction* repeatOneAction = playbackMenu->addAction("Repeat current file");
    repeatOneAction->setCheckable(true);
    repeatOneAction->setChecked(m_playbackMode == RepeatOne);

    QAction* noRepeatAction = playbackMenu->addAction("No repeat");
    noRepeatAction->setCheckable(true);
    noRepeatAction->setChecked(m_playbackMode == NoRepeat);

    // Группа взаимоисключающих действий для режимов воспроизведения
    QActionGroup* playbackGroup = new QActionGroup(this);
    playbackGroup->addAction(repeatAllAction);
    playbackGroup->addAction(repeatOneAction);
    playbackGroup->addAction(noRepeatAction);
    playbackGroup->setExclusive(true);

    // Меню скорости воспроизведения
    QMenu* speedMenu = menu.addMenu("Playback Speed");
    QActionGroup* speedGroup = new QActionGroup(this);

    QAction* halfSpeedAction = speedMenu->addAction("0.5x Speed");
    halfSpeedAction->setCheckable(true);
    halfSpeedAction->setChecked(m_playbackSpeed == SpeedHalf);
    halfSpeedAction->setData(static_cast<int>(SpeedHalf));
    speedGroup->addAction(halfSpeedAction);

    QAction* normalSpeedAction = speedMenu->addAction("Normal Speed");
    normalSpeedAction->setCheckable(true);
    normalSpeedAction->setChecked(m_playbackSpeed == SpeedNormal);
    normalSpeedAction->setData(static_cast<int>(SpeedNormal));
    speedGroup->addAction(normalSpeedAction);

    QAction* doubleSpeedAction = speedMenu->addAction("2.0x Speed");
    doubleSpeedAction->setCheckable(true);
    doubleSpeedAction->setChecked(m_playbackSpeed == SpeedDouble);
    doubleSpeedAction->setData(static_cast<int>(SpeedDouble));
    speedGroup->addAction(doubleSpeedAction);

    speedGroup->setExclusive(true);

    // Меню сортировки
    QMenu* sortMenu = menu.addMenu("Sort by");

    QAction* sortByNameAction = sortMenu->addAction("Name");
    sortByNameAction->setCheckable(true);
    sortByNameAction->setChecked(m_sortMode == SortByName);

    QAction* sortByDurationAction = sortMenu->addAction("Duration");
    sortByDurationAction->setCheckable(true);
    sortByDurationAction->setChecked(m_sortMode == SortByDuration);

    QAction* sortByLoadOrderAction = sortMenu->addAction("Load order");
    sortByLoadOrderAction->setCheckable(true);
    sortByLoadOrderAction->setChecked(m_sortMode == SortByLoadOrder);

    // Группа для сортировки
    QActionGroup* sortGroup = new QActionGroup(this);
    sortGroup->addAction(sortByNameAction);
    sortGroup->addAction(sortByDurationAction);
    sortGroup->addAction(sortByLoadOrderAction);
    sortGroup->setExclusive(true);

    menu.addSeparator();

    // Доп действия
    QAction* clearAction = menu.addAction("Clear playlist");
    QAction* shuffleAction = menu.addAction("Shuffle playlist");

    // Действие для визуализации
    QAction* visualizationAction = menu.addAction("Show channel visualization");
    visualizationAction->setCheckable(true);
    visualizationAction->setChecked(m_visualizationPanel && ui->visualizationWidget->isVisible());

    // Подкл обработчики для режимов игры
    connect(repeatAllAction, &QAction::triggered, this, [this]() {
        m_playbackMode = RepeatAll;
        savePlaybackMode();
        updateNavButtonsState();
    });

    connect(repeatOneAction, &QAction::triggered, this, [this]() {
        m_playbackMode = RepeatOne;
        savePlaybackMode();
        updateNavButtonsState();
    });

    connect(noRepeatAction, &QAction::triggered, this, [this]() {
        m_playbackMode = NoRepeat;
        savePlaybackMode();
        updateNavButtonsState();
    });

    // Подключаем обработчики для сортировки
    connect(sortByNameAction, &QAction::triggered, this, [this]() {
        m_sortMode = SortByName;
        saveSortSettings();
        sortPlaylist();
    });

    connect(sortByDurationAction, &QAction::triggered, this, [this]() {
        m_sortMode = SortByDuration;
        saveSortSettings();
        sortPlaylist();
    });

    connect(sortByLoadOrderAction, &QAction::triggered, this, [this]() {
        m_sortMode = SortByLoadOrder;
        saveSortSettings();
        sortPlaylist();
    });

    // Обработчики для скорости воспроизведения
    connect(speedGroup, &QActionGroup::triggered, this, [this](QAction* action) {
        PlaybackSpeed speed = static_cast<PlaybackSpeed>(action->data().toInt());
        setPlaybackSpeed(speed);  // Меняем скорость без перезагрузки трека
    });

    // Обработчики для управления плейлистом
    connect(clearAction, &QAction::triggered, this, [this]() {
        if (QMessageBox::question(this, "Clear Playlist",
            "Are you sure you want to clear the playlist?",
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
        {
            Files.clear();
            Paths.clear();
            Durs.clear();
            currentFileIndex = -1;
            stopPlay(true);
            refreshTable();
            updateButtonStates();
        }
    });

    connect(shuffleAction, &QAction::triggered, this, &PlayerWindow::shufflePlaylist);
    connect(visualizationAction, &QAction::triggered, this, &PlayerWindow::onShowVisualizationTriggered);

    // Показать меню под кнопкой настроек
    menu.exec(ui->settingsButton->mapToGlobal(QPoint(0, ui->settingsButton->height())));
}

void PlayerWindow::on_prevButton_clicked()
{
    if (Paths.empty()) return;

    if (currentFileIndex > 0) {
        // Переходим к предыдущему файлу если он существует
        playFile(currentFileIndex - 1);
    } else if (m_playbackMode == RepeatAll) {
        // Если это первый файл и repeat all - переходим к последнему
        playFile(Paths.size() - 1);
    }
}

void PlayerWindow::on_nextButton_clicked()
{
    if (Paths.empty()) return;

    if (currentFileIndex < Paths.size() - 1) {
        // Переходим к след файлу если он существует
        playFile(currentFileIndex + 1);
    } else if (m_playbackMode == RepeatAll) {
        // Если это последний файл и repeat all - переходим к первому
        playFile(0);
    }
}

void PlayerWindow::updateThemeIcons()
{
    if (m_isDestructing) return;

    QString themeName = getCurrentThemeName();
    if (themeName.isEmpty()) {
        themeName = "modern"; // чтоб не сломалось
    }

    // Проверяем размер вектора активности каналов
    if (m_channelActivity.size() != 16) {
        m_channelActivity = QVector<bool>(16, false);
    }

    // Проверяем валидность объектов визуализации
    if (!m_visualizationPanel || !ui->visualizationWidget) return;

    try {
        m_visualizationPanel->setStyle(themeName);
        if (ui->visualizationWidget->isVisible()) {
            m_visualizationPanel->updateChannels(m_channelActivity);
        }
    } catch (...) {
        qWarning() << "Failed to update theme icons";
    }

    // Настройка шрифтов для разных тем
    QFont mainFont;
    if (themeName == "paper") {
        mainFont.setFamily("Kristen ITC");
        mainFont.setWeight(QFont::Normal);
    }
    else if (themeName == "water") {
        mainFont.setFamily("Arial Rounded MT");
        mainFont.setWeight(QFont::Normal);
    }
    else { // modern
        mainFont.setFamily("Bahnschrift");
        mainFont.setWeight(QFont::Bold);
    }
    mainFont.setPointSize(18); // Установка шрифта размера 18

    // Применяем шрифт ко всем элементам
    QApplication::setFont(mainFont);
    ui->Table->setFont(mainFont);
    ui->currentLabel->setFont(mainFont);
    ui->totalLabel->setFont(mainFont);

    // Настройка стилей таблицы
    QString tableStyle;
    QString labelColor;
    if (themeName == "modern") {
        labelColor = "#CDEAFF";
        tableStyle = "QTableWidget {"
                     "background: transparent;"
                     "color: #1F9AFF;"
                     "border: 5px solid #CDEAFF;"
                     "}"
                     "QHeaderView::section {"
                     "background-color: #CDEAFF;"
                     "color: #1F9AFF; padding: 10px; border: none;"
                     "font-weight: bold;"
                     "}";
    }
    else if (themeName == "paper") {
        labelColor = "#C22C1D";
        tableStyle = "QTableWidget {"
                     "background: transparent;"
                     "color: #0F085C;"
                     "border: 5px solid #B1D5DE;"
                     "}"
                     "QHeaderView::section {"
                     "background: #B1D5DE;"
                     "color: #C22C1D; padding: 10px; border: none;"
                     "}";
    }
    else { // water
        labelColor = "#00B6D3";
        tableStyle = "QTableWidget {"
                     "background: transparent;"
                     "color: #013F6E;"
                     "border: none;"
                     "}"
                     "QHeaderView::section {"
                     "background-color: #013F6E;"
                     "color: #00B6D3; padding: 10px; border: none;"
                     "}";
    }

    QString currentRowStyle;
    if (themeName == "modern") {
        currentRowStyle = "QTableView::item:selected { color: #CDEAFF; }";
    }
    else if (themeName == "paper") {
        currentRowStyle = "QTableView::item:selected { color: #C22C1D; }";
    }
    else { // water
        currentRowStyle = "QTableView::item:selected { color: #00B6D3; }";
    }


    ui->Table->setStyleSheet(tableStyle + currentRowStyle);
    QString labelStyle = QString("QLabel { color: %1; background: transparent; }").arg(labelColor);
    ui->currentLabel->setStyleSheet(labelStyle);
    ui->totalLabel->setStyleSheet(labelStyle);

    // Применяем стили к слайдерам
    ui->progressSlider->setStyleSheet(setSliderStyleSheet());
    ui->volumeSlider->setStyleSheet(setSliderStyleSheet());

    // Настройка кнопок
    QSize iconSize(100, 100);
    QString buttonStyle = "QPushButton {"
                          "background: transparent; border: none; }";

    QList<QPushButton*> buttons = findChildren<QPushButton*>();
    foreach (QPushButton* button, buttons) {
        button->setStyleSheet(buttonStyle);
        if (button->parent() && button->parent()->inherits("LedsPanel")) {
            continue; // Пропускаем светодиоды
        }
        if (button->parent() && button->parent()->inherits("QDialog")) {
            continue;
        }
        button->setIconSize(iconSize);
        HelperClass::setupButtonAnimation(button, iconSize, 100);
    }

    // Загрузка + установка иконок для всех кнопок
    auto loadIcon = [themeName](const QString& iconName) {
        return QIcon(QString(":/img/%1/%2.png").arg(themeName).arg(iconName));
    };

    ui->pauseButton->setIcon(loadIcon(is_playing ? "pause" : "play"));
    ui->stopButton->setIcon(loadIcon("start"));
    ui->prevButton->setIcon(loadIcon("previous"));
    ui->nextButton->setIcon(loadIcon("next"));
    ui->uploadButton->setIcon(loadIcon("upload"));
    ui->homeButton->setIcon(loadIcon("home"));
    ui->muteButton->setIcon(loadIcon(is_mute ? "mute" : "volume"));
    ui->settingsButton->setIcon(loadIcon("settings"));
    ui->themeButton->setIcon(loadIcon("theme"));
    ui->infoButton->setIcon(loadIcon("info"));

    // Установка фона
    QPixmap background(QString(":/img/%1/wallpaper.jpg").arg(themeName));
    if (!background.isNull()) {
        this->setFixedSize(background.size());
        QPalette palette;
        palette.setBrush(QPalette::Window, QBrush(background));
        this->setPalette(palette);
        this->setAutoFillBackground(true);
    }

    // Обнова открытых диалогов
    QList<ChannelDialog*> dialogs = findChildren<ChannelDialog*>();
    for (ChannelDialog* dialog : dialogs) {
        dialog->applyThemeStyle();
        dialog->update();
    }

    this->update();
    this->repaint();
}

void PlayerWindow::saveThemeSettings()
{
    QSettings settings;
    settings.setValue("theme", static_cast<int>(currentTheme));
}

void PlayerWindow::loadThemeSettings()
{
    QSettings settings;
    currentTheme = static_cast<Theme>(settings.value("theme", 0).toInt());
    updateThemeIcons();  // Применяем загруженную тему
}

QString PlayerWindow::getCurrentThemeName() const
{
    switch(currentTheme) {
    case Theme::Paper: return "paper";
    case Theme::Water: return "water";
    default: return "modern";
    }
}

QString PlayerWindow::getElidedText(const QString& text, const QFont& font, int width)
{
    QFontMetrics metrics(font);
    return metrics.elidedText(text, Qt::ElideRight, width);
}

void PlayerWindow::updateNavButtonsState()
{
    bool hasFiles = !Paths.empty();
    ui->prevButton->setEnabled(hasFiles && (m_playbackMode == RepeatAll || m_playbackMode == RepeatOne || currentFileIndex > 0));
    ui->nextButton->setEnabled(hasFiles && (m_playbackMode == RepeatAll || m_playbackMode == RepeatOne
       || (currentFileIndex < Paths.size() - 1 && currentFileIndex >= 0)));
}

void PlayerWindow::updateChannelActivity(int channel, bool active)
{
    if (m_isDestructing || channel < 0 || channel >= m_channelActivity.size()) return;

    m_channelActivity[channel] = active;// Всегда обновляем состояние активности

    // Обновляем визуализацию всегда, даже на паузе
    if (m_visualizationPanel && ui->visualizationWidget && ui->visualizationWidget->isVisible()) {
        QMetaObject::invokeMethod(m_visualizationPanel, [this]() {
            if (!m_isDestructing && m_visualizationPanel) {
                m_visualizationPanel->updateChannels(m_channelActivity);
            }
        }, Qt::QueuedConnection);
    }
}

void PlayerWindow::onShowVisualizationTriggered()
{
    if (m_isDestructing || !ui->visualizationWidget || !m_visualizationPanel) return;

    bool newVisibility = !ui->visualizationWidget->isVisible();
    ui->visualizationWidget->setVisible(newVisibility);

    if (newVisibility) {
        m_visualizationPanel->setStyle(getCurrentThemeName());// Обновляем стиль при показе
        m_visualizationPanel->updateChannels(m_channelActivity);//передаем текущее состояние
    }
}

QString PlayerWindow::setSliderStyleSheet() const
{
    QString themeName = getCurrentThemeName();
    if (themeName.isEmpty()) themeName = "modern";

    QString themeColors;
    if (themeName == "modern") {
        themeColors =
            "QSlider::groove:horizontal { background: #4A4A4A; }"
            "QSlider::sub-page:horizontal { background: #1F9AFF; }";
    }
    else if (themeName == "paper") {
        themeColors =
            "QSlider::groove:horizontal { background: #B1D5DE; }"
            "QSlider::sub-page:horizontal { background: black; }";
    }
    else { // water
        themeColors =
            "QSlider::groove:horizontal { background: #013F6E; }"
            "QSlider::sub-page:horizontal { background: #00B6D3; }";
    }

    QString baseSliderStyle = QString(R"(
    QSlider {
        min-height: 30px; max-height: 30px; padding: 0px; margin: 0px;
    }
    QSlider::groove:horizontal {
        height: 6px; border-radius: 3px;
    }
    QSlider::sub-page:horizontal {
        height: 6px; border-radius: 3px;
    }
    QSlider::handle:horizontal {
        width: 28px; height: 28px;
        margin: -11px 0;
        background: transparent;
        border: none;
        image: url(:/img/%1/handle.png);
        subcontrol-origin: margin;
        subcontrol-position: center center;
    } )").arg(themeName);

    return baseSliderStyle + themeColors;
}

void PlayerWindow::resetAllNotes()
{
    if (midiOut && midiOut->isConnected()) {
        // Останавливаем ноты, но не трогаем громкость каналов
        midiOut->stopAll();

        auto activeNotesCopy = activeNotes;
        for (const auto& notePair : activeNotesCopy) {
            HelperClass::sendNoteOff(notePair.second, notePair.first);
        }
        activeNotes.clear();
    }
    m_channelActivity.fill(false);
}

void PlayerWindow::resetMidiChannels()
{
    if (midiOut && midiOut->isConnected()) {
        // Сброс всех контроллеров на всех каналах
        for (int channel = 0; channel < 16; ++channel) {
            //Сброс питча и диапазона
            midiOut->pitchWheel(channel, 0x2000);
            HelperClass::sendControlChange(channel, 101, 0); //НЕ УБИРАТЬ НА НИХ
            HelperClass::sendControlChange(channel, 100, 0); //ДЕРЖИТСЯ ПИТЧ
            HelperClass::sendControlChange(channel, 6, 2);
            HelperClass::sendControlChange(channel, 38, 0);
            HelperClass::sendControlChange(channel, 101, 127);
            HelperClass::sendControlChange(channel, 100, 127);
        }
    }
}

void PlayerWindow::loadChannelSettings()
{
    QSettings settings;
    for (int channel = 0; channel < 16; ++channel) {
        // Добавляем проверку на границы
        if (channel >= 0 && channel < m_channelVolumes.size()) {
            m_channelVolumes[channel] = settings.value(
                QString("channel_%1_volume").arg(channel), 100).toInt();
        }
    }

    // Применяем загруженные настройки к MIDI-устройству
    if (midiOut && midiOut->isConnected()) {
        for (int channel = 0; channel < 16; ++channel) {
            if (channel >= 0 && channel < m_channelVolumes.size()) {
                int finalVolume = (m_channelVolumes[channel] * m_masterVolume * 1.27) / 100;
                HelperClass::sendControlChange(channel, 7, finalVolume);
            }
        }
    }
}

void PlayerWindow::resetAllChannelVolumes() {
    if (m_isSoloActive) unsoloAllChannels(); // Отключаем solo полностью

    for (int i = 0; i < channelStates.size(); ++i) {
        channelStates[i].currentVolume = 100;
        channelStates[i].savedVolume = 100;
        channelStates[i].isMuted = false;
        m_channelVolumes[i] = 100; // Сбрасываем индивидуальную громкость

        // Применяем с учетом мастер-громкости
        if (midiOut && midiOut->isConnected()) {
            int finalVolume = (100 * m_masterVolume * 1.27) / 100;
            HelperClass::sendControlChange(i, 7, finalVolume);
        }
    }
}

QString PlayerWindow::getChannelInstrumentName(int channel) const
{
    if (channel < 0 || channel >= 16) {
        return "Invalid Channel";
    }

    // Проверяем, есть ли информация об инструменте для этого канала
    if (channelInstruments.contains(channel)) {
        int programNumber = channelInstruments[channel];
        return HelperClass::getInstrumentName(programNumber, channel);
    }

    return "No instrument set";
}

void PlayerWindow::saveChannelSettings(int channel)
{
    if (channel < 0 || channel >= 16) return;

    QSettings settings;
    settings.setValue(QString("channel_%1_volume").arg(channel), m_savedVolumes[channel]);
    settings.setValue(QString("channel_%1_muted").arg(channel), m_channelMuted[channel]);
}

void PlayerWindow::setChannelVolume(int channel, int volume) {
    if (channel < 0 || channel >= channelStates.size()) return;

    channelStates[channel].currentVolume = volume;
    channelStates[channel].isMuted = (volume == 0);
    m_channelVolumes[channel] = volume;

    if (midiOut && midiOut->isConnected()) {
        // Применяем мастер-громкость и преобразуем в MIDI диапазон (0-127)
        int finalVolume = (volume * m_masterVolume * 1.27) / 100;
        HelperClass::sendControlChange(channel, 7, finalVolume);
    }
}

int PlayerWindow::getChannelVolume(int channel) const {
    if (channel < 0 || channel >= channelStates.size()) return -1;
    return channelStates[channel].currentVolume;
}

int PlayerWindow::getSavedChannelVolume(int channel) const {
    if (channel < 0 || channel >= channelStates.size()) return 100;
    return channelStates[channel].savedVolume;
}

bool PlayerWindow::isChannelMuted(int channel) const {
    if (channel < 0 || channel >= channelStates.size()) return false;
    return channelStates[channel].isMuted;
}

void PlayerWindow::sortPlaylist()
{
    if (Paths.empty()) return;

    // Сохраняем текущий воспроизводимый файл
    QString currentPath = (currentFileIndex >= 0) ? Paths[currentFileIndex] : "";
    bool wasPlaying = is_playing;

    stopPlay(true);// Останавливаем воспроизведение перед сортировкой

    // Если сортировка по порядку загрузки - не сортируем, просто обновляем
    if (m_sortMode == SortByLoadOrder) refreshTable();
    else {
        // Создаем список индексов для сортировки
        QVector<int> indexes(Paths.size());
        std::iota(indexes.begin(), indexes.end(), 0);

        // Сортируем индексы в зависимости от выбранного режима
        switch (m_sortMode) {
        case SortByName:
            std::sort(indexes.begin(), indexes.end(), [this](int a, int b) {
                QFileInfo fileInfoA(Paths[a]);
                QFileInfo fileInfoB(Paths[b]);
                return QString::compare(fileInfoA.fileName(), fileInfoB.fileName(), Qt::CaseInsensitive) < 0;
            });
            break;

        case SortByDuration:
            std::sort(indexes.begin(), indexes.end(), [this](int a, int b) {
                return Durs[a] < Durs[b];
            });
            break;

        case SortByLoadOrder:
            // Для порядка загрузки ничего не делаем - порядок уже правильный
            break;
        }

        // Создаем новые отсортированные списки
        QList<QString> newPaths;
        std::vector<std::shared_ptr<QMidiFile>> newFiles;
        QList<float> newDurs;

        for (int i : indexes) {
            newPaths.append(Paths[i]);
            newFiles.push_back(Files[i]);
            newDurs.append(Durs[i]);
        }

        // Присваиваем новые значения
        Paths = newPaths;
        Files = std::move(newFiles);
        Durs = newDurs;

        // Обновляем текущий индекс
        if (!currentPath.isEmpty())
            currentFileIndex = Paths.indexOf(currentPath);
        else
            currentFileIndex = -1;

    }

    refreshTable();// Обновляем таблицу

    // Если было воспроизведение, продолжаем
    if (wasPlaying && currentFileIndex >= 0)
        playFile(currentFileIndex);

}

void PlayerWindow::saveSortSettings()
{
    QSettings settings;
    settings.setValue("sortMode", static_cast<int>(m_sortMode));
}

void PlayerWindow::loadSortSettings()
{
    QSettings settings;
    m_sortMode = static_cast<SortMode>(settings.value("sortMode", SortByLoadOrder).toInt()); // По умолчанию порядок загрузки
}

void PlayerWindow::setSoloChannel(int channel)
{
    if (channel < 0 || channel >= 16) return;

    if (m_isSoloActive && m_soloChannel == channel) {
        // Если уже solo на этом канале - отключаем solo
        unsoloAllChannels();
        return;
    }

    // Сохраняем текущие громкости перед ПЕРВЫМ включением solo
    if (!m_isSoloActive) {
        m_preSoloVolumes.resize(16);
        for (int i = 0; i < 16; ++i) {
            m_preSoloVolumes[i] = getChannelVolume(i); // Сохраняем ТЕКУЩИЕ значения
        }
    }

    // Заглушаем предыдущий solo канал (если переключаемся)
    if (m_isSoloActive) {
        int previousSoloChannel = m_soloChannel;
        // Восстанавливаем сохраненную громкость для предыдущего solo канала
        setChannelVolume(previousSoloChannel, m_preSoloVolumes[previousSoloChannel]);
    }

    // Включаем новый solo канал
    m_isSoloActive = true;
    m_soloChannel = channel;

    // Для нового solo канала воспроизводим на СОХРАНЕННОЙ громкости
    setChannelVolume(channel, m_preSoloVolumes[channel]);

    // Заглушаем все остальные каналы
    for (int i = 0; i < 16; ++i) {
        if (i != channel) {
            setChannelVolume(i, 0);
        }
    }
}

void PlayerWindow::unsoloAllChannels()
{
    if (!m_isSoloActive) return;

    // Восстанавливаем СОХРАНЕННЫЕ громкости для ВСЕХ каналов
    for (int i = 0; i < 16; ++i) {
        setChannelVolume(i, m_preSoloVolumes[i]);
    }

    m_isSoloActive = false;
    m_soloChannel = -1;
}

void PlayerWindow::saveSoloState()
{
    if (m_isSoloActive) {
        m_preSoloVolumes.resize(16);
        for (int i = 0; i < 16; ++i) {
            m_preSoloVolumes[i] = getChannelVolume(i);
        }
    }
}

void PlayerWindow::restoreSoloState()
{
    if (m_isSoloActive && m_soloChannel >= 0) {
        for (int i = 0; i < 16; ++i) {
            if (i == m_soloChannel)
                setChannelVolume(i, m_preSoloVolumes[i]);
            else
                setChannelVolume(i, 0);
        }
    }
}

void PlayerWindow::saveCurrentState()
{
    // Инициализируем вектор если нужно
    if (m_currentIndividualVolumes.size() != 16)
        m_currentIndividualVolumes.resize(16);


    for (int i = 0; i < 16; ++i) {
        m_currentIndividualVolumes[i] = m_channelVolumes[i];
    }

    m_wasSoloActive = m_isSoloActive;
    m_savedSoloChannel = m_soloChannel;
    m_savedPreSoloVolumes = m_preSoloVolumes; // Копируем весь вектор
}

void PlayerWindow::restoreInstrumentsAndControllers()
{
    auto& midiFile = Files[currentFileIndex];
    const auto& events = midiFile->events();

    for (QMidiEvent* event : events) {
        if (event->tick() <= m_previousTick) {
            if (event->type() == QMidiEvent::ProgramChange) {
                midiOut->sendEvent(*event);
                channelInstruments[event->voice()] = event->number();
            }
            else if (event->type() == QMidiEvent::ControlChange && event->number() != 7) {
                midiOut->sendEvent(*event);
            }
        }
    }
}

void PlayerWindow::restoreChannelVolumes()
{
    for (int channel = 0; channel < 16; ++channel) {
        m_channelVolumes[channel] = m_currentIndividualVolumes[channel];
        setChannelVolume(channel, m_currentIndividualVolumes[channel]);
    }
}

void PlayerWindow::updateProgressUI()
{
    int seconds = (currentPlayPos / 1000) % 60;
    int minutes = (currentPlayPos / 1000) / 60;

    ui->currentLabel->setText(QString("%1:%2")
        .arg(minutes, 2, 10, QLatin1Char('0'))
        .arg(seconds, 2, 10, QLatin1Char('0')));

    int progress = (currentPlayPos * 1000) / totalDur;
    ui->progressSlider->setValue(progress);
}

void PlayerWindow::setPlaybackSpeed(PlaybackSpeed speed)
{
    if (m_playbackSpeed == speed) return;

    // Сохраняем текущую позицию воспроизведения
    float currentTime = currentPlayPos / 1000.0f;
    bool wasPlaying = is_playing;
    bool wasPaused = is_paused;

    if (is_playing) playTimer.stop(); // Останавливаем таймер если играет

    m_playbackSpeed = speed;

    // Пересчитываем предыдущий тик для новой скорости
    if (currentFileIndex >= 0 && currentFileIndex < Files.size()) {
        auto& midiFile = Files[currentFileIndex];
        m_previousTick = midiFile->tickFromTime(currentTime);
    }

    // Перезапускаем таймер если воспроизведение было активно
    if (wasPlaying) {
        float speedMultiplier = playbackSpeedMultiplier();
        int timerInterval = 16 / speedMultiplier;
        playTimer.start(timerInterval);
    }

    // Если была пауза, обновляем позицию паузы
    if (wasPaused) {
        m_pausePosition = currentPlayPos;
    }

    savePlaybackSpeedSettings();
}

QString PlayerWindow::playbackSpeedName(PlaybackSpeed speed) const
{
    switch(speed) {
    case SpeedHalf: return "0.5x Speed";
    case SpeedNormal: return "Normal Speed";
    case SpeedDouble: return "2.0x Speed";
    default: return "Normal Speed";
    }
}

float PlayerWindow::playbackSpeedMultiplier() const
{
    float multiplier;
    switch(m_playbackSpeed) {
    case SpeedHalf: multiplier = 0.5f; break;
    case SpeedNormal: multiplier = 1.0f; break;
    case SpeedDouble: multiplier = 2.0f; break;
    default: multiplier = 1.0f; break;
    }
    return multiplier;
}

void PlayerWindow::savePlaybackSpeedSettings()
{
    QSettings settings;
    settings.setValue("playbackSpeed", static_cast<int>(m_playbackSpeed));
}

void PlayerWindow::loadPlaybackSpeedSettings()
{
    QSettings settings;
    m_playbackSpeed = static_cast<PlaybackSpeed>(
        settings.value("playbackSpeed", SpeedNormal).toInt());
}

void PlayerWindow::resetMidiState()
{
    resetAllNotes();//Останавливаем все ноты

    //Сбрасываем все контроллеры на всех каналах
    if (midiOut && midiOut->isConnected()) {
        for (int channel = 0; channel < 16; ++channel) {
            // Reset All Controllers (121)
            HelperClass::sendControlChange(channel, 121, 0);
            //midiOut->pitchWheel(channel, 8192);// Pitch Bend reset to center (8192 = center)
            HelperClass::sendControlChange(channel, 1, 0);// Modulation wheel reset
            HelperClass::sendControlChange(channel, 7, 100);// Volume reset
            HelperClass::sendControlChange(channel, 10, 64);// Pan reset to center
            HelperClass::sendControlChange(channel, 123, 0);// All Notes Off
            HelperClass::sendControlChange(channel, 120, 0);// All Sound Off
        }
    }

    //Очищаем внутренние состояния
    activeNotes.clear();
    channelInstruments.clear();

    //Сбрасываем визуализацию
    m_channelActivity.fill(false);
    if (m_visualizationPanel && ui->visualizationWidget->isVisible()) {
        m_visualizationPanel->updateChannels(m_channelActivity);
    }
}

void PlayerWindow::on_infoButton_clicked()
{
    QFile file(":/txt/about_player.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    QString infoText = in.readAll();
    file.close();

    if (infoText.isEmpty()) infoText = "Text unavailable";

    QDialog infoDialog(this);
    infoDialog.setWindowTitle("About MIDI Player");
    infoDialog.setFixedSize(700, 500);

    QVBoxLayout *layout = new QVBoxLayout(&infoDialog);
    layout->setContentsMargins(0, 0, 0, 0); // Нет отступов

    // Текстовое поле с прокруткой
    QTextEdit *textEdit = new QTextEdit(&infoDialog);
    textEdit->setPlainText(infoText);
    textEdit->setReadOnly(true);

    // Убираем рамку и настраиваем скроллбары
    textEdit->setFrameStyle(QFrame::NoFrame);
    textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // QFont infoFont("Bahnschrift", 16, QFont::Bold);
    // textEdit->setFont(infoFont);

    // Выравнивание + перенос
    textEdit->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    textEdit->setWordWrapMode(QTextOption::WordWrap);

    layout->addWidget(textEdit);// Текстовое поле в layout
    infoDialog.exec();
}

