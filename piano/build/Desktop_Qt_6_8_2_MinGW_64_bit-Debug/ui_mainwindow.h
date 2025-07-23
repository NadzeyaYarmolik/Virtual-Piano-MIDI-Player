/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QSlider *volumeBar;
    QPushButton *mute;
    QWidget *horizontalLayoutWidget_2;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *c4;
    QPushButton *d4;
    QPushButton *e4;
    QPushButton *f4;
    QPushButton *g4;
    QPushButton *a4;
    QPushButton *b4;
    QPushButton *c5;
    QWidget *horizontalLayoutWidget_3;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *c_4;
    QPushButton *d_4;
    QWidget *horizontalLayoutWidget_4;
    QHBoxLayout *horizontalLayout_6;
    QPushButton *f_4;
    QPushButton *g_4;
    QPushButton *a_4;
    QWidget *horizontalLayoutWidget_5;
    QHBoxLayout *horizontalLayout_7;
    QComboBox *instrumentBox;
    QSpinBox *octaveBox;
    QPushButton *gotoButton;
    QPushButton *infoButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(599, 437);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayoutWidget = new QWidget(centralwidget);
        verticalLayoutWidget->setObjectName("verticalLayoutWidget");
        verticalLayoutWidget->setGeometry(QRect(20, 30, 41, 191));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        volumeBar = new QSlider(verticalLayoutWidget);
        volumeBar->setObjectName("volumeBar");
        volumeBar->setMaximum(100);
        volumeBar->setValue(50);
        volumeBar->setSliderPosition(50);
        volumeBar->setOrientation(Qt::Orientation::Vertical);

        horizontalLayout_2->addWidget(volumeBar);


        verticalLayout->addLayout(horizontalLayout_2);

        mute = new QPushButton(verticalLayoutWidget);
        mute->setObjectName("mute");
        mute->setStyleSheet(QString::fromUtf8("background: transparent;\n"
"        border: none;\n"
"        padding: 0px;"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/img/volume_.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        mute->setIcon(icon);
        mute->setIconSize(QSize(35, 35));

        verticalLayout->addWidget(mute);

        horizontalLayoutWidget_2 = new QWidget(centralwidget);
        horizontalLayoutWidget_2->setObjectName("horizontalLayoutWidget_2");
        horizontalLayoutWidget_2->setGeometry(QRect(90, 220, 411, 152));
        horizontalLayout_4 = new QHBoxLayout(horizontalLayoutWidget_2);
        horizontalLayout_4->setSpacing(0);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        c4 = new QPushButton(horizontalLayoutWidget_2);
        c4->setObjectName("c4");
        c4->setMinimumSize(QSize(50, 150));
        c4->setMaximumSize(QSize(50, 150));
        c4->setStyleSheet(QString::fromUtf8("#c4{\n"
" border: 2px solid #e2e2e2;\n"
"        border-radius: 8px;\n"
"        padding: 8px;\n"
"        color: white;\n"
"        font-size: 14px;\n"
"background: qlineargradient(spread:pad, x1:0.51, y1:0, x2:0.533, y2:1, stop:0 rgba(180, 180, 180, 255), stop:1 rgba(226, 226, 226, 255));\n"
"}\n"
"#c4:pressed{\n"
"background-color: rgb(180, 180, 180);\n"
"        border: 2px solid #e2e2e2\n"
"}"));
        c4->setIconSize(QSize(200, 150));
        c4->setAutoRepeat(true);

        horizontalLayout_4->addWidget(c4);

        d4 = new QPushButton(horizontalLayoutWidget_2);
        d4->setObjectName("d4");
        d4->setMinimumSize(QSize(50, 150));
        d4->setMaximumSize(QSize(50, 150));
        d4->setStyleSheet(QString::fromUtf8("#d4{\n"
" border: 2px solid #e2e2e2;\n"
"        border-radius: 8px;\n"
"        padding: 8px;\n"
"        color: white;\n"
"        font-size: 14px;\n"
"background: qlineargradient(spread:pad, x1:0.51, y1:0, x2:0.533, y2:1, stop:0 rgba(180, 180, 180, 255), stop:1 rgba(226, 226, 226, 255));\n"
"}\n"
"#d4:pressed{\n"
"background-color: rgb(180, 180, 180);\n"
"        border: 2px solid #e2e2e2\n"
"}"));
        d4->setIconSize(QSize(200, 150));
        d4->setAutoRepeat(true);

        horizontalLayout_4->addWidget(d4);

        e4 = new QPushButton(horizontalLayoutWidget_2);
        e4->setObjectName("e4");
        e4->setMinimumSize(QSize(50, 150));
        e4->setMaximumSize(QSize(50, 150));
        e4->setStyleSheet(QString::fromUtf8("#e4{\n"
"        border: 2px solid #e2e2e2;\n"
"        border-radius: 8px;\n"
"        padding: 8px;\n"
"        color: white;\n"
"        font-size: 14px;\n"
"background: qlineargradient(spread:pad, x1:0.51, y1:0, x2:0.533, y2:1, stop:0 rgba(180, 180, 180, 255), stop:1 rgba(226, 226, 226, 255));\n"
"}\n"
"#e4:pressed{\n"
"background-color: rgb(180, 180, 180);\n"
"        border: 2px solid #e2e2e2\n"
"}"));
        e4->setIconSize(QSize(200, 150));
        e4->setAutoRepeat(true);

        horizontalLayout_4->addWidget(e4);

        f4 = new QPushButton(horizontalLayoutWidget_2);
        f4->setObjectName("f4");
        f4->setMinimumSize(QSize(50, 150));
        f4->setMaximumSize(QSize(50, 150));
        f4->setStyleSheet(QString::fromUtf8("#f4{\n"
"background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                  stop:0 #8e2de2, stop:1 #4a00e0);\n"
"        border: 2px solid #e2e2e2;\n"
"        border-radius: 8px;\n"
"        padding: 8px;\n"
"        color: white;\n"
"        font-size: 14px;\n"
"background: qlineargradient(spread:pad, x1:0.51, y1:0, x2:0.533, y2:1, stop:0 rgba(180, 180, 180, 255), stop:1 rgba(226, 226, 226, 255));\n"
"}\n"
"#f4:pressed{\n"
"background-color: rgb(180, 180, 180);\n"
"        border: 2px solid #e2e2e2\n"
"}"));
        f4->setIconSize(QSize(200, 150));
        f4->setAutoRepeat(true);

        horizontalLayout_4->addWidget(f4);

        g4 = new QPushButton(horizontalLayoutWidget_2);
        g4->setObjectName("g4");
        g4->setMinimumSize(QSize(50, 150));
        g4->setMaximumSize(QSize(50, 150));
        g4->setStyleSheet(QString::fromUtf8("#g4{\n"
" border: 2px solid #e2e2e2;\n"
"        border-radius: 8px;\n"
"        padding: 8px;\n"
"        color: white;\n"
"        font-size: 14px;\n"
"background: qlineargradient(spread:pad, x1:0.51, y1:0, x2:0.533, y2:1, stop:0 rgba(180, 180, 180, 255), stop:1 rgba(226, 226, 226, 255));\n"
"}\n"
"#g4:pressed{\n"
"background-color: rgb(180, 180, 180);\n"
"        border: 2px solid #e2e2e2\n"
"}"));
        g4->setIconSize(QSize(200, 150));
        g4->setAutoRepeat(true);

        horizontalLayout_4->addWidget(g4);

        a4 = new QPushButton(horizontalLayoutWidget_2);
        a4->setObjectName("a4");
        a4->setMinimumSize(QSize(50, 150));
        a4->setMaximumSize(QSize(50, 150));
        a4->setStyleSheet(QString::fromUtf8("#a4{\n"
" border: 2px solid #e2e2e2;\n"
"        border-radius: 8px;\n"
"        padding: 8px;\n"
"        color: white;\n"
"        font-size: 14px;\n"
"background: qlineargradient(spread:pad, x1:0.51, y1:0, x2:0.533, y2:1, stop:0 rgba(180, 180, 180, 255), stop:1 rgba(226, 226, 226, 255));\n"
"}\n"
"#a4:pressed{\n"
"background-color: rgb(180, 180, 180);\n"
"        border: 2px solid #e2e2e2\n"
"}"));
        a4->setIconSize(QSize(200, 150));
        a4->setAutoRepeat(true);

        horizontalLayout_4->addWidget(a4);

        b4 = new QPushButton(horizontalLayoutWidget_2);
        b4->setObjectName("b4");
        b4->setMinimumSize(QSize(50, 150));
        b4->setMaximumSize(QSize(50, 150));
        b4->setStyleSheet(QString::fromUtf8("#b4{\n"
" border: 2px solid #e2e2e2;\n"
"        border-radius: 8px;\n"
"        padding: 8px;\n"
"        color: white;\n"
"        font-size: 14px;\n"
"background: qlineargradient(spread:pad, x1:0.51, y1:0, x2:0.533, y2:1, stop:0 rgba(180, 180, 180, 255), stop:1 rgba(226, 226, 226, 255));\n"
"}\n"
"#b4:pressed{\n"
"background-color: rgb(180, 180, 180);\n"
"        border: 2px solid #e2e2e2\n"
"}"));
        b4->setIconSize(QSize(200, 150));
        b4->setAutoRepeat(true);

        horizontalLayout_4->addWidget(b4);

        c5 = new QPushButton(horizontalLayoutWidget_2);
        c5->setObjectName("c5");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(c5->sizePolicy().hasHeightForWidth());
        c5->setSizePolicy(sizePolicy);
        c5->setMinimumSize(QSize(50, 150));
        c5->setMaximumSize(QSize(50, 150));
        c5->setStyleSheet(QString::fromUtf8("#c5{\n"
" border: 2px solid #e2e2e2;\n"
"        border-radius: 8px;\n"
"        padding: 8px;\n"
"        color: white;\n"
"        font-size: 14px;\n"
"background: qlineargradient(spread:pad, x1:0.51, y1:0, x2:0.533, y2:1, stop:0 rgba(180, 180, 180, 255), stop:1 rgba(226, 226, 226, 255));\n"
"}\n"
"#c5:pressed{\n"
"background-color: rgb(180, 180, 180);\n"
"        border: 2px solid #e2e2e2\n"
"}"));
        c5->setIconSize(QSize(200, 150));
        c5->setAutoRepeat(true);

        horizontalLayout_4->addWidget(c5);

        horizontalLayoutWidget_3 = new QWidget(centralwidget);
        horizontalLayoutWidget_3->setObjectName("horizontalLayoutWidget_3");
        horizontalLayoutWidget_3->setGeometry(QRect(110, 220, 111, 102));
        horizontalLayout_5 = new QHBoxLayout(horizontalLayoutWidget_3);
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        c_4 = new QPushButton(horizontalLayoutWidget_3);
        c_4->setObjectName("c_4");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(c_4->sizePolicy().hasHeightForWidth());
        c_4->setSizePolicy(sizePolicy1);
        c_4->setMinimumSize(QSize(30, 100));
        c_4->setMaximumSize(QSize(30, 100));
        c_4->setStyleSheet(QString::fromUtf8("#c_4{\n"
"        border: 2px solid #121212;\n"
"        border-radius: 8px;\n"
"        padding: 8px;\n"
"        color: white;\n"
"        font-size: 14px;\n"
"background: qlineargradient(spread:pad, x1:0.51, y1:0, x2:0.533, y2:1, stop:0 rgba(12, 12, 12, 255), stop:1 rgba(100, 100, 100, 255));\n"
"}\n"
"#c_4:pressed{\n"
"background-color: rgb(0, 0, 0);\n"
"        border: 2px solid #000000\n"
"}"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/bkey.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        c_4->setIcon(icon1);
        c_4->setIconSize(QSize(50, 99));

        horizontalLayout_5->addWidget(c_4);

        d_4 = new QPushButton(horizontalLayoutWidget_3);
        d_4->setObjectName("d_4");
        sizePolicy1.setHeightForWidth(d_4->sizePolicy().hasHeightForWidth());
        d_4->setSizePolicy(sizePolicy1);
        d_4->setMinimumSize(QSize(30, 100));
        d_4->setMaximumSize(QSize(30, 100));
        d_4->setStyleSheet(QString::fromUtf8("#d_4{\n"
"        border: 2px solid #121212;\n"
"        border-radius: 8px;\n"
"        padding: 8px;\n"
"        color: white;\n"
"        font-size: 14px;\n"
"background: qlineargradient(spread:pad, x1:0.51, y1:0, x2:0.533, y2:1, stop:0 rgba(12, 12, 12, 255), stop:1 rgba(100, 100, 100, 255));\n"
"}\n"
"#d_4:pressed{\n"
"background-color: rgb(0, 0, 0);\n"
"        border: 2px solid #000000\n"
"}"));
        d_4->setIcon(icon1);
        d_4->setIconSize(QSize(50, 99));

        horizontalLayout_5->addWidget(d_4);

        horizontalLayoutWidget_4 = new QWidget(centralwidget);
        horizontalLayoutWidget_4->setObjectName("horizontalLayoutWidget_4");
        horizontalLayoutWidget_4->setGeometry(QRect(270, 220, 160, 102));
        horizontalLayout_6 = new QHBoxLayout(horizontalLayoutWidget_4);
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        horizontalLayout_6->setContentsMargins(0, 0, 0, 0);
        f_4 = new QPushButton(horizontalLayoutWidget_4);
        f_4->setObjectName("f_4");
        sizePolicy1.setHeightForWidth(f_4->sizePolicy().hasHeightForWidth());
        f_4->setSizePolicy(sizePolicy1);
        f_4->setMinimumSize(QSize(30, 100));
        f_4->setMaximumSize(QSize(30, 100));
        f_4->setStyleSheet(QString::fromUtf8("#f_4{\n"
"        border: 2px solid #121212;\n"
"        border-radius: 8px;\n"
"        padding: 8px;\n"
"        color: white;\n"
"        font-size: 14px;\n"
"background: qlineargradient(spread:pad, x1:0.51, y1:0, x2:0.533, y2:1, stop:0 rgba(12, 12, 12, 255), stop:1 rgba(100, 100, 100, 255));\n"
"}\n"
"#f_4:pressed{\n"
"background-color: rgb(0, 0, 0);\n"
"        border: 2px solid #000000\n"
"}"));
        f_4->setIcon(icon1);
        f_4->setIconSize(QSize(50, 99));

        horizontalLayout_6->addWidget(f_4);

        g_4 = new QPushButton(horizontalLayoutWidget_4);
        g_4->setObjectName("g_4");
        sizePolicy1.setHeightForWidth(g_4->sizePolicy().hasHeightForWidth());
        g_4->setSizePolicy(sizePolicy1);
        g_4->setMinimumSize(QSize(30, 100));
        g_4->setMaximumSize(QSize(30, 100));
        g_4->setStyleSheet(QString::fromUtf8("#g_4{\n"
"        border: 2px solid #121212;\n"
"        border-radius: 8px;\n"
"        padding: 8px;\n"
"        color: white;\n"
"        font-size: 14px;\n"
"background: qlineargradient(spread:pad, x1:0.51, y1:0, x2:0.533, y2:1, stop:0 rgba(12, 12, 12, 255), stop:1 rgba(100, 100, 100, 255));\n"
"}\n"
"#g_4:pressed{\n"
"background-color: rgb(0, 0, 0);\n"
"        border: 2px solid #000000\n"
"}"));
        g_4->setIcon(icon1);
        g_4->setIconSize(QSize(50, 99));
        g_4->setAutoRepeat(true);

        horizontalLayout_6->addWidget(g_4);

        a_4 = new QPushButton(horizontalLayoutWidget_4);
        a_4->setObjectName("a_4");
        sizePolicy1.setHeightForWidth(a_4->sizePolicy().hasHeightForWidth());
        a_4->setSizePolicy(sizePolicy1);
        a_4->setMinimumSize(QSize(30, 100));
        a_4->setMaximumSize(QSize(30, 100));
        a_4->setStyleSheet(QString::fromUtf8("#a_4{\n"
"        border: 2px solid #121212;\n"
"        border-radius: 8px;\n"
"        padding: 8px;\n"
"        color: white;\n"
"        font-size: 14px;\n"
"background: qlineargradient(spread:pad, x1:0.51, y1:0, x2:0.533, y2:1, stop:0 rgba(12, 12, 12, 255), stop:1 rgba(100, 100, 100, 255));\n"
"}\n"
"#a_4:pressed{\n"
"background-color: rgb(0, 0, 0);\n"
"        border: 2px solid #000000\n"
"}"));
        a_4->setIcon(icon1);
        a_4->setIconSize(QSize(50, 99));
        a_4->setAutoRepeat(true);

        horizontalLayout_6->addWidget(a_4);

        horizontalLayoutWidget_5 = new QWidget(centralwidget);
        horizontalLayoutWidget_5->setObjectName("horizontalLayoutWidget_5");
        horizontalLayoutWidget_5->setGeometry(QRect(170, 170, 251, 51));
        horizontalLayout_7 = new QHBoxLayout(horizontalLayoutWidget_5);
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        horizontalLayout_7->setContentsMargins(0, 0, 0, 0);
        instrumentBox = new QComboBox(horizontalLayoutWidget_5);
        instrumentBox->setObjectName("instrumentBox");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(instrumentBox->sizePolicy().hasHeightForWidth());
        instrumentBox->setSizePolicy(sizePolicy2);

        horizontalLayout_7->addWidget(instrumentBox);

        octaveBox = new QSpinBox(horizontalLayoutWidget_5);
        octaveBox->setObjectName("octaveBox");
        octaveBox->setEnabled(true);
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(octaveBox->sizePolicy().hasHeightForWidth());
        octaveBox->setSizePolicy(sizePolicy3);
        octaveBox->setFocusPolicy(Qt::FocusPolicy::NoFocus);
        octaveBox->setReadOnly(false);
        octaveBox->setKeyboardTracking(false);
        octaveBox->setMinimum(2);
        octaveBox->setMaximum(5);
        octaveBox->setValue(4);

        horizontalLayout_7->addWidget(octaveBox);

        gotoButton = new QPushButton(centralwidget);
        gotoButton->setObjectName("gotoButton");
        gotoButton->setGeometry(QRect(540, 20, 41, 41));
        gotoButton->setStyleSheet(QString::fromUtf8("background: transparent;\n"
"        border: none;\n"
"        padding: 0px; "));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/img/goto_.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        gotoButton->setIcon(icon2);
        gotoButton->setIconSize(QSize(35, 35));
        infoButton = new QPushButton(centralwidget);
        infoButton->setObjectName("infoButton");
        infoButton->setGeometry(QRect(540, 70, 41, 41));
        infoButton->setStyleSheet(QString::fromUtf8("background: transparent;\n"
"        border: none;\n"
"        padding: 0px;"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/img/info_.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        infoButton->setIcon(icon3);
        infoButton->setIconSize(QSize(35, 35));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 599, 25));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        mute->setText(QString());
        c4->setText(QString());
#if QT_CONFIG(shortcut)
        c4->setShortcut(QCoreApplication::translate("MainWindow", "Q", nullptr));
#endif // QT_CONFIG(shortcut)
        d4->setText(QString());
#if QT_CONFIG(shortcut)
        d4->setShortcut(QCoreApplication::translate("MainWindow", "W", nullptr));
#endif // QT_CONFIG(shortcut)
        e4->setText(QString());
#if QT_CONFIG(shortcut)
        e4->setShortcut(QCoreApplication::translate("MainWindow", "E", nullptr));
#endif // QT_CONFIG(shortcut)
        f4->setText(QString());
#if QT_CONFIG(shortcut)
        f4->setShortcut(QCoreApplication::translate("MainWindow", "R", nullptr));
#endif // QT_CONFIG(shortcut)
        g4->setText(QString());
#if QT_CONFIG(shortcut)
        g4->setShortcut(QCoreApplication::translate("MainWindow", "T", nullptr));
#endif // QT_CONFIG(shortcut)
        a4->setText(QString());
#if QT_CONFIG(shortcut)
        a4->setShortcut(QCoreApplication::translate("MainWindow", "Y", nullptr));
#endif // QT_CONFIG(shortcut)
        b4->setText(QString());
#if QT_CONFIG(shortcut)
        b4->setShortcut(QCoreApplication::translate("MainWindow", "U", nullptr));
#endif // QT_CONFIG(shortcut)
        c5->setText(QString());
#if QT_CONFIG(shortcut)
        c5->setShortcut(QCoreApplication::translate("MainWindow", "I", nullptr));
#endif // QT_CONFIG(shortcut)
        c_4->setText(QString());
#if QT_CONFIG(shortcut)
        c_4->setShortcut(QCoreApplication::translate("MainWindow", "2", nullptr));
#endif // QT_CONFIG(shortcut)
        d_4->setText(QString());
#if QT_CONFIG(shortcut)
        d_4->setShortcut(QCoreApplication::translate("MainWindow", "3", nullptr));
#endif // QT_CONFIG(shortcut)
        f_4->setText(QString());
#if QT_CONFIG(shortcut)
        f_4->setShortcut(QCoreApplication::translate("MainWindow", "5", nullptr));
#endif // QT_CONFIG(shortcut)
        g_4->setText(QString());
#if QT_CONFIG(shortcut)
        g_4->setShortcut(QCoreApplication::translate("MainWindow", "6", nullptr));
#endif // QT_CONFIG(shortcut)
        a_4->setText(QString());
#if QT_CONFIG(shortcut)
        a_4->setShortcut(QCoreApplication::translate("MainWindow", "7", nullptr));
#endif // QT_CONFIG(shortcut)
        gotoButton->setText(QString());
        infoButton->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
