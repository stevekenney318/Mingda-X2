#include "../TSC_Menu.h"
#include "../../../../../module/current_sample.h"

#if HAS_CURRENT_SAMPLE
static MENUITEMS TestItems = {
// title
LABEL_BACKGROUND,
// icon                       label
 {{ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND}, //Reserved for gantry position to be added later
  {ICON_BACKGROUND,           LABEL_BACKGROUND}, //Reserved for gantry position to be added later
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},}
  // {ICON_BACK,                 LABEL_BACK},}
};

#define GETTIME     10
#define TC_DATA_1   170
#define TC_DATA_2   320

// static uint32_t motors_num = 0, bed_num = 0, hotend_num = 0;
// static uint8_t ifFirst = 1;
static const GUI_RECT TestCurrentRect_0 = { (int16_t)24, (int16_t) 75, (int16_t)455, (int16_t)100 };
static const GUI_RECT TestCurrentRect_1 = { (int16_t)24, (int16_t)101, (int16_t)455, (int16_t)126 };
static const GUI_RECT TestCurrentRect_2 = { (int16_t)24, (int16_t)127, (int16_t)455, (int16_t)152 };
static const GUI_RECT TestCurrentRect_3 = { (int16_t)24, (int16_t)153, (int16_t)455, (int16_t)178 };

/********************************************************/
void redrawTestCurrent(CurrentItemName item, int raw, int current){
  char tempstr[100];

  memset(tempstr, 0, 100);
  GUI_SetColor(GANTRYLBL_COLOR);
#if 0
  uint16_t motors_A = 0, bed_A = 0, hotend_A = 0;

  // 计算电机的电流(放大1000倍变成mA)
  // 分子:330-->3.3v放大100倍,与下面采样电阻相抵消, 1000-->V变为mV
  // 分母:1024-->ADC分辨率10bit, 50-->放大倍数, 1-->采样电阻0.01欧放大100倍与前面3.3v放大100倍刚好抵消
  motors_A = ((motors_num/GETTIME*330)*1000)/(1024*50*1);
  sprintf_P(tempstr, "Motors ocp: %d, Motors: %dmA", motors_num/GETTIME, motors_A);
  GUI_DispStringInPrect(&TestCurrentRect_0,(uint8_t *)tempstr);

  memset(tempstr, 0, 100);
  GUI_ClearPrect(&TestCurrentRect_1);
  // 计算热床加热的电流(放大1000倍变成mA)
  // 分子:3300-->3.3v放大1000倍,与下面采样电阻相抵消, 1000-->V变为mV
  // 分母:1024-->ADC分辨率10bit, 100-->放大倍数, 2-->采样电阻0.002欧放大1000倍与前面3.3v放大1000倍刚好抵消
 #ifdef R4_PRO
  bed_A = ((bed_num/GETTIME*3300)*1000)/(1024*130*1);
 #else
  bed_A = ((bed_num/GETTIME*3300)*1000)/(1024*100*2);
 #endif
  sprintf_P(tempstr, "Bed ocp: %d, Bed: %dmA", bed_num/GETTIME, bed_A);
  GUI_DispStringInPrect(&TestCurrentRect_1,(uint8_t *)tempstr);

  memset(tempstr, 0, 100);
  GUI_ClearPrect(&TestCurrentRect_2);
  // 计算加热头的电流(放大1000倍变成mA) // 400和600: 0.005 100倍
  // 分子:330-->3.3v放大100倍,与下面采样电阻相抵消, 1000-->V变为mV
  // 分母:1024-->ADC分辨率10bit, 100-->放大倍数, 1-->采样电阻0.01欧放大100倍与前面3.3v放大100倍刚好抵消
  hotend_A = ((hotend_num/GETTIME*330)*1000)/(1024*100*1);
  sprintf_P(tempstr, "Hotend_ocp: %d, Hotend: %dmA", (hotend_num/GETTIME), hotend_A);
  GUI_DispStringInPrect(&TestCurrentRect_2,(uint8_t *)tempstr);
#else
  switch (item)
  {
  case MOTORS_OCP:
    // sprintf_P(tempstr, "Motors_ocp:       , Motors:       mA", raw, current);
    // GUI_ClearPrect(&TestCurrentRect_0);
    // GUI_DispStringInPrect(&TestCurrentRect_0,(uint8_t *)tempstr);
    GUI_ClearRect(TC_DATA_1+1, 75, 228, 100);
    GUI_DispDec(TC_DATA_1+1, 80, raw, 8, RIGHT);
    GUI_ClearRect(TC_DATA_2+1, 75, 364, 100);
    GUI_DispDec(TC_DATA_2+1, 80, current, 8, RIGHT);
    break;
  
  case HOTEND_OCP:
    // sprintf_P(tempstr, "Hotend_ocp:       , Hotend:       mA", raw, current);
    // GUI_ClearPrect(&TestCurrentRect_1);
    // GUI_DispStringInPrect(&TestCurrentRect_1,(uint8_t *)tempstr);
    GUI_ClearRect(TC_DATA_1+1, 101, 228, 126);
    GUI_DispDec(TC_DATA_1+1, 106, raw, 8, RIGHT);
    GUI_ClearRect(TC_DATA_2+1, 101, 364, 126);
    GUI_DispDec(TC_DATA_2+1, 106, current, 8, RIGHT);
    break;

  case BED_OCP:
    // sprintf_P(tempstr, "   Bed_ocp:       ,    Bed:       mA", raw, current);
    // GUI_ClearPrect(&TestCurrentRect_2);
    // GUI_DispStringInPrect(&TestCurrentRect_2,(uint8_t *)tempstr);
    GUI_ClearRect(TC_DATA_1+1, 127, 228, 152);
    GUI_DispDec(TC_DATA_1+1, 132, raw, 8, RIGHT);
    GUI_ClearRect(TC_DATA_2+1, 127, 364, 152);
    GUI_DispDec(TC_DATA_2+1, 132, current, 8, RIGHT);
    break;
  
  default:break;
  }
#endif
}
// void Get_Current(void){
//   motors_num = 0; bed_num = 0; hotend_num = 0;
//   // TERN_(USE_GD32, uint16_t get_motor  = 0;)
//   // TERN_(USE_GD32, uint16_t get_bed    = 0;)
//   // TERN_(USE_GD32, uint16_t get_hotend = 0;)
//   for(uint8_t i=0; i<GETTIME; i++){
//    #if 1//def USE_GD32
//     // get_motor   = (Get_Adc(MOTORS_OCP) >> 2);
//     // get_bed     = (Get_Adc(BED_OCP) >> 2);
//     // get_hotend  = (Get_Adc(HOTEND_OCP) >> 2);
//     HAL_START_ADC(MOTORS_OCP_PIN);
//     motors_num += HAL_READ_ADC();
//     delay(5);
//     HAL_START_ADC(BED_OCP_PIN);
//     bed_num    += HAL_READ_ADC();
//     delay(5);
//     HAL_START_ADC(HOTEND_OCP_PIN);
//     hotend_num += HAL_READ_ADC();
//     delay(5);
//    #else
//     motors_num += Get_Adc(MOTORS_OCP);
//     bed_num    += Get_Adc(BED_OCP);
//     hotend_num += Get_Adc(HOTEND_OCP);
//     delay(5);
//    #endif
//   }
// }
void menuCallBackTestC(void)
{
  static uint32_t nowTime_ms = 0;
  static uint32_t old_motors_raw = 0, old_bed_raw = 0, old_hotend_raw = 0;
  
  KEY_VALUES  key_num = menuKeyGetValue();

  // switch(key_num)
  if (key_num != KEY_IDLE)
  {
    // case KEY_ICON_7:
      infoMenu.cur--;
      // break;

    // default:break;
  }
  // Refresh per 1 sec
  // if (millis() - nowTime_ms > 1000) 
  {
    nowTime_ms = millis();
    int motors_raw = machine_current.get_machine_raw(MOTORS_OCP);
    int hotend_raw = machine_current.get_machine_raw(HOTEND_OCP);
    int bed_raw    = machine_current.get_machine_raw(BED_OCP);
    int current_num = 0;

    if(old_motors_raw != motors_raw)
    {
      old_motors_raw = motors_raw;
      // current_num = (int)machine_current.analog_to_current(MOTORS_OCP, motors_raw);
      current_num = (int)machine_current.get_machine_current(MOTORS_OCP);
      redrawTestCurrent(MOTORS_OCP, motors_raw, current_num);
    }
    if(old_hotend_raw != hotend_raw)
    {
      old_hotend_raw = hotend_raw;
      // current_num = (int)machine_current.analog_to_current(HOTEND_OCP, hotend_raw);
      current_num = (int)machine_current.get_machine_current(HOTEND_OCP);
      redrawTestCurrent(HOTEND_OCP, hotend_raw, current_num);
    }
    if(old_bed_raw != bed_raw)
    {
      old_bed_raw = bed_raw;
      // current_num = (int)machine_current.analog_to_current(BED_OCP, bed_raw);
      current_num = (int)machine_current.get_machine_current(BED_OCP);
      redrawTestCurrent(BED_OCP, bed_raw, current_num);
    }
  }
}

void menuTestC(void)
{
  char tempstr[100];
  #if 0//ENABLED(NEW_BOARD)
    if(ifFirst == 1){
      ifFirst = 3;
      delay(250);
    }
  #endif
  menuDrawPage(&TestItems);
  #if 0
  // Get_Current();
  // redrawTestCurrent();
  #else
  sprintf_P(tempstr, "Motors_ocp:          , Motors:          mA");
  // GUI_ClearPrect(&TestCurrentRect_0);
  GUI_DispStringInRect(0,  75, 479, 100, (uint8_t *)tempstr);
  sprintf_P(tempstr, "Hotend_ocp:          , Hotend:          mA");
  // GUI_ClearPrect(&TestCurrentRect_1);
  GUI_DispStringInRect(0, 101, 479, 126, (uint8_t *)tempstr);
  sprintf_P(tempstr, "   Bed_ocp:          ,    Bed:          mA");
  // GUI_ClearPrect(&TestCurrentRect_2);
  GUI_DispStringInRect(0, 127, 479, 152, (uint8_t *)tempstr);
  
  redrawTestCurrent(MOTORS_OCP, 0, 0);
  redrawTestCurrent(BED_OCP,    0, 0);
  redrawTestCurrent(HOTEND_OCP, 0, 0);
  #endif
  GUI_DispStringInRect(20, LCD_HEIGHT_PIXEL - (BYTE_HEIGHT*2), LCD_WIDTH_PIXEL-20, LCD_HEIGHT_PIXEL, textSelect(LABEL_TOUCH_TO_EXIT));
  menuSetFrontCallBack(menuCallBackTestC);
}

#endif // HAS_CURRENT_SAMPLE
