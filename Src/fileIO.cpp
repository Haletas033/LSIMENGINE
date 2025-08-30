//
// Created by halet on 8/30/2025.
//

#include <../include/fileIO.h>

void IO::saveToFile(std::ofstream &file, const std::vector<std::unique_ptr<Mesh>> &meshes) {
    for (const auto& mesh : meshes) {

        int nameLen = mesh->name.size();
        int verticesLen = mesh->vertices.size();
        int indicesLen = mesh->indices.size();

        file.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));

        //Write name
        file.write(mesh->name.c_str(), mesh->name.size() * sizeof(char));

        //Write vertices
        file.write(reinterpret_cast<const char*>(&verticesLen), sizeof(verticesLen));
        for (auto vertex : mesh->vertices) {
            file.write(reinterpret_cast<const char*>(&vertex), sizeof(vertex));
        }

        //Write indices
        file.write(reinterpret_cast<const char*>(&indicesLen), sizeof(indicesLen));
        for (auto index : mesh->indices) {
            file.write(reinterpret_cast<const char*>(&index), sizeof(index));
        }

        //Write useTexture
        file.write(reinterpret_cast<const char*>(&mesh->useTexture), sizeof(mesh->useTexture));

        //Write color
        file.write(reinterpret_cast<const char*>(&mesh->color), sizeof(mesh->color));

        //Write VAO
        file.write(reinterpret_cast<const char*>(&mesh->vao), sizeof(mesh->vao));

        //Write the transformation matrix
        file.write(reinterpret_cast<const char*>(&mesh->position), sizeof(mesh->position));
        file.write(reinterpret_cast<const char*>(&mesh->rotation), sizeof(mesh->rotation));
        file.write(reinterpret_cast<const char*>(&mesh->scale), sizeof(mesh->scale));
    }
    file.close();
}

