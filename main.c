/*
  Extended functions for DND
  (C) Maxim Volkov     2019 <Maxim.N.Volkov@ya.ru>
  (C) Xuan Son NGUYEN  2021 <contact@ngxson.com>
*/

#include <libbip.h>

#include "main.h"
#include "BipStatusbarLib.h"
#include "utils.h"

//  screen menu structure - each screen has its own
struct regmenu_ screen_data = {
  55, //  main screen number, value 0-255, for custom windows it is better to take from 50 and above
  1, //  auxiliary screen number (usually 1)
  0, //  0
  dispatch_screen, //  pointer to the function handling touch, swipe, long press
  key_press_screen, //  pointer to the function handling pressing the button
  screen_job, //  pointer to the callback function of the timer  
  0, //  0
  show_screen, //  pointer to the screen display function
  0, //  
  0 //  long press of the button
};
int main(int param0, char **argv) { //  here the variable argv is not defined
  show_screen((void *) param0);
}

void show_screen(void *proc) {
  struct app_data_ **app_data_p = get_ptr_temp_buf_2();
  struct app_data_ *app_data;

  if ((proc == *app_data_p)) {
    app_data = *app_data_p;
    *app_data_p = NULL;
    reg_menu( &screen_data, 0); 
    *app_data_p = app_data;

  } else {
    reg_menu( & screen_data, 0);
    *app_data_p = (struct app_data_ *) pvPortMalloc(sizeof(struct app_data_));
    app_data = *app_data_p;
    _memclr(app_data, sizeof(struct app_data_));
    app_data->proc = get_proc_by_addr(main);
    if (proc && app_data->proc->elf_finish) 
      app_data->ret_f = app_data->proc->elf_finish;
    else 
      app_data->ret_f = show_watchface;
    app_data->current_screen = SCREEN_MAIN;
    app_data->is_finding_phone = 0;
    app_data->main_screen_x = 0;
    app_data->music_is_paused = 1;
    app_data->music_last_btn_x = -1;
    app_data->music_last_btn_y = -1;
    app_data->last_action_tick = get_tick_count();
  }

  set_display_state_value(8, 1);
  set_display_state_value(2, 1);

  draw_screen(app_data);
}

void key_press_screen() {
  struct app_data_ **app_data_p = get_ptr_temp_buf_2();
  struct app_data_ *app_data = *app_data_p;
  if (app_data->current_screen == SCREEN_MUSIC) send_music_command(CMD_AMC_DISABLE);
  if (app_data->current_screen == SCREEN_FLASH) set_flash_light_mode(0);
  show_menu_animate(app_data->ret_f, (unsigned int) show_watchface, ANIMATE_UP);
};

void screen_job() {
  struct app_data_ **app_data_p = get_ptr_temp_buf_2(); 
  struct app_data_ *app_data = *app_data_p;
  if (
    app_data->current_screen == SCREEN_MAIN
    && (get_tick_count() - app_data->last_action_tick > EXIT_AFTER_INACTIVE)
  ) {
    key_press_screen(); // exit
  } else {
    draw_screen(app_data);
  }
}

void vib() {
  vibrate(1, 40, 0);
}

void switch_to_screen(struct app_data_ *app_data, int screen) {
  if (screen == SCREEN_MAIN) {
    if (app_data->current_screen == SCREEN_MUSIC) send_music_command(CMD_AMC_DISABLE);
    if (app_data->current_screen == SCREEN_FLASH) set_flash_light_mode(0);
  } else if (screen == SCREEN_MUSIC) {
    send_music_command(CMD_AMC_ENABLE);
  } else if (screen == SCREEN_FLASH) {
    set_flash_light_mode(1);
  }
  app_data->current_screen = screen;
  draw_screen(app_data);
}

void click_music_btn(struct app_data_ *app_data, int code, int x, int y) {
  vib();
  send_music_command(code);
  app_data->music_last_btn_x = x;
  app_data->music_last_btn_y = y;
  draw_screen(app_data);
}

int click_calc_btn() {
  int result = load_elf_by_name("calc", show_watchface, 0, 0, NULL);
  repaint_screen();
  if (result == ERROR_NONE) {
    Elf_proc_* proc = get_proc_by_addr(main);
    proc->ret_f = NULL;
    elf_finish(main);
    return ERROR_NONE;
  }
  return 0;
}

int dispatch_screen(void *param) {
  struct app_data_ **app_data_p = get_ptr_temp_buf_2(); //  pointer to a pointer to screen data 
  struct app_data_ *app_data = *app_data_p; //  pointer to screen data

  struct gesture_ *gest = param;

  int main_x = app_data->main_screen_x;
  if (main_x != -176 && main_x != 0 && main_x != 176) {
    // don't process touches on performing animation
    return 0;
  }

  if (gest->gesture == GESTURE_CLICK) {


    // main screen
    if (app_data->current_screen == SCREEN_MAIN) {
      if (check_touch_in_range(gest, BTN_X_COL_1_OF_2, BTN_Y_ROW_1_OF_2, BTN_X_COL_2_OF_2, BTN_Y_ROW_2_OF_2)) {
        // DND button
        vib();
        switch_dnd_mode();
        draw_screen(app_data);
      } else if (check_touch_in_range(gest, BTN_X_COL_2_OF_2, BTN_Y_ROW_1_OF_2, BTN_X_END, BTN_Y_ROW_2_OF_2)) {
        // Find phone button
        if (IS_BT_CONNECTED) {
          vib();
          send_host_app_msg(app_data->is_finding_phone ? CMD_FIND_PHONE_STOP : CMD_FIND_PHONE_START);
          app_data->is_finding_phone = ~app_data->is_finding_phone;
          draw_screen(app_data);
        }
      } else if (check_touch_in_range(gest, BTN_X_COL_1_OF_2, BTN_Y_ROW_2_OF_2, BTN_X_COL_2_OF_2, BTN_Y_END)) {
        vib();
        switch_to_screen(app_data, SCREEN_FLASH);
      } else if (check_touch_in_range(gest, BTN_X_COL_2_OF_2, BTN_Y_ROW_2_OF_2, BTN_X_END, BTN_Y_END)) {
        // Calculator button
        vib();
        return click_calc_btn();
      }
    }
    

    // music screen
    else if (app_data->current_screen == SCREEN_MUSIC) {
      if (check_touch_in_range(gest, BTN_X_COL_1_OF_3, BTN_Y_ROW_1_OF_2, BTN_X_COL_2_OF_3, BTN_Y_ROW_2_OF_2)) {
        // back button
        vib();
        switch_to_screen(app_data, SCREEN_MAIN);
      } else if (check_touch_in_range(gest, BTN_X_COL_2_OF_3, BTN_Y_ROW_1_OF_2, BTN_X_COL_3_OF_3, BTN_Y_ROW_2_OF_2)) {
        // vol -
        click_music_btn(app_data, CMD_VOL_DOWN, BTN_X_COL_2_OF_3, BTN_Y_ROW_1_OF_2);
      } else if (check_touch_in_range(gest, BTN_X_COL_3_OF_3, BTN_Y_ROW_1_OF_2, BTN_X_END, BTN_Y_ROW_2_OF_2)) {
        // vol +
        click_music_btn(app_data, CMD_VOL_UP, BTN_X_COL_3_OF_3, BTN_Y_ROW_1_OF_2);
      } else if (check_touch_in_range(gest, BTN_X_COL_1_OF_3, BTN_Y_ROW_2_OF_2, BTN_X_COL_2_OF_3, BTN_Y_END)) {
        // prev
        click_music_btn(app_data, CMD_PREV, BTN_X_COL_1_OF_3, BTN_Y_ROW_2_OF_2);
      } else if (check_touch_in_range(gest, BTN_X_COL_2_OF_3, BTN_Y_ROW_2_OF_2, BTN_X_COL_3_OF_3, BTN_Y_END)) {
        // play & pause
        int _cmd = app_data->music_is_paused ? CMD_PLAY : CMD_PAUSE;
        app_data->music_is_paused = ~app_data->music_is_paused;
        click_music_btn(app_data, _cmd, BTN_X_COL_2_OF_3, BTN_Y_ROW_2_OF_2);
      } else if (check_touch_in_range(gest, BTN_X_COL_3_OF_3, BTN_Y_ROW_2_OF_2, BTN_X_END, BTN_Y_END)) {
        // next
        click_music_btn(app_data, CMD_NEXT, BTN_X_COL_3_OF_3, BTN_Y_ROW_2_OF_2);
      }
    }



    // torch screen
    else if (app_data->current_screen == SCREEN_FLASH) {
      vib();
      switch_to_screen(app_data, SCREEN_MAIN);
    }



    // something wrong
    else {
      switch_to_screen(app_data, SCREEN_MAIN);
    }

  } else if (gest->gesture == GESTURE_SWIPE_UP) {
    // go back to watchface
    if (app_data->current_screen == SCREEN_MUSIC) send_music_command(CMD_AMC_DISABLE);
    if (app_data->current_screen == SCREEN_FLASH) set_flash_light_mode(0);
    return show_menu_animate(app_data->ret_f, (int)main, ANIMATE_UP);


  } else {
    // swipe left/right
    if (app_data->current_screen == SCREEN_MAIN) {
      if (gest->gesture == GESTURE_SWIPE_LEFT) switch_to_screen(app_data, SCREEN_MUSIC);
      if (gest->gesture == GESTURE_SWIPE_RIGHT) switch_to_screen(app_data, SCREEN_FLASH);
    } else if (app_data->current_screen == SCREEN_MUSIC) {
      if (gest->gesture == GESTURE_SWIPE_RIGHT) switch_to_screen(app_data, SCREEN_MAIN);
    } else if (app_data->current_screen == SCREEN_FLASH) {
      if (gest->gesture == GESTURE_SWIPE_LEFT) switch_to_screen(app_data, SCREEN_MAIN);
    }
  }

  app_data->last_action_tick = get_tick_count();

  return 0;
};

// custom function
void draw_screen(struct app_data_ *app_data) {
  int is_doing_animation = update_animation(app_data) || app_data->music_last_btn_x != -1;
  int main_x = app_data->main_screen_x;

  if (is_doing_animation == 0) {
    set_bg_color(COLOR_BLACK);
    fill_screen_bg();
  }
  set_graph_callback_to_ram_1();
  load_font();

  if (-176 < main_x && main_x < 176) {
    // show main screen
    draw_screen_main(main_x, app_data);

  }
  
  if (main_x < 0) {
    // show music screen
    draw_screen_music(main_x + 176, app_data);
  }
  
  if (main_x > 0) {
    // show flash light screen
    draw_screen_flash(main_x - 176);
  }

  // only re-render status bar if...
  if (
    app_data->current_screen != SCREEN_FLASH // not in flash screen
    && is_doing_animation == 0               // AND not doing animation
  ) {
    show_statusbar(3, COLOR_BLACK, COLOR_WHITE);
  }

  if (is_doing_animation) {
    repaint_screen_lines(21, 176);
    set_update_period(1, ANIMATION_FRAME_TIME);
  } else if (app_data->current_screen == SCREEN_FLASH) {
    repaint_screen_lines(0, 176);
    set_update_period(0, 0);
  } else {
    repaint_screen_lines(0, 176);
    set_update_period(1, DISPLAY_UPDATE_PERIOD);
  }
};