#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <strings.h>
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

#define _TREE_B 255
#define TREE_THRESHOLD _TREE_B - 1
#define KEY_WIDTH 8
#define KEY_LEN (_TREE_B) * (KEY_WIDTH)
typedef struct _tree_node
{
    bool is_leaf;
    uintptr_t p[_TREE_B];
    char keys[KEY_LEN];
    unsigned int len;

} tree_node;

typedef struct
{
    tree_node nodes[8192];
    size_t len;
} node_pool;
extern node_pool global_node_pool;

size_t key_find(const char *key, size_t n, const char *keys);
void insert(tree_node **root, const char *key, void *data);
tree_node *search(char *key, tree_node *root);
tree_node *malloc_node(node_pool *np);

#define to_rel(ptr, ref) ptr = (uintptr_t)(((char *)ref) - ((char *)(&ptr)))
#define to_ptr(TYPE, ref) (TYPE *)((ref == 0) ? NULL : ((ref) + ((char *)(&ref))))

#endif
