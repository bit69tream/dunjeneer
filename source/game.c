#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <math.h>

#include <raylib.h>

#include "rand.h"
#include "level_generator.h"
#include "ui.h"
#include "utils.h"
#include "font.h"
#include "player.h"
#include "audio.h"

int main(void) {
  time_t seed = time(0);
  srand((unsigned int)seed);
  srand48(seed);

  static LevelMap map = {0};

  static LevelObject objects[OBJECTS_MAX] = {0};
  size_t objects_len = 0;

  Player player = {0};
  init_player(&player);

  generate_level(&map, objects, &objects_len, &player.location);

  /* TODO: https://www.squidi.net/three/entry.php?id=83 */

  init_rendering();
  init_audio();

  play_audio();

  bool is_cursor_enabled = false;
  while (!WindowShouldClose()) {
    if (!is_window_big_enough()) {
      if (!is_cursor_enabled) {
        EnableCursor();
        is_cursor_enabled = true;
      }

      BeginDrawing(); {
        ClearBackground(BLACK);

        render_text_centered("window is too small",
                             (Vector2) {
                               .x = (float)(GetScreenWidth()) / 2.0f,
                               .y = (float)(GetScreenHeight()) / 2.0f,
                             },
                             WHITE,
                             4);

      } EndDrawing();
      continue;
    }

    if (is_cursor_enabled &&
        (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ||
         IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))) {
      DisableCursor();
      is_cursor_enabled = false;
    }

    if (!is_cursor_enabled) {
      update_mouse();
    }

    process_player_movement(&player, map);

    process_mouse(&player, &map);

    render(map, objects, objects_len, player);
  }

  cleanup_rendering();

  CloseWindow();

  return 0;
}
