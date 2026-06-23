#include "../../include/editor/editorInputs.h"

#include <algorithm>

void EditorInputs::Init(Scene &scene, const std::string& workingDir, SharedState &sharedState,
			const Defaults &defaults, const Camera &camera, Inputs &inputs) {
	meshInputs.Init(scene, sharedState, defaults, camera, inputs);
	lightInputs.Init(scene, sharedState, defaults, camera, inputs);
	ioInputs.Init(scene, workingDir, inputs);

	Inputs::BindingTable editorInputs = {
		std::numeric_limits<uint32_t>::max(),
		true
	};

	editorInputs.addAction("mesh_mode", Inputs::KeyCode::M, true);
	editorInputs.addAction("light_mode", Inputs::KeyCode::L, true);

	editorInputs.addFunctionForAction("mesh_mode", [&](const Inputs::InputContext& context) {
		lightInputs.SetEnabled(false);
		meshInputs.SetEnabled(true);
	});

	editorInputs.addFunctionForAction("light_mode", [&](const Inputs::InputContext& context) {
		lightInputs.SetEnabled(true);
		meshInputs.SetEnabled(false);
	});

	bindingTable = editorInputs;
	inputs.addBindingTable(&bindingTable);
}
