set(TARGET_NAME kiran-windowswitcher)

install(DIRECTORY simple/
        DESTINATION ${KDE_INSTALL_DATADIR}/kwin/tabbox/kiran-windowswitcher-simple
        FILES_MATCHING PATTERN "*")

install(DIRECTORY thumbnail_grid/
        DESTINATION ${KDE_INSTALL_DATADIR}/kwin/tabbox/kiran-windowswitcher-thumbnail
        FILES_MATCHING PATTERN "*")