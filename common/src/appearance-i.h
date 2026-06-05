/**
 * Copyright (c) 2026 ~ 2027 KylinSec Co., Ltd.
 * kiran-session-guard is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yangfeng <yangfeng@kylinsec.com.cn>
 */

#pragma once

#ifdef HAVE_KIRAN_CC_DAEMON

#include <kiran-session-daemon/appearance-i.h>

#else

#define APPEARANCE_DBUS_NAME "com.kylinsec.Kiran.SessionDaemon.Appearance"
#define APPEARANCE_OBJECT_PATH "/com/kylinsec/Kiran/SessionDaemon/Appearance"
#define APPEARANCE_DBUS_INTERFACE_NAME "com.kylinsec.Kiran.SessionDaemon.Appearance"

#define APPEARANCE_DEFAULT_LIGHT_META_THEME "Kiran-white"
#define APPEARANCE_DEFAULT_LIGHT_GTK_THEME "Kiran-white"
#define APPEARANCE_DEFAULT_DARK_META_THEME "Kiran-dark"
#define APPEARANCE_DEFAULT_DARK_GTK_THEME "Kiran-dark"

#define APPEARANCE_SCHAME_ID "com.kylinsec.kiran.appearance"
#define APPEARANCE_SCHEMA_KEY_DESKTOP_BG "desktopBackground"
#define APPEARANCE_SCHEMA_KEY_LOCKSCREEN_BG "lockScreenBackground"
#define APPEARANCE_SCHEMA_KEY_AUTO_SWITCH_WINDOW_THEME "autoSwitchWindowTheme"
#define APPEARANCE_SCHEMA_KEY_SHOW_DESKTOP_BACKGROUND "showDesktopBackground"

enum AppearanceFontType
{
    APPEARANCE_FONT_TYPE_APPLICATION = 0,
    APPEARANCE_FONT_TYPE_DOCUMENT,
    APPEARANCE_FONT_TYPE_DESKTOP,
    APPEARANCE_FONT_TYPE_WINDOW_TITLE,
    APPEARANCE_FONT_TYPE_MONOSPACE,
    APPEARANCE_FONT_TYPE_LAST
};

enum AppearanceThemeType
{
    APPEARANCE_THEME_TYPE_META = 0,
    APPEARANCE_THEME_TYPE_GTK,
    APPEARANCE_THEME_TYPE_METACITY,
    APPEARANCE_THEME_TYPE_ICON,
    APPEARANCE_THEME_TYPE_CURSOR,
    APPEARANCE_THEME_TYPE_LAST,
};

#endif
