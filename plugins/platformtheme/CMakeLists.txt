set(TARGET_NAME "qkiran-next")

find_package(KiranDBusGenerate REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Qt5ThemeSupport REQUIRED)
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)
pkg_search_module(QGSETTINGS REQUIRED gsettings-qt)

# 生成使用的DBus接口源文件 kiran session appearance proxy
set_source_files_properties(
  ${PROJECT_SOURCE_DIR}/data/com.kylinsec.Kiran.SessionDaemon.Appearance.xml
  PROPERTIES CLASSNAME AppearanceProxy NO_NAMESPACE true)

kiran_qt5_add_dbus_interface(
  KIRAN_INTERFACE_SRC
  ${PROJECT_SOURCE_DIR}/data/com.kylinsec.Kiran.SessionDaemon.Appearance.xml
  appearance_proxy)

# kiran session display proxy
set_source_files_properties(
  ${PROJECT_SOURCE_DIR}/data/com.kylinsec.Kiran.SessionDaemon.Display.xml
  PROPERTIES CLASSNAME DisplayProxy NO_NAMESPACE true)

kiran_qt5_add_dbus_interface(
  KIRAN_INTERFACE_SRC
  ${PROJECT_SOURCE_DIR}/data/com.kylinsec.Kiran.SessionDaemon.Display.xml
  display_proxy)

# 避免cmake错误
foreach(interface_item ${KIRAN_INTERFACE_SRC})
  set_property(SOURCE ${interface_item} PROPERTY SKIP_AUTOGEN ON)
endforeach()

file(GLOB_RECURSE SRC "*.cpp" "*.h" "*.ui")

add_library(${TARGET_NAME} SHARED ${SRC} ${KIRAN_INTERFACE_SRC})

target_include_directories(
  ${TARGET_NAME}
  PUBLIC ${PROJECT_SOURCE_DIR}
         ${PROJECT_BINARY_DIR}
         ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
         ${Qt5Widgets_PRIVATE_INCLUDE_DIRS}
         ${Qt5ThemeSupport_INCLUDE_DIRS}
         ${QGSETTINGS_INCLUDE_DIRS}
         ${KIRAN_CC_DAEMON_INCLUDE_DIRS})

target_link_libraries(
  ${TARGET_NAME}
  lib-common
  kiran-integration-theme
  Qt5::Core
  Qt5::Widgets
  Qt5::Gui
  Qt5::DBus
  Qt5::Svg
  ${Qt5ThemeSupport_LIBRARIES}
  ${QGSETTINGS_LIBRARIES})

install(TARGETS ${TARGET_NAME} DESTINATION ${QT5_PLATFORM_THEME_INSTALL_DIR})
