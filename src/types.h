#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>

#include "linmath.h"

#define TAU 6.28318530718

typedef const char *string;

typedef struct _light
{
    vec3 position;
    vec3 color;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} light;

static void debug_vec3(vec3 v)
{
    printf("x: %f, y: %f, z: %f\n", v[0], v[1], v[2]);
}
static void debug_vec4(vec4 v)
{
    printf("x: %f, y: %f, z: %f, w: %f\n", v[0], v[1], v[2], v[3]);
}

static void debug_mat(mat4x4 m)
{
    printf("%f %f %f %f\n", m[0][0], m[0][1], m[0][2], m[0][3]);
    printf("%f %f %f %f\n", m[1][0], m[1][1], m[1][2], m[1][3]);
    printf("%f %f %f %f\n", m[2][0], m[2][1], m[2][2], m[2][3]);
    printf("%f %f %f %f\n", m[3][0], m[3][1], m[3][2], m[3][3]);
}

static void debug_bounds(float bounds[6])
{
    printf("x:%f = %f - %f\n", bounds[3] - bounds[0], bounds[3], bounds[0]);
    printf("y:%f = %f - %f\n", bounds[4] - bounds[1], bounds[4], bounds[1]);
    printf("z:%f = %f - %f\n", bounds[5] - bounds[2], bounds[5], bounds[2]);
}

#define RADS 0.01745329251994329576923690768489
static float radians(float degrees)
{
    return degrees * RADS;
}

#endif
