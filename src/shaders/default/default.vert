#version 330

uniform mat4 mvp_matrix;
uniform vec4 tint;

layout(location = 0) in vec3 position;

out vec4 color;

void main() {
    gl_Position = mvp_matrix * vec4(position, 1.0);
    color = tint;
}
