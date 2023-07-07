#ifndef _PREHEATMENU_H_
#define _PREHEATMENU_H_

#define RESET_PERHEAT   {{60, 190}, {80, 230}, {60,210}}

#define INFO_OFFSET_X0  54
#define INFO_OFFSET_X1  15
#define INFO_OFFSET_Y0  20
#define INFO_OFFSET_Y1  61

enum{
    THE_PLA=0,
    THE_ABS,
    THE_TPU,
    THE_MATERIAL_NUM,
};  // 枚举材料
typedef enum
{
  BOTH = 0,
  BED_PREHEAT = 1,
  NOZZLE0_PREHEAT = 2,
}TOOLPREHEAT;
enum{
  THE_BED=0,
  THE_NOZZLE,
};

extern uint16_t PreheatTemp[THE_MATERIAL_NUM][2];
const uint16_t originalPreheatTemp[THE_MATERIAL_NUM][2] = RESET_PERHEAT;
extern const GUI_RECT info_rect[];

void menuPreheat(void);
void getCurrentPreheat(uint8_t name, uint16_t *Temp);
void setCurrentPreheat(uint8_t name, uint16_t *Temp);
void drawTemperatureInfo(void);

#endif

