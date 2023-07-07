#include "xleveling.h"
#include "motion.h"
#include "planner.h"
#include "stepper.h"
#include "endstops.h"
#include "../gcode/gcode.h"

#ifdef USE_X_LEVELING

X_Leveling xleveling;
float X_Leveling::x_leveling_1 = 0.0f;
float X_Leveling::x_leveling_2 = 0.0f;
float X_Leveling::x_leveling_value = 0.0f;

uint8_t X_Leveling::analysis_NeedToMove(void){
  uint8_t res = 0;
  // 光电开关未触发
  if(READ(Z_MIN_PIN) == Z_MIN_ENDSTOP_INVERTING){ res += MOVE_X1; }
  if(READ(Z2_MIN_PIN) == Z2_MIN_ENDSTOP_INVERTING){ res += MOVE_X2; }

  return res;
}
void X_Leveling::autoLevelingX(void){
  uint8_t res = 0;
  uint16_t i=0;
  float z=0.0f;

  set_X1AxisValue(0);
  set_X2AxisValue(0);
  set_XLevelingValue(0);
  gcode.process_subcommands_now("G28 Z");
  planner.synchronize();
  gcode.process_subcommands_now("M84");
  gcode.process_subcommands_now("M420 S0");
  wait_for_user_response(1000);
  gcode.process_subcommands_now("G0 Z5");
  planner.synchronize();
  // gcode.process_subcommands_now("M92 Z10");
  z = current_position.z;
  for(i = 0; i<1000; i++){
    if((READ(Z_MIN_PIN) != Z_MIN_ENDSTOP_INVERTING) || (READ(Z2_MIN_PIN) != Z2_MIN_ENDSTOP_INVERTING)) break;
    do_blocking_move_to_z(z-0.1*i);
  }
  stepper.set_separate_multi_axis(true);
  endstops.enable(false); // Enable endstops for next homing move
  endstops.set_live_state(0);
  res = analysis_NeedToMove();
  z = current_position.z;
  switch (res)
  {
    case MOVE_X1:
      stepper.set_z2_lock(true);
      for(i=0; i<500; i++){
        if(READ(Z_MIN_PIN) != Z_MIN_ENDSTOP_INVERTING) break;
        do_blocking_move_to_z(z-(0.01*i));
      }
      if(i<500) set_XLevelingValue(0.01*i);
      // do_blocking_move_to_z(0.01*i);
      stepper.set_z2_lock(false);
      break;
    
    case MOVE_X2:
      stepper.set_z1_lock(true);
      for(i=0; i<500; i++){
        if(READ(Z2_MIN_PIN) != Z2_MIN_ENDSTOP_INVERTING) break;
        do_blocking_move_to_z(z-(0.01*i));
      }
      if(i<500) set_XLevelingValue(-0.01*i);
      // do_blocking_move_to_z(0.01*i);
      stepper.set_z1_lock(false);
      break;

    default:break;
  }
  stepper.set_separate_multi_axis(false);
}
void X_Leveling::set_X1AxisValue(float v){
  x_leveling_1 = v;
}
void X_Leveling::set_X2AxisValue(float v){
  x_leveling_2 = v;
}
float X_Leveling::get_X1AxisValue(void){
  return x_leveling_1;
}
float X_Leveling::get_X2AxisValue(void){
  return x_leveling_2;
}
void X_Leveling::set_XLevelingValue(float v){
  x_leveling_value = v;
}
float X_Leveling::get_XLevelingValue(void){
  return x_leveling_value;
}
void X_Leveling::move_XLeveling_Z1(float v){
  float z = current_position.z;

  stepper.set_separate_multi_axis(true);
  endstops.enable(false); // Enable endstops for next homing move
  endstops.set_live_state(0);

  stepper.set_z2_lock(true);
  do_blocking_move_to_z(z+v);
  stepper.set_z2_lock(false);
  
  stepper.set_separate_multi_axis(false);
}
void X_Leveling::move_XLeveling_Z2(float v){
  float z = current_position.z;

  stepper.set_separate_multi_axis(true);
  endstops.enable(false); // Enable endstops for next homing move
  endstops.set_live_state(0);

  stepper.set_z1_lock(true);
  do_blocking_move_to_z(z+v);
  stepper.set_z1_lock(false);
  
  stepper.set_separate_multi_axis(false);
}

#endif
