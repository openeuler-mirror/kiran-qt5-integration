cmake_minimum_required(VERSION 3.16..${CMKAE_VERSION})

project(kiran-integration VERSION 2.7.0)

set(KF5_MIN_VERSION "5.89.0")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)

if(SYSCONF_INSTALL_DIR STREQUAL "etc")
    set(SYSCONF_INSTALL_DIR "/etc")
endif()

find_package(PkgConfig REQUIRED)
find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${ECM_MODULE_PATH}")

# Base
find_package(KiranDBusGenerate REQUIRED)
find_package(KQI REQUIRED)

# Qt Deps
find_package(Qt5 COMPONENTS Core Gui Widgets Svg DBus LinguistTools X11Extras)
pkg_search_module(GSETTINGS_QT REQUIRED gsettings-qt)
find_package(Qt5ThemeSupport REQUIRED)

# GTK Deps
pkg_search_module(GTK REQUIRED gtk+-3.0)

# KF5 Deps
find_package(KDecoration2 REQUIRED)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS Config CoreAddons
                                                        GuiAddons WindowSystem)
include(KDEInstallDirs)

# Kiran Desp
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake-macros.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/cmake-macros.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(data)
add_subdirectory(lib)
add_subdirectory(plugins)
add_subdirectory(examples)
add_subdirectory(translations)
