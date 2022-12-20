#version 330

uniform mat4 mvp_matrix;
uniform mat4 proj_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;
uniform vec3 cam_pos;
uniform float time;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 color;

out vec4 colorInterp;
out vec2 uvInterp;
out vec3 fragPosInterp;
out vec3 normalInterp;
out float timInterp;

#define TAU 6.28318530718
#define RED vec4(1,0,0,1)

void main() {
    gl_Position = mvp_matrix * vec4(position, 1.0);

    colorInterp = vec4(0.6f, 0.6f, 0.6f, 2.0f);
    uvInterp = uv;
    timInterp = time;
    normalInterp = (view_matrix * model_matrix * vec4(normal, 0.0f)).xyz;
    fragPosInterp = vec3(model_matrix * vec4(position, 1.0));
}
