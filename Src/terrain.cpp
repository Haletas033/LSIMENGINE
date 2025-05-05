#include"../include/terrain.h"

#include <cfloat>
#include <random>

//Define regions based on height, color, and name
std::vector<TerrainType> regions = {
    {"Deep Water", 0.3f, Color(0.0f, 0.0f, 0.5f, 1.0f)},
    {"Shallow Water", 0.4f, Color(0.0f, 0.3f, 0.7f, 1.0f)},
    {"Sand", 0.5f, Color(0.9f, 0.8f, 0.6f, 1.0f)},
    {"Grass", 0.7f, Color(0.1f, 0.8f, 0.1f, 1.0f)},
    {"Forest", 0.8f, Color(0.0f, 0.5f, 0.0f, 1.0f)},
    {"Mountain", 0.9f, Color(0.5f, 0.5f, 0.5f, 1.0f)},
    {"Snow", 1.0f, Color(1.0f, 1.0f, 1.0f, 1.0f)}
};

std::vector<std::vector<float>> GenerateNoiseMap(int width, int height, int seed, float scale, int octaves, float persistence, float lacunarity) {


    //Initialize the noise generator with an OpenSimplex2 noise.
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFrequency(1.0f / scale);  //Makes the frequency the inverse of the scale
    noise.SetSeed(seed);

    //Random distribution for octave offsets
    std::vector<std::pair<float, float>> octaveOffsets(octaves);
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> dist(-100000.0f, 100000.0f);

    for (int i = 0; i < octaves; i++) {
        octaveOffsets[i] = {dist(rng), dist(rng)};
    }

    std::vector<std::vector<float>> noiseMap(height, std::vector<float>(width));

    //Prevent a divide-by-zero error if scale is equal to zero
    if (scale <= 0) {
        scale = 0.0001f;
    }
    //Initialize the maximum and minimum noise sizes for normalization
    float maxNoise = -FLT_MAX;
    float minNoise = FLT_MAX;

    //Generate multi-octave noise values for each (x, y) point
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            float amplitude = 1.0f;
            float frequency = 1.0f;
            float noiseHeight = 0.0f;

            for (int i = 0; i < octaves; i++) {
                //Sample the noise value at a certain position
                float sampleX = x / scale * frequency + octaveOffsets[i].first;
                float sampleY = y / scale * frequency + octaveOffsets[i].second;

                float PerlinValue = noise.GetNoise(sampleX, sampleY);
                noiseHeight += PerlinValue * amplitude;

                //Change the default amplitude and frequency values by the persistence and lacunarity
                amplitude *= persistence;
                frequency *= lacunarity;
            }
            //Edit the max and min range for normalization
            if (noiseHeight < minNoise) minNoise = noiseHeight;
            if (noiseHeight > maxNoise) maxNoise = noiseHeight;

            //Generate the unnormalized noiseMap
            noiseMap[y][x] = noiseHeight;
        }
    }
    //Normalize the noiseMap
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

    //Generate and bind a new OpenGL 2D texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    //Allocate texture storage with RGBA channels
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    auto* colorMap = new unsigned char[width * height * 4];

    //Loop that creates a texture using the defined regions
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float perlinValue = noiseMap[y][x];
            int index = (y * height + x) * 4;

            unsigned char r = 255, g = 255, b = 255, a = 255;

            //Assign the color value based on the perlinValue at its position
            for (int i = 0; i < regions.size(); i++) {
                if (perlinValue <= regions[i].height) {
                    r = static_cast<unsigned char>(regions[i].color.r * 255.0f);
                    g = static_cast<unsigned char>(regions[i].color.g * 255.0f);
                    b = static_cast<unsigned char>(regions[i].color.b * 255.0f);
                    a = static_cast<unsigned char>(regions[i].color.a * 255.0f);
                    break;
                }
            }

            //Set the R, G, B and A values of the texel
            colorMap[index + 0] = r;
            colorMap[index + 1] = g;
            colorMap[index + 2] = b;
            colorMap[index + 3] = a;
        }
    }
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, colorMap);

    //Delete the colorMap to free up memory
    delete[] colorMap;

    return textureID;
}
