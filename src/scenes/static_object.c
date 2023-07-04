
#include <stdlib.h>

#include <cblas.h>

#include "geometry.h"
#include "scenes.h"

typedef struct _static_object_params
{
    int x;
    int count;
    bool cube_enabled;
    material *material;
    material *debug_material;
} static_object_params;

void init_static_geometry(scene *self)
{
    static_object_params *params = (static_object_params *)self->parameters;
    int x = params->x, count = params->count;

    model *first_model = *(model **)get_slice_item(&self->models, 0);
    float *bounds = first_model->bounds;

    mat4x4 S, T;
    float ranges[3] = {
        bounds[3] - bounds[0],
        bounds[4] - bounds[1],
        bounds[5] - bounds[2],
    };
    float max = ranges[0];
    for (int x = 1; x < 3; x++)
    {
        if (ranges[x] > max)
        {
            max = ranges[x];
        }
    }

    float scale = 1.0f / max;
    mat4x4_translate( // starts with identity matrix then sets translation
        self->position,
        ((ranges[0] / 2.0f - bounds[3]) * scale) + ((float)x * 2) - (float)count,
        (ranges[1] / 2.0f - bounds[4]) * scale,
        (ranges[2] / 2.0f - bounds[5]) * scale);

    mat4x4_identity(S);
    mat4x4_scale_aniso(S, S, scale, scale, scale);
    mat4x4_mul(self->position, self->position, S);

    // debug_mat(m);
    // debug_bounds(bounds);
    // debug_vec3(ranges);
}

void init_static_object(scene *self, mesh_factory_t mesh_factory, model_factory_t model_factory)
{
    static_object_params *params = (static_object_params *)self->parameters;
    model *first_model = *(model **)get_slice_item(&self->models, 0);

    first_model->material = params->material;
    float *bounds = first_model->bounds;

    if (params->cube_enabled)
    {
        model *cube_model;
        model_factory(&cube_model);
        cube_model->material = params->debug_material;

        mesh cube_mesh = cube(bounds);
        mesh *current_mesh;
        mesh_factory(&current_mesh);
        *current_mesh = cube_mesh;
        append_slice(&cube_model->meshes, &current_mesh);

        append_slice(&self->models, &cube_model);
    }

    init_static_geometry(self);
}

void mingl_rotate_Z(mat4x4 dst, const mat4x4 src, float angle)
{
    mat4x4 temp;
    float s = sinf(angle);
    float c = cosf(angle);
    mat4x4 R = {
        {c, 0.f, -s, 0.f},
        {0.f, 1.f, 0.f, 0.f},
        {s, 0.f, c, 0.f},
        {0.f, 0.f, 0.f, 1.f}};

    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 4, 4, 4, 1.0f,
                (float *)src, 4, (float *)R, 4, 0.0f, (float *)temp, 4);
    mat4x4_dup(dst, temp);
}

void update_static_object(scene *self, float dt, float time)
{
    static_object_params *params = (static_object_params *)self->parameters;

    mat4x4 S, R, tmp;
    mat4x4_identity(S);
    mat4x4_identity(R);

    // mat4x4_dup(self->current_position, self->position);
    // mat4x4_identity(destination_position);
    // mat4x4_translate_in_place(destination_position, .1f, .2f, .5f);

    // float scale = (sinf(time * TAU * .25) + 1.1) * ((params->x + params->count) / params->count);
    // mat4x4_scale_aniso(S, S, scale, scale, scale);

    // mat4x4_rotate_X(R, R, time * TAU * .1);
    // mat4x4_rotate_Y(R, R, time * TAU * .1);
    mingl_rotate_Z(R, R, time * TAU * .1);
    // mat4x4_rotate_Z(R, R, time * TAU * .1);

    // mat4x4_mul(self->current_position, self->position, S);
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 4, 4, 4, 1.0f,
                (float *)S, 4, (float *)self->position, 4, 0.0f, (float *)tmp, 4);
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 4, 4, 4, 1.0f,
                (float *)R, 4, (float *)tmp, 4, 0.0f, (float *)self->current_position, 4);
    // mat4x4_mul(self->current_position, tmp, R);

    // mat4x4_mul(self->current_position, self->current_position, R);
    // void cblas_sgemm(OPENBLAS_CONST enum CBLAS_ORDER Order,
    //                  OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA,
    //                  OPENBLAS_CONST enum CBLAS_TRANSPOSE TransB,
    //                  OPENBLAS_CONST blasint M,
    //                  OPENBLAS_CONST blasint N,
    //                  OPENBLAS_CONST blasint K,
    //                  OPENBLAS_CONST float alpha,
    //                  OPENBLAS_CONST float *A,
    //                  OPENBLAS_CONST blasint lda,
    //                  OPENBLAS_CONST float *B,
    //                  OPENBLAS_CONST blasint ldb,
    //                  OPENBLAS_CONST float beta,
    //                  float *C,
    //                  OPENBLAS_CONST blasint ldc);

    // debug_mat(destination_position);
}

void static_object(scene *scene, model **model, material *base_material,
                   material *debug_material, int x, int count,
                   bool display_bounds)
{

    static_object_params *params = malloc(sizeof(static_object_params));
    params->x = x;
    params->count = count;
    params->cube_enabled = display_bounds;
    params->material = base_material;
    params->debug_material = debug_material;

    scene->init = &init_static_object;
    scene->update = &update_static_object;
    scene->parameters = params;

    append_slice(&scene->models, model);
}
