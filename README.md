# LSIMENGINE

This is my attempt at making a game engine with opengl

## Current Features
* OpenGL rendering pipeline with GLFW windowing
* Support for basic 3D transformations via GLM
* Controllable camera (WASD + mouse)
* Procedural terrain generation (Perlin noise)
* Creating geometric primitives
* Simple lighting

## In progress features
- Heirarchy system
- GUI improvements
- Multiple lights, cameras, and improved lighting

## Upcoming Features
- Importing models
- File I/O for scene loading and saving
- Basic physics and collision detection

## Prerequisites (Fedora)

Intsall neccessary dependencies:

```bash
sudo dnf install glfw-devel glm-devel
```

## How to run
1. Clone the repository
```bash
git clone https://github.com/Haletas033/LSIMENGINE.git
cd opengl-clion-project
```
2. Create a build directory
```bash
mkdir build
cd build
```
3. Run cmake to configure the project
```bash
cmake ..
```
4. Build the project using make
```bash
make
```
5. Run the project
```bash
./LSIMENGINE
```
## Project Structure
```bash
.
LSIMENGINE/
├── .gitignore
├── CMakeLists.txt
├── external/
│   └── glad/
│       ├── CMakeLists.txt
│       ├── include/
│       │   ├── glad/
│       │   │   └── glad.h
│       │   └── KHR/
│       │       └── khrplatform.h
│       └── src/
│           └── glad.c
├── imgui/
│   ├── .editorconfig
│   ├── .gitattributes
│   ├── .github/
│   │   ├── FUNDING.yml
│   │   ├── ISSUE_TEMPLATE/
│   │   │   ├── config.yml
│   │   │   └── issue_template.yml
│   │   ├── pull_request_template.md
│   │   └── workflows/
│   │       ├── build.yml
│   │       ├── manual.yml
│   │       ├── scheduled.yml
│   │       └── static-analysis.yml
│   ├── .gitignore
│   ├── backends/
│   │   ├── imgui_impl_glfw.cpp
│   │   ├── imgui_impl_glfw.h
│   │   ├── imgui_impl_opengl3.cpp
│   │   └── imgui_impl_opengl3.h
│   ├── imconfig.h
│   ├── imgui_demo.cpp
│   ├── imgui_draw.cpp
│   ├── imgui_impl_opengl3_loader.h
│   ├── imgui_internal.h
│   ├── imgui_tables.cpp
│   ├── imgui_widgets.cpp
│   ├── imgui.cpp
│   ├── imgui.h
│   ├── imstb_rectpack.h
│   ├── imstb_textedit.h
│   └── imstb_truetype.h
├── include/
│   ├── camera.h
│   ├── EBO.h
│   ├── FastNoiseLite.h
│   ├── gui.h
│   ├── inputs.h
│   ├── mesh.h
│   ├── primitives.h
│   ├── shaderClass.h
│   ├── terrain.h
│   ├── VAO.h
│   └── VBO.h
├── LICENSE
├── README.md
├── shaders/
│   ├── default.frag
│   └── default.vert
└── Src/
    ├── camera.cpp
    ├── EBO.cpp
    ├── gui.cpp
    ├── inputs.cpp
    ├── main.cpp
    ├── mesh.cpp
    ├── primitives.cpp
    ├── shaderClass.cpp
    ├── terrain.cpp
    ├── VAO.cpp
    └── VBO.cpp


```

## Contributing
Contributions are welcome! Whether it's a bug fix, new feature, or documentation improvement, feel free to open an issue or submit a pull request.

## License

This project is licensed under the [MIT License](https://opensource.org/licenses/MIT).  See the [LICENSE](LICENSE) file for details.
