set(TARGET_NAME "qkiran")

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets Gui Svg DBus)
find_package(Qt5ThemeSupport REQUIRED)

pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(QGSETTINGS REQUIRED gsettings-qt)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

file(GLOB_RECURSE SRC "*.cpp" "*.h" "*.ui")

add_library(${TARGET_NAME} SHARED
        ${SRC})

target_include_directories(${TARGET_NAME} PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
        ${Qt5Widgets_PRIVATE_INCLUDE_DIRS}
        ${Qt5ThemeSupport_INCLUDE_DIRS}
        ${QGSETTINGS_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME}
        common
        kiran-style-helper
        Qt5::Widgets
        Qt5::Gui
        Qt5::DBus
        Qt5::Svg
        ${Qt5ThemeSupport_LIBRARIES}
        ${QGSETTINGS_LIBRARIES})

install(TARGETS ${TARGET_NAME} DESTINATION ${QT5_PLATFORM_THEME_INSTALL_DIR})