#include "pianowindow.h"
#include "ui_pianowindow.h"
#include "ui_pianowindow.h"

PianoWindow::PianoWindow(QMidiOut* mainMidiOut, QWidget *parent)
    : QWidget(parent),
    ui(new Ui::PianoWindow), midiOut(mainMidiOut)
{
    ui->setupUi(this);

    pianoKeys = {
        ui->c4, ui->c_4, ui->d4, ui->d_4, ui->e4,
        ui->f4, ui->f_4, ui->g4, ui->g_4, ui->a4, ui->a_4, ui->b4,
        ui->c5, ui->c_5, ui->d5, ui->d_5, ui->e5,
        ui->f5, ui->f_5, ui->g5, ui->g_5, ui->a5, ui->a_5, ui->b5, ui->c6
    };

    // Шрифт для всего окна
    QFont pianoFont("Bahnschrift", 16, QFont::Bold);
    this->setFont(pianoFont);

    // Применение шрифта ко всем дочерним элементам
    QList<QWidget*> children = this->findChildren<QWidget*>();
    foreach (QWidget* child, children) {
        child->setFont(pianoFont);
    }
    ui->bpmLabel->setFont(pianoFont);
    ui->bpmSlider->setTracking(true); // Непрерывное обновление при движении слайдера

    // Установка политики фокуса для обработки клавиатуры
    // StrongFocus позволяет окну получать все keyboard events
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    // Настройка слайдера BPM
    ui->bpmSlider->setRange(40, 400);
    ui->bpmSlider->setValue(120); // Стандартный темп
    connect(ui->bpmSlider, &QSlider::valueChanged, this, [this](int value) {
        ui->bpmLabel->setText(QString::number(value) + " BPM");
        if (metronomeTimer->isActive()) {
            int interval = 60000 / value; // Расчет интервала в миллисекундах
            metronomeTimer->setInterval(interval);
        }
    });

    // Анимация кнопок
    HelperClass::setupButtonAnimation(ui->metronomeButton, ui->metronomeButton->iconSize(), 100);
    HelperClass::setupButtonAnimation(ui->infoButton, ui->infoButton->iconSize(), 100);
    HelperClass::setupButtonAnimation(ui->homeButton, ui->homeButton->iconSize(), 100);

    QPixmap background(":/img/piano/background.jpg");// Фон
    if (!background.isNull()) {
        resize(background.size());
        setFixedSize(background.size()); // Фиксированный размер окна
        this->setFixedSize(background.size());

        QPalette palette;
        palette.setBrush(QPalette::Window, QBrush(background));
        this->setPalette(palette);
        this->setAutoFillBackground(true);
    }

    ui->volumeDial->setValue(100);

    // Список инструментов из HelperClass
    for (int i = 0; i < HelperClass::instruments.size(); ++i) {
        ui->instrumentBox->addItem(HelperClass::instruments[i], i);
    }

    // Устанавливаем пианино в MIDI
    if (midiOut->isConnected()) {
        QMidiEvent event;
        event.setType(QMidiEvent::ProgramChange);
        event.setVoice(0);
        event.setNumber(0);
        midiOut->sendEvent(event);
    }

    ui->octaveBox->setValue(currentOctave); // Установка текущей октавы

    connect(ui->instrumentBox, &QComboBox::currentIndexChanged,
            this, &PianoWindow::onInstrumentChanged);
    connect(ui->octaveBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &PianoWindow::onOctaveChanged);

    // Чтобы не реагировали на клаву
    ui->instrumentBox->setFocusPolicy(Qt::ClickFocus);
    ui->octaveBox->setFocusPolicy(Qt::NoFocus);

    // Возврат фокуса окну после изменения инструмента
    // Обеспечивает продолжение обработки клавиатурных событий
    connect(ui->instrumentBox, &QComboBox::currentIndexChanged,
            this, [this](int index) {
                onInstrumentChanged(index);
                this->setFocus();
            });

    // Возврат фокуса окну после изменения октавы
    // Поддерживает непрерывную игру на клавиатуре
    connect(ui->octaveBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, [this](int value) {
                onOctaveChanged(value);
                this->setFocus();
            });

    // Настройка регулятора громкости 1-100%
    ui->volumeDial->setRange(1, 100);
    ui->volumeDial->setValue(volume);
    ui->volumeDial->setWrapping(false);
    connect(ui->volumeDial, &QDial::valueChanged,
            this, &PianoWindow::updateVolume);

    // Инициализация таймера метронома. Интервал будет установлен основан на величине BPM
    metronomeTimer = new QTimer(this);
    connect(metronomeTimer, &QTimer::timeout, this, &PianoWindow::metronomeTick);
    connect(ui->metronomeButton, &QPushButton::clicked, this, &PianoWindow::toggleMetronome);

    setupPianoKeys();

    const auto buttons = this->findChildren<QPushButton*>();
    for (QPushButton* b : buttons)
        b->setFocusPolicy(Qt::NoFocus);

    setupKeyboardMapping(); // Настройка через native scan codes чтоб работало на любой раскладке

    // Возврат на главный экран
    connect(ui->homeButton, &QPushButton::clicked, this, [this]() {
        emit homeButtonClicked();
    });
}

PianoWindow::~PianoWindow()
{
    // Остановка метронома
    if (metronomeTimer) {
        metronomeTimer->stop();
        disconnect(metronomeTimer, nullptr, this, nullptr);
    }

    activeNotes.clear();
    m_pressedKeys.clear();
    m_keyToButtonMap.clear();

    delete ui;
}

void PianoWindow::closeEvent(QCloseEvent *event)
{
    // Остановить метроном при закрытии
    if (metronomeTimer && metronomeTimer->isActive())
        metronomeTimer->stop();

    // Отпускание всех активных нот чтобы не зависали при закрытии
    for (auto it = activeNotes.begin(); it != activeNotes.end(); ++it) {
        if (it.value()) {
            int note = it.key();
            int channel = (note >= 128) ? 9 : 0; // Канал 9 - ударные
            if (note >= 128) note -= 128; // Коррекция номера ноты для ударных
            HelperClass::sendNoteOff(note, channel);
        }
    }
    activeNotes.clear();

    emit homeButtonClicked();
    event->accept();
}

void PianoWindow::updateVolume(int volume)
{
    if (!midiOut || !midiOut->isConnected()) return;
    int midiVolume = volume * 127 / 100;
    // Установка громкости для всех 16 каналов
    for (int channel = 0; channel < 16; ++channel)
        HelperClass::sendControlChange(channel, 7, midiVolume);
}

void PianoWindow::metronomeTick()
{
    if (!midiOut || !midiOut->isConnected()) return;

    // Выбор ноты на основе beat position
    // 75 - сильная доля, 76 - слабая доля
    int note = (metronomeBeat % 4 == 0) ? 75 : 76;
    int velocity = (metronomeBeat % 4 == 0) ? 100 : 80; // Разная громкость

    HelperClass::sendNoteOn(note, 9, velocity); // канал 9 - ударные

    // Автоматическое отпускание ноты через 50ms
    // Создает короткий щелчок метронома
    QTimer::singleShot(50, this, [ note]() {
        HelperClass::sendNoteOff(note, 9); // канал 9 - ударные
    });

    metronomeBeat++; // Следующая доля
}

void PianoWindow::keyPressEvent(QKeyEvent *event)
{
    const int sc = event->nativeScanCode();
    if (event->isAutoRepeat()) return; // Игнор auto-repeat

    if (sc == 0x39) { // ScanCode пробела (вкл/выкл метроном)
        toggleMetronome();
        return;
    }

    if (!m_keyToButtonMap.contains(sc)) return; // Проверка если есть mapping у клавиши
    if (m_pressedKeys.contains(sc)) return; // Предотвратить повторную обработку

    m_pressedKeys.insert(sc);
    // Активация клавиш
    if (QPushButton* button = m_keyToButtonMap.value(sc)) {
        button->setDown(true);
        QMetaObject::invokeMethod(button, "pressed", Qt::QueuedConnection);
    }
}

void PianoWindow::keyReleaseEvent(QKeyEvent *event)
{
    const int sc = event->nativeScanCode();
    if (event->isAutoRepeat()) return; // Игнорирование auto-repeat

    // Проверка если есть mapping у клавиши
    if (!m_keyToButtonMap.contains(sc)) return;
    if (!m_pressedKeys.contains(sc)) return;

    m_pressedKeys.remove(sc);
    // Деактивация клавиши
    if (QPushButton* button = m_keyToButtonMap.value(sc)) {
        button->setDown(false);
        QMetaObject::invokeMethod(button, "released", Qt::QueuedConnection);
    }
}

void PianoWindow::onInstrumentChanged(int index) {
    currentInstrument = ui->instrumentBox->itemData(index).toInt();
    if (midiOut->isConnected()) {
        QMidiEvent event;
        event.setType(QMidiEvent::ProgramChange);
        event.setVoice(0); // Канал 0 - мелодические инструменты
        event.setNumber(currentInstrument);
        midiOut->sendEvent(event);
    }
}

void PianoWindow::onOctaveChanged(int octave){
    currentOctave = octave;}

void PianoWindow::on_homeButton_clicked(){
    emit homeButtonClicked();
    this->close();
}

void PianoWindow::setupPianoKeys() {

    // Кнопка + MIDI-нота ударных
    drums = {{ui->bassDrum, 35}, {ui->snareDrum, 38}, {ui->highTom, 48}, {ui->midTom, 47},
    {ui->floorTom, 43}, {ui->hiHat, 46}, {ui->crashCymbal1, 49}, {ui->crashCymbal2, 57}, {ui->rideCymbal, 51}};

    // Настройка обработчиков для клавиш пианино
    for (int i = 0; i < 25; ++i) {
        pianoKeys[i]->setFocusPolicy(Qt::StrongFocus);
        pianoKeys[i]->setAutoRepeat(false);

        // Обработчик нажатия клавиши
        connect(pianoKeys[i], &QPushButton::pressed, [this, i]() {
            int note = i + (currentOctave + 1) * 12; // Расчет ноты
            if (!activeNotes.value(note, false)) {
                activeNotes[note] = true;
                HelperClass::sendNoteOn(note, 0);
            }
        });

        // Обработчик отпускания клавиши
        connect(pianoKeys[i], &QPushButton::released, [this, i]() {
            int note = i + (currentOctave + 1) * 12;
            if (activeNotes.value(note, false)) {
                activeNotes[note] = false;
                HelperClass::sendNoteOff(note, 0);
            }
        });

        // Отключение фокуса у всех кнопок
        QList<QPushButton*> buttons = this->findChildren<QPushButton*>();
        for (QPushButton* b : buttons) {
            b->setFocusPolicy(Qt::NoFocus);
        }
    }

    // Настройка ударных инструментов
    for (int i = 0; i < 9; ++i) {
        drums[i].first->setFocusPolicy(Qt::StrongFocus);
        drums[i].first->setAutoRepeat(false);

        HelperClass::setupButtonAnimation(drums[i].first, drums[i].first->iconSize());

        // Нажатие на ударник
        connect(drums[i].first, &QPushButton::pressed, [this, i]() {
            if (!activeNotes.value(drums[i].second+128, false)) {
                activeNotes[drums[i].second+128] = true;
                HelperClass::sendNoteOn(drums[i].second, 9); // Канал 9 - ударники
            }
        });

        // Отпускание ударника
        connect(drums[i].first, &QPushButton::released, [this, i]() {
            if (activeNotes.value(drums[i].second+128, false)) {
                activeNotes[drums[i].second+128] = false;
                HelperClass::sendNoteOff(drums[i].second, 9);
            }
        });
    }
}

void PianoWindow::toggleMetronome()
{
    if (metronomeTimer->isActive())
        metronomeTimer->stop(); // Остановка метронома
    else {
        // Запуск с текущим BPM
        metronomeBeat = 0;
        int bpm = ui->bpmSlider->value();
        int interval = 60000 / bpm; // Расчет интервала в ms
        metronomeTimer->start(interval);
        metronomeTick();
    }
}

void PianoWindow::setupKeyboardMapping() {
    m_keyToButtonMap.clear();
    m_pressedKeys.clear();

    // Основной цикл для всех клавиш
    for (int i = 0; i < 25; ++i) {
        if (pianoKeys[i]) // Проверка на nullptr
            m_keyToButtonMap[pianoScanCodes[i]] = pianoKeys[i];
    }

    m_keyToButtonMap[0x10] = ui->c5; // клавиша Q для ui->c5
}

void PianoWindow::initButtonMappings()
{
    keyToButtonMap.clear();

    for (int i = 0; i < 25; ++i) // Присваивание
        keyToButtonMap[pianoKeyKeys[i]] = pianoKeys[i];

    keyToButtonMap[Qt::Key_Q] = ui->c5; // Особое присваивание для клавиши Q - переопределяем предыдущее
}

void PianoWindow::initKeyMappings()
{
    keyToNoteMap.clear();

    int baseOctave = currentOctave + 1;

    // Основной цикл для всех 24 клавиш
    for (int i = 0; i < 25; ++i) {
        int octaveOffset = i / 12;  // Каждые 12 нот увеличиваем октаву
        int noteOffset = i % 12;    // Номер ноты внутри октавы (0-11)

        int note = 12 * (baseOctave + octaveOffset) + noteOffset;
        keyToNoteMap[pianoKeyKeys[i]] = note;
    }

    // Особый случай: клавиша Q - дубликат для C5 (нота 60 при baseOctave = 4)
    keyToNoteMap[Qt::Key_Q] = 12 * baseOctave + 12; // C5 (60)
}

void PianoWindow::playNote(int note, int channel, bool pressed) {
    if (!midiOut || !midiOut->isConnected()) return;

    int noteId = (channel == 9) ? note + 128 : note; // Отдельный идентификатор для ударных

    if (pressed) {
        if (!activeNotes.value(noteId, false)) {
            activeNotes[noteId] = true;
            HelperClass::sendNoteOn(note, channel, 100); // Фиксированная скорость
        }
    } else {
        if (activeNotes.value(noteId, false)) {
            activeNotes[noteId] = false;
            HelperClass::sendNoteOff(note, channel);
        }
    }
}

void PianoWindow::on_infoButton_clicked()
{
    QFile file(":/txt/about_piano.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    QString infoText = in.readAll();
    file.close();

    if (infoText.isEmpty()) infoText = "Text unavailable";

    QDialog infoDialog(this);
    infoDialog.setWindowTitle("About Virtual Piano");
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

    QFont infoFont("Bahnschrift", 16, QFont::Bold);
    textEdit->setFont(infoFont);

    // Выравнивание + перенос
    textEdit->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    textEdit->setWordWrapMode(QTextOption::WordWrap);

    layout->addWidget(textEdit); // Текстовое поле в layout
    infoDialog.exec();
}

