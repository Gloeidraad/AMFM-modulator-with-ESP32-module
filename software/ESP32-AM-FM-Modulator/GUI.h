#ifndef _GUI_H_
#define _GUI_H_

#define GUI_RESULT_NOP                  0
#define GUI_RESULT_TEMP_PAUSE_PLAY    101
#define GUI_RESULT_PAUSE_PLAY         102
#define GUI_RESULT_NEXT_PLAY          103
#define GUI_RESULT_PREV_PLAY          104
#define GUI_RESULT_SEEK_PLAY          105
#define GUI_RESULT_TIMEOUT            106
#define GUI_RESULT_NEW_AM_FREQ        107
#define GUI_RESULT_NEW_FM_FREQ        108
#define GUI_RESULT_NEW_AF_SOURCE      109
#define GUI_RESULT_NEW_TV_CHANNEL     110
#define GUI_RESULT_NEW_VID_IMAGE      111
//#define GUI_RESULT_NEW_PATTERN_FILE_NAME  112
//#define GUI_RESULT_TOGGLE_INTERLACE       113
#define GUI_RESULT_NEW_OUTPUT_SEL     114
#define GUI_RESULT_NEW_AM_TRIM        115
#define GUI_RESULT_NEW_AM_DEPTH       116
#define GUI_RESULT_NEW_FM_MODULATION  117
#define GUI_RESULT_NEW_FM_MOD_TYPE    118
#define GUI_RESULT_NEW_SSID           119
#define GUI_RESULT_NEW_FM_PGA         120
#define GUI_RESULT_NEW_WIFI_TX_LEVEL  121
#define GUI_RESULT_NEW_BT_TX_LEVEL    122
#define GUI_RESULT_NEW_TRACK          123

void GuiInit();
int  GuiCallback(int key_press);

#endif // _GUI_H_
