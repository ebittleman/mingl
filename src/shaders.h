#ifndef _SHADERS_H
#define _SHADERS_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include "linmath.h"
#include "types.h"

enum Uniforms
{
    U_MVP = 0,
    U_MODEL,
    U_VIEW,
    U_PROJECTION,
    U_NORMAL_MATRIX,
    U_VIEW_POSITION,
    U_MATERIAL_AMBIENT,
    U_MATERIAL_DIFFUSE,
    U_MATERIAL_SPECULAR,
    U_MATERIAL_SHININESS,
    U_LIGHT_POSITION,
    U_LIGHT_COLOR,
    U_LIGHT_AMBIENT,
    U_LIGHT_DIFFUSE,
    U_LIGHT_SPECULAR,
    U_TIME,
    COUNT_UNIFORMS
};

static const char *UniformNames[] = {
    "mvp_matrix",
    "model_matrix",
    "view_matrix",
    "proj_matrix",
    "normal_matrix",
    "view_position",
    "material.ambient",
    "material.diffuse",
    "material.specular",
    "material.shininess",
    "light.position",
    "light.color",
    "light.ambient",
    "light.diffuse",
    "light.specular",
    "time",
};

typedef struct _shader
{
    GLuint id;
    const char *vert_file;
    const char *frag_file;
    GLint *uniforms;
    GLint *input_locations;

    void (*draw)(struct _shader, void *parameters);
} shader;

GLuint create_shader_program_from_strings(const char *vertex_shader_str, const char *fragment_shader_str);
GLuint create_shader_program_from_files(const char *vertex_shader_filename, const char *fragment_shader_filename);
void reload_shader_program_from_files(GLuint *program, const char *vertex_shader_filename, const char *fragment_shader_filename);

#endif
