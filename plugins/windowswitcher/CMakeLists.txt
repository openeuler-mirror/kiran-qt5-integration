set(TARGET_NAME kiran-windowswitcher)

# find_package(KF5 REQUIRED COMPONENTS
#     Config
#     CoreAddons
#     WindowSystem
# )
find_package(KF5Config)
find_package(KF5CoreAddons)
find_package(KF5GuiAddons)
find_package(KF5WindowSystem)

# find_package(KWin REQUIRED)

install(DIRECTORY simple/
        DESTINATION ${KDE_INSTALL_DATADIR}/kwin/tabbox/kiran-windowswitcher-simple
        FILES_MATCHING PATTERN "*")

install(DIRECTORY thumbnail_grid/
        DESTINATION ${KDE_INSTALL_DATADIR}/kwin/tabbox/kiran-windowswitcher-thumbnail
        FILES_MATCHING PATTERN "*")