cmake_minimum_required(VERSION 3.14)
project(cpphysics-engine CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        master
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(raylib)

set(SOURCES
    src/main.cpp
    src/physics/engine.cpp
    src/render/renderer.cpp
)

add_executable(silnik_dod ${SOURCES})

target_include_directories(silnik_dod PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(silnik_dod PRIVATE raylib)