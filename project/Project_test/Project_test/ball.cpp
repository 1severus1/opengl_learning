#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <math.h>
#include "shader.h"

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

	std::vector <float> ball;

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

	std::vector <int> ball_index;

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

	Shader m_shader("C:\\Users\\severus\\Desktop\\OpenGL_learn\\shadervertexshader.vs", "C:\\Users\\severus\\Desktop\\OpenGL_learn\\shaderfragmentshader.fs");


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
		m_shader.use();

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