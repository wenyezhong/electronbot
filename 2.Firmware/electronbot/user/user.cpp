#include <cmath>
#include "common_inc.h"
#include "screen.h"
#include "robot.h"
#include <stdio.h>

Robot electron(&hspi1, &hi2c1);
float jointSetPoints[7];
bool isEnabled = false;

float setKp;
float setKi;
float setKv;
float setKd;
float setTq;

uint8_t setkp_flag;
uint8_t setki_flag;
uint8_t setkv_flag;
uint8_t setkd_flag;
uint8_t settq_flag;

uint8_t  newid;
// float initAngle;

/* Thread Definitions -----------------------------------------------------*/


/* Timer Callbacks -------------------------------------------------------*/

void setPara(uint8_t *ptr)
{
    // Robot::JointStatus_t *pJointStatus; 
    uint8_t id = ptr[0];
    setKp=*((float*) (ptr + 2));
    setKi=*((float*) (ptr + 7));
    setKv=*((float*) (ptr + 12));
    setKd=*((float*) (ptr + 17));
    setTq=*((float*) (ptr + 22));

    setkp_flag = ptr[1];
    setki_flag = ptr[6];
    setkv_flag = ptr[11];
    setkd_flag = ptr[16];
    settq_flag = ptr[21];

    for(int i=0 ; i<32; i++)
        printf("%.2x ",ptr[i]);
    printf("\r\n");
  /*   switch(id)
    {
        case 2:pJointStatus=&electron.joint[1];break;
        case 4:pJointStatus=&electron.joint[2];break;
        case 6:pJointStatus=&electron.joint[3];break;
        case 8:pJointStatus=&electron.joint[4];break;
        case 10:pJointStatus=&electron.joint[5];break;
        case 12:pJointStatus=&electron.joint[6];break;
        default:pJointStatus=&electron.joint[0];break; 

    } */

    if(setkp_flag)
        electron.SetJointKp(electron.joint[id/2], setKp);
    if(setki_flag)
        electron.SetJointKi(electron.joint[id/2], setKi);
    if(setkv_flag)        
        electron.SetJointKv(electron.joint[id/2], setKv);
    if(setkd_flag)
        electron.SetJointKd(electron.joint[id/2], setKd);
    if(settq_flag)
        electron.SetJointTorqueLimit(electron.joint[id/2], setTq);
   /*  for(int i=0 ; i<32; i++)
        printf("%.2x ",ptr[i]);
    printf("\r\n"); */
    /*float t=*((float*) (ptr + 2));    
    int dat=ceil(t);
    // printf("jointSetPoints=%u\r\n",jointSetPoints[0]);
    printf("jointSetPoints=%ld\r\n",dat);    */
   
}
void setAngle(uint8_t *ptr)
{
    uint8_t id = ptr[0];
   
    jointSetPoints[id/2] = *((float*) (ptr + 1));
    isEnabled = ptr[5];
    printf("id = %d\r\n",id);
    for(int i=0 ; i<32; i++)
        printf("%.2x ",ptr[i]);
    printf("\r\n");
    /* switch(id)
    {
        case 2:{
            jointSetPoints[1]=*((float*) (ptr + 1));
            electron.UpdateJointAngle(electron.joint[1], jointSetPoints[1]);
            electron.SetJointEnable(electron.joint[1], isEnabled);
        }break;
        case 4:{
            jointSetPoints[2]=*((float*) (ptr + 1));
            electron.UpdateJointAngle(electron.joint[2], jointSetPoints[2]);
            electron.SetJointEnable(electron.joint[2], isEnabled);
        }break;
        case 6:{
            jointSetPoints[3]=*((float*) (ptr + 1));
            electron.UpdateJointAngle(electron.joint[3], jointSetPoints[3]);
             electron.SetJointEnable(electron.joint[3], isEnabled);
        }break;
        case 8:{
            jointSetPoints[4]=*((float*) (ptr + 1));
            electron.UpdateJointAngle(electron.joint[4], jointSetPoints[4]);
            electron.SetJointEnable(electron.joint[1], isEnabled);
        }break;
        case 10:{
            jointSetPoints[5]=*((float*) (ptr + 1));
            electron.UpdateJointAngle(electron.joint[5], jointSetPoints[5]);
            electron.SetJointEnable(electron.joint[5], isEnabled);
        }break;
        case 12:{
            jointSetPoints[6]=*((float*) (ptr + 1));
            electron.UpdateJointAngle(electron.joint[6], jointSetPoints[6]);
            electron.SetJointEnable(electron.joint[6], isEnabled);
        }break;
        default:{
            jointSetPoints[0]=*((float*) (ptr + 1));
            electron.UpdateJointAngle(electron.joint[0], jointSetPoints[0]);
            electron.SetJointEnable(electron.joint[0], isEnabled);
        }break;
    } */
    // printf("id = %.2x\r\n",JointStatus.id);
    // printf("id = %d\r\n",id);  
    electron.UpdateJointAngle(electron.joint[id/2], jointSetPoints[id/2]);   
    electron.SetJointEnable(electron.joint[id/2], isEnabled);
}
void setid(uint8_t *ptr)
{
    uint8_t id = ptr[0];   
    newid=ptr[1];
    electron.SetJointId(electron.joint[id/2], newid);
}
void setInitAngle(uint8_t *ptr)
{
    uint8_t id = ptr[0];
    jointSetPoints[id/2]=*((float*) (ptr + 1));
    for(int i=0 ; i<32; i++)
        printf("%.2x ",ptr[i]);
    printf("\r\n");
    
    electron.SetJointInitAngle(electron.joint[id/2], jointSetPoints[id/2]);
}
void readAngle(uint8_t *ptr)
{
    uint8_t id = ptr[0];
    //jointSetPoints[id/2]=*((float*) (ptr + 1));
    for(int i=0 ; i<32; i++)
        printf("%.2x ",ptr[i]);
    printf("\r\n");
    
    electron.UpdateJointAngle(electron.joint[id/2]);
}
void JumpBootLoader(uint8_t *ptr)
{
    electron.pUsbBuffer->extraDataTx[31] = 0xef;
    electron.pUsbBuffer->extraDataTx[30] = 0x01;
    electron.pUsbBuffer->extraDataTx[29] = 0x00;
    electron.SendUsbPacket(electron.pUsbBuffer->extraDataTx, 32);
}
/* Default Entry -------------------------------------------------------*/
void Main(void)
{
    int i;
    int p=0;
    HAL_Delay(1000);
    electron.lcd->Init(Screen::DEGREE_0);
    electron.lcd->SetWindow(0, 239, 0, 239);
    for(int i=0;i<32;i++)
        electron.pUsbBuffer->extraDataTx[i]=32-i;
    /* electron.SetJointEnable(electron.joint[6], true);
    electron.UpdateJointAngle(electron.joint[6], 30); */

    /* electron.UpdateJointAngle(electron.joint[1], 0);
    electron.UpdateJointAngle(electron.joint[2], 0);
    electron.UpdateJointAngle(electron.joint[3], 0);
    electron.UpdateJointAngle(electron.joint[4], 0);
    electron.UpdateJointAngle(electron.joint[5], 0);  */
    //electron.UpdateJointAngle(electron.joint[6], 0); 
    while(1)
    {
        
        electron.ReceiveUsbPacketUntilSizeIs(224); // last packet is 224bytes
        uint8_t* ptr = electron.GetExtraDataRxPtr();
        
        if(ptr[31] == 0xee)  //设置参数
        {
            setPara(ptr);
        }
        else if(ptr[31] == 0xef)
        {
            setAngle(ptr);
            
        }
        else if(ptr[31] == 0xf0)
        {            
             setid(ptr);         
        }
        else if(ptr[31] == 0xf1)
        {
             setInitAngle(ptr);
        }
        else if(ptr[31] == 0xf2)
        {
             readAngle(ptr);
        }
        else if((ptr[31] == 0xfe)&&(ptr[30] == 0x01))
        {
             JumpBootLoader(ptr);
        }
        else
        {
            printf("task  p=%d : %d\r\n",p,ptr[31]);
            //for(p=0;p<4;p++)
           
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
            //printf("33333\r\n");
            while (electron.lcd->isBusy);
            // memset(electron.GetLcdBufferPtr(),200,60*240*3);
            //printf("222222\r\n");
            /* for(i=0 ; i<100; i++)
                printf("%.2x ",electron.GetLcdBufferPtr()[i]);
            printf("\r\n");  */
            if (p == 0)
                electron.lcd->WriteFrameBuffer(electron.GetLcdBufferPtr(),60 * 240 * 3,false);
            else
                electron.lcd->WriteFrameBuffer(electron.GetLcdBufferPtr(),60 * 240 * 3, true);
            if(p++>=3)
                p=0;

            
        }
        for (int j = 0; j < 6; j++)
        {
            for (int i = 0; i < 4; i++)
            {
                auto* b = (unsigned char*) &(electron.joint[j + 1].angle);
                electron.pUsbBuffer->extraDataTx[j * 4 + i + 1] = *(b + i);
            }
        }            
        electron.SendUsbPacket(electron.pUsbBuffer->extraDataTx, 32);
        HAL_Delay(1);
        
    }
    
   
 

    float t = 0;    
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
            
            electron.SendUsbPacket(electron.pUsbBuffer->extraDataTx, 32);           
            electron.ReceiveUsbPacketUntilSizeIs(224); // last packet is 224bytes           
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
                electron.lcd->WriteFrameBuffer(electron.GetLcdBufferPtr(),60 * 240 * 3,false);
            else
                electron.lcd->WriteFrameBuffer(electron.GetLcdBufferPtr(),60 * 240 * 3, true);			
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