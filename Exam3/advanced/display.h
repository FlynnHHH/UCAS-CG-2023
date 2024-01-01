#ifndef DISPLAY_H
#define DISPLAY_H

#include <GL/glut.h>
#include <iostream>
#include <math.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "main.h"

#define MAX 1000
#define ROTATE_DIFF 5
#define SCALE_DIFF 0.1

// extern double x_rotate;
// extern double y_rotate;
// extern double scale;

// extern std::vector<Face> faces;
// extern std::vector<Vertex> vertices;
// extern std::vector<Normal> vn;

struct VECTOR3{
    double x;
    double y;
    double z;
};
struct POINT3 {
    double X;
    double Y;
    double Z;
};

class PIC{
    public:
        vector<POINT3> vertex;
        vector<Texture> VT;
        vector<Normal> VN;
        vector<Face> F;
};

void getnorm();
void Initscene();
void GLCube();
void display();
void trim(int button, int state, int x, int y);
void rotate(int key, int x, int y);
#endif