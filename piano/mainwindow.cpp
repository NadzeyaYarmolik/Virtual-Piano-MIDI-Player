#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helperclass.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(1000, 800); // Используем this->

    // Инициализируем указатели
    playerWindow.reset();
    pianoWindow.reset();

    // Создаем stacked widget с правильным родителем
    stackedWidget = new QStackedWidget(this);
    this->setCentralWidget(stackedWidget);
    stackedWidget->setFixedSize(1000, 800);

    // Создаем главное меню с пузырями
    mainMenu = new MainMenuWidget(this); // Указываем родителя
    stackedWidget->addWidget(mainMenu);

    // Инициализируем MIDI
    midiOut = new QMidiOut();
    QMap<QString, QString> devicesMap = QMidiOut::devices();
    if (!devicesMap.isEmpty()) {
        midiOut->connect(devicesMap.keys().first());
    }

    // Устанавливаем MIDI устройство в HelperClass
    HelperClass::setMidiOut(midiOut);

    // Подключаем сигналы от главного меню - исправленный connect
    connect(mainMenu, &MainMenuWidget::playerButtonClicked,
            this, &MainWindow::showPlayerWindow);
    connect(mainMenu, &MainMenuWidget::pianoButtonClicked,
            this, &MainWindow::showPianoWindow);




}

MainWindow::~MainWindow()
{
    // Очищаем умные указатели (вызовут деструкторы)
    playerWindow.reset();
    pianoWindow.reset();

    // Удаляем главное меню
    if (mainMenu) {
        delete mainMenu;
        mainMenu = nullptr;
    }

    // Удаляем stacked widget
    if (stackedWidget) {
        delete stackedWidget;
        stackedWidget = nullptr;
    }

    // Удаляем MIDI
    if (midiOut) {
        delete midiOut;
        midiOut = nullptr;
    }

    delete ui;
}

void MainWindow::showPlayerWindow()
{
    cleanupCurrentWindow();

    if (!isPlayerWindowActive) {
        PlayerWindow *playerWindow = new PlayerWindow(midiOut, stackedWidget);

        // ПРАВИЛЬНОЕ подключение сигналов
        connect(playerWindow, &PlayerWindow::homeButtonClicked, this, [this, playerWindow]() {
            // Переключаемся на главное меню
            stackedWidget->setCurrentWidget(mainMenu);

            // Устанавливаем флаг
            isPlayerWindowActive = false;

            // Удаляем окно (опционально)
            stackedWidget->removeWidget(playerWindow);
            playerWindow->deleteLater();
        });

        stackedWidget->addWidget(playerWindow);
        stackedWidget->setCurrentWidget(playerWindow);
        isPlayerWindowActive = true;
    }
}

void MainWindow::showPianoWindow()
{
    cleanupCurrentWindow();

    if (!isPianoWindowActive) {
        PianoWindow *pianoWindow = new PianoWindow(midiOut, stackedWidget);

        connect(pianoWindow, &PianoWindow::homeButtonClicked, this, [this, pianoWindow]() {
            stackedWidget->setCurrentWidget(mainMenu);
            isPianoWindowActive = false;
            stackedWidget->removeWidget(pianoWindow);
            pianoWindow->deleteLater();
        });

        stackedWidget->addWidget(pianoWindow);
        stackedWidget->setCurrentWidget(pianoWindow);
        isPianoWindowActive = true;
    }
}

void MainWindow::showMainMenu()
{
    cleanupCurrentWindow();
    stackedWidget->setCurrentWidget(mainMenu);
}

void MainWindow::cleanupCurrentWindow()
{
    QWidget *currentWidget = stackedWidget->currentWidget();
    if (currentWidget && currentWidget != mainMenu) {
        // Не удаляем виджет здесь, просто переключаемся
        // Удаление будет происходить через сигнал destroyed
        stackedWidget->setCurrentWidget(mainMenu);
    }
}
