//
// Created by halet on 9/10/2025.
//

#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

#include "include/utils/json.h"
#include "include/utils/logging/log.h"

class Texture {
public:
    static void InitTextures();
    static unsigned int GetTexId(const char* path);
    static unsigned int GetCubemapId(std::string faces[6]);
};

#endif //TEXTURE_H
