#ifndef GENERAL_H
#define GENERAL_H

#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

#define Jump2App()      asm(" LB 0x084000")

class ClassGeneral
{
    public:
    Uint16 CRC16Modbus(Uint16 *Addr, Uint16 Num);

};

extern ClassGeneral General;

#endif
