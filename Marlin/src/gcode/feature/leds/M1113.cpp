#include "../../../inc/MarlinConfig.h"

#ifdef WS2812_LED
  #include "../../gcode.h"
  #include "../../../feature/leds/leds.h"

  void GcodeSuite::M1113(){
    if(parser.seenval('S')){
      bool a = parser.value_bool();
      if(a){
        leds4.set_led_mode(AUTO_MODE);
      }else{
        leds4.set_led_mode(0);
        leds4.set_color((uint32_t)0);
      }
    }
    if(parser.seenval('C')){
      int b = parser.value_int();
      if(b<0x1000000){
        b = 0xffffff;
      }
      leds4.set_led_mode(b);
      leds4.set_color((uint32_t)b);
    }

    int the_led_mode = leds4.get_led_mode();
    if(the_led_mode == AUTO_MODE)
      SERIAL_ECHOPAIR_P("The led mode: ", "auto\n");
    else if(the_led_mode == 0)
      SERIAL_ECHOPAIR_P("The led mode: ", "off\n");
    else
      SERIAL_ECHOPAIR_P("The led color: ", the_led_mode, "\n");
  }
#endif
