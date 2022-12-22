#ifndef _ENTITIES_H
#define _ENTITIES_H

#include "data_structures.h"
#include "types.h"
#include "linmath.h"

#define MAX_BONE_INFLUENCE 4

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

typedef struct _vertex
{
    vec3 position;
    vec3 normal;
    vec2 uv;
    vec3 tangent;
    vec3 bitangent;

    int bones[MAX_BONE_INFLUENCE];
    int bone_weight[MAX_BONE_INFLUENCE];
} vertex;

typedef struct _texture
{
    unsigned int id;
    const char *type;
} texture;

typedef struct _mesh
{
    slice vertices;
    slice textures;
    float bounds[6];
    Program *shader;
} mesh;

typedef struct _model
{
    slice meshes;
    slice vaos;
    mat4x4 position;
    mat4x4 current_position;
    float bounds[6];
} model;

#endif
