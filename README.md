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
│   ├── ECS
│   │   ├── entityManager.cpp
│   │   └── name.traits.cpp
│   ├── editor
│   │   ├── IOInputs.cpp
│   │   ├── editorGUI.cpp
│   │   ├── editorInputs.cpp
│   │   ├── meshInputs.cpp
│   │   └── sharedState.cpp
│   ├── engine.cpp
│   ├── geometry
│   │   ├── mesh.cpp
│   │   ├── meshData.cpp
│   │   ├── meshPool.cpp
│   │   ├── model.cpp
│   │   ├── primitive.cpp
│   │   ├── terrain.cpp
│   │   ├── transform.cpp
│   │   └── transform.traits.cpp
│   ├── inputs
│   │   ├── gui.cpp
│   │   └── inputs.cpp
│   ├── main.cpp
│   ├── platform
│   │   └── window.cpp
│   ├── rendering
│   │   ├── light.cpp
│   │   ├── light.traits.cpp
│   │   ├── material.cpp
│   │   ├── material.traits.cpp
│   │   ├── meshRenderer.traits.cpp
│   │   └── renderSystem.cpp
│   ├── resources
│   │   └── resourceManager.cpp
│   ├── scene
│   │   ├── camera.cpp
│   │   └── scene.cpp
│   ├── utils
│   │   ├── fileIO.cpp
│   │   ├── json.cpp
│   │   ├── logging
│   │   │   └── log.cpp
│   │   ├── serialization.cpp
│   │   └── texture.cpp
│   └── vk
│       ├── buffer.cpp
│       ├── command.cpp
│       ├── context.cpp
│       ├── device.cpp
│       ├── framebuffer.cpp
│       ├── pipeline.cpp
│       ├── renderPass.cpp
│       └── swapchain.cpp
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
├── cmake
│   ├── ECS.cmake
│   ├── editor.cmake
│   ├── engine.cmake
│   ├── geometry.cmake
│   ├── inputs.cmake
│   ├── platform.cmake
│   ├── rendering.cmake
│   ├── resources.cmake
│   ├── scene.cmake
│   ├── shaderCompilation.cmake
│   ├── utils.cmake
│   └── vk.cmake
├── config
│   └── config.json
├── include
│   ├── ECS
│   │   ├── componentTraits.h
│   │   ├── entityManager.h
│   │   ├── handle.h
│   │   ├── name.h
│   │   ├── name.traits.h
│   │   ├── nameSystem.h
│   │   ├── registry.h
│   │   ├── system.h
│   │   └── systemManager.h
│   ├── LSIMhelpers.h
│   ├── LSIMtypes.h
│   ├── editor
│   │   ├── IOInputs.h
│   │   ├── editorGUI.h
│   │   ├── editorInputs.h
│   │   ├── meshInputs.h
│   │   └── sharedState.h
│   ├── engine.h
│   ├── engineContext.h
│   ├── geometry
│   │   ├── mesh.h
│   │   ├── meshData.h
│   │   ├── meshPool.h
│   │   ├── model.h
│   │   ├── primitive.h
│   │   ├── terrain.h
│   │   ├── transform.h
│   │   ├── transform.traits.h
│   │   └── transformSystem.h
│   ├── inputs
│   │   ├── gui.h
│   │   ├── inputs.h
│   │   ├── keyDispatch.h
│   │   ├── keyDispatcherUndef.h
│   │   └── keys.def
│   ├── platform
│   │   └── window.h
│   ├── rendering
│   │   ├── light.h
│   │   ├── light.traits.h
│   │   ├── material.h
│   │   ├── material.traits.h
│   │   ├── meshRenderer.h
│   │   ├── meshRenderer.traits.h
│   │   └── renderSystem.h
│   ├── resources
│   │   └── resourceManager.h
│   ├── scene
│   │   ├── camera.h
│   │   ├── scene.h
│   │   └── script.h
│   ├── utils
│   │   ├── defaults.h
│   │   ├── fileIO.h
│   │   ├── json.h
│   │   ├── logging
│   │   │   ├── colorCodes.def
│   │   │   └── log.h
│   │   ├── meshPicking.h
│   │   ├── serialization.h
│   │   └── texture.h
│   └── vk
│       ├── buffer.h
│       ├── command.h
│       ├── context.h
│       ├── device.h
│       ├── framebuffer.h
│       ├── pipeline.h
│       ├── renderPass.h
│       ├── swapchain.h
│       └── vk.h
├── shaders
│   ├── test.frag
│   └── test.vert
├── skybox
│   ├── back.jpg
│   ├── bottom.jpg
│   ├── front.jpg
│   ├── left.jpg
│   ├── right.jpg
│   └── top.jpg
├── tree.txt
└── website.dsp

35 directories, 163 files
```
<!-- TREE_END -->
## Contributing
Contributions are welcome! Whether it's a bug fix, new feature, or documentation improvement, feel free to open an issue or submit a pull request.

## License

This project is licensed under the [MIT License](https://opensource.org/licenses/MIT).  See the [LICENSE](LICENSE) file for details.
