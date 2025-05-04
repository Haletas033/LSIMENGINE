#ifndef TERRAIN_CLASS_H
#define TERRAIN_CLASS_H

#include<vector>
#include<ctime>

#include"FastNoiseLite.h"
#include "glad/glad.h"

std::vector<std::vector<float>> GenerateNoiseMap(int width, int height, int seed, float scale, int octaves, float persistence, float lacunarity);

GLuint noiseMapToTexture(std::vector<std::vector<float>>& noiseMap);

#endif //TERRAIN_CLASS_H
