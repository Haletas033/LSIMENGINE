cmake_minimum_required(VERSION 3.10)
project(LSIM)

set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_subdirectory(external/glad)

set(IMGUI_SOURCES
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_demo.cpp
        imgui/backends/imgui_impl_glfw.cpp
        imgui/backends/imgui_impl_opengl3.cpp
        include/gui.h
        Src/gui.cpp
        include/inputs.h
        Src/inputs.cpp
)

add_executable(LSIM
        Src/main.cpp
        include/shaderClass.h
        Src/shaderClass.cpp
        include/VBO.h
        Src/VBO.cpp
        include/VAO.h
        Src/VAO.cpp
        include/EBO.h
        Src/EBO.cpp
        include/camera.h
        Src/camera.cpp
        include/terrain.h
        Src/terrain.cpp
        include/mesh.h
        Src/mesh.cpp
        include/primitives.h
        Src/primitives.cpp

        ${IMGUI_SOURCES}
)

target_include_directories(LSIM PRIVATE
        external/glad/include
        ${PROJECT_SOURCE_DIR}/imgui
        ${PROJECT_SOURCE_DIR}/imgui/backends
)

target_link_libraries(LSIM PRIVATE glfw glm::glm glad)
