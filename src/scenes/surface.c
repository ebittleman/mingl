#include "scenes.h"

typedef struct _surface_params
{
    vec3 normal;
    vec3 position;
    float width;
    float height;
    material *material;
} surface_params;

// void init_surface(scene *self)
// {
//     surface_params *params = (surface_params *)self->parameters;
//     // vec3 normal = params->normal;
//     // vec3 position = params->position;
//     float width = params->width;
//     float height = params->height;

//     vec3 up = {0.0f, 1.0f, 0.0f};
//     vec3 right = {1.0f, 0.0f, 0.0f};
//     vec3 forward = {0.0f, 0.0f, 1.0f};

//     vec3 axis = {0.0f, 0.0f, 0.0f};
//     vec3_cross(axis, params->normal, up);
//     float angle = acosf(vec3_dot(params->normal, up));
//     mat4x4 R;
//     mat4x4_rotate(R, R, angle, axis);

//     vec3 axis2 = {0.0f, 0.0f, 0.0f};
//     vec3_cross(axis2, params->normal, right);
//     float angle2 = acosf(vec3_dot(params->normal, right));
//     mat4x4 R2;
//     mat4x4_rotate(R2, R2, angle2, axis2);

//     vec3 axis3 = {0.0f, 0.0f, 0.0f};
//     vec3_cross(axis3, params->normal, forward);
//     float angle3 = acosf(vec3_dot(params->normal, forward));
//     mat4x4 R3;
//     mat4x4_rotate(R3, R3, angle3, axis3);

//     mat4x4 T;
//     mat4x4_translate(T, params->position[0], params->position[1], params->position[2]);

//     mat4x4 S;
//     mat4x4_identity(S);
//     mat4x4_scale_aniso(S, S, width, height, 1.0f);

//     mat4x4_mul(self->position, T, R);
//     mat4x4_mul(self->position, self->position, R2);
//     mat4x4_mul(self->position, self->position, R3);
//     mat4x4_mul(self->position, self->position, S);
// }
