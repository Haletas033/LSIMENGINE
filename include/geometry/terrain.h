#ifndef TERRAIN_CLASS_H
#define TERRAIN_CLASS_H

#include<vector>
#include<ctime>
#include <string>

#include "../../external/FastNoiseLite/FastNoiseLite.h"
#include "glad/glad.h"

//Color struct
struct Color {
    float r, g, b, a;
    Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
};

//Region struct
struct TerrainType {
    std::string name;
    float height;
    Color color;
};

//Generates a 2D noise map using OpenSimplex2 with multiple octaves
std::vector<std::vector<float>> GenerateNoiseMap(int width, int height, int seed, float scale, int octaves, float persistence, float lacunarity);

//Converts a generated noise map into an RGBA opengl Texture
GLuint noiseMapToTexture(std::vector<std::vector<float>>& noiseMap);

void noiseMapToMesh(std::vector<std::vector<float>>& noiseMap, std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, float heightScale, float gridScale);

#endif //TERRAIN_CLASS_H
