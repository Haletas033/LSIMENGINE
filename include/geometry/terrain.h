#ifndef TERRAIN_CLASS_H
#define TERRAIN_CLASS_H

#include<vector>
#include<ctime>
#include <string>

#include <FastNoiseLite.h>

#include "meshData.h"
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
    static std::expected<TextureResource*, LSIM::Error> noiseMapToTexture(
        std::vector<std::vector<float>>& noiseMap, const std::string& texName, const char* outputLocation);

    static void noiseMapToMesh(std::vector<std::vector<float>>& noiseMap, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, float heightScale, float gridScale);
};
#endif //TERRAIN_CLASS_H
