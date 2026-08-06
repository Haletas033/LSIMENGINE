cmake_minimum_required(VERSION 3.10)
project(LSIM)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(external/glad)
add_subdirectory(external/glm)
add_subdirectory(external/glfw)

if(DEFINED SCRIPTS)
    message(STATUS "Including extra scripts from: ${SCRIPTS}")
    file(GLOB_RECURSE SCRIPTS_SOURCES "${SCRIPTS}/*.cpp")
else()
    set(SCRIPTS_SOURCES "")
endif()

set(IMGUI_SOURCES
        external/imgui/imgui.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_widgets.cpp
        external/imgui/imgui_demo.cpp
        external/imgui/backends/imgui_impl_glfw.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
        include/ECS/entityManager.h
)
add_executable(LSIM
        ${IMGUI_SOURCES}
        ${SCRIPTS_SOURCES}
        Src/main.cpp
        include/gl/shaderClass.h
        Src/gl/shaderClass.cpp
        include/gl/VBO.h
        Src/gl/VBO.cpp
        include/gl/VAO.h
        Src/gl/VAO.cpp
        include/gl/EBO.h
        Src/gl/EBO.cpp
        include/scene/camera.h
        Src/scene/camera.cpp
        include/geometry/terrain.h
        Src/geometry/terrain.cpp
        include/geometry/mesh.h
        Src/geometry/mesh.cpp
        include/geometry/primitive.h
        Src/geometry/primitive.cpp
        include/geometry/model.h
        Src/geometry/model.cpp
        include/utils/fileIO.h
        Src/utils/fileIO.cpp
        include/inputs/gui.h Src/inputs/gui.cpp
        include/inputs/inputs.h Src/inputs/inputs.cpp
        Src/main.cpp Src/main.cpp include/scene/light.h
        include/scene/scene.h include/utils/defaults.h
        include/utils/logging/log.h Src/utils/logging/log.cpp
        include/utils/json.h Src/utils/json.cpp Src/utils/texture.cpp
        include/utils/texture.h
        include/scene/script.h
        include/utils/meshPicking.h
        include/LSIMtypes.h
        include/editor/sharedState.h
        Src/editor/sharedState.cpp
        include/editor/meshInputs.h
        Src/editor/meshInputs.cpp
        include/inputs/keyDispatch.h
        include/inputs/keyDispatcherUndef.h
        include/LSIMhelpers.h
        Src/editor/lightInputs.cpp
        include/editor/lightInputs.h
        include/editor/IOInputs.h
        Src/editor/IOInputs.cpp
        include/editor/editorInputs.h
        Src/editor/editorInputs.cpp
        include/ECS/entity.h
        Src/ECS/entityManager.cpp
        include/ECS/registry.h
        include/geometry/transform.h
        Src/geometry/transform.cpp
        include/rendering/material.h
        include/resources/resourceManager.h
        Src/rendering/material.cpp
        Src/resources/resourceManager.cpp
)
target_include_directories(LSIM
        PRIVATE external/glad/include
        external/glfw/include external/
        external/imgui
        external/imgui/backends
        include
)
file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/skybox DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/config DESTINATION ${CMAKE_BINARY_DIR})
target_link_libraries(LSIM PRIVATE glfw glm::glm-header-only glad)
option(GAME "Build LSIM in GAME mode" OFF)
if(GAME)
    target_compile_definitions(LSIM PRIVATE GAME=1)
else()
    target_compile_definitions(LSIM PRIVATE ENGINE=1)
endif()