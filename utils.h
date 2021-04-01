#ifndef __UTILS_H__
#define __UTILS_H__

#include <libbip.h>
#include "main.h"
#include "BipStatusbarLib.h"

int check_touch_in_range(struct gesture_ *gest, int from_x, int from_y, int to_x, int to_y) {
  return gest->touch_pos_y > from_y && gest->touch_pos_y < to_y
    && gest->touch_pos_x > from_x && gest->touch_pos_x < to_x;
}

void switch_dnd_mode(struct app_data_ *app_data) {
  char dnd;
  get_generic_data(GENERIC_DATA_DND_MODE, &dnd);
  dnd++; dnd%=3;
  set_generic_data(GENERIC_DATA_DND_MODE, &dnd);
  app_data->state_hash = -1;
}

/*
  Custom button must be EXACTLY 88x78 pixels
  Image for 1st button must be placed at res ID 0000
  Image for 2nd button must be placed at res ID 0001
*/
void draw_custom_button(int id, int x, int y) {
  struct res_params_ res_params;
  get_res_params(INDEX_MAIN_RES, id, &res_params);
  if (res_params.height < 80) {
    show_elf_res_by_id(INDEX_MAIN_RES, id, x, y);
  } else {
    // if the res doesn't have the correct dimensions, use the default
    show_elf_res_by_id(ELF_INDEX_SELF, id == 0 ? RES_IC_A : RES_IC_B, x, y);
  }
}

void draw_screen_main(int x, struct app_data_ *app_data) {
  char dnd;
  int dnd_icon = RES_IC_DND_OFF;
  int find_phone_icon = IS_BT_CONNECTED
    ? (app_data->is_finding_phone ? RES_IC_FIND_ON : RES_IC_FIND_OFF)
    : RES_IC_FIND_BT_OFF;
  get_generic_data(GENERIC_DATA_DND_MODE, &dnd);
  switch (dnd) {
    default: 
    case DND_MODE_OFF:   dnd_icon = RES_IC_DND_OFF;  break;
    case DND_MODE_ON:    dnd_icon = RES_IC_DND_ON;   break;
    case DND_MODE_AUTO:  dnd_icon = RES_IC_DND_AUTO; break;
  }
  show_elf_res_by_id(ELF_INDEX_SELF, dnd_icon,        x+BTN_X_COL_1_OF_2, BTN_Y_ROW_1_OF_2);
  show_elf_res_by_id(ELF_INDEX_SELF, find_phone_icon, x+BTN_X_COL_2_OF_2, BTN_Y_ROW_1_OF_2);
  draw_custom_button(BTN_A_RES_ID,                    x+BTN_X_COL_1_OF_2, BTN_Y_ROW_2_OF_2);
  draw_custom_button(BTN_B_RES_ID,                    x+BTN_X_COL_2_OF_2, BTN_Y_ROW_2_OF_2);
}

void draw_screen_music(int x, struct app_data_ *app_data) {
  show_elf_res_by_id(ELF_INDEX_SELF, RES_MUSIC_SCREEN, x, 20);
  // flash animation on clicking music control buttons
  if (app_data->music_last_btn_x != -1) {
    show_elf_res_by_id(ELF_INDEX_SELF, RES_MUSIC_BTN_CLICKED, app_data->music_last_btn_x, app_data->music_last_btn_y);
    app_data->music_last_btn_x = -1;
    app_data->state_hash = -1;
  }
}

void draw_screen_flash(int x) {
  set_fg_color(COLOR_WHITE);
  draw_filled_rect(0, 0, x+176, 176);
}

int update_animation(struct app_data_ *app_data) {
  if (app_data->current_screen == SCREEN_MAIN) {
    if (app_data->main_screen_x < 0)    { app_data->main_screen_x += ANIMATION_STEP; return 1; }
    if (app_data->main_screen_x > 0)    { app_data->main_screen_x -= ANIMATION_STEP; return 1; }
  } else if (app_data->current_screen == SCREEN_MUSIC) {
    if (app_data->main_screen_x > -176) { app_data->main_screen_x -= ANIMATION_STEP; return 1; }
  } else if (app_data->current_screen == SCREEN_FLASH) {
    if (app_data->main_screen_x < 176)  { app_data->main_screen_x += ANIMATION_STEP; return 1; }
  }
  return 0;
}

int send_music_command(int cmd) {
  unsigned char d[4];
  int size = 2;
  d[0] = 0xFE;
  d[1] = cmd;

  if (IS_BT_CONNECTED) {
    send_host_app_data(0x42, 0x41, size, &d[0], 0);
  }

  return 0;
}

static char backlight_percent[6] = {0, 10, 20, 40, 60, 90};

void set_flash_light_mode(int activate) {
  if (activate) {
    set_display_state_value(4, 1);
    set_backlight_percent(90);
    set_backlight_state(1);
  } else {
    set_display_state_value(4, 0);
    int bl_index = get_backlight_value();
    set_backlight_percent(backlight_percent[bl_index]);
    set_backlight_state(1);
  }
}

/*
  small hack: decide if we should re-render screen
  we calculate state_hash and compare to the old one
*/
int should_update_status(struct app_data_ *app_data) {
  struct datetime_ dt; get_current_date_time(&dt);
  int bt = IS_BT_CONNECTED;
  int state_hash = (bt << 4) + (dt.hour << 2) + dt.min;
  if (app_data->state_hash == state_hash) {
    return 0;
  } else {
    app_data->state_hash = state_hash;
    return 1;
  }
}

#endif