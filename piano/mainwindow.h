#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStyle>
#include <QComboBox>
#include <QSoundEffect>
#include <QPushButton>
#include <QTimer>
#include "thirdparty/QMidi-master/src/QMidiOut.h"
#include <playerwindow.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

// protected: //для работы с клавой
//     void keyPressEvent(QKeyEvent *event) override;
//     void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void on_mute_clicked();
    //void onVolumeChanged(int value);
    void onInstrumentChanged(int index);
    //void playNote(int note);
    //void releaseNote(int note);
    void onOctaveChanged(int octave);

    void on_gotoButton_clicked() {
        playerWindow = new PlayerWindow(this);
        playerWindow->setAttribute(Qt::WA_DeleteOnClose);
        playerWindow->setWindowModality(Qt::ApplicationModal);
        playerWindow->setWindowFlag(Qt::Dialog);
        playerWindow->show();
    }

    void on_volumeBar_valueChanged(int value);

private:
    Ui::MainWindow *ui;
    bool is_mute=0;
    int saved_volume = 50;
    void updateVolume(int volume) {
        if (midiOut && midiOut->isConnected()) {
            // MIDI использует значения 0-127
            int midiVolume = volume * 1.27;
            midiOut->controlChange(0, 7, midiVolume);  // Канал 0, CC7 (Volume)
        }
    }

    QMidiOut* midiOut;
    int currentInstrument = 0; //Acoustic Grand Piano
    int currentOctave = 4;
    int currentVelocity = 64;
    void setupPianoKeys();
    void sendNoteOn(int note);
    void sendNoteOff(int note);
    QMap<int, QTimer*> noteTimers;
    QMap<int, QTimer*> fadeTimers;
    QSet<int> pressedNotes; //сейчас задержаны
    QMap<int, bool> activeNotes; //нажата или нет

    PlayerWindow *playerWindow;

};
#endif // MAINWINDOW_H
