#include "shaders.h"

const char *default_vert_file = "src/shaders/default/default.vert";
const char *default_frag_file = "src/shaders/default/default.frag";

shader default_shader()
{
    shader shader = {0};
    shader.uniforms = malloc(sizeof(GLint) * COUNT_UNIFORMS);
    shader.input_locations = malloc(sizeof(GLint) * VERTEX_PARAM_COUNT);
    load_mesh_shader(&shader, default_vert_file, default_frag_file);
    shader.draw = &simple_draw_shader;

    return shader;
}
