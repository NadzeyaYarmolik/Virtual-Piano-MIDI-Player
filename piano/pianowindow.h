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
    const QVector<int> pianoScanCodes = { // Scan codes клавиш
            0x2C, 0x1F, 0x2D, 0x20, 0x2E, 0x2F, 0x22, 0x30,
            0x23, 0x31, 0x24, 0x32, 0x33, 0x03, 0x11, 0x04,
            0x12, 0x13, 0x06, 0x14, 0x07, 0x15, 0x08, 0x16, 0x17};
    const QVector<int> pianoKeyKeys = { // Qt key codes клавиш
        Qt::Key_Z, Qt::Key_S, Qt::Key_X, Qt::Key_D, Qt::Key_C,
        Qt::Key_V, Qt::Key_G, Qt::Key_B, Qt::Key_H, Qt::Key_N, Qt::Key_J, Qt::Key_M,
        Qt::Key_Comma, Qt::Key_2, Qt::Key_W, Qt::Key_3, Qt::Key_E,
        Qt::Key_R, Qt::Key_5, Qt::Key_T, Qt::Key_6, Qt::Key_Y, Qt::Key_7, Qt::Key_U, Qt::Key_I };

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
