#include "engine.h"

int main(int argc, char** argv) {
	Engine engine(argc, argv);
	engine.start();
	engine.update();
	engine.exit();
	return 0;
}

