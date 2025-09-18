//
// Created by halet on 8/30/2025.
//

#include <include/utils/fileIO.h>

#include "include/utils/json.h"
#include "include/utils/texture.h"

OPENFILENAME ofn;                           //common dialog box structure
char szFile[260] = {"untitled.lsim"};       //File size buffer
HWND hwnd;                                  //owner window

extern json config;

extern std::vector<Logger> logs;

static std::unordered_map<std::string, std::unique_ptr<Logger>> loggers;

static void Log(const std::string& key, const std::string& msg) {
    if (const auto it = loggers.find(key); it != loggers.end())
        (*it->second)(msg, logs);
}

void IO::InitIO() {
    JSONManager::LoadLoggers(config, loggers);

    // Ensure logger exists
    if (!loggers.count("stdInfo"))
        loggers["stdInfo"] = std::make_unique<Logger>();

    loggers["stdInfo"]->SetModule("FILE");
    loggers["stdWarn"]->SetModule("FILE");
    loggers["stdError"]->SetModule("FILE");

    Log("stdInfo", "Successfully initialized the file loggers");
}


std::string IO::Dialog(const char *filter, const FileDialogFunc func) {
    Log("stdInfo", "Initializing file dialog");
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
    Log("stdInfo", "Displaying file dialog");
    if (func(&ofn) == TRUE) {
        return ofn.lpstrFile;
    }
    Log("stdInfo", "File dialog closed without selecting a file");
    return {};
}


void IO::saveToFile(std::ofstream &file, const Scene& scene) {
    std::cout << std::endl;
    Log("stdInfo", "Beginning to write to file");

    auto safeWrite = [&](const auto* data, const std::streamsize size, const char* errorMsg) {
        if (!file.write(reinterpret_cast<const char*>(data), size)) {
            throw std::ios_base::failure(errorMsg);
        }
    };

    try {
        const int meshCount = scene.meshes.size();
        safeWrite(&meshCount, sizeof(meshCount), "Failed to write mesh count");

        uint16_t NextMeshID = 0;

        for (auto& mesh : scene.meshes) {
            mesh->meshID = NextMeshID;
            NextMeshID++;
        }

        for (const auto& mesh : scene.meshes) {
            int nameLen = mesh->name.size();
            int verticesLen = mesh->vertices.size();
            int indicesLen = mesh->indices.size();
            int texturePathLen = mesh->texturePath.size();
            int specMapPathLen = mesh->specMapPath.size();
            int normalMapPathLen = mesh->normalMapPath.size();

            safeWrite(&nameLen, sizeof(nameLen), "Failed to write name length");

            //Write name
            safeWrite(mesh->name.c_str(), mesh->name.size() * sizeof(char), "Failed to write namee");

            //Write vertices
            safeWrite(&verticesLen, sizeof(verticesLen), "Failed to write verticesLen");
            safeWrite(mesh->vertices.data(), verticesLen * sizeof(mesh->vertices[0]), "Failed to write vertices");

            //Write indices
            safeWrite(&indicesLen, sizeof(indicesLen), "Failed to write indicesLen");
            safeWrite(mesh->indices.data(), indicesLen * sizeof(mesh->indices[0]), "Failed to write indices");

            //Write useTexture
            safeWrite(&mesh->useTexture, sizeof(mesh->useTexture), "Failed to write useTexture");

            //Write useNormalMap
            safeWrite(&mesh->useNormalMap, sizeof(mesh->useNormalMap), "Failed to write useNormalMap");

            //Write texturePath
            safeWrite(&texturePathLen, sizeof(texturePathLen), "Failed to write texturePathLen");
            safeWrite(mesh->texturePath.data(), texturePathLen * sizeof(char), "Failed to write texturePath");

            //Write specMapPath
            safeWrite(&specMapPathLen, sizeof(specMapPathLen), "Failed to write specMapPathLen");
            safeWrite(mesh->specMapPath.data(), specMapPathLen * sizeof(char), "Failed to write specMapPath");

            //Write normalMapPath
            safeWrite(&normalMapPathLen, sizeof(normalMapPathLen), "Failed to write normalMapPathLen");
            safeWrite(mesh->normalMapPath.data(), normalMapPathLen * sizeof(char), "Failed to write normalMapPath");

            //Write color
            safeWrite(&mesh->color, sizeof(mesh->color), "Failed to write colour");

            //Write meshID
            safeWrite(&mesh->meshID, sizeof(mesh->meshID), "Failed to write meshID");

            //Write the meshID of the parent node
            uint16_t parentID = -1;
            if (const Gui::Node* node = Gui::FindNodeByMesh(Gui::root, mesh.get());
                node->parent && node->parent != Gui::root) {
                parentID = node->parent->mesh->meshID;
            }

            safeWrite(&parentID, sizeof(parentID), "Failed to write parentID");

            //Write the transformation matrix
            safeWrite(&mesh->position, sizeof(mesh->position), "Failed to write position");
            safeWrite(&mesh->rotation, sizeof(mesh->rotation), "Failed to write rotation");
            safeWrite(&mesh->scale, sizeof(mesh->scale), "Failed to write scale");

            //Write the model matrix
            safeWrite(&mesh->modelMatrix, sizeof(mesh->modelMatrix), "Failed to write model matrix");
        }

        const int lightCount = scene.lights.size();
        safeWrite(&lightCount, sizeof(lightCount), "Failed to write lightCount");

        for (Light light : scene.lights) {
            safeWrite(&light.lightPos, sizeof(light.lightPos), "Failed to write light position");
            safeWrite(&light.lightColor, sizeof(light.lightColor), "Failed to write light colour");
            safeWrite(&light.attenuationScale, sizeof(light.attenuationScale), "Failed to write attenuation scale");
        }
    } catch (std::ios_base::failure &e) {
        Log("stdError", e.what());
    }

    Log("stdInfo", "Successfully wrote to file");
    std::cout << std::endl;
}

Scene IO::loadFromFile(std::ifstream &file, const std::string &workingDir) {
    std::cout << std::endl;
    Log("stdInfo", "beginning to read from file");

    Gui::ClearRoot();

    std::vector<std::unique_ptr<Mesh>> meshes;
    std::vector<Light> lights;

    auto safeRead = [&](auto* data, const std::streamsize size, const char* errorMsg) {
        if (!file.read(reinterpret_cast<char*>(data), size)) {
            throw std::ios_base::failure(errorMsg);
        }
    };

    try {
        int meshCount;
        safeRead(&meshCount, sizeof(meshCount), "Failed to read mesh count");

        for (int i = 0; i < meshCount; ++i) {
            int nameLen;
            int verticesLen;
            int indicesLen;
            int texturePathLen;
            int specMapPathLen;
            int normalMapPathLen;

            //Get name length
            safeRead(&nameLen, sizeof(nameLen), "Failed to read name len");
            if (nameLen < 0 || nameLen > 1024) {
                throw std::ios_base::failure("Invalid name length");
            }

            Mesh mesh;


            mesh.name.resize(nameLen);

            //Read name
            safeRead(mesh.name.data(), nameLen, "Failed to read name");

            //Get vertices length
            safeRead(&verticesLen, sizeof(verticesLen), "Failed to read verticesLen");
            mesh.vertices.resize(verticesLen);

            //Read every vertex
            safeRead(mesh.vertices.data(), verticesLen * sizeof(GLfloat), "Failed to read vertices");

            //Get indices length
            safeRead(&indicesLen, sizeof(indicesLen), "Failed to read indicesLen");
            mesh.indices.resize(indicesLen);

            //Read every index
            safeRead(mesh.indices.data(), indicesLen * sizeof(GLuint), "Failed to read indices");

            //Read useTexture
            safeRead(&mesh.useTexture, 1, "Failed to read useTexture");

            //Read useNormalMap
            safeRead(&mesh.useNormalMap, 1, "Failed to read useNormalMap");

            //Read texturePath
            safeRead(&texturePathLen, sizeof(texturePathLen), "Failed to read texturePathLen");
            mesh.texturePath.resize(texturePathLen);
            safeRead(mesh.texturePath.data(), texturePathLen, "Failed to read texturePathLen");

            //Read specMapPath
            safeRead(&specMapPathLen, sizeof(specMapPathLen), "Failed to read specMapPathLen");
            mesh.specMapPath.resize(specMapPathLen);
            safeRead(mesh.specMapPath.data(), specMapPathLen, "Failed to read specMapPathLen");

            //Read normalMapPath
            safeRead(&normalMapPathLen, sizeof(normalMapPathLen), "Failed to read normalMapPathLen");
            mesh.normalMapPath.resize(normalMapPathLen);
            safeRead(mesh.normalMapPath.data(), normalMapPathLen, "Failed to read normalMapPathLen");

            //Load tex IDs
            mesh.texId = Texture::GetTexId((std::string(workingDir + "resources/") + mesh.texturePath.data()).c_str());
            mesh.specMapId = Texture::GetTexId((std::string(workingDir + "resources/") + mesh.specMapPath.data()).c_str());
            mesh.normalMapId = Texture::GetTexId((std::string(workingDir + "resources/") + mesh.normalMapPath.data()).c_str());

            //Read color
            safeRead(&mesh.color[0], 4 * sizeof(float), "Failed to read colour");

            //Read meshID
            safeRead(&mesh.meshID, sizeof(mesh.meshID), "Failed to read meshID");

            //Read parentID
            uint16_t parentID;
            safeRead(&parentID, sizeof(parentID), "Failed to read parentID");

            //Read Position, Rotation, and Scale
            safeRead(&mesh.position[0], 3 * sizeof(float), "Failed to read position");
            safeRead(&mesh.rotation[0], 3 * sizeof(float), "Failed to read rotation");
            safeRead(&mesh.scale[0], 3 * sizeof(float), "Failed to read scale");

            //Read model matrix
            safeRead(&mesh.modelMatrix[0][0], 16 * sizeof(float), "Failed to read model matrix");

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
        safeRead(&lightCount, sizeof(lightCount), "Failed to read ligth count");

        for (int i = 0; i < lightCount; ++i) {
            Light light;

            safeRead(&light.lightPos[0], 3 * sizeof(float), "Failed to read light position");
            safeRead(&light.lightColor[0], 4 * sizeof(float), "Failed to read light colour");
            safeRead(&light.attenuationScale, sizeof(light.attenuationScale), "Failed to read attenuation scale");

            lights.push_back(light);
        }
    } catch (std::ios_base::failure &e) {
        Log("stdError", e.what());
        return Scene{std::vector<std::unique_ptr<Mesh>>{}, std::vector<Light>{}};
    }

    Log("stdInfo", "Successfully read file");
    std::cout << std::endl;

    return Scene {std::move(meshes), std::move(lights)};
}


