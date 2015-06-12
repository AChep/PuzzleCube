#include "logic_bp.h"

namespace logic_bp {
	const char* FILENAME = "__save.txt";
}

/* Puts the current game state into a file */
void logic_bp::save(Logic* logic) {
	std::ofstream stream(FILENAME);

	Ceil* ceil = logic->ceil;
	stream << ' ' << ceil->x << ' ' << ceil->y << ' ' << ceil->z;

	const short w = logic->field->width;
	const short h = logic->field->height;
	const short d = logic->field->depth;
	for (short i = 0; i < w; i++) {
		for (short j = 0; j < h; j++) {
			for (short k = 0; k < d; k++) {
				Ceil* ceil = logic->map[i][j][k];
				stream << ' ' << ceil->x << ' ' << ceil->id_x;
				stream << ' ' << ceil->y << ' ' << ceil->id_y;
				stream << ' ' << ceil->z << ' ' << ceil->id_z;
				stream << ' ' << ceil->id;
			}
		}
	}

	stream << ' ' << logic->clock_cc << ' ' << logic->steps;
	stream.close();
}

/* Loads the saved game state from a file */
void logic_bp::load(Logic* logic) {
	std::ifstream stream;
	try{
		stream.open(FILENAME);

		short x, y, z;
		stream >> x >> y >> z;
		logic->ceil = logic->map[x][y][z];

		const short w = logic->field->width;
		const short h = logic->field->height;
		const short d = logic->field->depth;
		for (short i = 0; i < w; i++) {
			for (short j = 0; j < h; j++) {
				for (short k = 0; k < d; k++) {
					Ceil* ceil = logic->map[i][j][k];
					stream >> ceil->x >> ceil->id_x;
					stream >> ceil->y >> ceil->id_y;
					stream >> ceil->z >> ceil->id_z;
					stream >> ceil->id;
				}
			}
		}

		stream >> logic->clock_cc >> logic->steps;
		stream.close();
	}
	catch (...) { /* unlucky */ }
}