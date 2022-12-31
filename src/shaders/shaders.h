#ifndef _SHADERS_H
#define _SHADERS_H

#include <stdlib.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include "linmath.h"
#include "opengl_util.h"
#include "types.h"

// default uniforms
enum Uniforms
{
    U_MVP = 0,
    U_MODEL,
    U_VIEW,
    U_PROJECTION,
    U_NORMAL_MATRIX,
    U_VIEW_POSITION,
    U_TIME,
    COUNT_UNIFORMS
};

static const char *UniformNames[] = {
    // default uniforms
    "mvp_matrix",
    "model_matrix",
    "view_matrix",
    "proj_matrix",
    "normal_matrix",
    "view_position",
    "time",
};

// vertex buffers/inputs
enum vertex_parameters
{
    VERTEX_POSITION = 0,
    VERTEX_NORMAL,
    VERTEX_UV,
    VERTEX_TANGENT,
    VERTEX_BITANGENT,
    VERTEX_BONE_IDS,
    VERTEX_BONE_WEIGHTS,
    VERTEX_PARAM_COUNT,
};

static const char *vertex_param_names[] = {
    "position",
    "normal",
    "uv",
    "tangent",
    "bitangent",
    "bones",
    "bone_weight",
};

typedef struct _shader
{
    GLuint id;
    const char *vert_file;
    const char *frag_file;
    GLint *uniforms;
    GLint *input_locations;

    void (*draw)(struct _shader);
} shader;

enum IlluminationMode
{
    ColorOnAmbientOff = 0,
    ColorOnAmbientOn,
    HighlightOn,
    ReflectionOnRayTraceOn,
    TransparencyGlassOnRayTraceOn,
    ReflectionFresnelOnRayTraceOn,
    TransparencyRefractionOnReflectionFresnelOffRayTraceOn,
    TransparencyRefractionOnReflectionFresnelOnRayTraceOn,
    ReflectionOnRayTraceOff,
    TransparencyGlassOnReflectionRayTraceOff,
    CastsShadowsOntoInvisibleSurfaces
};

typedef struct _material
{
    shader *shader;
    void *parameters;

    void (*draw)(struct _material, shader);

} material;

static void load_mesh_shader(shader *program, const char *vert_file, const char *frag_file)
{
    GLuint id = create_shader_program_from_files(vert_file, frag_file);
    program->id = id;
    program->vert_file = vert_file;
    program->frag_file = frag_file;

    for (size_t i = 0; i < COUNT_UNIFORMS; i++)
    {
        program->uniforms[i] = glGetUniformLocation(id, UniformNames[i]);
    }

    // buffer attributes
    for (int x = 0; x < VERTEX_PARAM_COUNT; x++)
    {
        program->input_locations[x] = glGetAttribLocation(id, vertex_param_names[x]);
    }
}

static void simple_draw_shader(shader shader)
{
    glEnable(GL_CULL_FACE); // cull face
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glPolygonMode(GL_FRONT, GL_FILL);
}

typedef struct _phong_material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;

    float alpha;
    vec3 transmission;

    float optical_density;

    unsigned int illum;

} phong_material;

typedef struct _phong_params
{
    phong_material *phong_material;
    light *light;
} phong_params;

shader phong_shader();
material new_phong_material(shader *shader, phong_params *params);
material new_debug_phong_material(shader *shader, phong_params *params);

typedef struct _lamp_shader_params
{
    phong_material *phong_material;
    light *light;
} lamp_shader_params;

shader lamp_shader();
material lamp_material(shader *shader, lamp_shader_params *params);

typedef struct _textured_shader_params
{
    GLuint texture_id;
} textured_shader_params;

shader textured_shader();
material textured_material(shader *shader, textured_shader_params *params);

#endif
