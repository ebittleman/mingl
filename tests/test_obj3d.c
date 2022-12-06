#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "obj3d.h"

int test_new_slice(char *error_buffer);
int test_expand_slice(char *error_buffer);
int test_append_slice(char *error_buffer);
int test_get_slice_item(char *error_buffer);
int test_read_vec3(char *error_buffer);
int test_read_face(char *error_buffer);

int main()
{
    int err;
    char error_buffer[128];

    printf("==========\n\n");

    printf("Running: test_new_slice\n");
    err = test_new_slice((char *)error_buffer);
    if (err > 0)
    {
        printf("FAIL: test_new_slice\n\t%s\n", error_buffer);
        return 1;
    }
    printf("Success: test_new_slice\n");

    printf("\n==========\n\n");

    printf("Running: test_expand_slice\n");
    err = test_expand_slice((char *)error_buffer);
    if (err > 0)
    {
        printf("FAIL: test_expand_slice\n\t%s\n", error_buffer);
        return 1;
    }
    printf("Success: test_expand_slice\n");

    printf("\n==========\n\n");

    printf("Running: test_append_slice\n");
    err = test_append_slice((char *)error_buffer);
    if (err > 0)
    {
        printf("FAIL: test_append_slice\n\t%s\n", error_buffer);
        return 1;
    }
    printf("Success: test_append_slice\n");

    printf("\n==========\n\n");

    printf("Running: test_get_slice_item\n");
    err = test_get_slice_item((char *)error_buffer);
    if (err > 0)
    {
        printf("FAIL: test_get_slice_item\n\t%s\n", error_buffer);
        return 1;
    }
    printf("Success: test_get_slice_item\n");

    printf("\n==========\n\n");

    printf("Running: test_read_vec3\n");
    err = test_read_vec3((char *)error_buffer);
    if (err > 0)
    {
        printf("FAIL: test_read_vec3\n\t%s\n", error_buffer);
        return 1;
    }
    printf("Success: test_read_vec3\n");

    printf("\n==========\n\n");

    printf("Running: test_read_face\n");
    err = test_read_face((char *)error_buffer);
    if (err > 0)
    {
        printf("FAIL: test_read_face\n\t%s\n", error_buffer);
        return 1;
    }
    printf("Success: test_read_face\n");

    printf("\n==========\n\n");

    return 0;
}

#define EPSILON 0.0000001f

bool compare_float(float x, float y, float epsilon)
{
    if (fabs(x - y) < epsilon)
        return true; // they are same
    return false;    // they are not same
}

int util_cmp_slice(char *error_buffer, struct slice actual,
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
        sprintf(error_buffer, "len error. Expected: %d, Got: %d", expected_len, actual.len);
        return 1;
    }

    if (actual.cap != expected_cap)
    {
        sprintf(error_buffer, "cap error. Expected: %d, Got: %d", expected_cap, actual.cap);
        return 1;
    }

    if (actual.size != expected_size)
    {
        sprintf(error_buffer, "size error. Expected: %d, Got: %d", expected_size, actual.size);
        return 1;
    }

    return 0;
}

int test_new_slice(char *error_buffer)
{
    struct slice expected = {0, 10, sizeof(float), NULL};
    struct slice actual = new_slice(sizeof(float));

    if (actual.data == NULL)
    {
        sprintf(error_buffer, "Error allocating slice data");
        return 1;
    }
    free(actual.data);

    if (actual.len != expected.len)
    {
        sprintf(error_buffer, "len error. Expected: %d, Got: %d", expected.len, actual.len);
        return 1;
    }

    if (actual.cap != expected.cap)
    {
        sprintf(error_buffer, "cap error. Expected: %d, Got: %d", expected.cap, actual.cap);
        return 1;
    }

    if (actual.size != expected.size)
    {
        sprintf(error_buffer, "size error. Expected: %d, Got: %d", expected.size, actual.size);
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

    struct slice actual = {6, DEFAULT_SLICE_CAP, expected_size, (void *)previous_data};

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
    size_t expected_len = strlen(expected_data);
    size_t expected_cap = 80;
    size_t expected_size = sizeof(char);

    struct slice actual = new_slice(expected_size);
    for (int i = 0; i < expected_len; i++)
    {
        char ch = expected_data[i];
        append_slice(&actual, &ch);
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
    struct slice actual = {expected_len, expected_cap, expected_size, (void *)previous_data};

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

int test_read_vec3(char *error_buffer)
{
    const char *p = "v 4.500502 1.153829 0.924606\nv 4.445763 1.159163 0.949913";
    struct Vector3 actual;
    struct Vector3 expected = {4.500502, 1.153829, 0.924606};

    int num = read_vec3(p + 1, &actual);
    // printf("x: %f, y: %f, z: %f'\n", actual.x, actual.y, actual.z);
    // printf("%d: '%s'\n", num, p + 1 + num);

    if (!compare_float(actual.x, expected.x, EPSILON))
    {
        sprintf(error_buffer, "Vector3.x. Expected: %f, Got: %f", expected.x, actual.x);
        return 1;
    }

    if (!compare_float(actual.y, expected.y, EPSILON))
    {
        sprintf(error_buffer, "Vector3.y. Expected: %f, Got: %f", expected.y, actual.y);
        return 1;
    }

    if (!compare_float(actual.z, expected.z, EPSILON))
    {
        sprintf(error_buffer, "Vector3.z. Expected: %f, Got: %f", expected.z, actual.z);
        return 1;
    }

    return 0;
}

int test_read_face(char *error_buffer)
{

    struct slice nums = new_slice(sizeof(int));
    // char *line = "f 12558//8857 12518//8857 12517//8857 12559//8857 12560//8857 12561//8857";
    char *line = "f 5/1/1 3/2/1 1/3/1";

    parse_face_line(&nums, line + 2);

    int num_faces = nums.len / 3;
    int *item;
    int indexes = *(int *)get_slice_item(&nums, 0);
    printf("Indexes: %d\n", indexes);
    for (int i = 1; i < nums.len; i += 3)
    {
        printf("x: %d, y: %d, z: %d\n",
               *(int *)get_slice_item(&nums, i),
               *(int *)get_slice_item(&nums, i + 1),
               *(int *)get_slice_item(&nums, i + 2));
    }
    printf("Num Faces: %d\n", num_faces);
    printf("Num By Idx: %d\n", *(int *)get_slice_item(&nums, 0));
    return 0;
}