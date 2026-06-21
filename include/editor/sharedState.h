#ifndef LSIM_SHAREDSTATE_H
#define LSIM_SHAREDSTATE_H
#include <set>

#include "geometry/primitive.h"
#include "scene/scene.h"
#include "utils/logging/log.h"

extern Logger logger;

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
	std::set<unsigned int> currentMeshes;
	std::set<unsigned int> currentLights;
	Mode currentMode = Mode::MESH_MODE;
	Transform currentTransform = Transform::POSITION;

	Primitive::Type selectedMeshType = Primitive::CUBE;

	static void safe_insert(std::set<unsigned>& field, const unsigned max, const std::set<unsigned int> &rhs, const std::string& type) {
		for (const unsigned rh : rhs) {
			if (max == 0 || rh > max-1) {
				logger("stdWarn", type + " HAS AN OUT OF BOUNDS INDEX");
				continue;
			}
			field.insert(rh);
		}
	}

	static void safe_remove(std::set<unsigned>& field, const std::set<unsigned int> &rhs) {
		for (const unsigned rh : rhs) field.erase(rh);
	}
public:
	static void InitSharedState();

	[[nodiscard]] std::set<unsigned int> current_meshes() const { return currentMeshes; }
	[[nodiscard]] std::set<unsigned int> current_lights() const { return currentLights; }
	[[nodiscard]] Mode current_mode() const { return currentMode; }

	void set_current_meshes(const Scene& scene, const std::set<unsigned int> &current_meshes) {
		currentMeshes.clear();
		safe_insert(currentMeshes, scene.meshes.size(), current_meshes, "MESH");
	}

	void add_to_current_meshes(const Scene& scene, const std::set<unsigned int> &new_meshes) {
		safe_insert(currentMeshes, scene.meshes.size(), new_meshes, "MESH");
	}

	void remove_from_current_meshes(const std::set<unsigned int> &old_meshes) {
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

	[[nodiscard]] Primitive::Type selected_mesh_type() const {
		return selectedMeshType;
	}

	void set_selected_mesh_type(const Primitive::Type selected_mesh_type) {
		selectedMeshType = selected_mesh_type;
	}

	[[nodiscard]] Transform current_transform() const {
		return currentTransform;
	}

	void set_current_transform(const Transform current_transform) {
		currentTransform = current_transform;
	}
};

#endif //LSIM_SHAREDSTATE_H
