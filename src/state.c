#include "state.h"

#include "level.h"
#include "renderer/render.h"

state_t state;

void state_init(void) {
  state.window = NULL;
  state.renderer = NULL;
  state.quit = false;
  state.current_level = 0, state.loaded_levels = 0;

  render_init(SCREEN_WIDTH, SCREEN_HEIGHT);
  sprite_sheet_init(&state.font_sheet, "res/font.png", 8, 8, 3.0f);
  sprite_sheet_init(&state.bg_sheet, "res/bg.png", 8, 8, 4.0f);

  level_init_all();
  level_set_current(0);
}

void state_destroy(void) {
  sprite_sheet_destroy(&state.font_sheet);
  sprite_sheet_destroy(&state.bg_sheet);

  for (int i = 0; i < state.loaded_levels; ++i) {
    level_destroy(state.levels[i]);
  }

  SDL_DestroyRenderer(state.renderer);
  SDL_DestroyWindow(state.window);
  SDL_Quit();
}
