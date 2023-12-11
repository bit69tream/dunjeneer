#include "player.h"
#include "font.h"
#include "level_generator.h"
#include "rand.h"
#include "raylib.h"
#include "types.h"
#include "ui.h"
#include "utils.h"
#include "config.h"

#include <assert.h>
#include <string.h>
#include <math.h>

Vector2I player_interactable_offsets[MAX_PLATER_INTERACTABLE_OFFSETS];
size_t player_interactable_offsets_len;

void process_player_movement(Player *player, LevelMap map) {
  if (is_action_key_down(KEYBIND_ACTION_MOVE_UP)) {
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

  if (is_action_key_down(KEYBIND_ACTION_MOVE_LEFT)) {
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

  if (is_action_key_down(KEYBIND_ACTION_MOVE_DOWN)) {
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

  if (is_action_key_down(KEYBIND_ACTION_MOVE_RIGHT)) {
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

bool can_interact(Player *player, Point tile_location, LevelTile tile) {
  (void) player;
  (void) tile_location;

  switch (tile) {
  case TILE_NONE:
  case TILE_FLOOR:
  case TILE_WALL:
    return false;
  case LEVEL_TILE_COUNT:
    assert(false && "bug");
  default: return true;
  }

  return true;
}

Action get_action_from_menu(void) {
  Point mouse_position = mouse_in_world();

  for (size_t i = 0; i < SIZE_OF(action_menu_offsets); i++) {
    Point position = {
      .x = (size_t)((ssize_t)ui_state.action_tile_location.x + action_menu_offsets[i].x),
      .y = (size_t)((ssize_t)ui_state.action_tile_location.y + action_menu_offsets[i].y),
    };

    if (mouse_position.x == position.x &&
        mouse_position.y == position.y) {
      return (Action)i;
    }
  }

  return ACTION_NONE;
}

#define TILE_FROM_LOCATION(map, location) ((map)[(location).y][(location.x)])

void action_open(Player *player, LevelMap *map, Point location) {
  (void) player;

  LevelTile *tile = &TILE_FROM_LOCATION(*map, location);

  switch (*tile) {
  case TILE_HORIZONTAL_CLOSED_DOOR: *tile = TILE_HORIZONTAL_OPENED_DOOR; break;
  case TILE_VERTICAL_CLOSED_DOOR: *tile = TILE_VERTICAL_OPENED_DOOR; break;
  default: return;
  }
}

void action_close(Player *player, LevelMap *map, Point location) {
  (void) player;

  LevelTile *tile = &TILE_FROM_LOCATION(*map, location);

  switch (*tile) {
  case TILE_HORIZONTAL_OPENED_DOOR: *tile = TILE_HORIZONTAL_CLOSED_DOOR; break;
  case TILE_VERTICAL_OPENED_DOOR: *tile = TILE_VERTICAL_CLOSED_DOOR; break;
  default: return;
  }
}

void action_kick(Player *player, LevelMap *map, Point location) {
  (void) player;

  LevelTile *tile = &TILE_FROM_LOCATION(*map, location);

  switch (*tile) {
  case TILE_HORIZONTAL_CLOSED_DOOR:
  case TILE_VERTICAL_CLOSED_DOOR:
  case TILE_HORIZONTAL_LOCKED_DOOR:
  case TILE_VERTICAL_LOCKED_DOOR:
  case TILE_HORIZONTAL_OPENED_DOOR:
  case TILE_VERTICAL_OPENED_DOOR: {
    /* TODO: keep track of how damaged a tile is and decide stuff based on that */
    if (roll_dice(3)) {
       *tile = TILE_FLOOR;
    }
  } break;
  default: break;               /* TODO: game need some way to show messages to player */
  }
}

void apply_action(Player *player, LevelMap *map, Point location, Action action) {
  switch (action) {
  case ACTION_NONE: assert(false && "cannot apply ACTION_NONE");
  case ACTION_OPEN: action_open(player, map, location); return;
  case ACTION_CLOSE: action_close(player, map, location); return;
  case ACTION_KICK: action_kick(player, map, location); return;
  case ACTION_PICK_UP: assert(false && "TODO");
  case ACTION_EAT: assert(false && "TODO");
  case ACTION_CLIMB: assert(false && "TODO");
  case ACTION_COUNT: assert(false && "( ._.)");
  }
}

void process_mouse(Player *player, LevelMap *map) {
  (void) player;

  /* TODO: allow to interact with objects only inside the FOV */

  if (is_action_key_pressed(KEYBIND_ACTION_ACTION_MENU)) {
    assert(ui_state.type == UI_STATE_NONE);

    Point mouse_position = mouse_in_world();

    LevelTile tile = (*map)[mouse_position.y][mouse_position.x];

    if (!can_interact(player, mouse_position, tile)) {
      /* TODO: play some sound */
      return;
    }

    ui_state.type = UI_STATE_ACTION_MENU;
    ui_state.action_tile_location = mouse_position;

    assert(ui_state.type == UI_STATE_ACTION_MENU);
  }

  if (is_action_key_released(KEYBIND_ACTION_ACTION_MENU) && ui_state.type == UI_STATE_ACTION_MENU) {
    Action action = get_action_from_menu();
    assert(action >= ACTION_NONE && action < ACTION_COUNT);

    if (action != ACTION_NONE) {
      /* TODO: play some sound */
      apply_action(player, map, ui_state.action_tile_location, action);
    }

    ui_state.type = UI_STATE_NONE;
  }
}

Color health_to_color(Player player) {
  float health_percentage = (float)player.health / (float)player.max_health;

  Color dead = CLITERAL(Color) {
    .r = 214,
    .g = 19,
    .b = 48,
    .a = 255,
  };
  Color alive = GREEN;

  return
    (CLITERAL(Color) {
      .r = (unsigned char)LERP((float)dead.r, (float)alive.r, health_percentage),
      .g = (unsigned char)LERP((float)dead.g, (float)alive.g, health_percentage),
      .b = (unsigned char)LERP((float)dead.b, (float)alive.b, health_percentage),
      .a = 255,
    });
}

/* using midpoint ellipse algorithm */
void generate_fov_outline(float radius_x, float radius_y,
                          ssize_t center_x, ssize_t center_y) {

  float x = 0;
  float y = radius_y;

  float rx2 = radius_x * radius_x;
  float ry2 = radius_y * radius_y;

  float d1 = ry2 - (rx2 * radius_y) + (0.25f * rx2);
  float dx = 2 * ry2 * x;
  float dy = 2 * rx2 * y;

#define ADD_NEW_POINT(xx, yy)                             \
  do {                                                    \
    size_t new_point = 0;                                 \
    PUSH(player_interactable_offsets,                     \
         &player_interactable_offsets_len,                \
         MAX_PLATER_INTERACTABLE_OFFSETS,                 \
         &new_point);                                     \
                                                          \
    player_interactable_offsets[new_point] = (Vector2I) { \
      .x = (xx),                                          \
      .y = (yy),                                          \
    };                                                    \
  } while(0)

  while (dx < dy) {
    ADD_NEW_POINT((ssize_t)(x) + center_x, (ssize_t)(y) + center_y);
    ADD_NEW_POINT((ssize_t)(-x) + center_x, (ssize_t)(y) + center_y);
    ADD_NEW_POINT((ssize_t)(x) + center_x, (ssize_t)(-y) + center_y);
    ADD_NEW_POINT((ssize_t)(-x) + center_x, (ssize_t)(-y) + center_y);

    x++;
    if (d1 < 0) {
      dx = dx + (2 * ry2);
      d1 = d1 + dx + ry2;
    } else {
      y--;
      dx = dx + (2 * ry2);
      dy = dy - (2 * rx2);
      d1 = d1 + dx - dy + ry2;
    }
  }

  float d2 = (ry2 * ((x + 0.5f) * (x + 0.5f))) + (rx2 * ((y - 1) * (y - 1))) - (rx2 * ry2);

  while (y >= 0) {
    ADD_NEW_POINT((ssize_t)(x) + center_x, (ssize_t)(y) + center_y);
    ADD_NEW_POINT((ssize_t)(-x) + center_x, (ssize_t)(y) + center_y);
    ADD_NEW_POINT((ssize_t)(x) + center_x, (ssize_t)(-y) + center_y);
    ADD_NEW_POINT((ssize_t)(-x) + center_x, (ssize_t)(-y) + center_y);

    y--;
    if (d2 > 0) {
      dy = dy - (2 * rx2);
      d2 = d2 + rx2 - dy;
    } else {
      x++;
      dx = dx + (2 * ry2);
      dy = dy - (2 * rx2);
      d2 = d2 + dx - dy + rx2;
    }
  }
#undef ADD_NEW_POINT
}

void init_player(Player *player) {
  player->location = (Point){0};
  player->location_offset = (Vector2I){0};
  player->direction = DIRECTION_NONE;

  #define PLAYER_INITIAL_MAX_HEALTH 50

  player->health =
    player->max_health =
    PLAYER_INITIAL_MAX_HEALTH;

  generate_fov_outline(floorf(PLAYER_VIEW_RADIUS * ((float)GLYPH_HEIGHT / (float)GLYPH_WIDTH)),
                       PLAYER_VIEW_RADIUS,
                       0, 0);
}
