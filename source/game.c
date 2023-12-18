#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include <raylib.h>

#include "rand.h"
#include "level_generator.h"
#include "raymath.h"
#include "ui.h"
#include "utils.h"
#include "font.h"
#include "player.h"
#include "audio.h"
#include "config.h"

#include <stdio.h>

#define LEVELS_MAX 10

int main(void) {
  time_t seed = time(0);
  srand((unsigned int)seed);
  srand48(seed);

  printf("seed: %ld\n", seed);

  if (!try_load_config()) {
    default_config();
  }

  static Level levels[LEVELS_MAX] = {0};
  size_t current_level = 0;

  memset(levels, 0, sizeof(levels));

  Player player = {0};
  init_player(&player);

  generate_level(&levels[0], &player.location, LEVEL_SURFACE);

  /* TODO: https://www.squidi.net/three/entry.php?id=83 */

  init_rendering();
  init_audio();

  play_audio();

  bool is_cursor_enabled = false;

  size_t next_level = current_level;
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
                             2);

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

    if (next_level != current_level) {
      if (!levels[next_level].is_generated) {
        generate_level(&levels[next_level], &player.location, LEVEL_DUNGEON);
      } else {
        LevelTileType elevator =
          next_level < current_level
          ? TILE_ELEVATOR_DOWN
          : TILE_ELEVATOR_UP;

        for (size_t yi = 0; yi < LEVEL_HEIGHT; yi++) {
          for (size_t xi = 0; xi < LEVEL_WIDTH; xi++) {
            if (levels[next_level].map[yi][xi].type == elevator) {
              player.location.x = xi;
              player.location.y = yi;
              break;
            }
          }
        }

        current_level = next_level;
      }
    }

    process_player_movement(&player, &levels[current_level]);
    update_drill_position(&player);
    process_mouse(&player, &levels[current_level], &next_level, LEVELS_MAX);
    trace_rays_for_fov(player, &levels[current_level]);

    if (IsKeyDown(KEY_X)) {
      for (size_t yi = 0; yi < LEVEL_HEIGHT; yi++) {
        for (size_t xi = 0; xi < LEVEL_WIDTH; xi++) {
          levels[current_level].map[yi][xi].is_visible = true;
        }
      }
    }

    if (IsWindowResized()) {
      adjust_universe_to_the_window_size();
    }

    render(&levels[current_level], player);

    if (player.is_drilling) {
      play_drill();
    }
  }

  cleanup_rendering();

  CloseWindow();

  return 0;
}
