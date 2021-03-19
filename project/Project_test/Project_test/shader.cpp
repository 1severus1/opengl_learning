#include "shader.h"



Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath) {
	
	//��ȡGLSL��shader�������ַ����洢
	std::string vertexCode;
	std::string fragmentCode;

	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	vShaderFile.open(vertexPath);
	fShaderFile.open(fragmentPath);

	//to be finish
	//
	//
	

	//���GLSL������Ϊ�ַ�������vShaderCode��fShaderCode
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	std::cout << vShaderCode << std::endl;

	//��Ϊ��ɫ���ı�ʶ��
	unsigned int vertex, fragment;
	
	//���벢���
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	int success;
	char info_log[512];
	// �����ɫ���Ƿ�ɹ����룬�������ʧ�ܣ���ӡ������Ϣ
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
	}
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << std::endl;
	}

	//���Ӷ����Ƭ����ɫ��������ID����ɫ����
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	//�����ɫ����
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use() {
	glUseProgram(ID);
}