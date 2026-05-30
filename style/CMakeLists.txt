set(TARGET_NAME "kiranstyle")

file(GLOB_RECURSE SRC "src/*.cpp" "src/*.h" "src/*.ui")

add_library( ${TARGET_NAME} SHARED
        ${SRC} )

#获取kiran-style-helper之中的私有的include,kiranstyle之中的绘制需要引用到相关私有头文件
get_target_property(KIRAN_STYLE_HELPER_INCLUDE_DIRS kiran-style-helper INCLUDE_DIRECTORIES)

# 当 Qt5Svg 提供的 PRIVATE_INCLUDE_DIRS 为空或不含 QtSvg 路径时，尝试自行探测
# 这种情况当前在qt5.6.1下存在
if(NOT Qt5Svg_PRIVATE_INCLUDE_DIRS OR NOT Qt5Svg_PRIVATE_INCLUDE_DIRS MATCHES "QtSvg")
    if(Qt5Svg_VERSION_STRING)
        set(_svg_version "${Qt5Svg_VERSION_STRING}")
        set(Qt5Svg_PRIVATE_INCLUDE_DIRS
            "/usr/include/qt5/QtSvg/${_svg_version}"
            "/usr/include/qt5/QtSvg/${_svg_version}/QtSvg"
            ${Qt5Svg_PRIVATE_INCLUDE_DIRS}
        )
    endif()
endif()

target_include_directories(${TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
        ${Qt5Svg_PRIVATE_INCLUDE_DIRS}
        ${Qt5Widgets_PRIVATE_INCLUDE_DIRS}
        ${KIRAN_STYLE_HELPER_INCLUDE_DIRS}
        src/)

target_link_libraries(${TARGET_NAME}
        kiran-style-helper
        Qt5::Widgets
        Qt5::DBus
        Qt5::Svg)

#kiran style 插件
install(TARGETS ${TARGET_NAME} DESTINATION ${QT5_STYLE_INSTALL_DIR})
