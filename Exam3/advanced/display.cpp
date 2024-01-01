#include "display.h"

using namespace std;

int x_rotate = 0;
int y_rotate = 0;
double scale = 1.0;

PIC m_pic;
std::vector<Face> faces;
std::vector<Vertex> vertices;

void getnorm(){
    Face *f;
    Vertex v0, v1, v2;
    Normal *vn;
    double norm;

    for(Face& f : faces){
        v0 = vertices[f.vertex[0]];
        v1 = vertices[f.vertex[1]];
        v2 = vertices[f.vertex[2]];
        VECTOR3 v0_v1 = {v1.x - v0.x, v1.y - v0.y, v1.z - v0.z};
        VECTOR3 v0_v2 = {v2.x - v0.x, v2.y - v0.y, v2.z - v0.z};
        VECTOR3 fv =   {v0_v1.y*v0_v2.z - v0_v1.z*v0_v2.y, \
                       -v0_v1.x*v0_v2.z + v0_v1.z*v0_v2.x, \
                        v0_v1.x*v0_v2.y - v0_v1.y*v0_v2.x};

        m_pic.VN[f.normal[0]].nx += fv.x;
        m_pic.VN[f.normal[1]].nx += fv.x;
        m_pic.VN[f.normal[2]].nx += fv.x;

        m_pic.VN[f.normal[0]].ny += fv.y;
        m_pic.VN[f.normal[1]].ny += fv.y;
        m_pic.VN[f.normal[2]].ny += fv.y;

        m_pic.VN[f.normal[0]].nz += fv.z;
        m_pic.VN[f.normal[1]].nz += fv.z;
        m_pic.VN[f.normal[2]].nz += fv.z;    
    }
    for(uint32_t i=0; i<m_pic.VN.size(); i++){
        vn = &m_pic.VN[i];
        norm = sqrt(vn->nx*vn->nx + vn->ny*vn->ny + vn->nz*vn->nz);
        vn->nx = vn->nx/norm;
        vn->ny = vn->ny/norm;
        vn->nz = vn->nz/norm;
    }
}

void Initscene() {
    getnorm();
    glClearColor(0, 0, 0, 1);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    static GLfloat glfMatAmbient[] = { 0.135f,0.2225f,0.1575f,0.95f };
    static GLfloat glfMatDiffuse[] = { 0.54f,0.89f,0.63f,0.95f };
    static GLfloat glfMatSpecular[] = { 0.316228f,0.316228f,0.316228f,0.95f };
    static GLfloat glfMatEmission[] = { 0.000f, 0.000f, 0.000f, 1.0f };
    static GLfloat fShininess = 12.800f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, glfMatAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, glfMatDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, glfMatSpecular);
    glMaterialfv(GL_FRONT, GL_EMISSION, glfMatEmission);
    glMaterialf (GL_FRONT, GL_SHININESS, fShininess);

}

void GLCube(){
    for (int i = 0; i < faces.size(); i++){
        glBegin(GL_TRIANGLES);
        for(int j = 0; j < 3; j++){
            if (m_pic.VT.size() != 0){
                glTexCoord2f(m_pic.VT[faces[i].texture[j]].TU, m_pic.VT[faces[i].texture[j]].TV);
            }
            glNormal3f(m_pic.VN[faces[i].normal[j]].nx, m_pic.VN[faces[i].normal[j]].ny, m_pic.VN[faces[i].normal[j]].nz);
            glVertex3f(vertices[faces[i].vertex[j]].x / MAX, vertices[faces[i].vertex[j]].y / MAX, vertices[faces[i].vertex[j]].z / MAX);
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