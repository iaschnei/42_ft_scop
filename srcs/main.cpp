#include <GL/glew.h>

#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <GL/gl.h>
#include <cstdio>
#include <cstdlib>

#include "./Mat4.hpp"
#include "./include.hpp"

const char *vertexShaderSrc = R"(
#version 330 core
layout(location = 0) in vec3 position;
uniform mat4 MVP;
void main() {
    gl_Position = MVP * vec4(position, 1.0);
}
)";

const char *fragmentShaderSrc = R"(
#version 330 core
out vec3 color;
void main() {
    color = vec3(1, 0, 0); // red
}
)";



int main(void) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* win = glfwCreateWindow(800, 600, "Manual MVP", NULL, NULL);
    glfwMakeContextCurrent(win);

    glewExperimental = GL_TRUE;
    glewInit();

    glClearColor(0, 0, 0.4f, 1);

    // Triangle
    float verts[] = {
        -1, -1, 0,
         1, -1, 0,
         0,  1, 0
    };

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    GLuint program = createProgram(vertexShaderSrc, fragmentShaderSrc);
    glUseProgram(program);

    GLint mvpLoc = glGetUniformLocation(program, "MVP");

    // Build matrices manually
    Mat4 proj = Mat4::perspective(3.14159f/4.0f, 800.0f/600.0f, 0.1f, 100.0f);
    Mat4 view = Mat4::lookAt(4,3,3, 0,0,0, 0,1,0);
    Mat4 model = Mat4::identity();
    Mat4 vp = Mat4::multiply(proj, view);
    Mat4 mvp = Mat4::multiply(vp, model);

    while (!glfwWindowShouldClose(win)) {
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp.m);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glDeleteProgram(program);
    glfwTerminate();
    return 0;
}
