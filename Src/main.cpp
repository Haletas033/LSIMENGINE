// #include "engine.h"

#include <chrono>

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
	auto context = std::move(*ctx);

	uint32_t frameRate{};
	std::chrono::steady_clock::time_point last_time = std::chrono::steady_clock::now();
	while (!glfwWindowShouldClose(window)) {
		frameRate++;
		auto current_time = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(current_time - last_time);

		if (elapsed.count() >= 1) {
			glfwSetWindowTitle(window, std::to_string(frameRate).c_str());
			frameRate = 0;
			last_time = current_time;
		}

		glfwPollEvents();
		if (!context.render()) return 1;
	}

	return 0;
}
