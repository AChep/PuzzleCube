#pragma once

#include <GLUT/glut.h>

#include "menu_item.h"
#include "math_utils.h"

static const float COLOR_NORMAL[] = { 0.1f, 0.1f, 0.1f };
static const float COLOR_HIGHLIGHTED[] = { .5f, 0.2f, 0.08f };
static const float COLOR_PREPARED[] = { 1.f, 0.3f, 0.15f };

static const int PADDING = 10;

class Menu {
private:
	MenuItem** items;
	MenuItem*  selectedItem;
	void(*callback)(int);
	unsigned int length;
	unsigned int width;
	unsigned int height;
	unsigned int screenWidth;
	unsigned int screenHeight;

	MenuItem* findMenuItem(int x, int y)
	{
		// Normalize the coordinates.
		x -= (screenWidth - width) / 2;
		y -= (screenHeight - height) / 2;
		unsigned int h = 0;
		// Find the item 
		for (unsigned int i = 0; i < length; i++) {
			MenuItem* item = items[i];
			if (x >= 0 && x <= item->width
				&& y >= h && y <= h + item->height
				&& items[i]->id) return items[i];
			h += item->height + PADDING;
		}
		return 0;
	}


	int lengthHorizontal(const char* text) {
		int m = 0;
		int n = 0;
		while (*text) {
			if (*text == 10) {
				if (n > m) m = n;
				n = 0;
			}
			else {
				n++;
			}
			text++;
		}
		return max(n, m);
	}


	int lengthVertical(const char* text) {
		int n = 1;
		while (*text) {
			if (*text == 10) n++;
			text++;
		}
		return n;
	}


	void acString(void *font, const char* text, float x, float y)
	{
		if (!text) return;
		float ratio = (float)screenWidth / screenHeight;
		float xn = (x / screenWidth * 2 - 1) * ratio;
	new_line:
		float yn = y / screenHeight * 2 - 1;
		glRasterPos3f(xn, -yn, 0.01f);
		while (*text)
		{
			if (*text == 10) { 
				text++; 
				y += 20; // Move carete
				goto new_line; 
			}
			glutBitmapCharacter(font, *text);
			text++;
		}
	}


	void acVertex2f(float x, float y)
	{
		float ratio = (float)screenWidth / screenHeight;
		x = (x / screenWidth * 2 - 1) * ratio;
		y = y / screenHeight * 2 - 1;
		glVertex2f(x, -y);
	}
public:

	Menu(MenuItem** items, unsigned int length, void(*f)(int)) {
		this->callback = f;
		this->items = items;
		this->length = length;
		// Calculate total width and height
		for (unsigned int i = 0; i < length; i++) {
			int w = items[i]->width;
			if (w > width) width = w;
			height += items[i]->height;
		}
	}


	void windowMouseMotionPassive(int x, int y)
	{
		windowMouseMotion(x, y);
	}


	void windowMouseMotion(int x, int y)
	{
		// Hover effect
		for (int i = 0; i < length; i++) items[i]->highlighted = false;
		MenuItem* item = findMenuItem(x, y);
		if (item != 0) item->highlighted = true;
		if (item != selectedItem) selectedItem = 0;
	}


	/*
	 *  GLUT calls this routine when a mouse button is clicked
	 */
	void windowMouse(int btn, int state, int x, int y)
	{
		if (state == 1) {
			if (selectedItem != 0) {
				callback(selectedItem->id);
				selectedItem->highlighted = false;
			}
		}
		else switch (btn) {
		case 0: // Left
			selectedItem = findMenuItem(x, y);
			break;
		default:
			selectedItem = 0;
		}
	}


	void reshape(int width, int height)
	{
		screenWidth = width;
		screenHeight = height;
	}


	void render()
	{
		unsigned int x = (screenWidth - width) / 2;
		unsigned int y = (screenHeight - height) / 2;
		for (int i = 0; i < length; i++) {
			MenuItem* item = items[i];

			// Draw the item
			const float* color = item->highlighted
				? selectedItem
				? COLOR_PREPARED
				: COLOR_HIGHLIGHTED
				: COLOR_NORMAL;
			glColor3f(color[0], color[1], color[2]);
			glBegin(GL_POLYGON);
			acVertex2f(x, y);
			acVertex2f(x + item->width, y);
			acVertex2f(x + item->width, y + item->height);
			acVertex2f(x, y + item->height);
			glEnd();

			// Draw label
			glColor3f(1, 1, 1);
			acString(GLUT_BITMAP_HELVETICA_18, item->label,
				x + max(item->width - lengthHorizontal(item->label) * 8.2, 0) / 2,
				y + max(item->height - (lengthVertical(item->label) - 1) * 19 + 10, 0) / 2);

			y += item->height + PADDING;
		}
	}
};