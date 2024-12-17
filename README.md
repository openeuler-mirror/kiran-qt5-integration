# kiran-qt5-integration

## 介绍
Kiran桌面qt5平台一体化插件,其中包含QPlatformTheme插件，QStyle插件以及提供给开发使用的kiran-style-helper的共享库

## 编译安装
```shell
# yum install cmake gcc-c++ qt5-linguist qt5-qtbase-devel qt5-qtbase-private-devel qt5-qtx11extras-devel  kiran-qdbusxml2cpp kiran-log-qt5-devel qt5-qtsvg-devel gsettings-qt-devel
# mkdir
# cd build && cmake -DCMAKE_INSTALL_PREFIX=/usr ..
# make
# make install 
```

## 运行
不需要运行,安装后，若是kiran桌面环境则会加载kiran QPlatformTheme插件，kiran QPlatformTheme插件会使Qt应用加载Kiran QStyle插件。也可通过启动参数-platformtheme和-style指定加载

插件会自动匹配指定的应用程序采用该主题样式，主要通过两种方式指定应用程序：
### 1. 代码内置

### 2. 配置文件
配置文件地址： `/etc/kiran-integration/kiran-qt5-integration.ini`
内容如下：
```ini
[Style]
#启用kiran style的应用列表,已内置部分kiran桌面环境的组件，eg: kiran-*,kiran-panel
enabled-apps=ksl-os-gui,ksl-server-gui,ks-scmc-gui
```


## 代码结构

- lib
  - style: 实现自定义的QStyle，提供给QT插件和上层应用使用
- plugins
  - platformtheme: qt平台主题插件
  - style: qt style插件
  - decoration: kwin窗口标题栏样式插件
  

## Other
```shell
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j12
sudo cmake --install build
./build/examples/kiran-integration-example

# uninstall (just remove installed file)
sudo xargs rm -v < ./build/install_manifest.txt
```

### DEBUG
```shell
export QT_QPA_PLATFORMTHEME=kiran
export QT_DEBUG_PLUGINS=1
```
