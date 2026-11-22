#include "scene/scene.h"

#include <array>

#include "LSIMtypes.h"

#include "engineContext.h"
#include "ECS/name.h"
#include "ECS/registry.h"
#include "editor/sharedState.h"
#include "geometry/mesh.h"
#include "rendering/light.h"
#include "utils/fileIO.h"
#include "utils/serialization.h"
#include "utils/texture.h"

std::expected<void, LSIM::Error> Scene::createDefaultScene(SharedState& sharedState, EntityHandle& skybox) {
	const EntityHandle firstLight = Light::create(Registry::getDefaultRegistry(), Light::Type::POINT);
	Registry::getDefaultRegistry().getComponent<Name>(firstLight)->value = "First Light";

	auto firstCube = Mesh::create(
		Primitive::CUBE,
		MeshMode::STATIC,
		Registry::getDefaultRegistry(),
		MeshPool::getDefaultMeshPool(),
		Material::createStandardPBR(),
		Transform()
	);
	if (!firstCube) return std::unexpected(firstCube.error());

	sharedState.current_entities() = {*firstCube};
	Registry::getDefaultRegistry().getComponent<Name>(*firstCube)->value = "First Cube";

	//Skybox faces
	const std::array<std::string, 6> faces = {
		"skybox/right.jpg",
		"skybox/left.jpg",
		"skybox/top.jpg",
		"skybox/bottom.jpg",
		"skybox/front.jpg",
		"skybox/back.jpg",
	};

	auto tex = ResourceManager::addCubeMapTexture(
		"skybox",
		faces,
		VK_FILTER_LINEAR,
		TextureType::COLOR
	);
	if (!tex) return std::unexpected(tex.error());

	Material skyboxMat;
	skyboxMat.addPipeline("skyboxShader");
	skyboxMat.addTexture("skybox");
	auto mesh = Mesh::create(
		Primitive::CUBE,
		MeshMode::STATIC,
		Registry::getDefaultRegistry(),
		MeshPool::getDefaultMeshPool(),
		skyboxMat
	);
	if (!mesh) return std::unexpected(mesh.error());
	skybox = *mesh;

	Registry::getDefaultRegistry().getComponent<Name>(skybox)->value = "Skybox";

	engineLogger("stdInfo", "Successfully created the default scene");

	return {};
}

std::expected<void, LSIM::Error> Scene::initializeScene(const Device& device, Command& command, SharedState& sharedState, Camera& camera, EntityHandle& skybox) {
	if (EngineContext::getWorkingDir().empty()) {
		if (const auto res = createDefaultScene(sharedState, skybox); !res)
			return std::unexpected(res.error());
	} else {
		bool foundFile = false;
		for (const auto &file : std::filesystem::recursive_directory_iterator(EngineContext::getWorkingDir())) {
			if (file.path().extension().string() == ".lsim") {
				engineLogger("stdInfo", file.path().string());
				std::ifstream LSIMfile(file.path().string(), std::ios::binary);
				Serialization::loadFromFile(device, command, LSIMfile, Registry::getDefaultRegistry(), sharedState, EngineContext::getWorkingDir());
				engineLogger("stdInfo", "Successfully loaded the LSIM project");
				foundFile = true;
				break;
			}
		}
		if (!foundFile) {
			engineLogger("stdInfo", "Unable to find LSIM project. Creating default scene");
			if (const auto res = createDefaultScene(sharedState, skybox); !res)
				return std::unexpected(res.error());
		}
	}

	Defaults defaults = EngineContext::getDefaults();
	auto res = Camera::create(
		device,
		static_cast<int>(defaults.defaultWindowWidth),
		static_cast<int>(defaults.defaultWindowHeight),
		glm::vec3(0.0f, 0.0f, 2.0f)
	);
	if (!res) return std::unexpected(res.error());
	camera = std::move(*res);

	return {};
}