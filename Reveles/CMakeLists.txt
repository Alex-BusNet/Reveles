cmake_minimum_required(VERSION 3.5)
project(Reveles)

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

add_subdirectory(Libraries/pybind11)
pybind11_add_module(example RPI/Python/revelesio.cpp)

#MinGW CMake:
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#MSVC CMake:
set(PYBIND11_CPP_STANDARD /std:c++latest)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")

set(SOURCE_FILES
    Arduino/placeholder_for_INO_files
    RPi/Common/datatypes.h
    RPi/Common/gpio.cpp
    RPi/Common/gpio.h
    RPi/Core/mainwidget.cpp
    RPi/Core/mainwidget.h
    RPi/Core/revelescore.cpp
    RPi/Core/revelescore.h
    RPi/Core/revelesmap.cpp
    RPi/Core/revelesmap.h
    RPi/Python/revelesio.cpp
    RPi/Python/revelesio.h
    RPi/Sensors/gps.cpp
    RPi/Sensors/gps.h
    main.cpp
    RPi/Core/RevelesAnalyticalEngine.cpp
    RPi/Core/RevelesAnalyticalEngine.h)

add_executable(Reveles ${SOURCE_FILES})

target_link_libraries(Reveles ${PYTHON_LIBRARIES})