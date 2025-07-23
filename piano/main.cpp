#include "mainwindow.h"
#include <QPixmap>
#include <QIcon>
#include <QMediaPlayer>
#include <QSoundEffect>

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.show();
    return a.exec();
}
