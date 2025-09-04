//
// Created by halet on 8/30/2025.
//

#ifndef FILEIO_CLASS_H
#define FILEIO_CLASS_H

#include <fstream>
#include <memory>
#include <windows.h>
#include <commdlg.h>

#include "mesh.h"
#include "gui.h"
#include "glad/glad.h"
#include "glm/glm.hpp"

class IO {
public:
    typedef BOOL (__stdcall *FileDialogFunc)(LPOPENFILENAMEA);

    static std::string Dialog(const char* filter, FileDialogFunc func);
    static void saveToFile(std::ofstream &file, const std::vector<std::unique_ptr<Mesh>> &meshes);
    static std::vector<std::unique_ptr<Mesh>> loadFromFile(std::ifstream &file);
};

#endif //FILEIO_CLASS_H
