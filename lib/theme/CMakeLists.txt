set(TARGET_NAME kiran-integration-theme)

set(STYLE_QRC ${CMAKE_CURRENT_BINARY_DIR}/style.qrc)
configure_file(${PROJECT_SOURCE_DIR}/resources/style.qrc.in ${STYLE_QRC})

file(GLOB_RECURSE STYLE_CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE STYLE_H_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${TARGET_NAME} SHARED ${STYLE_CPP_FILES} ${STYLE_QRC}
                                  ${STYLE_H_FILES})

set_target_properties(${TARGET_NAME} PROPERTIES VERSION ${PROJECT_VERSION}
                                                SOVERSION 2)

target_include_directories(
  ${TARGET_NAME}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
          ${CMAKE_CURRENT_BINARY_DIR}
          ${PROJECT_SOURCE_DIR}
          ${PROJECT_SOURCE_DIR}/include
          ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
          ${Qt5Widgets_PRIVATE_INCLUDE_DIRS}
          ${Qt5Svg_PRIVATE_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Svg)

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})

# deprecated
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/font-size-manager.h
        DESTINATION ${KQI_INSTALL_INCLUDEDIR}/theme/)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/palette.h
        DESTINATION ${KQI_INSTALL_INCLUDEDIR}/theme/)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/style-helper.h
        DESTINATION ${KQI_INSTALL_INCLUDEDIR}/theme/)