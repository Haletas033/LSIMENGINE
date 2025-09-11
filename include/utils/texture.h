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
};

#endif //TEXTURE_H
