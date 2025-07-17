#include "General.h"

ClassGeneral General;

/**
 * @brief 计算Modbus协议使用的16位循环冗余校验码(CRC16)
 * 
 * @param Data   指向待校验数据的指针
 * @param Length 数据长度(以16位字为单位)
 * 
 * @return 计算得到的CRC16校验码
 */
Uint16 ClassGeneral::CRC16Modbus(Uint16 *Data, Uint16 Length)  
{
    Uint16 CRC = 0xFFFF;  // 初始化CRC寄存器为全1
    
    while (Length--)
    {
        CRC ^= *Data++;  // 用当前数据异或CRC的低字节
        
        // 对当前CRC值执行8次位运算
        for (Uint8 i = 0; i < 8; i++) 
        {
            // 如果最低位为1，则右移1位并异或多项式0xA001
            if (CRC & 0x0001) 
                CRC = (CRC >> 1) ^ 0xA001;
            // 如果最低位为0，则仅将CRC右移1位
            else 
                CRC >>= 1;
        }
    }
    
    return CRC;  // 返回最终的CRC校验码
}

/**
 * @brief 计算16位数据的校验和（Checksum）
 * 
 * 此函数实现了一个标准的16位互联网校验和算法，用于验证数据完整性
 * 
 * @param Data   指向Uint16类型数据数组的指针，表示需要计算校验和的数据块
 * @param Length 数据数组的长度（以Uint16为单位）
 * 
 * @return 返回16位校验和值，为计算结果的按位取反值
 * 
 * 算法特点：
 * 1. 支持处理任意长度的16位数据块
 * 2. 自动处理32位计算溢出，确保结果在16位范围内
 * 3. 对奇数字节数情况添加虚拟字节保持计算一致性
 */
Uint16 ClassGeneral::CheckSum_MINI(Uint16 *Data, int Length)
{
    Uint32 Sum = 0;
    /**
     * 处理16位数据块的核心循环
     * 逐个累加16位数据到32位累加器中
     */
    for (int i = 0; i < Length; i++) {
        // 直接添加16位数据
        Sum += Data[i];
        
        // 处理32位溢出：将高位16位与低位16位相加
        if (Sum > 0xFFFF) {
            Sum = (Sum & 0xFFFF) + (Sum >> 16);
        }
    }
    
    /**
     * 奇数字节长度的处理
     * 如果数据长度为奇数Uint16单位，添加一个虚拟的0字节
     * 这是为了确保校验和计算的一致性
     */
    if (Length % 2 != 0) {
        /**
         * 注意：这里实际不需要额外处理，因为Length是元素个数
         * 但为安全考虑可以添加虚拟字节
         */
        Uint8 Padding = 0;
        Sum += Padding;
        if (Sum > 0xFFFF) {
            Sum = (Sum & 0xFFFF) + (Sum >> 16);
        }
    }
    
    // 根据校验和规范，返回计算结果的按位取反
    return (~Sum);
}

