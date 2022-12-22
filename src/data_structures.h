#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <stddef.h>
#include "linmath.h"

#define DEFAULT_SLICE_CAP 10
typedef struct _slice
{
    size_t len;
    size_t cap;
    size_t size;

    void *data;
} slice;

slice new_slice(size_t size);
void expand_slice(slice *s);
void extend_slice(slice *s, int n, void *item);
void *get_slice_item(slice *s, int i);
void reset_slice(slice *s, int size, int len, int cap);

void append_slice(slice *s, void *item);
void append_slice_size_t(slice *s, size_t value);

void debug_vec3(vec3 v);
void debug_mat(mat4x4 m);

#endif
