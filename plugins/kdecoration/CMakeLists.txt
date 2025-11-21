set(TARGET_NAME kiran-decoration)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)

find_package (KDecoration2 REQUIRED)
find_package(KF5Config)
find_package(KF5CoreAddons)
find_package(KF5GuiAddons)
find_package(KF5WindowSystem)

file(GLOB_RECURSE DECORATION_SRCS "*.cpp" "*.h" "*.ui")
set(DECORATION_QRC ${PROJECT_SOURCE_DIR}/data/kdecoration/kdecoration.qrc)

add_library (${TARGET_NAME} MODULE
    ${DECORATION_SRCS}
    ${DECORATION_QRC}
)

target_link_libraries (${TARGET_NAME}
    PUBLIC
        Qt5::Core
        Qt5::Gui
        KF5::ConfigCore
        KF5::ConfigGui
        KF5::CoreAddons
        KF5::GuiAddons
        KF5::WindowSystem
    PRIVATE
        KDecoration2::KDecoration
        kiran-integration-theme
)

install (TARGETS ${TARGET_NAME}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/${KDE_INSTALL_PLUGINDIR}/org.kde.kdecoration2)
