#include <stddef.h>

#include <callback.h>

#define DEFAULT_SLICE_CAP 10

struct Vector4
{
    float x, y, z, w;
};

struct Vector3
{
    float x, y, z;
};

struct Face
{
    int n;
    int *indexes;
};

struct slice
{
    size_t len;
    size_t cap;
    size_t size;

    void *data;
};

typedef void(line_callback_t)(char *);

struct slice new_slice(size_t size);
void expand_slice(struct slice *s);
void append_slice(struct slice *s, void *item);
void *get_slice_item(struct slice *s, int i);

void parse_uv_line(struct slice *normal_data, char *line);
void parse_face_line(struct slice *num_buffer, char *line);
void parse_vec3_line(struct slice *normal_data, char *line);

struct slice faces_to_elements(struct slice faces_slice);

void handle_vertex_line(struct slice *vertices, va_alist);
void handle_normal_line(struct slice *normal_data, va_alist list);
void handle_uv_line(struct slice *uv_data, va_alist list);
void handle_face_line(struct slice *face_data, va_alist list);

int line_reader(
    const char *filename,
    line_callback_t *callback,
    line_callback_t *callback2,
    line_callback_t *callback3,
    line_callback_t *callback4);
void load_obj_file(
    const char *obj_file_name, struct slice buffers[]);
