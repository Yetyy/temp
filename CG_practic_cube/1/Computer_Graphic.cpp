#define GLM_ENABLE_EXPERIMENTAL

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/transform.hpp>
#include <stdio.h>
#include <iostream>


const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

layout (location = 1) in vec3 aColor;
out vec3 color;

uniform mat4 rotation;
void main()
{
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	color = aColor;
}
)";

const char* fragmentShader1Source = R"(
#version 330 core
in vec3 color;
out vec4 FragColor;


void main()
{
	FragColor = vec4(0.4f, 0.1f, 0.5f, 1.0f);
	FragColor = vec4(color, 1.0f);
}
)";

const char* fragmentShader2Source = R"(
#version 330 core
in vec3 color;
out vec4 FragColor;
void main()
{
	FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);
	FragColor = vec4(color, 1.0f);
}
)";

int main()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}


	GLFWwindow* window = glfwCreateWindow(640, 480, "Hello Triangle", NULL, NULL);
	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	/* tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

	/* OTHER STUFF GOES HERE NEXT */

	// close GL context and any other GLFW resources
	//glfwTerminate();


	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

	unsigned int fragmenShaderFirst = glCreateShader(GL_FRAGMENT_SHADER);
	unsigned int fragmenShaderSecond = glCreateShader(GL_FRAGMENT_SHADER);

	unsigned int shaderProgramFirst = glCreateProgram();
	unsigned int shaderProgramSecond = glCreateProgram();

	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	glShaderSource(fragmenShaderFirst, 1, &fragmentShader1Source, NULL);
	glCompileShader(fragmenShaderFirst);

	glShaderSource(fragmenShaderSecond, 1, &fragmentShader2Source, NULL);
	glCompileShader(fragmenShaderSecond);


	glAttachShader(shaderProgramFirst, vertexShader);
	glAttachShader(shaderProgramFirst, fragmenShaderFirst);
	glLinkProgram(shaderProgramFirst);


	glAttachShader(shaderProgramSecond, vertexShader);
	glAttachShader(shaderProgramSecond, fragmenShaderSecond);
	glLinkProgram(shaderProgramSecond);

	float firstTriangle[] = {

		-0.9f, 0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
		-0.0f, 0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
		-0.45f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f
	};
	float secondTriangle[] = {

		0.0f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
		0.9f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
		0.45f, 0.5f, 0.0f,  0.0f, 1.0f, 0.0f
	};


	/*
	float firstTriangle[] = {
		-0.9f, -0.5f,  0.0f,
		-0.0f, -0.5f,  0.0f,
		-0.45f, 0.5f,  0.0f
	};
	float secondTriangle[] = {
		0.0f, -0.5f,  0.0f,
		0.9f, -0.5f,  0.0f,
		0.45f, 0.5f,  0.0f
	};
	*/

	unsigned int VBOs[2], VAOs[2];
	glGenVertexArrays(2, VAOs);
	glGenBuffers(2, VBOs);

	// 1
	glBindVertexArray(VAOs[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(firstTriangle), firstTriangle, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // ÷вета
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);// ÷вета

	// 2
	//glBindVertexArray(VAOs[1]);
	//glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(secondTriangle), secondTriangle, GL_STATIC_DRAW);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // ÷вета
	//glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);// ÷вета

	while (!glfwWindowShouldClose(window)) {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glUseProgram(shaderProgramFirst);
		glBindVertexArray(VAOs[0]);

		
		glm::mat4  model(1.0f);
		//model = glm::translate(model, glm::vec3(0.3f, -0.3f, 0.0f));
		model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
		GLuint rotationLoc = glGetUniformLocation(shaderProgramFirst, "rotation");
		glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, &model[0][0]);
		

		glDrawArrays(GL_TRIANGLES, 0, 3);


		glUseProgram(shaderProgramSecond);
		glBindVertexArray(VAOs[1]);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	glDeleteVertexArrays(2, VAOs);
	glDeleteBuffers(2, VBOs);

	glfwTerminate();

	return 0;
}





//while (!glfwWindowShouldClose(window)) {
	//    // очищаем экран
	//    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//    // обновить другие событи€, такие как обработка ввода
	//    glfwPollEvents();
	//    // поместите то, что мы рисовали, на дисплей
	//    glfwSwapBuffers(window);
	//}
/*
const char* vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 color;

uniform mat4 rotation;

void main()
{
	gl_Position = rotation * vec4(aPos, 1.0);
	color = aColor;
}

)";
*/