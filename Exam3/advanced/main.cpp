#include "main.h"
#include "display.h"

extern std::vector<Face> faces;
extern std::vector<Vertex> vertices;

void OutputUsage()
{
    cout<<"Usage:./main input_name output_name ratio"<<endl;
    cout<<"input_name: the name of input model"<<endl;
    cout<<"output_name: the name of output model"<<endl;
    cout<<"ratio: the ratio of the number of output model to the number of input model"<<endl;
    cout<<"Example: ./main dragon dragon_simplified 0.5"<<endl;
}

int main(int argc, char *argv[]) {
    char input_name[256];
    char output_name[256];
    float c;
    if (argc == 4) {
        strcpy(input_name, argv[1]);
        strcpy(output_name, argv[2]);
        sscanf(argv[3], "%f", &c);
        
        mesh model(input_name);
        model.Simplify(c);
        model.Savemodel(output_name);
        mesh simlified_model(output_name);
        faces = simlified_model.faces;
        vertices = simlified_model.vertices;
    }
    else {
        OutputUsage();
    }
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Exam3-advanced");

    Initscene();
    glutDisplayFunc(display);

    glutMouseFunc(trim);
    glutSpecialFunc(rotate);

    glutMainLoop();
    return 0;
}

