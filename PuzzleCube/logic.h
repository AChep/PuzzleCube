#pragma once

#include <iostream>
#include <ctime>

#include "field.h"
#include "ceil.h"

using std::cout;
using std::endl;

class Logic {
private:
public:
	static const short NEW_GAME = 1;
	static const short RESUMED = 2;
	static const short COMPLETED = 4;
	static const short PAUSED = 8;

	static class swap_consts {
	public:
		static const short LEFT = 1;
		static const short TOP = 2;
		static const short RIGHT = 4;
		static const short BOTTOM = 8;
		static const short ABOVE = 16;
		static const short BELOW = 32;
	};

	/* callback var */
	void(*callback)(int);

	Field* field;
	Ceil**** map;
	Ceil* ceil;

	int  steps;
	bool resumed;

	std::clock_t clock;
	int  clock_dd;
	int  clock_cc;

	Logic(Field* field, void(*f)(int)) {
		this->field = field;
		this->callback = f;
		init();
	}

	short genId(short i, short j, short k) {
		const short w = field->width;
		const short h = field->height;
		const short d = field->depth;
		return i + j * w + k * w * d + 1;
	}

	void init() {
		const short w = field->width;
		const short h = field->height;
		const short d = field->depth;
		map = new Ceil***[w];
		for (short i = 0; i < w; i++) {
			map[i] = new Ceil**[h];
			for (short j = 0; j < h; j++) {
				map[i][j] = new Ceil*[d];
				for (short k = 0; k < d; k++) {
					Ceil* ceil = new Ceil();
					ceil->x = ceil->id_x = i;
					ceil->y = ceil->id_y = j;
					ceil->z = ceil->id_z = k;
					ceil->id = genId(i, j, k);
					// Set the ceil.
					map[i][j][k] = ceil;
				}
			}
		}
		// Set the current ceil
		ceil = map[w - 1][h - 1][d - 1];
	}

	/* Shuffles the current field. */
	void shuffle();

	/* Tries to move the empty ceil and
	returns `true` if succeed, `false` otherwise. */
	bool swapWith(short direction);

	/* Moves the empty ceil to specified position. */
	void swapWith(short x, short y, short z);

	/* Returns `true` if the puzzle has been solved,
	`false` otherwise. */
	bool isComplete();

	void onComplete() {
		pause();
		// Notify everyone that the user has
		// completed the game.
		callback(COMPLETED);
	}

	void newGame() {
		shuffle();
		steps = 0;
		clock_cc = 0;
		clock_dd = 0;
	}

	void resume() {
		cout << "Logic is being resumed." << endl;
		resumed = true;
		// Clock
		clock = std::clock();
		clock_dd = clock_cc;
		clock_cc = 0;
		// Notify everyone that the user has
		// resumed the game.
		callback(RESUMED);
	}

	void pause() {
		// Notify everyone that the user has
		// resumed the game.
		callback(PAUSED);

		clock_cc = getTime();
		// Other
		cout << "Logic is being paused." << endl;
		resumed = false;
	}

	bool isPaused() {
		return !resumed;
	}

	int getSteps() {
		return steps;
	}

	int getTime() {
		return isPaused()
			? clock_cc  /* return paused time */
			: (std::clock() - clock) / 1000 + clock_dd;
	}
};