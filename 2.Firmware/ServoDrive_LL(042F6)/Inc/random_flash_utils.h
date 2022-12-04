/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_EEPROM_H
#define __STM32_EEPROM_H

/* Includes ------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#include <stm32f0xx.h>
#include "stm32f0xx_ll_flash.h"

#endif

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#if defined(STM32MP1xx)

#else
#ifndef FLASH_PAGE_SIZE
/*
 * FLASH_PAGE_SIZE is not defined for STM32F2xx, STM32F4xx and STM32F7xx
 * Could be redefined in variant.h or using build_opt.h
 * Warning: This is not the sector size, only the size used for EEPROM
 * emulation. Anyway, all the sector size will be erased.
 * So pay attention to not use this sector for other stuff.
 */
#define FLASH_PAGE_SIZE     ((uint32_t)(1*1024)) /* 1kB page */
#endif

#if defined(DATA_EEPROM_BASE) || defined(FLASH_EEPROM_BASE)
#if defined (DATA_EEPROM_END)
#define E2END (DATA_EEPROM_END - DATA_EEPROM_BASE)
#elif defined (DATA_EEPROM_BANK2_END)
/* assuming two contiguous banks */
#define DATA_EEPROM_END DATA_EEPROM_BANK2_END
#define E2END (DATA_EEPROM_BANK2_END - DATA_EEPROM_BASE)
#elif defined (FLASH_EEPROM_END)
#define DATA_EEPROM_BASE FLASH_EEPROM_BASE
#define DATA_EEPROM_END FLASH_EEPROM_END
#define E2END (DATA_EEPROM_END - DATA_EEPROM_BASE)
#endif /* __EEPROM_END */

#else /* _EEPROM_BASE */
#define E2END (FLASH_PAGE_SIZE - 1)
#endif /* _EEPROM_BASE */

#endif


/* Exported macro ------------------------------------------------------------*/
#define FLASH_USER_START_ADDR       0x08007800UL // 256B
#define FLASH_USER_END_ADDR         0x08007bFFUL
/* Exported functions ------------------------------------------------------- */

uint8_t eeprom_read_byte(uint32_t pos);
void eeprom_write_byte(uint32_t pos, uint8_t value);

#if !defined(DATA_EEPROM_BASE)
void eeprom_buffer_fill();
void eeprom_buffer_flush();
uint8_t eeprom_buffered_read_byte(uint32_t pos);
void eeprom_buffered_write_byte(uint32_t pos, uint8_t value);
#endif /* ! DATA_EEPROM_BASE */

#ifdef __cplusplus
}


#endif
#endif /* __STM32_EEPROM_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
