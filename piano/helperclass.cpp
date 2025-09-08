#include "helperclass.h"
#include "QMidiFile.h"

HelperClass::HelperClass() {}

QMap<QAbstractButton*, QSize> HelperClass::buttonBaseSizes;

void HelperClass::setMidiOut(QMidiOut* midiDevice)
{
    midiOut = midiDevice;
}

QMidiOut* HelperClass::midiOut = nullptr;

void HelperClass::setupButtonAnimation(QAbstractButton* button, const QSize& baseSize, int duration)
{
    if (!button) return; // Проверка валидности указателя на кнопку

    buttonBaseSizes[button] = baseSize; // Сохраняем исходный размер иконки
    button->setIconSize(baseSize); // Устанавливаем начальный размер

    // Подключаем обработчик нажатия кнопки
    QAbstractButton::connect(button, &QAbstractButton::pressed, [=]() {
        animateButton(button, true, duration); // Анимация при нажатии
    });

    // Подключаем обработчик отпускания кнопки
    QAbstractButton::connect(button, &QAbstractButton::released, [=]() {
        animateButton(button, false, duration); // Анимация при отпускании
    });
}

void HelperClass::animateButton(QAbstractButton* button, bool pressed, int duration)
{
    // Проверяем, что кнопка существует и для нее задан базовый размер
    if (!button || !buttonBaseSizes.contains(button)) return;

    QSize baseSize = buttonBaseSizes[button]; // Получаем сохраненный исходный размер

    // При нажатии - уменьшаем на 15%, При отпускании - возвращаем исходный размер
    QSize targetSize = pressed ?
        QSize(baseSize.width() * 0.85, baseSize.height() * 0.85) : baseSize;

    // Создаем анимацию свойства iconSize кнопки
    QPropertyAnimation* animation = new QPropertyAnimation(button, "iconSize");

    // Настройка
    animation->setDuration(duration); // Длительность
    animation->setStartValue(button->iconSize()); // Начальное значение (текущий размер)
    animation->setEndValue(targetSize); // Конечное значение
    animation->setEasingCurve(QEasingCurve::OutQuad); // Плавность анимации

    // Удаляем анимацию после завершения дабы не было утечки памяти
    QAbstractAnimation::connect(animation, &QPropertyAnimation::finished,
            animation, &QPropertyAnimation::deleteLater);

    animation->start(); // Запуск
}

void HelperClass::sendNoteOn(int note, int channel, int velocity) {
    //Проверка: подключено ли устройство
    if (!midiOut || !midiOut->isConnected()) return;
    //Событие типа NoteOn
    //Нота, 0 канал, скорость + отправка события
    QMidiEvent event;
    event.setType(QMidiEvent::NoteOn);
    event.setNote(note);
    event.setVoice(channel);
    event.setVelocity(velocity);
    midiOut->sendEvent(event);

}

void HelperClass::sendNoteOff(int note, int channel) {
    //Проверка: подключено ли устройство
    if (!midiOut || !midiOut->isConnected()) return;
    //Событие типа NoteOff
    //Нота, 0 канал, 0 скорость + отправка события
    QMidiEvent event;
    event.setType(QMidiEvent::NoteOff);
    event.setNote(note);
    event.setVoice(channel);
    event.setVelocity(0);
    midiOut->sendEvent(event);
}

const QStringList HelperClass::instruments={ //названия инструментов
    "Acoustic Grand Piano","Bright Acoustic Piano", "Electric Grand Piano", "Honky-tonk Piano",                  // Piano
    "Electric Piano 1", "Electric Piano 2", "Harpsichord", "Clavinet",
    "Celesta", "Glockenspiel", "Music Box", "Vibraphone",                                                        // Chromatic Percussion
    "Marimba", "Xylophone", "Tubular Bells", "Dulcimer",
    "Drawbar Organ", "Percussive Organ", "Rock Organ", "Church Organ",                                           // Organ
    "Reed Organ", "Accordion", "Harmonica", "Tango Accordion",
    "Acoustic Guitar (nylon)", "Acoustic Guitar (steel)", "Electric Guitar (jazz)", "Electric Guitar (clean)",   // Guitar
    "Electric Guitar (muted)", "Overdriven Guitar", "Distortion Guitar", "Guitar Harmonics",
    "Acoustic Bass", "Electric Bass (finger)", "Electric Bass (picked)", "Electric Bass (fretless)",             // Bass
    "Slap Bass 1", "Slap Bass 2", "Synth Bass 1", "Synth Bass 2",
    "Violin", "Viola", "Cello", "Contrabass",                                                                    // Strings
    "Tremolo Strings", "Pizzicato Strings", "Orchestral Harp", "Timpani",
    "String Ensemble 1", "String Ensemble 2", "Synth Strings 1", "Synth Strings 2",                              // Ensemble
    "Choir Aahs", "Voice Oohs", "Synth Voice", "Orchestra Hit",
    "Trumpet", "Trombone", "Tuba", "Muted Trumpet",                                                              // Brass
    "French Horn", "Brass Section", "Synth Brass 1", "Synth Brass 2",
    "Soprano Sax", "Alto Sax", "Tenor Sax", "Baritone Sax",                                                      // Reed
    "Oboe", "English Horn", "Bassoon", "Clarinet",
    "Piccolo", "Flute", "Recorder", "Pan Flute",                                                                 // Pipe
    "Blown bottle", "Shakuhachi", "Whistle", "Ocarina",
    "Lead 1 (square)", "Lead 2 (sawtooth)", "Lead 3 (calliope)", "Lead 4 (chiff)",                               // Synth Lead
    "Lead 5 (charang)", "Lead 6 (voice)", "Lead 7 (fifths)", "Lead 8 (bass and lead)",
    "Pad 1 (new age)", "Pad 2 (warm)", "Pad 3 (polysynth)", "Pad 4 (choir)",                                     // Synth Pad
    "Pad 5 (bowed glass)", "Pad 6 (metallic)", "Pad 7 (halo)", "Pad 8 (sweep)",
    "FX 1 (rain)", "FX 2 (soundtrack)", "FX 3 (crystal)", "FX 4 (atmosphere)",                                   // Synth Effects
    "FX 5 (brightness)", "FX 6 (goblins)", "FX 7 (echoes)", "FX 8 (sci-fi)",
    "Sitar", "Banjo", "Shamisen", "Koto",                                                                        // Ethnic
    "Kalimba", "Bag pipe", "Fiddle", "Shanai",
    "Tinkle Bell", "Agogo", "Steel Drums", "Woodblock",                                                          // Percussive
    "Taiko Drum", "Melodic Tom", "Synth Drum", "Reverse Cymbal",
    "Guitar Fret Noise", "Breath Noise", "Seashore", "Bird Tweet",                                               // Sound Effects
    "Telephone Ring", "Helicopter", "Applause", "Gunshot"
};

QString HelperClass::getInstrumentName(int programNumber, int channel)
{
    if (channel == 9) // 10-й канал
        return "Drums";

    if (programNumber >= 0 && programNumber < instruments.size())
        return instruments[programNumber];

    return "Unknown Instrument";
}

void HelperClass::sendControlChange(int channel, int controllerNumber, int controllerValue) {
    if (!midiOut || !midiOut->isConnected()) return; // Подключено ли устройство

    // Создаём событие типа ControlChange
    QMidiEvent event;
    event.setType(QMidiEvent::ControlChange);
    event.setVoice(channel);           // канал
    event.setNumber(controllerNumber); // номер контроллера (например 64 = sustain)
    event.setValue(controllerValue);   // значение (0-127)

    midiOut->sendEvent(event);
}

