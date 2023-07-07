#ifndef _FLASHSTORE_H_
#define _FLASHSTORE_H_

enum MachineFlags:uint8_t{
  MF_FAN = 0,
  MF_MAXTEMP,
};

bool readStoredPara(void);
void storePara(void);
uint32_t get_MachineFlag(void);
void set_MachineFlag(uint32_t flag);
uint32_t get_infoSettings(char a);
void set_infoSettings(char a, uint32_t num);

#endif // _FLASHSTORE_H_
