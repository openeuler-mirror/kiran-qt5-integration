set(TARGET_NAME "common")

find_package(Qt5 COMPONENTS Gui DBus)
find_package(PkgConfig REQUIRED)
find_package(KiranDBusGenerate REQUIRED)

pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(GSETTINGS_QT REQUIRED gsettings-qt)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

#生成使用的DBus接口源文件
##kiran session appearance proxy
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/data/com.kylinsec.Kiran.SessionDaemon.Appearance.xml
        PROPERTIES
        CLASSNAME KiranAppearanceProxy
        NO_NAMESPACE true)
kiran_qt5_add_dbus_interface(KIRAN_INTERFACE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/data/com.kylinsec.Kiran.SessionDaemon.Appearance.xml kiran_appearance_proxy)
##kiran session display proxy
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/data/com.kylinsec.Kiran.SessionDaemon.Display.xml
        PROPERTIES
        CLASSNAME KiranDisplayProxy
        NO_NAMESPACE true)
kiran_qt5_add_dbus_interface(KIRAN_INTERFACE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/data/com.kylinsec.Kiran.SessionDaemon.Display.xml kiran_display_proxy)
#避免cmake错误
foreach (interface_item ${KIRAN_INTERFACE_SRC})
    set_property(SOURCE ${interface_item} PROPERTY SKIP_AUTOGEN ON)
endforeach ()

file(GLOB_RECURSE COMMON_H_FILES src/*.h)
file(GLOB_RECURSE COMMON_CPP_FILES src/*.cpp)

set (ALL_FILES ${COMMON_H_FILES} ${COMMON_CPP_FILES} ${KIRAN_INTERFACE_SRC})
add_library(${TARGET_NAME} STATIC ${ALL_FILES})

target_link_libraries(${TARGET_NAME}
        Qt5::Gui
        Qt5::DBus
        ${QGSETTINGS_LIBRARIES}
        ${GSETTINGS_QT_LIBRARIES})

target_include_directories(${TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
        ${QGSETTINGS_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
        ${GSETTINGS_QT_INCLUDE_DIRS})

target_include_directories(${TARGET_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src)

install(FILES data/kiran-qt5-integration.ini 
        DESTINATION /${CMAKE_INSTALL_SYSCONFDIR}/kiran-qt5-integration/ )