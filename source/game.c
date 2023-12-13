#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <math.h>

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

#define SCALE 5


#if 0
int main2(void) {
  time_t seed = time(0);
  srand((unsigned int)seed);
  srand48(seed);

  /* SetConfigFlags(FLAG_WINDOW_RESIZABLE); */
  InitWindow(LEVEL_WIDTH * SCALE, LEVEL_HEIGHT * SCALE, "dunjeneer");
  SetTargetFPS(60);

  Image noise = GenImagePerlinNoise(LEVEL_WIDTH, LEVEL_HEIGHT,
                                    0, 0,
                                    3.0f);

  LevelMap map;
  for (size_t yi = 0; yi < LEVEL_HEIGHT; yi++) {
    for (size_t xi = 0; xi < LEVEL_WIDTH; xi++) {
      uint8_t a = ((Color *)(noise.data))[(yi * LEVEL_WIDTH) + xi].r;

      LevelTile t = TILE_NONE;

      if (a < 5) {
        t = TILE_GROUND;
      } else if (a < 50) {
        t = TILE_GROUND;
      } else if (a < 120) {
        t = TILE_HILL;
      } else {
        t = TILE_MOUNTAIN;
      }
      map[yi][xi] = t;
    }
  }

  Texture2D t = LoadTextureFromImage(noise);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexturePro(t,
                   (Rectangle) {
                     .x = 0,
                     .y = 0,
                     .width = LEVEL_WIDTH,
                     .height = LEVEL_HEIGHT,
                   },
                   (Rectangle) {
                     .x = 0,
                     .y = 0,
                     .width = LEVEL_WIDTH * SCALE,
                     .height = LEVEL_HEIGHT * SCALE,
                   },
                   Vector2Zero(),
                   0, WHITE);
    EndDrawing();
  }

  UnloadTexture(t);
  UnloadImage(noise);
  return 0;
}
#endif

int main(void) {
  time_t seed = time(0);
  srand((unsigned int)seed);
  srand48(seed);

  if (!try_load_config()) {
    default_config();
  }

  static LevelMap map = {0};

  static LevelObject objects[OBJECTS_MAX] = {0};
  size_t objects_len = 0;

  Player player = {0};
  init_player(&player);

  generate_level(&map, objects, &objects_len, &player.location, LEVEL_SURFACE);

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

    trace_rays_for_fov(player, map);

    render(map, objects, objects_len, player);
  }

  cleanup_rendering();

  CloseWindow();

  return 0;
}
