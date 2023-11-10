#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>

#define PI 3.1415926
#define lineWidth 2

int border[3]={0,0,0};
int grey[3]={210,210,210};
int yellow[3]={255,243,0};
int red[3]={237,28,36};
int blue[3]={153,204,255};
int darkGrey[3]={126,126,126};
int white[3]={255,255,255};

void glRect(int leftX,int leftY,int rightX,int rightY,int MODE){
	
    glBegin(MODE);
	glVertex2d(leftX,leftY);
	glVertex2d(rightX,leftY);
	glVertex2d(rightX,rightY);
	glVertex2d(leftX,rightY);

	glEnd();
}

void glRoundRec(int centerX,int centerY,int width,int height,float cirR,int MODE){
	float PI_HALF = PI/2;
	float divide=20.0;
    float tx,ty; 
	
    glBegin(MODE);

    int opX[4]={1,-1,-1,1};
    int opY[4]={1,1,-1,-1};

    float x=0;

	float part=1/divide;

	int w=width/2-cirR;
	int h=height/2-cirR;

    for(x=0;x<4;x+=part){
	
		float rad = PI_HALF*x;
		
	    tx=cirR*cos(rad)+opX[(int)x]*w+centerX;
	    ty=cirR*sin(rad)+opY[(int)x]*h+centerY;
	    glVertex2f(tx,ty);
	}
   glEnd();
}

void glArc(double x,double y,double start_angle,double end_angle,double radius,int MODE)
{   
	glBegin(MODE);
	double delta_angle=PI/180;
	for (double i=start_angle;i<=end_angle;i+=delta_angle)
	{
		double vx=x+radius * cos(i);
		double vy=y+radius*sin(i);
		glVertex2d(vx,vy);
	}
	glEnd();
}

void glCircle(double x, double y, double radius,int MODE)  
{  
    glArc(x,y,0,2*PI,radius,MODE);  
}  


void glTri(int x1,int y1,int x2,int y2,int x3,int y3,int MODE){
    glBegin(MODE);
	glVertex2d(x1,y1);
	glVertex2d(x2,y2);
	glVertex2d(x3,y3);
	glEnd();
}

// (leftX, leftY) is the left top point,
// (rightX, rightY) is the right bottom point
void glFillRect(int leftX,int leftY,int rightX,int rightY,int color[3]){
	glColor3ub(border[0],border[1],border[2]);
	glRect(leftX,leftY,rightX,rightY,GL_LINE_LOOP);
	glColor3ub(color[0],color[1],color[2]);
	glRect(leftX+lineWidth/2,leftY-lineWidth/2,rightX-lineWidth/2,rightY+lineWidth/2,GL_POLYGON);
}

void glFillTri(int x1,int y1,int x2,int y2,int x3,int y3,int color[3]){
	glColor3ub(border[0],border[1],border[2]);
	glTri(x1,y1,x2,y2,x3,y3,GL_LINE_LOOP);
	glColor3ub(color[0],color[1],color[2]);
	glTri(x1+lineWidth/2,y1+lineWidth/2,x2-lineWidth/2,y2+lineWidth/2,x3,y3-lineWidth/2,GL_POLYGON);
}

void glFillRoundRec(int centerX,int centerY,int width,int height,float cirR,int color[3]){
	glColor3ub(border[0],border[1],border[2]);
	glRoundRec(centerX,centerY,width,height,cirR,GL_LINE_LOOP);
	glColor3ub(color[0],color[1],color[2]);
	glRoundRec(centerX,centerY,width-lineWidth/2,height-lineWidth/2,cirR,GL_POLYGON);
}

void glFillCircle(double x, double y, double radius,int color[3]){
	glColor3ub(border[0],border[1],border[2]);
	glCircle(x,y,radius,GL_LINE_LOOP);
	glColor3ub(color[0],color[1],color[2]);
	glCircle(x,y,radius-lineWidth/2,GL_POLYGON);
}


void display(void)
{
	
	glClear(GL_COLOR_BUFFER_BIT);
	glLineWidth(lineWidth);

    glColor3ub(0,0,0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // screen
    glFillRect(-74,64,74,-64,grey);
	glFillRoundRec(0,0,140,120,15,blue);
	// triangle on screen
	glFillTri(-30,-15,30,-15,0,28,yellow);
	// circle on triangle
	glFillCircle(0,0,10,red);
	// base
	glFillRoundRec(0,-80,80,16,4,darkGrey);
    // rod
	glFillRect(-20,-64,20,-80,darkGrey);
	glFlush();
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(400, 400);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("polygon_fill");
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glutDisplayFunc(display);
    gluOrtho2D(-150, 150, -150, 150);
    glutMainLoop();
    return 0;
}