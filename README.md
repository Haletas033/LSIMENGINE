# LSIM ENGINE
**v1.0**\
Documentation is available [here](https://haletas033.github.io/HaletasWeb/LSIMdocs.html)

<img width="1908" height="981" alt="Maze Photo" src="https://github.com/user-attachments/assets/16993191-a488-4359-8955-9331246de6be" />

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
* Way to build games into `.exe` files

## v1.1 (light update) Projected Features:
* Sky boxes
* More light types (directional, spotlight)
* Shadows
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
├── Website
│   ├── LSIMdocs
│   │   ├── 001welcome.md
│   │   ├── 002gettingStarted.md
│   │   ├── 003fixingLauncherIssues.md
│   │   ├── 004theBasics.md
│   │   ├── 005movement.md
│   │   ├── 006transformations.md
│   │   ├── 007creations.md
│   │   ├── 008textures.md
│   │   ├── 009fileIO.md
│   │   ├── 010goingFurther.md
│   │   ├── 011workingWithConfigs.md
│   │   ├── 012logger.md
│   │   ├── 013fileIO.md
│   │   ├── 014otherSystems.md
│   │   ├── 015primitives.md
│   │   ├── 016mesh.md
│   │   ├── 017light.md
│   │   ├── 018gui.md
│   │   ├── 019inputs.md
│   │   ├── 020texture.md
│   │   ├── 021programmingYourGame.md
│   │   ├── 022exampleGame.md
│   │   ├── 023theEnd.md
│   │   └── imgs
│   │       ├── after.png
│   │       ├── before.png
│   │       ├── components.png
│   │       ├── configMisconfig.png
│   │       ├── normalExample.jpg
│   │       ├── shaderMisconfig.png
│   │       ├── specularExample.png
│   │       └── success.png
│   └── Src
│       ├── docs
│       │   └── LSIMdocs.h
│       └── styles
│           └── LSIMENGINE_Styles.css.h
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
├── tree.txt
└── website.dsp

23 directories, 77 files
```
<!-- TREE_END -->
## Contributing
Contributions are welcome! Whether it's a bug fix, new feature, or documentation improvement, feel free to open an issue or submit a pull request.

## License

This project is licensed under the [MIT License](https://opensource.org/licenses/MIT).  See the [LICENSE](LICENSE) file for details.
