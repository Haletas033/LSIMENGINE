//
// Created by halet on 8/30/2025.
//

#ifndef FILEIO_CLASS_H
#define FILEIO_CLASS_H

#include <fstream>
#include <utility>
#include <memory>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#include <commdlg.h>
#endif

#include <include/utils/logging/log.h>
#include "../scene/scene.h"
#include "../inputs/gui.h"

#include "glad/glad.h"
#include "glm/glm.hpp"

class IO {
public:
#ifdef _WIN32
    typedef BOOL (__stdcall *FileDialogFunc)(LPOPENFILENAMEA);
#endif
    static void InitIO();
#ifdef _WIN32
    static std::string Dialog(const char* filter, FileDialogFunc func);
#else
    static std::string Dialog(const char* filter);
#endif
    static std::string DirectoryDialog();
    static std::string OpenDialog(const char* filter);
    static std::string SaveDialog(const char* filter);
    static void saveToFile(std::ofstream &file, const Scene &scene);
    static Scene loadFromFile(std::ifstream &file, const std::string &workingDir);
};

#endif //FILEIO_CLASS_H
