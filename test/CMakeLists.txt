cmake_minimum_required(VERSION 3.5)

project(test)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets Svg)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
#pkg_search_module(KIRAN_STYLE_HELPER REQUIRED kiran-style-helper)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

file(GLOB_RECURSE SRC "src/*.cpp" "src/*.h" "src/*.ui")

add_executable(${PROJECT_NAME} ${SRC})

message("current source:${SRC}")

target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt5::Widgets
        Qt5::Gui
        Qt5::Svg
        kiran-style-helper
        ${KLOG_QT5_LIBRARIES})

target_include_directories(${PROJECT_NAME} PRIVATE
        ${KLOG_QT5_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
        ${Qt5Widgets_PRIVATE_INCLUDE_DIRS}
        ${Qt5Svg_PRIVATE_INCLUDE_DIRS})
