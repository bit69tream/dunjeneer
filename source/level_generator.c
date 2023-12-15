#include "level_generator.h"
#include "rand.h"
#include "raylib.h"
#include "utils.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/types.h>

bool is_tile_solid(LevelTileType tile) {
  switch (tile) {
  case TILE_FLOOR:
  case TILE_VERTICAL_OPENED_DOOR:
  case TILE_HORIZONTAL_OPENED_DOOR:
  case TILE_GROUND:
  case TILE_HILL:
  case TILE_ELEVATOR_UP:
  case TILE_ELEVATOR_DOWN:
  case TILE_ROCK_GRAVEL:
  case TILE_GREY_ROCK_GRAVEL:
    return false;

  case TILE_NONE:
  case TILE_HORIZONTAL_CLOSED_DOOR:
  case TILE_HORIZONTAL_LOCKED_DOOR:
  case TILE_VERTICAL_CLOSED_DOOR:
  case TILE_VERTICAL_LOCKED_DOOR:
  case TILE_WALL:
  case TILE_ROCK:
  case TILE_HARD_ROCK:
  case TILE_GREY_ROCK:
    return true;

  case LEVEL_TILE_COUNT: assert(false && "nuh uh");
  }

  assert(false);
}

bool generate_leafs(Leaf *leafs, size_t *leafs_len, size_t leaf_index) {
  float where_to_split = frand_range(0.4f, 0.6f);

  if (((int)(leafs[leaf_index].rect.height * where_to_split) <= MIN_LEAF_SIZE) ||
      ((int)(leafs[leaf_index].rect.width * where_to_split) <= MIN_LEAF_SIZE)) {
    return false;
  }

  size_t new_left_leaf = 0;
  size_t new_right_leaf = 0;

  if (roll_dice(2)) {
    // horizontal split
    int split_y = (int)(leafs[leaf_index].rect.height * where_to_split);

    /* first leaf */
    {
      size_t new_leaf = 0;
      PUSH(leafs, leafs_len, LEAFS_MAX, &new_leaf);

      leafs[new_leaf].rect.x = leafs[leaf_index].rect.x;
      leafs[new_leaf].rect.y = leafs[leaf_index].rect.y;
      leafs[new_leaf].rect.width = leafs[leaf_index].rect.width;
      leafs[new_leaf].rect.height = (float)split_y;
      leafs[new_leaf].left_leaf = 0;
      leafs[new_leaf].right_leaf = 0;
      leafs[new_leaf].parent = leaf_index;

      new_left_leaf = new_leaf;
    }

    /* second leaf */
    {
      size_t new_leaf = 0;
      PUSH(leafs, leafs_len, LEAFS_MAX, &new_leaf);

      leafs[new_leaf].rect.x = leafs[leaf_index].rect.x;
      leafs[new_leaf].rect.y = leafs[leaf_index].rect.y + (float)split_y;
      leafs[new_leaf].rect.width = leafs[leaf_index].rect.width;
      leafs[new_leaf].rect.height = (float)((int)leafs[leaf_index].rect.height - split_y);
      leafs[new_leaf].left_leaf = 0;
      leafs[new_leaf].right_leaf = 0;
      leafs[new_leaf].parent = leaf_index;

      new_right_leaf = new_leaf;
    }
  } else {
    // vertical split
    int split_x = (int)(leafs[leaf_index].rect.width * where_to_split);

    /* first leaf */
    {
      size_t new_leaf = 0;
      PUSH(leafs, leafs_len, LEAFS_MAX, &new_leaf);

      leafs[new_leaf].rect.x = leafs[leaf_index].rect.x;
      leafs[new_leaf].rect.y = leafs[leaf_index].rect.y;
      leafs[new_leaf].rect.width = (float)split_x;
      leafs[new_leaf].rect.height = leafs[leaf_index].rect.height;
      leafs[new_leaf].left_leaf = 0;
      leafs[new_leaf].right_leaf = 0;
      leafs[new_leaf].parent = leaf_index;

      new_left_leaf = new_leaf;
    }

    /* second leaf */
    {
      size_t new_leaf = 0;
      PUSH(leafs, leafs_len, LEAFS_MAX, &new_leaf);

      leafs[new_leaf].rect.x = leafs[leaf_index].rect.x + (float)split_x;
      leafs[new_leaf].rect.y = leafs[leaf_index].rect.y;
      leafs[new_leaf].rect.width = (float)((int)leafs[leaf_index].rect.width - split_x);
      leafs[new_leaf].rect.height = leafs[leaf_index].rect.height;
      leafs[new_leaf].left_leaf = 0;
      leafs[new_leaf].right_leaf = 0;
      leafs[new_leaf].parent = leaf_index;

      new_right_leaf = new_leaf;
    }
  }

  leafs[leaf_index].left_leaf = new_left_leaf;
  leafs[leaf_index].right_leaf = new_right_leaf;

  return true;
}

bool generate_leafs_until_unable_to(Leaf *leafs, size_t *leafs_len, size_t leaf_index) {
  if (!generate_leafs(leafs, leafs_len, leaf_index)) {
    return false;
  }

  if (!generate_leafs_until_unable_to(leafs, leafs_len, leafs[leaf_index].left_leaf) &&
      !generate_leafs_until_unable_to(leafs, leafs_len, leafs[leaf_index].right_leaf)) {
    return false;
  }

  return true;
}

void generate_rooms(Room rooms[ROOMS_MAX], size_t *rooms_len) {
  Leaf leafs[LEAFS_MAX] = {0};
  size_t leafs_len = 0;

  float where_to_split = frand_range(0.4f, 0.6f);

  /* TODO: remove the huge leaf generation duplication */

  if (roll_dice(2)) {
    // horizontal split
    int split_y = (int)(LEVEL_HEIGHT * where_to_split);

    /* first leaf */
    {
      size_t new_leaf = 0;
      PUSH(leafs, &leafs_len, LEAFS_MAX, &new_leaf);

      leafs[new_leaf].rect.x = 0;
      leafs[new_leaf].rect.y = 0;
      leafs[new_leaf].rect.width = LEVEL_WIDTH;
      leafs[new_leaf].rect.height = (float)split_y;
      leafs[new_leaf].left_leaf = 0;
      leafs[new_leaf].right_leaf = 0;
      leafs[new_leaf].parent = 0;
    }

    /* second leaf */
    {
      size_t new_leaf = 0;
      PUSH(leafs, &leafs_len, LEAFS_MAX, &new_leaf);

      leafs[new_leaf].rect.x = 0;
      leafs[new_leaf].rect.y = (float)split_y;
      leafs[new_leaf].rect.width = LEVEL_WIDTH;
      leafs[new_leaf].rect.height = (float)(LEVEL_HEIGHT - split_y);
      leafs[new_leaf].left_leaf = 0;
      leafs[new_leaf].right_leaf = 0;
      leafs[new_leaf].parent = 0;
    }
  } else {
    // vertical split
    int split_x = (int)(LEVEL_WIDTH * where_to_split);

    /* first leaf */
    {
      size_t new_leaf = 0;
      PUSH(leafs, &leafs_len, LEAFS_MAX, &new_leaf);

      leafs[new_leaf].rect.x = 0;
      leafs[new_leaf].rect.y = 0;
      leafs[new_leaf].rect.width = (float)split_x;
      leafs[new_leaf].rect.height = LEVEL_HEIGHT;
      leafs[new_leaf].left_leaf = 0;
      leafs[new_leaf].right_leaf = 0;
      leafs[new_leaf].parent = 0;
    }

    /* second leaf */
    {
      size_t new_leaf = 0;
      PUSH(leafs, &leafs_len, LEAFS_MAX, &new_leaf);

      leafs[new_leaf].rect.x = (float)split_x;
      leafs[new_leaf].rect.y = 0;
      leafs[new_leaf].rect.width = (float)(LEVEL_WIDTH - split_x);
      leafs[new_leaf].rect.height = LEVEL_HEIGHT;
      leafs[new_leaf].left_leaf = 0;
      leafs[new_leaf].right_leaf = 0;
      leafs[new_leaf].parent = 0;
    }
  }

  assert(leafs_len == 2);
  while (true) {
    generate_leafs_until_unable_to(leafs, &leafs_len, 0);
    generate_leafs_until_unable_to(leafs, &leafs_len, 1);

    size_t rooms_counter = 0;
    for (size_t i = 0; i < leafs_len; i++) {
      if (leafs[i].left_leaf == 0 || leafs[i].right_leaf == 0) {
        rooms_counter++;
      }
    }

    if (rooms_counter < 10) {
      leafs_len = 2;
      continue;
    }

    break;
  }

  *rooms_len = 0;

  for (size_t i = 0; i < leafs_len; i++) {
    if (leafs[i].left_leaf != 0 && leafs[i].right_leaf != 0) {
      continue;
    }

    size_t new_room = 0;
    PUSH(rooms, rooms_len, ROOMS_MAX, &new_room);

    rooms[new_room].width = (float)((int)((frand_range(0.6f, 0.9f) * (leafs[i].rect.width))));
    rooms[new_room].height = (float)((int)((frand_range(0.6f, 0.9f) * (leafs[i].rect.height))));
    rooms[new_room].x = leafs[i].rect.x + (float)(rand_range(0, (int)(leafs[i].rect.width - rooms[new_room].width - 1)));
    rooms[new_room].y = leafs[i].rect.y + (float)(rand_range(0, (int)(leafs[i].rect.height - rooms[new_room].height - 1)));
  }
}

bool do_vertical_lines_intersect(Point start_a, Point end_a,
                                 Point start_b, Point end_b) {
  assert(start_a.x == end_a.x);
  assert(start_b.x == end_b.x);

  if (start_a.x != start_b.x) {
    return false;
  }

  if (start_a.y > end_a.y) {
    SWAP(size_t, &start_a.y, &end_a.y);
  }

  if (start_b.y > end_b.y) {
    SWAP(size_t, &start_a.y, &end_a.y);
  }

  if (start_a.y < start_b.y) {
    return end_a.y >= start_b.y;
  } else {
    return end_b.y >= start_a.y;
  }
}

bool do_horizontal_lines_intersect(Point start_a, Point end_a,
                                   Point start_b, Point end_b) {
  assert(start_a.y == end_a.y);
  assert(start_b.y == end_b.y);

  if (start_a.y != start_b.y) {
    return false;
  }

  if (start_a.x > end_a.x) {
    SWAP(size_t, &start_a.x, &end_a.x);
  }

  if (start_b.x > end_b.x) {
    SWAP(size_t, &start_a.x, &end_a.x);
  }

  if (start_a.x < start_b.x) {
    return end_a.x >= start_b.x;
  } else {
    return end_b.x >= start_a.x;
  }
}

bool is_line_inside_vertical_walls(Room rooms[ROOMS_MAX], size_t room_index,
                                   Point line_start, Point line_end) {
  return
    do_vertical_lines_intersect(line_start, line_end,
                                (Point) {
                                  .x = (size_t)rooms[room_index].x,
                                  .y = (size_t)rooms[room_index].y,
                                },
                                (Point) {
                                  .x = (size_t)rooms[room_index].x,
                                  .y = (size_t)rooms[room_index].y + (size_t)rooms[room_index].height,
                                }) ||
    do_vertical_lines_intersect(line_start, line_end,
                                     (Point) {
                                       .x = (size_t)rooms[room_index].x + (size_t)rooms[room_index].width,
                                       .y = (size_t)rooms[room_index].y,
                                     },
                                     (Point) {
                                       .x = (size_t)rooms[room_index].x + (size_t)rooms[room_index].width,
                                       .y = (size_t)rooms[room_index].y + (size_t)rooms[room_index].height,
                                     });
}

bool is_line_inside_horizontal_walls(Room rooms[ROOMS_MAX], size_t room_index,
                                     Point line_start, Point line_end) {
  return
    do_horizontal_lines_intersect(line_start, line_end,
                                  (Point) {
                                    .x = (size_t)rooms[room_index].x,
                                    .y = (size_t)rooms[room_index].y,
                                  },
                                  (Point) {
                                    .x = (size_t)rooms[room_index].x + (size_t)rooms[room_index].width,
                                    .y = (size_t)rooms[room_index].y,
                                  }) ||
    do_horizontal_lines_intersect(line_start, line_end,
                                  (Point) {
                                    .x = (size_t)rooms[room_index].x,
                                    .y = (size_t)rooms[room_index].y + (size_t)rooms[room_index].height,
                                  },
                                  (Point) {
                                    .x = (size_t)rooms[room_index].x + (size_t)rooms[room_index].width,
                                    .y = (size_t)rooms[room_index].y + (size_t)rooms[room_index].height,
                                  });
}

bool is_inside_a_wall(Room rooms[ROOMS_MAX], size_t rooms_len,
                      Path paths[PATHS_MAX], size_t path_index) {
  for (size_t i = 0; i < rooms_len; i++) {
    if (paths[path_index].vertical_first) {
      if (is_line_inside_vertical_walls(rooms, i,
                                        paths[path_index].start,
                                        paths[path_index].middle) ||
          is_line_inside_horizontal_walls(rooms, i,
                                          paths[path_index].middle,
                                          paths[path_index].end)) {
        return true;
      }
    } else {
      if (is_line_inside_horizontal_walls(rooms, i,
                                          paths[path_index].start,
                                          paths[path_index].middle) ||
          is_line_inside_vertical_walls(rooms, i,
                                        paths[path_index].middle,
                                        paths[path_index].end)) {
        return true;
      }
    }
  }

  return false;
}


#define RANDOM_POINT_INSIDE_OF_THE_ROOM(point, room)                    \
  do {                                                                  \
    (point)->x = (size_t)(((room).x + 1) + (drand48() * ((room).width - 2))); \
    (point)->y = (size_t)(((room).y + 1) + (drand48() * ((room).height - 2))); \
  } while (0)

void generate_paths_between_rooms(Room rooms[ROOMS_MAX], size_t rooms_len,
                                  Path paths[PATHS_MAX], size_t *paths_len) {
  for (size_t i = 1; i < rooms_len; i++) {
  try_again:;

    Point point_a = {0};
    RANDOM_POINT_INSIDE_OF_THE_ROOM(&point_a, rooms[i - 1]);

    Point point_b = {0};
    RANDOM_POINT_INSIDE_OF_THE_ROOM(&point_b, rooms[i]);

    size_t new_path = 0;
    PUSH(paths, paths_len, PATHS_MAX, &new_path);
    if (roll_dice(2)) {
      /* vertical - horizontal */
      paths[new_path] = (Path){
          .start = point_a,
          .middle = (Point){.x = point_a.x, .y = point_b.y},
          .end = point_b,
          .vertical_first = true,
      };
    } else {
      /* horizontal - vertical */
      paths[new_path] = (Path){
          .start = point_a,
          .middle = (Point){.x = point_b.x, .y = point_a.y},
          .end = point_b,
          .vertical_first = false,
      };
    }

    if (is_inside_a_wall(rooms, rooms_len, paths, new_path)) {
      *paths_len -= 1;
      goto try_again;
    }
  }
}

void put_horizontal_path(Level *output_map,
                         size_t x_start,
                         size_t x_end,
                         size_t y) {
  if (x_end < x_start) {
    size_t tmp = x_end;
    x_end = x_start;
    x_start = tmp;
  }

  for (size_t xi = x_start; xi <= x_end; xi++) {
    if (output_map->map[y][xi].type == TILE_WALL && roll_dice(2)) {
      output_map->map[y][xi].type = roll_dice(4) ? TILE_VERTICAL_LOCKED_DOOR : TILE_VERTICAL_CLOSED_DOOR;
    } else {
      output_map->map[y][xi].type = TILE_FLOOR;
    }
  }
}

void put_vertical_path(Level *output_map,
                       size_t y_start,
                       size_t y_end,
                       size_t x) {
  if (y_end < y_start) {
    size_t tmp = y_end;
    y_end = y_start;
    y_start = tmp;
  }

  for (size_t yi = y_start; yi <= y_end; yi++) {
    if (output_map->map[yi][x].type == TILE_WALL && roll_dice(2)) {
      output_map->map[yi][x].type = roll_dice(4) ? TILE_HORIZONTAL_LOCKED_DOOR : TILE_HORIZONTAL_CLOSED_DOOR;
    } else {
      output_map->map[yi][x].type = TILE_FLOOR;
    }
  }
}

void construct_map(Level *output_map,
                             Room rooms[ROOMS_MAX], size_t rooms_len,
                             Path paths[PATHS_MAX], size_t paths_len) {
  (void)paths;
  (void)paths_len;

  for (size_t i = 0; i < LEVEL_HEIGHT; i++) {
    for (size_t j = 0; j < LEVEL_WIDTH; j++) {
      output_map->map[i][j].type = TILE_NONE;
    }
  }

  for (size_t i = 0; i < rooms_len; i++) {
    size_t y = (size_t)rooms[i].y;
    size_t x = (size_t)rooms[i].x;

    /* top */
    for (size_t xi = 0; xi < (size_t)rooms[i].width; xi++) {
      output_map->map[y][x + xi].type = TILE_WALL;
    }

    /* bottom */
    y += (size_t)rooms[i].height;
    for (size_t xi = 0; xi < (size_t)rooms[i].width; xi++) {
      output_map->map[y][x + xi].type = TILE_WALL;
    }

    /* left */
    y = (size_t)rooms[i].y;
    for (size_t yi = 0; yi <= (size_t)rooms[i].height; yi++) {
      output_map->map[y + yi][x].type = TILE_WALL;
    }

    /* right */
    x += (size_t)rooms[i].width;
    for (size_t yi = 0; yi <= (size_t)rooms[i].height; yi++) {
      output_map->map[y + yi][x].type = TILE_WALL;
    }

    /* floor */
    x = (size_t)rooms[i].x;
    for (size_t yi = 1; yi < (size_t)rooms[i].height; yi++) {
      for (size_t xi = 1; xi < (size_t)rooms[i].width; xi++) {
        output_map->map[y + yi][x + xi].type = TILE_FLOOR;
      }
    }
  }

  for (size_t i = 0; i < paths_len; i++) {
    if (paths[i].vertical_first) {
      /* vertical horizontal */
      put_vertical_path(output_map,
                        (size_t)paths[i].start.y,
                        (size_t)paths[i].middle.y,
                        (size_t)paths[i].start.x);

      put_horizontal_path(output_map,
                          (size_t)paths[i].middle.x,
                          (size_t)paths[i].end.x,
                          (size_t)paths[i].middle.y);
    } else {
      /* horizontal vertical */
      put_horizontal_path(output_map,
                          (size_t)paths[i].start.x,
                          (size_t)paths[i].middle.x,
                          (size_t)paths[i].start.y);

      put_vertical_path(output_map,
                        (size_t)paths[i].middle.y,
                        (size_t)paths[i].end.y,
                        (size_t)paths[i].middle.x);
    }
  }
}

void generate_surface(Level *output_map,
                      Point *player_location) {
  Image noise = GenImagePerlinNoise(LEVEL_WIDTH, LEVEL_HEIGHT,
                                    0, 0,
                                    (float)drand48() * 10);

  for (size_t yi = 0; yi < LEVEL_HEIGHT; yi++) {
    for (size_t xi = 0; xi < LEVEL_WIDTH; xi++) {
      uint8_t a = ((Color *)(noise.data))[(yi * LEVEL_WIDTH) + xi].r;

      LevelTileType t = TILE_NONE;

      if (a < 50) {
        t = TILE_GROUND;
      } else if (a < 120) {
        t = TILE_HILL;
      } else if (a < 140) {
        t = TILE_GREY_ROCK;
      } else if (a < 180) {
        t = TILE_ROCK;
      } else {
        t = TILE_HARD_ROCK;
      }
      output_map->map[yi][xi].type = t;
    }
  }

  for (size_t yi = 0; yi < LEVEL_HEIGHT; yi++) {
    size_t x = (size_t)((5 * (sin((float)yi / 4) / 4 + 1)) - 3);
    output_map->map[yi][x].type = TILE_HARD_ROCK;
    output_map->map[yi][LEVEL_WIDTH - 1 - x].type = TILE_HARD_ROCK;

    for (size_t xi = 0; xi < x; xi++) {
    output_map->map[yi][xi].type = TILE_HARD_ROCK;
    output_map->map[yi][LEVEL_WIDTH - 1 - xi].type = TILE_HARD_ROCK;
    }
  }

  for (size_t xi = 0; xi < LEVEL_WIDTH; xi++) {
    size_t y = (size_t)((5 * (cos((float)xi / 4) / 3 + 1)) - 3);
    output_map->map[y][xi].type = TILE_HARD_ROCK;
    output_map->map[LEVEL_HEIGHT - 1 - y][xi].type = TILE_HARD_ROCK;

    for (size_t yi = 0; yi < y; yi++) {
    output_map->map[yi][xi].type = TILE_HARD_ROCK;
    output_map->map[LEVEL_HEIGHT - 1 - yi][xi].type = TILE_HARD_ROCK;
    }
  }

  ssize_t px = rand_range(0, LEVEL_WIDTH - 1);
  ssize_t py = rand_range(0, LEVEL_HEIGHT - 1);
  while (is_tile_solid(output_map->map[py][px].type)) {
    px = rand_range(0, LEVEL_WIDTH - 1);
    py = rand_range(0, LEVEL_HEIGHT - 1);
  }

  player_location->x = (size_t)px;
  player_location->y = (size_t)py;

#define ELEVATOR_DOWN_RANGE 25

  ssize_t ex = rand_range(0, ELEVATOR_DOWN_RANGE * 2) - ELEVATOR_DOWN_RANGE;
  ssize_t ey = rand_range(0, ELEVATOR_DOWN_RANGE * 2) - ELEVATOR_DOWN_RANGE;

  while (((px + ex) < 0 || (px + ex) >= LEVEL_WIDTH) ||
         ((py + ey) < 0 || (py + ey) >= LEVEL_HEIGHT) ||
         is_tile_solid(output_map->map[py + ey][px + ex].type)) {
    ex = rand_range(0, ELEVATOR_DOWN_RANGE * 2) - ELEVATOR_DOWN_RANGE;
    ey = rand_range(0, ELEVATOR_DOWN_RANGE * 2) - ELEVATOR_DOWN_RANGE;
  }

  output_map->map[py + ey][px + ex].type = TILE_ELEVATOR_DOWN;

  UnloadImage(noise);
}

void generate_dungeon(Level *output_map,
                      Point *player_location) {
  static Room rooms[ROOMS_MAX] = {0};
  size_t rooms_len = 0;

  static Path paths[PATHS_MAX] = {0};
  size_t paths_len = 0;

  generate_rooms(rooms, &rooms_len);
  generate_paths_between_rooms(rooms, rooms_len, paths, &paths_len);

  construct_map(output_map, rooms, rooms_len, paths, paths_len);

  size_t i = (size_t)rand_range(0, (int)rooms_len - 1);

  Point loc;
  RANDOM_POINT_INSIDE_OF_THE_ROOM(&loc, rooms[i]);
  output_map->map[loc.y][loc.x].type = TILE_ELEVATOR_DOWN;

  size_t player_room = (size_t)rand_range(0, (int)rooms_len - 1);

  RANDOM_POINT_INSIDE_OF_THE_ROOM(&loc, rooms[player_room]);
  output_map->map[loc.y][loc.x].type = TILE_ELEVATOR_UP;

  RANDOM_POINT_INSIDE_OF_THE_ROOM(player_location, rooms[player_room]);
}

bool can_be_drilled(LevelTileType tile) {
  switch (tile) {
  case TILE_NONE:
  case TILE_FLOOR:
  case TILE_GROUND:
  case TILE_HILL:
  case TILE_WALL:
  case TILE_ELEVATOR_DOWN:
  case TILE_ELEVATOR_UP:

  case TILE_HARD_ROCK:

  case TILE_ROCK_GRAVEL:
  case TILE_GREY_ROCK_GRAVEL:
    return false;

  case TILE_VERTICAL_OPENED_DOOR:
  case TILE_HORIZONTAL_OPENED_DOOR:
  case TILE_VERTICAL_CLOSED_DOOR:
  case TILE_HORIZONTAL_CLOSED_DOOR:
  case TILE_VERTICAL_LOCKED_DOOR:
  case TILE_HORIZONTAL_LOCKED_DOOR:

  case TILE_ROCK:
  case TILE_GREY_ROCK:
    return true;

  case LEVEL_TILE_COUNT: assert(false);
  }

  assert(false);
}

void set_up_tile_metadata(Level *output_map) {
  for (size_t yi = 0; yi < LEVEL_HEIGHT; yi++) {
    for (size_t xi = 0; xi < LEVEL_WIDTH; xi++) {
      switch (output_map->map[yi][xi].type) {
      case TILE_NONE:
      case TILE_FLOOR:
      case TILE_GROUND:
      case TILE_HILL:
      case TILE_WALL:
      case TILE_ELEVATOR_UP:
      case TILE_ELEVATOR_DOWN:
      case TILE_HARD_ROCK:
      case TILE_ROCK_GRAVEL:
      case TILE_GREY_ROCK_GRAVEL:
        break;

      case TILE_VERTICAL_OPENED_DOOR:
      case TILE_HORIZONTAL_OPENED_DOOR:
      case TILE_VERTICAL_CLOSED_DOOR:
      case TILE_HORIZONTAL_CLOSED_DOOR:
      case TILE_VERTICAL_LOCKED_DOOR:
      case TILE_HORIZONTAL_LOCKED_DOOR:
        output_map->map[yi][xi].durability = 2;
        break;

      case TILE_ROCK:
        output_map->map[yi][xi].durability = 15;
        break;
      case TILE_GREY_ROCK:
        output_map->map[yi][xi].durability = 5;
        break;

      case LEVEL_TILE_COUNT: assert(false);
      }
    }
  }
}

void generate_level(Level *output_map,
                    Point *player_location,
                    LevelType type) {
  switch (type) {
  case LEVEL_NONE:
  case LEVEL_TYPE_COUNT: assert(false && ":<");

  case LEVEL_SURFACE: generate_surface(output_map, player_location); break;
  case LEVEL_DUNGEON: generate_dungeon(output_map, player_location); break;
  }

  set_up_tile_metadata(output_map);
  output_map->type = type;
  output_map->is_generated = true;
}

bool is_tile_floor(LevelTileType tile) {
  switch (tile) {
  case TILE_NONE:
  case LEVEL_TILE_COUNT: assert(false && "nah");

  case TILE_FLOOR:
  case TILE_HILL:
  case TILE_GROUND:

  case TILE_ROCK_GRAVEL:
  case TILE_GREY_ROCK_GRAVEL:
    return true;

  case TILE_ELEVATOR_UP:
  case TILE_ELEVATOR_DOWN:

  case TILE_WALL:

  case TILE_HORIZONTAL_CLOSED_DOOR:
  case TILE_HORIZONTAL_LOCKED_DOOR:
  case TILE_HORIZONTAL_OPENED_DOOR:
  case TILE_VERTICAL_CLOSED_DOOR:
  case TILE_VERTICAL_LOCKED_DOOR:
  case TILE_VERTICAL_OPENED_DOOR:

  case TILE_ROCK:
  case TILE_GREY_ROCK:
  case TILE_HARD_ROCK:
    return false;
  }

  assert(false);
}


const char *tile_type_name(LevelTileType type) {
  switch (type) {
  case TILE_NONE:
  case LEVEL_TILE_COUNT: assert(false && "nah");

  case TILE_FLOOR: return "Floor";

  case TILE_WALL: return "Wall";

  case TILE_ELEVATOR_DOWN: return "Elevator Down";
  case TILE_ELEVATOR_UP: return "Elevator Up";

  case TILE_VERTICAL_CLOSED_DOOR:
  case TILE_HORIZONTAL_CLOSED_DOOR:
  case TILE_VERTICAL_LOCKED_DOOR:
  case TILE_HORIZONTAL_LOCKED_DOOR:
  case TILE_VERTICAL_OPENED_DOOR:
  case TILE_HORIZONTAL_OPENED_DOOR:
    return "Door";

  case TILE_GROUND: return "Ground";
  case TILE_HILL: return "Hill";
  case TILE_ROCK: return "Rock";
  case TILE_HARD_ROCK: return "Hard rock";
  case TILE_GREY_ROCK: return "Grey rock";

  case TILE_ROCK_GRAVEL:
  case TILE_GREY_ROCK_GRAVEL: return "Gravel";
  }

  assert(false);
}

const char *object_type_name(LevelObjectType type) {
  switch (type) {
  case OBJECT_NONE:
  case LEVEL_OBJECT_COUNT: assert(false && "no way");
  }

  assert(false);
}

/* TODO: add an ability to also leave objects */
LevelTileType what_is_left_after_destruction(LevelTileType tile) {
  switch (tile) {
  case TILE_NONE:
  case LEVEL_TILE_COUNT: assert(false && "nah");

  case TILE_FLOOR: assert(false && "unexpected");

  case TILE_WALL: return TILE_FLOOR;

  case TILE_ELEVATOR_DOWN: assert(false && "unbelievable");
  case TILE_ELEVATOR_UP: assert(false && "unbelievable");

  case TILE_VERTICAL_CLOSED_DOOR:
  case TILE_HORIZONTAL_CLOSED_DOOR:
  case TILE_VERTICAL_LOCKED_DOOR:
  case TILE_HORIZONTAL_LOCKED_DOOR:
  case TILE_VERTICAL_OPENED_DOOR:
  case TILE_HORIZONTAL_OPENED_DOOR:
    return TILE_FLOOR;

  case TILE_GROUND: assert(false && "cannot be possible");
  case TILE_HILL: assert(false && "nope");
  case TILE_ROCK: return TILE_ROCK_GRAVEL;
  case TILE_HARD_ROCK: assert(false && "cheater");
  case TILE_GREY_ROCK: return TILE_GREY_ROCK_GRAVEL;

  case TILE_ROCK_GRAVEL:
  case TILE_GREY_ROCK_GRAVEL:
    assert(false && "nuh uh");
  }

  assert(false);
}
