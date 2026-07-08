#include "../../include/editor/lightInputs.h"

void LightInputs::add(glm::vec3& lhs, const glm::vec3 rhs) {
	lhs += rhs;
}

void LightInputs::sub(glm::vec3& lhs, const glm::vec3 rhs) {
	lhs -= rhs;
}

void LightInputs::Move(Scene &scene, SharedState &sharedState, const Defaults& defaults, const Camera& camera,
	const Inputs::InputContext& context, const Direction directionType, const std::function<void(glm::vec3&, glm::vec3)>& op) {
	auto [forward, side] = camera.getDirection();

	if (sharedState.current_transform() == SharedState::Transform::ROTATION)
		std::swap(forward, side);
	glm::vec3 direction = forward;
	if (directionType == Direction::SIDE) direction = side;
	else if (directionType == Direction::UP) direction = glm::vec3(0,1,0);

	for (const auto light : sharedState.current_lights())
		op(scene.lights[light].lightPos, direction * context.deltaTime * defaults.transformSpeed);
}

void LightInputs::Init(Scene &scene, SharedState &sharedState, const Defaults &defaults, const Camera &camera, Inputs &inputs) {
	Inputs::BindingTable lightInputs = {
		1,
		false
	};

	lightInputs.addAction("move_lights_forward", Inputs::KeyCode::UP, false);
	lightInputs.addAction("move_lights_backward", Inputs::KeyCode::DOWN, false);
	lightInputs.addAction("move_lights_left", Inputs::KeyCode::LEFT, false);
	lightInputs.addAction("move_lights_right", Inputs::KeyCode::RIGHT, false);
	lightInputs.addAction("move_lights_up", Inputs::KeyCode::PAGE_UP, false);
	lightInputs.addAction("move_lights_down", Inputs::KeyCode::PAGE_DOWN, false);

	lightInputs.addFunctionForAction("move_lights_forward", [&](const Inputs::InputContext& context) {
		Move(scene, sharedState, defaults, camera, context, Direction::FORWARD, sub);
	});
	lightInputs.addFunctionForAction("move_lights_backward", [&](const Inputs::InputContext& context) {
		Move(scene, sharedState, defaults, camera, context, Direction::FORWARD, add);
	});
	lightInputs.addFunctionForAction("move_lights_left", [&](const Inputs::InputContext& context) {
		Move(scene, sharedState, defaults, camera, context, Direction::SIDE, sub);
	});
	lightInputs.addFunctionForAction("move_lights_right", [&](const Inputs::InputContext& context) {
		Move(scene, sharedState, defaults, camera, context, Direction::SIDE, add);
	});
	lightInputs.addFunctionForAction("move_lights_up", [&](const Inputs::InputContext& context) {
		Move(scene, sharedState, defaults, camera, context, Direction::UP, add);
	});
	lightInputs.addFunctionForAction("move_lights_down", [&](const Inputs::InputContext& context) {
		Move(scene, sharedState, defaults, camera, context, Direction::UP, sub);
	});

	bindingTable = lightInputs;
	inputs.addBindingTable(&bindingTable);
}