# kiran-qt5-integration

#### 介绍
Kiran桌面qt5平台一体化插件,其中包含QPlatformTheme插件，QStyle插件以及提供给开发使用的kiran-style-helper的共享库

#### 编译安装
```shell
# yum install cmake gcc-c++ qt5-linguist qt5-qtbase-devel qt5-qtbase-private-devel qt5-qtx11extras-devel  kiran-qdbusxml2cpp kiran-log-qt5-devel qt5-qtsvg-devel gsettings-qt-devel
# mkdir
# cd build && cmake -DCMAKE_INSTALL_PREFIX=/usr ..
# make
# make install 
```

#### 运行
不需要运行,安装后，若是kiran桌面环境则会加载kiran QPlatformTheme插件，kiran QPlatformTheme插件会使Qt应用加载Kiran QStyle插件。也可通过启动参数-platformtheme和-style指定加载
