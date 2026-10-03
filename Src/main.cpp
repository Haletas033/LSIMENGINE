#include "engine.h"

int main(const int argc, char** argv) {
	Engine engine(argc, argv);
	if (const auto err = engine.start(); err) return 1;
	if (const auto err = engine.update(); err) return 1;
	return 0;
}