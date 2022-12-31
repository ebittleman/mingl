#version 460

uniform mat4 mvp_matrix;
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;
uniform mat3 normal_matrix;
uniform vec3 view_position;
uniform float time;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

out vec2 uvInterp;

void main() {
    gl_Position = mvp_matrix * vec4(position, 1.0);
    uvInterp = uv;
}
