#version 330

uniform mat4 mvp_matrix;
uniform mat4 proj_matrix;
uniform mat4 view_matrix;

layout(location = 0) in vec3 position;

void main() {
    gl_Position = mvp_matrix * vec4(position, 1.0);
}