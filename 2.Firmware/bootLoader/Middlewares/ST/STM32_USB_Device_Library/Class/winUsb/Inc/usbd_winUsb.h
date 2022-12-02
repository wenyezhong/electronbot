/**
  ******************************************************************************
  * @file    usbd_cdc.h
  * @author  MCD Application Team
  * @brief   header file for the usbd_cdc.c file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_WIN_USB_H
#define __USB_WIN_USB_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "usbd_ioreq.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */

/** @defgroup usbd_cdc
  * @brief This file is the Header file for usbd_cdc.c
  * @{
  */


/** @defgroup usbd_cdc_Exported_Defines
  * @{
  */

#define WIN_USB_EPIN_ADDR  0x83
#define WIN_USB_EPOUT_ADDR  0x03

// #define WIN_USB_MAX_PACKET_SIZE  64U

#define WIN_DATA_HS_MAX_PACKET_SIZE                 512U  /* Endpoint IN & OUT Packet size */
#define WIN_DATA_FS_MAX_PACKET_SIZE                 64U  /* Endpoint IN & OUT Packet size */

#define WIN_DATA_FS_IN_PACKET_SIZE                  WIN_DATA_FS_MAX_PACKET_SIZE
#define WIN_DATA_FS_OUT_PACKET_SIZE                 WIN_DATA_FS_MAX_PACKET_SIZE

#define WIN_DATA_HS_IN_PACKET_SIZE                  WIN_DATA_HS_MAX_PACKET_SIZE
#define WIN_DATA_HS_OUT_PACKET_SIZE                 WIN_DATA_HS_MAX_PACKET_SIZE

#define winAPP_RX_DATA_SIZE  2048
#define winAPP_TX_DATA_SIZE  2048
#define WinUsb_DATA_MAX_PACKET_SIZE  512U

/*---------------------------------------------------------------------*/
/*  CDC definitions                                                    */
/*---------------------------------------------------------------------*/
/* #define CDC_SEND_ENCAPSULATED_COMMAND               0x00U
#define CDC_GET_ENCAPSULATED_RESPONSE               0x01U
#define CDC_SET_COMM_FEATURE                        0x02U
#define CDC_GET_COMM_FEATURE                        0x03U
#define CDC_CLEAR_COMM_FEATURE                      0x04U
#define CDC_SET_LINE_CODING                         0x20U
#define CDC_GET_LINE_CODING                         0x21U
#define CDC_SET_CONTROL_LINE_STATE                  0x22U
#define CDC_SEND_BREAK                              0x23U */

/**
  * @}
  */


/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */

/**
  * @}
  */
/* typedef struct
{
  uint32_t bitrate;
  uint8_t  format;
  uint8_t  paritytype;
  uint8_t  datatype;
} USBD_CDC_LineCodingTypeDef; */

typedef struct _USBD_WinUsb_Itf
{
  int8_t (* Init)(void);
  int8_t (* DeInit)(void);  
  int8_t (* Receive)(uint8_t *Buf, uint32_t *Len);
  int8_t (* TransmitCplt)(uint8_t *Buf, uint32_t *Len, uint8_t epnum);
} USBD_WinUsb_ItfTypeDef;


typedef struct
{
  uint32_t data[WinUsb_DATA_MAX_PACKET_SIZE / 4U];   
  uint8_t  CmdOpCode;
  uint8_t  CmdLength;
  uint8_t  *RxBuffer;
  uint8_t  *TxBuffer;
  uint32_t RxLength;
  uint32_t TxLength;

  __IO uint32_t TxState;
  __IO uint32_t RxState;
} USBD_WinUsb_HandleTypeDef;



/** @defgroup USBD_CORE_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup USBD_CORE_Exported_Variables
  * @{
  */

extern USBD_ClassTypeDef USBD_WinUsb;
//#define USBD_CDC_CLASS &USBD_CDC

extern USBD_WinUsb_ItfTypeDef USBD_WinUsb_Interface_fops_FS;
/**
  * @}
  */

/** @defgroup USB_CORE_Exported_Functions
  * @{
  */

uint8_t USBD_WinUsb_RegisterInterface(USBD_HandleTypeDef *pdev,
                                   USBD_WinUsb_ItfTypeDef *fops);

/* uint8_t USBD_CDC_RegisterInterface(USBD_HandleTypeDef *pdev,
                                   USBD_CDC_ItfTypeDef *fops);

uint8_t USBD_CDC_SetTxBuffer(USBD_HandleTypeDef *pdev, uint8_t *pbuff,
                             uint32_t length);

uint8_t USBD_CDC_SetRxBuffer(USBD_HandleTypeDef *pdev, uint8_t *pbuff);
uint8_t USBD_CDC_ReceivePacket(USBD_HandleTypeDef *pdev);
uint8_t USBD_CDC_TransmitPacket(USBD_HandleTypeDef *pdev); */
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif  /* __USB_CDC_H */
/**
  * @}
  */

/**
  * @}
  */

