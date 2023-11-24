#include <GL/glut.h>
#include <iostream>
#include <math.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
#define MAX 1000
float x_rotate = 0.0;
float y_rotate = 0.0;
float scale = 0.8;

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
    ifstream ifs("./bunny.obj");
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

void Initscene() {
    ReadPic();
    glClearColor(0.000f, 0.000f, 0.000f, 1.0f);

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
    glMaterialf(GL_FRONT, GL_SHININESS, fShininess);

}

void GLCube(){
    for (int i = 0; i < m_pic.F.size(); i++){
        glBegin(GL_TRIANGLES);
        for(int j=0; j <3; j++){
            if (m_pic.VT.size() != 0){
                glTexCoord2f(m_pic.VT[m_pic.F[i].T[j]].TU, m_pic.VT[m_pic.F[i].T[j]].TV);
            }
            glNormal3f(m_pic.VN[m_pic.F[i].N[j]].NX, m_pic.VN[m_pic.F[i].N[j]].NY, m_pic.VN[m_pic.F[i].N[j]].NZ);
            glVertex3f(m_pic.V[m_pic.F[i].V[j]].X / MAX, m_pic.V[m_pic.F[i].V[j]].Y / MAX, m_pic.V[m_pic.F[i].V[j]].Z / MAX);
        }
        // drawing end
        glEnd();
    }
}

void DrawScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Add a light source
    GLfloat glfLight[] = { -4.0f, 4.0f, -4.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, glfLight);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

    // 上下旋转
    glRotatef(x_rotate, 1.0f, 0.0f, 0.0f);
    // 左右旋转
    glRotatef(y_rotate, 0.0f, 1.0f, 0.0f);
    // 放大缩小
    glScalef(scale, scale, scale);


    GLCube();

    glFlush();
    glutSwapBuffers();
}

// 键盘上下左右键调整视角
void SpecialKeys(int key, int x, int y) {
    if (key == GLUT_KEY_UP)
        x_rotate -= ROTATE_GAP;
    else if (key == GLUT_KEY_DOWN)
        x_rotate += ROTATE_GAP;
    else if (key == GLUT_KEY_LEFT)
        y_rotate -= ROTATE_GAP;
    else if (key == GLUT_KEY_RIGHT)
        y_rotate += ROTATE_GAP;
    DrawScene();
}

// 鼠标左击缩小，右击放大，每次操作步幅为SCALE_GAP
void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        if(scale>=UPPER_BOUND)
            printf("Reach the scale upper bound!\n");
        else
            scale += SCALE_GAP;
    }

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
        if(scale<=LOWER_BOUND)    // 避免scale减小为非正值
            printf("Reach the scale lower bound!\n");
        else
            scale -= SCALE_GAP;
    }
    DrawScene();
}


int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("bunny");

    Initscene();
    glutDisplayFunc(DrawScene);
    glutSpecialFunc(SpecialKeys);
    glutMouseFunc(mouseClick);

    glutMainLoop();
    return 0;
}



/*
顶点数据(Vertex data)：
    v 几何体顶点(Geometric vertices)
    vt 贴图坐标点(Texture vertices)
    vn 顶点法线(Vertex normals)
    vp 参数空格顶点 (Parameter space vertices)

自由形态曲线(Free-form curve)/表面属性(surface attributes):
    deg 度(Degree)
    bmat 基础矩阵(Basis matrix)
    step 步尺寸(Step size)
    cstype 曲线或表面类型 (Curve or surface type)

元素(Elements):
    p 点(Point)
    l 线(Line)
    f 面(Face)
    curv 曲线(Curve)
    curv2 2D曲线(2D curve)
    surf 表面(Surface)
 */