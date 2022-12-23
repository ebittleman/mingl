
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "data_structures.h"

slice new_slice(size_t size)
{
    slice s = {0, DEFAULT_SLICE_CAP, size, malloc(size * DEFAULT_SLICE_CAP)};

    return s;
}

void expand_slice(slice *s)
{
    void *old_data = s->data;

    if (s->cap < 1)
    {
        s->cap = 100;
    }
    else
    {
        s->cap *= 2;
    }
    s->data = malloc(s->size * s->cap);

    if (old_data == NULL)
    {
        return;
    }

    memcpy(s->data, old_data, s->len * s->size);

    free(old_data);
    old_data = NULL;
}

void append_slice_size_t(slice *s, size_t value)
{
    append_slice(s, &value);
}

void append_slice(slice *s, void *item)
{
    if (s->len + 1 > s->cap)
    {
        expand_slice(s);
    }

    char *data = (char *)s->data;
    void *dst = &data[s->len * s->size];
    memcpy(dst, item, s->size);
    s->len += 1;
}

void extend_slice(slice *s, int n, void *item)
{
    while (s->len + n > s->cap)
    {
        expand_slice(s);
    }
    char *data = (char *)s->data;
    void *dst = &data[s->len * s->size];
    memcpy(dst, item, n * s->size);
    s->len += n;
}

void set_slice_item(slice *s, int i, void *item)
{
    if (0 <= i && i < s->len)
    {
        char *data = (char *)s->data;
        void *dst = &data[i * s->size];
        memcpy(dst, item, s->size);
    }
}

void *get_slice_item(slice *s, int i)
{
    if (0 <= i && i < s->len)
    {
        char *data = (char *)s->data;
        return &data[i * s->size];
    }

    return NULL;
}

void reset_slice(slice *s, int size, int len, int cap)
{
    if (s->data != NULL)
    {
        free(s->data);
        s->data = NULL;
    }
    s->size = size;
    s->len = len;
    s->cap = cap;

    s->data = malloc(s->cap * s->size);
    memset(s->data, 0, s->cap * s->size);
}

void debug_vec3(vec3 v)
{
    printf("x: %f, y: %f, z: %f\n", v[0], v[1], v[2]);
}

void debug_mat(mat4x4 m)
{
    printf("%f %f %f %f\n", m[0][0], m[0][1], m[0][2], m[0][3]);
    printf("%f %f %f %f\n", m[1][0], m[1][1], m[1][2], m[1][3]);
    printf("%f %f %f %f\n", m[2][0], m[2][1], m[2][2], m[2][3]);
    printf("%f %f %f %f\n", m[3][0], m[3][1], m[3][2], m[3][3]);
}

void debug_bounds(float bounds[6])
{
    printf("x:%f = %f - %f\n", bounds[1] - bounds[0], bounds[1], bounds[0]);
    printf("y:%f = %f - %f\n", bounds[3] - bounds[2], bounds[3], bounds[2]);
    printf("z:%f = %f - %f\n", bounds[5] - bounds[4], bounds[5], bounds[4]);
}
