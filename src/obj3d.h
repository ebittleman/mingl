#include <stddef.h>

#include "types.h"
#include "linmath.h"
#include "entities.h"

typedef void(line_callback_t)(slice *, char *);
typedef size_t size_t3[3];

typedef struct _face_elements
{
    size_t position_idx, texture_idx, normal_idx;
} face_elements;

typedef face_elements face[3];

enum Buffers
{
    VERTS = 0,
    UVS,
    NORMALS,
    ELEMENTS,
    COUNT_BUFFERS
};

void sscanf_face(char *line, size_t len, size_t vertex_count, face_elements data[]);

void parse_vec2_line(slice *vec2_data, char *line);
void parse_vec3_line(slice *vec3_data, char *line);
void parse_face_line(slice *element_data, char *line);

void handle_vertex_line(slice *vertex_data, char *line);
void handle_normal_line(slice *normal_data, char *line);
void handle_uv_line(slice *uv_data, char *line);
void handle_face_line(slice *element_data, char *line);

int line_reader(
    const char *filename,
    line_callback_t *callbacks[COUNT_BUFFERS],
    slice *buffers[COUNT_BUFFERS]);

void faces_to_elements(slice *elements, size_t n, face_elements face_data[]);

void calculate_normal_per_vertex(slice buffer_slices[COUNT_BUFFERS]);
void load_mesh(const char *obj_file_name, mesh *mesh);
