#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
using namespace std;

// Global variables for window's size
int width = 1200;
int height = 600;

// Global variables for moving camera
int mouseButton = 0;
bool mouseRotate = false;
bool mouseZoom = false;
bool mousePan = false;
int xClick = 0;
int yClick = 0;
int xAux = 0;
int yAux = 0;
double rotX = 0.;
double rotY = 0.;
double panX = 0.;
double panY = 0.;
double zoomZ = -20.;

//NEW GLOBAL VARIABLES

//The only variables that can be modified
int m = 3;
int n = 2;
//End

int mainWindow, subWindow1, subWindow2, subWindow3;
int maxFloors = m+n;						//Maximum number of random floors

float rate = 1 / (float)maxFloors;			//Rate of decreasing size of the floors in a building
float velocity = m*n*0.005 / 6;				//Velocity of the car

int * randoms = new int[m * m * n * n];		//Array with the random numbers
int cont = 0;								//Iterator of the array of random numbers

int ** intersectionsX = new int*[m + 1];	//'x' coordinates of the intersections
int ** intersectionsZ = new int*[m + 1];	//'z' coordinates of the intersections

float movementX = 0;						//'x' position of the car
float movementZ = 0;						//'z' position of the car

int flag = 0;								//Determines the direction of the car: flag==0 right; flag==1 left; flag==2 up; flag==3 down 
int previousFlag = 0;						//Previous direction of the car (in order to avoid the car to return)

int angle = 0;								//Direction of the car: angle==0 right; angle==90 up; angle==180 left; angle==270 down
int random;									//The random direction

float alpha = 0.005;						//Angle of the last view

//END OF THE NEW GLOBAL VARIABLES

void init(){
	glutSetWindow(subWindow1);
	//glClearColor(1.0, 0.0, 0.0, 0.0);
	//glColor3f(0.0, 0.0, 0.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0., 0., 0.); // Camera/World initial position

	glutSetWindow(subWindow2);
	//glClearColor(0.0, 1.0, 0.0, 0.0);
	//glColor3f(0.0, 0.00001, 0.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0., 0., 0.); // Camera/World initial position

	glutSetWindow(subWindow3);
	//glClearColor(0.0, 0.0, 1.0, 0.0);
	//glColor3f(1.0, 1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0., 0., 0.); // Camera/World initial position
}

void reshape(int w, int h){
	if (height == 0){
		height = 1;
	}
	width = w;
	height = h;

	glutSetWindow(subWindow1);
	glutPositionWindow(0, 0);
	glutReshapeWindow(width / 2, height / 2);

	glutSetWindow(subWindow2);
	glutPositionWindow(width / 2, 0);
	glutReshapeWindow(width / 2, height / 2);

	glutSetWindow(subWindow3);
	glutPositionWindow(0, height / 2);
	glutReshapeWindow(width, height / 2);
}

void reshape1(int w, int h){
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (h == 0){
		h = 1;
	}
	gluPerspective(25.0, (double)w / h, 0.1, 1000.0);	//proyección en perspectiva
	glMatrixMode(GL_MODELVIEW);
}

void reshape2(int w, int h){
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (h == 0){
		h = 1;
	}
	gluPerspective(25.0, (double)w / h, 0.1, 1000.0);	//proyección en perspectiva
	glMatrixMode(GL_MODELVIEW);
}

void reshape3(int w, int h){
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (h == 0){
		h = 1;
	}
	gluPerspective(25.0, (double)w / h, 0.1, 1000.0);	//proyección en perspectiva
	glMatrixMode(GL_MODELVIEW);
}

void displayAxis(){
	glBegin(GL_LINES);
	glVertex3f(0., 0., 0.);
	glVertex3f(1., 0., 0.);
	glEnd();
}

void displayCoordinateFrame(){
	glPushMatrix();
	glColor3f(1., 0., 0.);
	displayAxis();				//Eje X
	glPushMatrix();
	glRotatef(90., 0., 0., 1.);
	glColor3f(0., 1., 0.);
	displayAxis();				//Eje Y
	glPopMatrix();
	glColor3f(0., 0., 1.);
	glRotatef(-90., 0., 1., 0.); //Eje Z
	displayAxis();
	glColor3f(1., 1., 1.);
	glPopMatrix();
}

void displayBuilding(){
	glPushMatrix();
	glTranslatef(1., .5, 1.);
	glutWireCube(1.);
	float t = 1;
	for (int i = 0; i < randoms[cont]; i++){
		glTranslatef(0., 1., 0.);
		t -= rate / 2;
		glScalef(t, 1., t);
		glutWireCube(1.);
	}
	glPopMatrix();
	cont++;
	if (cont == m*m*n*n){
		cont = 0;
	}
}

void displayBlock(){
	glPushMatrix();
	for (int i = 0; i < m; i++){
		glPushMatrix();
		for (int j = 0; j < n; j++){
			displayBuilding();
			glTranslatef(1., 0., 0.);
		}
		glPopMatrix();
		glTranslatef(0., 0., 1.);
	}
	glPopMatrix();
}

void displayCity(){
	glPushMatrix();
	for (int i = 0; i < m; i++){
		glPushMatrix();
		for (int j = 0; j < n; j++){
			displayBlock();
			glTranslatef(n + 1, 0., 0.);
		}
		glPopMatrix();
		glTranslatef(0., 0., m + 1);
	}
	glPopMatrix();
}

void displayIntersections(){
	glPushMatrix();
	int i = 0, j = 0;
	for (int i = 0; i < m + 1; i++){
		glPushMatrix();
		for (int j = 0; j < n + 1; j++){
			glPushMatrix();
			glTranslatef(intersectionsX[i][j], 0., intersectionsZ[i][j]);
			glutSolidSphere(0.2, 10., 10.);
			glPopMatrix();
		}
		glPopMatrix();
		glPushMatrix();
		glTranslatef(intersectionsX[i][j], 0., intersectionsZ[i][j]);
		glPopMatrix();
	}
	glPopMatrix();
}

void calculate(int i, int j){
	//If the car is in one of the four corners
	if ((i == 0 && j == 0) || (i == m && j == 0) || (i == 0 && j == n) || (i == m && j == n)){
		if (i == 0 && j == 0){
			if (previousFlag == 1){
				random = 3;
			}
			else{
				random = 0;
			}
		}
		else if (i == m && j == 0){
			if (previousFlag == 3){
				random = 0;
			}
			else{
				random = 2;
			}
		}
		else if (i == 0 && j == n){
			if (previousFlag == 0){
				random = 3;
			}
			else{
				random = 1;
			}
		}
		else{
			if (previousFlag == 3){
				random = 1;
			}
			else{
				random = 2;
			}
		}
	}
	//If the car is in the center of the city
	else if ((i>0 && i < m) && (j>0 && j < n)){
		if (previousFlag == 0){
			int arr[3] = { 0, 2, 3 };
			random = arr[rand() % 3];
		}
		else if (previousFlag == 1){
			int arr[3] = { 1, 2, 3 };
			random = arr[rand() % 3];
		}
		else if (previousFlag == 2){
			int arr[3] = { 0, 1, 2 };
			random = arr[rand() % 3];
		}
		else{
			int arr[3] = { 0, 1, 3 };
			random = arr[rand() % 3];
		}
	}
	//If the car is in one of the four edges
	else{
		if (i == 0){
			if (previousFlag == 0){
				int arr[2] = { 0, 3 };
				random = arr[rand() % 2];
			}
			else if (previousFlag == 1){
				int arr[2] = { 1, 3 };
				random = arr[rand() % 2];
			}
			else{
				int arr[2] = { 0, 1 };
				random = arr[rand() % 2];
			}
		}
		else if (j == 0){
			if (previousFlag == 1){
				int arr[2] = { 2, 3 };
				random = arr[rand() % 2];
			}
			else if (previousFlag == 2){
				int arr[2] = { 0, 2 };
				random = arr[rand() % 2];
			}
			else{
				int arr[2] = { 0, 3 };
				random = arr[rand() % 2];
			}
		}
		else if (i == m){
			if (previousFlag == 0){
				int arr[2] = { 0, 2 };
				random = arr[rand() % 2];
			}
			else if (previousFlag == 1){
				int arr[2] = { 1, 2 };
				random = arr[rand() % 2];
			}
			else{
				int arr[2] = { 0, 1 };
				random = arr[rand() % 2];
			}
		}
		else{
			if (previousFlag == 0){
				int arr[2] = { 2, 3 };
				random = arr[rand() % 2];
			}
			else if (previousFlag == 2){
				int arr[2] = { 1, 2 };
				random = arr[rand() % 2];
			}
			else{
				int arr[2] = { 1, 3 };
				random = arr[rand() % 2];
			}
		}
	}

	//Update de value of the angle
	if (random == 0){
		angle = 0;
	}
	else if (random == 1){
		angle = 180;
	}
	else if (random == 2){
		angle = 90;
	}
	else if (random == 3){
		angle = 270;
	}
}

void displayCar(){
	glTranslatef(movementX, 0., movementZ);
	glRotatef(angle, 0., 1., 0.);
	glutWireTeapot(0.3);

	//Right direction
	if (flag == 0){
		movementX += velocity;
	}
	//Left direction
	else if (flag == 1){
		movementX -= velocity;
	}
	//Up direction
	else if (flag == 2){
		movementZ -= velocity;
	}
	//Down direction
	else{
		movementZ += velocity;
	}

	for (int i = 0; i < m + 1; i++){
		for (int j = 0; j < n + 1; j++){
			if ((movementX >= intersectionsX[i][j] - 0.004 && movementX <= intersectionsX[i][j] + 0.004) && (movementZ >= intersectionsZ[i][j] - 0.004 && movementZ <= intersectionsZ[i][j] + 0.004)){
				previousFlag = flag;
				calculate(i, j);
				flag = random;
			}
		}
	}
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glutSwapBuffers();	// Swap buffers
}

void display1(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Moving camera
	glPushMatrix();
	glTranslatef(panX, panY, zoomZ);
	glRotatef(rotY, 1.0, 0.0, 0.0);
	glRotatef(rotX, 0.0, 1.0, 0.0);
	glPopMatrix();
	glPushMatrix();

	//View from above
	gluLookAt((n*n + n) / 2, m*n*5, (m*m + m) / 2, (n*n + n) / 2, 0., (m*m + m) / 2, 0., 0., -1.);

	// Scene Model
	glPushMatrix();
	displayCoordinateFrame();
	glPopMatrix();

	glPushMatrix();
	displayCity();
	glPopMatrix();

	glPushMatrix();
	displayIntersections();
	glPopMatrix();

	glPushMatrix();
	displayCar();
	glPopMatrix();
	//End Scene Model

	glPopMatrix();

	glutSwapBuffers();	// Swap buffers
}

void display2(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Moving camera
	glPushMatrix();
	glTranslatef(panX, panY, zoomZ);
	glRotatef(rotY, 1.0, 0.0, 0.0);
	glRotatef(rotX, 0.0, 1.0, 0.0);
	glPopMatrix();
	glPushMatrix();

	//View from inside car
	if (angle == 0){
		gluLookAt(movementX + 0.35, 0., movementZ, 999999, 0., 0., 0., 1., 0.);
	}
	else if (angle == 90){
		gluLookAt(movementX, 0., movementZ - 0.35, 0., 0., -999999, 0., 1., 0.);
	}
	else if (angle == 180){
		gluLookAt(movementX - 0.35, 0., movementZ, -999999, 0., 0., 0., 1., 0.);
	}
	else if (angle == 270){
		gluLookAt(movementX, 0., movementZ + 0.35, 0., 0., 999999, 0., 1., 0.);
	}

	// Scene Model
	glPushMatrix();
	displayCoordinateFrame();
	glPopMatrix();

	glPushMatrix();
	displayCity();
	glPopMatrix();

	glPushMatrix();
	displayIntersections();
	glPopMatrix();

	glPushMatrix();
	displayCar();
	glPopMatrix();
	//End Scene Model

	glPopMatrix();

	glutSwapBuffers();	// Swap buffers
}

void display3(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Moving camera
	glPushMatrix();
	glTranslatef(panX, panY, zoomZ);
	glRotatef(rotY, 1.0, 0.0, 0.0);
	glRotatef(rotX, 0.0, 1.0, 0.0);
	glPopMatrix();
	glPushMatrix();

	//View from satellite
	alpha += 0.005;
	gluLookAt(movementX + 1.5*sin(alpha), 4, movementZ + 1.5*cos(alpha), movementX, 0., movementZ, 1., 0., 0.);

	// Scene Model
	glPushMatrix();
	displayCoordinateFrame();
	glPopMatrix();

	glPushMatrix();
	displayCity();
	glPopMatrix();

	glPushMatrix();
	displayIntersections();
	glPopMatrix();

	glPushMatrix();
	displayCar();
	glPopMatrix();
	//End Scene Model

	glPopMatrix();

	glutSwapBuffers();	// Swap buffers
}

void idle(){
	glutSetWindow(subWindow1);
	glutPostRedisplay();    // call display
	glutSetWindow(subWindow2);
	glutPostRedisplay();    // call display
	glutSetWindow(subWindow3);
	glutPostRedisplay();    // call display
}

void mouse(int button, int state, int x, int y){
	mouseButton = button;
	mouseRotate = false;
	mouseZoom = false;
	mousePan = false;
	xClick = x;
	yClick = y;
	if (button == GLUT_LEFT_BUTTON){
		mouseRotate = true;
		xAux = rotX;
		yAux = rotY;
	}
	else if (button == GLUT_RIGHT_BUTTON){
		mouseZoom = true;
		xAux = zoomZ;
	}
	else if (button == GLUT_MIDDLE_BUTTON){
		mousePan = true;
		xAux = panX;
		yAux = panY;
	}
}

void mouseMotion(int x, int y){
	if (mouseRotate == true){
		if (mouseButton == GLUT_LEFT_BUTTON){
			if ((x - xClick + xAux) > 360.0){
				xAux = xAux - 360.0;
			}
			if ((x - xClick + xAux) < 0.0){
				xAux = xAux + 360.0;
			}
			if ((y - yClick + yAux) > 360.0){
				yAux = yAux - 360.0;
			}
			if ((y - yClick + yAux) < 0.0){
				yAux = yAux + 360.0;
			}
			rotX = x - xClick + xAux;
			rotY = y - yClick + yAux;
		}
	}
	else if (mouseZoom == true){
		if (mouseButton == GLUT_RIGHT_BUTTON){
			zoomZ = ((x - xClick) / 10.0) + xAux;
		}
	}
	else if (mousePan == true){
		if (mouseButton == GLUT_MIDDLE_BUTTON){
			panX = ((x - xClick) / 63.0) + xAux;
			panY = ((y - yClick) / (-63.0)) + yAux;
		}
	}
}

int main(int argc, char** argv){
	srand(time(NULL));
	for (int i = 0; i < m * m * n * n; i++){
		randoms[i] = rand() % maxFloors;
	}

	//Initializing 'x' coordinates of the intersection's matrix
	for (int i = 0; i < m + 1; ++i){
		intersectionsX[i] = new int[n + 1];
	}

	//Initializing 'z' coordinates of the intersection's matrix
	for (int i = 0; i < m + 1; ++i){
		intersectionsZ[i] = new int[n + 1];
	}

	//Filling the matrix
	int k = 0;
	int l = 0;
	for (int i = 0; i < m * m + m + 1; i += m + 1){
		for (int j = 0; j < n * n + n + 1; j += n + 1){
			intersectionsX[k][l] = j;
			intersectionsZ[k][l] = i;
			l++;
		}
		l = 0;
		k++;
	}


	// Init Window (GLUT)
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(width, height);

	mainWindow = glutCreateWindow("City X: Alberto Villagómez Vargas A01063154");
	subWindow1 = glutCreateSubWindow(mainWindow, 0, 0, width / 2, height / 2);
	subWindow2 = glutCreateSubWindow(mainWindow, width / 2, 0, width / 2, height / 2);
	subWindow3 = glutCreateSubWindow(mainWindow, 0, height / 2, width, height / 2);

	// Callback functions registration (GLUT)
	glutSetWindow(mainWindow);
	glutDisplayFunc(display);		// display scene function
	glutReshapeFunc(reshape);		// reshape window function
	glutIdleFunc(idle);				// idle function
	glutMouseFunc(mouse);			// processing mouse events function
	glutMotionFunc(mouseMotion);	// processing mouse motion function

	// Callback functions registration (GLUT)
	glutSetWindow(subWindow1);
	glutDisplayFunc(display1);		// display scene function
	glutReshapeFunc(reshape1);		// reshape window function
	glutIdleFunc(idle);				// idle function
	glutMouseFunc(mouse);			// processing mouse events function
	glutMotionFunc(mouseMotion);	// processing mouse motion function

	// Callback functions registration (GLUT)
	glutSetWindow(subWindow2);
	glutDisplayFunc(display2);		// display scene function
	glutReshapeFunc(reshape2);		// reshape window function
	glutIdleFunc(idle);				// idle function
	glutMouseFunc(mouse);			// processing mouse events function
	glutMotionFunc(mouseMotion);	// processing mouse motion function

	// Callback functions registration (GLUT)
	glutSetWindow(subWindow3);
	glutDisplayFunc(display3);		// display scene function
	glutReshapeFunc(reshape3);		// reshape window function
	glutIdleFunc(idle);				// idle function
	glutMouseFunc(mouse);			// processing mouse events function
	glutMotionFunc(mouseMotion);	// processing mouse motion function

	GLenum err = glewInit();
	if (GLEW_OK != err){
		fprintf(stderr, "GLEW error");
		return 1;
	}

	init();							// init OpenGL context
	glutMainLoop();					// Main loop waiting for events!
	return 0;
}