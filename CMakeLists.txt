cmake_minimum_required(VERSION 3.16)
project(raylib-mini-engine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib 5.0 QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(raylib GIT_REPOSITORY https://github.com/raysan5/raylib.git GIT_TAG 5.5 GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(raylib)
endif()

file(GLOB_RECURSE LAB_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${LAB_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    CONFIG_FILE_PATH="${CMAKE_SOURCE_DIR}/params.cfg"
    ASSETS_DIR="${CMAKE_SOURCE_DIR}/assets"
    LEVEL_FILE_PATH="${CMAKE_SOURCE_DIR}/level.map")

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W3 /utf-8)
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()