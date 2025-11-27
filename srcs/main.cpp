#include "../include/include.hpp"


// ---------------- Globals ----------------
bool useTexture = false;   // toggle between greyscale and texture
GLuint texID = 0;          // texture ID

// ---------------- Main ----------------
int main(int argc, char** argv) {
    if(argc < 2){
        printf("Usage: %s model.obj\n", argv[0]);
        return -1;
    }
    std::string objPath = argv[1];

    // Initialize window
    GLFWwindow* win = initWindow(800, 600, "ft_scop-iaschnei");
    if(!win) return -1;

    // Load mesh
    Mesh mesh;
    if(!loadOBJ(objPath, mesh)){
        printf("Failed to load OBJ: %s\n", objPath.c_str());
        return -1;
    }
    printf("Loaded OBJ: %s (%zu vertices)\n", objPath.c_str(), mesh.vertices.size()/3);

    if(mesh.normals.empty()){
        printf("No normals found. Generating normals...\n");
        generateNormals(mesh);
    }

    // Compute center and scale
    float cx, cy, cz, scale;
    computeCenterScale(mesh, cx, cy, cz, scale);

    // Interleave vertex data
    std::vector<float> interleaved = interleaveMesh(mesh, cx, cy, cz, scale);

    // Setup VAO/VBO
    GLuint vao, vbo;
    setupMeshBuffers(interleaved, vao, vbo);

    // Load shaders
    GLuint program = createProgram(vertexShaderSrc, fragmentShaderSrc);
    glUseProgram(program);

    GLint mvpLoc = glGetUniformLocation(program,"MVP");
    GLint modelLoc = glGetUniformLocation(program,"Model");
    GLint useTexLoc = glGetUniformLocation(program,"useTexture");
    GLint texLoc = glGetUniformLocation(program,"tex");
    GLint texScaleLoc = glGetUniformLocation(program, "textureScale");

    // Load texture
    texID = loadTexture("ressources/texture.png");
    glUniform1f(texScaleLoc, 5.0f);

    // Setup matrices
    Mat4 proj = Mat4::perspective(3.14159f/4.0f, 800.0f/600.0f, 0.1f, 100.0f);
    float camDist = 4.0f;
    Mat4 view = Mat4::lookAt(camDist, camDist*0.6f, camDist, 0,0,0, 0,1,0);
    Mat4 vp = Mat4::multiply(proj, view);

    // Start render loop
    renderLoop(win, vao, mesh.vertices.size()/3, program, texID,
               mvpLoc, modelLoc, useTexLoc, texLoc, vp);

    glDeleteProgram(program);
    glfwTerminate();
    return 0;
}
