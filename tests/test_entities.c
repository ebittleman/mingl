#include "tests.h"

#include "data_structures.h"
#include "obj3d.h"
#include "entities.h"

int test_load_vertex_by_face(char *error_buffer);

test_case entities_tests[] = {
    {"test_load_vertex_by_face", test_load_vertex_by_face},
};

void register_entities_tests(slice *tests_suites)
{
    test_suite suite = {
        "entities",
        entities_tests,
        sizeof(entities_tests) / sizeof(test_case)};

    append_slice(tests_suites, &suite);
}

int test_load_vertex_by_face(char *error_buffer)
{

    float verts[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.25f, 0.5f, 0.0f,
        0.0f, 0.5f, 0.0f};

    float normals[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f};

    size_t elements[] = {
        0, 0, 0, 1, 0, 0, 2, 0, 0,
        0, 0, 1, 2, 0, 1, 3, 0, 1};

    vec3 *normals_index = (vec3 *)normals;
    vec3 *vertices_index = (vec3 *)verts;
    face *faces = (face *)elements;

    size_t size_elements = sizeof(elements);
    size_t num_faces = size_elements / sizeof(face);

    triangle triangles[num_faces];
    vertex vert = {0};

    printf("size of of triangles: %llu\n", sizeof(triangles));

    size_t position_size = sizeof(vert.position);
    for (int i = 0; i < num_faces; i++)
    {
        face_elements *current_face = (face_elements *)&faces[i];
        vertex *current_triangle = (vertex *)triangles[i];
        memset(current_triangle, 0, sizeof(triangle));

        size_t v1_idx = current_face[0].position_idx,
               v2_idx = current_face[1].position_idx,
               v3_idx = current_face[2].position_idx;

        vertex *vert1 = (vertex *)&current_triangle[0],
               *vert2 = (vertex *)&current_triangle[1],
               *vert3 = (vertex *)&current_triangle[2];

        memcpy(&vert1->position, vertices_index[v1_idx], position_size);
        memcpy(&vert2->position, vertices_index[v2_idx], position_size);
        memcpy(&vert3->position, vertices_index[v3_idx], position_size);
    }

    if (sizeof(normals) > 0)
    {
        size_t normal_size = sizeof(vert.normal);
        for (int i = 0; i < num_faces; i++)
        {
            face_elements *current_face = (face_elements *)&faces[i];
            vertex *current_triangle = (vertex *)triangles[i];

            size_t n1_idx = current_face[0].normal_idx,
                   n2_idx = current_face[1].normal_idx,
                   n3_idx = current_face[2].normal_idx;

            vertex *vert1 = (vertex *)&current_triangle[0],
                   *vert2 = (vertex *)&current_triangle[1],
                   *vert3 = (vertex *)&current_triangle[2];

            memcpy(&vert1->normal, normals_index[n1_idx], normal_size);
            memcpy(&vert2->normal, normals_index[n2_idx], normal_size);
            memcpy(&vert3->normal, normals_index[n2_idx], normal_size);
        }
    }

    return 0;
}