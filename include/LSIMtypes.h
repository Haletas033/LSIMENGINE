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
		GLAD_LOAD_FAILURE,
		SHADER_MISSING,
		SHADER_COMPILATION_FAILURE,
		SHADER_LINK_FAILURE,
		SHADER_SOURCE_EMPTY
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
