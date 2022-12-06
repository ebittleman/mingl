#version 330
uniform mat4 MVP;
uniform float time;

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vCol;
layout(location = 2) in vec2 vUV;

out vec4 interp;
out vec2 uv;

#define TAU 6.28318530718

#define RED 1,0,0
#define BLACK 0,0,0
#define WHITE 1,1,1

void main() {
    // vec3 color = vPos.xxx;
    // vec3 color = vec3(vUV.x, vUV.x, vUV.x);
    // vec3 color = inverseMix(-0.3, 0.3, vPos.x);
    // vec3 color = mix(vec3(BLACK), vec3(WHITE), vPos.xxx);

    gl_Position = MVP * vec4(vPos, 1.0);
    interp = vec4(vCol, time);
    uv = vUV;
    // interp = vec4(vCol, time);
    // color = vec3(cos(time * vCol.r * TAU), 0, 0);
    // color = vec3(cos(time * TAU), 0, 0);
    // color = vec3(mix(0, vCol.r, 1.0), 0, 0);
    // color = vec3(mix(0, vCol.r, 1.0), 0, 0);
}
