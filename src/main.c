#define DEMOS_BASIC
#include "demos/basic.c"

int main(void)
{
    GLFWwindow *window = init_opengl(&init);
    start(window, update, shaders, mingl_basic_scenes);
    exit(EXIT_SUCCESS);
}
