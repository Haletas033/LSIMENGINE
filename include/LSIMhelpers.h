#ifndef LSIM_LSIMHELPERS_H
#define LSIM_LSIMHELPERS_H

#include <algorithm>
#include <type_traits>

#include "utils/logging/log.h"

template <typename T>
T MoveEnum(T lhs, const int min, const int max, const int shift, const bool wrapAround = false) {
	static_assert(std::is_enum_v<T>, "MoveEnum requires an enum type");

	const int current = static_cast<int>(lhs);

	if (wrapAround) {
		const int range = max - min + 1;
		if (range <= 0) return lhs;

		const int normalized_wrapped = ((current + shift - min) % range + range) % range;
		return static_cast<T>(min + normalized_wrapped);
	}

	if (current + shift > max)
		engineLogger("stdError", "Moving enum by " + std::to_string(shift) + " would exceed " + std::to_string(max));
	else if (current + shift < min)
		engineLogger("stdError", "Moving enum by " + std::to_string(shift) + " would make it less than " + std::to_string(min));

	return static_cast<T>(std::clamp(current + shift, min, max));
}

template <typename T>
T MoveEnum(T lhs, const T min, const T max, const int shift, const bool wrapAround = false) {
	return MoveEnum(lhs, static_cast<int>(min), static_cast<int>(max), shift, wrapAround);
}

#endif //LSIM_LSIMHELPERS_H
