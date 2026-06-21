#include "../../include/editor/sharedState.h"

Logger logger;

void SharedState::InitSharedState() {
	logger = Logger("EDITOR");
}