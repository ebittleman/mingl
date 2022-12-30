#version 460

uniform mat4 mvp_matrix;
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;
uniform mat3 normal_matrix;
uniform vec3 view_position;
uniform float time;

layout(location = 0) in vec3 position;

void main() {
    gl_Position = mvp_matrix * vec4(position, 1.0);
}