# LSIM ENGINE
**v1.1**\
Documentation is available [here](https://haletas033.github.io/HaletasWeb/LSIMdocs.html)

<img width="1908" height="981" alt="Maze Photo" src="https://github.com/user-attachments/assets/16993191-a488-4359-8955-9331246de6be" />

## Current Features:
* OpenGL rendering pipeline with GLFW windowing
* Support for basic 3D transformations via GLM
* Controllable camera (WASD + mouse)
* Procedural terrain generation (Perlin noise)
* Creating geometric primitives
* File I/O for scene loading and saving
* Config system
* A launcher [here](https://github.com/Haletas033/LSIMLAUNCHER)
## Added in v1.1:
* Model loading
* Mesh picking
* PBR lighting
* More light types (directional, spotlight)
* Emissive map
* Terrain texture saving 
* Backwards compatibility
* Sky-boxes
* Linux support

## Prerequisites:
This project requires Git, Cmake, Ninja(or another build system)
## How to run:
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
│   │   ├── scene.h
│   │   └── script.h
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
│   ├── default.vert
│   └── instance.vert
└── tree.txt

17 directories, 43 files
```
<!-- TREE_END -->
## Contributing
Contributions are welcome! Whether it's a bug fix, new feature, or documentation improvement, feel free to open an issue or submit a pull request.

## License

This project is licensed under the [MIT License](https://opensource.org/licenses/MIT).  See the [LICENSE](LICENSE) file for details.
