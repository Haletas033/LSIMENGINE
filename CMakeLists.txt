cmake_minimum_required(VERSION 3.10)
project(LSIM)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
find_package(Vulkan REQUIRED)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
)

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.92.2
)

FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG master
)

FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.12.0
)

FetchContent_Declare(
        fastnoiselite
        GIT_REPOSITORY https://github.com/Auburn/FastNoiseLite.git
        GIT_TAG v1.1.1
)

FetchContent_MakeAvailable(
        glfw
        glm
        imgui
        stb
        nlohmann_json
        fastnoiselite
)

if(DEFINED SCRIPTS)
    message(STATUS "Including extra scripts from: ${SCRIPTS}")
    file(GLOB_RECURSE SCRIPTS_SOURCES "${SCRIPTS}/*.cpp")
else()
    set(SCRIPTS_SOURCES "")
endif()

set(IMGUI_SOURCES
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
)

include(cmake/ECS.cmake)
include(cmake/editor.cmake)
include(cmake/engine.cmake)
include(cmake/geometry.cmake)
include(cmake/gl.cmake)
include(cmake/inputs.cmake)
include(cmake/platform.cmake)
include(cmake/rendering.cmake)
include(cmake/resources.cmake)
include(cmake/scene.cmake)
include(cmake/utils.cmake)
include(cmake/shaderCompilation.cmake)

add_executable(LSIM
        ${IMGUI_SOURCES}
        ${SCRIPTS_SOURCES}
        Src/main.cpp
#        ${ECS_SOURCES}
#        ${EDITOR_SOURCES}
#        ${ENGINE_SOURCES}
#        ${GEOMETRY_SOURCES}
#        ${GL_SOURCES}
#        ${INPUT_SOURCES}
#        ${PLATFORM_SOURCES}
#        ${RENDERING_SOURCES}
#        ${RESOURCE_SOURCES}
#        ${SCENE_SOURCES}
#        ${UTIL_SOURCES}
)
target_include_directories(LSIM PRIVATE
        include
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        ${stb_SOURCE_DIR}
        ${fastnoiselite_SOURCE_DIR}/Cpp
)

file(COPY ${CMAKE_SOURCE_DIR}/skybox DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/config DESTINATION ${CMAKE_BINARY_DIR})
target_link_libraries(LSIM PRIVATE Vulkan::Vulkan glfw glm::glm nlohmann_json::nlohmann_json)

add_dependencies(LSIM compile_shaders)

option(GAME "Build LSIM in GAME mode" OFF)
if(GAME)
    target_compile_definitions(LSIM PRIVATE GAME=1)
else()
    target_compile_definitions(LSIM PRIVATE ENGINE=1)
endif()