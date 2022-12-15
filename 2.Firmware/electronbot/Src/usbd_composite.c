#include "usbd_composite.h"
#include "usbd_conf.h"
#include "usbd_def.h"
#include "usbd_msc.h"
#include "usbd_winUsb.h"

#include "usbd_core.h"
#include "usbd_desc.h"



#include <stdio.h>

#define USBD_PRODUCT_XSTR(s) USBD_PRODUCT_STR(s)
#define USBD_PRODUCT_STR(s) #s


#if 1
#define USBD_CMPSIT_VID     0x1214
#define USBD_CMPSIT_LANGID_STRING     1033
#define USBD_CMPSIT_MANUFACTURER_STRING     "wenyz@wolfGroup"
#define USBD_CMPSIT_PID_FS     0x0D3A
#define USBD_CMPSIT_PRODUCT_STRING_FS     "STM32 composite product"
#define USBD_CMPSIT_CONFIGURATION_STRING_FS     "Composite Config"
#define USBD_CMPSIT_INTERFACE_STRING_FS     "Composite Interface"
#else
#define USBD_CMPSIT_VID     1223
#define USBD_CMPSIT_LANGID_STRING     1033
#define USBD_CMPSIT_MANUFACTURER_STRING     "test"
#define USBD_CMPSIT_PID_FS     55663
#define USBD_CMPSIT_PRODUCT_STRING_FS     "test"
#define USBD_CMPSIT_CONFIGURATION_STRING_FS     "test Config"
#define USBD_CMPSIT_INTERFACE_STRING_FS     "test Interface"
#endif

#define USBD_MSC_INTERFACE_STRING_FS     "MSC Interface"
#define USBD_WIN_USB_INTERFACE_STRING_FS  Electronbot Native Interface


uint8_t * USBD_CMPSIT_FS_DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t * USBD_CMPSIT_FS_LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t * USBD_CMPSIT_FS_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t * USBD_CMPSIT_FS_ProductStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t * USBD_CMPSIT_FS_SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t * USBD_CMPSIT_FS_ConfigStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t * USBD_CMPSIT_FS_InterfaceStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
#if (USBD_LPM_ENABLED == 1)
uint8_t * USBD_CMPSIT_FS_USR_BOSDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
#endif /* (USBD_LPM_ENABLED == 1) */


USBD_DescriptorsTypeDef usbCmpsitFS_Desc =
{
  USBD_CMPSIT_FS_DeviceDescriptor
, USBD_CMPSIT_FS_LangIDStrDescriptor
, USBD_CMPSIT_FS_ManufacturerStrDescriptor
, USBD_CMPSIT_FS_ProductStrDescriptor
, USBD_CMPSIT_FS_SerialStrDescriptor
, USBD_CMPSIT_FS_ConfigStrDescriptor
, USBD_CMPSIT_FS_InterfaceStrDescriptor
#if (USBD_LPM_ENABLED == 1)
, USBD_CMPSIT_FS_USR_BOSDescriptor
#endif /* (USBD_LPM_ENABLED == 1) */
};

__ALIGN_BEGIN uint8_t USBD_CMPSIT_FS_DeviceDesc[USB_LEN_DEV_DESC] __ALIGN_END =
{
	0x12,						/*bLength */
	USB_DESC_TYPE_DEVICE, 	  /*bDescriptorType*/
#if (USBD_LPM_ENABLED == 1)
	0x01, 					  /*bcdUSB */ /* changed to USB version 2.01
											 in order to support LPM L1 suspend
											 resume test of USBCV3.0*/
#else
	0x00, 					  /*bcdUSB */
#endif /* (USBD_LPM_ENABLED == 1) */
	0x02,
	// Notify OS that this is a composite device
	0xEF, 					  /*bDeviceClass*/
	0x02, 					  /*bDeviceSubClass*/
	0x01, 					  /*bDeviceProtocol*/
	USB_MAX_EP0_SIZE, 		  /*bMaxPacketSize*/
	LOBYTE(USBD_CMPSIT_VID), 		  /*idVendor*/
	HIBYTE(USBD_CMPSIT_VID), 		  /*idVendor*/
	LOBYTE(USBD_CMPSIT_PID_FS),		  /*idProduct*/
	HIBYTE(USBD_CMPSIT_PID_FS),		  /*idProduct*/
	0x00, 					  /*bcdDevice rel. 2.00*/
	0x02, 					  
	USBD_IDX_MFC_STR, 		  /*Index of manufacturer  string*/
	USBD_IDX_PRODUCT_STR, 	  /*Index of product string*/
	USBD_IDX_SERIAL_STR,		  /*Index of serial number string*/
	USBD_MAX_NUM_CONFIGURATION  /*bNumConfigurations*/

};

#if (USBD_LPM_ENABLED == 1)
__ALIGN_BEGIN uint8_t USBD_CMPSIT_FS_BOSDesc[USB_SIZ_BOS_DESC] __ALIGN_END =
{
  0x5,
  USB_DESC_TYPE_BOS,
  0xC,
  0x0,
  0x1,  /* 1 device capability*/
        /* device capability*/
  0x7,
  USB_DEVICE_CAPABITY_TYPE,
  0x2,
  0x2,  /* LPM capability bit set*/
  0x0,
  0x0,
  0x0
};
#endif /* (USBD_LPM_ENABLED == 1) */

/** USB lang identifier descriptor. */
__ALIGN_BEGIN uint8_t USBD_CMPSIT_LangIDDesc[USB_LEN_LANGID_STR_DESC] __ALIGN_END =
{
     USB_LEN_LANGID_STR_DESC,
     USB_DESC_TYPE_STRING,
     LOBYTE(USBD_CMPSIT_LANGID_STRING),
     HIBYTE(USBD_CMPSIT_LANGID_STRING)
};


/* Internal string descriptor. */
__ALIGN_BEGIN uint8_t USBD_CMPSIT_StrDesc[USBD_MAX_STR_DESC_SIZ] __ALIGN_END;


__ALIGN_BEGIN uint8_t USBD_CMPSIT_StringSerial[USB_SIZ_STRING_SERIAL] __ALIGN_END = {
  USB_SIZ_STRING_SERIAL,
  USB_DESC_TYPE_STRING,
};
 

uint8_t * USBD_CMPSIT_FS_DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  UNUSED(speed);
  *length = sizeof(USBD_CMPSIT_FS_DeviceDesc);
  return USBD_CMPSIT_FS_DeviceDesc;
}

uint8_t * USBD_CMPSIT_FS_LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  UNUSED(speed);
  *length = sizeof(USBD_CMPSIT_LangIDDesc);
  return USBD_CMPSIT_LangIDDesc;
}
uint8_t * USBD_CMPSIT_FS_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  UNUSED(speed);
  USBD_GetString((uint8_t *)USBD_CMPSIT_MANUFACTURER_STRING, USBD_CMPSIT_StrDesc, length);
  return USBD_CMPSIT_StrDesc;
}

uint8_t * USBD_CMPSIT_FS_ProductStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  if(speed == 0)
  {
    USBD_GetString((uint8_t *)USBD_CMPSIT_PRODUCT_STRING_FS, USBD_CMPSIT_StrDesc, length);
  }
  else
  {
    USBD_GetString((uint8_t *)USBD_CMPSIT_PRODUCT_STRING_FS, USBD_CMPSIT_StrDesc, length);
  }
  return USBD_CMPSIT_StrDesc;
}

static void IntToUnicode(uint32_t value, uint8_t * pbuf, uint8_t len)
{
  uint8_t idx = 0;

  for (idx = 0; idx < len; idx++)
  {
    if (((value >> 28)) < 0xA)
    {
      pbuf[2 * idx] = (value >> 28) + '0';
    }
    else
    {
      pbuf[2 * idx] = (value >> 28) + 'A' - 10;
    }

    value = value << 4;

    pbuf[2 * idx + 1] = 0;
  }
}
static void Get_CMPSIT_SerialNum(void)
{
  uint32_t deviceserial0, deviceserial1, deviceserial2;

  deviceserial0 = *(uint32_t *) DEVICE_ID1;
  deviceserial1 = *(uint32_t *) DEVICE_ID2;
  deviceserial2 = *(uint32_t *) DEVICE_ID3;

  deviceserial0 += deviceserial2;

  if (deviceserial0 != 0)
  {
    IntToUnicode(deviceserial0, &USBD_CMPSIT_StringSerial[2], 8);
    IntToUnicode(deviceserial1, &USBD_CMPSIT_StringSerial[18], 4);
  }
}
uint8_t * USBD_CMPSIT_FS_SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  UNUSED(speed);
  *length = USB_SIZ_STRING_SERIAL;

  /* Update the serial number string descriptor with the data from the unique
   * ID */
  Get_CMPSIT_SerialNum();
  /* USER CODE BEGIN USBD_FS_SerialStrDescriptor */

  /* USER CODE END USBD_FS_SerialStrDescriptor */
  return (uint8_t *) USBD_CMPSIT_StringSerial;
}

uint8_t * USBD_CMPSIT_FS_InterfaceStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  if(speed == 0)
  {
    USBD_GetString((uint8_t *)USBD_CMPSIT_INTERFACE_STRING_FS, USBD_CMPSIT_StrDesc, length);
  }
  else
  {
    USBD_GetString((uint8_t *)USBD_CMPSIT_INTERFACE_STRING_FS, USBD_CMPSIT_StrDesc, length);
  }
  return USBD_CMPSIT_StrDesc;
}


uint8_t * USBD_CMPSIT_FS_ConfigStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  if(speed == USBD_SPEED_HIGH)
  {
    USBD_GetString((uint8_t *)USBD_CMPSIT_CONFIGURATION_STRING_FS, USBD_CMPSIT_StrDesc, length);
  }
  else
  {
    USBD_GetString((uint8_t *)USBD_CMPSIT_CONFIGURATION_STRING_FS, USBD_CMPSIT_StrDesc, length);
  }
  return USBD_CMPSIT_StrDesc;
}


#if (USBD_LPM_ENABLED == 1)
/**
  * @brief  Return the BOS descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t * USBD_CMPSIT_FS_USR_BOSDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  UNUSED(speed);
  *length = sizeof(USBD_CMPSIT_FS_BOSDesc);
  return (uint8_t*)USBD_CMPSIT_FS_BOSDesc;
}
#endif /* (USBD_LPM_ENABLED == 1) */


__ALIGN_BEGIN static uint8_t USBD_CMPSIT_CfgDesc[USB_CMPSIT_CONFIG_DESC_SIZ] __ALIGN_END =
{
  /* Configuration Descriptor */
  //0
  0x09,                                       /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,                /* bDescriptorType: Configuration */
  USB_CMPSIT_CONFIG_DESC_SIZ,                    /* wTotalLength */
  0x00,
  0x02,                                       /* bNumInterfaces: 2 interfaces */
  0x01,                                       /* bConfigurationValue: Configuration value */
  0x00,                                       /* iConfiguration: Index of string descriptor
                                                 describing the configuration */
#if (USBD_SELF_POWERED == 1U)
  0xC0,                                       /* bmAttributes: Bus Powered according to user configuration */
#else
  0x80,                                       /* bmAttributes: Bus Powered according to user configuration */
#endif /* USBD_SELF_POWERED */
  USBD_MAX_POWER,                             /* MaxPower (mA) */

  /*---------------------------------------------------------------------------*/
  //9
  /* Interface Association Descriptor: CDC device (virtual com port) */
  0x08,   /* bLength: IAD size */
  0x0B,   /* bDescriptorType: Interface Association Descriptor */
  0x00,   /* bFirstInterface */
  0x01,   /* bInterfaceCount */
  0x08,   /* bFunctionClass: Communication Interface Class */
  0x06,   /* bFunctionSubClass: Abstract Control Model */
  0x50,   /* bFunctionProtocol: Common AT commands */
  0x06,   /* iFunction */
  //17
  /* Interface Descriptor */
  /********************  Mass Storage interface ********************/
  0x09,                                            /* bLength: Interface Descriptor size */
  USB_DESC_TYPE_INTERFACE,                         /* bDescriptorType: */
  0x00,                                            /* bInterfaceNumber: Number of Interface */
  0x00,                                            /* bAlternateSetting: Alternate setting */
  0x02,                                            /* bNumEndpoints */
  0x08,                                            /* bInterfaceClass: MSC Class */
  0x06,                                            /* bInterfaceSubClass : SCSI transparent*/
  0x50,                                            /* nInterfaceProtocol */
  0x00,                                            /* iInterface: */

  
  //26 
   /********************  Mass Storage Endpoints ********************/
  0x07,                                            /* Endpoint descriptor length = 7 */
  0x05,                                            /* Endpoint descriptor type */
  MSC_EPIN_ADDR,                                   /* Endpoint address (IN, address 1) */
  0x02,                                            /* Bulk endpoint type */
  LOBYTE(MSC_MAX_FS_PACKET),
  HIBYTE(MSC_MAX_FS_PACKET),
  0x00,                                            /* Polling interval in milliseconds */
  //33
  0x07,                                            /* Endpoint descriptor length = 7 */
  0x05,                                            /* Endpoint descriptor type */
  MSC_EPOUT_ADDR,                                  /* Endpoint address (OUT, address 1) */
  0x02,                                            /* Bulk endpoint type */
  LOBYTE(MSC_MAX_FS_PACKET),
  HIBYTE(MSC_MAX_FS_PACKET),
  0x00,                                             /* Polling interval in milliseconds */

  
  //40
  /* Interface Association Descriptor: custom device */
  0x08,   /* bLength: IAD size */
  0x0B,   /* bDescriptorType: Interface Association Descriptor */
  0x01,   /* bFirstInterface */
  0x01,   /* bInterfaceCount */
  0x00,   /* bFunctionClass: */
  0x00,   /* bFunctionSubClass: */
  0x00,   /* bFunctionProtocol: */
  0x07,   /* iFunction */
  //48
  /********************  winUsb interface ********************/
   /*Data class interface descriptor*/
  0x09,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: */
  0x01,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints: Two endpoints used */
  0x00,   /* bInterfaceClass: vendor specific */
  0x01,   /* bInterfaceSubClass: ODrive Communication */
  0x00,   /* bInterfaceProtocol: */
  0x00,   /* iInterface: */
  /********************  Mass Storage Endpoints ********************/
  //57
  0x07,                                            /* Endpoint descriptor length = 7 */
  USB_DESC_TYPE_ENDPOINT,                                            /* Endpoint descriptor type */
  WIN_USB_EPOUT_ADDR,                                   /* Endpoint address (IN, address 1) */
  0x02,                                            /* Bulk endpoint type */
  LOBYTE(WIN_DATA_FS_MAX_PACKET_SIZE),
  HIBYTE(WIN_DATA_FS_MAX_PACKET_SIZE),
  0x00,                                            /* Polling interval in milliseconds */
  //64
  0x07,                                            /* Endpoint descriptor length = 7 */
  USB_DESC_TYPE_ENDPOINT,                                            /* Endpoint descriptor type */
  WIN_USB_EPIN_ADDR,                                  /* Endpoint address (OUT, address 1) */
  0x02,                                            /* Bulk endpoint type */
  LOBYTE(WIN_DATA_FS_MAX_PACKET_SIZE),
  HIBYTE(WIN_DATA_FS_MAX_PACKET_SIZE),
  0x00                                             /* Polling interval in milliseconds */
  //71
};
__ALIGN_BEGIN static uint8_t USBD_CMPSIT_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};

static uint8_t *USBD_CMPSIT_GetFSCfgDesc(uint16_t *length);
static uint8_t *USBD_CMPSIT_GetHSCfgDesc(uint16_t *length);
static uint8_t *USBD_CMPSIT_GetOtherSpeedCfgDesc(uint16_t *length);
uint8_t *USBD_CMPSIT_GetDeviceQualifierDescriptor(uint16_t *length);



USBD_ClassTypeDef USBD_CMPSIT=
{
  NULL,
  NULL,
  NULL,
  NULL,                 /* EP0_TxSent */
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  USBD_CMPSIT_GetHSCfgDesc,
  USBD_CMPSIT_GetFSCfgDesc,  
  USBD_CMPSIT_GetOtherSpeedCfgDesc,
  USBD_CMPSIT_GetDeviceQualifierDescriptor  
};

#ifdef USE_USBD_COMPOSITE
void USBD_CMPSIT_AddClass(USBD_HandleTypeDef *pdev, USBD_ClassTypeDef *pclass, USBD_CompositeClassTypeDef classtype, uint8_t *EpAddr)
{
	//printf("classId=%d  : %d\r\n",pdev->classId,classtype);
	switch(classtype)
	{
		case CLASS_TYPE_MSC:{
			pdev->tclasslist[pdev->classId].ClassType = CLASS_TYPE_MSC;
			
			pdev->tclasslist[pdev->classId].Active = 1U;
			pdev->tclasslist[pdev->classId].NumEps = 2;
			
			pdev->tclasslist[pdev->classId].Eps[1].add = MSC_EPOUT_ADDR;
			pdev->tclasslist[pdev->classId].Eps[1].type = USBD_EP_TYPE_BULK;
			pdev->tclasslist[pdev->classId].Eps[1].size = MSC_MAX_FS_PACKET;
			pdev->tclasslist[pdev->classId].Eps[1].is_used = 1U;
			
			pdev->tclasslist[pdev->classId].Eps[0].add = MSC_EPIN_ADDR;
			pdev->tclasslist[pdev->classId].Eps[0].type = USBD_EP_TYPE_BULK;
			pdev->tclasslist[pdev->classId].Eps[0].size = MSC_MAX_FS_PACKET;
			pdev->tclasslist[pdev->classId].Eps[0].is_used = 1U;

			pdev->tclasslist[pdev->classId].NumIf = 1;
			pdev->tclasslist[pdev->classId].Ifs[0] = 0;			
			
		}break;

		case CLASS_TYPE_WINUSB:{
			pdev->tclasslist[pdev->classId].ClassType = CLASS_TYPE_WINUSB;
			
			pdev->tclasslist[pdev->classId].Active = 1U;
			pdev->tclasslist[pdev->classId].NumEps = 2;
			
			pdev->tclasslist[pdev->classId].Eps[0].add = WIN_USB_EPOUT_ADDR;
			pdev->tclasslist[pdev->classId].Eps[0].type = USBD_EP_TYPE_BULK;
			pdev->tclasslist[pdev->classId].Eps[0].size = WIN_DATA_FS_MAX_PACKET_SIZE;
			pdev->tclasslist[pdev->classId].Eps[0].is_used = 1U;
			
			pdev->tclasslist[pdev->classId].Eps[1].add = WIN_USB_EPIN_ADDR;
			pdev->tclasslist[pdev->classId].Eps[1].type = USBD_EP_TYPE_BULK;
			pdev->tclasslist[pdev->classId].Eps[1].size = WIN_DATA_FS_MAX_PACKET_SIZE;
			pdev->tclasslist[pdev->classId].Eps[1].is_used = 1U;
			

			pdev->tclasslist[pdev->classId].NumIf = 1;
			pdev->tclasslist[pdev->classId].Ifs[0] = 1;
			
		}break;
		default:break;
	}
	pdev->tclasslist[pdev->classId].CurrPcktSze = 0U;
		
}
void USBD_CMPST_ClearConfDesc(USBD_HandleTypeDef *pdev)
{
}

#endif
__ALIGN_BEGIN uint8_t USBD_MS_OS_StringDescriptor[]  __ALIGN_END =
{
  0x12,           //  bLength           1 0x12  Length of the descriptor
  0x03,           //  bDescriptorType   1 0x03  Descriptor type
                  //  qwSignature      14 ‘MSFT100’ Signature field
  0x4D, 0x00,     //  'M'
  0x53, 0x00,     //  'S'
  0x46, 0x00,     //  'F'
  0x54, 0x00,     //  'T'
  0x31, 0x00,     //  '1'
  0x30, 0x00,     //  '0'
  0x30, 0x00,     //  '0'
  MS_VendorCode,  //  bMS_VendorCode    1 Vendor-specific Vendor code
  0x00            //  bPad              1 0x00  Pad field
};
uint8_t * USBD_UsrStrDescriptor(struct _USBD_HandleTypeDef *pdev, uint8_t index,  uint16_t *length)
{
  *length = 0;  
  // printf("index=%x\r\n",index);
  if (USBD_IDX_MICROSOFT_DESC_STR == index) {
    *length = sizeof (USBD_MS_OS_StringDescriptor);
    return USBD_MS_OS_StringDescriptor;
  } else if (USBD_IDX_MSC_INTF_STR == index) {
    USBD_GetString((uint8_t *)USBD_MSC_INTERFACE_STRING_FS, USBD_CMPSIT_StrDesc, length);
    return USBD_CMPSIT_StrDesc;
  }  
  else if (USBD_IDX_WIN_USB_INTF_STR == index) {
    USBD_GetString((uint8_t *)USBD_PRODUCT_XSTR(USBD_WIN_USB_INTERFACE_STRING_FS), USBD_CMPSIT_StrDesc, length);
    return USBD_CMPSIT_StrDesc;
  }  
  return NULL;
}

static uint8_t *USBD_CMPSIT_GetFSCfgDesc(uint16_t *length)
{
  USBD_EpDescTypeDef *pMscEpOutDesc = USBD_GetEpDesc(USBD_CMPSIT_CfgDesc, MSC_EPOUT_ADDR);
  USBD_EpDescTypeDef *pMscEpInDesc = USBD_GetEpDesc(USBD_CMPSIT_CfgDesc, MSC_EPIN_ADDR);

  USBD_EpDescTypeDef *pWinUsbEpOutDesc = USBD_GetEpDesc(USBD_CMPSIT_CfgDesc, WIN_USB_EPOUT_ADDR);
  USBD_EpDescTypeDef *pWinUsbEpInDesc = USBD_GetEpDesc(USBD_CMPSIT_CfgDesc, WIN_USB_EPIN_ADDR);

  

  if (pMscEpOutDesc != NULL)
  {
    pMscEpOutDesc->wMaxPacketSize = MSC_MAX_FS_PACKET;
  }

  if (pMscEpInDesc != NULL)
  {
    pMscEpInDesc->wMaxPacketSize = MSC_MAX_FS_PACKET;
  }
  if (pWinUsbEpOutDesc != NULL)
  {
    pWinUsbEpOutDesc->wMaxPacketSize = WIN_DATA_FS_MAX_PACKET_SIZE;
  }

  if (pWinUsbEpInDesc != NULL)
  {
    pWinUsbEpInDesc->wMaxPacketSize = WIN_DATA_FS_MAX_PACKET_SIZE;
  }
	
	*length = (uint16_t)sizeof(USBD_CMPSIT_CfgDesc);
	return USBD_CMPSIT_CfgDesc;
}
static uint8_t *USBD_CMPSIT_GetHSCfgDesc(uint16_t *length)
{
  USBD_EpDescTypeDef *pMscEpOutDesc = USBD_GetEpDesc(USBD_CMPSIT_CfgDesc, MSC_EPOUT_ADDR);
  USBD_EpDescTypeDef *pMscEpInDesc = USBD_GetEpDesc(USBD_CMPSIT_CfgDesc, MSC_EPIN_ADDR);

  USBD_EpDescTypeDef *pWinUsbEpOutDesc = USBD_GetEpDesc(USBD_CMPSIT_CfgDesc, WIN_USB_EPOUT_ADDR);
  USBD_EpDescTypeDef *pWinUsbEpInDesc = USBD_GetEpDesc(USBD_CMPSIT_CfgDesc, WIN_USB_EPIN_ADDR);

  if (pMscEpOutDesc != NULL)
  {
    pMscEpOutDesc->wMaxPacketSize = MSC_MAX_HS_PACKET;
  }

  if (pMscEpInDesc != NULL)
  {
    pMscEpInDesc->wMaxPacketSize = MSC_MAX_HS_PACKET;
  }
  if (pWinUsbEpOutDesc != NULL)
  {
    pWinUsbEpOutDesc->wMaxPacketSize = WIN_DATA_HS_MAX_PACKET_SIZE;
  }

  if (pWinUsbEpInDesc != NULL)
  {
    pWinUsbEpInDesc->wMaxPacketSize = WIN_DATA_HS_MAX_PACKET_SIZE;
  }

  *length = (uint16_t)sizeof(USBD_CMPSIT_CfgDesc);
	return USBD_CMPSIT_CfgDesc;
}
static uint8_t *USBD_CMPSIT_GetOtherSpeedCfgDesc(uint16_t *length)
{
  USBD_EpDescTypeDef *pMscEpOutDesc = USBD_GetEpDesc(USBD_CMPSIT_CfgDesc, MSC_EPOUT_ADDR);
  USBD_EpDescTypeDef *pMscEpInDesc = USBD_GetEpDesc(USBD_CMPSIT_CfgDesc, MSC_EPIN_ADDR);

  USBD_EpDescTypeDef *pWinUsbEpOutDesc = USBD_GetEpDesc(USBD_CMPSIT_CfgDesc, WIN_USB_EPOUT_ADDR);
  USBD_EpDescTypeDef *pWinUsbEpInDesc = USBD_GetEpDesc(USBD_CMPSIT_CfgDesc, WIN_USB_EPIN_ADDR);

  if (pMscEpOutDesc != NULL)
  {
    pMscEpOutDesc->wMaxPacketSize = MSC_MAX_FS_PACKET;
  }

  if (pMscEpInDesc != NULL)
  {
    pMscEpInDesc->wMaxPacketSize = MSC_MAX_FS_PACKET;
  }
  if (pWinUsbEpOutDesc != NULL)
  {
    pWinUsbEpOutDesc->wMaxPacketSize = WIN_DATA_FS_MAX_PACKET_SIZE;
  }

  if (pWinUsbEpInDesc != NULL)
  {
    pWinUsbEpInDesc->wMaxPacketSize = WIN_DATA_FS_MAX_PACKET_SIZE;
  }
  *length = (uint16_t)sizeof(USBD_CMPSIT_CfgDesc);
  return USBD_CMPSIT_CfgDesc;
}
uint8_t *USBD_CMPSIT_GetDeviceQualifierDescriptor(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_CMPSIT_DeviceQualifierDesc);
  return USBD_CMPSIT_DeviceQualifierDesc;
}

