//
// Created by halet on 9/10/2025.
//

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "include/utils/texture.h"

extern json config;

extern std::vector<Logger> logs;

static std::unordered_map<std::string, std::unique_ptr<Logger>> loggers;

static void Log(const std::string& key, const std::string& msg) {
    if (const auto it = loggers.find(key); it != loggers.end())
        (*it->second)(msg, logs);
}

void Texture::InitTextures() {
    JSONManager::LoadLoggers(config, loggers);

    // Ensure logger exists
    if (!loggers.count("stdInfo"))
        loggers["stdInfo"] = std::make_unique<Logger>();

    loggers["stdInfo"]->SetModule("TEXTURE");
    loggers["stdWarn"]->SetModule("TEXTURE");
    loggers["stdError"]->SetModule("TEXTURE");

    Log("stdInfo", "Successfully initialized the texture loggers");
}

unsigned int Texture::GetTexId(const char* path) {
    unsigned int textureID;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    //Set the texture wrapping/filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //Load and generate the texture
    int width, height, nrChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        try {
            GLenum format;
            if (nrChannels == 1) format = GL_RED;
            else if (nrChannels == 3) format = GL_RGB;
            else if (nrChannels == 4) format = GL_RGBA;
            else throw std::ios_base::failure("Unknown amount of channels");

            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        catch (std::exception &e) {
            Log("stdError", e.what());
        }
    }
    else
    {
        Log("stdError", "Failed to load texture");
    }
    stbi_image_free(data);

    return textureID;
}

unsigned int Texture::GetCubemapId(std::string faces[6]) {
    unsigned int textureID;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    //Set the texture wrapping/filtering options
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    //Load and generate the texture
    for (int i = 0; i < 6; ++i) {
        int width, height, nrChannels;
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            try {
                GLenum format;
                if (nrChannels == 1) format = GL_RED;
                else if (nrChannels == 3) format = GL_RGB;
                else if (nrChannels == 4) format = GL_RGBA;
                else throw std::ios_base::failure("Unknown amount of channels");

                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            }
            catch (std::exception &e) {
                Log("stdError", e.what());
            }
        }
        else
        {
            Log("stdError", "Failed to load texture");
        }
        stbi_image_free(data);
    }
    return textureID;
}


