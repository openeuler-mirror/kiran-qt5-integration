set(TARGET_NAME "lib-common")

file(GLOB_RECURSE COMMON_SRC "./*.cpp" "./*.h")
add_library(${TARGET_NAME} STATIC ${COMMON_SRC})
target_link_libraries(${TARGET_NAME} Qt5::Core ${GSETTINGS_QT_LIBRARIES})
target_include_directories(${TARGET_NAME}
        PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/include
        ${GSETTINGS_QT_INCLUDE_DIRS})