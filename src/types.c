#include "types.h"

const int BUFFER_SIZES[COUNT_BUFFERS] = {
    3, // VERTS
    2, // UVS
    3, // NORMALS
    3, // COLORS
    3  // ELEMENTS
};

const char *IN_NAMES[ELEMENTS] = {
    "position", // VERTS
    "uv",       // UVS
    "normal",   // NORMALS
    "color"     // COLORS
};
