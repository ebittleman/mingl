#ifndef CAMERA_H
#define CAMERA_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include "linmath.h"

#define RADS 0.01745329251994329576923690768489

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement
{
    FORWARD = 0,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
static const float YAW = -90.0f;
static const float PITCH = 0.0f;
static const float SPEED = 5.0f;
static const float SENSITIVITY = 0.1f;
static const float ZOOM = 45.0f;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL

static float radians(float degrees)
{
    return degrees * RADS;
}

typedef struct _camera
{
    // camera Attributes
    vec3 position;
    vec3 front;
    vec3 up;
    vec3 right;
    vec3 world_up;
    // euler Angles
    float yaw;
    float pitch;
    // camera options
    float movement_speed;
    float mouse_sensitivity;
    float zoom;

} camera;

void init_camera(camera *cam);
void get_view_matrix(mat4x4 m, camera cam);
void camera_process_keyboard(camera *cam, enum Camera_Movement direction, float deltaTime);
void process_mouse_movement(camera *cam, float x_offset, float y_offset, GLboolean constrainPitch);
void process_mouse_movement_by_offset(camera *cam, float x_offset, float y_offset, GLboolean constrainPitch);
void process_mouse_scroll(camera *cam, float y_offset);

#endif
