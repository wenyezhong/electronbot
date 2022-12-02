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

  USBD_LL_OpenEP(pdev,WIN_USB_EPIN_ADDR,USBD_EP_TYPE_BULK,WIN_DATA_FS_IN_PACKET_SIZE);
  USBD_LL_OpenEP(pdev,WIN_USB_EPOUT_ADDR,USBD_EP_TYPE_BULK,WIN_DATA_FS_OUT_PACKET_SIZE);
  
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

  USBD_LL_PrepareReceive(pdev,
                          WIN_USB_EPOUT_ADDR,
                          hcdc->RxBuffer,
                          WIN_DATA_FS_OUT_PACKET_SIZE);
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
	return (uint8_t)USBD_OK;

}
static uint8_t USBD_WinUsb_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
	return (uint8_t)USBD_OK;

}

static uint8_t USBD_WinUsb_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
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


USBD_WinUsb_ItfTypeDef USBD_WinUsb_Interface_fops_FS =
{
  WinUsb_Init_FS,
  WinUsb_DeInit_FS,
  NULL,
  NULL,
  NULL
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

