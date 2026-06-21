#ifndef LSIM_MESHINPUTS_H
#define LSIM_MESHINPUTS_H
#include "sharedState.h"
#include "inputs/inputs.h"
#include "scene/scene.h"

class MeshInputs {
private:
	Inputs::BindingTable bindingTable;
public:
	void Init(Scene& scene, SharedState& sharedState, Inputs& inputs);
};

#endif //LSIM_MESHINPUTS_H
