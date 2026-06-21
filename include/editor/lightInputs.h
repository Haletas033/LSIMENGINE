#ifndef LSIM_LIGHTINPUTS_H
#define LSIM_LIGHTINPUTS_H
#include "sharedState.h"
#include "inputs/inputs.h"

class LightInputs {
private:
	enum class Direction {
		FORWARD,
		SIDE,
		UP
	};

	Inputs::BindingTable bindingTable;
public:
	static void add(glm::vec3& lhs, glm::vec3 rhs);

	static void sub(glm::vec3& lhs, glm::vec3 rhs);
	static void Move(Scene &scene, const SharedState &sharedState, const Defaults &defaults, const Camera &camera, const Inputs::
			InputContext &context, Direction directionType, const std::function<void(glm::vec3 &, glm::vec3)> &op);

	void Init(Scene &scene, const SharedState &sharedState, const Defaults &defaults, const Camera &camera, Inputs &inputs);
};

#endif //LSIM_LIGHTINPUTS_H
