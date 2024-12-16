set(TARGET_NAME "lib-common")

find_package(KiranDBusGenerate REQUIRED)

pkg_search_module(GSETTINGS_QT REQUIRED gsettings-qt)

file(GLOB_RECURSE COMMON_H_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE COMMON_CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(${TARGET_NAME} STATIC ${COMMON_H_FILES} ${COMMON_CPP_FILES})

target_link_libraries(${TARGET_NAME} Qt5::Core ${GSETTINGS_QT_LIBRARIES})

target_include_directories(${TARGET_NAME}
        PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/include
        ${GSETTINGS_QT_INCLUDE_DIRS})