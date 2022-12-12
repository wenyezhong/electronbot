#include <cmath>
#include "common_inc.h"
#include "screen.h"
#include "robot.h"
#include <stdio.h>

Robot electron(&hspi1, &hi2c1);
float jointSetPoints[6];
bool isEnabled = false;
/* Thread Definitions -----------------------------------------------------*/


/* Timer Callbacks -------------------------------------------------------*/


/* Default Entry -------------------------------------------------------*/
void Main(void)
{
    int i;
    HAL_Delay(1000);
    electron.lcd->Init(Screen::DEGREE_0);
    electron.lcd->SetWindow(0, 239, 0, 239);
    for(int i=0;i<32;i++)
        electron.pUsbBuffer->extraDataTx[i]=32-i;
    while(1)
    {
        //electron.SendUsbPacket(electron.pUsbBuffer->extraDataTx, 32);
        // HAL_Delay(500);
        electron.ReceiveUsbPacketUntilSizeIs(224); // last packet is 224bytes
        uint8_t* ptr = electron.GetExtraDataRxPtr();
        if(ptr[31] == 0xee)  //设置参数
        {
            for(i=0 ; i<32; i++)
                printf("%.2x ",ptr[i]);
            printf("\r\n");
            jointSetPoints[0] = *((float*) (ptr + 2));
            jointSetPoints[1] = 45.0f;
            printf("jointSetPoints=%f\r\n",jointSetPoints[0]);
            printf("jointSetPoints=%f\r\n",jointSetPoints[1]);
            *((float*) (ptr + 2))= 45.0f;
            for(i=0 ; i<32; i++)
                printf("%.2x ",ptr[i]);
            printf("\r\n");
        }
        //printf("111111\r\n");
    }
    printf("2333\r\n");
    electron.SetJointInitAngle(electron.joint[6], 0);
    electron.SetJointKp(electron.joint[6], 150);
    electron.SetJointKi(electron.joint[6], 0.8);
    electron.SetJointKd(electron.joint[6], 300);
    electron.SetJointTorqueLimit(electron.joint[6], 0.8);
    electron.SetJointEnable(electron.joint[6], true);
   /*  electron.UpdateJointAngle(electron.joint[1], 0);
    electron.UpdateJointAngle(electron.joint[2], 0);
    electron.UpdateJointAngle(electron.joint[3], 0);
    electron.UpdateJointAngle(electron.joint[4], 0);
    electron.UpdateJointAngle(electron.joint[5], 0); */
    electron.UpdateJointAngle(electron.joint[6], 45);
    

    float t = 0;
    printf("2333\r\n");
    while (true)
    {
#if 1
        for (int p = 0; p < 4; p++)
        {
            // send joint angles
            for (int j = 0; j < 6; j++)
                for (int i = 0; i < 4; i++)
                {
                    auto* b = (unsigned char*) &(electron.joint[j + 1].angle);
                    electron.pUsbBuffer->extraDataTx[j * 4 + i + 1] = *(b + i);
                }
                printf("123\r\n");
            electron.SendUsbPacket(electron.pUsbBuffer->extraDataTx, 32);
            printf("4444\r\n");
            electron.ReceiveUsbPacketUntilSizeIs(224); // last packet is 224bytes
            printf("111111\r\n");
            // get joint angles
            uint8_t* ptr = electron.GetExtraDataRxPtr();
			
            if (isEnabled != (bool) ptr[0])
            {
                isEnabled = ptr[0];
                /* electron.SetJointEnable(electron.joint[1], isEnabled);
                electron.SetJointEnable(electron.joint[2], isEnabled);
                electron.SetJointEnable(electron.joint[3], isEnabled);
                electron.SetJointEnable(electron.joint[4], isEnabled);
                electron.SetJointEnable(electron.joint[5], isEnabled); */
                // electron.SetJointEnable(electron.joint[6], isEnabled);
            }
            for (int j = 0; j < 6; j++)
            {
                jointSetPoints[j] = *((float*) (ptr + 4 * j + 1));
            }
            printf("33333\r\n");
            while (electron.lcd->isBusy);
            printf("222222\r\n");
            if (p == 0)
                electron.lcd->WriteFrameBuffer(electron.GetLcdBufferPtr(),
                                               60 * 240 * 3);
            else
                electron.lcd->WriteFrameBuffer(electron.GetLcdBufferPtr(),
                                               60 * 240 * 3, true);			
        }
        HAL_Delay(1);
#endif


        t += 0.01;

       /*  electron.UpdateJointAngle(electron.joint[1], jointSetPoints[0]);
        electron.UpdateJointAngle(electron.joint[2], jointSetPoints[1]);
        electron.UpdateJointAngle(electron.joint[3], jointSetPoints[2]);
        electron.UpdateJointAngle(electron.joint[4], jointSetPoints[3]);
        electron.UpdateJointAngle(electron.joint[5], jointSetPoints[4]);
        electron.UpdateJointAngle(electron.joint[6], jointSetPoints[5]); */

        HAL_Delay(1);

//      electron.UpdateJointAngle(electron.joint[ANY], 65 + 75 * std::sin(t));

        printf("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
               jointSetPoints[0], jointSetPoints[1], jointSetPoints[2],
               jointSetPoints[3], jointSetPoints[4], jointSetPoints[5]);
    }
}


extern "C"
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef* hspi)
{
    electron.lcd->isBusy = false;
}