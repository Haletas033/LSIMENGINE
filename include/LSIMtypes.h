//
// Created by haletas on 4/19/26.
//

#ifndef LSIM_LSIMTYPES_H
#define LSIM_LSIMTYPES_H
#include <deque>

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

#endif //LSIM_LSIMTYPES_H
