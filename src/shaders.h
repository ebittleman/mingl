#ifndef _SHADERS_H
#define _SHADERS_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include "types.h"

enum Uniforms
{
    U_MVP = 0,
    U_MODEL,
    U_VIEW,
    U_PROJECTION,
    U_CAM_POS,
    U_TIME,
    COUNT_UNIFORMS
};

static const char *UniformNames[] = {
    "mvp_matrix",
    "model_matrix",
    "view_matrix",
    "proj_matrix",
    "camera_pos",
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
