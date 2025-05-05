#ifndef TERRAIN_CLASS_H
#define TERRAIN_CLASS_H

#include<vector>
#include<ctime>

#include"FastNoiseLite.h"
#include "glad/glad.h"

//Generates a 2D noise map using OpenSimplex2 with multiple octaves
std::vector<std::vector<float>> GenerateNoiseMap(int width, int height, int seed, float scale, int octaves, float persistence, float lacunarity);

//Converts a generated noise map into an RGBA opengl Texture
GLuint noiseMapToTexture(std::vector<std::vector<float>>& noiseMap);

#endif //TERRAIN_CLASS_H
