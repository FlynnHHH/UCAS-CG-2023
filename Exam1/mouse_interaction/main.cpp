#include <math.h>
#include <GL/glut.h>
#include <GL/gl.h>

#define PI 3.1415926
#define N 1000
#define LINE 0
#define REC  1
#define TRI  2

int Line[N][4], xy_Line;
int Rec[N][4], xy_Rec;
int Tri[N][4], xy_Tri;

int ww, hh;
int paint = 0;

void myMouse(int button, int state, int x, int y)
{

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
        // LINE
		if (paint == LINE) {
			Line[xy_Line][0] = x;
			Line[xy_Line][1] = hh - y;
		}
        // REC
		if (paint == REC) {
			Rec[xy_Rec][0] = x;
			Rec[xy_Rec][1] = hh - y;
		}
        // TRI
		if (paint == TRI) {
			Tri[xy_Tri][0] = x;
			Tri[xy_Tri][1] = hh - y;
		}
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{	
		// LINE
		if (paint == LINE) {
			Line[xy_Line][2] = x;
			Line[xy_Line][3] = hh - y;
			xy_Line++;
		}
		// REC
		if (paint == REC) {
			Rec[xy_Rec][2] = x;
			Rec[xy_Rec][3] = hh - y;
			xy_Rec++;
		}
		// TRI
		if (paint == TRI) {
			Tri[xy_Tri][2] = x;
			Tri[xy_Tri][3] = hh - y;
			xy_Tri++;
		}
		glutPostRedisplay();
	}
}

void myMotion(int x, int y)
{
	// LINE
	if (paint == LINE) {
		Line[xy_Line][2] = x;
		Line[xy_Line][3] = hh - y;
	}
	// REC
	if (paint == REC) {
		Rec[xy_Rec][2] = x;
		Rec[xy_Rec][3] = hh - y;
	}
	// TRI
	if (paint == TRI) {
		Tri[xy_Tri][2] = x;
		Tri[xy_Tri][3] = hh - y;
	}
	glutPostRedisplay();
}

void Reshape(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluOrtho2D(0, w, 0, h);
	ww = w;
	hh = h;
}

void drawLine() 
{
	for (int i = 0; i <= xy_Line; i++) {
		glColor3f(1, 1, 1);
		glBegin(GL_LINE_STRIP);
		glVertex2i(Line[i][0], Line[i][1]);
		glVertex2i(Line[i][2], Line[i][3]);
		glEnd();
	}
}

void drawRec() 
{
	for (int i = 0; i <= xy_Rec; i++) {
		glColor3f(1, 0, 0);
		glRectf(Rec[i][0], Rec[i][1], Rec[i][2], Rec[i][3]);
	}
}

void drawTri() {
	glColor3f(0, 0, 1);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i <= xy_Tri; i++) {
		int x = Tri[i][0] - Tri[i][2];
		int y = Tri[i][1] - Tri[i][3];

		glVertex2f(Tri[i][0], Tri[i][1]);
		glVertex2f(Tri[i][2], Tri[i][3]);
		glVertex2f(Tri[i][2] + 50, Tri[i][3] + 60);
	}
	glEnd();
}

void processMenuEvents(int option) {
	switch (option)
	{
	case LINE:
		paint = LINE; break;
	case REC:
		paint = REC; break;
	case TRI:
		paint = TRI; break;
	default:
		break;
	}
}

void GLUTMenus() {
	int menu;
	menu = glutCreateMenu(processMenuEvents);

	glutAddMenuEntry("Line", LINE);
	glutAddMenuEntry("Rectangle", REC);
	glutAddMenuEntry("Triangle", TRI);

	glutAttachMenu(GLUT_MIDDLE_BUTTON);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	drawLine();
	drawRec();
	drawTri();

	glFlush();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(400, 400);
	glutInitWindowPosition(100,100); 
	glutCreateWindow("mouse_interaction");
    glClearColor(0.0, 0.0, 0.0, 0.0);
    GLUTMenus();
	glutDisplayFunc(display);

	glutMouseFunc(myMouse);
	glutMotionFunc(myMotion);
	glutReshapeFunc(Reshape);

	glutMainLoop();
	return 0;
}
