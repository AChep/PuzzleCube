#pragma once

#include <fstream>

#include "logic.h"

namespace logic_bp {

	/* Puts the current game state into a file */
	void save(Logic* logic);

	/* Loads the saved game state from a file */
	void load(Logic* logic);

}