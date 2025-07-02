#ifndef LED_H
#define LED_H

#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

#define LED1_TOGGLE     (GpioDataRegs.GPCTOGGLE.bit.GPIO68=1)

void LED_Init(void);

#endif
