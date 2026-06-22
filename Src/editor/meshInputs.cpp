#include "../../include/editor/meshInputs.h"

#include "LSIMhelpers.h"

void MeshInputs::add(glm::vec3& lhs, const glm::vec3 rhs) {
	lhs += rhs;
}

void MeshInputs::sub(glm::vec3& lhs, const glm::vec3 rhs) {
	lhs -= rhs;
}

glm::vec3 Mesh::* TransformToMeshProperty(const SharedState& sharedState) {
	glm::vec3 Mesh::* field = &Mesh::position;
	switch (sharedState.current_transform()) {
		case SharedState::Transform::POSITION:
			field = &Mesh::position;
			break;
		case SharedState::Transform::ROTATION:
			field = &Mesh::rotation;
			break;
		case SharedState::Transform::SCALE:
			field = &Mesh::scale;
			break;
		default: break;
	}
	return field;
}

void MeshInputs::Move(Scene &scene, const SharedState &sharedState, const Defaults& defaults, const Camera& camera,
	const Inputs::InputContext& context,const Direction directionType, const std::function<void(glm::vec3&, glm::vec3)>& op) {
	auto [forward, side] = camera.getDirection();

	glm::vec3 Mesh::* field = TransformToMeshProperty(sharedState);

	if (sharedState.current_transform() == SharedState::Transform::ROTATION)
		std::swap(forward, side);
	glm::vec3 direction = forward;
	if (directionType == Direction::SIDE) direction = side;
	else if (directionType == Direction::UP) direction = glm::vec3(0,1,0);
	if (sharedState.current_transform() == SharedState::Transform::SCALE && directionType != Direction::UP)
		direction = -direction;

	for (const auto mesh : sharedState.current_meshes())
		op(scene.meshes[mesh][0].get()->*field, direction * context.deltaTime * defaults.transformSpeed);
}

void MeshInputs::Init(Scene &scene, SharedState &sharedState, const Defaults& defaults, const Camera& camera, Inputs &inputs) {
	Inputs::BindingTable meshInputs = {
		1,
		true
	};

	meshInputs.addAction("move_meshes_forward", Inputs::KeyCode::UP, false);
	meshInputs.addAction("move_meshes_backward", Inputs::KeyCode::DOWN, false);
	meshInputs.addAction("move_meshes_left", Inputs::KeyCode::LEFT, false);
	meshInputs.addAction("move_meshes_right", Inputs::KeyCode::RIGHT, false);
	meshInputs.addAction("move_meshes_up", Inputs::KeyCode::PAGE_UP, false);
	meshInputs.addAction("move_meshes_down", Inputs::KeyCode::PAGE_DOWN, false);

	meshInputs.addAction("select_move_mode", Inputs::KeyCode::G, true);
	meshInputs.addAction("select_rotation_mode", Inputs::KeyCode::R, true);
	meshInputs.addAction("select_scale_mode", Inputs::KeyCode::N, true);

	meshInputs.addAction("add_mesh", Inputs::KeyCode::F, true);
	meshInputs.addAction("delete_mesh", Inputs::KeyCode::DELETE, true);

	meshInputs.addFunctionForAction("move_meshes_forward", [&](const Inputs::InputContext& context) {
		Move(scene, sharedState, defaults, camera, context, Direction::FORWARD, sub);
	});
	meshInputs.addFunctionForAction("move_meshes_backward", [&](const Inputs::InputContext& context) {
		Move(scene, sharedState, defaults, camera, context, Direction::FORWARD, add);
	});
	meshInputs.addFunctionForAction("move_meshes_left", [&](const Inputs::InputContext& context) {
		Move(scene, sharedState, defaults, camera, context, Direction::SIDE, sub);
	});
	meshInputs.addFunctionForAction("move_meshes_right", [&](const Inputs::InputContext& context) {
		Move(scene, sharedState, defaults, camera, context, Direction::SIDE, add);
	});
	meshInputs.addFunctionForAction("move_meshes_up", [&](const Inputs::InputContext& context) {
		Move(scene, sharedState, defaults, camera, context, Direction::UP, add);
	});
	meshInputs.addFunctionForAction("move_meshes_down", [&](const Inputs::InputContext& context) {
		Move(scene, sharedState, defaults, camera, context, Direction::UP, sub);
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
		scene.addMeshSignal = true;
	});

	meshInputs.addFunctionForAction("delete_mesh", [&](const Inputs::InputContext& context) {
		scene.deleteMeshSignal = true;
	});

	// Generate 0-9 bindings
	for (int i = 0; i < 10; i++) {
		std::string name = "select_mesh_type_" + std::to_string(i);
		meshInputs.addAction(
			name,
			MoveEnum(Inputs::KeyCode::NUM_0, Inputs::KeyCode::NUM_0, Inputs::KeyCode::NUM_9,i, false),
			true
		);

		meshInputs.addFunctionForAction(name, [&](const Inputs::InputContext& context) {
			sharedState.set_selected_mesh_type(MoveEnum(Primitive::CUBE, Primitive::CUBE, Primitive::MODEL, i, false));
		});
	}

	bindingTable = meshInputs;
	inputs.addBindingTable(&bindingTable);
}
