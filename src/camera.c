#include <stdbool.h>
#include <stdio.h>

#include "camera.h"

Cam new_cam()
{
    Cam cam;

    // cam.cam_pos[0] = -1.1f;
    // cam.cam_pos[1] = 1.62f;
    // cam.cam_pos[2] = 4.72f;
    // cam.cam_pos[0] = -75.0f;
    // cam.cam_pos[1] = 121.0f;
    // cam.cam_pos[2] = -100.0f;
    cam.cam_pos[0] = 0.0f;
    cam.cam_pos[1] = 0.0f;
    cam.cam_pos[2] = 5.0f;

    cam.cam_look_at[0] = 0.0f;
    cam.cam_look_at[1] = 0.0f;
    cam.cam_look_at[2] = -1.0f;

    cam.cam_speed = 1.0f;
    cam.cam_yaw_speed = 5.0f;
    cam.cam_yaw = 0.0f;

    update_cam(&cam);

    return cam;
}

void update_cam(Cam *cam)
{
    static const vec3 up = {0.0f, 1.0f, 0.0};
    // mat4x4_translate(cam->T, -cam->cam_pos[0], -cam->cam_pos[1], -cam->cam_pos[2]);
    // mat4x4_identity(cam->R);
    // mat4x4_rotate_Y(cam->R, cam->R, -cam->cam_yaw);
    // mat4x4_mul(cam->view, cam->R, cam->T);
    vec3 eye;
    vec3_add(eye, cam->cam_pos, cam->cam_look_at);
    mat4x4_look_at(cam->view, cam->cam_pos, eye, up);
}

void handle_camera_events(
    GLFWwindow *window, double elapsed_seconds, Cam *cam)
{
    bool cam_moved = false;
    if (glfwGetKey(window, GLFW_KEY_A))
    {
        cam->cam_pos[0] -= cam->cam_speed * elapsed_seconds;
        cam_moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D))
    {
        cam->cam_pos[0] += cam->cam_speed * elapsed_seconds;
        cam_moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP))
    {
        cam->cam_pos[1] += cam->cam_speed * elapsed_seconds;
        cam_moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN))
    {
        cam->cam_pos[1] -= cam->cam_speed * elapsed_seconds;
        cam_moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_W))
    {
        cam->cam_pos[2] -= cam->cam_speed * elapsed_seconds;
        cam_moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S))
    {
        cam->cam_pos[2] += cam->cam_speed * elapsed_seconds;
        cam_moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT))
    {
        cam->cam_yaw += cam->cam_yaw_speed * elapsed_seconds;
        cam->cam_look_at[0] += cam->cam_yaw_speed * elapsed_seconds;
        cam_moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT))
    {
        cam->cam_yaw -= cam->cam_yaw_speed * elapsed_seconds;
        cam->cam_look_at[0] -= cam->cam_yaw_speed * elapsed_seconds;
        cam_moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_UP))
    {
        // cam->cam_yaw += cam->cam_yaw_speed * elapsed_seconds;
        cam->cam_look_at[1] += cam->cam_yaw_speed * elapsed_seconds;
        cam_moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN))
    {
        // cam->cam_yaw -= cam->cam_yaw_speed * elapsed_seconds;
        cam->cam_look_at[1] -= cam->cam_yaw_speed * elapsed_seconds;
        cam_moved = true;
    }

    if (cam_moved)
    {
        update_cam(cam);
        // printf("Cam Yaw: %f, %f, %f\n", cam->cam_yaw);
        // debug_mat(cam->view);

        // printf("Cam Position: %f, %f, %f\n",
        //        cam->cam_pos[0],
        //        cam->cam_pos[1],
        //        cam->cam_pos[2]);
    }
}
