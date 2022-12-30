#ifndef _ENTITIES_H
#define _ENTITIES_H

#include "data_structures.h"
#include "types.h"
#include "linmath.h"
#include "shaders/shaders.h"

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

DEFINE_SLICE_TYPE(vertex);

typedef vertex triangle[3];

DEFINE_SLICE_TYPE(triangle);

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
    unsigned int vao;
} mesh;

DEFINE_SLICE_TYPE(mesh);
typedef size_t (*mesh_factory_t)(mesh **);

typedef struct _model
{
    slice meshes;
    float bounds[6];

    material *material;
} model;

DEFINE_SLICE_TYPE(model);
typedef size_t (*model_factory_t)(model **);

typedef struct _scene
{
    slice models;
    mat4x4 position;
    mat4x4 current_position;

    void *parameters;

    void (*init)(struct _scene *, mesh_factory_t, model_factory_t);
    void (*update)(struct _scene *, float, float);
} scene;

DEFINE_SLICE_TYPE(scene);
typedef size_t (*scene_factory_t)(scene **);

#define INITIALIZE_ENTITY_STORAGE(ns)                                             \
    static slice ns##_meshes, ns##_models, ns##_scenes;                           \
                                                                                  \
    size_t ns##_mesh_factory(mesh **new_mesh)                                     \
    {                                                                             \
        mesh empty_mesh = {0};                                                    \
        append_slice(&ns##_meshes, &empty_mesh);                                  \
        if (new_mesh != NULL)                                                     \
        {                                                                         \
            *new_mesh = get_slice_item(&ns##_meshes, ns##_meshes.len - 1);        \
        }                                                                         \
                                                                                  \
        return ns##_meshes.len - 1;                                               \
    }                                                                             \
                                                                                  \
    size_t ns##_model_factory(model **new_model)                                  \
    {                                                                             \
        model empty_model = {0};                                                  \
        append_slice(&ns##_models, &empty_model);                                 \
        model *current_model = get_slice_item(&ns##_models, ns##_models.len - 1); \
        current_model->meshes = new_slice(sizeof(mesh *));                        \
        if (new_model != NULL)                                                    \
        {                                                                         \
            *new_model = current_model;                                           \
        }                                                                         \
        return ns##_models.len - 1;                                               \
    }                                                                             \
                                                                                  \
    size_t ns##_scene_factory(scene **new_scene)                                  \
    {                                                                             \
        scene empty_scene = {0};                                                  \
        append_slice(&ns##_scenes, &empty_scene);                                 \
        scene *current_scene = get_slice_item(&ns##_scenes, ns##_scenes.len - 1); \
        current_scene->models = new_slice(sizeof(model *));                       \
        if (new_scene != NULL)                                                    \
        {                                                                         \
            *new_scene = current_scene;                                           \
        }                                                                         \
        return ns##_scenes.len - 1;                                               \
    }                                                                             \
                                                                                  \
    void ns##_create_entity_tables()                                              \
    {                                                                             \
        ns##_meshes = new_slice(sizeof(mesh));                                    \
        ns##_models = new_slice(sizeof(model));                                   \
        ns##_scenes = new_slice(sizeof(scene));                                   \
    }

#endif
