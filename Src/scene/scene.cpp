#include "scene/scene.h"

#include <array>

#include "LSIMtypes.h"
#include <optional>

#include "engineContext.h"
#include "ECS/name.h"
#include "ECS/registry.h"
#include "geometry/mesh.h"
#include "rendering/light.h"
#include "utils/fileIO.h"
#include "utils/texture.h"

std::optional<LSIM::Error> Scene::createDefaultScene(SharedState& sharedState, EntityHandle& skybox) {
	const EntityHandle firstLight = Light::create(Registry::getDefaultRegistry(), Light::Type::POINT);
	Registry::getDefaultRegistry().getComponent<Name>(firstLight)->value = "First Light";

	EntityHandle firstCube = Mesh::create(Primitive::CUBE, MeshMode::STATIC, Registry::getDefaultRegistry(), MeshPool::getDefaultMeshPool(), Material::createStandardPBR(), Transform());
	sharedState.current_entities() = {firstCube};
	Registry::getDefaultRegistry().getComponent<Name>(firstCube)->value = "First Cube";

	//Skybox faces
	const std::array<std::string, 6> faces = {
		"skybox/right.jpg",
		"skybox/left.jpg",
		"skybox/top.jpg",
		"skybox/bottom.jpg",
		"skybox/front.jpg",
		"skybox/back.jpg",
	};

	//Get skybox texture id
	const GLuint skyboxTexId = Texture::GetCubemapId(faces, GL_LINEAR);

	Material skyboxMat;
	skyboxMat.addShader("skyboxShader");
	skyboxMat.addProperty("skybox", static_cast<int>(skyboxTexId));
	skybox = Mesh::create(
	    Primitive::CUBE,
	    MeshMode::STATIC,
	    Registry::getDefaultRegistry(),
	    MeshPool::getDefaultMeshPool(),
	    skyboxMat
	);
	Registry::getDefaultRegistry().getComponent<Name>(skybox)->value = "Skybox";

	engineLogger("stdInfo", "Successfully created the default scene");

	return std::nullopt;
}

std::optional<LSIM::Error> Scene::initializeScene(SharedState& sharedState, Camera& camera, EntityHandle& skybox) {
	if (EngineContext::getWorkingDir().empty()) {
		if (const auto err = createDefaultScene(sharedState, skybox); err)
			return err;
	} else {
		bool foundFile = false;
		for (const auto &file : std::filesystem::recursive_directory_iterator(EngineContext::getWorkingDir())) {
			if (file.path().extension().string() == ".lsim") {
				engineLogger("stdInfo", file.path().string());
				std::ifstream LSIMfile(file.path().string(), std::ios::binary);
				IO::loadFromFile(LSIMfile, Registry::getDefaultRegistry(), sharedState, EngineContext::getWorkingDir());
				engineLogger("stdInfo", "Successfully loaded the LSIM project");
				foundFile = true;
				break;
			}
		}
		if (!foundFile) {
			engineLogger("stdInfo", "Unable to find LSIM project. Creating default scene");
			if (const auto err = createDefaultScene(sharedState, skybox); err)
				return err;
		}
	}

	Defaults defaults = EngineContext::getDefaults();
	camera = Camera(
		static_cast<int>(defaults.defaultWindowWidth),
		static_cast<int>(defaults.defaultWindowHeight),
		glm::vec3(0.0f, 0.0f, 2.0f)
	);

	return std::nullopt;
}