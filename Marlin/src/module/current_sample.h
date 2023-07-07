#pragma once

#include "../inc/MarlinConfig.h"

enum CurrentItemName : uint8_t
{
  MOTORS_OCP=0,
  HOTEND_OCP,
  BED_OCP,
};
void motors_shutdown_callback(void);
void hotend_shutdown_callback(void);
void bed_shutdown_callback(void);
void my_external_interrupt_enable(int _pin, void (*callback)(void), uint32_t mode);
void my_external_interrupt_disable(int _pin);

class Current {
  public:
    static void init();
    static int analog_to_current(CurrentItemName item, int raw);
    static int get_machine_raw(CurrentItemName item);
    static float get_machine_current(CurrentItemName item);
    static bool hint_no24v(CurrentItemName item);
  private:
};

extern Current machine_current;
