set(TARGET_NAME "kiranstyle")

file(GLOB_RECURSE SRC "*.cpp" "*.h" "*.ui")

add_library(${TARGET_NAME} SHARED ${SRC})

target_include_directories(
  ${TARGET_NAME} PRIVATE ${PROJECT_SOURCE_DIR} ${Qt5Svg_PRIVATE_INCLUDE_DIRS}
                         ${Qt5Widgets_PRIVATE_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME} kiran-integration-theme Qt5::Widgets lib-common
                      Qt5::DBus Qt5::Svg)

# kiran style 插件
install(TARGETS ${TARGET_NAME} DESTINATION ${QT5_STYLE_INSTALL_DIR})
