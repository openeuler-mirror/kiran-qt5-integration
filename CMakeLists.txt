cmake_minimum_required(VERSION 3.5)

project(kiran-integration VERSION 2.7.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(PkgConfig REQUIRED)
find_package(KQI REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets Svg DBus)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake-macros.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/cmake-macros.h)

add_subdirectory(data)
add_subdirectory(lib)
add_subdirectory(plugins)
add_subdirectory(examples)
