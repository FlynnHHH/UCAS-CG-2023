// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// // Include GLEW
// #include <GL/glew.h>

// // Include GLFW
// #include <GLFW/glfw3.h>
// GLFWwindow* window;

// // Include GLM
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// using namespace glm;

// #include <common/shader.hpp>
// #include <common/texture.hpp>
// #include <common/controls.hpp>

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

class objModel
{
public:
	objModel(const char * objFileName);
	void setTextureFromBmp(const char *texFileName);  //从obj文件创建纹理
	void objDraw();
private:
	vector<Float3> mLocation;   //位置信息
	vector<Float3> mNormal;     //法线信息
	vector<Float2> mTexcoord;   //纹理坐标信息
	vector<Face> mFace;         //面信息
	GLuint mTexture;            //模型纹理	
};

objModel::objModel(const char * objFileName)
{
	int nFileSize = 0;
	unsigned char*fileContent = LoadFileContent(objFileName, nFileSize);    //读取文件内容
	if (fileContent == nullptr)     //文件为空
	{
		return;
	}
	stringstream ssFileContent((char*)fileContent);   //流读取文件内容
	string temp;       //接受无关信息
	char szoneLine[256];        //读取一行的数据
	while (!ssFileContent.eof())
	{
		memset(szoneLine, 0, 256);        //  每次循环初始化数组szoneLine
		ssFileContent.getline(szoneLine, 256);      //流读取一行
		if (strlen(szoneLine) > 0)       //该行不为空
		{
			if (szoneLine[0] == 'v')     //v开头的数据
			{
				stringstream ssOneLine(szoneLine);        //数据存储到流中 方便赋值
				if (szoneLine[1] == 't')       //纹理信息
				{
					ssOneLine >> temp;     //接受标识符 vt
					Float2 tempTexcoord;
					ssOneLine >> tempTexcoord.Data[0] >> tempTexcoord.Data[1];   //数据存入临时变量中
					mTexcoord.push_back(tempTexcoord);         //存入容器
 
				}
				else if (szoneLine[1] == 'n')            //法线信息
				{
					ssOneLine >> temp;      //接收标识符vn
					Float3 tempNormal;
					ssOneLine >> tempNormal.Data[0] >> tempNormal.Data[1] >> tempNormal.Data[2];
					mNormal.push_back(tempNormal);
				}
				else                          //点的位置信息
				{
					ssOneLine >> temp;
					Float3 tempLocation;
					ssOneLine >> tempLocation.Data[0] >> tempLocation.Data[1] >> tempLocation.Data[2];
					mLocation.push_back(tempLocation);
				}
			}
			else if (szoneLine[0] == 'f')          //面信息
			{
				stringstream ssOneLine(szoneLine);     //流读取一行数据
				ssOneLine >> temp; //接收标识符f
				//    f信息    exp： f 1/1/1 2/2/2 3/3/3      位置索引/纹理索引/法线索引   三角面片 三个点构成一个面
				string vertexStr;   //接收流的内容
				Face tempFace;
				for (int i = 0; i < 3; ++i)         //每个面三个点
				{
					ssOneLine >> vertexStr;           //从流中读取点的索引信息
					size_t pos = vertexStr.find_first_of('/');       //找到第一个/的位置      //即找到点的位置信息
					string locIndexStr = vertexStr.substr(0, pos);       //赋值点的位置信息
					size_t pos2 = vertexStr.find_first_of('/', pos + 1);   //找到第二个/   即找到点的纹理坐标信息
					string texIndexSrt = vertexStr.substr(pos + 1, pos2 - 1 - pos);       //赋值点的纹理坐标信息
					string norIndexSrt = vertexStr.substr(pos2 + 1, vertexStr.length() - 1 - pos2);   //赋值点的法线信息
					tempFace.vertex[i][0] = atoi(locIndexStr.c_str());        //将索引信息从 srting转换为 int     //位置索引信息赋值
					tempFace.vertex[i][1] = atoi(texIndexSrt.c_str());         //纹理坐标索引信息赋值
					tempFace.vertex[i][2] = atoi(norIndexSrt.c_str());         //法线信息赋值
				}
				mFace.push_back(tempFace);
			}
		}   //end 非0行
	}  //end while
	delete fileContent;
}

void objModel::objDraw()
{
	glBindTexture(GL_TEXTURE_2D, mTexture);
	glBegin(GL_TRIANGLES);
	for (auto faceIndex = mFace.begin(); faceIndex != mFace.end(); ++faceIndex)         //循环遍历face信息
	{
		//第一个点的法线，纹理，位置信息
		glNormal3fv(mNormal[faceIndex->vertex[0][2]-1].Data);
		glTexCoord2fv(mTexcoord[faceIndex->vertex[0][1]-1].Data);
		glVertex3fv(mLocation[faceIndex->vertex[0][0]-1].Data);
		//第二个点的法线，纹理，位置信息
		glNormal3fv(mNormal[faceIndex->vertex[1][2]-1].Data);
		glTexCoord2fv(mTexcoord[faceIndex->vertex[1][1]-1].Data);
		glVertex3fv(mLocation[faceIndex->vertex[1][0]-1].Data);
		//第三个点的法线，纹理，位置信息
		glNormal3fv(mNormal[faceIndex->vertex[2][2]-1].Data);
		glTexCoord2fv(mTexcoord[faceIndex->vertex[2][1]-1].Data);
		glVertex3fv(mLocation[faceIndex->vertex[2][0]-1].Data);
	}
	glEnd();
}

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Tutorial 07 - Model Loading", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Load the texture
	GLuint Texture = loadDDS("uvmap.DDS");
	
	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals; // Won't be used at the moment.
	bool res = loadOBJ("cube.obj", vertices, uvs, normals);

	// Load it into a VBO

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	do{

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(TextureID, 0);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() );

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
