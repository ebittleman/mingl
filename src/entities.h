#ifndef _ENTITIES_H
#define _ENTITIES_H

#include "data_structures.h"
#include "linmath.h"

#define MAX_BONE_INFLUENCE 4

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
} mesh;

typedef struct _model{
    slice meshes;
} model;

#endif
