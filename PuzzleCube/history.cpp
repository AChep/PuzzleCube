#include "history.h"

namespace history {
	const char* FILENAME = "__history.txt";
}

/* Puts the result into a db so stats can be created */
void history::put(Result* result) {
	std::ofstream stream;
	stream.open(FILENAME, std::ios_base::app);
	stream << ' ' << result->time << ' ' << result->steps;
}

/* Returns the best result */
Result* history::getBest() {
	std::ifstream stream;
	stream.open(FILENAME);

	Result* a = new Result(INT_MAX, INT_MAX);
	Result* b = new Result(INT_MAX, INT_MAX);
	while (stream >> a->time >> a->steps)
	{
		if (a->getScore() > b->getScore() || b->time == INT_MAX) {
			b->time = a->time;
			b->steps = a->steps;
		}
	}

	stream.close();
	return b->time == INT_MAX ? 0 : b;
}

/* Prints all results */
void history::printAll() {
	std::ifstream stream;
	stream.open(FILENAME);

	Result* a = new Result(INT_MAX, INT_MAX);
	while (stream >> a->time >> a->steps)
	{
		std::cout << "Score: " << a->getScore() << " Time: " << a->time << " Steps: " << a->steps << std::endl;
	}

	stream.close();
}