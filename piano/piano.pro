QT       += core gui
QT += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    channeldialog.cpp \
    helperclass.cpp \
    ledspanel.cpp \
    main.cpp \
    mainmenuwidget.cpp \
    mainwindow.cpp \
    pianowindow.cpp \
    playerwindow.cpp

HEADERS += \
    channeldialog.h \
    helperclass.h \
    ledspanel.h \
    mainmenuwidget.h \
    mainwindow.h \
    pianowindow.h \
    playerwindow.h

FORMS += \
    channeldialog.ui \
    ledspanel.ui \
    mainmenuwidget.ui \
    mainwindow.ui \
    pianowindow.ui \
    playerwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    img.qrc \
    txt.qrc
    SOURCES += thirdparty/QMidi-master/src/QMidiIn.cpp \
    thirdparty/QMidi-master/src/QMidiOut.cpp \
    thirdparty/QMidi-master/src/QMidiFile.cpp

    HEADERS += thirdparty/QMidi-master/src/QMidiIn.h \
    thirdparty/QMidi-master/src/QMidiOut.h \
    thirdparty/QMidi-master/src/QMidiFile.h
    # Правильный путь для инклюдов
    INCLUDEPATH += $$PWD/thirdparty/QMidi-master/src

    # Платформенно-специфичные файлы
    win32 {
        SOURCES += thirdparty/QMidi-master/src/OS/QMidi_Win32.cpp
        LIBS += -lwinmm
    }

DISTFILES +=
