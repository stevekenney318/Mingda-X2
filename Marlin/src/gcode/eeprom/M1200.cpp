#include "../gcode.h"
#include "../../lcd/extui/lib/tsc/flashStore.h"
#include "../../lcd/extui/lib/tsc/LCD_Init.h"

// show M1200 info
void GcodeSuite::report_M1200(){
  const uint32_t old_machine_flags = get_MachineFlag();
  const uint8_t ui = get_infoSettings('U');
  SERIAL_ECHOLNPAIR("       Fan:", ((old_machine_flags>>MF_FAN)&0x1));
  SERIAL_ECHOLNPAIR("       Max Temp:", ((old_machine_flags>>MF_MAXTEMP)&0x1));
  SERIAL_ECHOLNPAIR("       ui:", ui);
}
// machine flag
void GcodeSuite::M1200() {
  if (parser.seen('P')){
    const uint32_t old_machine_flags = get_MachineFlag();
    uint32_t new_machine_flags = get_MachineFlag();
    const int p = parser.value_byte();
    if(parser.seenval('S')){
      const int s = parser.value_byte();
      if(s){
        new_machine_flags |= 1<<p;
      }
      else{
        new_machine_flags &= ~(1<<p);
      }
      if(new_machine_flags != get_MachineFlag()){
        set_MachineFlag(new_machine_flags);
        storePara();
      }
    }
  }
  if(parser.seen('U')){
    const uint8_t ui = parser.value_byte();
    const uint8_t old_ui = (uint8_t)get_infoSettings('U');
    if(ui < 5 && ui != old_ui){
      set_infoSettings('U', ui);
      LCD_RefreshDirection(ui);
      storePara();
    }
  }
  SERIAL_ECHOLNPGM("M1200:");
  report_M1200();
}
