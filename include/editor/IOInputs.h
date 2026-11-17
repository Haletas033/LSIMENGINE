#ifndef LSIM_IOINPUTS_H
#define LSIM_IOINPUTS_H
#include "sharedState.h"
#include "inputs/inputs.h"

class IOInputs {
private:
	Inputs::BindingTable bindingTable;
public:
	void Init(const Device& device, Command& command, Registry& registry, SharedState& sharedState, const std::string& workingDir, Inputs
	          & inputs);
};

#endif //LSIM_IOINPUTS_H
