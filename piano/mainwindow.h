#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPropertyAnimation>
#include <QStyle>
#include <QComboBox>
#include <QSoundEffect>
#include <QPushButton>
#include <QDial>
#include <QTimer>
#include <QPainter>
#include <QLabel>
#include <QRandomGenerator>
#include <QGraphicsView>
#include <QPropertyAnimation>
#include <QStackedWidget>
#include <QEasingCurve>
#include <QGraphicsOpacityEffect>
#include <QDebug>
#include <QGraphicsItemAnimation>
#include <QTimeLine>
#include "thirdparty/QMidi-master/src/QMidiOut.h"
#include "playerwindow.h"
#include "pianowindow.h"
#include "mainmenuwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainMenuWidget;
class PlayerWindow;
class PianoWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showMainMenu();
    void showPlayerWindow();
    void showPianoWindow();

private:
    Ui::MainWindow *ui;
    QMidiOut *midiOut = nullptr;
    QStackedWidget *stackedWidget = nullptr;
    QWidget *mainMenuWidget;
    bool m_isMainMenuActive = true;
    MainMenuWidget *mainMenu = nullptr;

    void cleanupCurrentWindow();

    QSharedPointer<PlayerWindow> playerWindow;
    QSharedPointer<PianoWindow> pianoWindow;

    bool isPlayerWindowActive=0;
    bool isPianoWindowActive=0;
};

#endif // MAINWINDOW_H
