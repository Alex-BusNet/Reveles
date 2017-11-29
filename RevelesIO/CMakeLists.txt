cmake_minimum_required(VERSION 3.5)
project(RevelesIO)

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        main.cpp
        revelesio.h
        revelesio.cpp)

add_executable(RevelesIO ${SOURCE_FILES})