#include <stddef.h>

#include <callback.h>

#include "types.h"

#define DEFAULT_SLICE_CAP 10

typedef void(line_callback_t)(char *);

slice new_slice(size_t size);
void expand_slice(slice *s);
void append_slice(slice *s, void *item);
void extend_slice(slice *s, int n, void *item);
void *get_slice_item(slice *s, int i);
void reset_slice(slice *s, int size, int len, int cap);

void parse_uv_line(slice *normal_data, char *line);
void parse_face_line(slice *num_buffer, char *line);
void parse_vec3_line(slice *normal_data, char *line);

slice faces_to_elements(slice faces_slice);

void handle_vertex_line(slice *vertices, va_alist);
void handle_normal_line(slice *normal_data, va_alist list);
void handle_uv_line(slice *uv_data, va_alist list);
void handle_face_line(slice *face_data, va_alist list);

int line_reader(
    const char *filename,
    line_callback_t *callback,
    line_callback_t *callback2,
    line_callback_t *callback3,
    line_callback_t *callback4);

void load_obj_file(const char *obj_file_name, slice buffers[], float bounds[6]);
