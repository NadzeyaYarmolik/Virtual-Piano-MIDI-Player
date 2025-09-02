#ifndef CHANNELDIALOG_H
#define CHANNELDIALOG_H

#include <QDialog>

class PlayerWindow;

namespace Ui {
class ChannelDialog;
}

class ChannelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChannelDialog(PlayerWindow* parentPlayer, QWidget *parent = nullptr);
    ~ChannelDialog();

    void setChannel(int channel); //Загрузка + отображение настроек
    void restoreInitialVolume();  //восставовить изначальное значение
    void applyThemeStyle();       //Стиль



private slots:
    void onAccepted();

    void on_muteButton_clicked();
    void on_volumeSlider_valueChanged(int value); //Громкость
    void reject();

private:
    void updateChannelVolume(int volume);

    Ui::ChannelDialog *ui;
    PlayerWindow* m_parentPlayer;// Плеер
    int m_channel=-1;            // Текущий канал
    int m_initialVolume=100;     // значение при открытии диалога если будет cancel
    int m_savedVolume=100;       // сохр значение из плеера
    int m_tempSavedVolume=100;   // последнее ненулевое значение для mute/unmute
    bool m_isMute = 0;           // Текущее состояние mute в диалоге

};

#endif // CHANNELDIALOG_H
