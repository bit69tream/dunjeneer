#pragma once

#include "player.h"
#include "level_generator.h"
#include "config.h"

typedef enum {
  UI_STATE_NONE,
  UI_STATE_ACTION_MENU,
  UI_STATE_COUNT,
} UIStateType;

typedef struct {
  UIStateType type;
  Point action_tile_location;
} UIState;

extern UIState ui_state;

extern Vector2I action_menu_offsets[ACTION_COUNT];

void render(const Level *map,
            Player player);
void init_rendering(void);
void cleanup_rendering(void);

Point mouse_in_world(void);
void update_mouse();

void setup_action_menu(Point anchor);

bool is_window_big_enough();

void render_text_centered(const char *text, Vector2 position, Color color, float zoom);
void render_text(const char *text, Vector2 position, Color color, float zoom);
