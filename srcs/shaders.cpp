#include "../include/include.hpp"

const char *vertexShaderSrc = R"(
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord; // only if UVs exist

uniform mat4 MVP;

out vec3 vNormal;
out vec2 vTexCoord;

void main()
{
    gl_Position = MVP * vec4(position, 1.0);
    vNormal = normalize(normal);      // for grayscale
    vTexCoord = texCoord;             // for texture
}

)";

const char *fragmentShaderSrc = R"(
#version 330 core

in vec3 vNormal;
in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D tex;
uniform bool useTexture;

void main()
{
    if(useTexture)
    {
        // Display texture color
        FragColor = texture(tex, vTexCoord);
    }
    else
    {
        // Simple grayscale from normals
        vec3 gray = normalize(vNormal) * 0.5 + 0.5; // map -1..1 to 0..1
        FragColor = vec4(gray, 1.0);
    }
}


)";


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

// Combine the vertex shader and the fragment shader together 
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
