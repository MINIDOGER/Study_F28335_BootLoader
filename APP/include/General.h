#ifndef GENERAL_H
#define GENERAL_H

#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

#define Jump2App()      asm(" LB 0x328000") // 地址跳转

class ClassGeneral
{
    public:
    Uint16 CRC16Modbus(Uint16 *Addr, Uint16 Num); // CRC16校验
    Uint16 CheckSum_MINI(Uint16 *Data, int Length); // 16和校验
};

extern ClassGeneral General;

#endif
