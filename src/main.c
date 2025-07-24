#include <glad/glad.h>
#include <stdio.h>

#include "c-lib/misc.h"
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int main(void) {
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

  ASSERT(!SDL_Init(SDL_INIT_VIDEO), "failed SDL_Init: %s\n", SDL_GetError());

  SDL_Window* window = SDL_CreateWindow(
      "window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH,
      SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  ASSERT(window, "failed to create sdl window: %s\n", SDL_GetError());

  SDL_GL_CreateContext(window);
  ASSERT(gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress),
         "failed to load GL: %s\n", SDL_GetError());

  printf("OpenGL Loaded\n");
  printf("Vendor:   %s\n", glGetString(GL_VENDOR));
  printf("Renderer: %s\n", glGetString(GL_RENDERER));
  printf("Version:  %s\n", glGetString(GL_VERSION));

  return 0;
}
