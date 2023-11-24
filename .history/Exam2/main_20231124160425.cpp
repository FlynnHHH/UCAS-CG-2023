#include <GL/glut.h>
#include <iostream>
#include <stdio.h>

struct POINT3{
    double X;
    double Y;
    double Z;
};
struct WenLi{
    double TU;
    double TV;
};
struct FaXiangLiang{
    double NX;
    double NY;
    double NZ;
};
struct Mian{
    int V[3];
    int T[3];
    int N[3];
};
class PIC
{
public:
    vector<POINT3> V;  //V：代表顶点。格式为V X Y Z，V后面的X Y Z表示三个顶点坐标。浮点型
    vector<WenLi>  VT;  //表示纹理坐标。格式为VT TU TV。浮点型
    vector<FaXiangLiang> VN;  //VN：法向量。每个三角形的三个顶点都要指定一个法向量。格式为VN NX NY NZ。浮点型
    vector<Mian> F;  //F：面。面后面跟着的整型值分别是属于这个面的顶点、纹理坐标、法向量的索引。
                     //面的格式为：f Vertex1/Texture1/Normal1 Vertex2/Texture2/Normal2 Vertex3/Texture3/Normal3
};

void CMainWnd::ReadPIC()
{
    ifstream ifs(name);//cube bunny Eight
    string s;
    Mian *f;
    POINT3 *v;
    FaXiangLiang *vn;
    WenLi    *vt;
    while(getline(ifs,s))
    {
        if(s.length()<2)continue;
        if(s[0]=='v'){
            if(s[1]=='t'){//vt 0.581151 0.979929 纹理
                istringstream in(s);
                vt=new WenLi();
                string head;
                in>>head>>vt->TU>>vt->TV;
                m_pic.VT.push_back(*vt);
            }else if(s[1]=='n'){//vn 0.637005 -0.0421857 0.769705 法向量
                istringstream in(s);
                vn=new FaXiangLiang();
                string head;
                in>>head>>vn->NX>>vn->NY>>vn->NZ;
                m_pic.VN.push_back(*vn);
            }else{//v -53.0413 158.84 -135.806 点
                istringstream in(s);
                v=new POINT3();
                string head;
                in>>head>>v->X>>v->Y>>v->Z;
                m_pic.V.push_back(*v);
            }
        }
        else if(s[0]=='f'){ //f 2443//2656 2442//2656 2444//2656 面
            for(int k=s.size()-1;k>=0;k--){
                if(s[k]=='/')s[k]=' ';
            }
            istringstream in(s);
            f=new Mian();
            string head;
            in>>head;
            int i=0;
            while(i<3)
            {
                if(m_pic.V.size()!=0)
                {
                    in>>f->V[i];
                    f->V[i]-=1;
                }
                if(m_pic.VT.size()!=0)
                {
                    in>>f->T[i];
                    f->T[i]-=1;
                }
                if(m_pic.VN.size()!=0)
                {
                    in>>f->N[i];
                    f->N[i]-=1;
                }
                i++;
            }
            m_pic.F.push_back(*f);
        }
    }
}