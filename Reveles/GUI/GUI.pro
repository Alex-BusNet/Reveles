#-------------------------------------------------
#
# Project created by QtCreator 2018-01-12T13:47:50
#
#-------------------------------------------------

QT       += core dbus gui network
CONFIG += c++11 console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RevelesGUI
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        revelesgui.cpp\
        addlocationdialog.cpp \
        settingsscreen.cpp \
        reveles_dbus_interface.cpp \
        reveles_dbus_adaptor.cpp \
        tile.cpp \
    mapview.cpp

HEADERS += \
        revelesgui.h \
        addlocationdialog.h \
        locationpushbutton.h \
        settingsscreen.h \
        reveles_dbus_interface.h \
        reveles_dbus_adaptor.h \
        tile.h \
        noderenders.h \
    mapview.h

FORMS += \
        revelesgui.ui \
    settingsscreen.ui

DISTFILES += \
    Assets/Icons/locations.png \
    Assets/Icons/newLocation.png \
    Assets/Icons/settings.png \

INSTALLS = target
target.files = revelesgui
target.path = /home/pi/Desktop/Reveles
