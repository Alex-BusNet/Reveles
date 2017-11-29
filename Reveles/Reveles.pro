#-------------------------------------------------
#
# Project created by QtCreator 2017-11-27T19:01:03
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Reveles
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    main.cpp \
    RPi/Core/mainwidget.cpp \
    RPi/Core/revelescore.cpp \
    RPi/Core/revelesmap.cpp \
    RPi/Sensors/gps.cpp \
    RPi/Common/gpio.cpp \
    RPi/Python/revelesio.cpp

HEADERS += \
    RPi/Core/mainwidget.h \
    RPi/Core/revelescore.h \
    RPi/Core/datatypes.h \
    RPi/Core/revelesmap.h \
    RPi/Sensors/gps.h \
    RPi/Common/gpio.h \
    RPi/Python/revelesio.h

FORMS += \
    RPi/Core/mainwidget.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../AppData/Local/Programs/Python/Python36/libs/ -lpython36
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../AppData/Local/Programs/Python/Python36/libs/ -lpython36
else:unix:!macx: LIBS += -L$$PWD/../../../../AppData/Local/Programs/Python/Python36/libs/ -lpython36

INCLUDEPATH += $$PWD/../../../../AppData/Local/Programs/Python/Python36/include
DEPENDPATH += $$PWD/../../../../AppData/Local/Programs/Python/Python36/include
