#ifndef HELPERCLASS_H
#define HELPERCLASS_H

#include <QObject>
#include <QAbstractButton>
#include <QPropertyAnimation>
#include <QMap>
#include "thirdparty/QMidi-master/src/QMidiOut.h"

class HelperClass
{
public:
    HelperClass();
    // Настройка анимации для кнопки
    // указатель на кнопку, исходный размер иконки, длительность анимации в мс
    static void setupButtonAnimation(QAbstractButton* button, const QSize& baseSize, int duration = 100);

    static QMidiOut* midiOut; // Статический указатель на MIDI устройство

    static void setMidiOut(QMidiOut* midiDevice); // Новый метод для установки MIDI

    static void sendNoteOn(int note, int channel = 0, int velocity = 64);

    static void sendNoteOff(int note, int channel = 0);

    static const QStringList instruments;

    static QString getInstrumentName(int programNumber, int channel);

    static void sendControlChange(int channel, int controller, int value);

private:
    // Хранилище исходных размеров иконок для всех кнопок
    // Ключ - указатель на кнопку, значение - исходный размер иконки
    static QMap<QAbstractButton*, QSize> buttonBaseSizes;

    // Внутренний метод для выполнения анимации
    //кнопка для анимации, состояние кнопки (нажата/отпущена), длительность анимации
    static void animateButton(QAbstractButton* button, bool pressed, int duration);
};

#endif // HELPERCLASS_H
