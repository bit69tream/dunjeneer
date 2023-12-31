#include "rand.h"
#include <bits/time.h>
#include <stddef.h>
#include <time.h>
#include <stdint.h>
#include <sys/types.h>

#include "ui.h"
#include "font.h"
#include "level_generator.h"
#include "player.h"
#include "utils.h"
#include "config.h"

#include <assert.h>
#include <string.h>

#include <stdio.h>

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#define X_TO_SCREEN(x, type) ((type)((x) * GLYPH_WIDTH) + (type)((GLYPH_GAP * (x)) + GLYPH_GAP))
#define Y_TO_SCREEN(x, type) ((type)((x) * GLYPH_HEIGHT) + (type)((GLYPH_GAP * (x)) + GLYPH_GAP))

static Camera2D camera;

static Texture2D font;

static RenderTexture2D world;
static RenderTexture2D world_with_cursor;
static RenderTexture2D universe;

static Shader cursor_shader;
static int cursor_shader_mouse_cursor;

static Shader noise_shader;
static int noise_shader_time;

static float shader_time;

static Shader fisheye_shader;
static int fisheye_shader_curvature;

static Shader scanline_shader;

static const char *scanline_shader_source =
  "#version 330\n"

  "in vec2 fragTexCoord;\n"
  "in vec4 fragColor;\n"

  "out vec4 finalColor;\n"

  "uniform sampler2D texture0;\n"
  "uniform vec4 colDiffuse;\n"

  "uniform vec2 resolution;\n"

  "void main() {\n"
  "    vec4 color = texture(texture0, fragTexCoord) * colDiffuse * fragColor;\n"

  "    float a = fragTexCoord.y * resolution.y * 2.0;\n"
  "    color.g *= (sin(a) + 1.0) * 0.15 + 1.0;\n"
  "    color.rb *= (cos(a) + 1.0) * 0.135 + 1.0;\n"

  "    finalColor = color;\n"
  "}\n";

static const char *fisheye_shader_source =
  "#version 330\n"

  "in vec2 fragTexCoord;\n"

  "out vec4 finalColor;\n"

  "uniform sampler2D texture0;\n"
  "uniform vec4 colDiffuse;\n"

  "uniform float curvature;\n"

  "void main() {\n"
  "    vec2 uv = fragTexCoord * 2.0 - 1.0;\n"
  "    vec2 offset = uv.yx / curvature;\n"
  "    uv = uv + uv * offset * offset;\n"
  "    uv = uv * 0.5f + 0.5f;\n"

  "    vec4 color = texture(texture0, uv);\n"
  "    if (uv.x <= 0.0 || 1.0 <= uv.x || uv.y <= 0.0 || 1.0 <= uv.y) {\n"
  "        color = vec4(0);\n"
  "    }\n"

  "    finalColor = color;\n"
  "}\n";

static const char *noise_shader_source =
  "#version 330\n\n"

  "in vec2 fragTexCoord;\n\n"

  "uniform vec2 resolution;\n"
  "uniform float time;\n"

  "out vec4 finalColor;\n"

  "float rand(vec2 co) {\n"
  "    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);\n"
  "}\n\n"

  "void main() {\n"
  "    vec2 pixelCoord = vec2(fragTexCoord.x * resolution.x, (1.0 - fragTexCoord.y) * resolution.y);\n"
  "    float color = 1.0;\n"
  "    if (rand(pixelCoord * time) < 0.9) {\n"
  "        color = 0.0;\n"
  "    }\n"
  "    finalColor = vec4(vec3(color), 0.2);\n"
  "}\n"
  ;


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
  "}\n"
  ;

UIState ui_state;

Vector2I action_menu_offsets[ACTION_COUNT] = {
  [ACTION_NONE] = {0, 0},

  [ACTION_OPEN] = {-1, -1},
  [ACTION_CLOSE] = {+1, -1},

  [ACTION_PICK_UP] = {-1, 1},
  [ACTION_CLIMB] = {1, 1},
};
static_assert(SIZE_OF(action_menu_offsets) == ACTION_COUNT);

void init_rendering(void) {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(800, 600, "dunjeneer");
  SetExitKey(KEY_NULL);

  HideCursor();
  DisableCursor();

  SetTargetFPS(60);

  font = font_texture();

  camera.rotation = 0;
  camera.zoom = 6;

  universe = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

  world = LoadRenderTexture(X_TO_SCREEN(LEVEL_WIDTH, int),
                            Y_TO_SCREEN(LEVEL_HEIGHT, int));

  world_with_cursor = LoadRenderTexture(X_TO_SCREEN(LEVEL_WIDTH, int),
                             Y_TO_SCREEN(LEVEL_HEIGHT, int));

  cursor_shader = LoadShaderFromMemory(NULL, cursor_shader_source);
  assert(IsShaderReady(cursor_shader));

  Vector2 resolution = {
    .x = (float)world.texture.width,
    .y = (float)world.texture.height,
  };
  SetShaderValue(cursor_shader,
                 GetShaderLocation(cursor_shader, "resolution"),
                 &resolution,
                 SHADER_UNIFORM_VEC2);

  cursor_shader_mouse_cursor = GetShaderLocation(cursor_shader, "mouse_cursor");

  noise_shader = LoadShaderFromMemory(NULL, noise_shader_source);
  assert(IsShaderReady(noise_shader));

  Vector2 world_resolution = {
    .x = (float)LEVEL_WIDTH,
    .y = (float)LEVEL_HEIGHT
  };
  SetShaderValue(noise_shader,
                 GetShaderLocation(noise_shader, "resolution"),
                 &world_resolution,
                 SHADER_UNIFORM_VEC2);

  noise_shader_time = GetShaderLocation(noise_shader, "time");

  fisheye_shader = LoadShaderFromMemory(NULL, fisheye_shader_source);
  fisheye_shader_curvature = GetShaderLocation(fisheye_shader, "curvature");

  scanline_shader = LoadShaderFromMemory(NULL, scanline_shader_source);

  SetShaderValue(scanline_shader,
                 GetShaderLocation(scanline_shader, "resolution"),
                 &resolution,
                 SHADER_UNIFORM_VEC2);

  shader_time = 0;

  ui_state.type = UI_STATE_NONE;
}

void adjust_universe_to_the_window_size(void) {
  UnloadRenderTexture(universe);

  universe = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
}

void cleanup_rendering(void) {
  EnableCursor();
  UnloadShader(fisheye_shader);
  UnloadShader(cursor_shader);
  UnloadShader(noise_shader);
  UnloadRenderTexture(world);
  UnloadRenderTexture(world_with_cursor);
  UnloadRenderTexture(universe);
  UnloadTexture(font);
}

Color tile_to_background(LevelTileType tile) {
  switch (tile) {
  case TILE_NONE:
  case TILE_WALL:
  case TILE_FLOOR:

  case TILE_ELEVATOR_DOWN:
  case TILE_ELEVATOR_UP:

  case TILE_VERTICAL_CLOSED_DOOR:
  case TILE_HORIZONTAL_CLOSED_DOOR:
  case TILE_VERTICAL_LOCKED_DOOR:
  case TILE_HORIZONTAL_LOCKED_DOOR:
  case TILE_VERTICAL_OPENED_DOOR:
  case TILE_HORIZONTAL_OPENED_DOOR:

  case TILE_GROUND:
  case TILE_HILL:
  case TILE_ROCK:
  case TILE_GREY_ROCK:
  case TILE_HARD_ROCK:

  case TILE_ROCK_GRAVEL:
  case TILE_GREY_ROCK_GRAVEL:

    return BLACK;

  case LEVEL_TILE_COUNT: assert(false && "nuh uh");
  }

  assert(false);
}

Color tile_to_color(LevelTileType tile) {
  switch (tile) {
  case TILE_NONE: return BLANK;
  case TILE_WALL: return WHITE;
  case TILE_FLOOR: return DARKGRAY;

  case TILE_ELEVATOR_UP: return WHITE;
  case TILE_ELEVATOR_DOWN: return WHITE;

  case TILE_VERTICAL_CLOSED_DOOR:
  case TILE_HORIZONTAL_CLOSED_DOOR:
  case TILE_VERTICAL_LOCKED_DOOR:
  case TILE_HORIZONTAL_LOCKED_DOOR:
  case TILE_VERTICAL_OPENED_DOOR:
  case TILE_HORIZONTAL_OPENED_DOOR: return BROWN;

  case TILE_GROUND: return BROWN;
  case TILE_HILL: return DARKBROWN;

  case TILE_ROCK_GRAVEL: return CLITERAL(Color) {102 - 10, 57 - 10, 49 - 10, 255};
  case TILE_ROCK: return CLITERAL(Color) {102, 57, 49, 255};

  case TILE_HARD_ROCK: return CLITERAL(Color) {102 - 30, 57 - 10, 49 - 10, 255};

  case TILE_GREY_ROCK_GRAVEL: return DARKGRAY;
  case TILE_GREY_ROCK: return GRAY;

  case LEVEL_TILE_COUNT: assert(false && "nuh uh");
  };

  assert(false && "unknown tile");
}

Color object_type_to_color(LevelObjectType type) {
  switch (type) {
  case OBJECT_NONE: return BLANK;
  case LEVEL_OBJECT_COUNT: assert(false && "nuh uh");
  }

  assert(false && "unknown object type");
}

void render_noise(int width, int height) {
  SetShaderValue(noise_shader,
                 noise_shader_time,
                 &shader_time,
                 SHADER_UNIFORM_FLOAT);

  BeginShaderMode(noise_shader); {
    DrawRectangle(0, 0,
                  width, height,
                  BLACK);
  } EndShaderMode();
}

typedef struct {
  float lifetime_secs;
  Message_Buf buffer;
  time_t spawntime_secs;
  ssize_t spawntime_nsecs;
} Message;

#define MESSAGE_STARTING_LIFETIME 3.0f

#define MESSAGES_MAX 10

static Message messages[MESSAGES_MAX];

void init_messages(void) {
  memset(messages, 0, sizeof(messages));
}

bool time_gt(time_t a_secs, ssize_t a_nsecs,
             time_t b_secs, ssize_t b_nsecs) {
  if (a_secs == b_secs) {
    return a_nsecs > b_nsecs;
  }

  return (a_secs > b_secs);
}

bool time_lt(time_t a_secs, ssize_t a_nsecs,
             time_t b_secs, ssize_t b_nsecs) {
  if (a_secs == b_secs) {
    return a_nsecs < b_nsecs;
  }

  return (a_secs < b_secs);
}

int message_comparator(const void *aa, const void *bb) {
  Message *a = (Message *)aa;
  Message *b = (Message *)bb;

  return time_gt(a->spawntime_secs, a->spawntime_nsecs,
                 b->spawntime_secs, b->spawntime_nsecs);
}

Message_Buf *push_message(void) {
  ssize_t new_message_index = -1;

  for (ssize_t i = 0; i < MESSAGES_MAX; i++) {
    if (messages[i].lifetime_secs <= 0.0f) {
      new_message_index = i;
      break;
    }
  }

  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);

  if (new_message_index == -1) {
    /* we need to find the oldest message and use it */

    time_t old_secs = messages[0].spawntime_secs;
    time_t old_nsecs = messages[0].spawntime_nsecs;
    new_message_index = 0;

    for (ssize_t i = 1; i < MESSAGES_MAX; i++) {
      if (time_gt(old_secs, old_nsecs,
                  messages[i].spawntime_secs, messages[i].spawntime_nsecs)) {
        old_secs = messages[i].spawntime_secs;
        old_nsecs = messages[i].spawntime_nsecs;
        new_message_index = i;
      }
    }
  }

  if (new_message_index == -1) {
    assert(false && "not sure how is this possible");
  }

  messages[new_message_index].spawntime_secs = now.tv_sec;
  messages[new_message_index].spawntime_nsecs = now.tv_nsec;
  messages[new_message_index].lifetime_secs = MESSAGE_STARTING_LIFETIME;
  messages[new_message_index].buffer[0] = 0;

  Message_Buf *o = &messages[new_message_index].buffer;

  return o;
}

void update_messages() {
  float dt = GetFrameTime();

  qsort(messages, MESSAGES_MAX, sizeof(Message),
        message_comparator);

  for (ssize_t i = 0; i < MESSAGES_MAX; i++) {
    if (strlen(messages[i].buffer) == 0) {
      messages[i].lifetime_secs = 0;
    }

    if (messages[i].lifetime_secs > 0.0f) {
      messages[i].lifetime_secs -= dt;
    }
  }
}

void render_map(const Level *map) {
  for (size_t y = 0; y < LEVEL_HEIGHT; y++) {
    for (size_t x = 0; x < LEVEL_WIDTH; x++) {
      if (!map->map[y][x].is_visible) {
        continue;
      }

      LevelTile tile = map->map[y][x];

      if (tile.type == TILE_NONE) {
        continue;
      }

      Rectangle position = {
        .x = X_TO_SCREEN(x, float),
        .y = Y_TO_SCREEN(y, float),
        .width = GLYPH_WIDTH,
        .height = GLYPH_HEIGHT,
      };

      Rectangle bigger_position = {
        .x = position.x - GLYPH_GAP,
        .y = position.y - GLYPH_GAP,
        .width = position.width + (2 * GLYPH_GAP),
        .height = position.height + (2 * GLYPH_GAP),
      };

      DrawRectangleRec(bigger_position, tile_to_background(tile.type));

      DrawTexturePro(font,
                     glyphs[tile_to_glyph(tile.type)],
                     position,
                     (Vector2) {0, 0},
                     0,
                     tile_to_color(tile.type));
    }
  }
}

void render_player(Player player) {
    DrawRectanglePro((Rectangle) {
        .x = (X_TO_SCREEN(player.location.x, float) - GLYPH_GAP) + (float)player.location_offset.x,
        .y = (Y_TO_SCREEN(player.location.y, float) - GLYPH_GAP) + (float)player.location_offset.y,
        .width = (GLYPH_WIDTH + (GLYPH_GAP * 2)),
        .height = (GLYPH_HEIGHT + (GLYPH_GAP * 2)),
      },
      (Vector2) {0, 0},
      0,
      health_to_color(player));

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
}

void render_action_menu(void) {
  static float alpha = 0.0f;
  float desired_alpha = 1.0f;

  static float alpha2 = 0.0f;
  float desired_alpha2 = 0.2f;

  if (ui_state.type == UI_STATE_ACTION_MENU) {
    desired_alpha = 1.0f;
    desired_alpha2 = 0.2f;
  } else {
    desired_alpha = 0.0f;
    desired_alpha2 = 0.0f;
  }

  alpha = LERP(alpha, desired_alpha, 0.2f);
  alpha2 = LERP(alpha2, desired_alpha2, 0.2f);

  DrawRectangle(0, 0,
                X_TO_SCREEN(LEVEL_WIDTH, int),
                Y_TO_SCREEN(LEVEL_HEIGHT, int),
                CLITERAL(Color) {
                  .r = 0,
                  .g = 0,
                  .b = 0,
                  .a = (unsigned char)(alpha2 * 255)
                });

  for (size_t i = 0; i < SIZE_OF(action_menu_offsets); i++) {
    if (i == ACTION_NONE) {
      continue;
    }

    Point position = {
      .x = (size_t)((ssize_t)ui_state.action_tile_location.x + action_menu_offsets[i].x),
      .y = (size_t)((ssize_t)ui_state.action_tile_location.y + action_menu_offsets[i].y),
    };

    DrawRectanglePro(
      (Rectangle) {
        .x = X_TO_SCREEN(position.x, float) - GLYPH_GAP,
        .y = Y_TO_SCREEN(position.y, float) - GLYPH_GAP,
        .width = GLYPH_WIDTH + (2 * GLYPH_GAP),
        .height = GLYPH_HEIGHT + (2 * GLYPH_GAP),
      },
      (Vector2) {0, 0},
      0,
      CLITERAL(Color) {0, 121, 241, (unsigned char)(255 * alpha)});

    DrawTexturePro(font,
                   glyphs[action_to_glyph((Action)(i))],
                   (Rectangle) {
                     .x = X_TO_SCREEN(position.x, float),
                     .y = Y_TO_SCREEN(position.y, float),
                     .width = GLYPH_WIDTH,
                     .height = GLYPH_HEIGHT,
                   },
                   (Vector2) {0, 0},
                   0,
                   CLITERAL(Color) {0, 0, 0, (unsigned char)(255 * alpha)});
  }
}

void render_drill(Player player) {
  if (!player.is_drilling) {
    return;
  }

  ssize_t dx = ((ssize_t)player.location.x + player.drill_offset.x);
  ssize_t dy = ((ssize_t)player.location.y + player.drill_offset.y);
  if ((dx < 0 || dx >= LEVEL_WIDTH) ||
      (dy < 0 || dy >= LEVEL_HEIGHT)) {
    return;
  }

  Rectangle position = {
    .x = X_TO_SCREEN(dx, float) - GLYPH_GAP + (float)rand_range(-1, 1),
    .y = Y_TO_SCREEN(dy, float) - GLYPH_GAP + (float)rand_range(-1, 1),
    .width = GLYPH_WIDTH + (GLYPH_GAP * 2),
    .height = GLYPH_HEIGHT + (GLYPH_GAP * 2),
  };

  DrawRectangleRec(position, WHITE);

  size_t drill_glyph = ' ';

  if (player.drill_offset.y == -1) {
    drill_glyph = '^';
  } else if (player.drill_offset.y == 1) {
    drill_glyph = 'v';
  } else if (player.drill_offset.x == -1) {
    drill_glyph = '<';
  } else if (player.drill_offset.x == 1) {
    drill_glyph = '>';
  }

  DrawTexturePro(font,
                 glyphs[drill_glyph],
                 (Rectangle) {
                   .x = X_TO_SCREEN(dx, float),
                   .y = Y_TO_SCREEN(dy, float),
                   .width = GLYPH_WIDTH,
                   .height = GLYPH_HEIGHT,
                 },
                 (Vector2) {0, 0},
                 0,
                 BLACK);
}

#define ZOOM 3
void render_thing_name_under_mouse(const Level *map) {
  Point pos = mouse_in_world();

  if (!map->map[pos.y][pos.x].is_visible) {
    return;
  }

  LevelTileType tile = map->map[pos.y][pos.x].type;

  if (tile == TILE_NONE || is_tile_floor(tile)) {
    return;
  }

  const char *text = tile_type_name(tile);
  size_t text_len = strlen(text);
  float actual_text_width = (float)text_width(text_len);

  #define X 5
  #define Y 5

  DrawRectangleRec((Rectangle) {
      .x = X,
      .y = Y,
      .width = (actual_text_width + (GLYPH_GAP * 2)) * ZOOM,
      .height = (GLYPH_HEIGHT + (GLYPH_GAP * 2)) * ZOOM,
    },
    BLACK);

  render_text(text, (Vector2) {
      .x = X + GLYPH_GAP * ZOOM,
      .y = Y + GLYPH_GAP * ZOOM
    },
    WHITE,
    ZOOM);

  #undef X
  #undef Y
}

#define MESSAGE_Y_OFFSET (((GLYPH_GAP * 2) + GLYPH_HEIGHT) * ZOOM)
void render_messages(void) {
  float y = (float)GetScreenHeight() - MESSAGE_Y_OFFSET - 5;
  float x = 5;

  for (size_t i = 0; i < MESSAGES_MAX; i++) {
    if (messages[i].lifetime_secs <= 0.0f) {
      continue;
    }

    size_t text_len = strlen(messages[i].buffer);

    if (text_len == 0) {
      continue;
    }

    float actual_text_width = (float)text_width(text_len);

    DrawRectangleRec((Rectangle) {
        .x = x,
        .y = y,
        .width = (actual_text_width + (GLYPH_GAP * 2)) * ZOOM,
        .height = (GLYPH_HEIGHT + (GLYPH_GAP * 2)) * ZOOM,
      },
      ColorAlpha(BLACK, messages[i].lifetime_secs));

    render_text(messages[i].buffer, (Vector2) {
        .x = x + GLYPH_GAP * ZOOM,
        .y = y + GLYPH_GAP * ZOOM
      },
      ColorAlpha(WHITE, messages[i].lifetime_secs),
      ZOOM);

    y -= MESSAGE_Y_OFFSET;
  }
}
#undef MESSAGE_Y_OFFSET
#undef ZOOM

void render(const Level *map,
            Player player) {
  float player_screen_x = X_TO_SCREEN(player.location.x, float) + (GLYPH_WIDTH / 2.0f);
  float player_screen_y = Y_TO_SCREEN(player.location.y, float) + (GLYPH_HEIGHT / 2.0f);

  float actual_screen_width = (float)GetScreenWidth() / camera.zoom;
  float actual_screen_height = (float)GetScreenHeight() / camera.zoom;

  float target_x = CLAMPF(actual_screen_width / 2,
                          (float)world.texture.width - (actual_screen_width / 2),
                          player_screen_x);
  float target_y = CLAMPF(actual_screen_height / 2,
                          (float)world.texture.height - (actual_screen_height / 2),
                          player_screen_y);

  camera.target.x = LERP(camera.target.x, target_x, 0.1f);
  camera.target.y = LERP(camera.target.y, target_y, 0.1f);

  camera.offset.x = (float)GetScreenWidth() / 2.0f;
  camera.offset.y = (float)GetScreenHeight() / 2.0f;

  int texture_width = world.texture.width;
  int texture_height = world.texture.height;

  BeginTextureMode(world); {
    render_noise(texture_width, texture_height);
    render_map(map);
    render_drill(player);
    render_player(player);

    render_action_menu();
  } EndTextureMode();

  BeginTextureMode(world_with_cursor); {
    Point mouse = mouse_in_world();

    Vector4 mouse_cursor = {
      .x = X_TO_SCREEN(mouse.x, float) - GLYPH_GAP,
      .y = Y_TO_SCREEN(mouse.y, float) - GLYPH_GAP,
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
                       .height = (float)world.texture.height,
                     },
                     (Vector2) {0, 0},
                     WHITE);
    } EndShaderMode();
  } EndTextureMode();

  BeginTextureMode(universe); {
    ClearBackground(BLACK);

    BeginMode2D(camera); {

      if (config.do_scanline_shader) {
        BeginShaderMode(scanline_shader);
      } {
        DrawTextureRec(world_with_cursor.texture,
                       (Rectangle) {
                         .x = 0,
                         .y = 0,
                         .width = (float)world.texture.width,
                         .height = (float)world.texture.height,
                       },
                       Vector2Zero(),
                       WHITE);
      } if (config.do_scanline_shader) {
        EndShaderMode();
      }

    } EndMode2D();

    if (ui_state.type != UI_STATE_ACTION_MENU) {
      render_thing_name_under_mouse(map);
    }

    render_messages();
  } EndTextureMode();

  SetShaderValue(fisheye_shader, fisheye_shader_curvature,
                 &config.crt_curvature,
                 SHADER_UNIFORM_FLOAT);

  BeginDrawing(); {
    ClearBackground(BLACK);

    if (config.do_crt_shader) {
      BeginShaderMode(fisheye_shader);
    } {
      DrawTextureRec(universe.texture,
                     (Rectangle) {
                       .x = 0,
                       .y = 0,
                       .width = (float) GetScreenWidth(),
                       .height = (float) -GetScreenHeight(),
                     },
                     Vector2Zero(),
                     WHITE);
    } if (config.do_crt_shader) {
      EndShaderMode();
    }

    DrawFPS(0, 0);
  } EndDrawing();

  shader_time += GetFrameTime();

#define SHADER_TIME_LIMIT 10.0f
  if (shader_time >= SHADER_TIME_LIMIT) {
    shader_time -= SHADER_TIME_LIMIT;
  }
}

static Vector2 mouse_position = {0};

void update_mouse() {
  Vector2 sensitivity = (Vector2) {
    .x = config.mouse_sensitivity,
    .y = config.mouse_sensitivity,
  };

  Vector2 top_left_cap = Vector2Zero();
  Vector2 bottom_right_cap = (Vector2) {
    .x = (float)GetScreenWidth(),
    .y = (float)GetScreenHeight()
  };

#define ENABLE_MOUSE_CLAMPING_WHEN_ACTION_MENU_IS_ACTIVE
#ifdef ENABLE_MOUSE_CLAMPING_WHEN_ACTION_MENU_IS_ACTIVE
  if (ui_state.type == UI_STATE_ACTION_MENU) {
    Vector2 top_left = (Vector2) {
      .x = X_TO_SCREEN(ui_state.action_tile_location.x - 1, float),
      .y = Y_TO_SCREEN(ui_state.action_tile_location.y - 1, float),
    };

    Vector2 bottom_right = (Vector2) {
      .x = X_TO_SCREEN(ui_state.action_tile_location.x + 1, float),
      .y = Y_TO_SCREEN(ui_state.action_tile_location.y + 1, float),
    };

    top_left_cap = GetWorldToScreen2D(top_left, camera);
    bottom_right_cap = GetWorldToScreen2D(bottom_right, camera);

    sensitivity = (Vector2) {
      .x = config.action_menu_mouse_sensitivity,
      .y = config.action_menu_mouse_sensitivity,
    };
  }
#endif

  mouse_position = Vector2Add(Vector2Multiply(GetMouseDelta(), sensitivity),
                              mouse_position);

  mouse_position = Vector2Clamp(mouse_position, top_left_cap, bottom_right_cap);
}

Point mouse_in_world(void) {
  Vector2 mouse_on_screen = GetScreenToWorld2D(mouse_position, camera);

  return (Point) {
    .x = (size_t)((mouse_on_screen.x) / (GLYPH_WIDTH + 1)),
    .y = (size_t)((mouse_on_screen.y) / (GLYPH_HEIGHT + 1)),
  };
}


#define MINIMAL_WINDOW_WIDTH 700
#define MINIMAL_WINDOW_HEIGHT 500
bool is_window_big_enough() {
  return
    (GetScreenWidth() >= MINIMAL_WINDOW_WIDTH) &&
    (GetScreenHeight() >= MINIMAL_WINDOW_HEIGHT);
}

void render_text_centered(const char *text, Vector2 position, Color color, float zoom) {
  size_t text_len = strlen(text);
  float actual_text_width = (float)text_width(text_len) * zoom;

  Vector2 actual_position = {
    .x = position.x - (actual_text_width / 2.0f),
    .y = position.y - (GLYPH_HEIGHT / 2.0f),
  };

  for (size_t i = 0; i < text_len; i++) {
    DrawTexturePro(font,
                   glyphs[(size_t)text[i]],
                   (Rectangle) {
                     .x = actual_position.x,
                     .y = actual_position.y,
                     .width = GLYPH_WIDTH * zoom,
                     .height = GLYPH_HEIGHT * zoom,
                   },
                   (Vector2) {0, 0},
                   0,
                   color);

    actual_position.x += (GLYPH_WIDTH + GLYPH_GAP) * zoom;
  }
}

void render_text(const char *text, Vector2 position, Color color, float zoom) {
  size_t text_len = strlen(text);

  for (size_t i = 0; i < text_len; i++) {
    DrawTexturePro(font,
                   glyphs[(size_t)text[i]],
                   (Rectangle) {
                     .x = position.x,
                     .y = position.y,
                     .width = GLYPH_WIDTH * zoom,
                     .height = GLYPH_HEIGHT * zoom,
                   },
                   (Vector2) {0, 0},
                   0,
                   color);
    position.x += (GLYPH_WIDTH + GLYPH_GAP) * zoom;
  }
}
