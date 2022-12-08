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
    U_MVP = 1,
    U_MODEL,
    U_VIEW,
    U_PROJECTION,
    U_TIME,
    COUNT_UNIFORMS
};

int BUFFER_SIZES[COUNT_BUFFERS] = {
    3,
    2,
    3,
    3,
    3};

const char *IN_NAMES[ELEMENTS] = {
    "position",
    "uv",
    "normal",
    "color"};
