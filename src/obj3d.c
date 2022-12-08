#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <callback.h>

#include "obj3d.h"

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
        reading = false;
    }

    int count = face_data->len - idx - 1;
    set_slice_item(face_data, idx, &count);
}

struct slice faces_to_elements(struct slice faces_slice)
{
    struct slice elements = new_slice(faces_slice.size);

    int *faces = (int *)faces_slice.data;
    int face_buffer[16];
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
        for (int x = 0; x < (count / 3) - 2; x++)
        {
            if (x == 0)
            {
                append_slice(&elements, &face_buffer[0]);
                append_slice(&elements, &face_buffer[1]);
                append_slice(&elements, &face_buffer[2]);
                // printf("%d, %d, %d\n",
                //        face_buffer[0],
                //        face_buffer[1],
                //        face_buffer[2]);
            }
            else
            {
                int offset = x + 1;
                append_slice(&elements, &face_buffer[offset]);
                append_slice(&elements, &face_buffer[offset + 1]);
                append_slice(&elements, &face_buffer[0]);
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
