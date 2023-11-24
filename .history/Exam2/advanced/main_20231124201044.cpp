#include <GL/glut.h>
#include <iostream>
#include <math.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
#define MAX 1000
#define ROTATE_DIFF 5
#define SCALE_DIFF 0.1

int x_rotate = 0;
int y_rotate = 0;
double scale = 1.0;

struct VECTOR3{
    double X;
    double Y;
    double Z;
};
struct POINT3 {
    double X;
    double Y;
    double Z;
};
struct Texture {
    double TU;
    double TV;
};
struct Normal {
    double NX;
    double NY;
    double NZ;
};
struct Face {
    int V[3];
    int T[3];
    int N[3];
};

class PIC{
    public:
        vector<POINT3> V;
        vector<Texture> VT;
        vector<Normal> VN;
        vector<Face> F;
};

PIC m_pic;

void ReadPic() {
    ifstream ifs("../bunny.obj");
    string s;
    Face* f;
    POINT3* v;
    Normal* vn;
    Texture* vt;
    while (getline(ifs, s)) {
        if (s.length() < 2) continue;
        if (s[0] == 'v') {
            if (s[1] == 't') {
                istringstream in(s);
                vt = new Texture();
                string head;
                in >> head >> vt->TU >> vt->TV;
                m_pic.VT.push_back(*vt);
            }
            else if (s[1] == 'n') {
                istringstream in(s);
                vn = new Normal();
                string head;
                in >> head >> vn->NX >> vn->NY >> vn->NZ;
                m_pic.VN.push_back(*vn);
            }
            else {
                istringstream in(s);
                v = new POINT3();
                string head;
                in >> head >> v->X >> v->Y >> v->Z;
                m_pic.V.push_back(*v);
            }
        }
        else if (s[0] == 'f') {
            for (int k = s.size() - 1; k >= 0; k--) {
                if (s[k] == '/') s[k] = ' ';
            }
            istringstream in(s);
            f = new Face();
            string head;
            in >> head;
            int i = 0;
            while (i < 3) {
                if (m_pic.V.size() != 0) {
                    in >> f->V[i];
                    f->V[i] -= 1;
                }
                if (m_pic.VT.size() != 0) {
                    in >> f->T[i];
                    f->T[i] -= 1;
                }
                if (m_pic.VN.size() != 0) {
                    in >> f->N[i];
                    f->N[i] -= 1;
                }
                i++;
            }
            m_pic.F.push_back(*f);
        }
    }
}

void getnorm(){
    Face *f;
    POINT3 v0, v1, v2;
    Normal *vn;
    double norm;

    for(uint32_t i=0; i<m_pic.F.size(); i++){
        f= &m_pic.F[i];
        // step1 根据三个顶点计算平面法向量
        v0 = m_pic.V[f->V[0]];
        v1 = m_pic.V[f->V[1]];
        v2 = m_pic.V[f->V[2]];
        VECTOR3 v0_v1 = {v1.X - v0.X, v1.Y - v0.Y, v1.Z - v0.Z};
        VECTOR3 v0_v2 = {v2.X - v0.X, v2.Y - v0.Y, v2.Z - v0.Z};
        VECTOR3 fv =   {v0_v1.Y*v0_v2.Z - v0_v1.Z*v0_v2.Y,
                       -v0_v1.X*v0_v2.Z + v0_v1.Z*v0_v2.X,
                        v0_v1.X*v0_v2.Y - v0_v1.Y*v0_v2.X};

        m_pic.VN[f->N[0]].NX += fv.X;
        m_pic.VN[f->N[1]].NX += fv.X;
        m_pic.VN[f->N[2]].NX += fv.X;

        m_pic.VN[f->N[0]].NY += fv.Y;
        m_pic.VN[f->N[1]].NY += fv.Y;
        m_pic.VN[f->N[2]].NY += fv.Y;

        m_pic.VN[f->N[0]].NZ += fv.Z;
        m_pic.VN[f->N[1]].NZ += fv.Z;
        m_pic.VN[f->N[2]].NZ += fv.Z;    
    }
    for(uint32_t i=0; i<m_pic.VN.size(); i++){
        vn = &m_pic.VN[i];
        norm = sqrt(vn->NX*vn->NX + vn->NY*vn->NY + vn->NZ*vn->NZ);
        vn->NX = vn->NX/norm;
        vn->NY = vn->NY/norm;
        vn->NZ = vn->NZ/norm;
    }
}

void Initscene() {
    ReadPic();
    getnorm();
    glClearColor(0, 0, 0, 1);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    static GLfloat glfMatAmbient[]  = {0.329412, 0.223529, 0.027451, 1.000000};
    static GLfloat glfMatDiffuse[]  = {0.780392, 0.568627, 0.113725, 1.000000};
    static GLfloat glfMatspecular[] = {0.992157, 0.941176, 0.807843, 1.000000};
    static GLfloat glfMatEmission[] = {0, 0, 0, 1};
    static GLfloat fShininess = 27.897400;

    glMaterialfv(GL_FRONT, GL_AMBIENT, glfMatAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, glfMatDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, glfMatspecular);
    glMaterialfv(GL_FRONT, GL_EMISSION, glfMatEmission);
    glMaterialf (GL_FRONT, GL_SHININESS, fShininess);

}

void GLCube(){
    for (int i = 0; i < m_pic.F.size(); i++){
        glBegin(GL_TRIANGLES);
        for(int j = 0; j < 3; j++){
            if (m_pic.VT.size() != 0){
                glTexCoord2f(m_pic.VT[m_pic.F[i].T[j]].TU, m_pic.VT[m_pic.F[i].T[j]].TV);
            }
            glNormal3f(m_pic.VN[m_pic.F[i].N[j]].NX, m_pic.VN[m_pic.F[i].N[j]].NY, m_pic.VN[m_pic.F[i].N[j]].NZ);
            glVertex3f(m_pic.V[m_pic.F[i].V[j]].X / MAX, m_pic.V[m_pic.F[i].V[j]].Y / MAX, m_pic.V[m_pic.F[i].V[j]].Z / MAX);
        }
        glEnd();
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    GLfloat glfLight[] = { -4.0f, 4.0f, -4.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, glfLight);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

    glRotatef(x_rotate, 1.0f, 0.0f, 0.0f);
    glRotatef(y_rotate, 0.0f, 1.0f, 0.0f);
    glScalef(scale, scale, scale);

    GLCube();

    glFlush();
    glutSwapBuffers();
}

void trim(int button, int state, int x, int y) {
    if (button == 3 && state == GLUT_DOWN) {
        scale += SCALE_DIFF;
    }
    if (button == 4 && state == GLUT_DOWN){
        scale -= SCALE_DIFF;
    }
    display();
}

void rotate(int key, int x, int y) {
    if (key == GLUT_KEY_UP) 
        x_rotate -= ROTATE_DIFF;
    else if (key == GLUT_KEY_DOWN) 
        x_rotate += ROTATE_DIFF;
    else if (key == GLUT_KEY_LEFT) 
        y_rotate -= ROTATE_DIFF;
    else if (key == GLUT_KEY_RIGHT) 
        y_rotate += ROTATE_DIFF;
    display();
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Exam2-advanced");

    Initscene();
    glutDisplayFunc(display);

    glutMouseFunc(trim);
    glutSpecialFunc(rotate);

    glutMainLoop();
    return 0;
}