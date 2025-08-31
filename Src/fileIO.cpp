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

        //Write the transformation matrix
        file.write(reinterpret_cast<const char*>(&mesh->position), sizeof(mesh->position));
        file.write(reinterpret_cast<const char*>(&mesh->rotation), sizeof(mesh->rotation));
        file.write(reinterpret_cast<const char*>(&mesh->scale), sizeof(mesh->scale));

        //Write the model matrix
        file.write(reinterpret_cast<const char*>(&mesh->modelMatrix), sizeof(mesh->modelMatrix));
    }
    file.close();
}

std::vector<std::unique_ptr<Mesh>> IO::loadFromFile(std::ifstream &file) {
    std::vector<std::unique_ptr<Mesh>> meshes;

    while (true) {
        int nameLen;
        int verticesLen;
        int indicesLen;

        //Get name length
        if (!file.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen)))
            break; // stop if we couldn't read (EOF)

        Mesh mesh;


        mesh.name.resize(nameLen);

        //Read name
        file.read(mesh.name.data(), nameLen);

        //Get vertices length
        file.read(reinterpret_cast<char*>(&verticesLen), sizeof(verticesLen));
        mesh.vertices.resize(verticesLen);

        //Read every vertex
        file.read(reinterpret_cast<char*>(mesh.vertices.data()), verticesLen * sizeof(GLfloat));

        //Get indices length
        file.read(reinterpret_cast<char*>(&indicesLen), sizeof(indicesLen));
        mesh.indices.resize(indicesLen);

        //Read every index
        file.read(reinterpret_cast<char*>(mesh.indices.data()), indicesLen * sizeof(GLuint));

        //Read useTexture
        file.read(reinterpret_cast<char*>(&mesh.useTexture), 1);

        //Read color
        file.read(reinterpret_cast<char*>(&mesh.color[0]), 4 * sizeof(float));

        //Read Position, Rotation, and Scale
        file.read(reinterpret_cast<char*>(&mesh.position[0]), 3 * sizeof(float));
        file.read(reinterpret_cast<char*>(&mesh.rotation[0]), 3 * sizeof(float));
        file.read(reinterpret_cast<char*>(&mesh.scale[0]), 3 * sizeof(float));

        //Read model matrix
        file.read(reinterpret_cast<char*>(&mesh.modelMatrix[0][0]), 16 * sizeof(float));

        mesh.setupBuffers();

        meshes.push_back(std::make_unique<Mesh>(mesh));
    }

    return meshes;
}


