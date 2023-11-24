#include <GL/glut.h>

struct Float3       //点的位置信息和法线信息的数据类型
{
	float Data[3];     //x,y,z
};
 
struct Float2      //点的纹理坐标数据类型
{
	float Data[2];   //u,v
};
 
struct Face          //面信息
{
	int vertex[3][3];       //三个点构成一个面  每个点有三个索引信息
};
 
unsigned char*LoadFileContent(const char*path, int &filesize)   //该函数来自于萌谷教程
//读取文件，返回文件内容，把文件大小赋值给filesize       
{
	unsigned char*fileContent = nullptr;
	filesize = 0;
	FILE*pFile = fopen(path, "rb");   //二进制方式读取
	if (pFile)
	{
		fseek(pFile, 0, SEEK_END);      //将文件指针移动到文件末尾
		int nLen = ftell(pFile);        //距离文件头部的距离   //这里指文件大小
		if (nLen > 0)
		{
			rewind(pFile);          //移动到文件头部
			fileContent = new unsigned char[nLen + 1];    
  //为文件指针开辟空间
			fread(fileContent, sizeof(unsigned char), nLen, pFile); 
  //将pFile的内容读入fileContent
			fileContent[nLen] = '\0';         //文件末尾加上\0
			filesize = nLen;                  //为文件大小赋值
		}
		fclose(pFile);
	}
	return fileContent;
}