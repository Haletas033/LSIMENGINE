#include "../../include/editor/editorInputs.h"

void EditorInputs::Init(const Device& device, Command& command, Registry& registry, MeshPool& meshPool, Scene &scene, const std::string& workingDir, SharedState &sharedState,
			const Defaults &defaults, const Camera &camera, Inputs &inputs) {
	meshInputs.Init(registry, meshPool, sharedState, defaults, camera, inputs);
	ioInputs.Init(device, command, registry, sharedState, workingDir, inputs);

	Inputs::BindingTable editorInputs = {
		std::numeric_limits<uint32_t>::max(),
		true
	};

	bindingTable = editorInputs;
	inputs.addBindingTable(bindingTable);
}
