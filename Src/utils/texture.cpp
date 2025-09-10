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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        Log("stdError", "Failed to load texture");
    }
    stbi_image_free(data);

    return textureID;
}
