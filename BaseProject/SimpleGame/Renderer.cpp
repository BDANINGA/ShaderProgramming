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
	m_FSShader = CompileShaders("./Shaders/FS.vs", "./Shaders/FS.fs");
	
	//Create VBOs
	// CreateVertexBufferObjects();
	// CreateParticleVBO(1000);
	CreateFSBufferObjects();

	if (m_SolidRectShader > 0 && m_VBORect > 0)
	{
		m_Initialized = true;
	}
}

bool Renderer::IsInitialized()
{
	return m_Initialized;
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

void Renderer::GetGLPosition(float x, float y, float* newX, float* newY)
{
	*newX = x * 2.f / m_WindowSizeX;
	*newY = y * 2.f / m_WindowSizeY;
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
		centerx - size / 2, centery - size / 2, 0, mass, vx, vy,
		centerx + size / 2, centery - size / 2, 0, mass, vx, vy,
		centerx + size / 2, centery + size / 2, 0, mass, vx, vy,// triangle1

		centerx - size / 2, centery - size / 2, 0, mass, vx, vy,
		centerx + size / 2, centery + size / 2, 0, mass, vx, vy,
		centerx - size / 2, centery + size / 2, 0, mass, vx, vy // triangle2
	};

	glGenBuffers(1, &m_VBOTriangle);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOTriangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

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

void Renderer::CreateFSBufferObjects()
{
	float centerx = 0;
	float centery = 0;

	float fs[]	// x, y, z, tx, ty : stride 5
		=
	{
		-1.f, -1.f, 0, 0, 1,
		1.f, -1.f, 0, 1, 1,
		1.f, 1.f, 0, 1, 0, // triangle1

		-1.f, -1.f, 0, 0, 1,
		1.f, 1.f, 0, 1, 0,
		-1.f, 1.f, 0, 0, 0 // triangle2
	};

	glGenBuffers(1, &m_VBOFS);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOFS);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fs), fs, GL_STATIC_DRAW);
}
	
void Renderer::DrawFS()
{
	//Program select
	glUseProgram(m_FSShader);

	int attribPosition = glGetAttribLocation(m_FSShader, "a_Position");
	int attribTPos = glGetAttribLocation(m_FSShader, "a_TPos");

	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(attribTPos);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOFS);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	glVertexAttribPointer(attribTPos, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float)*3));
	
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(attribPosition);
	glDisableVertexAttribArray(attribTPos);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::CreateParticleVBO(int numParticles)
{
	// 입자 1개당 정점 6개(삼각형 2개), 각 정점당 float 8개
	std::vector<float> vertices;
	vertices.reserve(numParticles * 6 * 8);

	for (int i = 0; i < numParticles; ++i)
	{
		float startX = 0.0f;
		float startY = 0.0f;
		float size = 0.005f; // 사각형 절반 크기
		float mass = 1.0f;
		float vx = ((rand() % 2000) / 1000.0f) - 1.0f; // -1.0 ~ 1.0
		float vy = ((rand() % 2000) / 1000.0f);        // 0.0 ~ 2.0
		float RV = ((rand() % 2000) / 1000.0f);

		// 사각형을 구성하는 6개의 정점 (두 개의 삼각형: LT-LB-RB, LT-RB-RT)
		float rectOffsets[6][2] = {
			{ -size,  size }, { -size, -size }, {  size, -size }, // Triangle 1
			{ -size,  size }, {  size, -size }, {  size,  size }  // Triangle 2
		};

		for (int j = 0; j < 6; ++j) {
			vertices.push_back(startX + rectOffsets[j][0]); // a_Position.x
			vertices.push_back(startY + rectOffsets[j][1]); // a_Position.y
			vertices.push_back(0.0f);                       // a_Position.z
			vertices.push_back(mass);                       // a_Mass
			vertices.push_back(vx);                         // a_Vel.x
			vertices.push_back(vy);                         // a_Vel.y
			vertices.push_back(RV);                         // a_RV
			vertices.push_back(size);                       // a_Size
		}
	}

	if (m_VBOParticle == 0) glGenBuffers(1, &m_VBOParticle);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOParticle);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
}

void Renderer::DrawParticles(int numParticles)
{
	g_time += 0.001;

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

	// Stride와 Offset 설정 (기존과 동일하게 sizeof(float) * 8 유지)
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
	glVertexAttribPointer(massLoc, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
	glVertexAttribPointer(velLoc, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 4));
	glVertexAttribPointer(RvelLoc, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));
	glVertexAttribPointer(sizeLoc, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 7));

	// ★ 중요: 그리는 정점 개수를 numParticles * 6으로 변경
	glDrawArrays(GL_TRIANGLES, 0, numParticles * 6);

	glDisableVertexAttribArray(posLoc);
	glDisableVertexAttribArray(massLoc);
	glDisableVertexAttribArray(velLoc);
	glDisableVertexAttribArray(RvelLoc);
	glDisableVertexAttribArray(sizeLoc);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}