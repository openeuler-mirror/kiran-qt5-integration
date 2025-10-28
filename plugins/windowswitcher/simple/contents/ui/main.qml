/**
 * Copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 * kiran-qt5-integration is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */
import QtQuick 2.0
import QtQuick.Layouts 1.1
import org.kde.kquickcontrolsaddons 2.0
import org.kde.kwin 2.0 as KWin
import org.kde.plasma.components 3.0 as PlasmaComponents3
import org.kde.plasma.core 2.0 as PlasmaCore

// KWin.Switcher: src/tabbox/tabboxhandler.cpp RegisterType Switcher
// see also: KWin src/tabbox/switcheritem.h
KWin.Switcher {
    id: windowSwitcher
    currentIndex: windowListView.currentIndex

    property color backgroundColor: windowSwitcher.darkTheme ? "#222222" : "#E8E8E8"
    property color textColor: windowSwitcher.darkTheme ? "#ffffff" : "#222222"
    property color highlightedTextColor: windowSwitcher.darkTheme ? "#2eb3ff" : "#00a2ff"

    // kf5-plasma: 顶层窗口
    PlasmaCore.Dialog {
        id: dialog

        // 该窗口为悬浮在其他窗口之上的自由浮动窗口，不固定在屏幕边缘或角落，通常用于弹出类、呼出式对话框，
        // 参见 PlasmaCore.Types.Location 枚举定义。
        location: PlasmaCore.Types.Floating
        visible: windowSwitcher.visible

        // 无边框、无背景 阴影需自己实现
        flags: Qt.X11BypassWindowManagerHint
        backgroundHints: PlasmaCore.Dialog.NoBackground

        // 居中
        x: windowSwitcher.screenGeometry.x + windowSwitcher.screenGeometry.width * 0.5 - dialogMainItem.width * 0.5
        y: windowSwitcher.screenGeometry.y + windowSwitcher.screenGeometry.height * 0.5 - dialogMainItem.height * 0.5
        
        mainItem: Item {
            id: dialogMainItem

            property int maxWidth: windowSwitcher.screenGeometry.width * 0.3
            property int maxHeight: windowSwitcher.screenGeometry.height * 0.5
            property real screenFactor: windowSwitcher.screenGeometry.width / windowSwitcher.screenGeometry.height
            property int optimalHeight: (windowListView.rowHeight * windowListView.count) + (windowListView.anchors.margins * 2)

            focus: true
            width: maxWidth
            height: Math.min(maxHeight, optimalHeight)

            // 背景
            Rectangle {
                anchors.fill: parent
                color: windowSwitcher.backgroundColor
                radius: 8
                opacity: 0.8
            }

            // 键盘导航功能
            Keys.onPressed: {
                if (event.key == Qt.Key_Up) {
                    windowListView.decrementCurrentIndex();
                } else if (event.key == Qt.Key_Down) {
                    windowListView.incrementCurrentIndex();
                }
            }

            // 列表项委托，绘制窗口项
            Component {
                id: listViewDelegate
                Item {
                    id: delegateItem
                    width: windowListView.width
                    height: windowListView.rowHeight
                    Row {
                        anchors.fill: parent
                        anchors.margins: 4
                        spacing: PlasmaCore.Units.smallSpacing
                        // 窗口图标
                        PlasmaCore.IconItem {
                            id: windowIcon
                            source: icon
                            width: PlasmaCore.Units.iconSizes.small
                            height: PlasmaCore.Units.iconSizes.small
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        // 窗口标题
                        PlasmaComponents3.Label {
                            id: captionItem
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignLeft
                            color: delegateItem.ListView.isCurrentItem ? windowSwitcher.highlightedTextColor : windowSwitcher.textColor
                            elide: Text.ElideMiddle
                            text: caption
                            anchors.verticalCenter: parent.verticalCenter
                            // 拓展label使用剩余水平空间
                            Layout.fillWidth: true
                        }
                    }

                    // 点击切换当前窗口功能
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            windowListView.currentIndex = index;
                        }
                    }
                }
            }

            ListView {
                id: windowListView
                property int rowHeight: PlasmaCore.Units.iconSizes.medium + PlasmaCore.Units.smallSpacing * 2
                model: windowSwitcher.model
                delegate: listViewDelegate
                anchors.fill: parent
                anchors.margins: 6
                // 处理TabBox的currentIndex变化时，更新UI中WindowSwitcher的当前索引
                Connections {
                    target: windowSwitcher
                    function onCurrentIndexChanged() {
                        windowListView.currentIndex = windowSwitcher.currentIndex;
                    }
                }
            }
        }

    }
}
