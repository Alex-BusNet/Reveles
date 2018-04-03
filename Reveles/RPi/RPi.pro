QT -= gui
QT += core dbus concurrent

CONFIG += c++11 console
CONFIG -= app_bundle

TARGET = RevelesCore

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
#DEFINES += QT_DEPRECATED_WARNINGS _GLIBCXX_USE_CXX11_ABI=0

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    Core/revelescore.cpp \
    Core/revelesmap.cpp \
    Core/navigationassisiant.cpp \
    Core/analyticalengine.cpp \
    Core/objectdetector.cpp \
    IO/revelescamera.cpp \
    IO/revelesio.cpp \
    IO/gps.cpp \
    IO/lsm9ds1.cpp \
    reveles_dbus_adaptor.cpp \
    Common/vector2i.cpp \
    Common/vector2f.cpp \
    Common/vectorpolar2f.cpp \
    Common/logger.cpp \
    reveles_dbus_interface.cpp \
    IO/tof.cpp

HEADERS += \
    Common/datatypes.h \
    Common/vector2i.h \
    Common/vector2f.h \
    Common/vectorpolar2f.h \
    Common/detectionqueue.h \
    Core/objectdetector.h \
    Core/revelesmap.h \
    Core/navigationassisiant.h \
    Core/analyticalengine.h \
    Core/revelescore.h \
    IO/revelesio.h \
    IO/gps.h \
    IO/lsm9ds1.h \
    IO/revelescamera.h \
    reveles_dbus_adaptor.h \
    Common/vectorutils.h \
    Common/messages.h \
    Common/logger.h \
    reveles_dbus_interface.h \
    rpi.h \
    IO/tof.h

DISTFILES += \
    Data/haarcascade_fullbody.xml \
    Data/haarcascade_lowerbody.xml \
    Data/haarcascade_upperbody.xml

INSTALLS = target
target.files = revelescore
target.path = /home/pi/Desktop/Reveles

# These need to point to the OpenCV libraries built for ARM
unix:!macx: LIBS += -L$$PWD/../../../../raspi/sysroot/usr/local/lib  \
    -lopencv_core \
    -lopencv_highgui \
    -lopencv_tracking \
    -lopencv_imgcodecs \
    -lopencv_videoio \
    -lopencv_imgproc \
    -lopencv_ml \
    -lopencv_objdetect \
    -lopencv_video

# This points to the install location of the libX11.so file
unix:!macx: LIBS += -L$$PWD/../../../../raspi/sysroot/usr/lib/arm-linux-gnueabihf -lX11

# This points to various EGL libraries needed to use with the PiCamera.
unix:!macx: LIBS += -L$$PWD/../../../../raspi/sysroot/opt/vc/lib \
    -lEGL\
    -lbcm_host \
    -lmmal_core \
    -lmmal_util \
    -lmmal_vc_client \
    -lvcos \
    -lGLESv2

# These point to libraries to use WiringPi and Raspicam (CV)
unix:!macx: LIBS += -L$$PWD/../../../../raspi/sysroot/usr/local/lib/ \
    -lraspicam_cv \
    -lwiringPi

INCLUDEPATH += $$PWD/../../../../raspi/sysroot/usr/local/include
DEPENDPATH += $$PWD/../../../../raspi/sysroot/usr/local/include
