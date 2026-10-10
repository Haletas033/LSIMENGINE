//
// Created by halet on 8/30/2025.
//

#ifndef FILEIO_CLASS_H
#define FILEIO_CLASS_H

#include <fstream>
#include <utility>
#include <memory>

#include <glm/glm.hpp>

class IO {
public:
    static std::string DirectoryDialog();
    static std::string OpenDialog(const char* filter);
    static std::string SaveDialog(const char* filter);

    static std::string GetFileContents(const std::string& filePath);

};

#endif //FILEIO_CLASS_H
