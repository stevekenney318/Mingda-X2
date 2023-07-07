#include "current_sample.h"
#include "temperature.h"
#include "../lcd/extui/lib/tsc/TSC_Menu.h"

Current machine_current;
void my_external_interrupt_enable(int _pin, void (*callback)(void), uint32_t mode)
{
  PinName p = digitalPinToPinName(_pin);
  uint32_t port = STM_PORT(p);
  GPIO_TypeDef *gpio = set_GPIO_Port_Clock(port);
  uint32_t ll_pin  = STM_LL_GPIO_PIN(p);
  
  SET_INPUT(_pin);
  stm32_interrupt_enable(gpio, ll_pin, callback, mode);
}
void my_external_interrupt_disable(int _pin)
{
  PinName p = digitalPinToPinName(_pin);
  uint32_t port = STM_PORT(p);
  GPIO_TypeDef *gpio = set_GPIO_Port_Clock(port);
  uint32_t ll_pin  = STM_LL_GPIO_PIN(p);

  HAL_GPIO_DeInit(gpio, ll_pin);
  stm32_interrupt_disable(gpio, ll_pin);
}
// 电机触发过流保护
void motors_shutdown_callback(void)
{
  disable_all_steppers();
  popupReminder(textSelect(LABEL_WARNING), (uint8_t*)"Excessive motors current!");
}
// 喷嘴触发过流保护
void hotend_shutdown_callback(void)
{
  #if HAS_HOTEND
    HOTEND_LOOP() {
      thermalManager.setTargetHotend(0, e);
      thermalManager.temp_hotend[e].soft_pwm_amount = 0;
    }
    #define DISABLE_HEATER(N) WRITE_HEATER_##N(LOW);
    REPEAT(HOTENDS, DISABLE_HEATER);
  #endif
  popupReminder(textSelect(LABEL_WARNING), (uint8_t*)"Excessive nozzle current!");
}
// 热床触发过流保护
void bed_shutdown_callback(void)
{
  #if HAS_HEATED_BED
    thermalManager.setTargetBed(0);
    thermalManager.temp_bed.soft_pwm_amount = 0;
    WRITE_HEATER_BED(LOW);
  #endif
  popupReminder(textSelect(LABEL_WARNING), (uint8_t*)"Excessive bed current!");
}
void Current::init()
{
  // HAL_adc_init();
  #if PIN_EXISTS(MOTORS_OCP)
    HAL_ANALOG_SELECT(MOTORS_OCP_PIN);
  #endif
  #if PIN_EXISTS(BED_OCP)
    HAL_ANALOG_SELECT(BED_OCP_PIN);
  #endif
  #if PIN_EXISTS(HOTEND_OCP)
    HAL_ANALOG_SELECT(HOTEND_OCP_PIN);
  #endif
  #if PIN_EXISTS(SHUTDOWN_HOTEND)
    // my_external_interrupt_enable(SHUTDOWN_HOTEND_PIN, hotend_shutdown_callback, GPIO_MODE_IT_FALLING);
    SET_INPUT_PULLUP(SHUTDOWN_HOTEND_PIN);
  #endif
  #if PIN_EXISTS(SHUTDOWN_BED)
    // my_external_interrupt_enable(SHUTDOWN_BED_PIN, bed_shutdown_callback, GPIO_MODE_IT_FALLING);
    SET_INPUT_PULLUP(SHUTDOWN_BED_PIN);
  #endif
}
#if HAS_CURRENT_SAMPLE
int Current::analog_to_current(CurrentItemName item, int raw)
{
  int result;
  switch(item)
  {
  case MOTORS_OCP:
    // 计算电机的电流(放大1000倍变成mA)
    // 分子:33-->3.3v放大10倍,与下面采样电阻相抵消, 1000-->V变为mV
    // 分母:1<<ADC_RESOLUTION-->ADC分辨率10bit, 5-->放大50倍,但要与后面的采样电阻抵消10倍, 1-->采样电阻0.01欧放大100倍与前面3.3v放大100倍刚好抵消
    // motors_A = ((motors_num/GETTIME*330)*1000)/(1024*50*1);
    result = ((raw/OVERSAMPLENR*33)*1000)/(_BV(ADC_RESOLUTION)*5*1);
    break;
  case HOTEND_OCP:
    // 计算加热头的电流(放大1000倍变成mA) // 400和600: 0.005 100倍
    // 分子:33-->3.3v放大10倍,与下面采样电阻相抵消, 1000-->V变为mV
    // 分母:1<<ADC_RESOLUTION-->ADC分辨率10bit, 10-->放大100倍,但要与后面的采样电阻抵消10倍 1-->采样电阻0.01欧放大100倍与前面3.3v放大100倍刚好抵消
    result = ((raw/OVERSAMPLENR*33)*1000)/(_BV(ADC_RESOLUTION)*10*1);
    break;
  case BED_OCP:
    // 计算热床加热的电流(放大1000倍变成mA)
    // 分子:33-->3.3v放大10倍,与下面采样电阻相抵消, 1000-->V变为mV
    // 分母:1<<ADC_RESOLUTION-->ADC分辨率10bit, 1-->放大100倍,但要与后面采样电阻抵消100倍 2-->采样电阻0.002欧放大1000倍与前面3.3v放大1000倍刚好抵消
   #ifdef R4_PRO
    result = ((raw/OVERSAMPLENR*330)*1000)/(_BV(ADC_RESOLUTION)*13*1);
   #else
    result = ((raw/OVERSAMPLENR*33)*1000)/(_BV(ADC_RESOLUTION)*1*2);
   #endif
    break;
  default:break;
  }
  return result;
}
int Current::get_machine_raw(CurrentItemName item)
{
  int result;
  switch(item)
  {
  case MOTORS_OCP:
    result = TERN0(HAS_MOTORS_OCP, motors_current_info.raw);
    break;
  case HOTEND_OCP:
    result = TERN0(HAS_HOTEND_OCP, hotend_current_info.raw);
    break;
  case BED_OCP:
    result = TERN0(HAS_BED_OCP,    bed_current_info.raw);
    break;
  default:
    break;
  }
  return result;
}
float Current::get_machine_current(CurrentItemName item){
  float result;
  switch(item)
  {
  case MOTORS_OCP:
    result = TERN0(HAS_MOTORS_OCP, motors_current_info.celsius);
    break;
  case HOTEND_OCP:
    result = TERN0(HAS_HOTEND_OCP, hotend_current_info.celsius);
    break;
  case BED_OCP:
    result = TERN0(HAS_BED_OCP,       bed_current_info.celsius);
    break;
  default:break;
  }
  return result;
}
bool Current::hint_no24v(CurrentItemName item){
  #if PIN_EXISTS(POWER_LOSS)
  switch(item){
  case MOTORS_OCP:
    break;
  case HOTEND_OCP:
    {
      if(!READ(POWER_LOSS_PIN))
      {volumeReminderMessage(LABEL_UNDER_VOLTAGE, STATUS_NORMAL);}
      else
      {volumeReminderMessage(LABEL_OVERCURRENT_PROTECTION, STATUS_NORMAL);}
      return true;
    }
    break;
  case BED_OCP:
    {
      if(!READ(POWER_LOSS_PIN))
      {volumeReminderMessage(LABEL_UNDER_VOLTAGE, STATUS_NORMAL);}
      else
      {volumeReminderMessage(LABEL_OVERCURRENT_PROTECTION, STATUS_NORMAL);}
      return true;
    }
    break;
  case 3:break;
  default:break;
  }
  #endif
  return false;
}
#endif
