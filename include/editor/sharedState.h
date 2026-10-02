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
	std::set<EntityHandle> currentEntities{};
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

	[[nodiscard]] std::set<EntityHandle>& current_entities() { return currentEntities; }
	[[nodiscard]] Mode current_mode() const { return currentMode; }

	void set_current_entities(const Registry& registry, const std::set<EntityHandle> &current_entities) {
		currentEntities.clear();
		safe_insert(currentEntities, registry, current_entities, "MESH");
	}

	void add_to_current_entities(const Registry& registry, const std::set<EntityHandle> &new_entities) {
		safe_insert(currentEntities, registry, new_entities, "MESH");
	}

	void remove_from_current_entities(const std::set<EntityHandle> &old_entities) {
		safe_remove(currentEntities, old_entities);
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
