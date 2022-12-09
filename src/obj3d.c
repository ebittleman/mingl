#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <callback.h>

#include "linmath.h"
#include "obj3d.h"
#include "types.h"

#define LINE_BUF_SIZE 1024

struct slice new_slice(size_t size)
{
    struct slice s = {0, DEFAULT_SLICE_CAP, size, malloc(size * DEFAULT_SLICE_CAP)};

    return s;
}

void expand_slice(struct slice *s)
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

void append_slice(struct slice *s, void *item)
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

void set_slice_item(struct slice *s, int i, void *item)
{
    if (0 <= i && i < s->len)
    {
        char *data = (char *)s->data;
        void *dst = &data[i * s->size];
        memcpy(dst, item, s->size);
    }
}
void *get_slice_item(struct slice *s, int i)
{
    if (0 <= i && i < s->len)
    {
        char *data = (char *)s->data;
        return &data[i * s->size];
    }

    return NULL;
}

void reset_slice(struct slice *s, int size, int len, int cap)
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

int line_reader(
    const char *filename,
    line_callback_t *callback,
    line_callback_t *callback2,
    line_callback_t *callback3,
    line_callback_t *callback4)
{
    FILE *f = NULL;
    int result = -1;
    char line_buffer[LINE_BUF_SIZE];

    f = fopen(filename, "r");
    if (f == NULL)
        goto fail;

    while (fgets(line_buffer, LINE_BUF_SIZE, f) != NULL)
    {
        if (ferror(f))
            goto fail;

        callback(line_buffer);
        callback2(line_buffer);
        callback3(line_buffer);
        callback4(line_buffer);
    }

    if (f)
    {
        fclose(f);
        errno = 0;
    }

    return 0;
fail:
    if (f)
    {
        int saved_errno = errno;
        fclose(f);
        errno = saved_errno;
    }
    return result;
}

void parse_vec3_line(struct slice *vec3_data, char *line)
{
    float x, y, z;
    sscanf(line, "%f %f %f", &x, &y, &z);
    append_slice(vec3_data, &x);
    append_slice(vec3_data, &y);
    append_slice(vec3_data, &z);
}

void parse_uv_line(struct slice *normal_data, char *line)
{
    float u, v;
    sscanf(line, "%f %f", &u, &v);
    append_slice(normal_data, &u);
    append_slice(normal_data, &v);
}

void handle_vertex_line(struct slice *vertices, va_alist list)
{
    char *line = va_arg_ptr(list, char *);

    char *pre = "v ";
    if (!strncmp(pre, line, strlen(pre)) == 0)
    {
        return;
    }
    parse_vec3_line(vertices, line + 2);
}

void handle_normal_line(struct slice *normal_data, va_alist list)
{
    struct Vector3 vec3;

    char *line = va_arg_ptr(list, char *);

    char *pre = "vn ";
    if (!strncmp(pre, line, strlen(pre)) == 0)
    {
        return;
    }
    parse_vec3_line(normal_data, line + 3);
}

void handle_uv_line(struct slice *uv_data, va_alist list)
{
    struct Vector3 vec3;

    char *line = va_arg_ptr(list, char *);

    char *pre = "vt ";
    if (!strncmp(pre, line, strlen(pre)) == 0)
    {
        return;
    }
    parse_uv_line(uv_data, line + 2);
}

void handle_face_line(struct slice *face_data, va_alist list)
{
    struct Vector3 vec3;

    char *line = va_arg_ptr(list, char *);

    char *pre = "f ";
    if (!strncmp(pre, line, strlen(pre)) == 0)
    {
        return;
    }
    parse_face_line(face_data, line + 2);
}

void parse_face_line(struct slice *face_data, char *line)
{

    int zero = 0;
    int num = -1;
    int idx = -1;

    bool no_slashes = strchr(line, '/') == NULL;

    idx = face_data->len;
    append_slice(face_data, &num);

    char num_buff[32] = {0}; // 32 chars + terminator */
    int len = strlen(line);

    char *cursor = (char *)line;
    // printf("%s\n", cursor);

    int i = 0;
    int start = 0;
    int end = 0;
    bool reading = false;
    char *last_seen = NULL;
    for (i = 0; i < len; i++)
    {
        char ch = cursor[i];
        if (ch != 0x20 && ch != 0x2f)
        {
            last_seen = &cursor[i];
            if (i == 0 || cursor[i - 1] == 0x20 || cursor[i - 1] == 0x2f)
            {
                start = i;
                reading = true;
            }
        }
        else if (ch == 0x20)
        {
            if (!reading)
            {
                if (last_seen != NULL && *last_seen == 0x2f)
                {
                    append_slice(face_data, &zero);
                }
                continue;
            }

            end = i;
            memset(num_buff, 0, sizeof(num_buff));
            strncpy(num_buff, cursor + start, end - start);
            num = atoi(num_buff);
            append_slice(face_data, &num);
            if (no_slashes)
            {
                append_slice(face_data, &zero);
                append_slice(face_data, &zero);
            }

            reading = false;
        }
        else if (ch == 0x2f)
        {
            last_seen = &cursor[i];
            if (!reading)
            {
                append_slice(face_data, &zero);
                continue;
            }
            end = i;
            memset(num_buff, 0, sizeof(num_buff));
            strncpy(num_buff, cursor + start, end - start);
            num = atoi(num_buff);
            append_slice(face_data, &num);

            reading = false;
        }
    }

    if (last_seen != NULL && *last_seen == 0x2f)
    {
        append_slice(face_data, &zero);
    }
    else if (reading)
    {
        end = len;
        memset(num_buff, 0, sizeof(num_buff));
        strncpy(num_buff, cursor + start, end - start);
        num = atoi(num_buff);
        append_slice(face_data, &num);
        if (no_slashes)
        {
            append_slice(face_data, &zero);
            append_slice(face_data, &zero);
        }
        reading = false;
    }

    int count = face_data->len - idx - 1;
    set_slice_item(face_data, idx, &count);
}

struct slice faces_to_elements(struct slice faces_slice)
{
    struct slice elements = new_slice(faces_slice.size);

    int *faces = (int *)faces_slice.data;
    int face_buffer[32];
    for (int i = 0; i < faces_slice.len; i++)
    {
        int count = faces[i];
        // printf("num items: %d\n", count);

        int start = i + 1;
        int end = start + count;
        for (int x = start; x < end; x += 3)
        {
            int step = (end - (end - x) - start) / 3;
            face_buffer[step] = faces[x] - 1;
            // printf("line_idx: %d, vert_idx: %d\n", step, faces[x]);
        }
        // printf("count: %d\n", (count / 3));

        int iters = (count / 3) - 2;

        if (iters <= 0)
        {
            append_slice(&elements, &face_buffer[0]);
            append_slice(&elements, &face_buffer[1]);
            append_slice(&elements, &face_buffer[2]);
            continue;
        }

        for (int x = 0; x < iters; x++)
        {
            if (x == 0)
            {
                append_slice(&elements, &face_buffer[0]);
                append_slice(&elements, &face_buffer[1]);
                append_slice(&elements, &face_buffer[2]);
                // append_slice(&elements, &face_buffer[3]);
                // printf("%d, %d, %d\n",
                //        face_buffer[0],
                //        face_buffer[1],
                //        face_buffer[2]);
            }
            else
            {
                int offset = x + 1;
                // int offset = x + 2;
                append_slice(&elements, &face_buffer[0]);
                append_slice(&elements, &face_buffer[offset]);
                append_slice(&elements, &face_buffer[offset + 1]);
                // append_slice(&elements, &face_buffer[offset + 2]);
                // printf("%d, %d, %d\n",
                //        face_buffer[offset],
                //        face_buffer[offset + 1],
                //        face_buffer[0]);
            }
        }

        i += count;
    }

    return elements;
}

void noop(char *line) {}

void load_obj_file(const char *obj_file_name, struct slice buffers[], float bounds[6])
{
    struct slice vertices_slice = {0, 0, sizeof(float), NULL};
    line_callback_t *vertices_callback = (line_callback_t *)alloc_callback(
        (callback_function_t)&handle_vertex_line, &vertices_slice);

    struct slice faces = new_slice(sizeof(int));
    line_callback_t *faces_callback = (line_callback_t *)alloc_callback(
        (callback_function_t)&handle_face_line, &faces);

    // struct slice normals_slice = new_slice(sizeof(float));
    // line_callback_t *normals_callback = (line_callback_t *)alloc_callback(
    //     (callback_function_t)&handle_normal_line, &normals_slice);

    struct slice normals_slice = new_slice(sizeof(float));
    line_callback_t *normals_callback = (line_callback_t *)alloc_callback(
        (callback_function_t)&noop, &normals_slice);

    struct slice uv_slice = new_slice(sizeof(float));
    line_callback_t *uv_callback = (line_callback_t *)alloc_callback(
        (callback_function_t)&handle_uv_line, &uv_slice);

    line_reader(obj_file_name,
                vertices_callback,
                faces_callback,
                normals_callback,
                uv_callback);

    free_callback((callback_t)vertices_callback);
    free_callback((callback_t)faces_callback);
    // free_callback((callback_t)normals_callback);
    free_callback((callback_t)uv_callback);

    struct slice elements_slice = faces_to_elements(faces);
    if (faces.cap > 0)
    {
        free(faces.data);
    }

    printf("Found %d floats\n", vertices_slice.len);
    // float bounds[6] = {0.0f};
    vec3 *verts = (vec3 *)vertices_slice.data;
    int num_verts = vertices_slice.len / 3;

    bounds[0] = verts[0][0];
    bounds[1] = verts[0][0];
    bounds[2] = verts[0][1];
    bounds[3] = verts[0][1];
    bounds[4] = verts[0][2];
    bounds[5] = verts[0][2];

    for (size_t i = 1; i < num_verts; i++)
    {
        for (int y = 0; y < 3; y++)
        {
            int offset = y * 2;
            if (verts[i][y] < bounds[offset])
            {
                bounds[offset] = verts[i][y];
            }
            else if (verts[i][y] > bounds[offset + 1])
            {
                bounds[offset + 1] = verts[i][y];
            }
        }

        // printf("x: %f, y: %f, z: %f\n", verts[0], verts[1], verts[2]);
    }

    printf("min x: %f, max x: %f\n", bounds[0], bounds[1]);
    printf("min y: %f, max y: %f\n", bounds[2], bounds[3]);
    printf("min z: %f, max z: %f\n", bounds[4], bounds[5]);

    printf("Found %d elements\n", elements_slice.len);
    // for (size_t i = 0; i < elements_slice.len; i += 3)
    // {
    //     printf("x: %d, y: %d, z: %d\n",
    //            elements[i],
    //            elements[i + 1],
    //            elements[i + 2]);
    // }

    buffers[VERTS] = vertices_slice;
    buffers[UVS] = uv_slice;
    buffers[NORMALS] = normals_slice;
    buffers[COLORS] = new_slice(vertices_slice.size);
    buffers[ELEMENTS] = elements_slice;
    return;
}
