set(TARGET_NAME kiran-decoration)

file(GLOB_RECURSE DECORATION_SRCS "*.cpp" "*.h" "*.ui")
set(DECORATION_QRC ${PROJECT_SOURCE_DIR}/data/kdecoration/kdecoration.qrc)

add_library (${TARGET_NAME} MODULE
    ${DECORATION_SRCS}
    ${DECORATION_QRC}
)

target_include_directories(${TARGET_NAME}
    PRIVATE
        ${GSETTINGS_QT_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
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
        ${GSETTINGS_QT_LIBRARIES}
)

install (TARGETS ${TARGET_NAME}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/${KDE_INSTALL_PLUGINDIR}/org.kde.kdecoration2)
