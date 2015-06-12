#pragma once

#include "math_utils.h"

class Result {
public:
	unsigned int time;
	unsigned int steps;
	
	Result(unsigned int time, unsigned int steps) {
		this->time = time;
		this->steps = steps;
	}

	int getScore() {
		return max(5000 - this->time * 5 - this->steps, 0);
	}
};