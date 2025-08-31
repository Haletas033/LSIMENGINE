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
cd build
```
3. Run cmake to configure the project
```bash
cmake ..
```
4. Build the project using ninja (or another build system)
```bash
ninja
```
5. Run the project
```bash
./LSIM
```
## Project Structure
<!-- TREE_START -->
<!-- TREE_END -->
## Contributing
Contributions are welcome! Whether it's a bug fix, new feature, or documentation improvement, feel free to open an issue or submit a pull request.

## License

This project is licensed under the [MIT License](https://opensource.org/licenses/MIT).  See the [LICENSE](LICENSE) file for details.
