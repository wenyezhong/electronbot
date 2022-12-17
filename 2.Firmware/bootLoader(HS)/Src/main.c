/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "usbd_winUsb.h"
#include "common_inc.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern UsbBuffer_t usbBuffer;
uint16_t recPacketNo;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int _write(int fd, char *ch, int len)
{
  HAL_UART_Transmit(&huart1, (uint8_t*)ch, len, 0xFFFF);
  return len;
}
void SendUsbPacket(uint8_t* _data, uint32_t _len)
{
    uint8_t ret;
    do
    {
        // ret = CDC_Transmit_HS(_data, _len);
        ret = WinUsb_Transmit_FS(_data, _len);
    } while (ret != USBD_OK);
}
void ReceiveUsbPacketUntilSizeIs(uint32_t _count)
{
    while (usbBuffer.receivedPacketLen != _count);
    usbBuffer.receivedPacketLen = 0;
}
uint8_t* GetExtraDataRxPtr()
{
  uint8_t* ptr;
  ptr=usbBuffer.rxData[usbBuffer.pingPongIndex == 0 ? 1 : 0];
  return ptr + 60 * 240 * 3;
}

void JumpBootLoader(uint8_t *ptr)
{
    usbBuffer.extraDataTx[31] = 0xef;
    usbBuffer.extraDataTx[30] = 0x01;
    usbBuffer.extraDataTx[29] = 0x00;
    SendUsbPacket(usbBuffer.extraDataTx, 32);
    // HAL_Delay(120);
    // NVIC_SystemReset();
}
void QueryBootLoaderReply(uint8_t *ptr)
{
  usbBuffer.extraDataTx[31] = 0xef;
  usbBuffer.extraDataTx[30] = 0x02;
  usbBuffer.extraDataTx[29] = 0x00;
  SendUsbPacket(usbBuffer.extraDataTx, 32);
}
void EraseAppCodeReply(uint8_t *ptr)
{
  usbBuffer.extraDataTx[31] = 0xef;
  usbBuffer.extraDataTx[30] = 0x03;
  usbBuffer.extraDataTx[29] = 0x00;
  SendUsbPacket(usbBuffer.extraDataTx, 32);
}
void downLoadAppCodeReply(uint8_t *ptr,uint8_t pacektNo)
{
  usbBuffer.extraDataTx[31] = 0xef;
  usbBuffer.extraDataTx[30] = 0x04;
  if(pacektNo == recPacketNo)
    usbBuffer.extraDataTx[29] = 0x00;
  else
    usbBuffer.extraDataTx[29] = 0x01;
  *(uint16_t*)&usbBuffer.extraDataTx[27] = pacektNo;
  SendUsbPacket(usbBuffer.extraDataTx, 32);
}

void NoticeCompleteReply(uint8_t *ptr,uint8_t pacektNo)
{
  usbBuffer.extraDataTx[31] = 0xef;
  usbBuffer.extraDataTx[30] = 0x05; 
  // usbBuffer.extraDataTx[29] = 0x00;  
  *(uint16_t*)&usbBuffer.extraDataTx[28] = pacektNo;
  SendUsbPacket(usbBuffer.extraDataTx, 32);

}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint16_t currentPacketNo;
  uint16_t perSize;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USB_DEVICE_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  printf("bootLoader begin...\r\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    ReceiveUsbPacketUntilSizeIs(224);
    uint8_t* ptr = GetExtraDataRxPtr();
    uint16_t cmd = *(uint16_t*)(ptr+30);
    printf("cmd = %.4x\r\n",cmd);
    switch(cmd)
    {
      case 0xfe01:{
        
        printf("notice to bootLoader\r\n");
        JumpBootLoader(ptr);
        
      }break;
      case 0xfe02:{
        printf("query bootLoader\r\n");
        QueryBootLoaderReply(ptr);

      }break;
      case 0xfe03:{
        uint32_t totalSize = *(uint32_t*)(ptr+26);
        currentPacketNo = 0;
        printf("Erase app code\r\n");
        printf("totalSize = %u\r\n",totalSize);
        EraseAppCodeReply(ptr);

      }break;
      case 0xfe04:{
        recPacketNo = *(uint16_t*)(ptr+28);
        perSize = *(uint16_t*)(ptr+24);        
        printf("downLoad app code\r\n");
        printf("recPacketNo = %d  perSize=%d\r\n",recPacketNo,perSize);
        downLoadAppCodeReply(ptr,currentPacketNo);
        currentPacketNo ++;

      }break;
      case 0xfe05:{        
        printf("complete downLoad app\r\n");
        NoticeCompleteReply(ptr,currentPacketNo);
        //跳转应用区

      }break;
      default:break;
    }
    if(ptr[31] == 0xfe)
    {
      
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
