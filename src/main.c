#define FIRST_DEMO
#include "demos/first_demo.c"
// #define DEMOS_BASIC
// #include "demos/basic.c"

int main(void)
{
     GLFWwindow *window = init_opengl(&init);
     start(window, update, shaders, mingl_first_scenes);
     exit(EXIT_SUCCESS);
 }

// int main(void)
// {
//     run();
// }
