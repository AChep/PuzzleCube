#pragma once

class MenuItem {
public:
	int id;
	char* label;
	unsigned int width;
	unsigned int height;
	// State
	bool highlighted;

	MenuItem(int id,
		char* label,
		unsigned int width,
		unsigned int height) {
		this->id = id;
		this->label = label;
		this->width = width;
		this->height = height;
	}
};
