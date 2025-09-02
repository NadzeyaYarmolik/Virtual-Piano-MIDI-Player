#ifndef MAINMENUWIDGET_H
#define MAINMENUWIDGET_H

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

class QPushButton; // Предварительное объявление

class MainMenuWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainMenuWidget(QWidget *parent = nullptr);
    ~MainMenuWidget();

signals:
    void playerButtonClicked();
    void pianoButtonClicked();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateBackground();
    void createBubbles();
    void moveNotes();
    void createBigBubbles();

private:
    QPushButton *playerButton; // Явно объявляем кнопки
    QPushButton *pianoButton;

    QPixmap backgroundImage;
    int imageHeight;
    int scrollPosition = 0;

    QTimer *scrollTimer;
    QTimer *createTimer;
    QTimer *bigBubbleTimer;
    QTimer *moveTimer;

    QList<QLabel*> bubbles;
    QList<int> bubbleSpeeds;
    QList<QLabel*> bigBubbles;
    QList<int> bigBubbleSpeeds;
    void setButtonStyle(QPushButton *button);
};

#endif // MAINMENUWIDGET_H
