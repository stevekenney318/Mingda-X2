#ifndef _STM32_FLASH_H_
#define _STM32_FLASH_H_

#include <stdint.h>

void STM32_FlashRead(uint8_t *data, uint32_t len);
void STM32_FlashWrite(uint8_t *data, uint32_t len);
void stm32f4_flash_erase(uint32_t addr, uint32_t size);
void stm32f4_flash_write(uint32_t addr, uint8_t *buf, uint32_t size, void (*stage_disp_func)(uint32_t i));
void stm32f4_flash_read(uint32_t addr, uint8_t *buf, uint32_t size);

#endif
