#include "player.h"
#include "font.h"
#include "level_generator.h"
#include "raylib.h"
#include "ui.h"
#include "utils.h"
#include <assert.h>

void process_player_movement(Player *player, LevelMap map) {
  if (IsKeyDown(KEY_E)) {
    if ((player->direction == DIRECTION_NONE ||
         player->direction == DIRECTION_LEFT ||
         player->direction == DIRECTION_RIGHT) &&
        ((map[player->location.y - 1][player->location.x] == TILE_FLOOR) ||
         (map[player->location.y - 1][player->location.x] == TILE_VERTICAL_OPENED_DOOR) ||
         (map[player->location.y - 1][player->location.x] == TILE_HORIZONTAL_OPENED_DOOR))) {
      player->direction |= DIRECTION_UP;
      player->location_offset.y = (GLYPH_HEIGHT + GLYPH_GAP);
      player->location.y -= 1;
    }
  }

  if (IsKeyDown(KEY_S)) {
    if ((player->direction == DIRECTION_NONE ||
         player->direction == DIRECTION_DOWN ||
         player->direction == DIRECTION_UP) &&
        ((map[player->location.y][player->location.x - 1] == TILE_FLOOR) ||
         (map[player->location.y][player->location.x - 1] == TILE_VERTICAL_OPENED_DOOR) ||
         (map[player->location.y][player->location.x - 1] == TILE_HORIZONTAL_OPENED_DOOR))) {
      player->direction |= DIRECTION_LEFT;
      player->location_offset.x = (GLYPH_WIDTH + GLYPH_GAP);
      player->location.x -= 1;
    }
  }

  if (IsKeyDown(KEY_D)) {
    if ((player->direction == DIRECTION_NONE ||
         player->direction == DIRECTION_LEFT ||
         player->direction == DIRECTION_RIGHT) &&
        ((map[player->location.y + 1][player->location.x] == TILE_FLOOR) ||
         (map[player->location.y + 1][player->location.x] == TILE_VERTICAL_OPENED_DOOR) ||
         (map[player->location.y + 1][player->location.x] == TILE_HORIZONTAL_OPENED_DOOR))) {
      player->direction |= DIRECTION_DOWN;
      player->location_offset.y = -(GLYPH_HEIGHT + GLYPH_GAP);
      player->location.y += 1;
    }
  }

  if (IsKeyDown(KEY_F)) {
    if ((player->direction == DIRECTION_NONE ||
         player->direction == DIRECTION_UP ||
         player->direction == DIRECTION_DOWN) &&
        ((map[player->location.y][player->location.x + 1] == TILE_FLOOR) ||
         (map[player->location.y][player->location.x + 1] == TILE_VERTICAL_OPENED_DOOR) ||
         (map[player->location.y][player->location.x + 1] == TILE_HORIZONTAL_OPENED_DOOR))) {
      player->direction |= DIRECTION_RIGHT;
      player->location_offset.x = -(GLYPH_WIDTH + GLYPH_GAP);
      player->location.x += 1;
    }
  }

  if (player->direction != DIRECTION_NONE) {
    if (player->location_offset.x == 0) {
      player->direction = player->direction & (unsigned int)(~DIRECTION_LEFT);
      player->direction = player->direction & (unsigned int)(~DIRECTION_RIGHT);
    }

    if (player->location_offset.y == 0) {
      player->direction = player->direction & (unsigned int)(~DIRECTION_UP);
      player->direction = player->direction & (unsigned int)(~DIRECTION_DOWN);
    }

    if (player->direction & DIRECTION_LEFT ||
        player->direction & DIRECTION_RIGHT) {
      if (player->location_offset.x < 0) {
        player->location_offset.x += 1;
      } else {
        player->location_offset.x -= 1;
      }
    }

    if (player->direction & DIRECTION_UP ||
        player->direction & DIRECTION_DOWN) {
      if (player->location_offset.y < 0) {
        player->location_offset.y += 1;
      } else {
        player->location_offset.y -= 1;
      }
    }
  }

  player->location.x = CLAMP(1, LEVEL_WIDTH - 2, player->location.x);
  player->location.y = CLAMP(1, LEVEL_HEIGHT - 2, player->location.y);
}

bool cannot_interact(Player *player, Point tile_location) {
  (void) player;
  (void) tile_location;

  return false;
}

bool can_interact(Player *player, Point tile_location) {
  (void) player;
  (void) tile_location;

  return true;
}

#include <stdio.h>

void action_crash(LevelMap *map, Point tile_location, Action action) {
  (void) map;
  (void) tile_location;
  (void) action;

  printf("tile location = (%lu %lu); action = %d\n", tile_location.x, tile_location.y, action);
  assert(false && "forbidden action");
}

void action_close(LevelMap *map, Point tile_location, Action action) {
  (void) map;
  (void) tile_location;
  (void) action;

  printf("closed\n");
}

void action_open(LevelMap *map, Point tile_location, Action action) {
  (void) map;
  (void) tile_location;
  (void) action;

  printf("opened\n");
}

struct {
  void (*apply_action)(LevelMap *map, Point tile_location, Action action);
  bool (*predicate)(Player *player, Point tile_location);
} interaction_table[] = {
  [TILE_NONE] = {
    .apply_action = action_crash,
    .predicate = cannot_interact,
  },
  [TILE_FLOOR] = {
    .apply_action = action_crash,
    .predicate = cannot_interact,
  },
  [TILE_WALL] = {
    .apply_action = action_crash,
    .predicate = cannot_interact,
  },

  [TILE_VERTICAL_CLOSED_DOOR] = {
    .apply_action = action_close,
    .predicate = can_interact,
  },
  [TILE_VERTICAL_OPENED_DOOR] = {
    .apply_action = action_close,
    .predicate = can_interact,
  },

  [TILE_HORIZONTAL_CLOSED_DOOR] = {
    .apply_action = action_open,
    .predicate = can_interact,
  },
  [TILE_HORIZONTAL_OPENED_DOOR] = {
    .apply_action = action_open,
    .predicate = can_interact,
  },
};
static_assert(SIZE_OF(interaction_table) == LEVEL_TILE_COUNT);

Action get_action_from_menu(void) {
  Point mouse_position = mouse_in_world();

  for (size_t i = 0; i < SIZE_OF(action_menu_offsets); i++) {
    Point position = {
      .x = (size_t)((ssize_t)ui_state.action_tile_location.x + action_menu_offsets[i].x),
      .y = (size_t)((ssize_t)ui_state.action_tile_location.y + action_menu_offsets[i].y),
    };

    if (mouse_position.x == position.x &&
        mouse_position.y == position.y) {
      return (Action)(i + 1);
    }
  }

  return ACTION_NONE;
}

void process_mouse(Player *player, LevelMap *map) {
  (void) player;

  if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
    assert(ui_state.type == UI_STATE_NONE);

    Point mouse_position = mouse_in_world();

    LevelTile tile = (*map)[mouse_position.y][mouse_position.x];

    if (!interaction_table[tile].predicate(player, mouse_position)) {
      /* TODO: play some sound */
      return;
    }

    ui_state.type = UI_STATE_ACTION_MENU;
    ui_state.action_tile_location = mouse_position;

    assert(ui_state.type == UI_STATE_ACTION_MENU);
  }

  if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT) && ui_state.type == UI_STATE_ACTION_MENU) {
    LevelTile tile = (*map)[ui_state.action_tile_location.y][ui_state.action_tile_location.x];

    Action action = get_action_from_menu();
    assert(action >= ACTION_NONE && action < ACTION_COUNT);

    if (action != ACTION_NONE) {
      /* TODO: play some sound */
      interaction_table[tile].apply_action(map,
                                           ui_state.action_tile_location,
                                           action);
    }

    ui_state.type = UI_STATE_NONE;
  }
}
