/********************************************************************************
** Form generated from reading UI file 'playerwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLAYERWINDOW_H
#define UI_PLAYERWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PlayerWindow
{
public:
    QPushButton *homeButton;
    QPushButton *uploadButton;
    QPushButton *infoButton;
    QPushButton *prevButton;
    QPushButton *stopButton;
    QPushButton *pauseButton;
    QPushButton *nextButton;
    QTableWidget *Table;
    QPushButton *muteButton;
    QSlider *volumeSlider;
    QLabel *vinylLabel;
    QSlider *progressSlider;

    void setupUi(QWidget *PlayerWindow)
    {
        if (PlayerWindow->objectName().isEmpty())
            PlayerWindow->setObjectName("PlayerWindow");
        PlayerWindow->resize(527, 295);
        homeButton = new QPushButton(PlayerWindow);
        homeButton->setObjectName("homeButton");
        homeButton->setGeometry(QRect(10, 10, 41, 41));
        homeButton->setStyleSheet(QString::fromUtf8("background: transparent;\n"
"        border: none;\n"
"        padding: 0px;"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/img/home.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        homeButton->setIcon(icon);
        homeButton->setIconSize(QSize(40, 40));
        uploadButton = new QPushButton(PlayerWindow);
        uploadButton->setObjectName("uploadButton");
        uploadButton->setGeometry(QRect(140, 190, 41, 41));
        uploadButton->setStyleSheet(QString::fromUtf8("background: transparent;\n"
"        border: none;\n"
"        padding: 0px;"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/img/upload.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        uploadButton->setIcon(icon1);
        uploadButton->setIconSize(QSize(40, 40));
        infoButton = new QPushButton(PlayerWindow);
        infoButton->setObjectName("infoButton");
        infoButton->setGeometry(QRect(190, 10, 41, 41));
        infoButton->setStyleSheet(QString::fromUtf8("background: transparent;\n"
"        border: none;\n"
"        padding: 0px;"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/img/info.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        infoButton->setIcon(icon2);
        infoButton->setIconSize(QSize(40, 40));
        prevButton = new QPushButton(PlayerWindow);
        prevButton->setObjectName("prevButton");
        prevButton->setGeometry(QRect(20, 190, 41, 41));
        prevButton->setStyleSheet(QString::fromUtf8("background: transparent;\n"
"        border: none;\n"
"        padding: 0px;"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/img/previous.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        prevButton->setIcon(icon3);
        prevButton->setIconSize(QSize(40, 40));
        stopButton = new QPushButton(PlayerWindow);
        stopButton->setObjectName("stopButton");
        stopButton->setGeometry(QRect(60, 190, 41, 41));
        stopButton->setStyleSheet(QString::fromUtf8("background: transparent;\n"
"        border: none;\n"
"        padding: 0px;"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/img/start.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        stopButton->setIcon(icon4);
        stopButton->setIconSize(QSize(40, 40));
        pauseButton = new QPushButton(PlayerWindow);
        pauseButton->setObjectName("pauseButton");
        pauseButton->setGeometry(QRect(100, 190, 41, 41));
        pauseButton->setStyleSheet(QString::fromUtf8("background: transparent;\n"
"        border: none;\n"
"        padding: 0px;"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/img/play.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        pauseButton->setIcon(icon5);
        pauseButton->setIconSize(QSize(40, 40));
        nextButton = new QPushButton(PlayerWindow);
        nextButton->setObjectName("nextButton");
        nextButton->setGeometry(QRect(180, 190, 41, 41));
        nextButton->setStyleSheet(QString::fromUtf8("background: transparent;\n"
"        border: none;\n"
"        padding: 0px;"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/img/next.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        nextButton->setIcon(icon6);
        nextButton->setIconSize(QSize(40, 40));
        Table = new QTableWidget(PlayerWindow);
        Table->setObjectName("Table");
        Table->setGeometry(QRect(240, 10, 271, 271));
        Table->setStyleSheet(QString::fromUtf8("QTableWidget {\n"
"       background-color: transparent;\n"
"       color: white;\n"
"       gridline-color: rgba(255, 255, 255, 50);\n"
"    }\n"
"    QHeaderView::section {\n"
"       background-color: transparent;\n"
"       color: white;\n"
"       border: 1px solid rgba(255, 255, 255, 50);\n"
"    }\n"
"    QTableCornerButton::section {\n"
"       background-color: transparent;\n"
"       border: 1px solid rgba(255, 255, 255, 50);\n"
"    }"));
        muteButton = new QPushButton(PlayerWindow);
        muteButton->setObjectName("muteButton");
        muteButton->setGeometry(QRect(20, 240, 41, 41));
        muteButton->setStyleSheet(QString::fromUtf8("background: transparent;\n"
"        border: none;\n"
"        padding: 0px;"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/img/volume.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        muteButton->setIcon(icon7);
        muteButton->setIconSize(QSize(40, 40));
        volumeSlider = new QSlider(PlayerWindow);
        volumeSlider->setObjectName("volumeSlider");
        volumeSlider->setGeometry(QRect(70, 250, 141, 18));
        volumeSlider->setOrientation(Qt::Orientation::Horizontal);
        vinylLabel = new QLabel(PlayerWindow);
        vinylLabel->setObjectName("vinylLabel");
        vinylLabel->setGeometry(QRect(60, 30, 121, 121));
        vinylLabel->setPixmap(QPixmap(QString::fromUtf8(":/img/vinyl.ico")));
        vinylLabel->setScaledContents(true);
        progressSlider = new QSlider(PlayerWindow);
        progressSlider->setObjectName("progressSlider");
        progressSlider->setGeometry(QRect(20, 160, 201, 18));
        progressSlider->setOrientation(Qt::Orientation::Horizontal);

        retranslateUi(PlayerWindow);

        QMetaObject::connectSlotsByName(PlayerWindow);
    } // setupUi

    void retranslateUi(QWidget *PlayerWindow)
    {
        PlayerWindow->setWindowTitle(QCoreApplication::translate("PlayerWindow", "Form", nullptr));
        homeButton->setText(QString());
        uploadButton->setText(QString());
        infoButton->setText(QString());
        prevButton->setText(QString());
        stopButton->setText(QString());
        pauseButton->setText(QString());
        nextButton->setText(QString());
        muteButton->setText(QString());
        vinylLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class PlayerWindow: public Ui_PlayerWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLAYERWINDOW_H
