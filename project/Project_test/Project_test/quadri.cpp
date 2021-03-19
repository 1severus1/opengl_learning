#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>

int draw_quadri() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	int screen_width = 1280;
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
	 
	// 四边形顶点位置数据
	// 配合EBO进行一定顺序的访问
	float quadri[] = {
		//     ---- 位置 ----    
			 -0.5f, -0.5f, 0.0f,   // 左下
			  0.5f, -0.5f, 0.0f,   // 右下
			  0.5f,  0.5f, 0.0f,   // 右上
			  -0.5f, 0.5f, 0.0f,   // 左上
	};

	//设置访问顺序，即索引数据
	unsigned int order[]{
		0,1,2,
		0,3,2
	};

	//生成绑定vao,vbo,ebo
	//注意相互之间的先后顺序，以下顺序为参考标准顺序
	//EBO不解绑，其存储在VAO中
	GLuint element_buffer_object;
	GLuint vertex_array_object;
	GLuint vertex_buffer_object;
	glGenVertexArrays(1, &vertex_array_object);
	glGenBuffers(1, &vertex_buffer_object);
	glGenBuffers(1, &element_buffer_object);
	glBindVertexArray(vertex_array_object);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadri), quadri, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(order), order, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// 解绑VAO和VBO
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
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window)) {

		// 清空颜色缓冲
		glClearColor(0.0f, 0.34f, 0.57f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// 使用着色器程序
		glUseProgram(shader_program);

		// 绘制三角形
		glBindVertexArray(vertex_array_object);	                                            
		// 当引入索引的时候必须使用glDrawElements代替glDrawArray,这两者的功能基本相同
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);                                
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