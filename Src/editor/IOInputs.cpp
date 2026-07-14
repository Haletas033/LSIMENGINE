#include "../../include/editor/IOInputs.h"
#include "utils/fileIO.h"


void IOInputs::Init(Scene &scene, const std::string &workingDir, Inputs &inputs) {
	Inputs::BindingTable ioInputs = {
		1,
		true
	};

	ioInputs.addAction("save", Inputs::KeyCode::O, Inputs::KeyState::JUST_PRESSED);
	ioInputs.addAction("load", Inputs::KeyCode::I, Inputs::KeyState::JUST_PRESSED);

	ioInputs.addFunctionForAction("save", [&](const Inputs::InputContext& context) {
		const std::string fileName = IO::SaveDialog("LSIM Files\0*.lsim\0All Files\0*.*\0\0");
		if (std::ofstream file(fileName, std::ios::out | std::ios::binary); file.is_open()) {
		    IO::saveToFile(file, scene);
		}
	});
	ioInputs.addFunctionForAction("load", [&](const Inputs::InputContext& context) {
		const std::string fileName = IO::OpenDialog("LSIM Files\0*.lsim\0All Files\0*.*\0\0");
		if (std::ifstream file(fileName, std::ios::in | std::ios::binary); file.is_open()) {
		    scene = IO::loadFromFile(file, workingDir);
		}
	});

	bindingTable = ioInputs;
	inputs.addBindingTable(bindingTable);
}
