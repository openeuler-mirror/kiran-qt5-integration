set(TARGET_NAME "kiranstyle-helper")

# light theme
set(KIRAN_LIGHT_COLORS ${CMAKE_CURRENT_BINARY_DIR}/kiran-light/kiran-light-colors.css)
add_custom_command(
        OUTPUT ${KIRAN_LIGHT_COLORS}
        COMMAND ./build-qt-theme.sh
        -c data/color-scheme/light.colors
        -b data/color-scheme/base.colors
        -t kiran-light
        -d ${CMAKE_CURRENT_BINARY_DIR}/kiran-light
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# dark theme
set(KIRAN_DARK_COLORS ${CMAKE_CURRENT_BINARY_DIR}/kiran-dark/kiran-dark-colors.css)
add_custom_command(
        OUTPUT ${KIRAN_DARK_COLORS}
        COMMAND ./build-qt-theme.sh
        -c data/color-scheme/dark.colors
        -b data/color-scheme/base.colors
        -t kiran-dark
        -d ${CMAKE_CURRENT_BINARY_DIR}/kiran-dark
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

set(KIRANSTYLE_HELPER_QRC ${CMAKE_CURRENT_BINARY_DIR}/kiranstyle-helper.qrc)
configure_file(kiranstyle-helper.qrc.in ${KIRANSTYLE_HELPER_QRC})

message("${CMAKE_CURRENT_SOURCE_DIR}")
find_package(Qt5 COMPONENTS Core Gui Widgets)
file(GLOB_RECURSE SRC "src/*.cpp" "src/*.h" "src/*.ui")
file(GLOB_RECURSE INCLUDE "include/*.h")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_library(${TARGET_NAME} SHARED
        ${SRC}
        ${KIRANSTYLE_HELPER_QRC}
        ${INCLUDE}
        ${KIRAN_DARK_COLORS}
        ${KIRAN_LIGHT_COLORS})

target_include_directories(${TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_BINARY_DIR}
        ${Qt5Gui_PRIVATE_INCLUDE_DIRS})

target_include_directories(${TARGET_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${TARGET_NAME}
        common
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets)

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/)