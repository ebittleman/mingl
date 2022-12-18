#include <stddef.h>

#include <callback.h>

#include "types.h"

typedef void(line_callback_t)(slice *, char *);
typedef size_t size_t3[3];

void sscanf_face(char *line, size_t len, size_t vertex_count, size_t3 data[]);

void parse_uv_line(slice *normal_data, char *line);
void parse_face_line(slice *num_buffer, char *line);
void parse_face_line_old(slice *element_data, char *line);
void parse_vec3_line(slice *normal_data, char *line);

void handle_vertex_line(slice *vertex_data, char *line);
void handle_normal_line(slice *normal_data, char *line);
void handle_uv_line(slice *uv_data, char *line);
void handle_face_line(slice *element_data, char *line);

int line_reader(
    const char *filename,
    line_callback_t *callbacks[COUNT_BUFFERS],
    slice *buffers[COUNT_BUFFERS]);

void faces_to_elements(slice *elements, size_t n, size_t3 face_data[]);

void load_obj_file(
    const char *obj_file_name,
    slice buffers[COUNT_BUFFERS],
    float bounds[6]);

enum FaceMetadata
{
    FACE_VERTS = 0,
    FACE_UVS,
    FACE_NORMALS,
    COUNT_FACE_METADATA
};

typedef struct _obj_data
{
    float *vertices;
    float *uvs;
    float *normals;
    int *faces;
} obj_data;

// void expanded_vertices(obj_data obj);
// void expanded_uvs(obj_data obj);

// void expanded_normals(obj_data obj);
// void calculate_normals_per_vertex(obj_data obj);
// void calculate_normals_per_face(obj_data obj);
// void normals_from_face_data(obj_data obj);
