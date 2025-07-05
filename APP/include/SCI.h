#ifndef SCI_H
#define SCI_H

#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

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
        Uint8 Data[8];   // 数据数组
        Uint16 DataCnt;  // 数据计数
    } DataBuff;

    Uint8 UpData;  // 更新标志

    // 公共成员函数
    void InitSCI(void);                     // 初始化SCI
    void SendString(Uint8* Data, Uint16 Len);  // 发送字符串
};

extern ClassSCI SCI;  // 声明外部类实例

#endif
