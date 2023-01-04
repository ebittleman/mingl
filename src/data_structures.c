
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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool is_leaf(struct _tree_node *node)
{
    return node->is_leaf;
}

unsigned int leaf_search(const char *key, tree_node *node, tree_node *path[])
{

    tree_node *cursor = node;
    unsigned int depth = 0;
    while (cursor != NULL)
    {
        path[depth] = cursor;
        depth++;

        if (is_leaf(cursor))
        {
            cursor = NULL;
            continue;
        }

        tree_node *next = NULL;
        for (size_t i = 0; i < cursor->len; i++)
        {
            int result = strcmp(key, cursor->keys[i]);
            if (result == 0)
            {
                struct _tree_node *child = cursor->p[i];
                next = child;
                break;
            }

            if (result < 0)
            {
                size_t idx = i == 0 ? 0 : i - 1;
                struct _tree_node *child = cursor->p[idx];
                next = child;
                break;
            }
        }

        if (next == NULL)
        {
            cursor = cursor->p[cursor->len - 1];
            continue;
        }

        cursor = next;
    }

    return depth;
}

tree_node *search(char *key, tree_node *root)
{
    tree_node *path[100] = {0};
    unsigned int path_len = leaf_search(key, root, path);
    tree_node *leaf = path[path_len - 1];
    return leaf;
}

size_t key_find(const char *key, size_t n, const char **keys)
{
    size_t start = 0, mid = 0, end = n;
    while (start < end)
    {
        mid = ((start + end) / 2);
        int result = strcmp(keys[mid], key);

        if (result < 0)
        {
            start = mid + 1;
        }
        else
        {
            end = mid;
        }
    }
    return start;
}

tree_node *split_node(tree_node *original)
{
    tree_node *new_node = malloc(sizeof(tree_node));
    memset(new_node, 0, sizeof(tree_node));
    int mid = original->len / 2;
    void *start = &original->p[mid];

    new_node->len = original->len - mid;
    memcpy(new_node->p, start, new_node->len * sizeof(tree_node *));

    start = &original->keys[mid];
    memcpy(new_node->keys, start, new_node->len * sizeof(const char *));
    new_node->is_leaf = original->is_leaf;

    original->p[TREE_THRESHOLD] = new_node;
    original->keys[TREE_THRESHOLD] = NULL;
    original->len = mid;

    return new_node;
}

void _insert(tree_node *node, const char *key, void *data)
{
    size_t index = key_find(key, node->len, node->keys);
    for (size_t i = TREE_THRESHOLD - 1; i > index; i--)
    {
        node->p[i] = node->p[i - 1];
        node->keys[i] = node->keys[i - 1];
        node->p[i] = node->p[i - 1];
    }

    char *trimmed_key = malloc(strlen(key) + 1);
    strcpy(trimmed_key, key);

    node->keys[index] = trimmed_key;
    node->p[index] = data;
    node->len++;
}

void insert(tree_node **root, const char *key, void *data)
{
    tree_node *path[10] = {0};
    unsigned int path_len = leaf_search(key, *root, path);
    tree_node *leaf = path[path_len - 1];
    _insert(leaf, key, data);

    for (int i = path_len - 1; i >= 0; i--)
    {
        tree_node *cursor = path[i];
        if (cursor->len == TREE_THRESHOLD)
        {
            tree_node *new_node = split_node(cursor);

            tree_node *parent;
            if (i == 0)
            {
                parent = malloc(sizeof(tree_node));
                memset(parent, 0, sizeof(tree_node));
                _insert(parent, cursor->keys[0], cursor);
                *root = parent;
            }
            else
            {
                parent = path[i - 1];
            }

            _insert(parent, new_node->keys[0], new_node);
            parent->is_leaf = false;
        }
    }
}
