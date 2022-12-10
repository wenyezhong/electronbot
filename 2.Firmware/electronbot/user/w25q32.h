#ifndef W25Q32_H
#define W25Q32_H
#include "spi.h"
#include "gpio.h"


#define FLASH_CS_High()  HAL_GPIO_WritePin(spiFlashCs_GPIO_Port, spiFlashCs_Pin, GPIO_PIN_SET)
#define FLASH_CS_Low()   HAL_GPIO_WritePin(spiFlashCs_GPIO_Port, spiFlashCs_Pin, GPIO_PIN_RESET);

//	#define FLASH_WP_Low()     GPIO_ResetBits(GPIOA, GPIO_Pin_4)
//	#define FLASH_WP_High()    GPIO_SetBits(GPIOA, GPIO_Pin_4)
#define DUMMY_BYTE 0xff
#define FLASH_BUSY_TIMEOUT  20000

#define FLASH_PAGE_SIZE     256  //spi flash page size
#define PAGES_PER_SECTOR    16

#define RDID_CMD  0X9F
#define RDSD_CMD  0X05
#define WRSR_CMD  0x01
#define REMS_CMD  0X90
#define DP_CMD    0XB9
#define RDP_CMD   0XAB
#define RES_CMD   0XAB
#define PP_CMD    0X02
#define WREN_CMD  0X06
#define WRDI_CMD  0X04
#define CE_CMD    0X60
#define BE_CMD    0X52
#define SE_CMD    0x20
#define READ_CMD  0X03
#define FREAD_CMD 0X0B
#define DREAD_CMD 0X3B
typedef struct
{
    uint8_t ManufacturerID;
    uint8_t DeviceID[2];
}flashInfoTypedef;
void SpiFlash_Init(void);
void FlashGet_Info(flashInfoTypedef * pFlashInfo);
void FlashWait_Busy(void);
void FlashGet_ElectronicInfo(flashInfoTypedef * pFlashInfo);
void FlashEnter_DeepPowerDown(void);
void FlashReleaseFrom_DeepPowerDown(void);
uint8_t FlashReleaseFrom_DeepPowerDownE(void);
void FlashPage_Read(uint16_t pageAddr,uint8_t *ptr,uint32_t pageSize);
void FlashPage_Write(uint16_t pageAddr,uint8_t *ptr,uint32_t pageSize);
void FlashChip_Erase(void);
void FlashSector_Erase(uint16_t sectorAddr);
void FlashFastRead(uint32_t addr,uint8_t *ptr,uint32_t len);
#endif
