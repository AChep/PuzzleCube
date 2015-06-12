#include "logic.h"

using std::cout;
using std::endl;

bool shuffling;
bool is_complete;
bool is_complete_needs_recheck = true;

void Logic::shuffle() {
	short consts[] = {
		Logic::swap_consts::LEFT, 
		Logic::swap_consts::RIGHT, 
		Logic::swap_consts::TOP, 
		Logic::swap_consts::BOTTOM, 
		Logic::swap_consts::ABOVE,
		Logic::swap_consts::BELOW,
	};
	shuffling = true;
	do {
		for (int i = 0; i < 500; i++) {
			swapWith(consts[rand() % 6]);
		}
	} while (isComplete());
	shuffling = false;
}

bool Logic::swapWith(short direction) {
	short
		x = this->ceil->x,
		y = this->ceil->y,
		z = this->ceil->z;
	x -= (direction & Logic::swap_consts::LEFT) != 0;
	x += (direction & Logic::swap_consts::RIGHT) != 0;
	y -= (direction & Logic::swap_consts::TOP) != 0;
	y += (direction & Logic::swap_consts::BOTTOM) != 0;
	z -= (direction & Logic::swap_consts::ABOVE) != 0;
	z += (direction & Logic::swap_consts::BELOW) != 0;
	if (x < 0 || y < 0 || z < 0 
		|| x >= this->field->width
		|| y >= this->field->height 
		|| z >= this->field->depth) return false;
	swapWith(x, y, z);
	return true;
}

void Logic::swapWith(short x, short y, short z) {
	Ceil* ceil = this->map[x][y][z];
	short x2 = this->ceil->x, 
		y2 = this->ceil->y, 
		z2 = this->ceil->z;
	// Swap coordinates.
	ceil->x = x2;
	ceil->y = y2;
	ceil->z = z2;
	this->ceil->x = x;
	this->ceil->y = y;
	this->ceil->z = z;
	// Swap items.
	this->map[x][y][z] = this->ceil;
	this->map[x2][y2][z2] = ceil;
	this->steps++;

	is_complete_needs_recheck = true;
	if (!shuffling && isComplete()) onComplete();
}

bool Logic::isComplete() {
	if (!is_complete_needs_recheck) return is_complete;
	is_complete_needs_recheck = false;

	const short w = field->width;
	const short h = field->height;
	const short d = field->depth;
	for (short i = 0; i < w; i++) {
		for (short j = 0; j < h; j++) {
			for (short k = 0; k < d; k++) {
				short id = genId(i, j, k);
				if (map[i][j][k]->id != id) return is_complete = false;
			}
		}
	}
	return is_complete = true;
}