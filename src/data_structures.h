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

#define DEFINE_SLICE_TYPE(type)                                                               \
    typedef struct _##type##_slice                                                            \
    {                                                                                         \
        size_t len;                                                                           \
        const type *data;                                                                     \
        slice *backing_slice;                                                                 \
    } type##_slice;                                                                           \
    static type##_slice slice_##type##_slice(slice *src, size_t start_idx, size_t end_offset) \
    {                                                                                         \
        const char *data = (const char *)src->data;                                           \
        size_t start = start_idx * src->size;                                                 \
        type##_slice dst = {                                                                  \
            end_offset - start_idx,                                                           \
            (type *)(data + start),                                                           \
            src};                                                                             \
        return dst;                                                                           \
    }

DEFINE_SLICE_TYPE(char);
DEFINE_SLICE_TYPE(size_t);

#endif
