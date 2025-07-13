#include "General.h"

ClassGeneral General;

Uint16 ClassGeneral::CRC16Modbus(Uint16 *Data, Uint16 Length)  
{
    Uint16 CRC = 0xFFFF;
    while (Length--)
    {
        CRC ^= *Data++;
        
        for (Uint8 i = 0; i < 8; i++) 
        {
            if (CRC & 0x0001) 
                CRC = (CRC >> 1) ^ 0xA001;
            else 
                CRC >>= 1;
        }
    }
    return CRC;
}

// 函数：计算16位数据的校验和
// 参数：Data - 16位数据指针, Length - 元素个数（不是字节数）
Uint16 ClassGeneral::CheckSum_MINI(Uint16 *Data, int Length)
{
    Uint32 Sum = 0;
    
    // 处理16位数据块
    for (int i = 0; i < Length; i++) {
        // 直接添加16位数据
        Sum += Data[i];
        
        // 处理32位溢出
        if (Sum > 0xFFFF) {
            Sum = (Sum & 0xFFFF) + (Sum >> 16);
        }
    }
    
    // 处理奇数长度情况（添加0字节填充）
    if (Length % 2 != 0) {
        // 注意：这里实际不需要额外处理，因为Length是元素个数
        // 但为安全考虑可以添加虚拟字节
        Uint8 Padding = 0;
        Sum += Padding;
        if (Sum > 0xFFFF) {
            Sum = (Sum & 0xFFFF) + (Sum >> 16);
        }
    }
    
    // 取反作为最终校验和
    return (~Sum);
}

