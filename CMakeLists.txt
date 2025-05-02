cmake_minimum_required(VERSION 3.10)
project(LSIM)

set(CMAKE_CXX_STANDARD 17)

# GLFW and GLM are installed system-wide
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# Add GLAD manually (see step 4)
add_subdirectory(external/glad)

add_executable(LSIM Src/main.cpp
        Src/shaderClass.h
        Src/shaderClass.cpp
        Src/VBO.h
        Src/VBO.cpp
        Src/VAO.h
        Src/VAO.cpp
        Src/EBO.h
        Src/EBO.cpp
        Src/camera.h
        Src/camera.cpp)

target_link_libraries(LSIM PRIVATE glfw glm::glm glad)
target_include_directories(LSIM PRIVATE external/glad/include)
