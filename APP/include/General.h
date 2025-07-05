#ifndef GENERAL_H
#define GENERAL_H

#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

typedef struct
{
	Uint16 Poly;
	Uint16 InitValue;
	Uint16 Xor;
	Uint8 InputReverse;
	Uint8 OutputReverse;
}TyCRC16;

class ClassGeneral
{
    public:
    TyCRC16 CRCData;

    Uint16 CRC16Modbus(Uint8 *Addr, Uint16 Num);

};

extern ClassGeneral General;

#endif
