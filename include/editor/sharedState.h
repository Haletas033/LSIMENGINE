#ifndef LSIM_SHAREDSTATE_H
#define LSIM_SHAREDSTATE_H
#include <set>

#include "ECS/entityManager.h"
#include "geometry/primitive.h"
#include "scene/scene.h"
#include "utils/logging/log.h"
#include "ECS/registry.h"

class SharedState {
public:
	enum class Mode {
		MESH_MODE,
		LIGHT_MODE
	};

	enum class Transform {
		POSITION,
		ROTATION,
		SCALE
	};
private:
	std::set<EntityHandle> currentMeshes{};
	std::set<unsigned int> currentLights{};
	Mode currentMode = Mode::MESH_MODE;
	Transform currentTransform = Transform::POSITION;

	Primitive::Type selectedMeshType = Primitive::CUBE;

	static void safe_insert(std::set<EntityHandle>& target, const Registry& registry, const std::set<EntityHandle>& source, const std::string& context);

	static void safe_insert(std::set<unsigned int> &field, unsigned max, const std::set<unsigned int> &rhs,
			 const std::string &type);

	static void safe_remove(std::set<EntityHandle>& field, const std::set<EntityHandle> &rhs) {
		for (const EntityHandle& rh : rhs) field.erase(rh);
	}

	static void safe_remove(std::set<unsigned>& field, const std::set<unsigned int> &rhs) {
		for (const unsigned rh : rhs) field.erase(rh);
	}
public:
	static void InitSharedState();

	[[nodiscard]] std::set<EntityHandle>& current_meshes() { return currentMeshes; }
	[[nodiscard]] std::set<unsigned int>& current_lights() { return currentLights; }
	[[nodiscard]] Mode current_mode() const { return currentMode; }

	void set_current_meshes(const Registry& registry, const std::set<EntityHandle> &current_meshes) {
		currentMeshes.clear();
		safe_insert(currentMeshes, registry, current_meshes, "MESH");
	}

	void add_to_current_meshes(const Registry& registry, const std::set<EntityHandle> &new_meshes) {
		safe_insert(currentMeshes, registry, new_meshes, "MESH");
	}

	void remove_from_current_meshes(const std::set<EntityHandle> &old_meshes) {
		safe_remove(currentMeshes, old_meshes);
	}

	void set_current_lights(const Scene& scene, const std::set<unsigned int> &current_lights) {
		currentLights.clear();
		safe_insert(currentLights, scene.lights.size(), current_lights, "LIGHT");
	}

	void add_to_current_lights(const Scene& scene, const std::set<unsigned int> &new_lights) {
		safe_insert(currentLights, scene.lights.size(), new_lights, "LIGHT");
	}

	void remove_from_current_lights(const std::set<unsigned int> &old_lights) {
		safe_remove(currentLights, old_lights);
	}

	void set_current_mode(const Mode current_mode) { currentMode = current_mode; }

	[[nodiscard]] Primitive::Type& selected_mesh_type() {
		return selectedMeshType;
	}

	void set_selected_mesh_type(const Primitive::Type selected_mesh_type) {
		selectedMeshType = selected_mesh_type;
	}

	[[nodiscard]] Transform& current_transform() {
		return currentTransform;
	}

	void set_current_transform(const Transform current_transform) {
		currentTransform = current_transform;
	}
};

#endif //LSIM_SHAREDSTATE_H
