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

typedef int(test_func_t)(char *);

int test_new_slice(char *error_buffer);
int test_expand_slice(char *error_buffer);
int test_append_slice(char *error_buffer);
int test_get_slice_item(char *error_buffer);

// int test_read_vec3(char *error_buffer);
// int test_read_face(char *error_buffer);
// int test_read_face_just_ints(char *error_buffer);

test_case tests[] = {
    {"test_new_slice", test_new_slice},
    {"test_expand_slice", test_expand_slice},
    {"test_append_slice", test_append_slice},
    {"test_get_slice_item", test_get_slice_item},
    // {"test_read_face", test_read_face},
    // {"test_read_face_just_ints", test_read_face_just_ints},
};

void register_data_structure_tests(slice *tests_suites)
{
    test_suite suite = {
        "data_structures",
        tests,
        sizeof(tests) / sizeof(test_case)};

    append_slice(tests_suites, &suite);
}

int main1()
{
    int err;
    char error_buffer[1024];
    struct test_case
    {
        const char *name;
        test_func_t *func;
    };

    int len = sizeof(tests) / sizeof(test_case);
    printf("==========\n\n");

    for (int x = 0; x < len; x++)
    {
        printf("==========\n\n");

        printf("Running: %s\n", tests[x].name);
        err = tests[x].func((char *)error_buffer);
        if (err > 0)
        {
            printf("FAIL: %s\n\t%s\n", tests[x].name, error_buffer);
            return 1;
        }
        printf("Success: %s\n", tests[x].name);
    }

    return 0;
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

// int test_read_face(char *error_buffer)
// {
//     return 0;

//     slice nums = new_slice(sizeof(int));
//     // char *line = "f 12558//8857 12518//8857 12517//8857 12559//8857 12560//8857 12561//8857";
//     char *line = "f 5/1/1 3/2/1 1/3/1";

//     parse_face_line(&nums, line + 2);

//     int num_faces = nums.len / 3;
//     int *item;
//     int indexes = *(int *)get_slice_item(&nums, 0);
//     printf("Indexes: %d\n", indexes);
//     for (int i = 1; i < nums.len; i += 3)
//     {
//         printf("x: %d, y: %d, z: %d\n",
//                *(int *)get_slice_item(&nums, i),
//                *(int *)get_slice_item(&nums, i + 1),
//                *(int *)get_slice_item(&nums, i + 2));
//     }
//     printf("Num Faces: %d\n", num_faces);
//     printf("Num By Idx: %d\n", *(int *)get_slice_item(&nums, 0));
//     return 0;
// }

// int test_read_face_just_ints(char *error_buffer)
// {

//     slice nums = new_slice(sizeof(int));
//     char *line = "f 12558 12518 12517 12559\n";
//     // char *line = "f 5/1/1 3/2/1 1/3/1";

//     parse_face_line(&nums, line + 2);

//     int num_faces = nums.len / 3;
//     int *item;
//     int indexes = *(int *)get_slice_item(&nums, 0);
//     printf("Indexes: %d\n", indexes);
//     for (int i = 1; i < nums.len; i += 3)
//     {
//         printf("x: %d, y: %d, z: %d\n",
//                *(int *)get_slice_item(&nums, i),
//                *(int *)get_slice_item(&nums, i + 1),
//                *(int *)get_slice_item(&nums, i + 2));
//     }
//     printf("Num Faces: %d\n", num_faces);
//     // printf("Num By Idx: %d\n", *(int *)get_slice_item(&nums, 0));
//     return 0;
// }
