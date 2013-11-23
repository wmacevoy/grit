#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>

#include <math.h>

GLint id;
GLint circle_points = 1081;
double r = 5;

// This is the draw function.
void draw() {
	glClear(GL_COLOR_BUFFER_BIT);
	double angle = (3 * M_PI) / (2 * circle_points) ;
	glColor3f(0.2, 0.5, 0.5 );
	glBegin(GL_POINTS);
	double angle1 = 7 * M_PI / 4;
	glVertex2d(r * cos(0.0), r * sin(0.0));
	int i;
	for (i = 0; i < circle_points; ++i) {
		printf( "angle = %f \n" , angle1);
		glVertex2d(r * cos(angle1), r * sin(angle1));
		angle1 += angle ;
	}
	glEnd();		
	glFlush();
}

void init() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-15.0, 15.0, -15.0, 15.0, -1.0, 1.0);
}

void keyboard (unsigned char key , int x, int y) {
	if(key == 27) {
		glutDestroyWindow(id);
		exit(0);
	}
}

int main( int argc,char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE |GLUT_RGB);
	glutInitWindowSize(260, 260);
	glutInitWindowPosition(100, 100);
	id = glutCreateWindow("circleGL");
	init();
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(draw);
	glutMainLoop();
	return 0; 
}
