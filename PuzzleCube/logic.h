#pragma once

#include "field.h"
#include "ceil.h"

class Logic {
public:
	static class swap_consts {
	public:
		static const short LEFT = 1;
		static const short TOP = 2;
		static const short RIGHT = 4;
		static const short BOTTOM = 8;
		static const short ABOVE = 16;
		static const short BELOW = 32;
	};

	Ceil*** map;

	Logic(Field* field) {
		this->field = field;
		init();
	}

	void init() {
		const short w = field->width;
		const short h = field->height;
		const short d = field->depth;
		Ceil*** map = new Ceil**[w];
		for (short i = 0; i < w; i++) {
			map[i] = new Ceil*[h];
			for (short j = 0; j < h; j++) {
				map[i][j] = new Ceil[d];
				for (short k = 0; k < d; k++) {
					Ceil ceil;
					ceil.id = i * h + j * d + k;
					ceil.x, ceil.y, ceil.z = i, j, k;
					// Set the ceil.
					map[i][j][k] = ceil;
				}
			}
		}
		// Set the current ceil
		ceil = &map[0][0][0];
	}

	/* Shuffles the current field. */
	void shuffle();

	/* Tries to move the empty ceil and
	returns `true` if succeed, `false` otherwise. */
	bool swapWith(short direction);

	/* Moves the empty ceil to specified position. */
	void swapWith(short x, short y, short z);
protected:
	Field* field;
	Ceil* ceil;
};