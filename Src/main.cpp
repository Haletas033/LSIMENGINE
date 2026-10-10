// #include "engine.h"

#include <chrono>
#include <thread>

#include <GLFW/glfw3.h>

#include "platform/window.h"
#include "vk/context.h"

static std::atomic_bool g_running = true;
static std::atomic_bool g_error = false;
static std::atomic<uint32_t> g_fps{};

static void update(GLFWwindow* window, Context &context) {
	uint32_t frameRate{};
	std::chrono::steady_clock::time_point last_time = std::chrono::steady_clock::now();
	while (g_running) {
		frameRate++;
		auto current_time = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(current_time - last_time);

		if (elapsed.count() >= 1) {
			g_fps = frameRate;
			frameRate = 0;
			last_time = current_time;
		}
		if (!context.render()) {
			g_error = true;
			return;
		}
	}
}

int main(const int argc, char** argv) {
	// Engine engine(argc, argv);
	// if (const auto err = engine.start(); err) return 1;
	// if (const auto err = engine.update(); err) return 1;

	GLFWwindow* window;
	if (const auto result = Window::initializeWindow(window); !result) return 1;

	auto ctx = Context::create(window);
	if (!ctx) return 1;
	auto context = std::move(*ctx);

	std::thread t_engine(update, std::ref(window), std::ref(context));

	uint32_t lastFps{};
	while (!glfwWindowShouldClose(window)) {
		if (g_error) {
			t_engine.join();
			return 1;
		}
		if (g_fps != lastFps) {
			lastFps = g_fps;
			glfwSetWindowTitle(window, std::to_string(g_fps).c_str());
		}
		glfwPollEvents();
	}
	g_running = false;
	t_engine.join();

	return 0;
}
