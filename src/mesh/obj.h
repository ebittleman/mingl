#include <stdint.h>

typedef struct
{
    float x, y, z;
} vertex_t;

typedef struct
{
    float u, v;
} texcoord_t;

typedef struct
{
    float x, y, z;
} normal_t;

typedef struct
{
    size_t v, vt, vn;
} face_vertex_t;

typedef struct
{
    char *name;
    size_t num_vertices;
    vertex_t *vertices;
    size_t num_texcoords;
    texcoord_t *texcoords;
    size_t num_normals;
    normal_t *normals;
    size_t num_face_vertices;
    face_vertex_t *face_vertices;
} group_t;

group_t *handle_g(char *name);
void handle_v(float x, float y, float z);
void handle_vt(float u, float v);
void handle_vn(float x, float y, float z);
void handle_f_vertex(size_t v, size_t vt, size_t vn);
