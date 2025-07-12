#ifndef SCI_H
#define SCI_H

#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

#include <string.h>
#include "Flash2833x_API_Config.h"
#include "Flash2833x_API_Library.h"

#include "Debug_LED.h"
#include "General.h"
#include "Timer.h"

/************************************************************************
|0      |1-2    |3-4    |5-7    |8-9    |9-521  |522    |523    |
|0x01   |0xCDfun|package|addr   |len    |data   |add    |end    |
8:522;(1+2+2+3+2+512+2=524)
16:262;
************************************************************************/

#define ErrorDevice         0xe1
#define ErrorFun            0xef
#define ErrorAddr           0xea
#define ErrorPack           0xe0
#define ErrorCheck          0xec
#define ReceptOK            0xcd

// 声明串口接收中断服务函数
interrupt void SCIARX_ISR(void);

class ClassSCI
{
private:
    // 私有成员函数：发送单个字节
    Uint8 SendByte(Uint8 Byte);

public:
    // 数据缓冲区结构体
    struct
    {
        Uint16 Data[262];   // 数据数组
        Uint16 DataCnt;  // 数据计数
        Uint8 IsLowByte;
        Uint16 TargeCnt;
        Uint16 PackageCnt;
    } DataBuff;

    Uint8 UpData;  // 更新标志
    Uint8 Msg;
    Uint8 NumFFRX;

    // 公共成员函数
    void InitSCI(void);                     // 初始化SCI
    void InitValue(void);
    void SendString(Uint8* Data, Uint16 Len);  // 发送字符串
    void UpDataTask();
};

extern ClassSCI SCI;  // 声明外部类实例

#endif
