#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stdio.h>
#include <glad/gl.h>

#include "data_structures.h"
#include "linmath.h"

enum Buffers
{
    VERTS = 0,
    UVS,
    NORMALS,
    COLORS,
    ELEMENTS,
    COUNT_BUFFERS
};

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

const static int BUFFER_SIZES[COUNT_BUFFERS] = {
    3, // VERTS
    2, // UVS
    3, // NORMALS
    3, // COLORS
    3  // ELEMENTS
};

const static char *IN_NAMES[ELEMENTS] = {
    "position", // VERTS
    "uv",       // UVS
    "normal",   // NORMALS
    "color"     // COLORS
};

typedef struct _program
{
    GLuint id;
    const char *vert_file;
    const char *frag_file;
    GLint uniforms[COUNT_UNIFORMS];
    GLint input_locations[ELEMENTS];
} Program;

typedef struct _object
{
    Program *program;
    GLuint vertex_array;
    GLuint vertex_buffers[COUNT_BUFFERS];
    slice buffer_slices[COUNT_BUFFERS];
    bool buffer_disabled[COUNT_BUFFERS];
    size_t element_count;
    mat4x4 init_model;
    mat4x4 model;
} object;

#endif
