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

## Prerequisites (Windows)
This project requires Git, Cmake, Ninja(or another build system)
## How to run
1. Clone the repository
```bash
git clone https://github.com/Haletas033/LSIMENGINE.git
cd LSIMENGINE
```
2. create build directory
```bash
mkdir build
```
3. Run cmake to configure the project
```bash
cmake -S . -B build -G Ninja
```
4. Build the project using ninja (or another build system)
```bash
cd build
ninja
```
5. Run the project
```bash
./LSIM
```
## Project Structure
<!-- TREE_START -->
```bash
.
├── CMakeLists.txt
├── CMakeSettings.json
├── LICENSE
├── README.md
├── Src
│   ├── EBO.cpp
│   ├── VAO.cpp
│   ├── VBO.cpp
│   ├── camera.cpp
│   ├── fileIO.cpp
│   ├── gui.cpp
│   ├── inputs.cpp
│   ├── main.cpp
│   ├── mesh.cpp
│   ├── primitives.cpp
│   ├── shaderClass.cpp
│   └── terrain.cpp
├── config
│   └── config.json
├── imgui
│   ├── backends
│   │   ├── imgui_impl_glfw.cpp
│   │   ├── imgui_impl_glfw.h
│   │   ├── imgui_impl_opengl3.cpp
│   │   └── imgui_impl_opengl3.h
│   ├── imconfig.h
│   ├── imgui.cpp
│   ├── imgui.h
│   ├── imgui_demo.cpp
│   ├── imgui_draw.cpp
│   ├── imgui_impl_opengl3_loader.h
│   ├── imgui_internal.h
│   ├── imgui_tables.cpp
│   ├── imgui_widgets.cpp
│   ├── imstb_rectpack.h
│   ├── imstb_textedit.h
│   └── imstb_truetype.h
├── include
│   ├── EBO.h
│   ├── FastNoiseLite.h
│   ├── VAO.h
│   ├── VBO.h
│   ├── camera.h
│   ├── defaults.h
│   ├── fileIO.h
│   ├── gui.h
│   ├── inputs.h
│   ├── light.h
│   ├── mesh.h
│   ├── primitives.h
│   ├── scene.h
│   ├── shaderClass.h
│   └── terrain.h
├── shaders
│   ├── default.frag
│   └── default.vert
└── tree.txt

7 directories, 51 files
```
<!-- TREE_END -->
## Contributing
Contributions are welcome! Whether it's a bug fix, new feature, or documentation improvement, feel free to open an issue or submit a pull request.

## License

This project is licensed under the [MIT License](https://opensource.org/licenses/MIT).  See the [LICENSE](LICENSE) file for details.
