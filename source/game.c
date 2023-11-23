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
#include "render.h"
#include "utils.h"
#include "font.h"
#include "player.h"

int main(void) {
  srand((unsigned int)time(0));

  static LevelMap map = {0};

  static LevelObject objects[OBJECTS_MAX] = {0};
  size_t objects_len = 0;

  Player player = {0};

  generate_level(&map, objects, &objects_len, &player.location);

  /* TODO: https://www.squidi.net/three/entry.php?id=83 */

  initialize_rendering();

  while (!WindowShouldClose()) {
    process_player_movement(&player, map);

    render(map, objects, objects_len, player);
  }

  cleanup_rendering();

  CloseWindow();

  return 0;
}
