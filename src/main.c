#include "background.h"
#include "c-lib/dynlist.h"
#include "c-lib/time.h"
#include "font.h"
#include "level.h"
#include "renderer/render.h"
#include "sprite_sheet/img.h"
#include "sprite_sheet/sprite_sheet.h"
#include "state.h"

int main(void) {
  state_init();

  state.quit = false;
  SDL_Event ev;

  const char* msg =
      "abcdefghijklmnopqrstuvwxyz"
      "\nABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "\n0123456789"
      "\n!@#$%^&*()_+="
      "\n,./<>?;':\"[]";

  while (!state.quit) {
    while (SDL_PollEvent(&ev)) {
      switch (ev.type) {
        case SDL_QUIT: state.quit = true; break;
      }
    }

    float t = 10.0f + (cosf(time_s()) * 10.0f);

    if ((t > 10.0f && state.current_level == 0) ||
        (t < 10.0f && state.current_level == 1)) {
      // set_level(&state, state.current_level == 0 ? 1 : 0);
    }

    fv2 a_pos = {.x = t, .y = t};
    fv2 b_pos = {.x = t, .y = a_pos.y + 8.0f};

    DYNLIST(sprite_t) batch = dynlist_create(sprite_t);

    push_background(&batch, &state.bg_sheet);
    push_level(&batch, state.levels[state.current_level]);
    push_font_ch(&batch, &state.font_sheet, 'A', a_pos);
    push_font_str(&batch, &state.font_sheet, msg, b_pos);

    render_begin();
    render_batch(&batch, true);
    render_end();
  }

  state_destroy();
}
