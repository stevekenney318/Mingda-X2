#include "../TSC_Menu.h"
#include "../../../../../module/xleveling.h"
#include "../../../../../module/endstops.h"

#if ENABLED(USE_X_LEVELING)

//1 title, ITEM_PER_PAGE items(icon+label) 
MENUITEMS XLevelingItems = {
//title
  LABEL_X_LEVELING,
//icon                        label
 {{ICON_Z_DEC,                LABEL_Z_DEC},
  {ICON_Z_INC,                LABEL_Z_INC},
  {ICON_Z_DEC,                LABEL_Z_DEC},
  {ICON_Z_INC,                LABEL_Z_INC},
  {ICON_LEVELING,             LABEL_X_LEVELING},
  {ICON_01_MM,                LABEL_01_MM},
  {ICON_EEPROM_SAVE,          LABEL_EEPROM_SAVE},
  {ICON_BACK,                 LABEL_BACK},}
};

#define ITEM_XLeveling_UNIT_NUM 3
const ITEM itemXLevelingUnit[ITEM_XLeveling_UNIT_NUM] = {
// icon                       label
 #ifdef QUICK_PRINT
  {ICON_005_MM,               LABEL_005_MM},
 #else
  {ICON_001_MM,               LABEL_001_MM},
 #endif
  {ICON_01_MM,                LABEL_01_MM},
  {ICON_1_MM,                 LABEL_1_MM},
};
#ifdef QUICK_PRINT
const float item_x_leveling_unit[ITEM_XLeveling_UNIT_NUM] = {0.05f, 0.1f, 1};
#else
const float item_x_leveling_unit[ITEM_XLeveling_UNIT_NUM] = {0.01f, 0.1f, 1};
#endif

static uint8_t item_x_leveling_num = 0;

static void redrawXLValue(void)
{
  GUI_SetBkColor(TITLE_COLOR);

  // GUI_DispString(START_X+1*SPACE_X+1*ICON_WIDTH, (TITLE_END_Y-10-BYTE_HEIGHT)/2,(uint8_t *)"Z:");
  // GUI_DispFloat(START_X+1*SPACE_X+1*ICON_WIDTH+2*BYTE_WIDTH, (TITLE_END_Y-10-BYTE_HEIGHT)/2, current_position.z, 4, 2, LEFT);

  GUI_DispString(START_X+2*SPACE_X+2*ICON_WIDTH, (TITLE_END_Y-10-BYTE_HEIGHT)/2,(uint8_t *)"Z: ");
  GUI_DispFloat(START_X+2*SPACE_X+2*ICON_WIDTH+2*BYTE_WIDTH, (TITLE_END_Y-10-BYTE_HEIGHT)/2, current_position.z, 4, 2, LEFT);
  GUI_DispString(START_X+3*SPACE_X+3*ICON_WIDTH, (TITLE_END_Y-10-BYTE_HEIGHT)/2,(uint8_t *)"ZL: ");
  GUI_DispFloat(START_X+3*SPACE_X+3*ICON_WIDTH+2*BYTE_WIDTH, (TITLE_END_Y-10-BYTE_HEIGHT)/2, xleveling.get_XLevelingValue(), 4, 2, LEFT);

  GUI_SetBkColor(BK_COLOR);
}
void menuCallBackXLeveling(void)
{
  static bool X_L_lock = false;
  static float xl1 = 0.0f, xl2 = 0.0f, z_1 = 0.0f;

  if((xl1 != xleveling.get_XLevelingValue()) /*|| (xl2 != xleveling.get_X2AxisValue())*/ || (z_1 != current_position.z)){
    redrawXLValue();
    xl1 = xleveling.get_XLevelingValue();
    // xl2 = xleveling.get_X2AxisValue();
    z_1 = current_position.z;
  }
  
  if(X_L_lock) return;

  X_L_lock = true;
  KEY_VALUES key_num = menuKeyGetValue();
  
  switch(key_num)
  {
    case KEY_ICON_0:
      xleveling.move_XLeveling_Z1(-item_x_leveling_unit[item_x_leveling_num]);
      xleveling.set_XLevelingValue(xleveling.get_XLevelingValue()-item_x_leveling_unit[item_x_leveling_num]);
      break;
    case KEY_ICON_1:
      xleveling.move_XLeveling_Z1(+item_x_leveling_unit[item_x_leveling_num]);
      xleveling.set_XLevelingValue(xleveling.get_XLevelingValue()+item_x_leveling_unit[item_x_leveling_num]);
      break;
    case KEY_ICON_2:
      xleveling.move_XLeveling_Z2(-item_x_leveling_unit[item_x_leveling_num]);
      xleveling.set_XLevelingValue(xleveling.get_XLevelingValue()+item_x_leveling_unit[item_x_leveling_num]);
      break;
    case KEY_ICON_3:
      xleveling.move_XLeveling_Z2(+item_x_leveling_unit[item_x_leveling_num]);
      xleveling.set_XLevelingValue(xleveling.get_XLevelingValue()-item_x_leveling_unit[item_x_leveling_num]);
      break;

    case KEY_ICON_4:
      xleveling.autoLevelingX();
      break;

    case KEY_ICON_5:
      item_x_leveling_num = (item_x_leveling_num+1) % ITEM_XLeveling_UNIT_NUM;
      XLevelingItems.items[KEY_ICON_5] = itemXLevelingUnit[item_x_leveling_num];
      menuDrawItem(&XLevelingItems.items[KEY_ICON_5], KEY_ICON_5);
      break;

    case KEY_ICON_6:
      if(settings.save()){
        popupReminder_SF(textSelect(LABEL_SAVE_POPUP),textSelect(LABEL_EEPROM_SAVE_SUCCESS), true);
      }else{
        popupReminder_SF(textSelect(LABEL_SAVE_POPUP),textSelect(LABEL_EEPROM_SAVE_FAILED), false);
      }
      break;

    case KEY_ICON_7:
      infoMenu.cur--;
      break;

    default: break;
  }
  X_L_lock = false;
}

void menuXLeveling(void)
{
  XLevelingItems.items[KEY_ICON_5] = itemXLevelingUnit[item_x_leveling_num];
  menuDrawPage(&XLevelingItems);
  redrawXLValue();
  menuSetFrontCallBack(menuCallBackXLeveling);
}

#endif
