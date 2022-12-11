#include <stddef.h>

#include <callback.h>

#include "types.h"

typedef void(line_callback_t)(slice *, char *);

void parse_uv_line(slice *normal_data, char *line);
void parse_face_line(slice *num_buffer, char *line);
void parse_vec3_line(slice *normal_data, char *line);

void handle_vertex_line(slice *vertex_data, char *line);
void handle_normal_line(slice *normal_data, char *line);
void handle_uv_line(slice *uv_data, char *line);
void handle_face_line(slice *element_data, char *line);

int line_reader(
    const char *filename,
    line_callback_t *callbacks[COUNT_BUFFERS],
    slice *buffers[COUNT_BUFFERS]);

void faces_to_elements(slice *elements, size_t n, size_t face_data[]);

void load_obj_file(
    const char *obj_file_name,
    slice buffers[COUNT_BUFFERS],
    float bounds[6]);
