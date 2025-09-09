#include "../../include/geometry/terrain.h"

#include<cfloat>
#include<random>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>

//Define regions based on height, color, and name
std::vector<TerrainType> regions = {
    {"Deep Water", 0.3f, Color(0.0f, 0.0f, 0.5f, 1.0f)},
    {"Shallow Water", 0.4f, Color(0.0f, 0.3f, 0.7f, 1.0f)},
    {"Sand", 0.5f, Color(0.9f, 0.8f, 0.6f, 1.0f)},
    {"Grass", 0.7f, Color(0.1f, 0.8f, 0.1f, 1.0f)},
    {"Forest", 0.8f, Color(0.0f, 0.5f, 0.0f, 1.0f)},
    {"Mountain", 0.95f, Color(0.5f, 0.5f, 0.5f, 1.0f)},
    {"Snow", 0.95f, Color(1.0f, 1.0f, 1.0f, 1.0f)}
};

std::vector<std::vector<float>> GenerateNoiseMap(const unsigned int width, const unsigned int height, int seed, float scale, int octaves, float persistence, float lacunarity) {


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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_FILL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_FILL);

    auto* colorMap = new unsigned char[width * height * 4];

    //Loop that creates a texture using the defined regions
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float perlinValue = noiseMap[y][x];
            int index = (y * width + x) * 4;

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

void noiseMapToMesh(std::vector<std::vector<float>> &noiseMap, std::vector<GLfloat> &vertices, std::vector<GLuint> &indices, float heightScale, float gridScale) {
    int width = noiseMap[0].size();
    int height = noiseMap.size();

    vertices.clear();
    indices.clear();

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals(width * height, glm::vec3(0.0f));

    // Step 1: Generate vertex positions (and placeholder normals/UVs)
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float heightValue = noiseMap[y][x];
            float z = 0.0f;

            if (heightValue < 0.4f) {
                z = 0.0f;
            } else if (heightValue < 0.7f) {
                float t = (heightValue - 0.4f) / 0.3f;
                z = pow(t, 1.0f) * (0.2f * heightScale);
            } else {
                float t = (heightValue - 0.7f) / 0.3f;
                z = (0.2f * heightScale) + (std::pow(t, 2.0f) * 0.8f * heightScale);
            }

            positions.push_back(glm::vec3(x * gridScale, z, y * gridScale));
        }
    }

    // Step 2: Create indices and accumulate normals
    for (int y = 0; y < height - 1; y++) {
        for (int x = 0; x < width - 1; x++) {
            int topLeft     = y * width + x;
            int topRight    = topLeft + 1;
            int bottomLeft  = (y + 1) * width + x;
            int bottomRight = bottomLeft + 1;

            glm::vec3 v0 = positions[topLeft];
            glm::vec3 v1 = positions[bottomLeft];
            glm::vec3 v2 = positions[topRight];
            glm::vec3 normal1 = glm::normalize(glm::cross(v1 - v0, v2 - v0));

            normals[topLeft] += normal1;
            normals[bottomLeft] += normal1;
            normals[topRight] += normal1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            glm::vec3 v3 = positions[topRight];
            glm::vec3 v4 = positions[bottomLeft];
            glm::vec3 v5 = positions[bottomRight];
            glm::vec3 normal2 = glm::normalize(glm::cross(v4 - v3, v5 - v3));

            normals[topRight] += normal2;
            normals[bottomLeft] += normal2;
            normals[bottomRight] += normal2;

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    // Step 3: Rebuild final vertices with positions, normals, and UVs
    for (int i = 0; i < positions.size(); i++) {
        glm::vec3 pos = positions[i];
        glm::vec3 norm = glm::normalize(normals[i]);
        float u = static_cast<float>(i % width) / (width - 1);
        float v = static_cast<float>(i / width) / (height - 1);

        vertices.push_back(pos.x);
        vertices.push_back(pos.y);
        vertices.push_back(pos.z);

        vertices.push_back(norm.x);
        vertices.push_back(norm.y);
        vertices.push_back(norm.z);

        vertices.push_back(u);
        vertices.push_back(v);
    }
}
