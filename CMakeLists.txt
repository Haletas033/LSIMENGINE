cmake_minimum_required(VERSION 3.10)
project(LSIM)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)

FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad.git
        GIT_TAG v2.0.8
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

FetchContent_GetProperties(glad)

if(NOT glad_POPULATED)
    FetchContent_Populate(glad)
endif()

set(GLAD_DIR ${CMAKE_CURRENT_BINARY_DIR}/glad)

execute_process(
        COMMAND
        ${Python3_EXECUTABLE} -m glad
        --api gl:core=4.2
        --out-path ${GLAD_DIR}
        WORKING_DIRECTORY ${glad_SOURCE_DIR}
        RESULT_VARIABLE GLAD_RESULT
)

if(NOT GLAD_RESULT EQUAL 0)
    message(FATAL_ERROR "Failed to generate GLAD 2")
endif()

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
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
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

add_executable(LSIM
        ${IMGUI_SOURCES}
        ${SCRIPTS_SOURCES}
        ${GLAD_DIR}/src/gl.c

        ${ECS_SOURCES}
        ${EDITOR_SOURCES}
        ${ENGINE_SOURCES}
        ${GEOMETRY_SOURCES}
        ${GL_SOURCES}
        ${INPUT_SOURCES}
        ${PLATFORM_SOURCES}
        ${RENDERING_SOURCES}
        ${RESOURCE_SOURCES}
        ${SCENE_SOURCES}
        ${UTIL_SOURCES}
)
target_include_directories(LSIM PRIVATE
        include
        ${GLAD_DIR}/include
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        ${stb_SOURCE_DIR}
        ${fastnoiselite_SOURCE_DIR}/Cpp
)

file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/skybox DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/config DESTINATION ${CMAKE_BINARY_DIR})
target_link_libraries(LSIM PRIVATE glfw glm::glm nlohmann_json::nlohmann_json)

option(GAME "Build LSIM in GAME mode" OFF)
if(GAME)
    target_compile_definitions(LSIM PRIVATE GAME=1)
else()
    target_compile_definitions(LSIM PRIVATE ENGINE=1)
endif()