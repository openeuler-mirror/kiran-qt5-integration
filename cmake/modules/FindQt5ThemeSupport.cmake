find_package(PkgConfig)
#　提取Qt5Gui.pc中的信息
pkg_check_modules(PKG_Qt5ThemeSupport QUIET Qt5Gui)

set(Qt5ThemeSupport_DEFINITIONS ${PKG_Qt5ThemeSupport_CFLAGS_OTHER})
set(Qt5ThemeSupport_VERSION ${PKG_Qt5ThemeSupport_VERSION})

# 拼出Qt5ThemeSupport头文件路径
find_path(Qt5ThemeSupport_INCLUDE_DIR
        NAMES
        QtThemeSupport/private/qgenericunixthemes_p.h
        HINTS
        ${PKG_Qt5ThemeSupport_INCLUDEDIR}/QtThemeSupport/${PKG_Qt5ThemeSupport_VERSION}/
        )

#　拼出Qt5ThemeSupport静态库位置
find_library(Qt5ThemeSupport_LIBRARY
        NAMES
        Qt5ThemeSupportKS
        Qt5ThemeSupport
        HINTS
        ${PKG_Qt5ThemeSupport_LIBRARY_DIRS}
        )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Qt5ThemeSupport
        FOUND_VAR
        Qt5ThemeSupport_FOUND
        REQUIRED_VARS
        Qt5ThemeSupport_LIBRARY
        Qt5ThemeSupport_INCLUDE_DIR
        VERSION_VAR
        Qt5ThemeSupport_VERSION
        )

if(Qt5ThemeSupport_FOUND AND NOT TARGET Qt5ThemeSupport::Qt5ThemeSupport)
    add_library(Qt5ThemeSupport::Qt5ThemeSupport UNKNOWN IMPORTED)
    set_target_properties(Qt5ThemeSupport::Qt5ThemeSupport PROPERTIES
            IMPORTED_LOCATION "${Qt5ThemeSupport_LIBRARY}"
            INTERFACE_COMPILE_OPTIONS "${Qt5ThemeSupport_DEFINITIONS}"
            INTERFACE_INCLUDE_DIRECTORIES "${Qt5ThemeSupport_INCLUDE_DIR}"
            )
endif()

mark_as_advanced(Qt5ThemeSupport_LIBRARY Qt5ThemeSupport_INCLUDE_DIR)

# compatibility variables
set(Qt5ThemeSupport_LIBRARIES ${Qt5ThemeSupport_LIBRARY})
set(Qt5ThemeSupport_INCLUDE_DIRS ${Qt5ThemeSupport_INCLUDE_DIR})
set(Qt5ThemeSupport_VERSION_STRING ${Qt5ThemeSupport_VERSION})


include(FeatureSummary)
set_package_properties(Qt5ThemeSupport PROPERTIES
        URL "https://www.qt.io"
        DESCRIPTION "Qt ThemeSupport module."
        )

