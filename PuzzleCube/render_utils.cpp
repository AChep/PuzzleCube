#include <GLUT/glut.h>

#include "render_utils.h"

void rutils_internal::renderText(void *font, const char* text, float x, float y)
{
	for (; *text; text++) glutBitmapCharacter(font, *text);
}

void rutils_internal::renderInt(void *font, int value, float x, float y, bool two_zeros)
{
	// Calc the length of the value.
	int n = 1;
	for (int v = value; v; v /= 10, n *= 10);

	// 01 format
	if (value <= 9 && two_zeros) glutBitmapCharacter(font, '0');
	// Display zero
	if (value == 0) glutBitmapCharacter(font, '0');

	while (n > 9)
	{
		int val = (value / (n /= 10)) % 10;
		char ch = (char)((int) '0' + val);
		glutBitmapCharacter(font, ch);
	}
}

void rutils::renderInt(void *font, int value, float x, float y)
{
	glRasterPos3f(x, y, 0.01f);
	rutils_internal::renderInt(font, value, x, y, false);
}

void rutils::renderText(void *font, const char* text, float x, float y)
{
	glRasterPos3f(x, y, 0.01f);
	rutils_internal::renderText(font, text, x, y);
}

void rutils::renderTime(void *font, int time, float x, float y)
{
	glRasterPos3f(x, y, 0.01f);

	int ss = time % 60;
	int mm = (time /= 60) % 60;
	int hh = (time /= 60);

	rutils_internal::renderInt(font, hh, x, y, true);
	rutils_internal::renderText(font, ":", x, y);
	rutils_internal::renderInt(font, mm, x, y, true);
	rutils_internal::renderText(font, ":", x, y);
	rutils_internal::renderInt(font, ss, x, y, true);
}