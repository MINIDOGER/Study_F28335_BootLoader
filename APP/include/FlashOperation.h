#ifndef FLASHOPERATION_H
#define FLASHOPERATION_H

#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

#include <string.h>
#include "Flash2833x_API_Config.h"
#include "Flash2833x_API_Library.h"

#include "SCI.h"

class ClassFlash
{
public:
    void Flash_Test(void);
    Uint16 MyFlashErase(void);
    Uint16 MyFlashCode(Uint16* Code);
};

extern ClassFlash BootFlash;

#endif
