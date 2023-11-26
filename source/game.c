#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <math.h>

#include <raylib.h>

#include "rand.h"
#include "level_generator.h"
#include "ui.h"
#include "utils.h"
#include "font.h"
#include "player.h"
#include "audio.h"

int main(void) {
  time_t seed = time(0);
  srand((unsigned int)seed);
  srand48(seed);

  static LevelMap map = {0};

  static LevelObject objects[OBJECTS_MAX] = {0};
  size_t objects_len = 0;

  Player player = {0};

  generate_level(&map, objects, &objects_len, &player.location);

  /* TODO: https://www.squidi.net/three/entry.php?id=83 */

  init_rendering();
  init_audio();

  play_audio();
  while (!WindowShouldClose()) {
    process_player_movement(&player, map);

    update_mouse();

    process_mouse(&player, &map);

    render(map, objects, objects_len, player);
  }

  cleanup_rendering();

  CloseWindow();

  return 0;
}
