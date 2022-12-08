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
    U_TIME,
    COUNT_UNIFORMS
};

extern const int BUFFER_SIZES[];
extern const char *IN_NAMES[];
