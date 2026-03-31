#include "../include/include.hpp"

bool useTexture = false;    // false = one color per face (default), true = texture
GLuint texID = 0;           // texture ID

int main(int argc, char** argv) {
    if(argc < 2){
        printf("Usage: %s model.obj [model2.obj ...]\n", argv[0]);
        return -1;
    }

    int objCount = argc - 1;
    if(objCount > 5){
        printf("Warning: max 5 objects supported, ignoring the rest\n");
        objCount = 5;
    }

    GLFWwindow* win = initWindow(800, 600, "ft_scop-iaschnei");
    if(!win) return -1;

    std::vector<SceneObject> objects;
    float spacing = 4.0f;

    for(int i = 0; i < objCount; i++){
        std::string objPath = argv[i + 1];

        Mesh mesh;
        if(!loadOBJ(objPath, mesh)){
            printf("Failed to load OBJ: %s\n", objPath.c_str());
            return -1;
        }
        printf("Loaded OBJ: %s (%zu vertices)\n", objPath.c_str(), mesh.vertices.size()/3);

        if(mesh.normals.empty()){
            printf("No normals found, generating normals for: %s\n", objPath.c_str());
            generateNormals(mesh);
        }

        // Calculate the scale of the object so it fits in our window
        float cx, cy, cz, scale;
        computeCenterScale(mesh, cx, cy, cz, scale);

        // Store all data for each vertex in succession in memory
        std::vector<float> interleaved = interleaveMesh(mesh, cx, cy, cz, scale);

        // Setup VAO and VBO for this object
        // VAO = Tells the GPU how to read data in VBO (what data is where)
        // VBO = Data stored in the GPU memory for all our meshes
        GLuint vao, vbo;
        setupMeshBuffers(interleaved, vao, vbo);

        // Make sure there is enough distance between objects
        float offsetX = i * spacing - (objCount - 1) * spacing / 2.0f;

        SceneObject obj;
        obj.vao         = vao;
        obj.vertexCount = mesh.vertices.size() / 3;
        obj.offsetX     = offsetX;
        objects.push_back(obj);
    }

    // Load our shaders and combine them in a program that OpenGL can use
    GLuint program = createProgram(vertexShaderSrc, fragmentShaderSrc);
    glUseProgram(program);

    // Make the texture repeat itself like tiles
    // We can change the float value of glUniform1f to change how often it repeats
    GLint tilingLoc = glGetUniformLocation(program, "textureTiling");
    glUniform1f(tilingLoc, 10.0f);

    // Get location of various useful variables in our shader program so we can use them
    GLint mvpLoc   = glGetUniformLocation(program, "MVP");
    GLint modelLoc = glGetUniformLocation(program, "Model");
    GLint useTexLoc = glGetUniformLocation(program, "useTexture");
    GLint texLoc   = glGetUniformLocation(program, "tex");

    // Load our texture
    texID = loadTexture("ressources/texture.png");

    // Setup matrices (more details in Mat4 file)
    // Push camera back further so all objects fit in view
    Mat4 proj = Mat4::perspective(3.14159f/4.0f, 800.0f/600.0f, 0.1f, 100.0f);
    float camDist = 4.0f + objCount * 2.0f;
    Mat4 view = Mat4::lookAt(camDist, camDist*0.6f, camDist, 0,0,0, 0,1,0);
    Mat4 vp = Mat4::multiply(proj, view);

    // Start render loop
    renderLoop(win, objects, program, texID, mvpLoc, modelLoc, useTexLoc, texLoc, vp);

    glDeleteProgram(program);
    glfwTerminate();
    return 0;
}