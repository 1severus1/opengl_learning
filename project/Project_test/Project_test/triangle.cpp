#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>
#include <vector>
using namespace std;


int draw_triangle() {
	//��ʼ�����汾�ţ��Լ������º���ģʽ�������¼��ݣ�
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//show glfw version
	int Major, Minor, Rev;
	glfwGetVersion(&Major, &Minor, &Rev);
	printf("GLFW %d.%d.%d initialized\n", Major, Minor, Rev);

	int screen_width = 1280;
	int screen_height = 720;

	//create windows
	//�ߴ磬���⣬�Ƿ�ȫ�����Ƿ��������Ĵ���
	GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "show result", NULL, NULL);
	if (window == NULL) {
		printf("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}
	//����Ϊ��ǰ�̵߳���������
	//opengl����Ϊ״̬����״̬��Ϊ������
	glfwMakeContextCurrent(window);

	//��ʼ��GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initialized GLAD");
		return -1;
	}

	//�ӿ�λ�ã�0,0���ͳߴ磬��opengl��Ⱦ������ʾ����
	glViewport(0, 0, screen_width, screen_height);

	// �����εĶ�������
	 float triangle[]= {
		//     ---- λ�� ----    
			 -0.5f, -0.5f, 0.0f,   // ����
			  0.5f, -0.5f, 0.0f,   // ����
			  0.0f,  0.5f, 0.0f    // ����
	};

	// VAO��VBO���ɺͰ󶨣�1��ʾֻ������һ��GLuint���󣨿ɴ������飩
	// GLuint ʵ��Ϊunsigned int
	// ��n*sizeof(GLuint)�ֽڵ���������Ч�ģ�VBO������ͬ
	// VBOһ�δ������ɶ�����Ϣ
	// ����ģʽҪ��VAO���ҿɲ��ٵ���VBO
	// ���Ժ���޸Ŀɽ��������������紫�����ݣ���������ָ��ȣ�����VAO,VBO��Ȼ�������Կ���
	GLuint vertex_array_object;
	glGenVertexArrays(1, &vertex_array_object);
	glBindVertexArray(vertex_array_object);
	GLuint vertex_buffer_object;
	glGenBuffers(1, &vertex_buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);

	// ���������ݰ�����ǰĬ�ϵĻ�����
	// ���ĸ�������ʾ������λ�����ݲ���
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

	// ���ö�������ָ��
	// λ��ֵ0����������Ϊ3�������������˴�ֻ�ж����λ�ã��������������ͣ��Ƿ��׼����
	// ��������������֮��ļ������֮ǰ��3������������Ӧ����ƫ��������ͷ�����ݿ�ʼ��λ�ã�
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//����0ͨ��
	glEnableVertexAttribArray(0);

	// ���VAO��VBO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// ������ɫ����Ƭ����ɫ��Դ��(GLSL) 
	const char* vertex_shader_source =
		"#version 330 core\n"							
		"layout (location = 0) in vec3 aPos;\n"           // λ�ñ���������λ��ֵΪ0���붥������ָ���0��Ӧ,���뵽aPos��
		"void main()\n"
		"{\n"
		"    gl_Position = vec4(aPos, 1.0);\n"			// ��������������aPos������λ�ã����һ��ֵ��Ϊ4�������������ڽ�����gl_Position
		"}\n\0";
	const char* fragment_shader_source =
		"#version 330 core\n"
		"out vec4 FragColor;\n"                           // �������ɫ����
		"void main()\n"
		"{\n"
		"    FragColor = vec4(1.0f, 0.8f, 0.4f, 1.0f);\n"
		"}\n\0";

	// ���ɲ�������ɫ��
   // ������ɫ��������Ӧ��Ϊ��get��ţ�getԴ�룬����
	int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);
	int success;
	char info_log[512];
	// �����ɫ���Ƿ�ɹ����룬�������ʧ�ܣ���ӡ������Ϣ
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
	}
	// Ƭ����ɫ��
	int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);
	// �����ɫ���Ƿ�ɹ����룬�������ʧ�ܣ���ӡ������Ϣ
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << std::endl;
	}
	// ���Ӷ����Ƭ����ɫ����һ����ɫ������
	int shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	// �����ɫ���Ƿ�ɹ����ӣ��������ʧ�ܣ���ӡ������Ϣ
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shader_program, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
	}
	// ɾ����ɫ��
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	// �߿�ģʽ
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// ��Ⱦѭ��
	while (!glfwWindowShouldClose(window)) {

		// �����ɫ����
		glClearColor(0.0f, 0.34f, 0.57f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// ʹ����ɫ������
		glUseProgram(shader_program);

		// ����������
		glBindVertexArray(vertex_array_object);                           // ��VAO
		// 
		glDrawArrays(GL_TRIANGLES, 0, 3);                                 // ���������Σ������ֱ�Ϊ�����������Σ�����������ʼ����������
		glBindVertexArray(0);                                             // �����

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