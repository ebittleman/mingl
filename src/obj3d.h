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

int read_vec3(const char *p, struct Vector3 *out);
int line_reader(const char *filename, line_callback_t *callback, line_callback_t *callback2);
void append_vertex(struct slice *vertices, va_alist);
void read_face_line(struct slice *face_data, va_alist list);
void parse_face_line(struct slice *num_buffer, char *line);
