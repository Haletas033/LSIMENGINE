//
// Created by halet on 9/10/2025.
//

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb/stb_image.h>
#include "include/utils/texture.h"

static Logger logger;

void Texture::InitTextures() {
    logger = Logger("TEXTURE");
    logger("stdInfo", "Successfully initialized the texture loggers");
}

GLenum Texture::ChannelsToFormat(const int nrChannels) {
    if (nrChannels == 1) return GL_RED;
    if (nrChannels == 3) return GL_RGB;
    if (nrChannels == 4) return GL_RGBA;
    throw std::runtime_error("Unknown amount of channels");
}

unsigned int Texture::GetTexId(const char* path, const int texFilter) {
    unsigned int textureID;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    //Set the texture wrapping/filtering options

    const int mipmapTexFilter = texFilter == GL_NEAREST ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmapTexFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texFilter);
    //Load and generate the texture
    int width, height, nrChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        try {
            const GLenum format = ChannelsToFormat(nrChannels);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        catch (std::exception &e) {
            logger("stdError", e.what());
            stbi_image_free(data);
            return 0;
        }
        stbi_image_free(data);
    }
    else {
        logger("stdError", "Failed to load texture");
        return 0;
    }
    return textureID;
}

unsigned int Texture::GetCubemapId(const std::array<std::string, 6>& faces, const int texFilter) {
    unsigned int textureID;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    //Set the texture wrapping/filtering options
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, texFilter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, texFilter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    //Load and generate the texture
    for (int i = 0; i < 6; ++i) {
        int width, height, nrChannels;
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            try {
                const GLenum format = ChannelsToFormat(nrChannels);
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            }
            catch (std::exception &e) {
                logger("stdError", e.what());
                stbi_image_free(data);
                return 0;
            }
            stbi_image_free(data);
        }
        else {
            logger("stdError", "Failed to load texture");
            return 0;
        }
    }
    return textureID;
}

void Texture::ByteArrayToPNG(const char* filename, const unsigned char* texture,
    const unsigned int width, const unsigned int height, const unsigned int channels)
{
    stbi_write_png(filename, width, height, channels, texture, width * channels);
}
