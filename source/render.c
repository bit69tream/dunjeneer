#include "render.h"
#include "font.h"
#include "level_generator.h"
#include "utils.h"
#include <assert.h>

#include <raylib.h>
#include <raymath.h>

#define X_TO_SCREEN(x, type) ((type)((x) * GLYPH_WIDTH) + (type)((GLYPH_GAP * (x)) + GLYPH_GAP))
#define Y_TO_SCREEN(x, type) ((type)((x) * GLYPH_HEIGHT) + (type)((GLYPH_GAP * (x)) + GLYPH_GAP))

static Camera2D camera;

static Texture2D font;

static RenderTexture2D world;

static Shader cursor_shader;
static int cursor_shader_resolution;
static int cursor_shader_mouse_cursor;

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

void init_rendering(void) {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  /* TODO: adapt ui to the window size, tell user that window is too small when it is */
  /* TODO: calculate the minimal window size when the ui is ready */
  InitWindow(800, 600, "dunjeneer");
  SetExitKey(KEY_NULL);

  HideCursor();
  DisableCursor();

  SetTargetFPS(60);

  font = font_texture();

  camera.rotation = 0;
  camera.zoom = 6;

  world = LoadRenderTexture(X_TO_SCREEN(LEVEL_WIDTH, int),
                            Y_TO_SCREEN(LEVEL_HEIGHT, int));

  cursor_shader = LoadShaderFromMemory(NULL, cursor_shader_source);
  assert(IsShaderReady(cursor_shader));

  cursor_shader_resolution = GetShaderLocation(cursor_shader, "resolution");

  Vector2 resolution = {
    .x = (float)world.texture.width,
    .y = (float)world.texture.height,
  };
  SetShaderValue(cursor_shader, cursor_shader_resolution, &resolution, SHADER_UNIFORM_VEC2);

  cursor_shader_mouse_cursor = GetShaderLocation(cursor_shader, "mouse_cursor");
}

void cleanup_rendering(void) {
  EnableCursor();
  UnloadShader(cursor_shader);
  UnloadRenderTexture(world);
  UnloadTexture(font);
}

Color tile_to_color(LevelTile tile) {
  switch (tile) {
  case TILE_NONE: return BLANK;
  case TILE_WALL: return WHITE;
  case TILE_FLOOR: return DARKGRAY;
  case TILE_VERTICAL_CLOSED_DOOR: return BROWN;
  case TILE_HORIZONTAL_CLOSED_DOOR: return BROWN;
  case TILE_VERTICAL_OPENED_DOOR: return BROWN;
  case TILE_HORIZONTAL_OPENED_DOOR: return BROWN;
  case LEVEL_TILE_COUNT: assert(false && "nuh uh");
  };

  assert(false && "unknown tile");
}

Color object_type_to_color(LevelObjectType type) {
  switch (type) {
  case OBJECT_NONE: return BLANK;
  case OBJECT_ELEVATOR_DOWN: return WHITE;
  case OBJECT_ELEVATOR_UP: return WHITE;
  case LEVEL_OBJECT_COUNT: assert(false && "nuh uh");
  }

  assert(false && "unknown object type");
}

void render(LevelMap map,
            LevelObject objects[OBJECTS_MAX], size_t objects_len,
            Player player) {
  float player_screen_x = X_TO_SCREEN(player.location.x, float) + (GLYPH_WIDTH / 2.0f);
  float player_screen_y = Y_TO_SCREEN(player.location.y, float) + (GLYPH_HEIGHT / 2.0f);

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
                       tile_to_color(tile));
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
                     object_type_to_color(objects[i].type));
    }

    DrawRectanglePro((Rectangle) {
        .x = (X_TO_SCREEN(player.location.x, float) - GLYPH_GAP) + (float)player.location_offset.x,
        .y = (Y_TO_SCREEN(player.location.y, float) - GLYPH_GAP) + (float)player.location_offset.y,
        .width = (GLYPH_WIDTH + (GLYPH_GAP * 2)),
        .height = (GLYPH_HEIGHT + (GLYPH_GAP * 2)),
      },
      (Vector2) {0, 0},
      0,
      GREEN);

    DrawTexturePro(font,
                   glyphs['@'],
                   (Rectangle) {
                     .x = X_TO_SCREEN(player.location.x, float) + (float)player.location_offset.x,
                     .y = Y_TO_SCREEN(player.location.y, float) + (float)player.location_offset.y,
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
      Point mouse = mouse_in_world();

      Vector4 mouse_cursor = {
        .x = X_TO_SCREEN(mouse.x, float) - GLYPH_GAP,
        .y = (Y_TO_SCREEN(mouse.y, float) - GLYPH_GAP),
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

static Vector2 mouse_position = {0};

void update_mouse(void) {
  mouse_position = Vector2Add(GetMouseDelta(), mouse_position);
  mouse_position = Vector2Clamp(mouse_position, (Vector2) {0, 0}, (Vector2) {(float)GetScreenWidth(), (float)GetScreenHeight()});
}

Point mouse_in_world(void) {
  Vector2 mouse_on_screen = GetScreenToWorld2D(mouse_position, camera);

  return (Point) {
    .x = (size_t)((mouse_on_screen.x) / (GLYPH_WIDTH + 1)),
    .y = (size_t)((mouse_on_screen.y) / (GLYPH_HEIGHT + 1)),
  };
}
