#include "tests.h"

#include "data_structures.h"
#include "obj3d.h"
#include "entities.h"

int test_loading_vertices(char *error_buffer);

test_case entities_tests[] = {
    {"test_loading_vertices", test_loading_vertices},
    // {"test_read_face_just_ints", test_read_face_just_ints},
};

void register_entities_tests(slice *tests_suites)
{
    test_suite suite = {
        "entities",
        entities_tests,
        sizeof(entities_tests) / sizeof(test_case)};

    append_slice(tests_suites, &suite);
}

int test_loading_vertices(char *error_buffer)
{

    float verts[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.25f, 0.5f, 0.0f,
        0.0f, 0.5f, 0.0f};

    vec3 *vertices_index = (vec3 *)verts;

    size_t elements[] = {
        0, 0, 0, 1, 0, 0, 2, 0, 0,
        0, 0, 0, 2, 0, 0, 3, 0, 0};

    vertex vert = {0};
    slice vertices_slice = new_slice(sizeof(vertex));
    vertex *vertices = (vertex *)vertices_slice.data;

    uintptr_t normal_offset = offsetof(vertex, normal);
    size_t normal_size = sizeof(vert.normal);

    int step;

    for (int x = 0; x < 18; x += 9)
    {
        append_slice(&vertices_slice, &vert);
        append_slice(&vertices_slice, &vert);
        append_slice(&vertices_slice, &vert);

        step = x / 9;
        memcpy(((void *)&vertices[0 + step]) + normal_offset, vertices_index[elements[x + FACE_VERTS]], normal_size);
        memcpy(((void *)&vertices[1 + step]) + normal_offset, vertices_index[elements[x + 3 + FACE_VERTS]], normal_size);
        memcpy(((void *)&vertices[2 + step]) + normal_offset, vertices_index[elements[x + 6 + FACE_VERTS]], normal_size);
    }

    return 0;
}