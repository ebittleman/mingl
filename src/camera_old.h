#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "types.h"
#include "linmath.h"

typedef struct _Cam
{
    float cam_pos[3];
    float cam_look_at[3];

    float cam_speed;
    float cam_yaw_speed;
    float cam_yaw;
    mat4x4 T;
    mat4x4 R;
    mat4x4 view;
} Cam;

Cam new_cam();
void update_cam(Cam *cam);
void update_cam1(Cam *cam);
void handle_camera_events(
    GLFWwindow *window, double elapsed_seconds, Cam *cam);