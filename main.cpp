/*
CPSC 589 Assignment 1 (W2017)
Geordie Tait 10013837

Based upon starter code given in tutorial
Compiled and tested using Visual Studio Community 2015 on Windows 10
*/

#ifdef _WIN32
#include <Windows.h>
#endif

#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <math.h>
using namespace std;

#define PI 3.14159265359

// Global vars
GLFWwindow *window;
int w, h;
double mouseX, mouseY;
bool rotating = false;

// Hypocycloid values
int n = 10;						// number of hypocycloid cycles
float smallr =    0.081f;		// radius of small circle
float smallrInc = 0.001f;		// increment for small circle radius
float bigr =    0.802f;			// radius of big circle
float bigrInc = 0.002f;			// increment for big circle radius
float increments = 20000.f;		// total number of points used

// Display values
float scale = 1.1f;				// scale factor
float zoomSpeed = 0.02f;		// speed for zooming in/out
float angle = 0.f;				// rotation angle
float rotSpeed =   100.f;		// speed for changing rotation
float colourAnim = 0.f;			// factor for colour animation
float colourAnimSpeed = 4.f;	// how fast to animate colour
int colourMode = 0;				// 0 = animated, 1 = colour, 2 = white

/*
Rendering function
*/
void render() {
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// transformation matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScalef(float(h) / float(w), 1.0f, 1.0f);	// correct for aspect ratio
	glScalef(scale, scale, scale);				// scale by factor
	glRotatef(angle, 0.0f, 0.0f, 1.0f);			// rotate by angle
    
    // precalculate values for drawing
	float tmax = 2.f * PI * float(n);			// maximum t value
	float tinc = tmax / increments;				// t increment
	float a = bigr - smallr;					// factors for hypocycloid equations
	float b = a / smallr;
	float rMod = (1.f + sin(colourAnim)) / 2.f;	// mod factors for each colour
	float gMod = (1.f + cos(colourAnim)) / 2.f; // (oscillate between 0 and 1 if animating)
	float bMod = 1.f - (1.f + cos(colourAnim)) / 2.f;
	double time = glfwGetTime();				// get initial time for animation

	// begin drawing
	glBegin(GL_LINE_STRIP);

	// set vertex position and colour for each t value
	for (float t = 0.f; t <= tmax; t += tinc) {
		// vertices
		float bt = b * t;
		float x = a * cos(t) + smallr * cos(bt);
		float y = a * sin(t) - smallr * sin(bt);
		glVertex2f(x, y);

		// colours
		float colR, colG, colB;
		if (colourMode == 2) {  // white only
			colR = 1.f;
			colG = 1.f;
			colB = 1.f;
		}
		else {  // varied colour
			float progress = t / tmax;
			colR = 1.f - rMod * progress;
			colG = gMod * progress;
			colB = bMod * (1.f + cos(2.f*PI*progress)) / 2.f;
		}
		glColor3f(colR, colG, colB);
	}

	// if animating, update colour animation value using time
	if (colourMode == 0) {
		colourAnim += colourAnimSpeed * (glfwGetTime() - time);
		if (colourAnim >= 2.f * PI) colourAnim = 0.f;
	}

	glEnd();
}

/*
Keyboard event function
*/
void keyboard(GLFWwindow *sender, int key, int scancode, int action, int mods) {
	// close window
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	// increase number of cycles
	if (key == GLFW_KEY_PAGE_UP && (action == GLFW_PRESS || action == GLFW_REPEAT))
		n++;

	// decrease number of cycles
	if (key == GLFW_KEY_PAGE_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT))
		if (n > 1) n--;

	// increase small circle radius
	if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT))
		smallr += smallrInc;

	// decrease small circle radius
	if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT))
		smallr -= smallrInc;

	// decrease large circle radius
	if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT))
		bigr -= bigrInc;

	// increase large circle radius
	if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT))
		bigr += bigrInc;

	// cycle through colour modes
	if (key == GLFW_KEY_DELETE && (action == GLFW_PRESS)) {
		colourMode++;
		if (colourMode > 2) colourMode = 0;
	}
}

/*
Mouseclick event function
*/
void mouseClick(GLFWwindow *sender, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		// when pressed, store mouse location and set rotating flag
		if (action == GLFW_PRESS) {
			glfwGetCursorPos(window, &mouseX, &mouseY);
			rotating = true;
		}
		// when released unset rotating flag
		else if (action == GLFW_RELEASE)
			rotating = false;
	}
}

/*
Mouse scroll event function
*/
void mouseScroll(GLFWwindow *sender, double x, double y) {
	// increase or decrease scale factor
	scale += zoomSpeed * y;
}

/*
Mouse position event function
*/
void mousePos(GLFWwindow *sender, double x, double y) {
	// rotate by dragging left or right while left clicking
	if (rotating) {
		angle -= rotSpeed * double(x - mouseX) / double(w);
		mouseX = x;
		mouseY = y;
	}
}

/*
Main program function
*/
int main() {
	// initialize GLFW
	if (!glfwInit()) return 1;

    // antialiasing
    glfwWindowHint(GLFW_SAMPLES, 4);

	// create window
	window = glfwCreateWindow(1024, 768, "Geordie Tait CPSC589 A1", NULL, NULL);
	if (!window) return 1;
	glfwMakeContextCurrent(window);

	// set callback functions
	glfwSetKeyCallback(window, keyboard);
	glfwSetMouseButtonCallback(window, mouseClick);
	glfwSetCursorPosCallback(window, mousePos);
	glfwSetScrollCallback(window, mouseScroll);

	// main loop
	while (!glfwWindowShouldClose(window)) {
		glfwGetFramebufferSize(window, &w, &h);
		glViewport(0, 0, w, h);

		render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// clean up and exit
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

