#version 330

uniform mat4 projection_matrix;
uniform float time;

layout(location = 0) in vec3 position;

void main() {
    gl_Position = projection_matrix * vec4(position, 1.0);
}
