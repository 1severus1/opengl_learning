#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class Shader {
public:
	unsigned int ID;
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
};
