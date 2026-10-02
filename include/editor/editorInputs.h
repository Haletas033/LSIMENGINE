#ifndef LSIM_EDITORINPUTS_H
#define LSIM_EDITORINPUTS_H
#include "IOInputs.h"
#include "meshInputs.h"
#include "inputs/inputs.h"

class SharedState;

class EditorInputs {
private:
	Inputs::BindingTable bindingTable;
	MeshInputs meshInputs;
	IOInputs ioInputs;
public:
	void Init(Registry &registry, MeshPool &meshPool, Scene &scene, const std::string &workingDir, SharedState &sharedState, const
	          Defaults &defaults, const Camera &camera, Inputs
	          &inputs);
};

#endif //LSIM_EDITORINPUTS_H
