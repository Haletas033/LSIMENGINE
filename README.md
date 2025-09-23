# LSIM ENGINE
**v1.0**\
Documentation is available [here](https://haletas033.github.io/HaletasWeb/LSIMdocs.html)

<img width="1147" height="727" alt="image" src="https://github.com/user-attachments/assets/9a74fad5-4774-465f-92ae-2d08e109c814" />

## Current Features:
* OpenGL rendering pipeline with GLFW windowing
* Support for basic 3D transformations via GLM
* Controllable camera (WASD + mouse)
* Procedural terrain generation (Perlin noise)
* Creating geometric primitives
* Simple lighting
* File I/O for scene loading and saving
* Config system
* A launcher [here](https://github.com/Haletas033/LSIMLAUNCHER)

## v1.1 Projected Features:
* Way to build games into `.exe` files
* More light types (directional, spotlight)
* Lights in Hierarchy
* A lot of bug fixes
* Some optimizations

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
│   ├── geometry
│   │   ├── mesh.cpp
│   │   ├── primitives.cpp
│   │   └── terrain.cpp
│   ├── gl
│   │   ├── EBO.cpp
│   │   ├── VAO.cpp
│   │   ├── VBO.cpp
│   │   └── shaderClass.cpp
│   ├── inputs
│   │   ├── gui.cpp
│   │   └── inputs.cpp
│   ├── main.cpp
│   ├── scene
│   │   └── camera.cpp
│   └── utils
│       ├── fileIO.cpp
│       ├── json.cpp
│       ├── logging
│       │   └── log.cpp
│       └── texture.cpp
├── config
│   └── config.json
├── include
│   ├── geometry
│   │   ├── mesh.h
│   │   ├── primitives.h
│   │   └── terrain.h
│   ├── gl
│   │   ├── EBO.h
│   │   ├── VAO.h
│   │   ├── VBO.h
│   │   └── shaderClass.h
│   ├── inputs
│   │   ├── gui.h
│   │   └── inputs.h
│   ├── scene
│   │   ├── camera.h
│   │   ├── light.h
│   │   └── scene.h
│   └── utils
│       ├── defaults.h
│       ├── fileIO.h
│       ├── json.h
│       ├── logging
│       │   └── log.h
│       └── texture.h
├── shaders
│   ├── default.frag
│   ├── default.geom
│   └── default.vert
└── tree.txt

17 directories, 41 files
```
<!-- TREE_END -->
## Contributing
Contributions are welcome! Whether it's a bug fix, new feature, or documentation improvement, feel free to open an issue or submit a pull request.

## License

This project is licensed under the [MIT License](https://opensource.org/licenses/MIT).  See the [LICENSE](LICENSE) file for details.
