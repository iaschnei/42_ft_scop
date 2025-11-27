#ifndef INCLUDE_HPP
# define INCLUDE_HPP

#include "parsing.hpp"
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <string>
#include <limits>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Mat4.hpp"

void generateNormals(Mesh &mesh);
void computeCenterScale(const Mesh &mesh, float &cx, float &cy, float &cz, float &scale);
std::vector<float> interleaveMesh(const Mesh &mesh, float cx, float cy, float cz, float scale);

GLuint createProgram(const char *vs, const char *fs);
GLuint loadTexture(const char* path);
GLFWwindow* initWindow(int width, int height, const char* title);
void setupMeshBuffers(const std::vector<float> &interleaved, GLuint &vao, GLuint &vbo);
void renderLoop(GLFWwindow* win, GLuint vao, size_t vertexCount, GLuint program, GLuint texID,
                GLint mvpLoc, GLint modelLoc, GLint useTexLoc, GLint texLoc,
                Mat4 vp);

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

extern const char* vertexShaderSrc;
extern const char* fragmentShaderSrc;


#endif