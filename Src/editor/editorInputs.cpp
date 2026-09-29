#include "../../include/editor/editorInputs.h"

void EditorInputs::Init(Registry& registry, MeshPool& meshPool, Scene &scene, const std::string& workingDir, SharedState &sharedState,
			const Defaults &defaults, const Camera &camera, Inputs &inputs) {
	meshInputs.Init(registry, meshPool, sharedState, defaults, camera, inputs);
	ioInputs.Init(registry, sharedState, workingDir, inputs);

	Inputs::BindingTable editorInputs = {
		std::numeric_limits<uint32_t>::max(),
		true
	};

	bindingTable = editorInputs;
	inputs.addBindingTable(bindingTable);
}
