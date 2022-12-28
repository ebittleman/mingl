#ifndef _ENTITIES_H
#define _ENTITIES_H

#include "data_structures.h"
#include "types.h"
#include "linmath.h"
#include "shaders.h"

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
    slice *meshes_table;
    slice meshes_idx;
    float bounds[6];
} model;

DEFINE_SLICE_TYPE(model);
typedef size_t (*model_factory_t)(model **);

typedef struct _scene
{
    slice *meshes_table;
    slice *models_table;

    slice models_idx;
    mat4x4 position;
    mat4x4 current_position;

    shader *shader;
    void *parameters;

    void (*init)(struct _scene *);
    void (*update)(struct _scene *, float, float);
    void (*draw)(struct _scene, shader);
} scene;

DEFINE_SLICE_TYPE(scene);
typedef size_t (*scene_factory_t)(scene **);

size_t mesh_factory(mesh **new_mesh);
size_t model_factory(model **new_model);
size_t scene_factory(scene **new_scene);

#define INITIALIZE_ENTITY_STORAGE()                                              \
    static slice shaders, meshes, models, scenes;                                \
                                                                                 \
    size_t mesh_factory(mesh **new_mesh)                                         \
    {                                                                            \
        mesh empty_mesh = {0};                                                   \
        append_slice(&meshes, &empty_mesh);                                      \
        if (new_mesh != NULL)                                                    \
        {                                                                        \
            *new_mesh = (mesh *)get_slice_item(&meshes, meshes.len - 1);         \
        }                                                                        \
                                                                                 \
        return meshes.len - 1;                                                   \
    }                                                                            \
                                                                                 \
    size_t model_factory(model **new_model)                                      \
    {                                                                            \
        model empty_model = {&meshes, 0, 0};                                     \
                                                                                 \
        append_slice(&models, &empty_model);                                     \
        model *current_model = (model *)get_slice_item(&models, models.len - 1); \
        current_model->meshes_idx = new_slice(sizeof(size_t));                   \
        if (new_model != NULL)                                                   \
        {                                                                        \
            *new_model = current_model;                                          \
        }                                                                        \
        return models.len - 1;                                                   \
    }                                                                            \
                                                                                 \
    size_t scene_factory(scene **new_scene)                                      \
    {                                                                            \
        scene empty_scene = {                                                    \
            &meshes, &models,                                                    \
            0, 0, 0,                                                             \
            0, 0,                                                                \
            0, 0, 0};                                                            \
                                                                                 \
        append_slice(&scenes, &empty_scene);                                     \
        scene *current_scene = (scene *)get_slice_item(&scenes, scenes.len - 1); \
        current_scene->models_idx = new_slice(sizeof(size_t));                   \
        if (new_scene != NULL)                                                   \
        {                                                                        \
            *new_scene = current_scene;                                          \
        }                                                                        \
        return scenes.len - 1;                                                   \
    }                                                                            \
                                                                                 \
    void create_entity_tables()                                                  \
    {                                                                            \
        meshes = new_slice(sizeof(mesh));                                        \
        models = new_slice(sizeof(model));                                       \
        scenes = new_slice(sizeof(scene));                                       \
    }

#endif
