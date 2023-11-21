#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <math.h>

#include <raylib.h>

#include "rand.h"
#include "level_generator.h"
#include "utils.h"
#include "font.h"

static const char *cursor_shader_source =
  "#version 330\n\n"

  // Input vertex attributes (from vertex shader)
  "in vec2 fragTexCoord;\n"
  "in vec4 fragColor;\n"

  // Input uniform values
  "uniform sampler2D texture0;\n"
  "uniform vec4 colDiffuse;\n"

  // Output fragment color
  "out vec4 finalColor;\n"

  // custom variables
  "uniform vec2 resolution;\n"
  "uniform vec4 mouse_cursor;\n"

  "void main() {\n"
  "    vec4 texelColor = texture(texture0, fragTexCoord)*colDiffuse*fragColor;\n"
  "    vec2 pixelCoord = vec2(fragTexCoord.x * resolution.x, (1.0 - fragTexCoord.y) * resolution.y);\n"
  "    if ((pixelCoord.x >= mouse_cursor.x) && \n"
  "        (pixelCoord.y >= mouse_cursor.y) && \n"
  "        (pixelCoord.x < (mouse_cursor.x + mouse_cursor.z)) && \n"
  "        (pixelCoord.y < (mouse_cursor.y + mouse_cursor.w))) {\n"
  "        finalColor = vec4(1.0 - texelColor.r, 1.0 - texelColor.g, 1.0 - texelColor.b, 1.0);\n"
  "    } else { \n"
  "        finalColor = texelColor;\n"
  "    }\n"
  "}\n";

int main(void) {
  srand((unsigned int)time(0));
  /* srand(1); */

  static LevelMap map = {0};

  static LevelObject objects[OBJECTS_MAX] = {0};
  size_t objects_len = 0;

  Point player_location = {0};

  generate_level(&map, objects, &objects_len, &player_location);

  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  /* TODO: adapt ui to the window size, tell user that window is too small when it is */
  /* TODO: calculate the minimal window size when the ui is ready */
  InitWindow(800, 600, "dunjeneer");
  SetExitKey(KEY_NULL);

  HideCursor();

  SetTargetFPS(60);

  Texture2D font = font_texture();

  Color tile_colors[] = {
    [TILE_NONE] = BLANK,
    [TILE_WALL] = WHITE,
    [TILE_FLOOR] = DARKGRAY,
    [TILE_VERTICAL_CLOSED_DOOR] = BROWN,
    [TILE_HORIZONTAL_CLOSED_DOOR] = BROWN,
  };

  static_assert(SIZE_OF(tile_colors) == LEVEL_TILE_COUNT);

  Color object_colors[] = {
    [OBJECT_NONE] = BLANK,
    [OBJECT_ELEVATOR_DOWN] = WHITE,
    [OBJECT_ELEVATOR_UP] = WHITE,
  };

  static_assert(SIZE_OF(object_colors) == LEVEL_OBJECT_COUNT);

#define X_TO_SCREEN(x, type) ((type)((x) * GLYPH_WIDTH) + (type)((GLYPH_GAP * (x)) + GLYPH_GAP))
#define Y_TO_SCREEN(x, type) ((type)((x) * GLYPH_HEIGHT) + (type)((GLYPH_GAP * (x)) + GLYPH_GAP))

  Camera2D camera = {0};

  camera.rotation = 0;
  camera.zoom = 6;

  camera.target.x = X_TO_SCREEN(player_location.x, float) + (GLYPH_WIDTH / 2.0f);
  camera.target.y = Y_TO_SCREEN(player_location.y, float) + (GLYPH_HEIGHT / 2.0f);

  /* TODO: https://www.squidi.net/three/entry.php?id=83 */

  RenderTexture2D world = LoadRenderTexture(X_TO_SCREEN(LEVEL_WIDTH, int),
                                            Y_TO_SCREEN(LEVEL_HEIGHT, int));

  Shader cursor_shader = LoadShaderFromMemory(NULL, cursor_shader_source);
  assert(IsShaderReady(cursor_shader));

  int cursor_shader_resolution = GetShaderLocation(cursor_shader, "resolution");
  static Vector2 resolution = {0};
  resolution.x = (float)world.texture.width;
  resolution.y = (float)world.texture.height;
  SetShaderValue(cursor_shader, cursor_shader_resolution, &resolution, SHADER_UNIFORM_VEC2);

  int cursor_shader_mouse_cursor = GetShaderLocation(cursor_shader, "mouse_cursor");

  enum {
    DIRECTION_NONE = 0,
    DIRECTION_UP = 1 << 0,
    DIRECTION_LEFT = 1 << 1,
    DIRECTION_DOWN = 1 << 2,
    DIRECTION_RIGHT = 1 << 3,
  } player_direction = DIRECTION_NONE;

  ssize_t player_x_delta = 0;
  ssize_t player_y_delta = 0;

  while (!WindowShouldClose()) {
    if (IsKeyDown(KEY_E)) {
      if ((player_direction == DIRECTION_NONE ||
           player_direction == DIRECTION_LEFT ||
           player_direction == DIRECTION_RIGHT) &&
          map[player_location.y - 1][player_location.x] == TILE_FLOOR) {
        player_direction |= DIRECTION_UP;
        player_y_delta = (GLYPH_HEIGHT + GLYPH_GAP);
        player_location.y -= 1;
      }
    }

    if (IsKeyDown(KEY_S)) {
      if ((player_direction == DIRECTION_NONE ||
           player_direction == DIRECTION_DOWN ||
           player_direction == DIRECTION_UP) &&
          map[player_location.y][player_location.x - 1] == TILE_FLOOR) {
        player_direction |= DIRECTION_LEFT;
        player_x_delta = (GLYPH_WIDTH + GLYPH_GAP);
        player_location.x -= 1;
      }
    }

    if (IsKeyDown(KEY_D)) {
      if ((player_direction == DIRECTION_NONE ||
           player_direction == DIRECTION_LEFT ||
           player_direction == DIRECTION_RIGHT) &&
          map[player_location.y + 1][player_location.x] == TILE_FLOOR) {
        player_direction |= DIRECTION_DOWN;
        player_y_delta = -(GLYPH_HEIGHT + GLYPH_GAP);
        player_location.y += 1;
      }
    }

    if (IsKeyDown(KEY_F)) {
      if ((player_direction == DIRECTION_NONE ||
           player_direction == DIRECTION_UP ||
           player_direction == DIRECTION_DOWN) &&
          map[player_location.y][player_location.x + 1] == TILE_FLOOR) {
        player_direction |= DIRECTION_RIGHT;
        player_x_delta = -(GLYPH_WIDTH + GLYPH_GAP);
        player_location.x += 1;
      }
    }

    printf("%ld %ld\n", player_x_delta, player_y_delta);

    if (player_direction != DIRECTION_NONE) {
      if (player_x_delta == 0) {
        player_direction = player_direction & (unsigned int)(~DIRECTION_LEFT);
        player_direction = player_direction & (unsigned int)(~DIRECTION_RIGHT);
      }

      if (player_y_delta == 0) {
        player_direction = player_direction & (unsigned int)(~DIRECTION_UP);
        player_direction = player_direction & (unsigned int)(~DIRECTION_DOWN);
      }

      if (player_direction & DIRECTION_LEFT ||
          player_direction & DIRECTION_RIGHT) {
        if (player_x_delta < 0) {
          player_x_delta += 1;
        } else {
          player_x_delta -= 1;
        }
      }

      if (player_direction & DIRECTION_UP ||
          player_direction & DIRECTION_DOWN) {
        if (player_y_delta < 0) {
          player_y_delta += 1;
        } else {
          player_y_delta -= 1;
        }
      }
    }

    player_location.x = CLAMP(1, LEVEL_WIDTH - 2, player_location.x);
    player_location.y = CLAMP(1, LEVEL_HEIGHT - 2, player_location.y);

    float player_screen_x = X_TO_SCREEN(player_location.x, float) + (GLYPH_WIDTH / 2.0f);
    float player_screen_y = Y_TO_SCREEN(player_location.y, float) + (GLYPH_HEIGHT / 2.0f);

    camera.target.x = LERP(camera.target.x, player_screen_x, 0.1f);
    camera.target.y = LERP(camera.target.y, player_screen_y, 0.1f);

    camera.offset.x = (float)GetScreenWidth() / 2.0f;
    camera.offset.y = (float)GetScreenHeight() / 2.0f;

    BeginTextureMode(world); {
      ClearBackground(BLACK);

      for (size_t y = 0; y < LEVEL_HEIGHT; y++) {
        for (size_t x = 0; x < LEVEL_WIDTH; x++) {
          LevelTile tile = map[y][x];

          DrawTexturePro(font,
                         glyphs[tile_to_glyph(tile)],
                         (Rectangle) {
                           .x = X_TO_SCREEN(x, float),
                           .y = Y_TO_SCREEN(y, float),
                           .width = GLYPH_WIDTH,
                           .height = GLYPH_HEIGHT,
                         },
                         (Vector2) {0, 0},
                         0,
                         tile_colors[tile]);
        }
      }

      for (size_t i = 0; i < objects_len; i++) {
        DrawRectangle(X_TO_SCREEN(objects[i].location.x, int),
                      Y_TO_SCREEN(objects[i].location.y, int),
                      GLYPH_WIDTH,
                      GLYPH_HEIGHT,
                      BLACK);

        DrawTexturePro(font,
                       glyphs[object_type_to_glyph(objects[i].type)],
                       (Rectangle) {
                         .x = X_TO_SCREEN(objects[i].location.x, float),
                         .y = Y_TO_SCREEN(objects[i].location.y, float),
                         .width = GLYPH_WIDTH,
                         .height = GLYPH_HEIGHT,
                       },
                       (Vector2) {0, 0},
                       0,
                       object_colors[objects[i].type]);
      }

      DrawRectanglePro((Rectangle) {
          .x = (X_TO_SCREEN(player_location.x, float) - GLYPH_GAP) + (float)player_x_delta,
          .y = (Y_TO_SCREEN(player_location.y, float) - GLYPH_GAP) + (float)player_y_delta,
          .width = (GLYPH_WIDTH + (GLYPH_GAP * 2)),
          .height = (GLYPH_HEIGHT + (GLYPH_GAP * 2)),
        },
        (Vector2) {0, 0},
        0,
        GREEN);

      DrawTexturePro(font,
                     glyphs['@'],
                     (Rectangle) {
                       .x = X_TO_SCREEN(player_location.x, float) + (float)player_x_delta,
                       .y = Y_TO_SCREEN(player_location.y, float) + (float)player_y_delta,
                       .width = GLYPH_WIDTH,
                       .height = GLYPH_HEIGHT,
                     },
                     (Vector2) {0, 0},
                     0,
                     BLACK);
    } EndTextureMode();

    BeginDrawing(); {
      ClearBackground(BLACK);

      BeginMode2D(camera); {
        Vector2 mouse_on_screen = GetScreenToWorld2D(GetMousePosition(), camera);

        Point mouse_in_world = {
          .x = (size_t)((mouse_on_screen.x) / (GLYPH_WIDTH + 1)),
          .y = (size_t)((mouse_on_screen.y) / (GLYPH_HEIGHT + 1)),
        };

        Vector4 mouse_cursor = {
          .x = X_TO_SCREEN(mouse_in_world.x, float) - GLYPH_GAP,
          .y = (Y_TO_SCREEN(mouse_in_world.y, float) - GLYPH_GAP),
          .z = GLYPH_WIDTH + (GLYPH_GAP * 2),
          .w = GLYPH_HEIGHT + (GLYPH_GAP * 2),
        };

        SetShaderValue(cursor_shader, cursor_shader_mouse_cursor,
                       &mouse_cursor, SHADER_UNIFORM_VEC4);

        BeginShaderMode(cursor_shader); {
          DrawTextureRec(world.texture,
                         (Rectangle) {
                           .x = 0,
                           .y = 0,
                           .width = (float)world.texture.width,
                           .height = (float)-world.texture.height,
                         },
                         (Vector2) {0, 0},
                         WHITE);
        } EndShaderMode();

      } EndMode2D();
    } EndDrawing();
  }

  UnloadShader(cursor_shader);
  UnloadRenderTexture(world);
  UnloadTexture(font);

  CloseWindow();

  return 0;
}
