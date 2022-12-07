#version 330

uniform mat4 mvp_matrix;
uniform mat4 proj_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;
uniform float time;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

out vec4 colorInterp;
out vec2 uvInterp;

#define TAU 6.28318530718

void main() {
    float scale = 1.0f;
    vec3 translate_by = vec3(0, 0, 0);
    vec3 pos = (position * scale) + translate_by;

    gl_Position = mvp_matrix * vec4(pos, 1.0);
    colorInterp = vec4(normal, 0.0f);
    uvInterp = uv;
    // gl_Position = proj_matrix * view_matrix * model_matrix * vec4(pos, 1.0);
}
