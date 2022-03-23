cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 11)
project(kiran-qt5-integration)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH} )
list(APPEND ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules/)

include(GNUInstallDirs)
include(Qt5PluginInstallDirs)

#共用代码,生成静态库
add_subdirectory(common)
#QPlatformTheme插件
add_subdirectory(platformtheme)
#KiranStyle提供的相关接口，也提供给KiranStyle获取颜色表内容的接口
add_subdirectory(style-helper)
#QStyle插件
add_subdirectory(style)
add_subdirectory(test)