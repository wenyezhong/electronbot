/**
  ******************************************************************************
  * @file    usbd_cdc.c
  * @author  MCD Application Team
  * @brief   This file provides the high layer firmware functions to manage the
  *          following functionalities of the USB CDC Class:
  *           - Initialization and Configuration of high and low layer
  *           - Enumeration as CDC Device (and enumeration for each implemented memory interface)
  *           - OUT/IN data transfer
  *           - Command IN transfer (class requests management)
  *           - Error management
  *
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
  *  @verbatim
  *
  *          ===================================================================
  *                                CDC Class Driver Description
  *          ===================================================================
  *           This driver manages the "Universal Serial Bus Class Definitions for Communications Devices
  *           Revision 1.2 November 16, 2007" and the sub-protocol specification of "Universal Serial Bus
  *           Communications Class Subclass Specification for PSTN Devices Revision 1.2 February 9, 2007"
  *           This driver implements the following aspects of the specification:
  *             - Device descriptor management
  *             - Configuration descriptor management
  *             - Enumeration as CDC device with 2 data endpoints (IN and OUT) and 1 command endpoint (IN)
  *             - Requests management (as described in section 6.2 in specification)
  *             - Abstract Control Model compliant
  *             - Union Functional collection (using 1 IN endpoint for control)
  *             - Data interface class
  *
  *           These aspects may be enriched or modified for a specific user application.
  *
  *            This driver doesn't implement the following aspects of the specification
  *            (but it is possible to manage these features with some modifications on this driver):
  *             - Any class-specific aspect relative to communication classes should be managed by user application.
  *             - All communication classes other than PSTN are not managed
  *
  *  @endverbatim
  *
  ******************************************************************************
  */

/* BSPDependencies
- "stm32xxxxx_{eval}{discovery}{nucleo_144}.c"
- "stm32xxxxx_{eval}{discovery}_io.c"
EndBSPDependencies */

/* Includes ------------------------------------------------------------------*/
#include "usbd_winUsb.h"
#include "usbd_ctlreq.h"
#include "usbd_desc.h"
#include <stdio.h>

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */
static uint8_t USBD_WinUsb_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_WinUsb_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_WinUsb_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t USBD_WinUsb_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_WinUsb_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_WinUsb_EP0_RxReady(USBD_HandleTypeDef *pdev);


/** @defgroup USBD_CDC
  * @brief usbd core module
  * @{
  */

USBD_ClassTypeDef  USBD_WinUsb =
{
  USBD_WinUsb_Init,
  USBD_WinUsb_DeInit,
  USBD_WinUsb_Setup,
  NULL,                 /* EP0_TxSent */
  USBD_WinUsb_EP0_RxReady,
  USBD_WinUsb_DataIn,
  USBD_WinUsb_DataOut,
  NULL,
  NULL,
  NULL,
#ifdef USE_USBD_COMPOSITE
  NULL,
  NULL,
  NULL,
  NULL,
#else
  USBD_WinUsb_GetHSCfgDesc,
  USBD_WinUsb_GetFSCfgDesc,
  USBD_WinUsb_GetOtherSpeedCfgDesc,
  USBD_WinUsb_GetDeviceQualifierDescriptor,
#endif /* USE_USBD_COMPOSITE  */
  USBD_UsrStrDescriptor
};


#define NUM_INTERFACES 1

#if NUM_INTERFACES == 2
#define USB_WINUSBCOMM_COMPAT_ID_OS_DESC_SIZ       (16 + 24 + 24)
#else
#define USB_WINUSBCOMM_COMPAT_ID_OS_DESC_SIZ       (16 + 24)
#endif


// This associates winusb driver with the device
__ALIGN_BEGIN uint8_t USBD_WinUSBComm_Extended_Compat_ID_OS_Desc[USB_WINUSBCOMM_COMPAT_ID_OS_DESC_SIZ]  __ALIGN_END =
{
                                                    //    +-- Offset in descriptor
                                                    //    |             +-- Size
                                                    //    v             v
  USB_WINUSBCOMM_COMPAT_ID_OS_DESC_SIZ, 0, 0, 0,    //    0 dwLength    4 DWORD The length, in bytes, of the complete extended compat ID descriptor
  0x00, 0x01,                                       //    4 bcdVersion  2 BCD The descriptor’s version number, in binary coded decimal (BCD) format
  0x04, 0x00,                                       //    6 wIndex      2 WORD  An index that identifies the particular OS feature descriptor
  NUM_INTERFACES,                                   //    8 bCount      1 BYTE  The number of custom property sections
  0, 0, 0, 0, 0, 0, 0,                              //    9 RESERVED    7 BYTEs Reserved
                                                    //    =====================
                                                    //                 16

                                                    //   +-- Offset from function section start
                                                    //   |                        +-- Size
                                                    //   v                        v
  2,                                                //   0  bFirstInterfaceNumber 1 BYTE  The interface or function number
  0,                                                //   1  RESERVED              1 BYTE  Reserved
  0x57, 0x49, 0x4E, 0x55, 0x53, 0x42, 0x00, 0x00,   //   2  compatibleID          8 BYTEs The function’s compatible ID      ("WINUSB")
  0, 0, 0, 0, 0, 0, 0, 0,                           //  10  subCompatibleID       8 BYTEs The function’s subcompatible ID
  0, 0, 0, 0, 0, 0,                                 //  18  RESERVED              6 BYTEs Reserved
                                                    //  =================================
                                                    //                           24
#if NUM_INTERFACES == 2
                                                    //   +-- Offset from function section start
                                                    //   |                        +-- Size
                                                    //   v                        v
  2,                                                //   0  bFirstInterfaceNumber 1 BYTE  The interface or function number
  0,                                                //   1  RESERVED              1 BYTE  Reserved
  0x57, 0x49, 0x4E, 0x55, 0x53, 0x42, 0x00, 0x00,   //   2  compatibleID          8 BYTEs The function’s compatible ID      ("WINUSB")
  0, 0, 0, 0, 0, 0, 0, 0,                           //  10  subCompatibleID       8 BYTEs The function’s subcompatible ID
  0, 0, 0, 0, 0, 0,                                 //  18  RESERVED              6 BYTEs Reserved
                                                    //  =================================
                                                    //                           24
#endif
};
// Properties are added to:
// HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Enum\USB\VID_xxxx&PID_xxxx\sssssssss\Device Parameters
// Use USBDeview or similar to uninstall

__ALIGN_BEGIN uint8_t USBD_WinUSBComm_Extended_Properties_OS_Desc[0xB6]  __ALIGN_END =
{
  0xB6, 0x00, 0x00, 0x00,   // 0 dwLength   4 DWORD The length, in bytes, of the complete extended properties descriptor
  0x00, 0x01,               // 4 bcdVersion 2 BCD   The descriptor’s version number, in binary coded decimal (BCD) format
  0x05, 0x00,               // 6 wIndex     2 WORD  The index for extended properties OS descriptors
  0x02, 0x00,               // 8 wCount     2 WORD  The number of custom property sections that follow the header section
                            // ====================
                            //             10
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  0x84, 0x00, 0x00, 0x00,   //  0       dwSize                  4 DWORD             The size of this custom properties section
  0x01, 0x00, 0x00, 0x00,   //  4       dwPropertyDataType      4 DWORD             Property data format
  0x28, 0x00,               //  8       wPropertyNameLength     2 DWORD             Property name length
                            // ========================================
                            //                                 10
                            // 10       bPropertyName         PNL WCHAR[]           The property name
  'D',0, 'e',0, 'v',0, 'i',0, 'c',0, 'e',0, 'I',0, 'n',0,
  't',0, 'e',0, 'r',0, 'f',0, 'a',0, 'c',0, 'e',0, 'G',0,
  'U',0, 'I',0, 'D',0, 0,0,
                            // ========================================
                            //                                 40 (0x28)

  0x4E, 0x00, 0x00, 0x00,   // 10 + PNL dwPropertyDataLength    4 DWORD             Length of the buffer holding the property data
                            // ========================================
                            //                                  4
    // 14 + PNL bPropertyData         PDL Format-dependent  Property data
  '{',0, 'E',0, 'A',0, '0',0, 'B',0, 'D',0, '5',0, 'C',0,
  '3',0, '-',0, '5',0, '0',0, 'F',0, '3',0, '-',0, '4',0,
  '8',0, '8',0, '8',0, '-',0, '8',0, '4',0, 'B',0, '4',0,
  '-',0, '7',0, '4',0, 'E',0, '5',0, '0',0, 'E',0, '1',0,
  '6',0, '4',0, '9',0, 'D',0, 'B',0, '}',0,  0 ,0,
                            // ========================================
                            //                                 78 (0x4E)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  0x3E, 0x00, 0x00, 0x00,   //  0 dwSize 0x00000030 (62 bytes)
  0x01, 0x00, 0x00, 0x00,   //  4 dwPropertyDataType 0x00000001 (Unicode string)
  0x0C, 0x00,               //  8 wPropertyNameLength 0x000C (12 bytes)
                            // ========================================
                            //                                  10
  'L',0, 'a',0, 'b',0, 'e',0, 'l',0, 0,0,
                            // 10 bPropertyName “Label”
                            // ========================================
                            //                                  12
  0x24, 0x00, 0x00, 0x00,   // 22 dwPropertyDataLength 0x00000016 (36 bytes)
                            // ========================================
                            //                                  4
  'O',0, 'D',0, 'r',0, 'i',0, 'v',0, 'e',0, 0,0
                            // 26 bPropertyData “ODrive”
                            // ========================================
                            //                                  14

};

static uint8_t  USBD_WinUSBComm_GetMSExtendedCompatIDOSDescriptor (USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  switch (req->wIndex)
  {
  case 0x04:
    USBD_CtlSendData (pdev, USBD_WinUSBComm_Extended_Compat_ID_OS_Desc, req->wLength);
    break;
  default:
   USBD_CtlError(pdev , req);
   return USBD_FAIL;
  }
  return USBD_OK;
}
static uint8_t  USBD_WinUSBComm_GetMSExtendedPropertiesOSDescriptor (USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  uint8_t byInterfaceIndex = (uint8_t)req->wValue;
  if ( req->wIndex != 0x05 )
  {
    USBD_CtlError(pdev , req);
    return USBD_FAIL;
  }
  switch ( byInterfaceIndex )
  {
  case 0:
#if NUM_INTERFACES == 2
  case 1:
#endif
    USBD_CtlSendData (pdev, USBD_WinUSBComm_Extended_Properties_OS_Desc, req->wLength);
    break;
  default:
    USBD_CtlError(pdev , req);
    return USBD_FAIL;
  }
  return USBD_OK;
}
static uint8_t  USBD_WinUSBComm_SetupVendorDevice(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  USBD_CtlError(pdev , req);
  return USBD_FAIL;
}
static uint8_t  USBD_WinUSBComm_SetupVendorInterface(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  USBD_CtlError(pdev , req);
  // TODO: check if this is important
  return USBD_FAIL;
}
static uint8_t  USBD_WinUSBComm_SetupVendor(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  switch ( req->bmRequest & USB_REQ_RECIPIENT_MASK )
  {
  case USB_REQ_RECIPIENT_DEVICE:
    return ( MS_VendorCode == req->bRequest ) ? USBD_WinUSBComm_GetMSExtendedCompatIDOSDescriptor(pdev, req) : USBD_WinUSBComm_SetupVendorDevice(pdev, req);
  case USB_REQ_RECIPIENT_INTERFACE:
    return ( MS_VendorCode == req->bRequest ) ? USBD_WinUSBComm_GetMSExtendedPropertiesOSDescriptor(pdev, req) : USBD_WinUSBComm_SetupVendorInterface(pdev, req);
  case USB_REQ_RECIPIENT_ENDPOINT:
    // fall through
  default:
    break;
  }
  USBD_CtlError(pdev , req);
  return USBD_FAIL;
}
static uint8_t WinUsbInEpAdd = WIN_USB_EPIN_ADDR;
static uint8_t WinUsbOutEpAdd = WIN_USB_EPOUT_ADDR;


static uint8_t USBD_WinUsb_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);
  USBD_WinUsb_HandleTypeDef *hcdc;

  hcdc = (USBD_WinUsb_HandleTypeDef *)USBD_malloc(sizeof(USBD_WinUsb_HandleTypeDef));

  if (hcdc == NULL)
  {
    pdev->pClassDataCmsit[pdev->classId] = NULL;
    return (uint8_t)USBD_EMEM;
  }

  (void)USBD_memset(hcdc, 0, sizeof(USBD_WinUsb_HandleTypeDef));

  pdev->pClassDataCmsit[pdev->classId] = (void *)hcdc;
  pdev->pClassData = pdev->pClassDataCmsit[pdev->classId];
  if (pdev->dev_speed == USBD_SPEED_HIGH)
  {
    USBD_LL_OpenEP(pdev,WIN_USB_EPIN_ADDR,USBD_EP_TYPE_BULK,WIN_DATA_HS_IN_PACKET_SIZE);
    USBD_LL_OpenEP(pdev,WIN_USB_EPOUT_ADDR,USBD_EP_TYPE_BULK,WIN_DATA_HS_OUT_PACKET_SIZE);
  }
  else
  {
    USBD_LL_OpenEP(pdev,WIN_USB_EPIN_ADDR,USBD_EP_TYPE_BULK,WIN_DATA_FS_IN_PACKET_SIZE);
    USBD_LL_OpenEP(pdev,WIN_USB_EPOUT_ADDR,USBD_EP_TYPE_BULK,WIN_DATA_FS_OUT_PACKET_SIZE);

  }
  
  
  pdev->ep_in[WinUsbInEpAdd & 0xFU].is_used = 1U;
  pdev->ep_in[WinUsbOutEpAdd & 0xFU].is_used = 1U;


  hcdc->RxBuffer = NULL;

  /* Init  physical Interface components */
  ((USBD_WinUsb_ItfTypeDef *)pdev->pUserData[pdev->classId])->Init();

  /* Init Xfer states */
  hcdc->TxState = 0U;
  hcdc->RxState = 0U;

  if (hcdc->RxBuffer == NULL)
  {
    return (uint8_t)USBD_EMEM;
  }
  if (pdev->dev_speed == USBD_SPEED_HIGH)
  {
    USBD_LL_PrepareReceive(pdev,
                          WIN_USB_EPOUT_ADDR,
                          hcdc->RxBuffer,
                          WIN_DATA_HS_OUT_PACKET_SIZE);
  }
  else
  {
    USBD_LL_PrepareReceive(pdev,
                          WIN_USB_EPOUT_ADDR,
                          hcdc->RxBuffer,
                          WIN_DATA_FS_OUT_PACKET_SIZE);
  }
  return (uint8_t)USBD_OK;

}

static uint8_t USBD_WinUsb_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);
	/* Close EP IN */
  USBD_LL_CloseEP(pdev,WinUsbInEpAdd);
  pdev->ep_out[WinUsbInEpAdd & 0xFU].is_used = 0U;  
  /* Close EP OUT */
  USBD_LL_CloseEP(pdev,WinUsbOutEpAdd); 
  pdev->ep_out[WinUsbOutEpAdd & 0xFU].is_used = 0U;


  /* DeInit  physical Interface components */
  if (pdev->pClassDataCmsit[pdev->classId] != NULL)
  {
    ((USBD_WinUsb_ItfTypeDef *)pdev->pUserData[pdev->classId])->DeInit();
    (void)USBD_free(pdev->pClassDataCmsit[pdev->classId]);
    pdev->pClassDataCmsit[pdev->classId] = NULL;
    pdev->pClassData = NULL;
  }
	return (uint8_t)USBD_OK;

}
static uint8_t USBD_WinUsb_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
	USBD_WinUsb_HandleTypeDef *hcdc = (USBD_WinUsb_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];
   
  USBD_StatusTypeDef ret = USBD_OK;

  if (hcdc == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }
//  printf("winusb bmRequest=%x\r\n",req->bmRequest);
 switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
    case USB_REQ_TYPE_VENDOR:{
        USBD_WinUSBComm_SetupVendor(pdev, req);
      }break;

    default:
      USBD_CtlError(pdev, req);
      ret = USBD_FAIL;
      break;
  }

  return (uint8_t)ret;

}
static uint8_t USBD_WinUsb_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  USBD_WinUsb_HandleTypeDef *hcdc;
  PCD_HandleTypeDef *hpcd = (PCD_HandleTypeDef *)pdev->pData;

  if (pdev->pClassDataCmsit[pdev->classId] == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  hcdc = (USBD_WinUsb_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];

  if ((pdev->ep_in[epnum & 0xFU].total_length > 0U) &&
      ((pdev->ep_in[epnum & 0xFU].total_length % hpcd->IN_ep[epnum & 0xFU].maxpacket) == 0U))
  {
    /* Update the packet total length */
    pdev->ep_in[epnum & 0xFU].total_length = 0U;

    /* Send ZLP */
    (void)USBD_LL_Transmit(pdev, epnum, NULL, 0U);
  }
  else
  {
    hcdc->TxState = 0U;

    if (((USBD_WinUsb_ItfTypeDef *)pdev->pUserData[pdev->classId])->TransmitCplt != NULL)
    {
      ((USBD_WinUsb_ItfTypeDef *)pdev->pUserData[pdev->classId])->TransmitCplt(hcdc->TxBuffer, &hcdc->TxLength, epnum);
    }
  }

  return (uint8_t)USBD_OK;	

}

static uint8_t USBD_WinUsb_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  USBD_WinUsb_HandleTypeDef *hcdc = (USBD_WinUsb_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];

  if (pdev->pClassDataCmsit[pdev->classId] == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  /* Get the received data length */
  hcdc->RxLength = USBD_LL_GetRxDataSize(pdev, epnum);

  /* USB data will be immediately processed, this allow next USB traffic being
  NAKed till the end of the application Xfer */

  ((USBD_WinUsb_ItfTypeDef *)pdev->pUserData[pdev->classId])->Receive(hcdc->RxBuffer, &hcdc->RxLength);

  return (uint8_t)USBD_OK;

}
static uint8_t USBD_WinUsb_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
	return (uint8_t)USBD_OK;

}

uint8_t WinUsbRxBufferFS[winAPP_RX_DATA_SIZE];
uint8_t WinUsbTxBufferFS[winAPP_TX_DATA_SIZE];
extern USBD_HandleTypeDef hUsbDeviceFS;

uint8_t USBD_WinUsb_SetTxBuffer(USBD_HandleTypeDef *pdev,
                             uint8_t *pbuff, uint32_t length)
{
  USBD_WinUsb_HandleTypeDef *hcdc = (USBD_WinUsb_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];

  if (hcdc == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  hcdc->TxBuffer = pbuff;
  hcdc->TxLength = length;

  return (uint8_t)USBD_OK;
}
uint8_t USBD_WinUsb_SetRxBuffer(USBD_HandleTypeDef *pdev, uint8_t *pbuff)
{
	USBD_WinUsb_HandleTypeDef *hcdc = (USBD_WinUsb_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];

	if (hcdc == NULL)
	{
	 	return (uint8_t)USBD_FAIL;
	}

	hcdc->RxBuffer = pbuff;

	return (uint8_t)USBD_OK;
}

uint8_t USBD_WinUsb_ReceivePacket(USBD_HandleTypeDef *pdev)
{
  USBD_WinUsb_HandleTypeDef *hcdc = (USBD_WinUsb_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];

  if (pdev->pClassDataCmsit[pdev->classId] == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }
  if (pdev->dev_speed == USBD_SPEED_HIGH)
  {
    /* Prepare Out endpoint to receive next packet */
    (void)USBD_LL_PrepareReceive(pdev, WinUsbOutEpAdd, hcdc->RxBuffer,
                                 WIN_DATA_HS_OUT_PACKET_SIZE);
  }
  else
  {
    /* Prepare Out endpoint to receive next packet */
    (void)USBD_LL_PrepareReceive(pdev, WinUsbOutEpAdd, hcdc->RxBuffer,
                                 WIN_DATA_FS_OUT_PACKET_SIZE);
  }
 return (uint8_t)USBD_OK;
}

uint8_t USBD_WinUsb_TransmitPacket(USBD_HandleTypeDef *pdev)
{
  USBD_WinUsb_HandleTypeDef *hcdc = (USBD_WinUsb_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];
  USBD_StatusTypeDef ret = USBD_BUSY;

  if (pdev->pClassDataCmsit[pdev->classId] == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  if (hcdc->TxState == 0U)
  {
    /* Tx Transfer in progress */
    hcdc->TxState = 1U;

    /* Update the packet total length */
    pdev->ep_in[WinUsbInEpAdd & 0xFU].total_length = hcdc->TxLength;

    /* Transmit next packet */
    (void)USBD_LL_Transmit(pdev, WinUsbInEpAdd, hcdc->TxBuffer, hcdc->TxLength);

    ret = USBD_OK;
  }

  return (uint8_t)ret;
}
static int8_t WinUsb_Init_FS(void)
{
  /* USER CODE BEGIN 3 */
  /* Set Application Buffers */
  USBD_WinUsb_SetTxBuffer(&hUsbDeviceFS, WinUsbTxBufferFS, 0);
  USBD_WinUsb_SetRxBuffer(&hUsbDeviceFS, WinUsbRxBufferFS);
  return (USBD_OK);
  /* USER CODE END 3 */
}
static int8_t WinUsb_DeInit_FS(void)
{
  /* USER CODE BEGIN 4 */
  return (USBD_OK);
  /* USER CODE END 4 */
}
uint8_t WinUsb_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
  uint8_t result = USBD_OK;
  /* USER CODE BEGIN 7 */
  USBD_WinUsb_HandleTypeDef *hcdc = (USBD_WinUsb_HandleTypeDef*)hUsbDeviceFS.pClassData;
  if (hcdc->TxState != 0){
    return USBD_BUSY;
  }
  USBD_WinUsb_SetTxBuffer(&hUsbDeviceFS, Buf, Len);
  result = USBD_WinUsb_TransmitPacket(&hUsbDeviceFS);
  /* USER CODE END 7 */
  return result;
}
static int8_t WinUsb_Receive_FS(uint8_t* Buf, uint32_t *Len)
{
  /* USER CODE BEGIN 6 */
  int i;
  for(i=0 ; i<*Len; i++)
    printf("%.2x ",Buf[i]);
  printf("\r\n");
  USBD_WinUsb_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
  USBD_WinUsb_ReceivePacket(&hUsbDeviceFS);
  return (USBD_OK);
  /* USER CODE END 6 */
}
static int8_t WinUsb_TransmitCplt_FS(uint8_t *Buf, uint32_t *Len, uint8_t epnum)
{
  uint8_t result = USBD_OK;
  /* USER CODE BEGIN 13 */
  UNUSED(Buf);
  UNUSED(Len);
  UNUSED(epnum);
  /* USER CODE END 13 */
  return result;
}
USBD_WinUsb_ItfTypeDef USBD_WinUsb_Interface_fops_FS =
{
  WinUsb_Init_FS,
  WinUsb_DeInit_FS, 
  WinUsb_Receive_FS,
  WinUsb_TransmitCplt_FS
};


uint8_t USBD_WinUsb_RegisterInterface(USBD_HandleTypeDef *pdev,
                                   USBD_WinUsb_ItfTypeDef *fops)
{
  if (fops == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }
  
  pdev->classId --;
  pdev->pUserData[pdev->classId] = fops;
 
  pdev->classId ++;
  return (uint8_t)USBD_OK;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

