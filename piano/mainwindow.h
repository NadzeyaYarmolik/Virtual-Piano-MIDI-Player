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

protected:
    //void keyPressEvent(QKeyEvent *event) override;
    //void paintEvent(QPaintEvent *event) override;
    //void resizeEvent(QResizeEvent *event) override;
    //bool eventFilter(QObject *watched, QEvent *event) override; // Добавляем фильтр событий

private slots:
    // void on_playerButton_clicked();
    // void on_pianoButton_clicked();
    // void updateBackground();
    // void createBubbles();
    // void moveNotes();
    // void createBigBubbles();

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

    //PlayerWindow *playerWindow = nullptr;
    //PianoWindow *pianoWindow = nullptr;

    // QPixmap backgroundImage;
    // QTimer *scrollTimer;
    // int scrollPosition = 0;
    // int imageHeight;

    // QTimer *m_createTimer;
    // QTimer *m_moveTimer;
    // QList<QLabel*> m_bubbles;
    // QList<int> m_bubbleSpeeds;

    // QTimer *m_bigBubbleTimer;
    // QList<QLabel*> m_bigBubbles;
    // QList<int> m_bigBubbleSpeeds;
};

#endif // MAINWINDOW_H
