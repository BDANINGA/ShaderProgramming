#include "stdafx.h"
#include "Renderer.h"
#include <vector>

Renderer::Renderer(int windowSizeX, int windowSizeY)
{
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
	m_TriangleShader = CompileShaders("./Shaders/Triangle.vs", "./Shaders/Triangle.fs");
	
	//Create VBOs
	CreateVertexBufferObjects();
	CreateParticleVBO(1000);

	if (m_SolidRectShader > 0 && m_VBORect > 0)
	{
		m_Initialized = true;
	}
}

bool Renderer::IsInitialized()
{
	return m_Initialized;
}

void Renderer::CreateVertexBufferObjects()
{
	float rect[]
		=
	{
		-1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f, -1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, 1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, //Triangle1
		-1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f,  1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, 1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f, //Triangle2
	};

	glGenBuffers(1, &m_VBORect);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);

	float centerx = 0;
	float centery = 0;
	float size = 0.1;
	float mass = 1;
	float vx = 1;
	float vy = 3;
	float triangle[]
		=
	{
		centerx - size/2, centery - size / 2, 0, mass, vx, vy,
		centerx + size/2, centery - size/2, 0, mass, vx, vy,
		centerx + size/2, centery + size/2, 0, mass, vx, vy,// triangle1
		
		centerx - size / 2, centery - size / 2, 0, mass, vx, vy,
		centerx + size / 2, centery + size / 2, 0, mass, vx, vy,
		centerx - size/2, centery + size / 2, 0, mass, vx, vy // triangle2
	};

	glGenBuffers(1, &m_VBOTriangle);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOTriangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

}

void Renderer::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	//���̴� ������Ʈ ����
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);
	//���̴� �ڵ带 ���̴� ������Ʈ�� �Ҵ�
	glShaderSource(ShaderObj, 1, p, Lengths);

	//�Ҵ�� ���̴� �ڵ带 ������
	glCompileShader(ShaderObj);

	GLint success;
	// ShaderObj �� ���������� ������ �Ǿ����� Ȯ��
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];

		//OpenGL �� shader log �����͸� ������
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		printf("%s \n", pShaderText);
	}

	// ShaderProgram �� attach!!
	glAttachShader(ShaderProgram, ShaderObj);
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
	GLuint ShaderProgram = glCreateProgram(); //�� ���̴� ���α׷� ����

	if (ShaderProgram == 0) { //���̴� ���α׷��� ����������� Ȯ��
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, fs;

	//shader.vs �� vs ������ �ε���
	if (!ReadFile(filenameVS, &vs)) {
		printf("Error compiling vertex shader\n");
		return -1;
	};

	//shader.fs �� fs ������ �ε���
	if (!ReadFile(filenameFS, &fs)) {
		printf("Error compiling fragment shader\n");
		return -1;
	};

	// ShaderProgram �� vs.c_str() ���ؽ� ���̴��� �������� ����� attach��
	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	// ShaderProgram �� fs.c_str() �����׸�Ʈ ���̴��� �������� ����� attach��
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	//Attach �Ϸ�� shaderProgram �� ��ŷ��
	glLinkProgram(ShaderProgram);

	//��ũ�� �����ߴ��� Ȯ��
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		// shader program �α׸� �޾ƿ�
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
	std::cout << filenameVS << ", " << filenameFS << " Shader compiling is done.";

	return ShaderProgram;
}

void Renderer::DrawSolidRect(float x, float y, float z, float size, float r, float g, float b, float a)
{
	float newX, newY;

	GetGLPosition(x, y, &newX, &newY);

	//Program select
	glUseProgram(m_SolidRectShader);

	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Trans"), newX, newY, 0, size);
	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Color"), r, g, b, a);

	int attribPosition = glGetAttribLocation(m_SolidRectShader, "a_Position");
	glEnableVertexAttribArray(attribPosition);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(attribPosition);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

float g_time = 0;

void Renderer::DrawTriangle()
{
	g_time += 0.0001;

	//Program select
	glUseProgram(m_TriangleShader);

	int uTime = glGetUniformLocation(m_TriangleShader, "u_Time");

	glUniform1f(uTime, g_time);

	int attribPosition = glGetAttribLocation(m_TriangleShader, "a_Position");
	int attribMass = glGetAttribLocation(m_TriangleShader, "a_Mass");
	int attribVel = glGetAttribLocation(m_TriangleShader, "a_Vel");

	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(attribMass);
	glEnableVertexAttribArray(attribVel);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOTriangle);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glVertexAttribPointer(attribMass, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (GLvoid*)(sizeof(float)* 3));
	glVertexAttribPointer(attribVel, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (GLvoid*)(sizeof(float) * 4));

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(attribPosition);
	glDisableVertexAttribArray(attribMass);
	glDisableVertexAttribArray(attribVel);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::GetGLPosition(float x, float y, float *newX, float *newY)
{
	*newX = x * 2.f / m_WindowSizeX;
	*newY = y * 2.f / m_WindowSizeY;
}

void Renderer::CreateParticleVBO(int numParticles)
{
	// 1. 데이터 컨테이너 준비
	// 삼각형 1개당 정점 3개, 정점당 float 6개 (x, y, z, mass, vx, vy)
	std::vector<float> vertices;
	vertices.reserve(numParticles * 3 * 8);

	for (int i = 0; i < numParticles; ++i)
	{
		// 입자별 공통 속성 생성 (랜덤 값 예시)
		float startX = 0.0f; 
		float startY = 0.0f; 
		float size = 0.15f;  
		float mass = 1.0f;  
		float vx = ((rand() % 2000) / 500.0f) - 2.0f;      // -1.0 ~ 1.0
		float vy = ((rand() % 2000) / 500.0f);     // 초기 상방 속도
		float RV = ((rand() % 2000) / 1000.0f);

		// 삼각형의 3개 정점 생성 (중심점을 기준으로 size만큼 떨어진 위치)
		// 각 정점은 동일한 질량과 속도 데이터를 공유해야 물리 연산이 일관되게 적용됨
		float triangleOffsets[3][2] = {
			{ 0.0f, size },           // 위
			{ -size, -size },         // 왼쪽 아래
			{ size, -size }           // 오른쪽 아래
		};

		for (int j = 0; j < 3; ++j) {
			vertices.push_back(startX + triangleOffsets[j][0]); // x
			vertices.push_back(startY + triangleOffsets[j][1]); // y
			vertices.push_back(0.0f);                           // z
			vertices.push_back(mass);                           // a_Mass
			vertices.push_back(vx);                             // a_Vel.x
			vertices.push_back(vy);                             // a_Vel.y
			vertices.push_back(RV);
			vertices.push_back(size);
		}
	}

	// 2. GPU 버퍼 생성 및 데이터 전송
	if (m_VBOParticle == 0) {
		glGenBuffers(1, &m_VBOParticle);
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOParticle);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	std::cout << numParticles << " Particles created in VBO.\n";
}

void Renderer::DrawParticles(int numParticles)
{
	g_time += 0.0001;

	glUseProgram(m_TriangleShader);
	glUniform1f(glGetUniformLocation(m_TriangleShader, "u_Time"), g_time);

	GLint posLoc = glGetAttribLocation(m_TriangleShader, "a_Position");
	GLint massLoc = glGetAttribLocation(m_TriangleShader, "a_Mass");
	GLint velLoc = glGetAttribLocation(m_TriangleShader, "a_Vel");
	GLint RvelLoc = glGetAttribLocation(m_TriangleShader, "a_RV");
	GLint sizeLoc = glGetAttribLocation(m_TriangleShader, "a_Size");

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOParticle);
	glEnableVertexAttribArray(posLoc);
	glEnableVertexAttribArray(massLoc);
	glEnableVertexAttribArray(velLoc);
	glEnableVertexAttribArray(RvelLoc);
	glEnableVertexAttribArray(sizeLoc);

	// Stride는 6 * sizeof(float)
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
	glVertexAttribPointer(massLoc, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
	glVertexAttribPointer(velLoc, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 4));
	glVertexAttribPointer(RvelLoc, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));
	glVertexAttribPointer(sizeLoc, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 7));

	// 정점 개수 = 입자 수 * 3
	glDrawArrays(GL_TRIANGLES, 0, numParticles * 3);

	glDisableVertexAttribArray(posLoc);
	glDisableVertexAttribArray(massLoc);
	glDisableVertexAttribArray(velLoc);
	glDisableVertexAttribArray(RvelLoc);
	glDisableVertexAttribArray(sizeLoc);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}