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
├── external
│   └── glad
├── .git
│   ├── hooks
│   ├── info
│   ├── logs
│   ├── objects
│   ├── refs
│   ├── COMMIT_EDITMSG
│   ├── config
│   ├── description
│   ├── HEAD
│   └── index
├── include
│   ├── camera.h
│   ├── EBO.h
│   ├── shaderClass.h
│   ├── VAO.h
│   └── VBO.h
├── shaders
│   ├── default.frag
│   └── default.vert
├── Src
│   ├── camera.cpp
│   ├── EBO.cpp
│   ├── main.cpp
│   ├── shaderClass.cpp
│   ├── VAO.cpp
│   └── VBO.cpp
├── CMakeLists.txt
└── .gitignore

```

## Contributing
Contributions are welcome! Whether it's a bug fix, new feature, or documentation improvement, feel free to open an issue or submit a pull request.

## License

This project is licensed under the [MIT License](https://opensource.org/licenses/MIT).  See the [LICENSE](LICENSE) file for details.
