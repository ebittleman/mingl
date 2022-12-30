#include "shaders.h"

const char *directional_light_vert_file = "src/shaders/directional_light/directional_light.vert";
const char *directional_light_frag_file = "src/shaders/directional_light/directional_light.frag";

shader directional_light()
{
    shader shader = {0};
    shader.uniforms = malloc(sizeof(GLint) * COUNT_UNIFORMS);
    shader.input_locations = malloc(sizeof(GLint) * VERTEX_PARAM_COUNT);
    load_mesh_shader(&shader, directional_light_vert_file, directional_light_frag_file);
    shader.draw = &simple_draw_shader;

    return shader;
}
