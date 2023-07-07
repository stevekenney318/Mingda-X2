#ifndef _XLEVELING_
#define _XLEVELING_

#include "../inc/MarlinConfigPre.h"

typedef enum Need_Move_Axis{
  NO_MOVE = 0,
  MOVE_X1,
  MOVE_X2,
  MOVE_ALL,
}N_M_A;
class X_Leveling{
  public:
    void set_X1AxisValue(float v);
    void set_X2AxisValue(float v);
    float get_X1AxisValue(void);
    float get_X2AxisValue(void);
    void set_XLevelingValue(float v);
    float get_XLevelingValue(void);
    void autoLevelingX(void);
    uint8_t analysis_NeedToMove(void);
    void move_XLeveling_Z1(float v);
    void move_XLeveling_Z2(float v);
  private:
    static float x_leveling_1;
    static float x_leveling_2;
    static float x_leveling_value;
};


extern X_Leveling xleveling;

#endif // _XLEVELING_
