#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H

#include <QWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QMediaPlayer>
#include <QFileInfo>
#include <QTimer>
#include "thirdparty/QMidi-master/src/QMidiFile.h"
#include "thirdparty/QMidi-master/src/QMidiOut.h"

namespace Ui {
class PlayerWindow;
}

class PlayerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerWindow(QWidget *parent = nullptr);
    ~PlayerWindow();

    std::vector<std::shared_ptr<QMidiFile> > Files;
    QStringList Paths;

private slots:
    void on_uploadButton_clicked();

    void on_homeButton_clicked();

    void on_pauseButton_clicked();

    void on_stopButton_clicked();

public slots:
        void refreshTable();

private:
    Ui::PlayerWindow *ui;
    bool is_mute=0;
    int saved_volume = 50;
    void updateVolume(int volume) {
        if (midiOut && midiOut->isConnected()) {
            int midiVolume = volume * 1.27;
            midiOut->controlChange(0, 7, midiVolume);
        }
    }
    bool is_playing=0;
    QMidiOut* midiOut;
    int currentFileIndex = -1; //ничего не выбрано
    int currentPlayPos = 0; // сча играет в мс
    QTimer playTimer; //сча играет
    int totalDur = 0; //длина трека
    void playFile(int row); //выбрать + остановить пред
    void startPlay();
    void pausePlay();
    void stopPlay(bool resetPos); //остановить или нет
    void updatePlay(); //поменять кнопочку
    void updatePlayProgress();
    void updateButtonStates();
    void seekToPos(); //для бегунка
    float durInSec(QMidiFile* midiFile) const;
    QVector<float> Durs;
signals:
    void homeButtonClicked();
};

#endif // PLAYERWINDOW_H
