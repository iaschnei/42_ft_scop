#include "../include/include.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

extern int useTexture;


// ---------------- Callback ----------------
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)window; (void)scancode; (void)mods;

    if (action == GLFW_PRESS && key == GLFW_KEY_SPACE) {
        useTexture = !useTexture;
        printf("Swapping grey and texture: %s\n", useTexture ? "Texture" : "Greyscale");
        fflush(stdout);
    }
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

// ---------------- Texture loading ----------------
GLuint loadTexture(const char* path){
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 3);
    if(!data){
        printf("Failed to load texture: %s\n", path);
        return 0;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return tex;
}

// ---------------- GLFW + GLEW initialization ----------------
GLFWwindow* initWindow(int width, int height, const char* title) {
    if(!glfwInit()){
        printf("Failed to initialize GLFW\n");
        return nullptr;
    }

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

    glfwSetKeyCallback(win, keyCallback);

    return win;
}

// ---------------- Setup VAO/VBO ----------------
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


// ---------------- Main render loop ----------------
void renderLoop(GLFWwindow* win, GLuint vao, size_t vertexCount, GLuint program, GLuint texID,
                GLint mvpLoc, GLint modelLoc, GLint useTexLoc, GLint texLoc,
                Mat4 vp) {

    (void) program;

    float angle = 0.0f;

    while(!glfwWindowShouldClose(win)){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        angle += 0.3f*(3.14159f/180.0f);
        Mat4 model = Mat4::rotateY(angle*0.4f);
        Mat4 mvp = Mat4::multiply(vp, model);

        glUniformMatrix4fv(mvpLoc,1,GL_FALSE,mvp.m);
        glUniformMatrix4fv(modelLoc,1,GL_FALSE,model.m);

        glUniform1i(useTexLoc, useTexture ? 1 : 0);
        if(useTexture && texID != 0){
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texID);
            glUniform1i(texLoc,0);
        }

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES,0,vertexCount);

        glfwSwapBuffers(win);
        glfwPollEvents();
    }
}
