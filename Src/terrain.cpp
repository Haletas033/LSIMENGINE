#include"../include/terrain.h"

#include <cfloat>
#include <random>

std::vector<std::vector<float>> GenerateNoiseMap(int width, int height, int seed, float scale, int octaves, float persistence, float lacunarity) {

    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFrequency(1.0f / scale);  // frequency is 1 / scale
    noise.SetSeed(seed);

    std::vector<std::pair<float, float>> octaveOffsets(octaves);
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> dist(-100000.0f, 100000.0f);

    for (int i = 0; i < octaves; i++) {
        octaveOffsets[i] = {dist(rng), dist(rng)};
    }

    std::vector<std::vector<float>> noiseMap(height, std::vector<float>(width));

    if (scale <= 0) {
        scale = 0.0001f;
    }

    float maxNoise = -FLT_MAX;
    float minNoise = FLT_MAX;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            float amplitude = 1.0f;
            float frequency = 1.0f;
            float noiseHeight = 0.0f;

            for (int i = 0; i < octaves; i++) {
                float sampleX = x / scale * frequency + octaveOffsets[i].first;
                float sampleY = y / scale * frequency + octaveOffsets[i].second;

                float PerlinValue = noise.GetNoise(sampleX, sampleY) * 2 - 1;
                noiseHeight += PerlinValue * amplitude;

                amplitude *= persistence;
                frequency *= lacunarity;
            }
            if (noiseHeight < minNoise) minNoise = noiseHeight;
            if (noiseHeight > maxNoise) maxNoise = noiseHeight;

            noiseMap[y][x] = noiseHeight;
        }
    }
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            noiseMap[y][x] = (noiseMap[y][x] - minNoise) / (maxNoise - minNoise);
        }
    }

    return noiseMap;

}

GLuint noiseMapToTexture(std::vector<std::vector<float>>& noiseMap) {
    int width = noiseMap.size();
    int height = noiseMap[0].size();

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    auto* colorMap = new unsigned char[width * height * 4];

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float perlinValue = noiseMap[y][x];

            auto color = static_cast<unsigned char>(perlinValue * 255.0f);

            int index = (y * width + x) * 4;
            colorMap[index + 0] = color;  //R
            colorMap[index + 1] = color;  //G
            colorMap[index + 2] = color;  //B
            colorMap[index + 3] = 255;    //A
        }
    }
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, colorMap);

    delete[] colorMap;

    return textureID;
}
