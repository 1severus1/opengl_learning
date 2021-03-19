#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>
#include <vector>
using namespace std;


int draw_triangle() {
	//初始化，版本号，以及工作下核心模式（不向下兼容）
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
	//尺寸，标题，是否全屏，是否共享上下文窗口
	GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "show result", NULL, NULL);
	if (window == NULL) {
		printf("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}
	//设置为当前线程的主上下文
	//opengl本身为状态机，状态称为上下文
	glfwMakeContextCurrent(window);

	//初始化GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initialized GLAD");
		return -1;
	}

	//视口位置（0,0）和尺寸，即opengl渲染部分显示窗口
	glViewport(0, 0, screen_width, screen_height);

	// 三角形的顶点数据
	 float triangle[]= {
		//     ---- 位置 ----    
			 -0.5f, -0.5f, 0.0f,   // 左下
			  0.5f, -0.5f, 0.0f,   // 右下
			  0.0f,  0.5f, 0.0f    // 正上
	};

	// VAO和VBO生成和绑定，1表示只传入了一个GLuint对象（可传入数组）
	// GLuint 实际为unsigned int
	// 即n*sizeof(GLuint)字节的数据是有效的，VBO生成相同
	// VBO一次传入若干顶点信息
	// 核心模式要求VAO，且可不再调用VBO
	// 绑定以后可修改可进行正常工作？如传入数据，定义属性指针等，解绑后VAO,VBO依然存在于显卡内
	GLuint vertex_array_object;
	glGenVertexArrays(1, &vertex_array_object);
	glBindVertexArray(vertex_array_object);
	GLuint vertex_buffer_object;
	glGenBuffers(1, &vertex_buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);

	// 将顶点数据绑定至当前默认的缓冲中
	// 第四个参数表示三角形位置数据不变
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

	// 设置顶点属性指针
	// 位置值0，顶点属性为3分量的向量（此处只有顶点的位置），顶点数据类型，是否标准化，
	// 步长（顶点属性之间的间隔，与之前的3分量浮点数对应），偏移量（从头到数据开始的位置）
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//开启0通道
	glEnableVertexAttribArray(0);

	// 解绑VAO和VBO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// 顶点着色器和片段着色器源码(GLSL) 
	const char* vertex_shader_source =
		"#version 330 core\n"							
		"layout (location = 0) in vec3 aPos;\n"           // 位置变量的属性位置值为0，与顶点属性指针的0对应,输入到aPos中
		"void main()\n"
		"{\n"
		"    gl_Position = vec4(aPos, 1.0);\n"			// 将顶点属性向量aPos（仅有位置）外加一个值作为4分量向量输入内渐变量gl_Position
		"}\n\0";
	const char* fragment_shader_source =
		"#version 330 core\n"
		"out vec4 FragColor;\n"                           // 输出的颜色向量
		"void main()\n"
		"{\n"
		"    FragColor = vec4(1.0f, 0.8f, 0.4f, 1.0f);\n"
		"}\n\0";

	// 生成并编译着色器
   // 顶点着色器，三步应该为：get编号，get源码，编译
	int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);
	int success;
	char info_log[512];
	// 检查着色器是否成功编译，如果编译失败，打印错误信息
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
	}
	// 片段着色器
	int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);
	// 检查着色器是否成功编译，如果编译失败，打印错误信息
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << std::endl;
	}
	// 链接顶点和片段着色器至一个着色器程序
	int shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	// 检查着色器是否成功链接，如果链接失败，打印错误信息
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shader_program, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
	}
	// 删除着色器
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	// 线框模式
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// 渲染循环
	while (!glfwWindowShouldClose(window)) {

		// 清空颜色缓冲
		glClearColor(0.0f, 0.34f, 0.57f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// 使用着色器程序
		glUseProgram(shader_program);

		// 绘制三角形
		glBindVertexArray(vertex_array_object);                           // 绑定VAO
		// 
		glDrawArrays(GL_TRIANGLES, 0, 3);                                 // 绘制三角形，参数分别为，绘制三角形，顶点数组起始索引，数量
		glBindVertexArray(0);                                             // 解除绑定

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