#version 330

uniform mat4 mvp_matrix;
uniform mat4 proj_matrix;
uniform mat4 view_matrix;
uniform mat3 normal_matrix;
uniform mat4 model_matrix;
uniform float time;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 bones;
layout(location = 6) in vec4 bone_weight;

out vec4 colorInterp;
out vec2 uvInterp;
out vec3 fragPosInterp;
out vec3 normalInterp;

#define TAU 6.28318530718
#define RED vec4(1,0,0,1)

void main() {
    gl_Position = mvp_matrix * vec4(position, 1.0);

    uvInterp = uv;
    // normalInterp = transpose(inverse(mat3(model_matrix))) * normal;
    normalInterp = normal_matrix * normal;
    // colorInterp = vec4(0.6f, 0.6f, 0.6f, 1.0f);
    colorInterp = vec4(normalize(normalInterp), 1.0f);

    fragPosInterp = vec3(model_matrix * vec4(position, 1.0));
}
