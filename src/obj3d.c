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

struct _file_data
{
    size_t len;
    char *data;
    bool ok;
};

int read_vec3(const char *p, struct Vector3 *out)
{
    int bytes_read = 0;
    int i = 0;
    float buffer[3] = {0, 0, 0};
    struct Vector3 result = {0, 0, 0};

    char *end;
    for (double f = strtod(p, &end); p != end; f = strtod(p, &end))
    {
        bytes_read += (int)(end - p);
        p = end;
        if (i >= 3)
        {
            printf("too many values\n");
            continue;
        }
        buffer[i] = f;
        i++;
    }

    memcpy(out, buffer, sizeof(buffer[0]) * 3);
    return bytes_read;
}

void append_vertex(struct slice *vertices, va_alist list)
{
    struct Vector3 vec3;

    char *line = va_arg_ptr(list, char *);

    char *pre = "v ";
    if (!strncmp(pre, line, strlen(pre)) == 0)
    {
        return;
    }
    read_vec3(line + 1, &vec3);
    append_slice(vertices, &vec3);
}

int line_reader(const char *filename, line_callback_t *callback, line_callback_t *callback2)
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

void read_face_line(struct slice *face_data, va_alist list)
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