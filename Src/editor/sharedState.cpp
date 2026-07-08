#include "../../include/editor/sharedState.h"

static Logger logger;

void SharedState::InitSharedState() {
	logger = Logger("EDITOR");
}

void SharedState::safe_insert(std::set<unsigned int> &field, const unsigned max, const std::set<unsigned int> &rhs, const std::string &type) {
	for (const unsigned rh : rhs) {
		if (max == 0 || rh > max-1) {
			logger("stdWarn", type + " HAS AN OUT OF BOUNDS INDEX");
			continue;
		}
		field.insert(rh);
	}
}