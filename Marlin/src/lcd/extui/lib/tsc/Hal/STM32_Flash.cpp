#include "../TSC_Menu.h"
// #include <EEPROM.h>
#include "stm32f4xx_hal.h"
// #include <stm32_eeprom.h>
// #include "../../../../src/HAL/STM32_F4_F7/eeprom_emul.h"


// #define EEPROM_START_ADDRESS (0x8000000UL + (STM32_FLASH_SIZE) * 1024UL - (EEPROM_PAGE_SIZE) * 2UL)
//#define SIGN_ADDRESS (0x08080000 - 0x800 - 0x800)
#define SIGN_ADDRESS (0x080E0000) //last sector (128KB) to save user parameters

void STM32_FlashRead(uint8_t *data, uint32_t len)
{
  uint32_t i = 0;
  for(i = 0; i < len; i++)
  {
    data[i] = *(volatile uint8_t*)(SIGN_ADDRESS + i);
  }
}

void STM32_FlashWrite(uint8_t *data, uint32_t len)
{
#if 0
  uint32_t i = 0;
  FLASH_Unlock();
  FLASH_ErasePage(SIGN_ADDRESS);
  for(i = 0; i < len; i += 2)
  {
    uint16_t data16 = data[i] | (data[min(i+1, len-1)] << 8);  //stm32f10x needs to write at least 16 bits at a time
    FLASH_ProgramHalfWord(SIGN_ADDRESS + i, data16);
  }
  FLASH_Lock();

#else
  HAL_FLASH_Unlock();
  // HAL_FLASHEx_erase
  // HAL_FLASHEx_Erase( SIGN_ADDRESS);

  HAL_FLASH_Lock();
#endif
}



typedef struct {
  uint32_t addr;
  uint32_t size;
}flash_sector_layout_t;

#define FLASH_SECTOR_0     0U  /*!< Sector Number 0   */
#define FLASH_SECTOR_1     1U  /*!< Sector Number 1   */
#define FLASH_SECTOR_2     2U  /*!< Sector Number 2   */
#define FLASH_SECTOR_3     3U  /*!< Sector Number 3   */
#define FLASH_SECTOR_4     4U  /*!< Sector Number 4   */
#define FLASH_SECTOR_5     5U  /*!< Sector Number 5   */
#define FLASH_SECTOR_6     6U  /*!< Sector Number 6   */
#define FLASH_SECTOR_7     7U  /*!< Sector Number 7   */
#define FLASH_SECTOR_8     8U  /*!< Sector Number 8   */
#define FLASH_SECTOR_9     9U  /*!< Sector Number 9   */
#define FLASH_SECTOR_10    10U /*!< Sector Number 10  */
#define FLASH_SECTOR_11    11U /*!< Sector Number 11  */

const flash_sector_layout_t sector_layouts[] = {
  {0x08000000UL, 16*0x400,},
  {0x08004000UL, 16*0x400,},
  {0x08008000UL, 16*0x400,},
  {0x0800C000UL, 16*0x400,},

  {0x08010000UL, 64*0x400,},

  {0x08020000UL, 128*0x400,},
  {0x08040000UL, 128*0x400,},
  {0x08060000UL, 128*0x400,},
  {0x08080000UL, 128*0x400,},
  {0x080A0000UL, 128*0x400,},
  {0x080C0000UL, 128*0x400,},
  {0x080E0000UL, 128*0x400,},
};

//this function can only find correct addr
uint32_t flash_sector_idx_find(uint32_t addr)
{
  #define ARRAY_CNT(a)  (sizeof(a)/sizeof(a[0]))
  uint32_t cnts = ARRAY_CNT(sector_layouts);
  for (int i=0; i<cnts; ++i) {
    if (addr == sector_layouts[i].addr ) 
    {
      return i;
    }
    else if (addr > sector_layouts[i].addr 
        && i < cnts 
        && addr < sector_layouts[i+1].addr) 
    {
      return i;
    }
  }
  return 0xff;  //fail
}

uint32_t flash_sector_nb_get(uint32_t addr, uint32_t require_size)
{
  uint32_t req_sector_cnt = 0;
  uint32_t current_total_size = 0;
  uint32_t i = flash_sector_idx_find(addr);
  do {
    current_total_size += sector_layouts[i].size;
    i++;
    req_sector_cnt++;
  } while (current_total_size < require_size);
  return req_sector_cnt;
}

void stm32f4_flash_erase(uint32_t addr, uint32_t size)
{
  HAL_FLASH_Unlock();
  static FLASH_EraseInitTypeDef fe;
  static uint32_t err;
  HAL_StatusTypeDef ret;
  fe.Banks = FLASH_BANK_1;
  fe.Sector = flash_sector_idx_find(addr);
  fe.NbSectors = flash_sector_nb_get(addr, size);
  fe.TypeErase = FLASH_TYPEERASE_SECTORS;
  fe.VoltageRange = FLASH_VOLTAGE_RANGE_3;  //2.7-3.6
  ret = HAL_FLASHEx_Erase(&fe, &err);
  // if (ret != HAL_OK) {
  //   rtt.printf("flash erase error. %d", err);
  // }
  HAL_FLASH_Lock();
}

void stm32f4_flash_write(uint32_t addr, uint8_t *buf, uint32_t size, void (*stage_disp_func)(uint32_t i))
{
  HAL_FLASH_Unlock();
  HAL_StatusTypeDef ret;
  uint32_t i=0;
  for (i=0; i<size; ++i) {
    // *stage = i * 100 / size;
    if (stage_disp_func) {
      stage_disp_func(i);
    }
    ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, addr+i, buf[i]);
    // if (ret != HAL_OK) {
    //   rtt.printf("flash write error. %d", i);
    // }
  }
  HAL_FLASH_Lock();
}
void stm32f4_flash_read(uint32_t addr, uint8_t *buf, uint32_t size){
  uint32_t i = 0;
  for(i = 0; i < size; i++)
  {
    buf[i] = *(volatile uint8_t*)(addr + i);
  }
}
