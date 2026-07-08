#ifndef LSIM_MESHINPUTS_H
#define LSIM_MESHINPUTS_H
#include "sharedState.h"
#include "inputs/inputs.h"
#include "scene/scene.h"

class MeshInputs {
private:
	enum class Direction {
		FORWARD,
		SIDE,
		UP
	};

	Inputs::BindingTable bindingTable;
	static void add(glm::vec3& lhs, glm::vec3 rhs);
	static void sub(glm::vec3& lhs, glm::vec3 rhs);
	static void Move(Scene &scene, SharedState &sharedState, const Defaults &defaults, const Camera &camera, const Inputs::InputContext &
			context, Direction directionType, const std::function<void(glm::vec3 &, glm::vec3)> &op);
public:
	void Init(Scene &scene, SharedState &sharedState, const Defaults &defaults, const Camera &camera, Inputs &inputs);
	void SetEnabled(const bool enabled) { bindingTable.is_enabled = enabled; }
};

#endif //LSIM_MESHINPUTS_H
