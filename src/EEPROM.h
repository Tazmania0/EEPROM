#ifndef __EEPROM_H
#define __EEPROM_H
#pragma once
#include "flash_stm32.h"


/* Page full define */
enum : uint16_t
{
  EEPROM_OK = ((uint16_t)0x0000),
  EEPROM_OUT_SIZE = ((uint16_t)0x0081),
  EEPROM_BAD_ADDRESS = ((uint16_t)0x0082),
  EEPROM_BAD_FLASH = ((uint16_t)0x0083),
  EEPROM_NOT_INIT = ((uint16_t)0x0084),
  EEPROM_SAME_VALUE = ((uint16_t)0x0085),
  EEPROM_NO_VALID_PAGE = ((uint16_t)0x00AB)
};

#define EEPROM_DEFAULT_DATA		0xFFFF


class EEPROMClass
{
public:
  EEPROMClass(void);

  uint16_t init(void);
  uint16_t init(uint32_t, uint32_t, uint32_t);

  uint16_t format(void);

  uint16_t erases(uint16_t*);
  uint16_t read(uint16_t address);
  uint16_t read(uint16_t address, uint16_t* data);
  uint16_t write(uint16_t address, uint16_t data);
  uint16_t update(uint16_t address, uint16_t data);
  uint16_t remove(uint16_t address);
  uint16_t count(uint16_t*);
  uint16_t maxcount(void);

  uint32_t PageBase0;
  uint32_t PageBase1;
  uint32_t PageSize;
  uint16_t Status = EEPROM_NOT_INIT;

  /**
 * Update eeprom cells from an arbitrary object
 * @param index
 * @param value
 */
 //Functionality to 'get' and 'put' objects to and from EEPROM

  template< typename T > T& get(const int& idx, T& t) {  
    uint16_t offset = idx;
    uint16_t* _pointer = (uint16_t*)&t;
    int16_t count = sizeof(t);
   
    if (Status == EEPROM_NOT_INIT)
      init();

    if (count % 2)
    {
      uint8_t* _last = (uint8_t*)&t + count - 1;
      *_last = (uint8_t)read(offset + count / 2);
      count--;
    }
    for (; count;count = count - 2, ++offset)
    {
      *_pointer++ = read(offset);
    }
    
   return t;
  }


  template< typename T > const T& put(const int& idx, const T& t) {
    // Copy the data from the flash to the buffer if not yet
    if (Status == EEPROM_NOT_INIT)
      init();

    uint16_t offset = idx;
    const uint16_t* _pointer = (const uint16_t*)&t;
    int16_t count = sizeof(T);
    //check if EEPROM can fit data
    if ((idx + count / 2 + count % 2) > maxcount() ) return t;
    
    if (count % 2)
    {
      update(offset + count / 2, (uint16_t) * ((uint8_t*)_pointer + count - 1)); //pointer to last address of t
      count--;
    }
    for (; count; count = count - 2, ++offset)
    {
      update(offset, *_pointer++);
    }

    return t;
  }

private:
  FLASH_Status EE_ErasePage(uint32_t);

  uint16_t EE_CheckPage(uint32_t, uint16_t);
  uint16_t EE_CheckErasePage(uint32_t, uint16_t);
  uint16_t EE_Format(void);
  uint32_t EE_FindValidPage(void);
  uint16_t EE_GetVariablesCount(uint32_t, uint16_t);
  uint16_t EE_PageTransfer(uint32_t, uint32_t, uint16_t);
  uint16_t EE_VerifyPageFullWriteVariable(uint16_t, uint16_t);
};

static  EEPROMClass EEPROM;


#endif	/* __EEPROM_H */
