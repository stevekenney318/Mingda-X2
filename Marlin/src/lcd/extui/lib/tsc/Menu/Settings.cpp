#include "Settings.h"
#include "../TSC_Menu.h"
#include "../../../../../module/settings.h"
#include "../../../../../feature/powerloss.h"
#if ENABLED(R_B_LED)
#include "../../../../../feature/leds/leds.h"
#endif
#ifdef WS2812_LED
#include "../../../../../feature/leds/leds.h"
#endif 

SETTINGS infoSettings, // Settings para
         lastSettings; // Last Settings para
extern uint32_t TSC_Para[7]; 
uint32_t lastTSC_Para[7];
static bool needSave = false;
#ifdef WS2812_LED
static uint8_t color_num = 0;
#endif
static bool setting_unlock = true;
// extern PrintJobRecovery recovery;

#ifdef FIL_RUNOUT_PIN
  #define ITEM_RUNOUT_NUM 2
  const ITEM itemRunout[ITEM_RUNOUT_NUM] = {
  // icon                       label
    {ICON_RUNOUT_OFF,           LABEL_RUNOUT_OFF},
    {ICON_RUNOUT_ON,            LABEL_RUNOUT_ON},
  };
  const  uint8_t item_runout[ITEM_RUNOUT_NUM] = {FILAMENT_RUNOUT_OFF, FILAMENT_RUNOUT_ON};
  static uint8_t item_runout_i = 0;
#endif


// Reset settings data
void infoSettingsReset(void)
{
  infoSettings.baudrate = 115200;
  infoSettings.language = TFT_ENGLISH;
  infoSettings.mode = SERIAL_TSC;
  infoSettings.runout = 1;
  // infoSettings.rotate_ui = 0;
  // infoSettings.bg_color = ST7920_BKCOLOR;
  // infoSettings.font_color = ST7920_FNCOLOR;
  infoSettings.silent = 0;
  infoSettings.auto_off = 0;
  infoSettings.machine_flags = 0xffffffff;
}

void loopCheckSettings(void)
{
  static uint32_t lastTime = 0;
  if (millis() - lastTime > 1000)
  {
    lastTime = millis();
    if (memcmp(&infoSettings, &lastSettings, sizeof(SETTINGS))
     || memcmp(lastTSC_Para, TSC_Para, sizeof(TSC_Para)))
    {
      memcpy(&lastSettings, &infoSettings, sizeof(SETTINGS));
      memcpy(lastTSC_Para, TSC_Para, sizeof(TSC_Para));
      storePara();
      //MYSERIAL1.println("Settings stored!");
    }
  }
}

// Version infomation
GUI_RECT lcdFullRect = {0, 0, LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL};
void menuCallBackInfo(void)
{
  KEY_VALUES key_num = (KEY_VALUES)KEY_GetValue(1, &lcdFullRect);
  if (key_num != KEY_IDLE)
  {
   infoMenu.cur--;
  }
}

void menuInfo(void)
{
  const char* hardware = "Board   : " BOARD_INFO_NAME ROBOT_NAME CHIP_NAME;
  char* firmware;
  if(get_MachineFlag()>>MF_MAXTEMP){
    firmware = "Firmware: " SHORT_BUILD_VERSION MATERIALS_NAME " (" __DATE__") ";
  }
  else{
    firmware = "Firmware: " SHORT_BUILD_VERSION MATERIALS_NAME_TI " (" __DATE__") ";
  }
  
  uint16_t HW_X = (LCD_WIDTH_PIXEL - GUI_StrPixelWidth((uint8_t *)hardware))/2;
  uint16_t FW_X = (LCD_WIDTH_PIXEL - GUI_StrPixelWidth((uint8_t *)firmware))/2;
  uint16_t centerY = LCD_HEIGHT_PIXEL/2;
  uint16_t startX = min(HW_X, FW_X);  
  GUI_Clear(BK_COLOR);
  GUI_DispString(startX, centerY - BYTE_HEIGHT, (uint8_t *)hardware);
  GUI_DispString(startX, centerY, (uint8_t *)firmware);
  GUI_DispStringInRect(20, LCD_HEIGHT_PIXEL - (BYTE_HEIGHT*2), LCD_WIDTH_PIXEL-20, LCD_HEIGHT_PIXEL, textSelect(LABEL_TOUCH_TO_EXIT));

  TSC_ReDrawIcon = 0; // invalid icon redraw function
  menuSetFrontCallBack(menuCallBackInfo);
}


// MENUITEMS settingsItems = {
// // title
// LABEL_SETTINGS,
// // icon                       label
//  {{ICON_SCREEN_SETTINGS,      LABEL_SCREEN_SETTINGS},
//   {ICON_MACHINE_SETTINGS,     LABEL_MACHINE_SETTINGS},
//   {ICON_FEATURE_SETTINGS,     LABEL_FEATURE_SETTINGS},
//   {ICON_SCREEN_INFO,          LABEL_SCREEN_INFO},
//   {ICON_BACKGROUND,           LABEL_BACKGROUND},
//   {ICON_BACKGROUND,           LABEL_BACKGROUND},
//   {ICON_BACKGROUND,           LABEL_BACKGROUND},
//   {ICON_BACK,                 LABEL_BACK},}
// };

MENUITEMS settingsItems = {
// title
LABEL_SETTINGS,
// icon                       label
 {
  // {ICON_SCREEN_SETTINGS,      LABEL_SCREEN_SETTINGS},
  {ICON_LANGUAGE,             LABEL_LANGUAGE}, 
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_SCREEN_INFO,          LABEL_SCREEN_INFO},
  // {ICON_BABYSTEP,             LABEL_BABYSTEP},
  #if 0//ENABLED(NEW_BOARD)
    {ICON_FEATURE_SETTINGS,     LABEL_SETTINGS},
    {ICON_FEATURE_SETTINGS,     LABEL_SETTINGS},
  #else
    // {}
    {ICON_PROBE_OFFSET,         LABEL_PROBE_OFFSET},
    // {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
  #endif
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACK,                 LABEL_BACK},}
};
#ifdef BEEPER_PIN // Speaker
  #define BUZZER_KEY_INDEX KEY_ICON_1

  #define ITEM_SILENT_NUM 2
  const ITEM itemSilent[ITEM_SILENT_NUM] = {
  // icon                       label
    {ICON_SILENT_OFF,           LABEL_SILENT_OFF},
    {ICON_SILENT_ON,            LABEL_SILENT_ON},
  };
  const  uint8_t item_silent[ITEM_SILENT_NUM] = {0, 1};
  static uint8_t item_silent_i = 0;
  static uint8_t item_silent_j = 0;
#endif

#if UPDATE_BOOT_FROM_SCREEN
void gotoBack(void){}
void gotoUpdateBoot(void){
  uint8_t res = 0;
  char *context;
  uint16_t old_color = GUI_GetColor();
  // infoMenu.cur--;
  idle();
  // key_lock = true;
  setDialogInfo(&bottomSingleBtn, textSelect(LABEL_TIPS), (uint8_t*)"Updating...", textSelect(LABEL_CONTINUE), NULL, gotoBack, NULL);
  infoMenu.menu[++infoMenu.cur] = menuDialog;
  idle();
  res = boot.updateBootloader();
  switch(res){
    case 0:{
      GUI_SetColor(RED);
      context = "Update error: Open error";
    }break;
    case 1:{
      GUI_SetColor(GREEN);
      context = "Update completed!   ";
    }break;
    case 2:{
      GUI_SetColor(RED);
      context = "Update error: File size is incorrect";
    }break;
    default:break;
  }
  GUI_DrawWindowInfo(&window, textSelect(LABEL_TIPS), (uint8_t *)context);
  GUI_SetColor(old_color);
  // key_lock = false;
}
#endif

void menuCallBackSettings(void)
{
  KEY_VALUES key_num = menuKeyGetValue();
  static uint32_t nextTime_ms = 0;
  static uint8_t touch_clicks = TOUCH_CLICKS-1;
  if(nextTime_ms < millis()){
    touch_clicks = TOUCH_CLICKS-1;
  }
  switch(key_num)
  {
    case KEY_ICON_0: 
      infoSettings.language = (infoSettings.language + 1) % LANGUAGE_NUM;
      menuDrawPage(&settingsItems);
      break;

   #ifdef BEEPER_PIN
    case BUZZER_KEY_INDEX:
      item_silent_i = (item_silent_i + 1) % ITEM_SILENT_NUM;                
      settingsItems.items[key_num] = itemSilent[item_silent_i];
      menuDrawItem(&settingsItems.items[key_num], key_num);
      infoSettings.silent = item_silent[item_silent_i];
      break;
   #endif
    
    // case KEY_ICON_0:
    //   infoMenu.menu[++infoMenu.cur] = menuScreenSettings;
    //   break;    
    // case KEY_ICON_1: 
    //   infoMenu.menu[++infoMenu.cur] = menuRGBSettings;
    //   break;
    // case KEY_ICON_2:
    //   infoMenu.menu[++infoMenu.cur] = menuFeatureSettings;
    //   break;

   #ifdef FIL_RUNOUT_PIN
    case KEY_ICON_2:
      item_runout_i = (item_runout_i + 1) % ITEM_RUNOUT_NUM;
      settingsItems.items[key_num] = itemRunout[item_runout_i];
      menuDrawItem(&settingsItems.items[key_num], key_num);
      infoSettings.runout = item_runout[item_runout_i];
      ExtUI::setFilamentRunoutEnabled(infoSettings.runout);
      needSave = true;
      break;
   #endif
    
    case KEY_ICON_3:
      infoMenu.menu[++infoMenu.cur] = menuInfo;
      break;
    case KEY_ICON_4:
      setLO_flag(true);
      infoMenu.menu[++infoMenu.cur] = menuSetLevelingValue;
      break;
   #if UPDATE_BOOT_FROM_SCREEN
    case KEY_ICON_5:{
      if(boot.scanHasFile("boot.bin"))
        setDialogInfo(bottomDoubleBtn, textSelect(LABEL_TIPS), (uint8_t*)"Update now?", textSelect(LABEL_CONTINUE), textSelect(LABEL_CANNEL), gotoUpdateBoot, gotoBack);
      else{
        setDialogInfo(&bottomSingleBtn, textSelect(LABEL_TIPS), (uint8_t*)"No file: boot.bin.", textSelect(LABEL_CONTINUE), NULL, gotoBack, NULL);
      }
      infoMenu.menu[++infoMenu.cur] = menuDialog;
    }break;
   #endif

    case KEY_ICON_6:
      switch (touch_clicks)
      {
        case 0:
          touch_clicks = TOUCH_CLICKS-1;
          infoMenu.menu[++infoMenu.cur] = menuDeveloper;
          break;
        case 1:
          touch_clicks--;
          break;
        case 2:
          touch_clicks--;
          nextTime_ms = (millis() + 1000);
          break;
        case 3:
          touch_clicks--;
          break;
        case 4:
          touch_clicks--;
          break;
        case 5:
          touch_clicks--;
          break;
        
        default:
          break;
      }
      break;

    case KEY_ICON_7:
      if(needSave){
        needSave = false;
        settings.save();
      }
      infoMenu.cur--;
      break;
    
    default:
      break;
  }
  #if ENABLED(WS2812_LED) && 0
    static uint32_t nextTime = 0;
    uint32_t the_color = 0;
    static uint8_t the_color_blue = 0xff;
    static uint8_t the_color_green = 0;
    static uint8_t the_color_red = 0;
    static uint8_t color_i = 2;
    // static uint8_t the_color_blue_i = 0;
    // static uint8_t the_color_green_i = 0;
    // static uint8_t the_color_red_i = 0;
    if(nextTime < millis())
    {
      nextTime = millis() + 10;
      
      // if(the_color_blue==0 && the_color_green==0 && the_color_red==0xff){
      //   the_color_blue = 0xff;
      //   the_color_red = 0;
      // }
      // else if(the_color_blue==0 && the_color_green>0 && the_color_red==0xff) the_color_green--;
      // else{
      //   if(the_color_blue > 0) the_color_blue--;
      //   if(the_color_green < 0xff) the_color_green++;
      //   if(the_color_red < 0xff) the_color_red++;
      // }
      // else if(the_color_green <= 80){
      //   // if(the_color_blue > 0) the_color_blue--;
      //   if(the_color_green < 0xff) the_color_green++;
      // }

      // if(the_color_blue==0xff && the_color_red!=0){
      //   if(the_color_red > 0)
      //     the_color_red--;
      // }
      // else{
      //   if(the_color_green < 0xff)
      //     the_color_green++;
      // }
      // if(the_color_green==0xff && the_color_blue!=0){
      //   if(the_color_blue > 0)
      //     the_color_blue--;
      // }
      // else{
      //   if(the_color_red < 0xff)
      //     the_color_red++;
      // }
      // if(the_color_red==0xff && the_color_green!=0){
      //   if(the_color_green>0)
      //     the_color_green--;
      // }
      // else{
      //   if(the_color_blue < 0xff)
      //     the_color_blue++;
      // }

      switch(color_i){
        case 0:
          the_color_green+=1;
          if(the_color_green == 100) color_i = 4;
          break;
        case 1:
          the_color_blue-=1;
          if(the_color_blue == 0) color_i = 0;
          break;
        case 2:
          the_color_red+=1;
          if(the_color_red == 0xff) color_i = 1;
          break;
        case 3:
          the_color_green-=1;
          if(the_color_green == 0) color_i = 4;
          break;
        case 4:
          if(the_color_green < 0xff) the_color_green+=1;
          the_color_blue+=1;
          if(the_color_blue == 0xff) color_i = 6;
          break;
        case 5:
          the_color_red-=1;
          if(the_color_red == 0) color_i = 0;
          break;
        case 6:
          the_color_blue = 0xff;the_color_red = 0;the_color_green = 0;
          color_i = 2;
          break;
      }
      the_color = the_color_green<<16 | the_color_red<<8 | the_color_blue;
      leds4.set_color(the_color);
    }
  #endif
}

void menuSettings(void)
{
 #ifdef BEEPER_PIN
  for(uint8_t i=0; i<ITEM_SILENT_NUM; i++)
  {
    if(infoSettings.silent == item_silent[i])
    {
      item_silent_i = i;
      settingsItems.items[BUZZER_KEY_INDEX] = itemSilent[i];
    }
  }
 #endif
 #ifdef FIL_RUNOUT_PIN
  for(uint8_t i=0; i<ITEM_RUNOUT_NUM; i++)
  {
    if(infoSettings.runout == item_runout[i])
    {
      item_runout_i = i;
      settingsItems.items[KEY_ICON_2] = itemRunout[item_runout_i];
    }
  }
 #endif

 #if ENABLED(WS2812_LED) && 0
  if(AUTO_MODE == leds4.get_led_mode()){
    item_silent_j = 0;
  }else{ item_silent_j = 1; }
  settingsItems.items[KEY_ICON_4] = itemSilent[item_silent_j];
 #endif

  menuDrawPage(&settingsItems);
  menuSetFrontCallBack(menuCallBackSettings);
}
