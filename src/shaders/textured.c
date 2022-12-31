#include "shaders.h"

const char *textured_vert_file = "src/shaders/textured/textured.vert";
const char *textured_frag_file = "src/shaders/textured/textured.frag";

enum TexturedUniforms
{
    U_TEXTURED_TEXTURE1 = COUNT_UNIFORMS,
    TEXTURED_UNIFORM_COUNT,
};

static const char *TexturedUniformNames[] = {
    "texture1",
};

static void draw_textured_material(material material, shader shader)
{
    textured_shader_params *mat = (textured_shader_params *)material.parameters;
    if (mat == NULL)
    {
        return;
    }

    glBindTexture(GL_TEXTURE_2D, mat->texture_id);
}

shader textured_shader()
{
    shader shader = {0};
    shader.uniforms = malloc(sizeof(GLint) * TEXTURED_UNIFORM_COUNT);
    shader.input_locations = malloc(sizeof(GLint) * VERTEX_PARAM_COUNT);
    load_mesh_shader(&shader, textured_vert_file, textured_frag_file);
    for (size_t i = COUNT_UNIFORMS; i < TEXTURED_UNIFORM_COUNT; i++)
    {
        const char *name = TexturedUniformNames[i - COUNT_UNIFORMS];
        shader.uniforms[i] = glGetUniformLocation(shader.id, name);
    }
    shader.draw = &simple_draw_shader;

    return shader;
}

material textured_material(shader *shader, textured_shader_params *params)
{
    material material = {
        shader,
        params,
        &draw_textured_material};

    return material;
}
