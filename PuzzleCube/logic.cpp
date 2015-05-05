#include <ctgmath>
#include "logic.h"

void Logic::shuffle() {
	short consts[] = {
		Logic::swap_consts::LEFT, 
		Logic::swap_consts::RIGHT, 
		Logic::swap_consts::TOP, 
		Logic::swap_consts::BOTTOM, 
		Logic::swap_consts::ABOVE,
		Logic::swap_consts::BELOW,
	};
	for (int i = 0; i < 5000; i++) {
		swapWith(consts[rand() % 6]);
	}
}

bool Logic::swapWith(short direction) {
	short
		x = this->ceil->x,
		y = this->ceil->y,
		z = this->ceil->z;
	x -= direction & Logic::swap_consts::LEFT != 0;
	x += direction & Logic::swap_consts::RIGHT != 0;
	y -= direction & Logic::swap_consts::TOP != 0;
	y += direction & Logic::swap_consts::BOTTOM != 0;
	z -= direction & Logic::swap_consts::ABOVE != 0;
	z += direction & Logic::swap_consts::BELOW != 0;
	if (x < 0 || y < 0 || z < 0 
		|| x >= this->field->width
		|| y >= this->field->height 
		|| z >= this->field->depth) return false;
	swapWith(x, y, z);
	return true;
}

void Logic::swapWith(short x, short y, short z) {
	Ceil ceil = this->map[x][y][z];
	// Swap coordinates.
	ceil.x, ceil.y, ceil.z = this->ceil->x, this->ceil->y, this->ceil->z;
	this->ceil->x, this->ceil->y, this->ceil->z = x, y, z;
	// Swap items.
	this->map[x][y][z] = *this->ceil;
	this->map[ceil.x][ceil.y][ceil.z] = ceil;
}