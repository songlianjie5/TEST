
/********************************** (C) COPYRIGHT *******************************
* File Name          :CompatibilityHID.C
* Author             : WCH
* Version            : V1.2
* Date               : 2018/02/28
* Description        : CH554模拟HID兼容设备，支持中断上下传，支持控制端点上下传，支持设置全速，低速
*******************************************************************************/

#include "CH554.H"
#include "Debug.H"
#include "DataFlash.H"
#include "GPIO.H"
#include "USBbase.H"
#include <stdio.h>
#include <string.h>

sbit LED = P1^4;

enum {
    ERROR=0,
    READ_SENSOR=1,
    PROCESS_HOST_CMD=2,
    SEND_DATA_TO_HOST=3,
} mcuState_e = PROCESS_HOST_CMD;
enum {
    NULL_CMD=0,
    GET_DATA_CMD=1,
    READ_EEPROM_CMD=2,
    WRITE_EEPROM_CMD=3,
    RUN_ISP_CMD=4,
} hostCmd_e=NULL_CMD;
int main()
{
    UINT8 i,m;

    CfgFsys( );                                                           //CH559时钟选择配置
    mDelaymS(5);                                                          //修改主频等待内部晶振稳定,必加
    mInitSTDIO( );                                                        //串口0初始化

    for(i=0; i<64; i++)                                                   //准备演示数据
    {
        UserEp2Buf[i] = 0;
    }
    USBDeviceInit();                                                      //USB设备模式初始化
    EA = 1;                                                               //允许单片机中断
    UEP1_T_LEN = 0;                                                       //预使用发送长度一定要清空
    UEP2_T_LEN = 0;                                                       //预使用发送长度一定要清空
    FLAG = 0;
    Ready = 0;

    //---------------------------------------------------------------------
    Port1Cfg(1,4); 														//LED
    while(1)
    {
        switch (mcuState_e)
        {
        case READ_SENSOR:
            LED = !LED;
            UserEp2Buf[0]++;
            mcuState_e = SEND_DATA_TO_HOST;
            break;
        case PROCESS_HOST_CMD:
            if(Ready && Endp2OutFlag)
            {
                Endp2OutFlag = 0;

                //if(Ep2Buffer[0]==0x01)
                //	hostCmd_e = GET_DATA_CMD;
                hostCmd_e = Ep2Buffer[0];

                switch (hostCmd_e)
                {
                case GET_DATA_CMD:
                    mcuState_e = READ_SENSOR;
                    break;
                case READ_EEPROM_CMD:
                    if(ReadDataFlash(1,10,&m) != 1)
                    {
                        ;//Error
                    }
                    mcuState_e = SEND_DATA_TO_HOST;
                    break;
                case WRITE_EEPROM_CMD:
                    if(WriteDataFlash(1,UserEp2Buf,10)!=1)
                    {
                        ;//Error
                    }
                    mcuState_e = PROCESS_HOST_CMD;
                    break;
                case RUN_ISP_CMD:
                    break;
                default:
                    break;
                }
                hostCmd_e = NULL_CMD;
            }
            break;
        case SEND_DATA_TO_HOST:
            //if(Ready && (Ep2InKey==0))
            if(Ready)
            {
                while( Endp2Busy );                                            //如果忙（上一包数据没有传上去），则等待。
                Endp2Busy = 1;                                                 //设置为忙状态
                Enp2BlukIn( );
                mcuState_e = PROCESS_HOST_CMD;
            }
            break;
        default:
            mcuState_e = PROCESS_HOST_CMD;
            break;
        }
    }
}


