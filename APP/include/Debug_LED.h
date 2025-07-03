#ifndef DEBUG_LED_H
#define DEBUG_LED_H

#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

#define DEBUG_LED_ON()          (GpioDataRegs.GPCSET.bit.GPIO68 = 1)
#define DEBUG_LED_OFF()         (GpioDataRegs.GPCCLEAR.bit.GPIO68 = 1)
#define DEBUG_LED_Toggle()      (GpioDataRegs.GPCTOGGLE.bit.GPIO68 = 1)

void InitDebugLED(void);

#endif
