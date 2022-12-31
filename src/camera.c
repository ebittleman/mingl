
#include "camera.h"

void update_camera_vectors(camera *cam);

void init_camera(camera *cam)
{
    vec3 position = {0.0f, 0.5f, 10.0f};
    vec3 world_up = {0.0f, 1.0f, 0.0f};
    vec3 front = {0.0f, 0.0f, -1.0f};

    memcpy(cam->position, position, sizeof(cam->position));
    memcpy(cam->front, front, sizeof(cam->front));
    memcpy(cam->world_up, world_up, sizeof(cam->world_up));

    cam->yaw = YAW;
    cam->pitch = PITCH;
    cam->movement_speed = SPEED;
    cam->mouse_sensitivity = SENSITIVITY;
    cam->zoom = ZOOM;

    update_camera_vectors(cam);
}

void get_view_matrix(mat4x4 m, camera cam)
{
    vec3 center;
    vec3_add(center, cam.position, cam.front);
    mat4x4_look_at(m, cam.position, center, cam.up);
}

void camera_process_keyboard(camera *cam, enum Camera_Movement direction, float deltaTime)
{
    float velocity = cam->movement_speed * deltaTime;
    vec3 tmp;
    if (direction == FORWARD)
    {
        vec3_scale(tmp, cam->front, velocity);
        vec3_add(cam->position, cam->position, tmp);
    }

    if (direction == BACKWARD)
    {
        vec3_scale(tmp, cam->front, velocity);
        vec3_sub(cam->position, cam->position, tmp);
    }

    if (direction == LEFT)
    {
        vec3_scale(tmp, cam->right, velocity);
        vec3_sub(cam->position, cam->position, tmp);
    }

    if (direction == RIGHT)
    {
        vec3_scale(tmp, cam->right, velocity);
        vec3_add(cam->position, cam->position, tmp);
    }

    if (direction == UP)
    {
        vec3_scale(tmp, cam->world_up, velocity);
        vec3_add(cam->position, cam->position, tmp);
    }

    if (direction == DOWN)
    {
        vec3_scale(tmp, cam->world_up, velocity);
        vec3_sub(cam->position, cam->position, tmp);
    }
}

void process_mouse_movement_by_offset(
    camera *cam, float x_offset, float y_offset,
    GLboolean constrainPitch)
{
    x_offset *= cam->mouse_sensitivity;
    y_offset *= cam->mouse_sensitivity;

    cam->yaw += x_offset;
    cam->pitch += y_offset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (cam->pitch > 89.0f)
            cam->pitch = 89.0f;
        if (cam->pitch < -89.0f)
            cam->pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    update_camera_vectors(cam);
}

void process_mouse_movement(camera *cam, float yaw, float pitch, GLboolean constrainPitch)
{

    cam->yaw = yaw;
    cam->pitch = pitch;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (cam->pitch > 89.0f)
            cam->pitch = 89.0f;
        if (cam->pitch < -89.0f)
            cam->pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    update_camera_vectors(cam);
}

void process_mouse_scroll(camera *cam, float y_offset)
{
    cam->zoom -= (float)y_offset;
    if (cam->zoom < 1.0f)
        cam->zoom = 1.0f;
    if (cam->zoom > 45.0f)
        cam->zoom = 45.0f;
}

void update_camera_vectors(camera *cam)
{
    // calculate the new Front vector
    vec3 front;
    front[0] = cosf(radians(cam->yaw)) * cosf(radians(cam->pitch));
    front[1] = sinf(radians(cam->pitch));
    front[2] = sinf(radians(cam->yaw)) * cosf(radians(cam->pitch));

    vec3_norm(cam->front, front);

    // also re-calculate the Right and Up vector
    vec3 front_up;
    vec3_mul_cross(front_up, cam->front, cam->world_up);
    vec3_norm(cam->right, front_up); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.

    vec3 right_front;
    vec3_mul_cross(right_front, cam->right, cam->front);
    vec3_norm(cam->up, right_front);
}
