//
// Created by halet on 8/30/2025.
//

#ifndef FILEIO_CLASS_H
#define FILEIO_CLASS_H

#include <fstream>
#include <utility>
#include <memory>
#include <windows.h>
#include <commdlg.h>

#include <include/utils/logging/log.h>
#include "../scene/scene.h"
#include "../inputs/gui.h"

#include "glad/glad.h"
#include "glm/glm.hpp"

class IO {
public:
    typedef BOOL (__stdcall *FileDialogFunc)(LPOPENFILENAMEA);

    static void InitIO();
    static std::string Dialog(const char* filter, FileDialogFunc func);
    static void saveToFile(std::ofstream &file, const Scene &scene);
    static Scene loadFromFile(std::ifstream &file);
};

#endif //FILEIO_CLASS_H
