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

	//�������������꣬����vector
	//ʵ���϶���͵׵����ظ���ÿһ�е����һ����͵�һ�����غ�
	//ע������ϵӦ����glͳһ����������˴�������������ĸ���ͼ����������ͼ����
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

	//��������
	//ÿ��vertex�������������Σ��ܹ�����
	//ע�ⶥ��͵׵��ظ��Ķ����ڴ�ʱ�����ã��Լ�ÿһ�����һ���ظ���Ҳ����ͬ����
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
	//�˴��������������ֽ�����C���͵�����ָ�룬��������ʹ����vector
	//����Ҫʹ�õ�һ��Ԫ�ص�ָ������ʾһ��C���͵�����ָ�룬Ҳ��ʹ��ball_index.data(),����ʾ��C11���ԣ�
	//����EBO��Ϣʱ��ͬ
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

	// �߿�ģʽ
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//�޳�����
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	while (!glfwWindowShouldClose(window)) {

		// �����ɫ����
		glClearColor(0.0f, 0.34f, 0.57f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// ʹ����ɫ������
		glUseProgram(shader_program);

		// ����������
		glBindVertexArray(vertex_array_object);
		// ������������ʱ�����ʹ��glDrawElements����glDrawArray,�����ߵĹ��ܻ�����ͬ
		glDrawElements(GL_TRIANGLES, (A_SEGMENTS+1)*(B_SEGMENTS+1)*6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// �������岢�Ҽ���Ƿ��д����¼�(����������롢����ƶ��ȣ�
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// ɾ��VAO��VBO
	glDeleteVertexArrays(1, &vertex_array_object);
	glDeleteBuffers(1, &vertex_buffer_object);

	// �������е���Դ����ȷ�˳�����
	glfwTerminate();

	return 0;
}