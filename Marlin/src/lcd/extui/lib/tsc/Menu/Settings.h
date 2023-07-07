#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "stdint.h"

typedef enum
{
  SERIAL_TSC = 0,
  LCD12864,
  LCD2004  
}LCD_MODE;

typedef struct
{
  uint32_t baudrate;
  uint8_t  language;
  uint8_t  mode;
  uint8_t  runout;
  uint8_t  rotate_ui;
  uint16_t bg_color;
  uint16_t font_color;
  uint8_t  silent;
  uint8_t  auto_off;
  uint32_t machine_flags; // 详见下面解析
}SETTINGS;
// this is machine flag:
// bit0:Fan speed setting   // bit1:The max Temp

#define TOUCH_CLICKS 3

extern SETTINGS infoSettings;

void infoSettingsReset(void);
void loopCheckSettings(void);
void menuSettings(void);
void menuInfo(void);

#endif
