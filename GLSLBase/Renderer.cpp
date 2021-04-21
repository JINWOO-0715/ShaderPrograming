#include "stdafx.h"
#include "Renderer.h"
#include "LoadPng.h"
#include <Windows.h>
#include <cstdlib>
#include <cassert>

Renderer::Renderer(int windowSizeX, int windowSizeY)
{
	//default settings
	glClearDepth(1.f);

	Initialize(windowSizeX, windowSizeY);
}


Renderer::~Renderer()
{
}

void Renderer::Initialize(int windowSizeX, int windowSizeY)
{
	//Set window size
	m_WindowSizeX = windowSizeX;
	m_WindowSizeY = windowSizeY;

	//Load shaders
	m_SolidRectShader = CompileShaders("./Shaders/SolidRect.vs", "./Shaders/SolidRect.fs");
	m_VBOFSSandboxShader = CompileShaders("./Shaders/FSSandbox.vs", "./Shaders/FSSandbox.fs");
	//Create VBOs
	CreateVertexBufferObjects();

	//Initialize camera settings
	m_v3Camera_Position = glm::vec3(0.f, 0.f, 1000.f);
	m_v3Camera_Lookat = glm::vec3(0.f, 0.f, 0.f);
	m_v3Camera_Up = glm::vec3(0.f, 1.f, 0.f);
	m_m4View = glm::lookAt(
		m_v3Camera_Position,
		m_v3Camera_Lookat,
		m_v3Camera_Up
	);

	//Initialize projection matrix
	m_m4OrthoProj = glm::ortho(
		-(float)windowSizeX / 2.f, (float)windowSizeX / 2.f,
		-(float)windowSizeY / 2.f, (float)windowSizeY / 2.f,
		0.0001f, 10000.f);
	m_m4PersProj = glm::perspectiveRH(45.f, 1.f, 1.f, 1000.f);

	//Initialize projection-view matrix
	m_m4ProjView = m_m4OrthoProj * m_m4View; //use ortho at this time
	//m_m4ProjView = m_m4PersProj * m_m4View;

	//Initialize model transform matrix :; used for rotating quad normal to parallel to camera direction
	m_m4Model = glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::vec3(1.f, 0.f, 0.f));

	//Creat test data
	float tempVertices[] = { 0.f,0.f,0.f,1.f,0.f,0.f,1.f,1.f,0.f };
	// 메모리 버퍼 할당
	glGenBuffers(1, &m_VBO);
	// 메모리 버퍼 바인드
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	// 바인드된 vbo에 할당.
	glBufferData(GL_ARRAY_BUFFER, sizeof(tempVertices), tempVertices, GL_STATIC_DRAW);
	// 여기까지가 gpu에 할당. 이제 의미를 부여해야함.

	float tempVertices1[] = { 0.f,0.f,0.f,-1.f,0.f,0.f,-1.f,1.f,0.f };
	glGenBuffers(1, &m_VBO1);
	// 메모리 버퍼 바인드
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO1);
	// 바인드된 vbo에 할당.
	glBufferData(GL_ARRAY_BUFFER, sizeof(tempVertices1), tempVertices1, GL_STATIC_DRAW);
	// 여기까지가 gpu에 할당. 이제 의미를 부여해야함.
	
	float sizeRect = 0.5f;
	float tempVertices2[] = {
		-sizeRect,-sizeRect,0.f,
		-sizeRect,sizeRect,0.f,
		 sizeRect,sizeRect,0.f,
		-sizeRect,-sizeRect,0.f,
		sizeRect,sizeRect,0.f,
		sizeRect,-sizeRect,0.f
	};
	glGenBuffers(1, &m_VBOFSSandBox);
	// 메모리 버퍼 바인드
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOFSSandBox);
	// 바인드된 vbo에 할당.
	glBufferData(GL_ARRAY_BUFFER, sizeof(tempVertices2), tempVertices2, GL_STATIC_DRAW);
	// 여기까지가 gpu에 할당. 이제 의미를 부여해야함.

	

	//CreateParicle
	CreateParticle(1000);

}

void Renderer::CreateVertexBufferObjects()
{
	float rect[]
		=
	{
		-0.5, -0.5, 0.f, -0.5, 0.5, 0.f, 0.5, 0.5, 0.f, //Triangle1
		-0.5, -0.5, 0.f,  0.5, 0.5, 0.f, 0.5, -0.5, 0.f, //Triangle2
	};

	glGenBuffers(1, &m_VBORect);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);
}

void Renderer::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	//쉐이더 오브젝트 생성
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = (GLint)strlen(pShaderText);
	//쉐이더 코드를 쉐이더 오브젝트에 할당
	glShaderSource(ShaderObj, 1, p, Lengths);

	//할당된 쉐이더 코드를 컴파일
	glCompileShader(ShaderObj);

	GLint success;
	// ShaderObj 가 성공적으로 컴파일 되었는지 확인
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];

		//OpenGL 의 shader log 데이터를 가져옴
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		printf("%s \n", pShaderText);
	}

	// ShaderProgram 에 attach!!
	glAttachShader(ShaderProgram, ShaderObj);
}

// count = 파티클 개수
void Renderer::CreateParticle(int count)
{
	int floatCount = count * (3+3+1+1+1+1+1+4) * 3 * 2;
	float* particlevertices = new float[floatCount];
	int vertexCount = count * 3 * 2;// drawarrys arg 중 vertex count에 넣어주기위해 미리 계산

	int index = 0;
	float particleSize = 0.01f;

	for (int i = 0; i < count; i++)
	{
		float randomValueX = 0.f;
		float randomValueY = 0.f;
		float randomValueZ = 0.f;
		float randomValueVX = 1.f;
		float randomValueVY = 0.f;
		float randomValueVZ = 0.f;
		float randomEmitTime = 0.f;
		float randomLifeTime = 2.f;
		float randomPeriod = 1.f;// 랜덤주기
		float randomAmp = 1.f; // 진폭
		float randValue = 0.f;

		float randR = 0.f;
		float randG = 0.f;
		float randB = 0.f;
		float randA = 0.f;
		//randomValueX = ((float)rand() / (float)RAND_MAX - 0.5f) * 2.f; //-1~1
		//randomValueY = ((float)rand() / (float)RAND_MAX - 0.5f) * 2.f;
		//randomValueZ = 0.f;

		randomValueVX = ((float)rand() / (float)RAND_MAX - 0.5f) * 0.1f; //-1~1
		randomValueVY = ((float)rand() / (float)RAND_MAX - 0.5f) * 0.1f;
		randomValueVZ = 0.f;

		randomEmitTime = ((float)rand() / (float)RAND_MAX) * 10.f;
		randomLifeTime = ((float)rand() / (float)RAND_MAX) *0.5f; //0~10;
		randomPeriod = ((float)rand() / (float)RAND_MAX) * 10.f +1;
		randomAmp = ((float)rand() / (float)RAND_MAX) * 0.02f - 0.01f;
		randValue = (float)rand() / (float)RAND_MAX;
		randR = (float)rand() / (float)RAND_MAX;
		randG = (float)rand() / (float)RAND_MAX;
		randB = (float)rand() / (float)RAND_MAX;
		randA = (float)rand() / (float)RAND_MAX;

		//V0
		particlevertices[index] = -particleSize / 2.f + randomValueX;
		index++;
		particlevertices[index] = -particleSize / 2.f + randomValueY;
		index++;
		particlevertices[index] = 0.f;
		index++; //position XYZ;

		particlevertices[index] = randomValueVX;
		index++;
		particlevertices[index] = randomValueVY;
		index++;
		particlevertices[index] = 0.f;
		index++; //속도 XYZ;
		particlevertices[index] = randomEmitTime; // 시간
		index++;
		particlevertices[index] = randomLifeTime; //라이프 시간
		index++;
		particlevertices[index] = randomPeriod;
		index++;//주기
		particlevertices[index] = randomAmp;
		index++;//폭

		particlevertices[index] = randValue;
		index++;//rand value

		particlevertices[index] = randR;
		index++;//rand R
		particlevertices[index] = randG;
		index++;//rand G
		particlevertices[index] = randB;
		index++;//rand B
		particlevertices[index] = randA;
		index++;//rand A

		//V1
		particlevertices[index] = particleSize / 2.f + randomValueX;
		index++;
		particlevertices[index] = -particleSize / 2.f + randomValueY;
		index++;
		particlevertices[index] = 0.f;
		index++;

		particlevertices[index] = randomValueVX;
		index++;
		particlevertices[index] = randomValueVY;
		index++;
		particlevertices[index] = 0.f;
		index++; //속도 XYZ;
		particlevertices[index] = randomEmitTime;
		index++;
		particlevertices[index] = randomLifeTime; //라이프 시간
		index++;
		particlevertices[index] = randomPeriod;
		index++;//주기
		particlevertices[index] = randomAmp;
		index++;//폭
		particlevertices[index] = randValue;
		index++;//rand value

		particlevertices[index] = randR;
		index++;//rand R
		particlevertices[index] = randG;
		index++;//rand G
		particlevertices[index] = randB;
		index++;//rand B
		particlevertices[index] = randA;
		index++;//rand A

		//V2
		particlevertices[index] = particleSize / 2.f + randomValueX;
		index++;
		particlevertices[index] = particleSize / 2.f + randomValueY;
		index++;
		particlevertices[index] = 0.f;
		index++;

		particlevertices[index] = randomValueVX;
		index++;
		particlevertices[index] = randomValueVY;
		index++;
		particlevertices[index] = 0.f;
		index++; //속도 XYZ;
		particlevertices[index] = randomEmitTime;
		index++;

		particlevertices[index] = randomLifeTime; //라이프 시간
		index++;
		particlevertices[index] = randomPeriod;
		index++;//주기
		particlevertices[index] = randomAmp;
		index++;//폭
		particlevertices[index] = randValue;
		index++;//rand value

		particlevertices[index] = randR;
		index++;//rand R
		particlevertices[index] = randG;
		index++;//rand G
		particlevertices[index] = randB;
		index++;//rand B
		particlevertices[index] = randA;
		index++;//rand A
		//V3
		particlevertices[index] = -particleSize / 2.f + randomValueX;
		index++;
		particlevertices[index] = -particleSize / 2.f + randomValueY;
		index++;
		particlevertices[index] = 0.f;
		index++;


		particlevertices[index] = randomValueVX;
		index++;
		particlevertices[index] = randomValueVY;
		index++;
		particlevertices[index] = 0.f;
		index++; //속도 XYZ;
		particlevertices[index] = randomEmitTime;
		index++;
		particlevertices[index] = randomLifeTime; //라이프 시간
		index++;
		particlevertices[index] = randomPeriod;
		index++;//주기
		particlevertices[index] = randomAmp;
		index++;//폭
		particlevertices[index] = randValue;
		index++;//rand value

		particlevertices[index] = randR;
		index++;//rand R
		particlevertices[index] = randG;
		index++;//rand G
		particlevertices[index] = randB;
		index++;//rand B
		particlevertices[index] = randA;
		index++;//rand A

		//V4
		particlevertices[index] = particleSize / 2.f + randomValueX;
		index++;
		particlevertices[index] = particleSize / 2.f + randomValueY;
		index++;
		particlevertices[index] = 0.f;
		index++;

		particlevertices[index] = randomValueVX;
		index++;
		particlevertices[index] = randomValueVY;
		index++;
		particlevertices[index] = 0.f;
		index++; //속도 XYZ;
		particlevertices[index] = randomEmitTime;
		index++;
		particlevertices[index] = randomLifeTime; //라이프 시간
		index++;
		particlevertices[index] = randomPeriod;
		index++;//주기
		particlevertices[index] = randomAmp;
		index++;//폭
		particlevertices[index] = randValue;
		index++;//rand value

		particlevertices[index] = randR;
		index++;//rand R
		particlevertices[index] = randG;
		index++;//rand G
		particlevertices[index] = randB;
		index++;//rand B
		particlevertices[index] = randA;
		index++;//rand A

		//V5
		particlevertices[index] = -particleSize / 2.f + randomValueX;
		index++;
		particlevertices[index] = particleSize / 2.f + randomValueY;
		index++;
		particlevertices[index] = 0.f;
		index++;

		particlevertices[index] = randomValueVX;
		index++;
		particlevertices[index] = randomValueVY;
		index++;
		particlevertices[index] = 0.f;
		index++; //속도 XYZ;
		particlevertices[index] = randomEmitTime;
		index++;
		particlevertices[index] = randomLifeTime; //라이프 시간
		index++;
		particlevertices[index] = randomPeriod;
		index++;//주기
		particlevertices[index] = randomAmp;
		index++;//폭
		particlevertices[index] = randValue;
		index++;//rand value

		particlevertices[index] = randR;
		index++;//rand R
		particlevertices[index] = randG;
		index++;//rand G
		particlevertices[index] = randB;
		index++;//rand B
		particlevertices[index] = randA;
		index++;//rand A
	}

	glGenBuffers(1, &m_VBOManyParticle);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOManyParticle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * floatCount, particlevertices,GL_STATIC_DRAW);
	m_VBOManyParticleCount = vertexCount;

}
bool Renderer::ReadFile(char* filename, std::string *target)
{
	std::ifstream file(filename);
	if (file.fail())
	{
		std::cout << filename << " file loading failed.. \n";
		file.close();
		return false;
	}
	std::string line;
	while (getline(file, line)) {
		target->append(line.c_str());
		target->append("\n");
	}
	return true;
}

GLuint Renderer::CompileShaders(char* filenameVS, char* filenameFS)
{
	GLuint ShaderProgram = glCreateProgram(); //빈 쉐이더 프로그램 생성

	if (ShaderProgram == 0) { //쉐이더 프로그램이 만들어졌는지 확인
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, fs;

	//shader.vs 가 vs 안으로 로딩됨
	if (!ReadFile(filenameVS, &vs)) {
		printf("Error compiling vertex shader\n");
		return -1;
	};

	//shader.fs 가 fs 안으로 로딩됨
	if (!ReadFile(filenameFS, &fs)) {
		printf("Error compiling fragment shader\n");
		return -1;
	};

	// ShaderProgram 에 vs.c_str() 버텍스 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	// ShaderProgram 에 fs.c_str() 프레그먼트 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	//Attach 완료된 shaderProgram 을 링킹함
	glLinkProgram(ShaderProgram);

	//링크가 성공했는지 확인
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		// shader program 로그를 받아옴
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error linking shader program\n" << ErrorLog;
		return -1;
	}

	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error validating shader program\n" << ErrorLog;
		return -1;
	}

	glUseProgram(ShaderProgram);
	std::cout << filenameVS << ", " << filenameFS << " Shader compiling is done.\n";

	return ShaderProgram;
}
unsigned char * Renderer::loadBMPRaw(const char * imagepath, unsigned int& outWidth, unsigned int& outHeight)
{
	std::cout << "Loading bmp file " << imagepath << " ... " << std::endl;
	outWidth = -1;
	outHeight = -1;
	// Data read from the header of the BMP file
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	// Actual RGB data
	unsigned char * data;

	// Open the file
	FILE * file = NULL;
	fopen_s(&file, imagepath, "rb");
	if (!file)
	{
		std::cout << "Image could not be opened, " << imagepath << " is missing. " << std::endl;
		return NULL;
	}

	if (fread(header, 1, 54, file) != 54)
	{
		std::cout << imagepath << " is not a correct BMP file. " << std::endl;
		return NULL;
	}

	if (header[0] != 'B' || header[1] != 'M')
	{
		std::cout << imagepath << " is not a correct BMP file. " << std::endl;
		return NULL;
	}

	if (*(int*)&(header[0x1E]) != 0)
	{
		std::cout << imagepath << " is not a correct BMP file. " << std::endl;
		return NULL;
	}

	if (*(int*)&(header[0x1C]) != 24)
	{
		std::cout << imagepath << " is not a correct BMP file. " << std::endl;
		return NULL;
	}

	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	outWidth = *(int*)&(header[0x12]);
	outHeight = *(int*)&(header[0x16]);

	if (imageSize == 0)
		imageSize = outWidth * outHeight * 3;

	if (dataPos == 0)
		dataPos = 54;

	data = new unsigned char[imageSize];

	fread(data, 1, imageSize, file);

	fclose(file);

	std::cout << imagepath << " is succesfully loaded. " << std::endl;

	return data;
}

GLuint Renderer::CreatePngTexture(char * filePath)
{
	//Load Pngs: Load file and decode image.
	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, filePath);
	if (error != 0)
	{
		lodepng_error_text(error);
		assert(error == 0);
		return -1;
	}

	GLuint temp;
	glGenTextures(1, &temp);

	glBindTexture(GL_TEXTURE_2D, temp);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

	return temp;
}

GLuint Renderer::CreateBmpTexture(char * filePath)
{
	//Load Bmp: Load file and decode image.
	unsigned int width, height;
	unsigned char * bmp
		= loadBMPRaw(filePath, width, height);

	if (bmp == NULL)
	{
		std::cout << "Error while loading bmp file : " << filePath << std::endl;
		assert(bmp != NULL);
		return -1;
	}

	GLuint temp;
	glGenTextures(1, &temp);

	glBindTexture(GL_TEXTURE_2D, temp);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bmp);

	return temp;
}

void Renderer::Test()
{
	glUseProgram(m_SolidRectShader);

	GLint VBOLocation = glGetAttribLocation(m_SolidRectShader, "a_Position");
	//int attribPosition = glGetAttribLocation(m_SolidRectShader, "a_Position");
	glEnableVertexAttribArray(VBOLocation);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glVertexAttribPointer(VBOLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	GLint VBOLocation1 = glGetAttribLocation(m_SolidRectShader, "a_Position1");
	glEnableVertexAttribArray(VBOLocation1);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO1);
	glVertexAttribPointer(VBOLocation1, 3, GL_FLOAT, GL_FALSE, 0, 0);


	static float gscale = 0.f;

	// 유니폼은 유니폼으로 받아오고
	// 어트리뷰트는 어트리뷰트로 받아와야함
	GLint ScaleUniform = glGetUniformLocation(m_SolidRectShader, "u_Scale");
	glUniform1f(ScaleUniform, gscale);

	GLint ColorUniform = glGetUniformLocation(m_SolidRectShader, "u_Color");
	glUniform4f(ColorUniform, 1, gscale, 1, 1);

	GLint PositionUniform = glGetUniformLocation(m_SolidRectShader, "u_position");
	glUniform3f(PositionUniform, gscale, gscale, 0);
	
	glDrawArrays(GL_TRIANGLES, 0, 6); // start rendering , primitive (삼각형으로 그려라)'

	gscale += 0.01f;
	if (gscale > 1.f)
	{
		gscale = 0.f;

	}

	glDisableVertexAttribArray(VBOLocation);
	glDisableVertexAttribArray(VBOLocation1);
}

float g_Time = 0.f;

void Renderer::Particle()
{
	GLuint shader = m_SolidRectShader;
	glUseProgram(shader); //shader program select

	GLint VBOLocation = glGetAttribLocation(m_SolidRectShader, "a_Position");
	glEnableVertexAttribArray(VBOLocation);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOManyParticle);
	// 덩어리 나누기 ,어디서 부터 읽어옴
	glVertexAttribPointer(VBOLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float)* 15, (GLvoid*)0);
	
	//a_Velocity
	GLint VBOVLocation = glGetAttribLocation(m_SolidRectShader, "a_Velocity");
	glEnableVertexAttribArray(VBOVLocation);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOManyParticle);
	glVertexAttribPointer(VBOVLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float)* 15, (GLvoid*)(sizeof(float)*3));


	GLint VBOEmitLocation = glGetAttribLocation(m_SolidRectShader, "a_EmitTime");
	glEnableVertexAttribArray(VBOEmitLocation);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOManyParticle);
	glVertexAttribPointer(VBOEmitLocation, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (GLvoid*)(sizeof(float) * 6));

	GLint VBOLifeLocation = glGetAttribLocation(m_SolidRectShader, "a_LifeTime");
	glEnableVertexAttribArray(VBOLifeLocation);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOManyParticle);
	glVertexAttribPointer(VBOLifeLocation, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 15,(GLvoid*)(sizeof(float) * 7));

	GLint VBOPLocation = glGetAttribLocation(m_SolidRectShader, "a_P");
	glEnableVertexAttribArray(VBOPLocation);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOManyParticle);
	glVertexAttribPointer(VBOPLocation, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (GLvoid*)(sizeof(float) * 8));

	GLint VBOALocation = glGetAttribLocation(m_SolidRectShader, "a_A");
	glEnableVertexAttribArray(VBOALocation);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOManyParticle);
	glVertexAttribPointer(VBOALocation, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (GLvoid*)(sizeof(float) * 9));
	
	GLint VBORLocation = glGetAttribLocation(m_SolidRectShader, "a_RandValue");
	glEnableVertexAttribArray(VBORLocation);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOManyParticle);
	glVertexAttribPointer(VBORLocation, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (GLvoid*)(sizeof(float) * 10));

	GLint VBOColorLocation = glGetAttribLocation(m_SolidRectShader, "a_Color");
	glEnableVertexAttribArray(VBOColorLocation);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOManyParticle);
	glVertexAttribPointer(VBOColorLocation, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (GLvoid*)(sizeof(float) * 11));

	//

	//유니폼 값 받아오기
	GLint UniformTime = glGetUniformLocation(shader, "u_Time");
	//어디에 넣을것인가, 뭘넣을 것인가
	glUniform1f(UniformTime, g_Time);

	glDrawArrays(GL_TRIANGLES, 0, m_VBOManyParticleCount);

	g_Time += 0.016f;
}

void Renderer::FSSandbox()
{
	GLuint shader = m_VBOFSSandboxShader;
	glUseProgram(shader); //shader program select

	GLuint attribPosLoc = glGetAttribLocation(shader, "a_Position");
	glEnableVertexAttribArray(attribPosLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOFSSandBox);
	glVertexAttribPointer(attribPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3,(GLvoid*)(0));

	GLuint uniformPointLoc = glGetUniformLocation(shader, "u_Point");
	glUniform3f(uniformPointLoc, 0.5f, 0.5f, 0.f);

	glDrawArrays(GL_TRIANGLES, 0, 6);

}
