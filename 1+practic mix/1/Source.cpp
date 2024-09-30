#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using glm::mat4;

#define M_PI 3.14159265358979323846

const int NUM_SEGMENTS = 100; // Количество сегментов для круга

// Функция для генерации круга
void createCircle(float* vertices, float radius, float centerX, float centerY, int offset) {
    for (int i = 0; i <= NUM_SEGMENTS; ++i) {
        float theta = 2.0f * M_PI * float(i) / float(NUM_SEGMENTS);
        float x = radius * cosf(theta);
        float y = radius * sinf(theta);

        vertices[(i + offset) * 3] = centerX + x;
        vertices[(i + offset) * 3 + 1] = centerY + y;
        vertices[(i + offset) * 3 + 2] = 0.0f;
    }
}

// Функция для создания треугольника
void createTriangle(float* vertices, int offset) {
    vertices[offset * 3] = -0.5f;     // Левая вершина
    vertices[offset * 3 + 1] = 0.1f;  // Смещение вверх
    vertices[offset * 3 + 2] = 0.0f;

    vertices[(offset + 1) * 3] = 0.5f;  // Правая вершина
    vertices[(offset + 1) * 3 + 1] = 0.1f;  // Смещение вверх
    vertices[(offset + 1) * 3 + 2] = 0.0f;

    vertices[(offset + 2) * 3] = 0.0f;  // Нижняя вершина
    vertices[(offset + 2) * 3 + 1] = -0.6f;  // Смещение вверх
    vertices[(offset + 2) * 3 + 2] = 0.0f;
}

// Функция для создания текстурных координат для круга
void createTextureCoords(float* texture_coords, int offset, float x_offset, float y_offset) {
    for (int i = 0; i <= NUM_SEGMENTS; ++i) {
        texture_coords[(i + offset) * 2] = x_offset + 0.5f * cosf(2.0f * M_PI * float(i) / float(NUM_SEGMENTS));
        texture_coords[(i + offset) * 2 + 1] = y_offset + 0.5f * sinf(2.0f * M_PI * float(i) / float(NUM_SEGMENTS));
    }
}

void createTriangleTextureCoords(float* texture_coords, int offset) {
    texture_coords[offset * 2] = 0.0f;       // Левая вершина
    texture_coords[offset * 2 + 1] = 2.0f;   

    texture_coords[(offset + 1) * 2] = 2.0f; // Правая вершина
    texture_coords[(offset + 1) * 2 + 1] = 2.0f;   

    texture_coords[(offset + 2) * 2] = 1.0f; // Нижняя вершина
    texture_coords[(offset + 2) * 2 + 1] = 0.0f; 
}


int main() {
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }

    GLFWwindow* window = glfwCreateWindow(640, 640, "Heart Shape", NULL, NULL);
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

    // Генерация вершин для сердца (2 круга + 1 треугольник)
    const int numVertices = (NUM_SEGMENTS + 1) * 2 + 3;
    float heart_points[(NUM_SEGMENTS + 1) * 2 * 3 + 3 * 3]; // Координаты вершин
    createCircle(heart_points, 0.25f, -0.25f, 0.25f, 0);     // Левый круг
    createCircle(heart_points, 0.25f, 0.25f, 0.25f, NUM_SEGMENTS + 1); // Правый круг
    createTriangle(heart_points, (NUM_SEGMENTS + 1) * 2);     // Треугольник

    // Текстурные координаты для всех вершин
    float heart_texture_coords[(NUM_SEGMENTS + 1) * 2 * 2 + 3 * 2]; // Исправлено: массив достаточного размера
    createTextureCoords(heart_texture_coords, 0, -0.25f, 0.25f); // Левый круг
    createTextureCoords(heart_texture_coords, NUM_SEGMENTS + 1, 0.25f, 0.25f); // Правый круг
    createTriangleTextureCoords(heart_texture_coords, (NUM_SEGMENTS + 1) * 2); // Треугольник

    GLuint vbo[2];
    glGenBuffers(2, vbo);

    GLuint points_vbo = vbo[0];
    GLuint texture_coords_vbo = vbo[1];

    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(heart_points), heart_points, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, texture_coords_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(heart_texture_coords), heart_texture_coords, GL_STATIC_DRAW);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, texture_coords_vbo);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    const char* vertex_shader =
        "#version 330 core\n"
        "in vec3 vertex_position;"
        "in vec2 texture_coords;"
        "out vec2 tex_coords;"
        "uniform mat4 model;"
        "void main() {"
        "    tex_coords = vec2(texture_coords.x, 1.0 - texture_coords.y);"
        "    gl_Position = model * vec4(vertex_position, 1.0);"
        "}";

    const char* fragment_shader =
        "#version 330 core\n"
        "in vec2 tex_coords;"
        "out vec4 frag_colour;"
        "uniform sampler2D texture_sampler_circles;"
        "uniform sampler2D texture_sampler_triangle;"
        "uniform int use_triangle_texture;"
        "uniform int use_mixed_texture;"
        "void main() {"
        "    if (use_triangle_texture == 1) {"
        "        frag_colour = texture(texture_sampler_triangle, tex_coords);"
        "    } else if (use_mixed_texture == 1) {"
        "        frag_colour = mix(texture(texture_sampler_triangle, tex_coords), texture(texture_sampler_circles, tex_coords), 0.7);"
        "    } else {"
        "        frag_colour = texture(texture_sampler_circles, tex_coords);"
        "    }"
        "}";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);
    GLint success;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vs, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fs, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
    }

    GLuint shader_programme = glCreateProgram();
    glAttachShader(shader_programme, fs);
    glAttachShader(shader_programme, vs);

    glBindAttribLocation(shader_programme, 0, "vertex_position");
    glBindAttribLocation(shader_programme, 1, "texture_coords");

    glLinkProgram(shader_programme);
    glGetProgramiv(shader_programme, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shader_programme, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    }

    // Загрузка текстур
    int width1, height1, channels1;
    int width2, height2, channels2;
    unsigned char* image_circles = stbi_load("gost.jpg", &width1, &height1, &channels1, STBI_rgb);
    unsigned char* image_triangle = stbi_load("waffle.jpg", &width2, &height2, &channels2, STBI_rgb);

    if (!image_circles) {
        fprintf(stderr, "ERROR: could not load texture gost.jpg\n");
        return 1;
    }

    if (!image_triangle) {
        fprintf(stderr, "ERROR: could not load texture waffle.jpg\n");
        return 1;
    }

    GLuint texture_circles, texture_triangle;
    glGenTextures(1, &texture_circles);
    glBindTexture(GL_TEXTURE_2D, texture_circles);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGB, GL_UNSIGNED_BYTE, image_circles);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Линейная фильтрация с мипмапами
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Линейная фильтрация

    glGenTextures(1, &texture_triangle);
    glBindTexture(GL_TEXTURE_2D, texture_triangle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, image_triangle);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); // Ближайшая фильтрация с мипмапами
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // Ближайшая фильтрация

    stbi_image_free(image_circles);
    stbi_image_free(image_triangle);
    GLuint texture_sampler_circles_location = glGetUniformLocation(shader_programme, "texture_sampler_circles");
    GLuint texture_sampler_triangle_location = glGetUniformLocation(shader_programme, "texture_sampler_triangle");
    GLuint use_triangle_texture_location = glGetUniformLocation(shader_programme, "use_triangle_texture");
    GLuint use_mixed_texture_location = glGetUniformLocation(shader_programme, "use_mixed_texture");

    glUseProgram(shader_programme);
    glUniform1i(texture_sampler_circles_location, 0);
    glUniform1i(texture_sampler_triangle_location, 1);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model(1.0f);

        GLuint location = glGetUniformLocation(shader_programme, "model");
        if (location >= 0) {
            glUniformMatrix4fv(location, 1, GL_FALSE, &model[0][0]);
        }

        glUseProgram(shader_programme);
        glBindVertexArray(vao);

        // Рисование треугольника с миксацией двух текстур
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_circles);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_triangle);
        glUniform1i(use_triangle_texture_location, 0);
        glUniform1i(use_mixed_texture_location, 1);
        glDrawArrays(GL_TRIANGLES, (NUM_SEGMENTS + 1) * 2, 3); // Треугольник

        // Рисование левого круга с текстурой пломбира и линейной фильтрацией
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_circles);
        glUniform1i(use_triangle_texture_location, 0);
        glUniform1i(use_mixed_texture_location, 0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, NUM_SEGMENTS + 1); // Левый круг

        // Рисование правого круга с ближайшей фильтрацией
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); // Ближайшая фильтрация с мипмапами
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // Ближайшая фильтрация
        glUniform1i(use_triangle_texture_location, 0);
        glUniform1i(use_mixed_texture_location, 0);
        glDrawArrays(GL_TRIANGLE_FAN, NUM_SEGMENTS + 1, NUM_SEGMENTS + 1); // Правый круг

        // Восстановление линейной фильтрации для левого круга
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Линейная фильтрация с мипмапами
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Линейная фильтрация

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
