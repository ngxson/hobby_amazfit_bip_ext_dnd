/*
  Extended functions for DND
  (C) Maxim Volkov     2019 <Maxim.N.Volkov@ya.ru>
  (C) Xuan Son NGUYEN  2021 <contact@ngxson.com>
*/


#ifndef __MAIN_H__
#define __MAIN_H__

#define DISPLAY_UPDATE_PERIOD  1000

#define SCREEN_MAIN            0
#define SCREEN_MUSIC           1
#define SCREEN_FLASH           2

#define BTN_A_RES_ID           0
#define BTN_A_APP_INDEX        4
#define BTN_B_RES_ID           1
#define BTN_B_APP_INDEX        5

#define RES_ICON_BT_CON        0
#define RES_ICON_BT_NOT_CON    1
#define RES_IC_DND_OFF         2
#define RES_IC_DND_ON          3
#define RES_IC_DND_AUTO        4
#define RES_IC_FIND_BT_OFF     5
#define RES_IC_FIND_ON         6
#define RES_IC_A               7
#define RES_IC_B               8
#define RES_IC_FIND_OFF        9
#define RES_MUSIC_SCREEN       10
#define RES_MUSIC_BTN_CLICKED  11

#define BTN_Y_ROW_1_OF_2       20
#define BTN_Y_ROW_2_OF_2       98

#define BTN_X_COL_1_OF_2       0
#define BTN_X_COL_2_OF_2       88

#define BTN_X_COL_1_OF_3       0
#define BTN_X_COL_2_OF_3       59
#define BTN_X_COL_3_OF_3       118

#define BTN_Y_END              176
#define BTN_X_END              176

#define CMD_PLAY               0
#define CMD_PAUSE              1
#define CMD_NEXT               3
#define CMD_PREV               4
#define CMD_VOL_UP             5
#define CMD_VOL_DOWN           6
#define CMD_AMC_ENABLE         0xE0
#define CMD_AMC_DISABLE        0xE1

#define ANIMATION_STEP         88   // = (176 / 2) pixels
#define ANIMATION_FRAME_TIME   5
#define EXIT_AFTER_INACTIVE    5100 // = 10 secs

// the data structure for our screen
struct app_data_ {
  void*      ret_f;          //  the address of the return function
	Elf_proc_* proc;
  int        is_finding_phone;
  int        current_screen;
  int        main_screen_x;  // for animation 0 <= x <= 176
  int        last_action_tick;
  int        music_is_paused;
  int        music_last_btn_x;
  int        music_last_btn_y;
  int        state_hash;
};

// template.c
void   show_screen (void *return_screen);
void   key_press_screen();
int   dispatch_screen (void *param);
void   screen_job();
void  draw_screen(struct app_data_ *a);

#endif