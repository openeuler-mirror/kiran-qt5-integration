set(TARGET_NAME "kiranstyle")

find_package(Qt5 COMPONENTS Widgets Svg DBus)
file(GLOB_RECURSE SRC "src/*.cpp" "src/*.h" "src/*.ui")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_library( ${TARGET_NAME} SHARED
        ${SRC} )

target_include_directories(${TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
        ${Qt5Svg_PRIVATE_INCLUDE_DIRS}
        ${Qt5Widgets_PRIVATE_INCLUDE_DIRS}
        src/)

target_link_libraries(${TARGET_NAME}
        kiran-style-helper
        Qt5::Widgets
        Qt5::DBus
        Qt5::Svg)

#kiran style 插件
install(TARGETS ${TARGET_NAME} DESTINATION ${QT5_STYLE_INSTALL_DIR})