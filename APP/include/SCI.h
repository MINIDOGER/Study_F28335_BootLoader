#ifndef SCI_H
#define SCI_H

#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

#include <string.h>
// #include "Flash2833x_API_Config.h"
// #include "Flash2833x_API_Library.h"

#include "Debug_LED.h"
#include "General.h"
#include "Timer.h"
#include "FlashOperation.h"

/************************************************************************
max
|0      |1      |2      |3-4    |5      |6-261  |262    |
|0-1    |2-3    |4-5    |6-9    |10-11  |12-523 |524-525|
|0x01   |0xCDfun|package|addr   |len    |data   |add    |
8:526;
16:263;
************************************************************************/

/**
 * @brief 错误码定义
 */
#define ErrorDevice         0xe1    // 设备错误
#define ErrorFun            0xef    // 功能错误
#define ErrorFlash          0xfa    // flash错误
#define ErrorCode           0xff    // flash错误
#define ErrorAddr           0xea    // 地址错误
#define ErrorPack           0xe0    // 数据包错误
#define ErrorCheck          0xec    // 校验错误
#define ErrorEnd            0xee    // 错误结束
#define ReceptOK            0xcd    // 接收成功

/**
 * @brief 地址范围定义
 */
#define AddrMin             0x00310000  // 最小有效地址SectorFStart
#define AddrMax             0x0032FFFF  // 最大有效地址SectorCEnd

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
        Uint16 Data[263]; // 数据数组
        Uint16 DataCnt; // 数据计数
        Uint8 IsLowByte;
        Uint16 TargeCnt;
        Uint16 PackageTarge;
        Uint16 PackageCnt;
    } DataBuff;

    Uint8 UpData;  // 更新标志
    Uint8 Msg[8];
    Uint16 MsgLen;
    Uint8 NumFFRX;

    // 公共成员函数
    void InitSCI(void);                     // 初始化SCI
    void InitValue(void);
    void SendString(Uint8* Data, Uint16 Len);  // 发送字符串
    void UpDataTask();
};

extern ClassSCI SCI;  // 声明外部类实例

#endif
