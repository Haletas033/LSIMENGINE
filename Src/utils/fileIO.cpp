//
// Created by halet on 8/30/2025.
//

#include "../../include/utils/fileIO.h"

OPENFILENAME ofn;                           //common dialog box structure
char szFile[260] = {"untitled.lsim"};       //File size buffer
HWND hwnd;                                  //owner window

std::string IO::Dialog(const char *filter, const FileDialogFunc func) {
    //Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;

    //Display the dialog box
    if (func(&ofn) == TRUE) {
        return ofn.lpstrFile;
    }

    return {};
}


void IO::saveToFile(std::ofstream &file, const Scene& scene) {
    const int meshCount = scene.meshes.size();
    file.write(reinterpret_cast<const char*>(&meshCount), sizeof(meshCount));

    uint16_t NextMeshID = 0;

    for (auto& mesh : scene.meshes) {
        mesh->meshID = NextMeshID;
        NextMeshID++;
    }

    for (const auto& mesh : scene.meshes) {

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

        //Write meshID
        file.write(reinterpret_cast<const char *>(&mesh->meshID), sizeof(mesh->meshID));

        //Write the meshID of the parent node
        uint16_t parentID = -1;
        if (const Gui::Node* node = Gui::FindNodeByMesh(Gui::root, mesh.get());
            node->parent && node->parent != Gui::root) {
            parentID = node->parent->mesh->meshID;
        }

        file.write(reinterpret_cast<const char*>(&parentID), sizeof(parentID));

        //Write the transformation matrix
        file.write(reinterpret_cast<const char*>(&mesh->position), sizeof(mesh->position));
        file.write(reinterpret_cast<const char*>(&mesh->rotation), sizeof(mesh->rotation));
        file.write(reinterpret_cast<const char*>(&mesh->scale), sizeof(mesh->scale));

        //Write the model matrix
        file.write(reinterpret_cast<const char*>(&mesh->modelMatrix), sizeof(mesh->modelMatrix));
    }

    const int lightCount = scene.lights.size();
    file.write(reinterpret_cast<const char*>(&lightCount), sizeof(lightCount));

    for (Light light : scene.lights) {
        file.write(reinterpret_cast<const char*>(&light.lightPos), sizeof(light.lightPos));
        file.write(reinterpret_cast<const char*>(&light.lightColor), sizeof(light.lightColor));
        file.write(reinterpret_cast<const char*>(&light.attenuationScale), sizeof(light.attenuationScale));
    }
    file.close();
}

Scene IO::loadFromFile(std::ifstream &file) {
    Gui::ClearRoot();

    std::vector<std::unique_ptr<Mesh>> meshes;
    std::vector<Light> lights;

    int meshCount;
    file.read(reinterpret_cast<char*>(&meshCount), sizeof(meshCount));

    for (int i = 0; i < meshCount; ++i) {
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

        //Read meshID
        file.read(reinterpret_cast<char*>(&mesh.meshID), sizeof(mesh.meshID));

        //Read parentID
        uint16_t parentID;
        file.read(reinterpret_cast<char*>(&parentID), sizeof(parentID));

        //Read Position, Rotation, and Scale
        file.read(reinterpret_cast<char*>(&mesh.position[0]), 3 * sizeof(float));
        file.read(reinterpret_cast<char*>(&mesh.rotation[0]), 3 * sizeof(float));
        file.read(reinterpret_cast<char*>(&mesh.scale[0]), 3 * sizeof(float));

        //Read model matrix
        file.read(reinterpret_cast<char*>(&mesh.modelMatrix[0][0]), 16 * sizeof(float));

        mesh.setupBuffers();

        //Recreate mesh hierarchy
        auto* node = new Gui::Node;
        auto meshPtr = std::make_unique<Mesh>(mesh);
        node->mesh = meshPtr.get();
        Gui::Node* parentNode = Gui::FindNodeByMeshID(Gui::root, parentID);
        if (!parentNode) parentNode = Gui::root;

        node->parent = parentNode;
        parentNode->children.push_back(node);

        meshes.push_back(std::move(meshPtr));
    }

    int lightCount;
    file.read(reinterpret_cast<char*>(&lightCount), sizeof(lightCount));

    for (int i = 0; i < lightCount; ++i) {
        Light light;

        file.read(reinterpret_cast<char*>(&light.lightPos[0]), 3 * sizeof(float));
        file.read(reinterpret_cast<char*>(&light.lightColor[0]), 4 * sizeof(float));
        file.read(reinterpret_cast<char*>(&light.attenuationScale), sizeof(light.attenuationScale));

        lights.push_back(light);
    }

    return Scene {std::move(meshes), std::move(lights)};

}


