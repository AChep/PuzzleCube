#include <GL/glut.h>
#include <math.h>
#include <iostream>

#include "render_utils.h"
#include "history.h"
#include "logic_bp.h"
#include "logic.h"
#include "menu.h"

using std::cout;
using std::endl;

// States
static const int STATE_MENU = 0;
static const int STATE_MENU_STATS = 4;
static const int STATE_MENU_ABOUT = 5;
static const int STATE_MENU_HELP = 6;
static const int STATE_GAME = 1;
static const int STATE_GAME_COMPLETED = 2;
static const int STATE_GAME_PAUSED = 3;

// Ids
static const int ID_EMPTY = 0;
// Back
static const int ID_BACK_TO_MENU = 1;
static const int ID_BACK_TO_OS = 2;
// Menu
static const int ID_MENU_STATS = 400;
static const int ID_MENU_ABOUT = 500;
static const int ID_MENU_HELP = 600;
// Game
static const int ID_GAME_NEW = 1000;
static const int ID_GAME_LOAD = 2000;
static const int ID_GAME_RESUME = 3000;
static const int ID_GAME_PAUSE = 4000;
static const int ID_GAME_SAVE = 5000;

static const int KEY_Q = 81;
static const int KEY_q = KEY_Q + 32;
static const int KEY_W = 87;
static const int KEY_w = KEY_W + 32;
static const int KEY_E = 69;
static const int KEY_e = KEY_E + 32;
static const int KEY_A = 65;
static const int KEY_a = KEY_A + 32;
static const int KEY_S = 83;
static const int KEY_s = KEY_S + 32;
static const int KEY_D = 68;
static const int KEY_d = KEY_D + 32;

// Render
static int   button = -1;
static int   w, h;
static float ph, th;
static float mouse_x, mouse_y;
static float dzoom = 1.0f;
static float  zoom = 1.0f;

// Puzzle
static Logic* logic;

// Screens
static int _state;
static Menu*  menu;
static Menu*  menuStats;
static Menu*  menuAbout;
static Menu*  menuHelp;
static Menu*  menuComplete;
static Menu*  menuPause;

// Extern
extern void menuBtnClick(int);


/* Sets the new viewport and perspective, based on the
   field's width and height, zoom parameter. */
void reshape(int width, int height)
{
	cout << "Reshaping..." << endl;
	cout << "width=" << width << endl;
	cout << "height=" << height << endl;

	if (_state != STATE_GAME) {
		zoom = 1 / dzoom; // No zoom at menu allowed
	}

	const double ratio = (float)width / height;
	const double fieldMax = 179.0;
	double field = 45.0 / (dzoom * zoom);
	// Check the field
	if (field > fieldMax) {
		field = fieldMax;
		zoom = 45.0 / (dzoom * fieldMax); // Fix zoom 
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(field, ratio, 1.0, 1000.0);
	glViewport(0, 0, width, height);

	// Reshape menu
	menu->reshape(width, height);
	menuStats->reshape(width, height);
	menuAbout->reshape(width, height);
	menuHelp->reshape(width, height);
	menuComplete->reshape(width, height);
	menuPause->reshape(width, height);

	w = width;
	h = height;
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.f, 0.f, 2.4f, 0.f, 0.f, -5.f, 0.f, 1.f, 0.f);

	switch (_state)
	{
	case STATE_MENU:
		menu->render();
		break;
	case STATE_MENU_STATS:
		menuStats->render();
		break;
	case STATE_MENU_ABOUT:
		menuAbout->render();
		break;
	case STATE_MENU_HELP:
		menuHelp->render();
		break;
	default:
		float c[4];

		/* draw info */
		c[0] = 0.8f;
		c[1] = 0.8f;
		c[2] = 0.8f;
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c);
		rutils::renderText(GLUT_BITMAP_TIMES_ROMAN_24, " ", 0, 0); // Prevent drawing a black letter
		rutils::renderText(GLUT_BITMAP_TIMES_ROMAN_24, " time:", -0.18, 0.7);
		rutils::renderTime(GLUT_BITMAP_TIMES_ROMAN_24, logic->getTime(), 0, 0.7);
		rutils::renderText(GLUT_BITMAP_TIMES_ROMAN_24, "steps:", -0.18, 0.6);
		rutils::renderInt(GLUT_BITMAP_TIMES_ROMAN_24, logic->getSteps(), 0, 0.6);

		if (_state == STATE_GAME_COMPLETED) {
			menuComplete->render();
			break; // Don't draw the game cube
		}
		else if (_state == STATE_GAME_PAUSED) {
			menuPause->render();
			break; // Don't draw the game cube
		}

		/* draw 2d slices */
		const short w = logic->field->width;
		const short h = logic->field->height;
		const short d = logic->field->depth;
		for (short i = 0; i < w; i++) {
			for (short j = 0; j < h; j++) {
				for (short k = 0; k < d; k++) {
					float p = 0.1f;

					Ceil* ceil = logic->map[i][j][k];
					if (ceil == logic->ceil) continue;

					glPushMatrix();

					c[0] = (float)ceil->id_x / w;
					c[1] = (float)ceil->id_y / h;
					c[2] = (float)ceil->id_z / d;
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c);
					glTranslatef(
						(i - (w - 1) / 2.f) * p - (k - (d - 1) / 2.f) / 1.5f,
						-(j - (h - 1) / 2.f) * p - 0.6f,
						0);
					glutSolidCube(0.1);

					c[0] = 1 - (float)ceil->id_x / w;
					c[1] = 1 - (float)ceil->id_y / h;
					c[2] = 1 - (float)ceil->id_z / d;
					c[0] *= c[0]; c[1] *= c[1]; c[2] *= c[2]; // Moar contrast!
					c[0] *= c[0]; c[1] *= c[1]; c[2] *= c[2]; // Moar contrast!
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c);
					glTranslatef(0, 0, 0.05);
					rutils::renderInt(GLUT_BITMAP_HELVETICA_18, ceil->id, (k - (d - 1) / 2.f) * 0.015 - 0.015, -0.005);

					glPopMatrix();
				}
			}
		}

		/* rotate it around the y, x axis */
		glRotatef(th, 0.0f, 1.0f, 0.0f);
		glRotatef(ph, 1.0f, 0.0f, 0.0f);

		for (short i = 0; i < w; i++) {
			for (short j = 0; j < h; j++) {
				for (short k = 0; k < d; k++) {
					float p = 0.2f;

					Ceil* ceil = logic->map[i][j][k];
					if (ceil == logic->ceil) continue;

					c[0] = (float)ceil->id_x / w;
					c[1] = (float)ceil->id_y / h;
					c[2] = (float)ceil->id_z / d;
					glPushMatrix();
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c);
					glTranslatef(
						(i - (w - 1) / 2.f) * p,
						-(j - (h - 1) / 2.f) * p,
						(k - (d - 1) / 2.f) * p);
					glutSolidCube(0.1);
					glPopMatrix();
				}
			}
		}
		break;
	}

	glFlush();
	glutSwapBuffers();
}

// ----------------
// --- GLUT EVENTS 
// ----------------

void windowMouseMotion(int x, int y)
{
	switch (_state) {
	case STATE_MENU:
		menu->windowMouseMotion(x, y);
		break;
	case STATE_MENU_STATS:
		menuStats->windowMouseMotion(x, y);
		break;
	case STATE_MENU_ABOUT:
		menuAbout->windowMouseMotion(x, y);
		break;
	case STATE_MENU_HELP:
		menuHelp->windowMouseMotion(x, y);
		break;
	case STATE_GAME_COMPLETED:
		menuComplete->windowMouseMotion(x, y);
		break;
	case STATE_GAME_PAUSED:
		menuPause->windowMouseMotion(x, y);
		break;
	case STATE_GAME:
		if (logic->isComplete()) {
			menuComplete->windowMouseMotion(x, y);
			break;
		}
		else if (logic->isPaused()) {
			menuPause->windowMouseMotion(x, y);
			break;
		}

		switch (button) {
		case 0:
		{
				  float factor = zoom * dzoom;
				  if (factor < 1) factor = 1;
				  th += (x - mouse_x) / factor; mouse_x = x;
				  ph += (y - mouse_y) / factor; mouse_y = y;
		}

			/*  Keep angles to +/-360 degrees */
			while (th >= 360) th -= 360;
			while (ph >= 360) ph -= 360;
			break;
		case 2: {
					if (w && h) {
						double cx = w / 2.0, cy = h / 2.0;
						double start = hypot(cx - mouse_x, cy - mouse_y);
						double end = hypot(cx - x, cy - y);
						dzoom = end / start;

						// Update field of view
						reshape(w, h);
					}
					break;
		}
		default:
			// Do not redisplay
			return;
		}
		break;
	}

	glutPostRedisplay();
}

void windowMouseMotionPassive(int x, int y)
{
	switch (_state)
	{
	case STATE_MENU:
		menu->windowMouseMotionPassive(x, y);
		break;
	case STATE_MENU_STATS:
		menuStats->windowMouseMotionPassive(x, y);
		break;
	case STATE_MENU_ABOUT:
		menuAbout->windowMouseMotionPassive(x, y);
		break;
	case STATE_MENU_HELP:
		menuHelp->windowMouseMotionPassive(x, y);
		break;
	case STATE_GAME_COMPLETED:
		menuComplete->windowMouseMotionPassive(x, y);
		break;
	case STATE_GAME_PAUSED:
		menuPause->windowMouseMotionPassive(x, y);
		break;
	case STATE_GAME:
		if (logic->isComplete()) {
			menuComplete->windowMouseMotionPassive(x, y);
		}
		else if (logic->isPaused()) {
			menuPause->windowMouseMotionPassive(x, y);
		}
		break;
	}

	glutPostRedisplay();
}


/*
 *  GLUT calls this routine when a mouse button is clicked
 */
void windowMouse(int btn, int state, int x, int y)
{
	switch (_state)
	{
	case STATE_MENU:
		menu->windowMouse(btn, state, x, y);
		break;
	case STATE_MENU_STATS:
		menuStats->windowMouse(btn, state, x, y);
		break;
	case STATE_MENU_ABOUT:
		menuAbout->windowMouse(btn, state, x, y);
		break;
	case STATE_MENU_HELP:
		menuHelp->windowMouse(btn, state, x, y);
		break;
	case STATE_GAME_COMPLETED:
		menuComplete->windowMouse(btn, state, x, y);
		break;
	case STATE_GAME_PAUSED:
		menuPause->windowMouse(btn, state, x, y);
		break;
	case STATE_GAME:
		if (logic->isComplete()) {
			menuComplete->windowMouse(btn, state, x, y);
		}
		else if (logic->isPaused()) {
			menuPause->windowMouse(btn, state, x, y);
		}
		break;
	}

	if (state == 1) {
		button = -1;
	}
	else switch (button = btn) {
	case 2: // Right
		zoom *= dzoom;
		dzoom = 1.0f;
	case 0: // Left
		mouse_x = x;
		mouse_y = y;
		break;
	}
}


/*
 * GLUT calls this routine when an arrow key is pressed
 */
void windowKeySpecial(int key, int x, int y)
{
	/*  Right arrow key - increase azimuth by 5 degrees */
	if (key == GLUT_KEY_RIGHT) th += 5;
	/*  Left arrow key - decrease azimuth by 5 degrees */
	else if (key == GLUT_KEY_LEFT) th -= 5;
	/*  Up arrow key - increase elevation by 5 degrees */
	else if (key == GLUT_KEY_UP) ph += 5;
	/*  Down arrow key - decrease elevation by 5 degrees */
	else if (key == GLUT_KEY_DOWN) ph -= 5;
	else return;

	/*  Keep angles to +/-360 degrees */
	while (th >= 360) th -= 360;
	while (ph >= 360) ph -= 360;

	glutPostRedisplay();
}

void windowTimerRender(int value) {
	glutPostRedisplay();
	if (!logic->isPaused()) glutTimerFunc(100, windowTimerRender, 0);
}


void setState(int state) {
	if (_state == state) return;
	_state = state;
	// Update mode
	if (_state == STATE_GAME) {
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_DEPTH_TEST);
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
		glEnable(GL_NORMALIZE);
		glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	}
	else {
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_NORMALIZE);

		if (_state == STATE_MENU_STATS) {
			cout << endl << "Stats:" << endl;
			history::printAll();
			cout << endl;
		}
	}
	// Update zoom & field
	reshape(w, h);
	// Render
	glutPostRedisplay();
}


/*
 *  GLUT calls this routine when a non-special key is pressed
 */
void windowKey(unsigned char key, int x, int y)
{
	if (key == 27) {
		if (_state == STATE_MENU) {
			/*  Exit on ESC */
			exit(0);
		}
		else if (_state != STATE_GAME
			&& _state != STATE_GAME_PAUSED) {
			/* Exit to main mune from any other screen except
			   the game */
			setState(STATE_MENU);
		}
		else {
			/* Switch Pause/Resume states */
			menuBtnClick(logic->isPaused()
				? ID_GAME_RESUME
				: ID_GAME_PAUSE);
		}
	}
	else {
		if (_state == STATE_GAME)
			switch (key)
		{
			case KEY_Q:
			case KEY_q:
				logic->swapWith(Logic::swap_consts::BELOW);
				break;
			case KEY_E:
			case KEY_e:
				logic->swapWith(Logic::swap_consts::ABOVE);
				break;
			case KEY_W:
			case KEY_w:
				logic->swapWith(Logic::swap_consts::TOP);
				break;
			case KEY_S:
			case KEY_s:
				logic->swapWith(Logic::swap_consts::BOTTOM);
				break;
			case KEY_A:
			case KEY_a:
				logic->swapWith(Logic::swap_consts::LEFT);
				break;
			case KEY_D:
			case KEY_d:
				logic->swapWith(Logic::swap_consts::RIGHT);
				break;
			default:
				cout << "Skipping key press." << endl;
				return;

		}
	}

	glutPostRedisplay();
}

// ----------------
// --- MAIN
// ----------------

void logicCallback(int event) {
	switch (event)
	{
	case Logic::COMPLETED:
	{
		cout << "Completed!" << endl;

		// Save result
		Result* result = new Result(logic->getTime(), logic->getSteps());
		history::put(result);
		delete result;

		setState(STATE_GAME_COMPLETED);
	}
		break;
	case Logic::NEW_GAME:
	case Logic::RESUMED:
		windowTimerRender(0);
		break;
	case Logic::PAUSED:
		break;
	}
}

void menuBtnClick(int id) {
	switch (id)
	{
	case ID_MENU_STATS:
		setState(STATE_MENU_STATS);
		break;
	case ID_MENU_ABOUT:
		setState(STATE_MENU_ABOUT);
		break;
	case ID_MENU_HELP:
		setState(STATE_MENU_HELP);
		break;

		/* go to main menu */
	case ID_BACK_TO_MENU:
		setState(STATE_MENU);
		break;
		/* exit */
	case ID_BACK_TO_OS:
		exit(0);
		return;

		/* new game */
	case ID_GAME_NEW:
		setState(STATE_GAME);
		logic->newGame();
		logic->resume();
		break;
		/* load the game */
	case ID_GAME_LOAD:
		setState(STATE_GAME);
		logic_bp::load(logic);
		logic->resume();
		break;
		/* resume the game */
	case ID_GAME_RESUME:
		setState(STATE_GAME);
		if (logic->isPaused()) logic->resume();
		break;
		/* pause the game */
	case ID_GAME_PAUSE:
		if (!logic->isPaused()) logic->pause();
		setState(STATE_GAME_PAUSED);
		break;
		/* load the game */
	case ID_GAME_SAVE:
		logic_bp::save(logic);
		break;
	}

	glutPostRedisplay();
}

void initLogic()
{
	Field* field = new Field();
	field->depth = field->height
		= field->width
		= 3; // Cubic field
	logic = new Logic(field, &logicCallback);
}


void initMenu()
{
	MenuItem* space = new MenuItem(ID_EMPTY, " ", 0, 16);
	MenuItem* back = new MenuItem(ID_BACK_TO_MENU, "<--", 64, 48);

	/* Main menu of the game */
	menu = new Menu(new MenuItem*[] {
			new MenuItem(ID_GAME_NEW, "New game", 156, 48),
			new MenuItem(ID_GAME_LOAD, "Load", 156, 48),
			new MenuItem(ID_MENU_STATS, "Stats", 156, 48),
			new MenuItem(ID_MENU_ABOUT, "About", 156, 48),
			new MenuItem(ID_MENU_HELP, "Help", 156, 48),
			new MenuItem(ID_BACK_TO_OS, "Exit", 156, 48),
	}, 6, &menuBtnClick);

	/* Stats */
	menuStats = new Menu(new MenuItem*[] { 
			new MenuItem(ID_EMPTY, "The stats has been printed\nto console.", 256, 86),
			space,
			back,
	}, 3, &menuBtnClick);

	/* About */
	menuAbout = new Menu(new MenuItem*[] {
			new MenuItem(ID_EMPTY, "(c) Artem Chepurnoy\n<mail@artemchep.com>\n\nPuzzle Cube is a free open\nsource game, created by a\nC++/OGL newbie as the uni\nproject.", 256, 200),
			space,
			back,
	}, 3, &menuBtnClick);

	/* Help */
	menuHelp = new Menu(new MenuItem*[] {
			new MenuItem(ID_EMPTY, "Rearrange the tiles as\nfollows:\n\n19-20-21  10-11-12  1-2-3\n22-23-24  13-14-15  4-5-6\n25-26        16-17-18  7-8-9\n\nusing Q,A,W,S,E,D keys\nto move.", 256, 200),
			space,
			back,
	}, 3, &menuBtnClick);

	/* Pause */
	menuPause = new Menu(new MenuItem*[] {
			new MenuItem(ID_GAME_NEW, "New game", 156, 48),
			new MenuItem(ID_GAME_RESUME, "Resume", 156, 48),
			new MenuItem(ID_GAME_SAVE, "Save", 156, 48),
			new MenuItem(ID_GAME_LOAD, "Load", 156, 48),
			space,
			back,	
	}, 6, &menuBtnClick);

	/* You have won the game */
	menuComplete = new Menu(new MenuItem*[] {
			new MenuItem(ID_EMPTY, "Congratulations!\nYou have solved the puzzle!", 256, 48),
			space,
			back,
	}, 3, &menuBtnClick);
}


void main(int argc, char **argv)
{
	initLogic();
	initMenu();

	glutInit(&argc, argv);
	glutInitWindowSize(1024, 786);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	glutCreateWindow("Puzzle Cube <mail@artemchep.com>");
	glutFullScreen();

	glutDisplayFunc(render);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(windowKey);
	glutSpecialFunc(windowKeySpecial);
	glutMouseFunc(windowMouse);
	glutPassiveMotionFunc(windowMouseMotionPassive);
	glutMotionFunc(windowMouseMotion);

	glClearColor(.15f, .15f, .15f, 1.f);
	glutMainLoop();
}