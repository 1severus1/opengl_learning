#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <math.h>
using namespace std;

#define A_SEGMENTS 100
#define B_SEGMENTS 100
#define PI 3.1415926
int draw_ball() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	int screen_width = 720;
	int screen_height = 720;

	GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "show result", NULL, NULL);
	if (window == NULL) {
		printf("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initialized GLAD");
		return -1;
	}

	glViewport(0, 0, screen_width, screen_height);

	vector <float> ball;

	//生成球表面点坐标，存入vector
	//实际上顶点和底点有重复，每一行的最后一个点和第一个点重合
	//注意坐标系应该与gl统一，否则例如此处画出来的是球的俯视图（或者仰视图？）
	for (int a = 0; a <=A_SEGMENTS; a++) {
		for (int b = 0; b <=B_SEGMENTS; b++) {
			float alph = (float)a / (float)A_SEGMENTS*PI;
			float beta = (float)b / (float)B_SEGMENTS*2*PI;

			float xPos = sin(alph) * cos(beta);
			float yPos = sin(alph) * sin(beta);
			float zPos = cos(alph);

			ball.push_back(xPos);
			ball.push_back(yPos);
			ball.push_back(zPos);

		}
	}

	vector <int> ball_index;

	//访问索引
	//每个vertex连接两个三角形，总共六个
	//注意顶点和底点重复的顶点在此时的作用，以及每一行最后一个重复点也有相同作用
	for (int i = 0; i <= A_SEGMENTS; i++) {
		for (int j = 0; j <= B_SEGMENTS; j++) {
			ball_index.push_back(i * (B_SEGMENTS+1) + j);
			ball_index.push_back(i * (B_SEGMENTS+1) + j + 1);
			ball_index.push_back((i + 1) * (B_SEGMENTS + 1) + j+1);

			ball_index.push_back(i * (B_SEGMENTS + 1) + j);
			ball_index.push_back((i + 1) * (B_SEGMENTS + 1) + j + 1);
			ball_index.push_back((i + 1) * (B_SEGMENTS + 1) + j );

		}
	}

	GLuint element_buffer_object;
	GLuint vertex_array_object;
	GLuint vertex_buffer_object;
	glGenVertexArrays(1, &vertex_array_object);
	glGenBuffers(1, &vertex_buffer_object);
	glGenBuffers(1, &element_buffer_object);
	glBindVertexArray(vertex_array_object);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
	//此处函数期望输入字节数和C类型的数组指针，但是我们使用了vector
	//则需要使用第一个元素的指针来表示一个C类型的数组指针，也可使用ball_index.data(),简洁表示（C11特性）
	//输入EBO信息时相同
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*ball.size(), &ball[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*ball_index.size(), &ball_index[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	const char* vertex_shader_source =
		"#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = vec4(aPos, 1.0);\n"
		"}\n\0";
	const char* fragment_shader_source =
		"#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"    FragColor = vec4(1.0f, 0.8f, 0.4f, 1.0f);\n"
		"}\n\0";


	int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);
	int success;
	char info_log[512];

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
	}

	int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << std::endl;
	}

	int shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shader_program, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	// 线框模式
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//剔除背面
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	while (!glfwWindowShouldClose(window)) {

		// 清空颜色缓冲
		glClearColor(0.0f, 0.34f, 0.57f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// 使用着色器程序
		glUseProgram(shader_program);

		// 绘制三角形
		glBindVertexArray(vertex_array_object);
		// 当引入索引的时候必须使用glDrawElements代替glDrawArray,这两者的功能基本相同
		glDrawElements(GL_TRIANGLES, (A_SEGMENTS+1)*(B_SEGMENTS+1)*6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// 交换缓冲并且检查是否有触发事件(比如键盘输入、鼠标移动等）
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// 删除VAO和VBO
	glDeleteVertexArrays(1, &vertex_array_object);
	glDeleteBuffers(1, &vertex_buffer_object);

	// 清理所有的资源并正确退出程序
	glfwTerminate();

	return 0;
}