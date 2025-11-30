#include "../include/include.hpp"

bool useTexture = false;    // false = one color per face (default), true = texture
GLuint texID = 0;           // texture ID

int main(int argc, char** argv) {
    if(argc < 2){
        printf("Usage: %s model.obj\n", argv[0]);
        return -1;
    }
    std::string objPath = argv[1];

    GLFWwindow* win = initWindow(800, 600, "ft_scop-iaschnei");
    if(!win) return -1;

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

    // Calculate the scale of the object so it displays in the center
    float cx, cy, cz, scale;
    computeCenterScale(mesh, cx, cy, cz, scale);

    // Store all data for each vertex in succession in memory
    std::vector<float> interleaved = interleaveMesh(mesh, cx, cy, cz, scale);

    // Setup Vertex Array Buffer (VAO) and Vertex Buffer Object (VBO)
    // VAO = stores data to tell the GPU how to read the VBO (how to display the mesh)
    // VBO = stores technical data about a vertex (position, normals, color etc..)
    GLuint vao, vbo;
    setupMeshBuffers(interleaved, vao, vbo);

    // Load our shaders and combine them in a program that OpenGL can use
    GLuint program = createProgram(vertexShaderSrc, fragmentShaderSrc);
    glUseProgram(program);

    // Get location of various useful variables in our shader program so we can use them
    GLint mvpLoc = glGetUniformLocation(program,"MVP");
    GLint modelLoc = glGetUniformLocation(program,"Model");
    GLint useTexLoc = glGetUniformLocation(program,"useTexture");
    GLint texLoc = glGetUniformLocation(program,"tex");
    GLint texScaleLoc = glGetUniformLocation(program, "textureScale");

    // Load our texture and scale it 
    texID = loadTexture("ressources/texture.png");
    glUniform1f(texScaleLoc, 5.0f);

    // Setup matrices (more details in Mat4 file)
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
