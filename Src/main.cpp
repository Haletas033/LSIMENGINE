// #include "engine.h"

#include <GLFW/glfw3.h>
#include "vk/context.h"

int main(const int argc, char** argv) {
	// Engine engine(argc, argv);
	// if (const auto err = engine.start(); err) return 1;
	// if (const auto err = engine.update(); err) return 1;

	glfwInit();
	auto ctx = Context::create();
	if (!ctx) return 1;
	const auto context = std::move(*ctx);

	return 0;
}
