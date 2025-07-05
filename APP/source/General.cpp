#include "General.h"

ClassGeneral General;

Uint16 ClassGeneral::CRC16Modbus(Uint8 *Addr, Uint16 Num)  
{
    Uint16 CRC = 0xFFFF;
    while (Num--)
    {
        CRC ^= *Addr++;
        
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

