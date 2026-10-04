// #include "engine.h"

#include <GLFW/glfw3.h>

#include "platform/window.h"
#include "vk/context.h"

int main(const int argc, char** argv) {
	// Engine engine(argc, argv);
	// if (const auto err = engine.start(); err) return 1;
	// if (const auto err = engine.update(); err) return 1;

	GLFWwindow* window;
	if (const auto result = Window::initializeWindow(window); !result) return 1;

	auto ctx = Context::create(window);
	if (!ctx) return 1;
	const auto context = std::move(*ctx);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}

	return 0;
}
