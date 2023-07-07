#include "../TSC_Menu.h"
#include "../../../../../feature/powerloss.h"
//1title, ITEM_PER_PAGE items
MENUITEMS developerItems = {
  // title
  LABEL_001_MM,
  // icon                       label
  { {ICON_START,                LABEL_BACKGROUND},
    {ICON_LEVELING,             LABEL_LEVELING},
    {ICON_RUNOUT_ON,            LABEL_RUNOUT_ON},
    {ICON_UNLOAD,               LABEL_UNLOAD},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACK,                 LABEL_BACK},  }
};
ITEM itemsPowerLoss[2] = {
  {ICON_SHUTDOWN,    LABEL_BACKGROUND},
  {ICON_EM_STOP,     LABEL_BACKGROUND},
};

MENUITEMS developer2Items = {
  // title
  LABEL_005_MM,
  // icon                       label
  { {ICON_HEAT,                 LABEL_BACKGROUND},
    {ICON_LEVELING,             LABEL_LEVELING},
    {ICON_RUNOUT_ON,            LABEL_RUNOUT_ON},
    {ICON_UNLOAD,               LABEL_UNLOAD},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACK,                 LABEL_BACK},  }
};

#define TEST_GCODE_NUM 9
static char Gcode_str[TEST_GCODE_NUM][24] = {
  "M104 S170",
  "M140 S50",
  "G28",
  "M302 P1",
  "M106 S255",
  "G1 X10 Y10 Z10",
  "G1 E10",
  "G1 E-10",
  "M302 P0",};
#define READ_FIL_NUM 5
static uint8_t itemsPowerLoss_i = 0;

void menuCallBackDeveloper(void)
{
  KEY_VALUES key_num = menuKeyGetValue();
  static uint16_t touch_num = IDLE_TOUCH;
  static uint8_t send_gcode_num = 0;
  // static bool Fil_pin_flag = false;
  static uint32_t nextTime_ms = 0;
  static uint8_t touch_clicks = TOUCH_CLICKS-1;
  if(nextTime_ms < millis()){
    touch_clicks = TOUCH_CLICKS-1;
  }
  switch(key_num)
  {
    case KEY_ICON_0:
      touch_num = KEY_ICON_0;
      break;
    
    case KEY_ICON_1:
      storeCmd("G28\n");
      storeCmd("G29\n");
      break;

    case KEY_ICON_2:
      // for(int read_num=0; read_num<READ_FIL_NUM; read_num++)
      // {
      //   if(READ(FIL_RUNOUT_PIN) != FIL_RUNOUT_STATE)    // 断料检测开关触发（有料）,当开关断开意味着有料，接通无料
      //   {
      //     Fil_pin_flag = true;
      //   }else
      //   {
      //     Fil_pin_flag = false;
      //   }
      // }
      // if(Fil_pin_flag)
      // {
      //   popupReminder((uint8_t  *)"Yes", (uint8_t  *)"Has filament runout");
      // }else
      // {
      //   popupReminder((uint8_t  *)"No", (uint8_t  *)"No filament runout");
      // }
      infoMenu.menu[++infoMenu.cur] = menuEndstopStates;
      break;

    case KEY_ICON_3:
      mustStoreCmd("M302 P1\n");
      infoMenu.menu[++infoMenu.cur] = menuExtrude2;
      break;

    #if ENABLED(NEW_BOARD)
      case KEY_ICON_4:
        itemsPowerLoss_i = (itemsPowerLoss_i+1)%2;
        if(recovery.checkIfEnabled()){
          storeCmd("M413 S0\n");   // 关闭断电续打
          storeCmd("M500\n");
        }else{
          storeCmd("M413 S1\n");   // 开启断电续打
          storeCmd("M500\n");
        }
        developerItems.items[4] = itemsPowerLoss[itemsPowerLoss_i];
        menuDrawItem(&developerItems.items[4], 4);
        break;
      case KEY_ICON_5:
        TERN_(HAS_CURRENT_SAMPLE, infoMenu.menu[++infoMenu.cur] = menuTestC);
        break;
      case KEY_ICON_6:
        switch (touch_clicks)
        {
          case 0:
            touch_clicks = TOUCH_CLICKS-1;
            infoMenu.menu[++infoMenu.cur] = menuDeveloper2;
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
    #endif

    case KEY_ICON_7:
      infoMenu.cur--; 
      break;
    default:
      break;
  }
  // 发送测试的G代码，注意测试G代码比较多，需要排队入队，防止丢失
  if (touch_num == KEY_ICON_0)
  {
    if(storeCmd(Gcode_str[send_gcode_num]))
    {
      send_gcode_num++;
    }
    if(send_gcode_num>=TEST_GCODE_NUM)
    {
      send_gcode_num = 0;
      touch_num = IDLE_TOUCH;
    }
  }
}
void menuDeveloper()
{
  if(recovery.checkIfEnabled()){
    itemsPowerLoss_i = 0;
  }else{
    itemsPowerLoss_i = 1;
  }
  developerItems.items[4] = itemsPowerLoss[itemsPowerLoss_i];
  menuDrawPage(&developerItems);
  menuSetFrontCallBack(&menuCallBackDeveloper);
}

void menuCallBackDeveloper2()
{
  KEY_VALUES key_num = menuKeyGetValue();
  static uint8_t heat_num = 0;
  switch (key_num)
  {
    case KEY_ICON_0:
      if ((heat_num++)%2)
        WRITE(HEATER_0_PIN, LOW);
      else
        WRITE(HEATER_0_PIN, HIGH);
      break;
    case KEY_ICON_1:
      #if 0
        char cmd[16];
        sprintf_P(cmd, "G1 X%dY%dZ5\n", X_MAX_BED/2, Y_BED_SIZE/2);
        mustStoreCmd("M420 S1\n");
        mustStoreCmd("G28\n");
        mustStoreCmd(cmd);
        mustStoreCmd("G1 Z0\n");
       #if ENABLED(LEVELING_OFFSET)
        infoMenu.menu[++infoMenu.cur] = menuSetLevelingOffset;
       #else
        infoMenu.menu[++infoMenu.cur] = menuBabyStep;
       #endif
      #else
        setLO_flag(true);
        infoMenu.menu[++infoMenu.cur] = menuSetLevelingValue;
      #endif
      break;
    case KEY_ICON_2:
      TERN_(USE_X_LEVELING, infoMenu.menu[++infoMenu.cur] = menuXLeveling);
      break;
    case KEY_ICON_6: mustStoreCmd("M240\n"); break;
    case KEY_ICON_7:
      infoMenu.cur--; 
      break;

    default:
      break;
  }

}
void menuDeveloper2()
{
  menuDrawPage(&developer2Items);
  menuSetFrontCallBack(&menuCallBackDeveloper2);
}

