#include "../include/include.hpp"

// Image loading library for the texture
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

extern int useTexture; // from main.cpp

// Stores an object position
// x, y and z can be modified to change the rendering
struct Transform {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

// Listens for any key we press
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void) mods;
    (void) scancode;

    // Ignore irrelevant callbacks (key releases for example)
    if(action != GLFW_PRESS && action != GLFW_REPEAT) return;

    // Retrieve the object's position so we can modify it
    Transform* transform = static_cast<Transform*>(glfwGetWindowUserPointer(window));
    if (!transform) return;

    float step = 0.1f;
    switch(key){
        case GLFW_KEY_SPACE:
            useTexture = !useTexture;
            printf("Swapping default and texture: %s\n", useTexture ? "Texture" : "Default");
            fflush(stdout);
            break;
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        case GLFW_KEY_LEFT:  transform->x -= step; break;
        case GLFW_KEY_RIGHT: transform->x += step; break;
        case GLFW_KEY_UP:    transform->y += step; break;
        case GLFW_KEY_DOWN:  transform->y -= step; break;
    }
}

// Different callback function for mouse scrolling
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {

    (void) xoffset;

    // Same thing
    Transform* transform = static_cast<Transform*>(glfwGetWindowUserPointer(window));
    if (!transform) return;

    float step = 0.2f;
    transform->z += (float)yoffset * step;
}

// Load the texture using stb library
GLuint loadTexture(const char* path){
    GLuint tex;

    // Generate a texture instance in openGL
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    int width, height, nrChannels;

    // This actually puts the texture on the right side (don't ask me why)
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 3);
    if(!data){
        printf("Failed to load texture: %s\n", path);
        return 0;
    }

    // Upload the image to the GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Add the settings we want
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return tex;
}

// Initialise GLFW (used for window management), and GLEW (used as a wrapper over opengl)
GLFWwindow* initWindow(int width, int height, const char* title) {
    if(!glfwInit()){
        printf("Failed to initialize GLFW\n");
        return nullptr;
    }

    // Specify OpenGL version (3.3 core)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* win = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if(!win){
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(win);
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK){
        printf("Failed to initialize GLEW\n");
        return nullptr;
    }

    glClearColor(0,0,0.4f,1);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    return win;
}

// Initialise VAO and VBO for OpenGL (see main for details)
void setupMeshBuffers(const std::vector<float> &interleaved, GLuint &vao, GLuint &vbo) {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, interleaved.size()*sizeof(float), interleaved.data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // UV
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
}

// The main render loop, runs until the program is closed
void renderLoop(GLFWwindow* win, GLuint vao, size_t vertexCount, GLuint program, GLuint texID,
                GLint mvpLoc, GLint modelLoc, GLint useTexLoc, GLint texLoc,
                Mat4 vp) 
{

    (void) program;
    Transform objPos;
    glfwSetWindowUserPointer(win, &objPos);
    glfwSetKeyCallback(win, keyCallback);
    glfwSetScrollCallback(win, scrollCallback);

    float angle = 0.0f;

    while(!glfwWindowShouldClose(win)){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Defines how fast our object rotates
        angle += 0.3f*(3.14159f/180.0f);

        // Apply any translation or rotation to our object, in our case there is always a rotation
        Mat4 translation = Mat4::translate(objPos.x, objPos.y, objPos.z);
        Mat4 rotation = Mat4::rotateY(angle*2.0f);
        Mat4 model = Mat4::multiply(translation, rotation);
        Mat4 mvp = Mat4::multiply(vp, model);

        // Send our matrices to the GPU
        glUniformMatrix4fv(mvpLoc,1,GL_FALSE,mvp.m);
        glUniformMatrix4fv(modelLoc,1,GL_FALSE,model.m);

        // Handle texture display
        glUniform1i(useTexLoc, useTexture ? 1 : 0);
        if(useTexture && texID != 0){
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texID);
            glUniform1i(texLoc,0);
        }

        // Draw the mesh
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES,0,vertexCount);

        glfwSwapBuffers(win);
        glfwPollEvents();
    }
}
