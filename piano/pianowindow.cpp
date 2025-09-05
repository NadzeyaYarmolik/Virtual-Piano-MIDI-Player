#include "pianowindow.h"
#include "ui_pianowindow.h"
#include "ui_pianowindow.h"

PianoWindow::PianoWindow(QMidiOut* mainMidiOut, QWidget *parent)
    : QWidget(parent),
    ui(new Ui::PianoWindow), midiOut(mainMidiOut)
{
    ui->setupUi(this);

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

    QPixmap background(":/img/pianobackground.jpg");// Фон
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
    for (QPushButton* b : buttons) {
        b->setFocusPolicy(Qt::NoFocus);
    }

    setupKeyboardMapping();// Настройка через native scan codes чтоб работало на любой раскладке

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
    for (int channel = 0; channel < 16; ++channel) {
        midiOut->controlChange(channel, 7, midiVolume); // CC7 - Volume
    }
}
void PianoWindow::metronomeTick()
{
    if (!midiOut || !midiOut->isConnected()) return;

    // Выбор ноты на основе beat position
    // 75 - сильная доля, 76 - слабая доля
    int note = (metronomeBeat % 4 == 0) ? 75 : 76;
    int velocity = (metronomeBeat % 4 == 0) ? 100 : 80; // Разная громкость

    // Отправка NoteOn сообщения
    QMidiEvent event;
    event.setType(QMidiEvent::NoteOn);
    event.setNote(note);
    event.setVoice(9); // Канал 9 - ударные
    event.setVelocity(velocity);
    midiOut->sendEvent(event);

    // Автоматическое отпускание ноты через 50ms
    // Создает короткий щелчок метронома
    QTimer::singleShot(50, this, [this, note]() {
        if (midiOut && midiOut->isConnected()) {
            QMidiEvent eventOff;
            eventOff.setType(QMidiEvent::NoteOff);
            eventOff.setNote(note);
            eventOff.setVoice(9);
            midiOut->sendEvent(eventOff);
        }
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

    // Проверка если есть mapping у клавиши
    if (!m_keyToButtonMap.contains(sc)) return;
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
    QVector<QPushButton*> keys = {ui->c4, ui->c_4, ui->d4, ui->d_4, ui->e4,
        ui->f4, ui->f_4, ui->g4, ui->g_4, ui->a4, ui->a_4, ui->b4, ui->c5, ui->c_5, ui->d5,
        ui->d_5, ui->e5, ui->f5, ui->f_5, ui->g5, ui->g_5, ui->a5, ui->a_5, ui->b5, ui->c6};

    // Кнопка + MIDI-нота ударных
    drums = {{ui->bassDrum, 35}, {ui->snareDrum, 38}, {ui->highTom, 48}, {ui->midTom, 47},
    {ui->floorTom, 43}, {ui->hiHat, 46}, {ui->crashCymbal1, 49}, {ui->crashCymbal2, 57}, {ui->rideCymbal, 51}};

    // Настройка обработчиков для клавиш пианино
    for (int i = 0; i < keys.size(); ++i) {
        keys[i]->setFocusPolicy(Qt::StrongFocus);
        keys[i]->setAutoRepeat(false);

        // Обработчик нажатия клавиши
        connect(keys[i], &QPushButton::pressed, [this, i]() {
            int note = i + (currentOctave + 1) * 12; // Расчет ноты
            if (!activeNotes.value(note, false)) {
                activeNotes[note] = true;
                HelperClass::sendNoteOn(note, 0);
            }
        });

        // Обработчик отпускания клавиши
        connect(keys[i], &QPushButton::released, [this, i]() {
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
    for (int i = 0; i < drums.size(); ++i) {
        drums[i].first->setFocusPolicy(Qt::StrongFocus);
        drums[i].first->setAutoRepeat(false);

        HelperClass::setupButtonAnimation(drums[i].first, drums[i].first->iconSize(), 100);

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
        metronomeTimer->stop();// Остановка метронома
    else {
        // Запуск с текущим BPM
        metronomeBeat = 0;
        int bpm = ui->bpmSlider->value();
        int interval = 60000 / bpm; // Расчет интервала в ms
        metronomeTimer->start(interval);
        metronomeTick();
    }
}

void PianoWindow::setupKeyboardMapping()
{
    m_keyToButtonMap.clear();
    m_pressedKeys.clear();

    // Добавление mapping
    auto mapSc = [this](int sc, QPushButton* btn) {
        if (!btn) return;
        m_keyToButtonMap[sc] = btn;
    };

    // Определение scan codes для клавиш
    const int SC_Z = 0x2C;
    const int SC_X = 0x2D;
    const int SC_C = 0x2E;
    const int SC_V = 0x2F;
    const int SC_B = 0x30;
    const int SC_N = 0x31;
    const int SC_M = 0x32;
    const int SC_COMMA = 0x33;
    const int SC_Q = 0x10;
    const int SC_W = 0x11;
    const int SC_E = 0x12;
    const int SC_R = 0x13;
    const int SC_T = 0x14;
    const int SC_Y = 0x15;
    const int SC_U = 0x16;
    const int SC_I = 0x17;

    const int SC_S = 0x1F;
    const int SC_D = 0x20;
    const int SC_G = 0x22;
    const int SC_H = 0x23;
    const int SC_J = 0x24;
    const int SC_2 = 0x03;
    const int SC_3 = 0x04;
    const int SC_5 = 0x06;
    const int SC_6 = 0x07;
    const int SC_7 = 0x08;

    // Белые
    mapSc(SC_Z, ui->c4);
    mapSc(SC_X, ui->d4);
    mapSc(SC_C, ui->e4);
    mapSc(SC_V, ui->f4);
    mapSc(SC_B, ui->g4);
    mapSc(SC_N, ui->a4);
    mapSc(SC_M, ui->b4);
    mapSc(SC_COMMA, ui->c5);
    mapSc(SC_Q, ui->c5);
    mapSc(SC_W, ui->d5);
    mapSc(SC_E, ui->e5);
    mapSc(SC_R, ui->f5);
    mapSc(SC_T, ui->g5);
    mapSc(SC_Y, ui->a5);
    mapSc(SC_U, ui->b5);
    mapSc(SC_I, ui->c6);

    // Черные
    mapSc(SC_S, ui->c_4);
    mapSc(SC_D, ui->d_4);
    mapSc(SC_G, ui->f_4);
    mapSc(SC_H, ui->g_4);
    mapSc(SC_J, ui->a_4);
    mapSc(SC_2, ui->c_5);
    mapSc(SC_3, ui->d_5);
    mapSc(SC_5, ui->f_5);
    mapSc(SC_6, ui->g_5);
    mapSc(SC_7, ui->a_5);
}

void PianoWindow::initButtonMappings()
{
    keyToButtonMap.clear();

    // Пример: связываем клавиши с кнопками из дизайнера
    keyToButtonMap[Qt::Key_Z] = ui->c4;
    keyToButtonMap[Qt::Key_S] = ui->c_4;
    keyToButtonMap[Qt::Key_X] = ui->d4;
    keyToButtonMap[Qt::Key_D] = ui->d_4;
    keyToButtonMap[Qt::Key_C] = ui->e4;
    keyToButtonMap[Qt::Key_V] = ui->f4;
    keyToButtonMap[Qt::Key_G] = ui->f_4;
    keyToButtonMap[Qt::Key_B] = ui->g4;
    keyToButtonMap[Qt::Key_H] = ui->g_4;
    keyToButtonMap[Qt::Key_N] = ui->a4;
    keyToButtonMap[Qt::Key_J] = ui->a_4;
    keyToButtonMap[Qt::Key_M] = ui->b4;
    keyToButtonMap[Qt::Key_Comma] = ui->c5;
    keyToButtonMap[Qt::Key_Q] = ui->c5;   // вторая клавиша для C5
    keyToButtonMap[Qt::Key_W] = ui->d5;
    keyToButtonMap[Qt::Key_3] = ui->d_5;
    keyToButtonMap[Qt::Key_E] = ui->e5;
    keyToButtonMap[Qt::Key_R] = ui->f5;
    keyToButtonMap[Qt::Key_5] = ui->f_5;
    keyToButtonMap[Qt::Key_T] = ui->g5;
    keyToButtonMap[Qt::Key_6] = ui->g_5;
    keyToButtonMap[Qt::Key_Y] = ui->a5;
    keyToButtonMap[Qt::Key_7] = ui->a_5;
    keyToButtonMap[Qt::Key_U] = ui->b5;
    keyToButtonMap[Qt::Key_I] = ui->c6;
}

void PianoWindow::initKeyMappings()
{
    keyToNoteMap.clear();

    int baseOctave = currentOctave + 1;

    // Белые клавиши
    keyToNoteMap[Qt::Key_Z] = 12 * baseOctave + 0;  // C4
    keyToNoteMap[Qt::Key_X] = 12 * baseOctave + 2;  // D4
    keyToNoteMap[Qt::Key_C] = 12 * baseOctave + 4;  // E4
    keyToNoteMap[Qt::Key_V] = 12 * baseOctave + 5;  // F4
    keyToNoteMap[Qt::Key_B] = 12 * baseOctave + 7;  // G4
    keyToNoteMap[Qt::Key_N] = 12 * baseOctave + 9;  // A4
    keyToNoteMap[Qt::Key_M] = 12 * baseOctave + 11; // B4

    // Запятая и альт-варианты
    int c5Note = 12 * (baseOctave + 1);
    keyToNoteMap[Qt::Key_Comma] = c5Note;
    keyToNoteMap[Qt::Key_Period] = c5Note;  // на всякий случай
    keyToNoteMap[Qt::Key_Less] = c5Note;    // для раскладок

    // Альтернативная клавиша C5 (Q)
    keyToNoteMap[Qt::Key_Q] = c5Note;

    keyToNoteMap[Qt::Key_W] = 12 * (baseOctave + 1) + 2; // D5
    keyToNoteMap[Qt::Key_E] = 12 * (baseOctave + 1) + 4; // E5
    keyToNoteMap[Qt::Key_R] = 12 * (baseOctave + 1) + 5; // F5
    keyToNoteMap[Qt::Key_T] = 12 * (baseOctave + 1) + 7; // G5
    keyToNoteMap[Qt::Key_Y] = 12 * (baseOctave + 1) + 9; // A5
    keyToNoteMap[Qt::Key_U] = 12 * (baseOctave + 1) + 11;// B5
    keyToNoteMap[Qt::Key_I] = 12 * (baseOctave + 2);     // C6

    // Черные клавиши
    keyToNoteMap[Qt::Key_S] = 12 * baseOctave + 1;       // C#4
    keyToNoteMap[Qt::Key_D] = 12 * baseOctave + 3;       // D#4
    keyToNoteMap[Qt::Key_G] = 12 * baseOctave + 6;       // F#4
    keyToNoteMap[Qt::Key_H] = 12 * baseOctave + 8;       // G#4
    keyToNoteMap[Qt::Key_J] = 12 * baseOctave + 10;      // A#4
    keyToNoteMap[Qt::Key_2] = 12 * (baseOctave + 1) + 1; // C#5
    keyToNoteMap[Qt::Key_3] = 12 * (baseOctave + 1) + 3; // D#5
    keyToNoteMap[Qt::Key_5] = 12 * (baseOctave + 1) + 6; // F#5
    keyToNoteMap[Qt::Key_6] = 12 * (baseOctave + 1) + 8; // G#5
    keyToNoteMap[Qt::Key_7] = 12 * (baseOctave + 1) + 10;// A#5
}

void PianoWindow::playNote(int note, int channel, bool pressed)
{
    if (!midiOut || !midiOut->isConnected()) return;

    // Использование отдельного идентификатора для ударных
    int noteId = (channel == 9) ? note + 128 : note;

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

    layout->addWidget(textEdit);// Текстовое поле в layout
    infoDialog.exec();
}

