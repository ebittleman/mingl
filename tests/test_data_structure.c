/**
 * here are some docs
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "tests.h"
#include "data_structures.h"

int test_new_slice(char *error_buffer);
int test_expand_slice(char *error_buffer);
int test_append_slice(char *error_buffer);
int test_get_slice_item(char *error_buffer);
int test_key_find(char *error_buffer);
int test_key_insert(char *error_buffer);

test_case data_structure_tests[] = {
    {"test_new_slice", test_new_slice},
    {"test_expand_slice", test_expand_slice},
    {"test_append_slice", test_append_slice},
    {"test_get_slice_item", test_get_slice_item},
    {"test_key_find", test_key_find},
    {"test_key_insert", test_key_insert},
};

void register_data_structure_tests(slice *tests_suites)
{
    test_suite suite = {
        "data_structures",
        data_structure_tests,
        sizeof(data_structure_tests) / sizeof(test_case)};

    append_slice(tests_suites, &suite);
}

#define EPSILON 0.0000001f

bool compare_float(float x, float y, float epsilon)
{
    if (fabs(x - y) < epsilon)
        return true; // they are same
    return false;    // they are not same
}

int util_cmp_slice(char *error_buffer, slice actual,
                   size_t expected_len,
                   size_t expected_cap,
                   size_t expected_size,
                   char *expected_data)
{
    if (strcmp(actual.data, expected_data) != 0)
    {
        sprintf(error_buffer, "data error. Expected: %s, Got: %s", expected_data, (char *)actual.data);
        free(actual.data);
        return 1;
    }
    free(actual.data);

    if (actual.len != expected_len)
    {
        sprintf(error_buffer, "len error. Expected: %llu, Got: %llu", expected_len, actual.len);
        return 1;
    }

    if (actual.cap != expected_cap)
    {
        sprintf(error_buffer, "cap error. Expected: %llu, Got: %llu", expected_cap, actual.cap);
        return 1;
    }

    if (actual.size != expected_size)
    {
        sprintf(error_buffer, "size error. Expected: %llu, Got: %llu", expected_size, actual.size);
        return 1;
    }

    return 0;
}

int test_new_slice(char *error_buffer)
{
    slice expected = {0, 10, sizeof(float), NULL};
    slice actual = new_slice(sizeof(float));

    if (actual.data == NULL)
    {
        sprintf(error_buffer, "Error allocating slice data");
        return 1;
    }
    free(actual.data);

    if (actual.len != expected.len)
    {
        sprintf(error_buffer, "len error. Expected: %llu, Got: %llu", expected.len, actual.len);
        return 1;
    }

    if (actual.cap != expected.cap)
    {
        sprintf(error_buffer, "cap error. Expected: %llu, Got: %llu", expected.cap, actual.cap);
        return 1;
    }

    if (actual.size != expected.size)
    {
        sprintf(error_buffer, "size error. Expected: %llu, Got: %llu", expected.size, actual.size);
        return 1;
    }

    return 0;
}

int test_expand_slice(char *error_buffer)
{

    size_t expected_len = 6;
    size_t expected_cap = 20;
    size_t expected_size = sizeof(char);
    char *expected_data = "hello";

    char *previous_data = malloc(6);
    strcpy(previous_data, expected_data);

    slice actual = {6, DEFAULT_SLICE_CAP, expected_size, (void *)previous_data};

    expand_slice(&actual);

    if (previous_data == actual.data)
    {
        sprintf(error_buffer, "data error. New memory not allocated");
        free(actual.data);
        return 1;
    }

    return util_cmp_slice(
        error_buffer, actual, expected_len, expected_cap,
        expected_size, expected_data);
}

int test_append_slice(char *error_buffer)
{
    char *expected_data = "The quick brown fox jumps over the lazy dog";
    size_t expected_len = strlen(expected_data) + 1;
    size_t expected_cap = 80;
    size_t expected_size = sizeof(char);

    slice actual = new_slice(expected_size);
    for (int i = 0; i < expected_len; i++)
    {
        char ch = expected_data[i];
        append_slice(&actual, &ch);
        // printf("'%c' == '%c'\n", ch, ((char *)actual.data)[i]);
    }

    return util_cmp_slice(
        error_buffer, actual, expected_len, expected_cap,
        expected_size, expected_data);
}

int test_get_slice_item(char *error_buffer)
{
    char *expected_data = "Thequickbrownfoxjumpsoverthelazydog";
    size_t expected_len = strlen(expected_data);
    size_t expected_cap = 80;
    size_t expected_size = sizeof(char);

    char *previous_data = malloc(expected_cap);
    strcpy(previous_data, expected_data);
    slice actual = {expected_len, expected_cap, expected_size, (void *)previous_data};

    int arg_i = 12;

    char *ch = (char *)get_slice_item(&actual, arg_i);

    if (*ch != expected_data[arg_i])
    {
        sprintf(error_buffer, "data. Expected: %c, Got: %c", expected_data[arg_i], *ch);
        free(actual.data);
        return 1;
    }

    return util_cmp_slice(
        error_buffer, actual, expected_len, expected_cap,
        expected_size, expected_data);
}

int test_key_find(char *error_buffer)
{
    const char *keys[] = {
        "along\0\0",
        "list\0\0\0",
        "memory\0",
        "of\0\0\0\0\0",
        "truth\0\0",
        "xylopho",
        "year\0\0\0",
        "zebra\0\0",
    };
    tree_node tree = {0};
    tree.len = sizeof(keys) / sizeof(const char *);

    memcpy(tree.keys, keys[0], sizeof(keys));

    size_t expected_index = 1;
    size_t index = key_find("before", tree.len, tree.keys);
    if (index != expected_index)
    {
        sprintf(error_buffer, "expected: %llu, got: %llu\n", expected_index, index);
        return 1;
    }
    return 0;
}

int test_key_insert(char *error_buffer)
{
    char line_buffer[32] = {0};
    char bytes[] = {0xFF, 0xDE};

    tree_node *root = malloc_node(&global_node_pool);

    root->is_leaf = true;

    char *key = NULL;
    for (int i = 1; i <= 4096; i++)
    {
        sprintf(line_buffer, "id%.5d", i);
        insert(&root, line_buffer, &bytes);
    }

    tree_node *ptr = search("id02000", root);
    size_t count = 0, nodes = 0, low_count = 0;
    while (ptr != NULL)
    {
        nodes++;
        if (ptr->len < _TREE_B / 2)
        {

            sprintf(line_buffer, "\nlow child count '%s'=%d", &ptr->keys[0], ptr->len);
            strcat(error_buffer, line_buffer);
            low_count++;
        }
        for (size_t i = 0; i < ptr->len; i++)
        {
            count++;
        }

        if (ptr->p[TREE_THRESHOLD] == 0)
        {
            ptr = NULL;
            continue;
        }

        ptr = to_ptr(tree_node, ptr->p[TREE_THRESHOLD]);
    }
    sprintf(line_buffer, "\nnum nodes: %llu", nodes);
    strcat(error_buffer, line_buffer);
    sprintf(line_buffer, "\nnum items: %llu", count);
    strcat(error_buffer, line_buffer);

    if (low_count > 0 || nodes != 17 || count != 2191)
    {
        return -1;
    }
    return 0;
}
