cmake_minimum_required(VERSION 3.5)
project(Reveles)

set(CMAKE_INCLUDE_CURRENT_DIR  ON)
set(CMAKE_AUTOMOC ON)

# TODO: Make this find directory dynamically for use on different systems
set(CMAKE_PREFIX_PATH "C:\\Qt\\Qt5.9\\5.9.2\\mingw53_52")
set(Qt5Core_DIR "C:\\Qt\\Qt5.9\\5.9.2\\mingw53_32\\lib\\cmake\\Qt5Core")
set(Qt5Gui_DIR "C:\\Qt\\Qt5.9\\5.9.2\\mingw53_32\\lib\\cmake\\Qt5Gui")
set(Qt5Widgets_DIR "C:\\Qt\\Qt5.9\\5.9.2\\mingw53_32\\lib\\cmake\\Qt5Widgets")

add_definitions(-DUSE_CLION)
add_definitions(-D__WIN32__)

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

find_package(Qt5Core REQUIRED)

find_package(Qt5Widgets REQUIRED)

find_package(Qt5Gui REQUIRED)
include_directories(${Qt5_INCLUDE_DIRS})

#add_subdirectory("Arduino/RevelesArduino")

#MinGW CMake:
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#MSVC CMake:
#set(PYBIND11_CPP_STANDARD /std:c++latest)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")

set(SOURCE_FILES
        main.cpp

        GUI/RevelesGUI/main.cpp
        GUI/RevelesGUI/revelesgui.h
        GUI/RevelesGUI/revelesgui.cpp
        GUI/RevelesGUI/revelesgui.ui
        GUI/RevelesGUI/addlocationdialog.cpp
        GUI/RevelesGUI/addlocationdialog.h
        GUI/RevelesGUI/locationpushbutton.h
        GUI/RevelesGUI/Assets/Icons/locations.png
        GUI/RevelesGUI/Assets/Icons/newLocation.png
        GUI/RevelesGUI/Assets/Icons/settings.png

        Libraries/Linux/asm/ioctl.h
        Libraries/Linux/sys/
        Libraries/Linux/uapi/asm/ioctl.h

        Libraries/wiringPi/

        RPi/Common/datatypes.h
        RPi/Common/gpio.cpp
        RPi/Common/gpio.h

        RPi/Core/mainwidget.cpp
        RPi/Core/mainwidget.h
        RPi/Core/mainwidget.ui
        RPi/Core/revelescore.cpp
        RPi/Core/revelescore.h
        RPi/Core/revelesmap.cpp
        RPi/Core/revelesmap.h
        RPi/Core/RevelesAnalyticalEngine.cpp
        RPi/Core/RevelesAnalyticalEngine.h

        RPi/Core/IO/revelesio.cpp
        RPi/Core/IO/revelesio.h
        RPi/Python/ArduinoPICommunication.py

        RPi/Sensors/gps.h
        RPi/Sensors/gps.cpp)

add_executable(Reveles ${SOURCE_FILES})

target_link_libraries(Reveles Qt5::Widgets Qt5::Core Qt5::Gui)
target_link_libraries(Reveles ${PYTHON_LIBRARIES})