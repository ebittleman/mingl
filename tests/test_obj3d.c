#include "tests.h"

#include "obj3d.h"

int test_sscanf_face(char *error_buffer);
// int test_read_face_just_ints(char *error_buffer);

test_case obj3d_tests[] = {
    {"test_sscanf_face", test_sscanf_face},
    // {"test_read_face_just_ints", test_read_face_just_ints},
};

void register_obj3d_tests(slice *tests_suites)
{
    test_suite suite = {
        "obj3d",
        obj3d_tests,
        sizeof(obj3d_tests) / sizeof(test_case)};

    append_slice(tests_suites, &suite);
}

int test_sscanf_face(char *error_buffer)
{

    char *line = "f 5/1/1 45 3/2/ 11// 6//100 77 1/2/3";
    const size_t expected_data[] = {
        5, 1, 1,
        45, 0, 0,
        3, 2, 0,
        11, 0, 0,
        6, 0, 100,
        77, 0, 0,
        1, 2, 3};

    size_t len = strlen(line) - 2;
    size_t vertex_count = sizeof(expected_data) / sizeof(size_t) / 3;
    size_t3 actual_data[vertex_count];

    sscanf_face(line + 2, len, vertex_count, actual_data);

    if (memcmp(expected_data, actual_data, sizeof(expected_data)) != 0)
    {
        sprintf(error_buffer, "Unexpected data found parsing element line");
        for (size_t i = 0; i < vertex_count; i++)
        {
            size_t *data = actual_data[i];
            printf("%llu, %llu, %llu\n", data[0], data[1], data[2]);
        }

        return 1;
    }

    return 0;
}

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
