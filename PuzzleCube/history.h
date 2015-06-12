#pragma once

#include <iostream>
#include <fstream>

#include "result.h"

namespace history {

	/* Puts the result into a db so stats can be created */
	void put(Result* result);

	/* Returns the best result */
	Result* getBest();

	/* Prints all results */
	void printAll();

}