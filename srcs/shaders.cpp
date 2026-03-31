#include "../include/include.hpp"

// Handles each vertex's atribute (pos, normals, uv)
const char *vertexShaderSrc = R"(
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord; // = UV

uniform mat4 MVP;

out vec3 vNormal;
out vec3 vWorldPos;
out vec3 vColor;

void main()
{
    gl_Position = MVP * vec4(position, 1.0);
    int triIndex = gl_VertexID / 3;
    vColor = vec3(mod(triIndex*0.37,1.0), mod(triIndex*0.91,1.0), mod(triIndex*0.53,1.0));
    vNormal = normalize(normal);
    vWorldPos =  position;
}
)";

// Handles the color of each pixel on the screen
const char *fragmentShaderSrc = R"(
#version 330 core

// input from the vertex shader
in vec3 vNormal;
in vec3 vWorldPos;
in vec3 vColor;

// Output final color of the pixel
out vec4 FragColor;

uniform sampler2D tex;
uniform bool useTexture;

uniform float textureTiling;

void main()
{
    if(useTexture)
    {
        vec3 weights = pow(abs(vNormal), vec3(8.0));
        weights /= (weights.x + weights.y + weights.z);

        vec2 uvX = vWorldPos.zy * textureTiling;
        vec2 uvY = vWorldPos.xz * textureTiling;
        vec2 uvZ = vWorldPos.xy * textureTiling;

        vec4 colX = texture(tex, uvX);
        vec4 colY = texture(tex, uvY);
        vec4 colZ = texture(tex, uvZ);

        FragColor = colX * weights.x + colY * weights.y + colZ * weights.z;
    }
    else
    {
        // One color per face
        FragColor = vec4(vColor, 1.0);
    }
}


)";

// Compile the source code of a shader
GLuint compileShader(const char *source, GLenum type) {
    GLuint id = glCreateShader(type);
    glShaderSource(id, 1, &source, NULL);
    glCompileShader(id);

    GLint success = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[1024];
        glGetShaderInfoLog(id, 1024, NULL, log);
        fprintf(stderr, "Shader compile error:\n%s\n", log);
        exit(1);
    }
    return id;
}

// Combine the vertex shader and the fragment shader together into an OpenGL "program"
GLuint createProgram(const char *vs, const char *fs) {
    GLuint v = compileShader(vs, GL_VERTEX_SHADER);
    GLuint f = compileShader(fs, GL_FRAGMENT_SHADER);

    GLuint p = glCreateProgram();
    glAttachShader(p, v);
    glAttachShader(p, f);
    glLinkProgram(p);

    GLint success;
    glGetProgramiv(p, GL_LINK_STATUS, &success);
    if (!success) {
        char log[1024];
        glGetProgramInfoLog(p, 1024, NULL, log);
        fprintf(stderr, "Program link error:\n%s\n", log);
        exit(1);
    }

    glDeleteShader(v);
    glDeleteShader(f);
    return p;
}
