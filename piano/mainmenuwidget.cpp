#include "mainmenuwidget.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

MainMenuWidget::MainMenuWidget(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(1000, 800);

    // Загружаем фон
    backgroundImage.load(":/img/mainbackground.png");
    imageHeight = backgroundImage.height();

    // Создаем кнопки

    playerButton = new QPushButton( this);
    pianoButton = new QPushButton( this);
    setButtonStyle(playerButton);
    setButtonStyle(pianoButton);
    pianoButton->setIcon(QIcon(QString(":/img/pianobutton.png")));
    playerButton->setIcon(QIcon(QString(":/img/playerbutton.png")));

    // Устанавливаем фиксированный размер кнопок
    //playerButton->setFixedSize(400, 200);
    //pianoButton->setFixedSize(400, 200);

    // Создаем вертикальный layout и добавляем кнопки одна под другой
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addStretch();
    mainLayout->addWidget(playerButton, 0, Qt::AlignCenter);
    mainLayout->addSpacing(0);
    mainLayout->addWidget(pianoButton, 0, Qt::AlignCenter);
    //mainLayout->addStretch();

    // УДАЛЯЕМ весь код с QHBoxLayout - он нам не нужен!
    // Размещаем кнопки по центру
    // QHBoxLayout *buttonLayout = new QHBoxLayout();
    // buttonLayout->addWidget(playerButton);
    // buttonLayout->addWidget(pianoButton);
    // buttonLayout->setAlignment(Qt::AlignCenter);

    // ПОДКЛЮЧАЕМ ВРУЧНУЮ - без автоматических имен
    connect(playerButton, &QPushButton::clicked, this, [this]() {
        emit playerButtonClicked();
    });

    connect(pianoButton, &QPushButton::clicked, this, [this]() {
        emit pianoButtonClicked();
    });

    // Настраиваем таймеры...
    scrollTimer = new QTimer(this);
    connect(scrollTimer, &QTimer::timeout, this, &MainMenuWidget::updateBackground);
    scrollTimer->start(60);

    createTimer = new QTimer(this);
    connect(createTimer, &QTimer::timeout, this, &MainMenuWidget::createBubbles);
    createTimer->start(1200);

    bigBubbleTimer = new QTimer(this);
    connect(bigBubbleTimer, &QTimer::timeout, this, &MainMenuWidget::createBigBubbles);
    bigBubbleTimer->start(3000);

    moveTimer = new QTimer(this);
    connect(moveTimer, &QTimer::timeout, this, &MainMenuWidget::moveNotes);
    moveTimer->start(16);
}

MainMenuWidget::~MainMenuWidget()
{
    // Очищаем пузыри
    for (QLabel *bubble : bubbles) {
        delete bubble;
    }
    bubbles.clear();

    for (QLabel *bigBubble : bigBubbles) {
        delete bigBubble;
    }
    bigBubbles.clear();
}

void MainMenuWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    int y1 = scrollPosition;
    int y2 = y1 - imageHeight;

    painter.drawPixmap(0, y1, width(), imageHeight, backgroundImage);
    painter.drawPixmap(0, y2, width(), imageHeight, backgroundImage);
}

// Остальные методы (updateBackground, createBubbles, moveNotes, createBigBubbles)
// копируем из MainMenuWidget.cpp без изменений

void MainMenuWidget::updateBackground()
{
    scrollPosition = (scrollPosition + 30) % imageHeight;
    update();
}

void MainMenuWidget::createBubbles()
{

    // Случайное количество пузырьков в группе (1-5)
    int bubbleCount = QRandomGenerator::global()->bounded(1, 6);

    for (int i = 0; i < bubbleCount; ++i) {
        // Случайный размер от 8 до 30 пикселей (еще меньше)
        int size = QRandomGenerator::global()->bounded(8, 31);

        // Случайная позиция X по всей горизонтали (0 до 1000)
        int xPos = QRandomGenerator::global()->bounded(0, 1001 - size);

        // Случайная скорость взлета (1-3 пикселя за кадр)
        int speed = QRandomGenerator::global()->bounded(1, 4);

        // Создаем прозрачный кружочек
        QLabel *bubble = new QLabel(this);
        bubble->setFixedSize(size, size);
        bubble->setAttribute(Qt::WA_TransparentForMouseEvents);

        // Создаем прозрачный кружок без контура
        QPixmap bubblePixmap(size, size);
        bubblePixmap.fill(Qt::transparent);

        QPainter painter(&bubblePixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(Qt::NoPen); // Убираем контур
        painter.setBrush(QBrush(QColor(255, 255, 255, 60))); // Очень прозрачный белый (альфа 60)
        painter.drawEllipse(0, 0, size, size);
        painter.end();

        bubble->setPixmap(bubblePixmap);
        // Начинаем снизу экрана (взлетаем вверх)
        bubble->setGeometry(xPos, 800, size, size);
        bubble->show();

        // Сохраняем пузырек и его скорость (отрицательная для движения вверх)
        bubbles.append(bubble);
        bubbleSpeeds.append(-speed); // Отрицательная скорость для движения вверх
    }
}

void MainMenuWidget::moveNotes()
{
    for (int i = bubbles.size() - 1; i >= 0; i--) {
        QLabel *bubble = bubbles[i];
        int speed = bubbleSpeeds[i];

        bubble->move(bubble->x(), bubble->y() + speed);

        // Если пузырек ушел выше экрана - удаляем
        if (bubble->y() + bubble->height() < 0) {
            delete bubble;
            bubbles.removeAt(i);
            bubbleSpeeds.removeAt(i);
        }
    }

    // Двигаем большие пузыри
    for (int i = bigBubbles.size() - 1; i >= 0; i--) {
        QLabel *bigBubble = bigBubbles[i];
        int speed = bigBubbleSpeeds[i];

        bigBubble->move(bigBubble->x(), bigBubble->y() + speed);

        // Если большой пузырь ушел выше экрана - удаляем
        if (bigBubble->y() + bigBubble->height() < 0) {
            delete bigBubble;
            bigBubbles.removeAt(i);
            bigBubbleSpeeds.removeAt(i);
        }
    }
}

void MainMenuWidget::createBigBubbles()
{
    // Случайное количество больших пузырей (1-3)
    int bubbleCount = QRandomGenerator::global()->bounded(1, 4);

    for (int i = 0; i < bubbleCount; ++i) {
        // Случайный размер от 300 до 500 пикселей
        int size = QRandomGenerator::global()->bounded(300, 501);

        // Случайная позиция X
        int xPos = QRandomGenerator::global()->bounded(-100, 900);

        // Случайный номер изображения пузыря (1-3)
        int bubbleType = QRandomGenerator::global()->bounded(1, 4);
        QString imagePath = QString(":/img/bubble%1.png").arg(bubbleType);

        // Случайная скорость взлета
        int speed = QRandomGenerator::global()->bounded(2, 6);

        // Создаем пузырь
        QLabel *bigBubble = new QLabel(this);
        bigBubble->setFixedSize(size, size);
        bigBubble->setAttribute(Qt::WA_TransparentForMouseEvents);

        // Загружаем изображение и устанавливаем прозрачность
        QPixmap originalPixmap(imagePath);
        if (!originalPixmap.isNull()) {
            // Масштабируем изображение
            QPixmap scaledPixmap = originalPixmap.scaled(size, size,
                                                         Qt::KeepAspectRatio, Qt::SmoothTransformation);

            // Создаем прозрачную версию изображения
            QPixmap transparentPixmap(scaledPixmap.size());
            transparentPixmap.fill(Qt::transparent);

            QPainter painter(&transparentPixmap);
            painter.setOpacity(0.6); // Устанавливаем прозрачность alpha / 255.0
            painter.drawPixmap(0, 0, scaledPixmap);
            painter.end();

            bigBubble->setPixmap(transparentPixmap);
        } else {
            // Fallback - простой полупрозрачный круг
            QPixmap fallbackPixmap(size, size);
            fallbackPixmap.fill(Qt::transparent);
            QPainter painter(&fallbackPixmap);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setPen(Qt::NoPen);
            painter.setBrush(QBrush(QColor(255, 255, 255, 0.6)));
            painter.drawEllipse(0, 0, size, size);
            painter.end();
            bigBubble->setPixmap(fallbackPixmap);
        }

        bigBubble->setGeometry(xPos, 800, size, size);
        bigBubble->show();

        bigBubbles.append(bigBubble);
        bigBubbleSpeeds.append(-speed);
    }

    bigBubbleTimer->start(QRandomGenerator::global()->bounded(5000, 10001));
}

void MainMenuWidget::setButtonStyle(QPushButton *button){
    QString buttonStyle =
        "QPushButton {"
        "    background-color: transparent;"  // Полупрозрачный белый фонrgba(255, 255, 255, 150)
        "border: none;"
        "}";
    button->setIconSize(QSize(400, 400));  // Уменьшаем размер иконки
    button->setStyleSheet(buttonStyle);
}
