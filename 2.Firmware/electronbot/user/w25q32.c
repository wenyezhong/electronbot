/* ---------------------------------- File Description --------------------------------------------
【文件功能】：本文件为 W25Q128  SpiFLASH芯片驱动文件
              W25Q128  存储量为128M bit  分为4096个扇区，每个扇区有4K byte  16个扇区为一个block 总共有256个block
              每page 为256个字节  总共65536页   每个扇区16页
              最快访问速度高达104MHz
【创建时间】：2012年10月22日星期一13:20:40
【文件版本】：V1.0
【作者声明】：本文件作者为温业中,可通过email与作者联系: eric540@163.com
------------------------------------------------------------------------------------------ */

#include "w25q32.h"
#include <stdio.h>

void SPI2_Write(uint8_t dat)
{
    HAL_SPI_Transmit(&hspi1, &dat, 1, 0xff);
}
uint8_t SPI2_Read(void)
{
    uint8_t rx_dat;
    // uint8_t tx_dat=0xff;
    // HAL_SPI_TransmitReceive(&hspi1, &tx_dat, &rx_dat, 1, 0xff);
    HAL_SPI_Receive(&hspi1, &rx_dat, 1, 20);
    return rx_dat;
}

void SpiFlash_Init(void)
{
    flashInfoTypedef flashInfo;   
    //WP
    /*GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    FLASH_WP_High();*/
    // SPI2_Configuration();
    
    FlashGet_ElectronicInfo(&flashInfo);
    printf("ManufacturerID=%02X\r\n",flashInfo.ManufacturerID);
    printf("DeviceID=%02X\r\n",flashInfo.DeviceID[0]);
    if((flashInfo.ManufacturerID==0xEF)&&(flashInfo.DeviceID[0]==0x17))
    {
        printf("\tFlash Info: WINBOND SPI FLASH\r\n");
    }
    FlashGet_Info(&flashInfo);
    printf("ManufacturerID=%02X\r\n",flashInfo.ManufacturerID);
    printf("DeviceID=%02X%02X\r\n",flashInfo.DeviceID[0],flashInfo.DeviceID[1]);
    if((flashInfo.DeviceID[0]==0x40)&&(flashInfo.DeviceID[1]==0x18))
    {
        printf("\tFlashType:W25Q128  FlashSize:128Mbit\r\n\tTotal 256 Blocks  16 Sectors/Block  4KB/Sector\r\n");
    }

#if 0    
    FlashEnter_DeepPowerDown();
    tmp=FlashReleaseFrom_DeepPowerDownE();
    printf("ElectronicSignature=%02x\r\n",tmp);  //��ӡ���Ϊ: 0x14
#endif    
}

void FlashWait_Busy(void)
{
    uint8_t flashSta;
    uint32_t retry=0;
    FLASH_CS_Low();
    SPI2_Write(RDSD_CMD);
    do{
        
        flashSta=SPI2_Read();
        if(retry++>FLASH_BUSY_TIMEOUT)
            break;
        //printf("flashSta=%02x----%d\r\n",flashSta,retry);
    }while(flashSta&0x01);
    //printf("%d\r\n",retry);
    FLASH_CS_High();    
}
void FlashGet_Info(flashInfoTypedef * pFlashInfo)
{
//	    FlashWait_Busy();
    FLASH_CS_Low();    
    SPI2_Write(RDID_CMD);
    pFlashInfo->ManufacturerID=SPI2_Read();
    pFlashInfo->DeviceID[0]=SPI2_Read();
    pFlashInfo->DeviceID[1]=SPI2_Read();
    FLASH_CS_High();    
}
void FlashGet_ElectronicInfo(flashInfoTypedef * pFlashInfo)
{
    int i;
	// FlashWait_Busy();
    FLASH_CS_Low();
   
    /* SPI2_Write(REMS_CMD);
    SPI2_Write(DUMMY_BYTE);
    SPI2_Write(DUMMY_BYTE);
    SPI2_Write(0x00);  //manufacturer's ID first
    pFlashInfo->ManufacturerID=SPI2_Read();
    pFlashInfo->DeviceID[0]=SPI2_Read(); */
    uint8_t tx_data[7];
    uint8_t rx_data[7];
    tx_data[0] = REMS_CMD;
    tx_data[1] = DUMMY_BYTE;
    tx_data[2] = DUMMY_BYTE;
    tx_data[3] = 0x00;
    tx_data[4] = DUMMY_BYTE;
    tx_data[5] = DUMMY_BYTE;
    HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, 6, 500);
    pFlashInfo->ManufacturerID=rx_data[4];
    pFlashInfo->DeviceID[0]=rx_data[5];
    
    FLASH_CS_High();
}
void FlashEnter_DeepPowerDown(void)
{
    FLASH_CS_Low();
    SPI2_Write(DP_CMD);   
    FLASH_CS_High();    
}
void FlashReleaseFrom_DeepPowerDown(void)
{
    FLASH_CS_Low();
    SPI2_Write(RDP_CMD);   
    FLASH_CS_High();    
}
uint8_t FlashReleaseFrom_DeepPowerDownE(void)
{   
    uint8_t ElectronicSignature;
    FLASH_CS_Low();
    SPI2_Write(RES_CMD);
    SPI2_Write(DUMMY_BYTE);
    SPI2_Write(DUMMY_BYTE);
    SPI2_Write(DUMMY_BYTE);
    ElectronicSignature=SPI2_Read();
    FLASH_CS_High();    
    return ElectronicSignature;
}
void FlashWriteEnable(void)
{
    FLASH_CS_Low();
    SPI2_Write(WREN_CMD);   
    FLASH_CS_High();    
}
void FlashWriteDisable(void)
{
    FLASH_CS_Low();
    SPI2_Write(WRDI_CMD);   
    FLASH_CS_High();    
}
void FlashWrite_StaRegister(uint8_t flashSta)
{
    FLASH_CS_Low();
    SPI2_Write(WRSR_CMD); 
    SPI2_Write(flashSta); 
    FLASH_CS_High();    
}
/*---- F L A S H   P A G E _   W R I T E ----
�����ܡ���****
����������pageAddr:ҳ��ַ���ں������ٳ���ҳ��С����ΪҪ�����24bit��ַ��  ҳ��Χ(0~65536) 
          *ptr:     ָ��Ҫд�����ݵ���ַ��
          pageSize: ҳ�Ĵ�С��һ���� FLASH_PAGE_SIZE 
�����ء���****
��˵���������ڴ�flash ����������ĵ�ַλΪ24bit �����8λ��Ϊȫ0ʱ��������ҳ��ַ�ķ�Χ���ݻḲ��ҳ��ʼ��Χ����
          ���С��ҳ��Χ����ҳ����ԭ���洢�����ݲ���ı䡣
--------------����:��ҵ��   2012��10��23�����ڶ�13:52:53--------------------------------*/
void FlashPage_Write(uint16_t pageAddr,uint8_t *ptr,uint32_t pageSize)
{
    uint32_t addr,i;
//	    addr=pageAddr*FLASH_PAGE_SIZE;
    addr=((uint32_t)pageAddr)<<8;  
    FlashWait_Busy();
    FlashWriteEnable();  
    FLASH_CS_Low();
    SPI2_Write(PP_CMD); 
    SPI2_Write(addr>>16);
    SPI2_Write(addr>>8);
    SPI2_Write(addr); 
    for(i=0;i<pageSize;i++)
        SPI2_Write(*ptr++); 
    FLASH_CS_High();  
    
}
void FlashChip_Erase(void)
{
    FlashWait_Busy();
    FlashWriteEnable();    
    FLASH_CS_Low();
    SPI2_Write(CE_CMD);     
    FLASH_CS_High();    
}
/*---- F L A S H   B L O C K _   E R A S E ----
�����ܡ���****
����������block: 0~256
�����ء���****
��˵������****
--------------����:��ҵ��   2012��10��23�����ڶ�14:43:05--------------------------------*/
void FlashBlock_Erase(uint8_t blockAddr)
{
    uint32_t addr;//,i;
//	    addr=blockAddr*16*4096;  //ÿ��block��16��������ÿ��������4KByte;
    

    addr=((uint32_t)blockAddr)<<16;
    FlashWait_Busy();
    FlashWriteEnable();
    FLASH_CS_Low();
    SPI2_Write(BE_CMD); 
    SPI2_Write(addr>>16);
    SPI2_Write(addr>>8);
    SPI2_Write(addr); 
    FLASH_CS_High();    
}
/*---- F L A S H   S E C T O R _   E R A S E ----
�����ܡ���****
����������sectorAddr: 0~4096
�����ء���****
��˵������****
--------------����:��ҵ��   2012��10��23�����ڶ�14:45:28--------------------------------*/
void FlashSector_Erase(uint16_t sectorAddr)
{
    uint32_t addr;//,i;
    addr=((uint32_t)sectorAddr)<<12;
    FlashWait_Busy();
    FlashWriteEnable();
    FLASH_CS_Low();
    SPI2_Write(SE_CMD); 
    SPI2_Write(addr>>16);
    SPI2_Write(addr>>8);
    SPI2_Write(addr); 
    FLASH_CS_High();    
}
void FlashRead(uint32_t addr,uint8_t *ptr,uint32_t len)
{
    uint32_t addr_tmp,i;
    addr_tmp=addr&0x00ffffff;
    FlashWait_Busy();    
    FLASH_CS_Low();
    SPI2_Write(READ_CMD); 
    SPI2_Write(addr_tmp>>16);
    SPI2_Write(addr_tmp>>8);
    SPI2_Write(addr_tmp); 
    for(i=0;i<len;i++)
        *ptr++=SPI2_Read(); 
    FLASH_CS_High();    
}
/*---- F L A S H   P A G E _   R E A D ----
�����ܡ���****
����������pageAddr:ҳ��ַ���ں������ٳ���ҳ��С����ΪҪ�����24bit��ַ��  ҳ��Χ(0~8191) 
�����ء���****
��˵������****
--------------����:��ҵ��   2012��10��23�����ڶ�15:11:50--------------------------------*/
void FlashPage_Read(uint16_t pageAddr,uint8_t *ptr,uint32_t pageSize)
{
    uint32_t addr,i;
//	    addr=pageAddr*FLASH_PAGE_SIZE;
    addr=((uint32_t)pageAddr)<<8;
    FlashWait_Busy();   
    FLASH_CS_Low();
    SPI2_Write(READ_CMD); 
    SPI2_Write(addr>>16);
    SPI2_Write(addr>>8);
    SPI2_Write(addr); 
    for(i=0;i<pageSize;i++)
        *ptr++=SPI2_Read(); 
    FLASH_CS_High();    
}
void FlashFastRead(uint32_t addr,uint8_t *ptr,uint32_t len)
{
    uint32_t addr_tmp,i;
    addr_tmp=addr&0x00ffffff;
    FlashWait_Busy();    
    FLASH_CS_Low();
    SPI2_Write(FREAD_CMD); 
    SPI2_Write(addr_tmp>>16);
    SPI2_Write(addr_tmp>>8);
    SPI2_Write(addr_tmp); 
    SPI2_Write(DUMMY_BYTE); 
    for(i=0;i<len;i++)
        *ptr++=SPI2_Read(); 
    FLASH_CS_High();    
}
#if 0
void FlashDoubleRead(u32 addr,u8 *ptr,u32 len)
{
    u32 addr_tmp,i;
    addr_tmp=addr&0x001fffff;
    FlashWait_Busy();    
    FLASH_CS_Low();
    SPI2_Write(DREAD_CMD); 
    SPI2_Write(addr_tmp>>16);
    SPI2_Write(addr_tmp>>8);
    SPI2_Write(addr_tmp); 
    SPI2_Write(DUMMY_BYTE); 
    for(i=0;i<len;i++)
    {
        //��˫������,��Ҫ���
        //*ptr++=SPI2_Read(); 
    }
    FLASH_CS_High();    
}
#endif

