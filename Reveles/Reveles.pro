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
    Libraries/wiringPi/ads1115.c \
    Libraries/wiringPi/bmp180.c \
    Libraries/wiringPi/ds18b20.c \
    Libraries/wiringPi/htu21d.c \
    Libraries/wiringPi/max31855.c \
    Libraries/wiringPi/max5322.c \
    Libraries/wiringPi/mcp23008.c \
    Libraries/wiringPi/mcp23016.c \
    Libraries/wiringPi/mcp23017.c \
    Libraries/wiringPi/mcp23s08.c \
    Libraries/wiringPi/mcp23s17.c \
    Libraries/wiringPi/mcp3002.c \
    Libraries/wiringPi/mcp3004.c \
    Libraries/wiringPi/mcp3422.c \
    Libraries/wiringPi/mcp4802.c \
    Libraries/wiringPi/pcf8574.c \
    Libraries/wiringPi/pcf8591.c \
    Libraries/wiringPi/piHiPri.c \
    Libraries/wiringPi/piThread.c \
    Libraries/wiringPi/rht03.c \
    Libraries/wiringPi/sn3218.c \
    Libraries/wiringPi/softPwm.c \
    Libraries/wiringPi/softServo.c \
    Libraries/wiringPi/softTone.c \
    Libraries/wiringPi/sr595.c \
    Libraries/wiringPi/wiringPi.c \
    Libraries/wiringPi/wiringPiI2C.c \
    Libraries/wiringPi/wiringPiSPI.c \
    Libraries/wiringPi/wiringSerial.c \
    Libraries/wiringPi/wiringShift.c \
    RPi/Common/gpio.cpp \
    RPi/Core/IO/revelesio.cpp \
    RPi/Core/mainwidget.cpp \
    RPi/Core/RevelesAnalyticalEngine.cpp \
    RPi/Core/revelescore.cpp \
    RPi/Core/revelesmap.cpp \
    RPi/GUI/addlocationdialog.cpp \
    RPi/GUI/revelesgui.cpp \
    RPi/Sensors/gps.cpp \
    main.cpp

HEADERS += \
    Libraries/wiringPi/wiringPi.h \
    Libraries/wiringPi/wiringPiI2C.h \
    Libraries/wiringPi/softPwm.h \
    Libraries/wiringPi/softTone.h \
    Libraries/wiringPi/ads1115.h \
    Libraries/wiringPi/bmp180.h \
    Libraries/wiringPi/ds18b20.h \
    Libraries/wiringPi/htu21d.h \
    Libraries/wiringPi/max31855.h \
    Libraries/wiringPi/max5322.h \
    Libraries/wiringPi/mcp23008.h \
    Libraries/wiringPi/mcp23016.h \
    Libraries/wiringPi/mcp23016reg.h \
    Libraries/wiringPi/mcp23017.h \
    Libraries/wiringPi/mcp23s08.h \
    Libraries/wiringPi/mcp23s17.h \
    Libraries/wiringPi/mcp23x08.h \
    Libraries/wiringPi/mcp23x0817.h \
    Libraries/wiringPi/mcp3002.h \
    Libraries/wiringPi/mcp3004.h \
    Libraries/wiringPi/mcp3422.h \
    Libraries/wiringPi/mcp4802.h \
    Libraries/wiringPi/pcf8574.h \
    Libraries/wiringPi/pcf8591.h \
    Libraries/wiringPi/rht03.h \
    Libraries/wiringPi/sn3218.h \
    Libraries/wiringPi/softPwm.h \
    Libraries/wiringPi/softServo.h \
    Libraries/wiringPi/softTone.h \
    Libraries/wiringPi/sr595.h \
    Libraries/wiringPi/wiringPi.h \
    Libraries/wiringPi/wiringPiI2C.h \
    Libraries/wiringPi/wiringPiSPI.h \
    Libraries/wiringPi/wiringSerial.h \
    Libraries/wiringPi/wiringShift.h \
    Libraries/version.h \
    RPi/Common/datatypes.h \
    RPi/Common/gpio.h \
    RPi/Core/IO/revelesio.h \
    RPi/Core/mainwidget.h \
    RPi/Core/RevelesAnalyticalEngine.h \
    RPi/Core/revelescore.h \
    RPi/Core/revelesmap.h \
    RPi/GUI/addlocationdialog.h \
    RPi/GUI/locationpushbutton.h \
    RPi/GUI/revelesgui.h \
    RPi/Sensors/gps.h


FORMS += \
    RPi/Core/mainwidget.ui \
    RPi/GUI/revelesgui.ui

DISTFILES += \
    RPi/GUI/Assets/Icons/locations.png \
    RPi/GUI/Assets/Icons/newLocation.png \
    RPi/GUI/Assets/Icons/settings.png \
    RPi/Python/ArduinoPICommunication.py
