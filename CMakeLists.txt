cmake_minimum_required(VERSION 3.10)
project(LSIM)
set(CMAKE_CXX_STANDARD 17)
add_subdirectory(external/glad)
add_subdirectory(external/glm)
add_subdirectory(external/glfw)
set(IMGUI_SOURCES
        external/imgui/imgui.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_widgets.cpp
        external/imgui/imgui_demo.cpp
        external/imgui/backends/imgui_impl_glfw.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
        include/inputs/gui.h Src/inputs/gui.cpp
        include/inputs/inputs.h Src/inputs/inputs.cpp
        Src/main.cpp Src/main.cpp include/scene/light.h
        include/scene/scene.h include/utils/defaults.h
        include/utils/logging/log.h Src/utils/logging/log.cpp
        include/utils/json.h Src/utils/json.cpp Src/utils/texture.cpp
        include/utils/texture.h
        include/scene/script.h
)
add_executable(LSIM
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
        include/geometry/primitives.h
        Src/geometry/primitives.cpp
        ${IMGUI_SOURCES}
        include/utils/fileIO.h
        Src/utils/fileIO.cpp
)
target_include_directories(LSIM
        PRIVATE external/glad/include
        external/glfw/include external/
        external/imgui
        external/imgui/backends
)
file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/config DESTINATION ${CMAKE_BINARY_DIR})
target_link_libraries(LSIM PRIVATE glfw glm::glm-header-only glad)
option(GAME "Build LSIM in GAME mode" OFF)
if(GAME)
    target_compile_definitions(LSIM PRIVATE GAME=1)
else()
    target_compile_definitions(LSIM PRIVATE ENGINE=1)
endif()