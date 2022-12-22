#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stdio.h>
#include <glad/gl.h>

#include "data_structures.h"
#include "linmath.h"

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

typedef struct _program
{
    GLuint id;
    const char *vert_file;
    const char *frag_file;
    GLint *uniforms;
    GLint *input_locations;
} Program;

#endif
