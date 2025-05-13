cmake_minimum_required(VERSION 3.10)
project(LSIM)

set(CMAKE_CXX_STANDARD 17)

# GLFW and GLM are installed system-wide
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# Add GLAD manually (see step 4)
add_subdirectory(external/glad)

add_executable(LSIM Src/main.cpp
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
        Src/primitives.cpp)

target_link_libraries(LSIM PRIVATE glfw glm::glm glad)
target_include_directories(LSIM PRIVATE external/glad/include)
