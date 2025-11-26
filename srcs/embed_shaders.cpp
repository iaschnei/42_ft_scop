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
