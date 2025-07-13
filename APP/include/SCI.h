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
max
|0      |1      |2      |3-4    |5      |6-261  |262    |
|0-1    |2-3    |4-5    |6-9    |10-11  |12-523 |524-525|
|0x01   |0xCDfun|package|addr   |len    |data   |add    |
8:526;
16:263;
************************************************************************/

#define ErrorDevice         0xe1
#define ErrorFun            0xef
#define ErrorAddr           0xea
#define ErrorPack           0xe0
#define ErrorCheck          0xec
#define ErrorEnd            0xee
#define ReceptOK            0xcd

#define AddrMin             0x00008400
#define AddrMax             0x00008700

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
        Uint16 Data[263];   // 数据数组
        Uint16 DataCnt;  // 数据计数
        Uint8 IsLowByte;
        Uint16 TargeCnt;
        Uint16 PackageTarge;
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
