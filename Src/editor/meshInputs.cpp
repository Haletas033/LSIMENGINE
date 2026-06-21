#include "../../include/editor/meshInputs.h"

void MeshInputs::Init(Scene &scene, SharedState &sharedState, Inputs &inputs) {
	const Inputs::BindingTable meshInputs = {
		1,
		true
	};

	bindingTable = meshInputs;
	inputs.addBindingTable(bindingTable);
}
