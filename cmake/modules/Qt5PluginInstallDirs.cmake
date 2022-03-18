find_package(PkgConfig REQUIRED)

#通过qt5的pkgconfig配置文件取出插件安装位置、插件共享库安装位置
pkg_search_module(Qt5 REQUIRED Qt5)
pkg_get_variable(QT5_PLUGIN_DIR Qt5 plugindir)

set(QT5_PLATFORM_THEME_INSTALL_DIR ${QT5_PLUGIN_DIR}/platformthemes/)
set(QT5_STYLE_INSTALL_DIR ${QT5_PLUGIN_DIR}/styles)