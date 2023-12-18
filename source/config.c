#include "config.h"
#include "raylib.h"
#include "utils.h"

#include <stdio.h>
#include <assert.h>

#ifdef __linux__
#include <unistd.h>
#include <limits.h>
#endif

#ifdef _WIN32
#error "windows is not currently supported"
#endif

#ifdef __APPLE__
#error "macos is not currently supported"
#endif

Config config = {0};

/* https://stackoverflow.com/questions/143174/how-do-i-get-the-directory-that-a-program-is-running-from#198099 */
const char *get_game_directory(void) {
  static char game_path[PATH_MAX] = {0};

  ssize_t bytes_read = readlink("/proc/self/exe", game_path, SIZE_OF(game_path));
  game_path[bytes_read] = 0;

  return game_path;
}


#define CONFIG_FILE_NAME "config.bin"
bool try_load_config(void) {
  static char config_path[PATH_MAX] = {0};

  snprintf(config_path, SIZE_OF(config_path), "%s/" CONFIG_FILE_NAME, get_game_directory());

  FILE *fp = fopen(config_path, "rb");
  if (fp == NULL) {
    return false;
  }

  fseek(fp, 0L, SEEK_END);
  ssize_t file_size = ftell(fp);
  rewind(fp);

  assert(file_size == sizeof(Config));

  size_t bytes_read = fread(&config, sizeof(Config), 1, fp);
  assert(bytes_read == sizeof(Config));

  fclose(fp);

  return true;
}

void save_config(void) {
  static char config_path[PATH_MAX] = {0};

  snprintf(config_path, SIZE_OF(config_path), "%s/" CONFIG_FILE_NAME, get_game_directory());

  FILE *fp = fopen(config_path, "wb");
  assert(fp && "cannot open config.bin in the game folder");

  size_t bytes_wrote = fwrite(&config, sizeof(Config), 1, fp);
  assert(bytes_wrote == sizeof(Config));

  fclose(fp);
}

#define DEFAULT_MOUSE_SENSITIVITY 0.7f
#define DEFAULT_ACTION_MENU_MOUSE_SENSITIVITY 0.4f

#define DEFAULT_CRT_CURVATURE 4.0f

void default_config() {
  config.mouse_sensitivity = DEFAULT_MOUSE_SENSITIVITY;
  config.action_menu_mouse_sensitivity = DEFAULT_ACTION_MENU_MOUSE_SENSITIVITY;

  config.crt_curvature = DEFAULT_CRT_CURVATURE;

  config.keybind[KEYBIND_ACTION_NONE] = (Keybind) {
    .type = KEYBIND_TYPE_NONE,
  };

  config.keybind[KEYBIND_ACTION_ACTION_MENU] = (Keybind) {
    .type = KEYBIND_TYPE_MOUSE,
    .mouse_button = MOUSE_BUTTON_RIGHT,
  };

  config.keybind[KEYBIND_ACTION_FIRE] = (Keybind) {
    .type = KEYBIND_TYPE_MOUSE,
    .mouse_button = MOUSE_BUTTON_LEFT,
  };

  config.keybind[KEYBIND_ACTION_MOVE_UP] = (Keybind) {
    .type = KEYBIND_TYPE_KEYBOARD,
    .keyboard_key = KEY_E,
  };

  config.keybind[KEYBIND_ACTION_MOVE_DOWN] = (Keybind) {
    .type = KEYBIND_TYPE_KEYBOARD,
    .keyboard_key = KEY_D,
  };

  config.keybind[KEYBIND_ACTION_MOVE_LEFT] = (Keybind) {
    .type = KEYBIND_TYPE_KEYBOARD,
    .keyboard_key = KEY_S,
  };

  config.keybind[KEYBIND_ACTION_MOVE_RIGHT] = (Keybind) {
    .type = KEYBIND_TYPE_KEYBOARD,
    .keyboard_key = KEY_F,
  };

  config.do_crt_shader = true;
}

bool is_action_key_up(KeybindAction action) {
  switch (config.keybind[action].type) {
  case KEYBIND_TYPE_KEYBOARD: return IsKeyUp((int)config.keybind[action].keyboard_key);
  case KEYBIND_TYPE_MOUSE: return IsMouseButtonUp((int)config.keybind[action].mouse_button);
  default: assert(false && "nuh uh");
  }
}

bool is_action_key_down(KeybindAction action) {
  switch (config.keybind[action].type) {
  case KEYBIND_TYPE_KEYBOARD: return IsKeyDown((int)config.keybind[action].keyboard_key);
  case KEYBIND_TYPE_MOUSE: return IsMouseButtonDown((int)config.keybind[action].mouse_button);
  default: assert(false && "nuh uh");
  }
}

bool is_action_key_pressed(KeybindAction action) {
  switch (config.keybind[action].type) {
  case KEYBIND_TYPE_KEYBOARD: return IsKeyPressed((int)config.keybind[action].keyboard_key);
  case KEYBIND_TYPE_MOUSE: return IsMouseButtonPressed((int)config.keybind[action].mouse_button);
  default: assert(false && "nuh uh");
  }
}

bool is_action_key_released(KeybindAction action) {
  switch (config.keybind[action].type) {
  case KEYBIND_TYPE_KEYBOARD: return IsKeyReleased((int)config.keybind[action].keyboard_key);
  case KEYBIND_TYPE_MOUSE: return IsMouseButtonReleased((int)config.keybind[action].mouse_button);
  default: assert(false && "nuh uh");
  }
}
