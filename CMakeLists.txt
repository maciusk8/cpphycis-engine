cmake_minimum_required(VERSION 3.14)
project(cpphysics-engine CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        master
    GIT_SHALLOW    ON
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(raylib)

set(imgui_SOURCE_DIR "${CMAKE_BINARY_DIR}/imgui-src" CACHE PATH "imgui dir" FORCE)
if(NOT EXISTS "${imgui_SOURCE_DIR}/imgui.h")
    message(STATUS "Reczne klonowanie Dear ImGui...")
    execute_process(COMMAND git clone --depth 1 https://github.com/ocornut/imgui.git ${imgui_SOURCE_DIR})
endif()

set(rlimgui_SOURCE_DIR "${CMAKE_BINARY_DIR}/rlimgui-src" CACHE PATH "rlimgui dir" FORCE)
if(NOT EXISTS "${rlimgui_SOURCE_DIR}/rlImGui.h")
    message(STATUS "Reczne klonowanie rlImGui...")
    execute_process(COMMAND git clone --depth 1 https://github.com/raylib-extras/rlImGui.git ${rlimgui_SOURCE_DIR})
endif()

set(SOURCES
    src/main.cpp
    src/physics/engine.cpp
    src/physics/softBodyFactory.cpp
    src/render/renderer.cpp
    src/ui/ui.cpp
    
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    
    ${rlimgui_SOURCE_DIR}/rlImGui.cpp
)

add_executable(silnik_dod ${SOURCES})

target_include_directories(silnik_dod PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${imgui_SOURCE_DIR}
    ${rlimgui_SOURCE_DIR}
)

target_link_libraries(silnik_dod PRIVATE raylib)