#include "geometry/model.h"

#include <filesystem>

#include "inputs/gui.h"
#include "scene/scene.h"
#include "utils/fileIO.h"
#include "utils/texture.h"

std::expected<Model, LSIM::Error> Model::create(Registry& registry, MeshPool& meshPool, const char* file) {
        Model model{};

        std::string text = IO::GetFileContents(file);
        model.JSON = json::parse(text);

        model.file = file;
        model.data = model.getData();

        //Traverse all nodes in the root and their children recursively
        const unsigned int sceneIndex = model.JSON.value("scene", 0);
        json sceneNodes = model.JSON["scenes"][sceneIndex]["nodes"];
        for (const auto& nodeIndex : sceneNodes) {
                auto res = model.TraverseNode(registry, meshPool, nodeIndex, glm::mat4(1.0f));
                if (!res) return std::unexpected(res.error());
        }

        return model;
}

std::vector<unsigned char> Model::getData() {
        const std::string uri = JSON["buffers"][0]["uri"];

        const auto fileStr = std::string(file);
        const std::string fileDir = std::filesystem::path(fileStr).parent_path().string();
        std::string bytesText = IO::GetFileContents((std::filesystem::path(fileDir) / uri).string());

        std::vector<unsigned char> fileData(bytesText.begin(), bytesText.end());
        return fileData;
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
        else if (type == "VEC2") numPerVert = 2;
        else if (type == "VEC3") numPerVert = 3;
        else if (type == "VEC4") numPerVert = 4;
        else throw std::invalid_argument("Type is invalid");

        const unsigned int beginningOfData = byteOffset + accByteOffset;
        const unsigned int lengthOfData = count * 4 * numPerVert;
        for (unsigned int i = beginningOfData; i < beginningOfData + lengthOfData; i += 4) {
                const unsigned char bytes[] = {data[i], data[i + 1], data[i + 2], data[i + 3]};
                float value;
                std::memcpy(&value, bytes, sizeof(float));
                floatVec.push_back(value);
        }

        return floatVec;
}

std::vector<uint32_t> Model::getIndices(json accessor) {
        std::vector<uint32_t> indices;

        const unsigned int buffViewInd = accessor.value("bufferView", 0);
        const unsigned int count = accessor["count"];
        const unsigned int accByteOffset = accessor.value("byteOffset", 0);
        const unsigned int componentType = accessor["componentType"];

        json bufferView = JSON["bufferViews"][buffViewInd];
        const unsigned int byteOffset = bufferView["byteOffset"];

        const unsigned int beginningOfData = byteOffset + accByteOffset;
        if (componentType == 5125) {
                for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 4; i += 4) {
                        const unsigned char bytes[] = {data[i], data[i + 1], data[i + 2], data[i + 3]};
                        unsigned int value;
                        std::memcpy(&value, bytes, sizeof(unsigned int));
                        indices.push_back(value);
                }
        }
        else if (componentType == 5123) {
                for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i += 2) {
                        const unsigned char bytes[] = {data[i], data[i + 1]};
                        unsigned short value;
                        std::memcpy(&value, bytes, sizeof(unsigned short));
                        indices.push_back(value);
                }
        }
        else if (componentType == 5122) {
                for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i += 2) {
                        const unsigned char bytes[] = {data[i], data[i + 1]};
                        short value;
                        std::memcpy(&value, bytes, sizeof(short));
                        indices.push_back(value);
                }
        }

        return indices;
}

std::vector<glm::vec2> Model::groupFloatsVec2(const std::vector<float>& floatVec) {
        std::vector<glm::vec2> vectors;
        for (int i = 0; i < floatVec.size(); i += 2) {
                vectors.emplace_back(floatVec[i], floatVec[i + 1]);
        }
        return vectors;
}

std::vector<glm::vec3> Model::groupFloatsVec3(const std::vector<float>& floatVec) {
        std::vector<glm::vec3> vectors;
        for (int i = 0; i < floatVec.size(); i += 3) {
                vectors.emplace_back(floatVec[i], floatVec[i + 1], floatVec[i + 2]);
        }
        return vectors;
}

std::vector<glm::vec4> Model::groupFloatsVec4(const std::vector<float>& floatVec) {
        std::vector<glm::vec4> vectors;
        for (int i = 0; i < floatVec.size(); i += 4) {
                vectors.emplace_back(floatVec[i], floatVec[i + 1], floatVec[i + 2], floatVec[i + 3]);
        }
        return vectors;
}

std::expected<std::vector<EntityHandle>, LSIM::Error> Model::loadMesh(
        Registry& registry,
        MeshPool& meshPool,
        const unsigned int indMesh
) {
        std::vector<EntityHandle> models;
        json primitives = JSON["meshes"][indMesh]["primitives"];
        for (auto& primitive : primitives) {
                const unsigned int posAccInd = primitive["attributes"]["POSITION"];
                const unsigned int normalAccInd = primitive["attributes"]["NORMAL"];
                const unsigned int texAccInd = primitive["attributes"]["TEXCOORD_0"];
                const unsigned int indAccInd = primitive["indices"];

                const std::vector<float> posVec = getFloats(JSON["accessors"][posAccInd]);
                const std::vector<glm::vec3> positions = groupFloatsVec3(posVec);
                const std::vector<float> normalVec = getFloats(JSON["accessors"][normalAccInd]);
                const std::vector<glm::vec3> normals = groupFloatsVec3(normalVec);
                const std::vector<float> texVec = getFloats(JSON["accessors"][texAccInd]);
                const std::vector<glm::vec2> texUVs = groupFloatsVec2(texVec);

                std::vector<Vertex> vertices;
                std::vector<uint32_t> indices = getIndices(JSON["accessors"][indAccInd]);

                for (int j = 0; j < positions.size(); ++j) {
                        Vertex vertex{
                                .position = {positions[j].x, positions[j].y, positions[j].z},
                                .normal = {normals[j].x, normals[j].y, normals[j].z},
                                .uv = {texUVs[j].x, texUVs[j].y}
                        };

                        vertices.push_back(vertex);
                }

                auto model = Mesh::create(vertices, indices, MeshMode::STATIC, registry, meshPool);
                if (!model) return std::unexpected(model.error());

                auto* node = new Gui::Node{*model, Gui::root, {}};
                Gui::root->children.push_back(node);
                if (auto tex = getTextures(registry, *model); !tex) return std::unexpected(tex.error());
                models.push_back(*model);
        }
        return models;
}

std::expected<void, LSIM::Error> Model::TraverseNode(
        Registry& registry,
        MeshPool& meshPool,
        const unsigned int nextNode,
        const glm::mat4& matrix
) {
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
        if (node.find("rotation") != node.end()) {
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
        if (node.find("scale") != node.end()) {
                float scaleValues[3];
                for (unsigned int i = 0; i < node["scale"].size(); i++)
                        scaleValues[i] = (node["scale"][i]);
                scale = glm::make_vec3(scaleValues);
        }

        //Load matrix
        glm::mat4 matNode(1.0f);
        if (node.find("matrix") != node.end()) {
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

        if (node.find("mesh") != node.end()) {
                auto models = loadMesh(registry, meshPool, node["mesh"]);
                if (!models) return std::unexpected(models.error());
                for (auto& model : *models) {
                        auto* transform = registry.getComponent<Transform>(model);
                        const auto euler = glm::degrees(glm::eulerAngles(rotation));
                        transform->setPosition(translation);
                        transform->setRotation(glm::vec3(euler.z, euler.y, euler.x));
                        transform->setScale(scale);
                        meshes.push_back(model);
                }
        }

        if (node.find("children") != node.end()) {
                for (const auto& i : node["children"]) {
                        auto res = TraverseNode(registry, meshPool, i, matNextNode);
                        if (!res) return std::unexpected(res.error());
                }
        }

        return {};
}

std::expected<void, LSIM::Error> Model::getTextures(Registry& registry, EntityHandle& model) {
        auto* material = registry.getComponent<Material>(model);

        std::string fileStr = std::string(file);
        std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('\\') + 1);
        // Go over all images
        for (unsigned int i = 0; i < JSON["images"].size(); i++) {
                std::expected<TextureResource*, LSIM::Error> tex;
                std::string texPath = JSON["images"][i]["uri"];
                if (texPath.find("Color") != std::string::npos) {
                        tex = material->setTexture("albedo", fileDirectory + texPath, VK_FILTER_NEAREST,
                                                   TextureType::COLOR);
                        if (!tex) return std::unexpected(tex.error());
                }
                else if (texPath.find("Roughness") != std::string::npos) {
                        tex = material->setTexture("specular", fileDirectory + texPath, VK_FILTER_NEAREST,
                                                   TextureType::DATA);
                        if (!tex) return std::unexpected(tex.error());
                }
                else if (texPath.find("Normal") != std::string::npos) {
                        tex = material->setTexture("normal", fileDirectory + texPath, VK_FILTER_NEAREST,
                                                   TextureType::DATA);
                        if (!tex) return std::unexpected(tex.error());
                }
        }

        return {};
}
