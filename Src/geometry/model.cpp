//
// Created by halet on 10/2/2025.
//

#include "include/geometry/model.h"
#include "scene/scene.h"
#include "utils/texture.h"

extern Scene scene;

Model::Model(const char *file) {
    std::string text = getFileContent(file);
    JSON = json::parse(text);

    Model::file = file;
    data = getData();

    //Traverse all nodes in the root and their children recursively
    const unsigned int sceneIndex = JSON.value("scene", 0);
    json sceneNodes = JSON["scenes"][sceneIndex]["nodes"];
    for (const auto &nodeIndex : sceneNodes) {
        TraverseNode(nodeIndex, glm::mat4(1.0f));
    }
}

std::vector<unsigned char> Model::getData() {
    const std::string uri = JSON["buffers"][0]["uri"];

    const auto fileStr = std::string(file);
    const std::string fileDir = fileStr.substr(0, fileStr.find_last_of('\\') + 1);
    std::string bytesText = getFileContent((fileDir + uri).c_str());

    std::vector<unsigned char> data(bytesText.begin(), bytesText.end());
    return data;
}


std::vector<float> Model::getFloats(json accessor) {
    std::vector<float> floatVec;

    const unsigned int buffViewInd = accessor.value("bufferView", 0);
    const unsigned int count = accessor["count"];
    const unsigned int accByteOffset = accessor.value("byteOffset", 0);
    std::string type = accessor["type"];

    json bufferView = JSON["bufferViews"][buffViewInd];
    const unsigned int byteOffset = bufferView["byteOffset"];

    unsigned int numPerVert;
    if (type == "SCALAR") numPerVert = 1;
    else if (type ==  "VEC2") numPerVert = 2;
    else if (type == "VEC3") numPerVert = 3;
    else if (type == "VEC4") numPerVert = 4;
    else throw std::invalid_argument("Type is invalid");

    const unsigned int beginningOfData = byteOffset + accByteOffset;
    const unsigned int lengthOfData = count * 4 * numPerVert;
    for (unsigned int i = beginningOfData; i < beginningOfData + lengthOfData; i+=4) {
        const unsigned char bytes[] = { data[i], data[i+1], data[i+2], data[i+3] };
        float value;
        std::memcpy(&value, bytes, sizeof(float));
        floatVec.push_back(value);
    }

    return floatVec;
}

std::vector<GLuint> Model::getIndices(json accessor) {
    std::vector<GLuint> indices;

    const unsigned int buffViewInd = accessor.value("bufferView", 0);
    const unsigned int count = accessor["count"];
    const unsigned int accByteOffset = accessor.value("byteOffset", 0);
    const unsigned int componentType = accessor["componentType"];

    json bufferView = JSON["bufferViews"][buffViewInd];
    const unsigned int byteOffset = bufferView["byteOffset"];

    const unsigned int beginningOfData = byteOffset + accByteOffset;
    if (componentType == 5125) {
        for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 4; i += 4) {
            const unsigned char bytes[] = { data[i], data[i+1], data[i+2], data[i+3] };
            unsigned int value;
            std::memcpy(&value, bytes, sizeof(unsigned int));
            indices.push_back(value);
        }
    }
    else if (componentType == 5123) {
        for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2;i += 2) {
            const unsigned char bytes[] = { data[i], data[i+1] };
            unsigned short value;
            std::memcpy(&value, bytes, sizeof(unsigned short));
            indices.push_back(value);
        }
    }
    else if (componentType == 5122) {
        for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i+=2) {
            const unsigned char bytes[] = { data[i], data[i+1] };
            short value;
            std::memcpy(&value, bytes, sizeof(short));
            indices.push_back(value);
        }
    }

    return indices;
}

std::vector<glm::vec2> Model::groupFloatsVec2(const std::vector<float> &floatVec) {
    std::vector<glm::vec2> vectors;
    for (int i = 0; i < floatVec.size(); i+=2) {
        vectors.push_back(glm::vec2(floatVec[i], floatVec[i+1]));
    }
    return vectors;
}
std::vector<glm::vec3> Model::groupFloatsVec3(const std::vector<float> &floatVec) {
    std::vector<glm::vec3> vectors;
    for (int i = 0; i < floatVec.size(); i += 3) {
        vectors.push_back(glm::vec3(floatVec[i], floatVec[i+1], floatVec[i+2]));
    }
    return vectors;
}
std::vector<glm::vec4> Model::groupFloatsVec4(const std::vector<float> &floatVec) {
    std::vector<glm::vec4> vectors;
    for (int i = 0; i < floatVec.size(); i += 4) {
        vectors.push_back(glm::vec4(floatVec[i], floatVec[i+1], floatVec[i+2], floatVec[i+3]));
    }
    return vectors;
}

std::vector<Mesh> Model::loadMesh(const unsigned int indMesh) {
    std::vector<Mesh> models;
    json primitives = JSON["meshes"][indMesh]["primitives"];
    for (unsigned int i = 0; i < primitives.size(); i++) {
        const unsigned int posAccInd = primitives[i]["attributes"]["POSITION"];
        const unsigned int normalAccInd = primitives[i]["attributes"]["NORMAL"];
        const unsigned int texAccInd = primitives[i]["attributes"]["TEXCOORD_0"];
        const unsigned int indAccInd = primitives[i]["indices"];

        const std::vector<float> posVec = getFloats(JSON["accessors"][posAccInd]);
        const std::vector<glm::vec3> positions = groupFloatsVec3(posVec);
        const std::vector<float> normalVec = getFloats(JSON["accessors"][normalAccInd]);
        const std::vector<glm::vec3> normals = groupFloatsVec3(normalVec);
        const std::vector<float> texVec = getFloats(JSON["accessors"][texAccInd]);
        const std::vector<glm::vec2> texUVs = groupFloatsVec2(texVec);

        std::vector<float> vertices;
        std::vector<GLuint> indices = getIndices(JSON["accessors"][indAccInd]);

        for (int j = 0; j < positions.size(); ++j) {
            vertices.push_back(positions[j].x);
            vertices.push_back(positions[j].y);
            vertices.push_back(positions[j].z);

            vertices.push_back(normals[j].x);
            vertices.push_back(normals[j].y);
            vertices.push_back(normals[j].z);

            vertices.push_back(texUVs[j].x);
            vertices.push_back(texUVs[j].y);
        }

        Mesh model{vertices, indices};
        model.name = "Model_" + std::to_string(i);
        getTextures(model);
        models.push_back(model);
    }
    return models;
}

void Model::TraverseNode(const unsigned int nextNode, const glm::mat4 &matrix) {
    json node = JSON["nodes"][nextNode];

    //Load translation
    glm::vec3 translation(0.0f);
    if (node.find("translation") != node.end()) {
        float transValues[3];
        for (unsigned int i = 0; i < node["translation"].size(); i++)
            transValues[i] = (node["translation"][i]);
        translation = glm::make_vec3(transValues);
    }

    //Load rotation
    glm::quat rotation(1.0f, 0.0f, 0.0f, 0.0f);
    if (node.find("rotation") != node.end())
    {
        const float rotValues[4] =
        {
            node["rotation"][3],
            node["rotation"][0],
            node["rotation"][1],
            node["rotation"][2]
        };
        rotation = glm::make_quat(rotValues);
    }

    //Load scale
    glm::vec3 scale(1.0f);
    if (node.find("scale") != node.end())
    {
        float scaleValues[3];
        for (unsigned int i = 0; i < node["scale"].size(); i++)
            scaleValues[i] = (node["scale"][i]);
        scale = glm::make_vec3(scaleValues);
    }

    //Load matrix
    glm::mat4 matNode(1.0f);
    if (node.find("matrix") != node.end())
    {
        float matValues[16];
        for (unsigned int i = 0; i < node["matrix"].size(); i++)
            matValues[i] = (node["matrix"][i]);
        matNode = glm::make_mat4(matValues);
    }

    //Initialize matrices
    auto trans = glm::mat4(1.0f);
    auto rot = glm::mat4(1.0f);
    auto sca = glm::mat4(1.0f);

    //Update initialized matrices
    trans = glm::translate(trans, translation);
    rot = glm::mat4_cast(rotation);
    sca = glm::scale(sca, scale);

    //Multiply all matrices together
    const glm::mat4 matNextNode = matrix * matNode * trans * rot * sca;

    if (node.find("mesh") != node.end())
    {
        auto models = loadMesh(node["mesh"]);
        for (auto &model : models) {
            model.position = translation;

            const auto euler = glm::degrees(glm::eulerAngles(rotation));
            model.rotation = glm::vec3(euler.z, euler.y, euler.x); //Flip rotation
            model.scale = scale;
            meshes.push_back(model);
        }
    }

    if (node.find("children") != node.end())
    {
        for (const auto & i : node["children"])
            TraverseNode(i, matNextNode);
    }
}

void Model::getTextures(Mesh &model)
{
    std::string fileStr = std::string(file);
    std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('\\') + 1);

    // Go over all images
    for (unsigned int i = 0; i < JSON["images"].size(); i++)
    {
        // uri of current texture
        std::string texPath = JSON["images"][i]["uri"];

        //Load diffuse texture
        if (texPath.find("baseColor") != std::string::npos)
        {
            model.useTexture = true;
            std::cout << fileDirectory + texPath.c_str();
            model.texturePath = fileDirectory + texPath.c_str();
            model.texId = Texture::GetTexId(model.texturePath.c_str());
        }
        //Load specular texture
        else if (texPath.find("metallicRoughness") != std::string::npos)
        {
            model.specMapPath = fileDirectory + texPath.c_str();
            model.specMapId = Texture::GetTexId(model.specMapPath.c_str());
        }

    }
}




