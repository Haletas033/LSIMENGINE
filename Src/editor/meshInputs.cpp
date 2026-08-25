#include "../../include/editor/meshInputs.h"

#include "LSIMhelpers.h"
#include "geometry/mesh.h"
#include "inputs/gui.h"

void MeshInputs::add(glm::vec3& lhs, const glm::vec3 rhs) {
	lhs += rhs;
}

void MeshInputs::sub(glm::vec3& lhs, const glm::vec3 rhs) {
	lhs -= rhs;
}

TransformAccessor MeshInputs::TransformToProperty(Transform& transform, SharedState& sharedState) {
	switch (sharedState.current_transform()) {
		case SharedState::Transform::POSITION:
			return { [&transform]{ return transform.getPosition(); },
				 [&transform](const glm::vec3& v){ transform.setPosition(v); } };
		case SharedState::Transform::ROTATION:
			return { [&transform] {
				return glm::degrees(glm::eulerAngles(transform.getRotation())); },
			[&transform](const glm::vec3& v) { transform.setRotation(v); } };
		case SharedState::Transform::SCALE:
			return { [&transform]{ return transform.getScale(); },
				 [&transform](const glm::vec3& v){ transform.setScale(v); } };
		default:
			return { [&transform]{ return transform.getPosition(); },
				 [&transform](const glm::vec3& v){ transform.setPosition(v); } };
	}
}

void MeshInputs::Move(Registry& registry, SharedState &sharedState, const Defaults &defaults, const Camera &camera,
	const Inputs::InputContext &context,const Direction directionType, const std::function<void(glm::vec3&, glm::vec3)>& op) {
	auto [forward, side] = camera.getDirection();

	if (sharedState.current_transform() == SharedState::Transform::ROTATION) {
		std::swap(forward, side);
		side = -side; // Flip left and right
	}
	glm::vec3 direction = forward;
	if (directionType == Direction::SIDE) direction = side;
	else if (directionType == Direction::UP) direction = glm::vec3(0,1,0);
	if (sharedState.current_transform() == SharedState::Transform::SCALE && directionType != Direction::UP)
		direction = -direction; // Flip for scale

	const float speed = sharedState.current_transform() == SharedState::Transform::ROTATION ? defaults.rotationSpeed : defaults.transformSpeed;
	const glm::vec3 delta = direction * context.deltaTime * speed;

	for (const EntityHandle& e : sharedState.current_meshes()) {
		auto* transform = registry.getComponent<Transform>(e);
		if (!transform) continue;
		auto [get, set] = TransformToProperty(*transform, sharedState);
		glm::vec3 value = get();
		op(value, delta);
		set(value);
	}
}

void MeshInputs::Init(Registry& registry, MeshPool& meshPool, SharedState &sharedState, const Defaults& defaults, const Camera& camera, Inputs &inputs) {
	Inputs::BindingTable meshInputs = {
		1,
		true
	};

	meshInputs.addAction("move_meshes_forward", Inputs::KeyCode::UP, Inputs::KeyState::HELD);
	meshInputs.addAction("move_meshes_backward", Inputs::KeyCode::DOWN, Inputs::KeyState::HELD);
	meshInputs.addAction("move_meshes_left", Inputs::KeyCode::LEFT, Inputs::KeyState::HELD);
	meshInputs.addAction("move_meshes_right", Inputs::KeyCode::RIGHT, Inputs::KeyState::HELD);
	meshInputs.addAction("move_meshes_up", Inputs::KeyCode::PAGE_UP, Inputs::KeyState::HELD);
	meshInputs.addAction("move_meshes_down", Inputs::KeyCode::PAGE_DOWN, Inputs::KeyState::HELD);

	meshInputs.addAction("select_move_mode", Inputs::KeyCode::G, Inputs::KeyState::JUST_PRESSED);
	meshInputs.addAction("select_rotation_mode", Inputs::KeyCode::R, Inputs::KeyState::JUST_PRESSED);
	meshInputs.addAction("select_scale_mode", Inputs::KeyCode::N, Inputs::KeyState::JUST_PRESSED);

	meshInputs.addAction("add_mesh", Inputs::KeyCode::F, Inputs::KeyState::JUST_PRESSED);
	meshInputs.addAction("delete_mesh", Inputs::KeyCode::DELETE, Inputs::KeyState::JUST_PRESSED);

	meshInputs.addFunctionForAction("move_meshes_forward", [&](const Inputs::InputContext& context) {
		Move(registry, sharedState, defaults, camera, context, Direction::FORWARD, sub);
	});
	meshInputs.addFunctionForAction("move_meshes_backward", [&](const Inputs::InputContext& context) {
		Move(registry, sharedState, defaults, camera, context, Direction::FORWARD, add);
	});
	meshInputs.addFunctionForAction("move_meshes_left", [&](const Inputs::InputContext& context) {
		Move(registry, sharedState, defaults, camera, context, Direction::SIDE, sub);
	});
	meshInputs.addFunctionForAction("move_meshes_right", [&](const Inputs::InputContext& context) {
		Move(registry, sharedState, defaults, camera, context, Direction::SIDE, add);
	});
	meshInputs.addFunctionForAction("move_meshes_up", [&](const Inputs::InputContext& context) {
		Move(registry, sharedState, defaults, camera, context, Direction::UP, add);
	});
	meshInputs.addFunctionForAction("move_meshes_down", [&](const Inputs::InputContext& context) {
		Move(registry, sharedState, defaults, camera, context, Direction::UP, sub);
	});

	meshInputs.addFunctionForAction("select_move_mode", [&](const Inputs::InputContext& context) {
		sharedState.set_current_transform(SharedState::Transform::POSITION);
	});
	meshInputs.addFunctionForAction("select_rotation_mode", [&](const Inputs::InputContext& context) {
		sharedState.set_current_transform(SharedState::Transform::ROTATION);
	});
	meshInputs.addFunctionForAction("select_scale_mode", [&](const Inputs::InputContext& context) {
		sharedState.set_current_transform(SharedState::Transform::SCALE);
	});

	meshInputs.addFunctionForAction("add_mesh", [&](const Inputs::InputContext& context) {
		const EntityHandle mesh = Mesh::create(sharedState.selected_mesh_type(), MeshMode::STATIC, registry, meshPool);
		sharedState.current_meshes() = {mesh};
		auto* node = new Gui::Node{ mesh, Gui::root, {} };
		Gui::root->children.push_back(node);
	});

	meshInputs.addFunctionForAction("delete_mesh", [&](const Inputs::InputContext& context) {
		for (const EntityHandle& e : sharedState.current_meshes()) {
	    		if (Gui::Node *node = Gui::FindNodeByMesh(Gui::root, e); node != nullptr) {
	    			std::erase(node->parent->children, node);
	    			Gui::DeleteNodeRecursively(registry, node);
	    		}
		}
		sharedState.current_meshes() = {};
	});

	// Generate 0-9 bindings
	for (int i = 0; i < 10; i++) {
		std::string name = "select_mesh_type_" + std::to_string(i);
		meshInputs.addAction(
			name,
			MoveEnum(Inputs::KeyCode::NUM_0, Inputs::KeyCode::NUM_0, Inputs::KeyCode::NUM_9,i, false),
			Inputs::KeyState::JUST_PRESSED
		);

		meshInputs.addFunctionForAction(name, [&, i](const Inputs::InputContext& context) {
			sharedState.set_selected_mesh_type(MoveEnum(Primitive::CUBE, Primitive::CUBE, Primitive::MODEL, i, false));
		});
	}

	bindingTable = meshInputs;
	inputs.addBindingTable(bindingTable);
}
