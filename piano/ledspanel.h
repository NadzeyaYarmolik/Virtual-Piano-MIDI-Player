#ifndef LEDSPANEL_H
#define LEDSPANEL_H

#include <QWidget>
#include <QPushButton>
#include <QVector>
#include <QIcon>
#include <QGridLayout>
#include <QMenu>
#include <QDebug>

class ChannelDialog;
class PlayerWindow;

class LedsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit LedsPanel(PlayerWindow* parentPlayer, QWidget *parent = nullptr);
    ~LedsPanel();

    void updateChannels(const QVector<bool> &activeChannels);//обновить состояние каналов вкл\выкл
    void setStyle(const QString &styleName);//установить стиль
    QString currentStyle() const;//вернуть текущий стиль
    void updateFonts(const QFont &font);//обнова шрифтов

public slots:
    void soloChannel(int channel);//

private:

    QVector<QPushButton*> m_leds;//список лампочек
    QIcon m_onIcon;//иконки лампочек
    QIcon m_offIcon;
    QString m_currentStyle;//стиль текущий
    ChannelDialog *m_channelDialog = nullptr;//диалог
    PlayerWindow* m_parentPlayer;//плеер родитель

    QMap<int, int> m_channelInstruments;//канал+инструмент
    void showChannelContextMenu(int channel, const QPoint &pos);//показать контекст меню

    void resetAllChannels();//сброс настроек всех каналов
private slots:
    void showChannelDialog(int channel);//откр диалог
};

#endif // LEDSPANEL_H
