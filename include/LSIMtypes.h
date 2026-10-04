#ifndef LSIM_LSIMTYPES_H
#define LSIM_LSIMTYPES_H
#include <deque>

#define LSIM_CHECK(func) do {\
	if (const auto err = func; err)\
		return err;\
	} while (0)

namespace LSIM {
	enum class ErrorCode {
		WINDOW_CREATION_FAILURE,
		VK_INSTANCE_CREATION_FAILURE,
		GLFW_GET_EXTENSIONS_FAILURE,
		VK_VALIDATION_LAYERS_MISSING,
		VK_DEBUG_MESSENGER_FUNCTION_POINTER_MISSING,
		VK_DEBUG_MESSENGER_CREATION_FAILURE,
		VK_SURFACE_CREATION_FAILURE,
		VK_ENUMERATE_PHYSICAL_DEVICES_FAILURE,
		VK_GET_PHYSICAL_DEVICE_SURFACE_SUPPORT_FAILURE,
		VK_NO_SUITABLE_DEVICE_FOUND,
		VK_DEVICE_CREATION_FAILURE
	};

	enum class FatalityLevel {
		FATAL,
		ERROR,
		WARNING
	};

	struct Error {
		ErrorCode code;
		FatalityLevel fatality;
	};

	template <typename T>
	struct CapacityBuffer {
		std::deque<T> buffer;
		unsigned int capacity = 1;
		void push_back(const T& value) {
			if (buffer.size() == capacity) buffer.pop_front();
			buffer.push_back(value);
		}
		void push_back(T&& value) {
			if (buffer.size() == capacity) buffer.pop_front();
			buffer.push_back(std::move(value));
		}
		explicit CapacityBuffer(const unsigned int c) : capacity(c){}
	};
}

#endif //LSIM_LSIMTYPES_H
