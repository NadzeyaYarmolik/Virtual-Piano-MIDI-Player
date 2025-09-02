#ifndef PIANOWINDOW_H
#define PIANOWINDOW_H

#include <QStyle>
#include <QComboBox>
#include <QSoundEffect>
#include <QPushButton>
#include <QDial>
#include <QShortcut>
#include <QApplication>
#include <QTimer>
#include <QTextEdit>
#include "thirdparty/QMidi-master/src/QMidiOut.h"
#include "playerwindow.h"
#include "helperclass.h"
#include <windows.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class PianoWindow;
}
QT_END_NAMESPACE

class MainWindow;

// Основное окно виртуального пианино
class PianoWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PianoWindow(QMidiOut* mainMidiOut, QWidget *parent = nullptr);
    ~PianoWindow();

    void updateVolume(int volume); // Обновляет громкость MIDI

protected:
    void keyPressEvent(QKeyEvent *event) override; // Обработка нажатия клавиш
    void keyReleaseEvent(QKeyEvent *event) override; // Обработка отпускания клавиш
    void closeEvent(QCloseEvent *event) override; // Обработка закрытия окна

private slots:
    void onInstrumentChanged(int index); // Смена инструмента
    void onOctaveChanged(int octave); // Смена октавы
    void on_homeButton_clicked(); // Нажатие кнопки домой
    void on_infoButton_clicked(); // Нажатие кнопки информации

private:
    Ui::PianoWindow *ui;
    QMidiOut *midiOut; // MIDI выход
    HelperClass* helper; // Вспомогательный класс
    int volume = 100; // Текущая громкость
    int currentInstrument = 0; // Текущий инструмент
    int currentOctave = 4; // Текущая октава
    int currentVelocity = 64; // Скорость нажатия

    QVector<QPushButton*> pianoKeys; // Все клавиши пианино
    QVector<int> keyScanCodes; // Scan codes клавиш
    QVector<int> keyQtKeys; // Qt key codes клавиш

    void setupPianoKeys(); // Настройка клавиш пианино

    QMap<int, QTimer*> noteTimers; // Таймеры нот
    QSet<int> pressedNotes; // Нажатые ноты
    QMap<int, bool> activeNotes; // Активные ноты

    QVector<std::pair<QPushButton*, int>> drums; // Кнопки ударных

    QTimer* metronomeTimer; // Таймер метронома
    int metronomeBeat = 0; // Счетчик долей

    void metronomeTick(); // Тик метронома
    void toggleMetronome(); // Вкл/выкл метроном

    void setupKeyboardMapping(); // Настройка mapping клавиатуры

    QMap<int, QPushButton*> m_keyToButtonMap; // Mapping клавиш на кнопки
    QVector<bool> m_keyboardState; // Состояние клавиатуры
    QSet<int> m_pressedKeys; // Нажатые клавиши
    QMap<int, QShortcut*> m_keyShortcuts; // Горячие клавиши

    QMap<int, int> keyToNoteMap; // Mapping клавиш на ноты
    QMap<int, QPushButton*> keyToButtonMap; // Mapping клавиш на кнопки UI
    QSet<int> pressedKeys; // Текущие зажатые клавиши

    void initKeyMappings(); // Инициализация mapping нот
    void initButtonMappings(); // Инициализация mapping кнопок
    void initializeTimers(); // Инициализация таймеров
    void initializeMIDI(); // Инициализация MIDI
    void initializeUI(); // Инициализация UI
    void stopAllNotes(); // Остановка всех нот
    void playNote(int note, int channel, bool pressed); // Воспроизведение ноты
    void setupDrums(); // Настройка ударников
    void setupPianoKey(QPushButton *button, int note, int channel); // Настройка клавиши

signals:
    void homeButtonClicked();
    void windowClosed();
};
#endif // PIANOWINDOW_H
