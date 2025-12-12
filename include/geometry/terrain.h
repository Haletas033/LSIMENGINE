#ifndef TERRAIN_CLASS_H
#define TERRAIN_CLASS_H

#include<vector>
#include<ctime>
#include <string>

#include "../../external/FastNoiseLite/FastNoiseLite.h"
#include "glad/glad.h"

#include "../utils/texture.h"

class Terrain {
private:
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
public:
    static std::vector<TerrainType> regions;

    //Generates a 2D noise map using OpenSimplex2 with multiple octaves
    static std::vector<std::vector<float>> GenerateNoiseMap(unsigned int width, unsigned int height, int seed, float scale, int octaves, float persistence, float lacunarity);

    //Converts a generated noise map into an RGBA opengl Texture
    static GLuint noiseMapToTexture(std::vector<std::vector<float>> &noiseMap, const char *outputLocation);

    static void noiseMapToMesh(std::vector<std::vector<float>>& noiseMap, std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, float heightScale, float gridScale);
};
#endif //TERRAIN_CLASS_H
