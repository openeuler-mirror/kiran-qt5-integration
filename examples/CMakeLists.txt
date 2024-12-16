set(TARGET_NAME "kiran-integration-example")

find_package(Qt5 COMPONENTS Widgets Svg)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)

file(GLOB_RECURSE EXAMPLE_H_FILES "./*.h")
file(GLOB_RECURSE EXAMPLE_CPP_FILES "./*.cpp")
qt5_wrap_ui(UI_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.ui)

add_executable(${TARGET_NAME} ${EXAMPLE_H_FILES} ${EXAMPLE_CPP_FILES}
                              ${UI_FILES})

target_link_libraries(${TARGET_NAME} PRIVATE Qt5::Widgets Qt5::Gui Qt5::Svg
                                             kiran-integration-theme)

target_include_directories(
  ${TARGET_NAME}
  PRIVATE ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR}
          ${Qt5Gui_PRIVATE_INCLUDE_DIRS} ${Qt5Widgets_PRIVATE_INCLUDE_DIRS}
          ${Qt5Svg_PRIVATE_INCLUDE_DIRS})
