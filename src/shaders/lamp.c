#include "shaders.h"

const char *lamp_vert_file = "src/shaders/lamp/lamp.vert";
const char *lamp_frag_file = "src/shaders/lamp/lamp.frag";

enum LampUniforms
{
    U_LAMP_LIGHT_POSITION = COUNT_UNIFORMS,
    U_LAMP_LIGHT_COLOR,
    U_LAMP_LIGHT_AMBIENT,
    U_LAMP_LIGHT_DIFFUSE,
    U_LAMP_LIGHT_SPECULAR,
    LAMP_UNIFORM_COUNT,
};

static const char *LampUniformNames[] = {
    "light.position",
    "light.color",
    "light.ambient",
    "light.diffuse",
    "light.specular",
};

static void draw_lamp_material(material material, shader shader)
{
    lamp_shader_params *mat = (lamp_shader_params *)material.parameters;
    if (mat == NULL)
    {
        return;
    }

    if (mat->light != NULL)
    {
        glUniform3fv(shader.uniforms[U_LAMP_LIGHT_POSITION], 1, mat->light->position);
        glUniform3fv(shader.uniforms[U_LAMP_LIGHT_COLOR], 1, mat->light->color);
        glUniform3fv(shader.uniforms[U_LAMP_LIGHT_AMBIENT], 1, mat->light->ambient);
        glUniform3fv(shader.uniforms[U_LAMP_LIGHT_DIFFUSE], 1, mat->light->diffuse);
        glUniform3fv(shader.uniforms[U_LAMP_LIGHT_SPECULAR], 1, mat->light->specular);
    }
}

shader lamp_shader()
{
    shader shader = {0};
    shader.uniforms = malloc(sizeof(GLint) * LAMP_UNIFORM_COUNT);
    shader.input_locations = malloc(sizeof(GLint) * VERTEX_PARAM_COUNT);
    load_mesh_shader(&shader, lamp_vert_file, lamp_frag_file);
    for (size_t i = COUNT_UNIFORMS; i < LAMP_UNIFORM_COUNT; i++)
    {
        const char *name = LampUniformNames[i - COUNT_UNIFORMS];
        shader.uniforms[i] = glGetUniformLocation(shader.id, name);
    }
    shader.draw = &simple_draw_shader;

    return shader;
}

material lamp_material(shader *shader, lamp_shader_params *params)
{
    material material = {
        shader,
        params,
        &draw_lamp_material};

    return material;
}
