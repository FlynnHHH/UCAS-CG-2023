#include "main.h"

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
    }
    else
        OutputUsage();
    return 0;
}
