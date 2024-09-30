#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION

#include "shader_m.h"
#include "camera.h"
#include "model.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D scene", nullptr, 0);
	if (!window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return 1;
	}
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwMakeContextCurrent(window);
	glewInit();
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glEnable(GL_DEPTH_TEST);

	Shader model_shader("model_shader.vs", "model_shader.fs");
	Model castle("scene/castle.obj");
	Model sea("scene/sea.obj");
	Model sky("scene/sky.obj");

	glm::vec3 pointLightPositions[] = {
		glm::vec3(-2.0f,  0.5f,  0.5f),
		glm::vec3(-0.5f, 0.5f, 0.5f),
		glm::vec3(1.0f,  0.5f, 0.5f)
	};

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		model_shader.use();
		model_shader.setVec3("viewPos", camera.Position);

		model_shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		model_shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		model_shader.setVec3("dirLight.diffuse", 0.004f, 0.004f, 0.004f);

		float pointLightDiffuse = 0.3f;

		model_shader.setVec3("pointLights[0].position", pointLightPositions[0]);
		model_shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		model_shader.setVec3("pointLights[0].diffuse", pointLightDiffuse, pointLightDiffuse, pointLightDiffuse);
		model_shader.setFloat("pointLights[0].constant", 1.0f);
		model_shader.setFloat("pointLights[0].linear", 0.09f);
		model_shader.setFloat("pointLights[0].quadratic", 0.005f);

		model_shader.setVec3("pointLights[1].position", pointLightPositions[1]);
		model_shader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		model_shader.setVec3("pointLights[1].diffuse", pointLightDiffuse, pointLightDiffuse, pointLightDiffuse);
		model_shader.setFloat("pointLights[1].constant", 1.0f);
		model_shader.setFloat("pointLights[1].linear", 0.09f);
		model_shader.setFloat("pointLights[1].quadratic", 0.005f);

		model_shader.setVec3("pointLights[2].position", pointLightPositions[2]);
		model_shader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		model_shader.setVec3("pointLights[2].diffuse", pointLightDiffuse, pointLightDiffuse, pointLightDiffuse);
		model_shader.setFloat("pointLights[2].constant", 1.0f);
		model_shader.setFloat("pointLights[2].linear", 0.09f);
		model_shader.setFloat("pointLights[2].quadratic", 0.005f);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		model_shader.setMat4("projection", projection);
		model_shader.setMat4("view", view);

		glm::mat4 model1 = glm::mat4(1.0f);
		model1 = glm::translate(model1, glm::vec3(0.0f, 0.0f, 0.0f));
		model1 = glm::scale(model1, glm::vec3(8.0f, 8.0f, 8.0f));
		model_shader.setMat4("model", model1);
		castle.Draw(model_shader);

		glm::mat4 model2 = glm::mat4(1.0f);
		model2 = glm::scale(model2, glm::vec3(4.0f, 4.0f, 4.0f));
		model2 = glm::translate(model2, glm::vec3(-4.5f, 0.8f, 3.5f));
		model_shader.setMat4("model", model2);
		sea.Draw(model_shader);

		glm::mat4 model3 = glm::mat4(1.0f);
		model3 = glm::translate(model3, glm::vec3(0.0f, 0.0f, 0.0f));
		model3 = glm::scale(model3, glm::vec3(8.0f, 8.0f, 8.0f));
		model_shader.setMat4("model", model3);
		sky.Draw(model_shader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}