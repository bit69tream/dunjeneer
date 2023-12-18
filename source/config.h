#pragma once

#include <raylib.h>

typedef enum {
  KEYBIND_ACTION_NONE,
  KEYBIND_ACTION_ACTION_MENU,
  KEYBIND_ACTION_FIRE,
  KEYBIND_ACTION_MOVE_UP,
  KEYBIND_ACTION_MOVE_DOWN,
  KEYBIND_ACTION_MOVE_LEFT,
  KEYBIND_ACTION_MOVE_RIGHT,
  KEYBIND_ACTION_COUNT,
} KeybindAction;

typedef enum {
  KEYBIND_TYPE_NONE,
  KEYBIND_TYPE_KEYBOARD,
  KEYBIND_TYPE_MOUSE,
} KeybindType;

typedef struct {
  KeybindType type;

  union {
    KeyboardKey keyboard_key;
    MouseButton mouse_button;
  };
} Keybind;

typedef struct {
  Keybind keybind[KEYBIND_ACTION_COUNT];
  float mouse_sensitivity;
  float action_menu_mouse_sensitivity;
  bool do_crt_shader;
} Config;

extern Config config;

bool try_load_config(void);
void default_config(void);

void save_config(void);

bool is_action_key_up(KeybindAction action);
bool is_action_key_down(KeybindAction action);
bool is_action_key_pressed(KeybindAction action);
bool is_action_key_released(KeybindAction action);
