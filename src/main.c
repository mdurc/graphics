#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

int main(void) {
  render_init(SCREEN_WIDTH, SCREEN_HEIGHT);

  while (!glfwWindowShouldClose(global.renderer.window)) {
  }

  return 0;
}
