# kiran-qt5-integration

## Overview

Kiran desktop Qt5 integration suite comprising QPlatformTheme, QStyle plugins, and the kiran-style-helper development library.

## Build and Installation

```shell
# yum install cmake gcc-c++ qt5-linguist qt5-qtbase-devel qt5-qtbase-private-devel qt5-qtx11extras-devel  kiran-qdbusxml2cpp kiran-log-qt5-devel qt5-qtsvg-devel gsettings-qt-devel kiran-cc-daemon-devel qt5-qtbase-static
# mkdir
# cd build && cmake -DCMAKE_INSTALL_PREFIX=/usr ..
# make
# make install 
```

## Run

No manual execution required. Once installed, it automatically loads the Kiran QPlatformTheme plugin within the Kiran Desktop Environment, which subsequently triggers the Kiran QStyle plugin for Qt applications. Alternatively, you can force-load these plugins via command-line options: `-platformtheme` and `-style`

The plugin automatically applies the theme to specified applications. Target applications can be defined in two ways:

### 1. Built-in Code

### 2. Configuration File

Configuration file address: `/etc/kiran-integration/kiran-qt5-integration.ini`  
Content:

```ini
[Style]
#Enables the Kiran-style application list. Includes built-in core components of the Kiran Desktop Environment. eg: kiran-*,kiran-panel
enabled-apps=ksl-os-gui,ksl-server-gui,ks-scmc-gui
```

## Code Structure

- lib
  - style: Custom QStyle implementation for QT plugins and upstream applications.
- plugins
  - platformtheme: QT platform theme plugin.
  - style: QT style plugin.
  - decoration: KWin window decoration plugin.
  
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
