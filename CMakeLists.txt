cmake_minimum_required (VERSION 3.8)

project (sand-game C)

list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2-cmake-modules)

set (CMAKE_C_STANDARD 23)
set (CMAKE_C_STANDARD_REQUIRED TRUE)

file (GLOB_RECURSE PROJ_SOURCES "src/*.c")
file (GLOB_RECURSE PROJ_HEADERS "src/*.h")

if (WIN32)
    add_executable (${PROJECT_NAME} WIN32 ${PROJ_SOURCES} ${PROJ_HEADERS})
else ()
    add_executable (${PROJECT_NAME} ${PROJ_SOURCES} ${PROJ_HEADERS})
endif ()

find_package (SDL2 REQUIRED)
find_package (SDL2_image REQUIRED)
find_package (SDL2_ttf REQUIRED)

target_link_libraries (
        ${PROJECT_NAME} PRIVATE
        SDL2::Main
        SDL2::Image
        SDL2::TTF
)

if (NOT MSVC)
    target_link_libraries (${PROJECT_NAME} PRIVATE m)
endif ()

if (WIN32)
    add_custom_command (
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMAND_EXPAND_LISTS
    )
endif ()

add_custom_command (
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/res
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/res
)