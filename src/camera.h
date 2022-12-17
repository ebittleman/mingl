#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "linmath.h"

struct Cam
{
    float cam_pos[3];
    float cam_look_at[3];

    float cam_speed;
    float cam_yaw_speed;
    float cam_yaw;
    mat4x4 T;
    mat4x4 R;
    mat4x4 view;
};

struct Cam new_cam();
void update_cam(struct Cam *cam);
void update_cam1(struct Cam *cam);
void handle_camera_events(
    GLFWwindow *window, double elapsed_seconds, struct Cam *cam);