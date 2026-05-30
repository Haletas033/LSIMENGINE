//
// Created by halet on 9/10/2025.
//

#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <stb/stb_image_write.h>
#include "include/utils/json.h"
#include "include/utils/logging/log.h"

class Texture {
private:
    static GLenum ChannelsToFormat(int nrChannels);
public:
    static void InitTextures();

    static unsigned int GetTexId(const char *path, int texFilter);
    static unsigned int GetCubemapId(const std::array<std::string, 6>& faces, int texFilter);

    static void ByteArrayToPNG(const char *filename, const unsigned char *texture, unsigned int width, unsigned int height, unsigned int channels);
};

#endif //TEXTURE_H
