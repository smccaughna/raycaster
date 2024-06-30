cmake_minimum_required (VERSION 3.8)

set (CMAKE_C_STANDARD 23)
set (CMAKE_C_STANDARD_REQUIRED TRUE)

set (VCPKG_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg")
set (CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

project (raycaster C)

file (GLOB_RECURSE PROJ_SOURCES "src/*.c")
file (GLOB_RECURSE PROJ_HEADERS "src/*.h")

find_package (SDL2 CONFIG REQUIRED)
find_package (SDL2_image CONFIG REQUIRED)
find_package (SDL2_ttf CONFIG REQUIRED)

if (WIN32)
    add_executable (${PROJECT_NAME} WIN32 ${PROJ_SOURCES} ${PROJ_HEADERS})
else ()
    add_executable (${PROJECT_NAME} ${PROJ_SOURCES} ${PROJ_HEADERS})
endif ()

target_link_libraries (
        ${PROJECT_NAME} PRIVATE
        SDL2::SDL2
        SDL2::SDL2main
        SDL2_image::SDL2_image
        SDL2_ttf::SDL2_ttf
)