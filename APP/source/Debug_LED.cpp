#include "Debug_LED.h"

void InitDebugLED(void)
{
	EALLOW;
	SysCtrlRegs.PCLKCR3.bit.GPIOINENCLK = 1;// 开启GPIO时钟

    GpioCtrlRegs.GPCMUX1.bit.GPIO68 = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO68 = 1;
    GpioCtrlRegs.GPCPUD.bit.GPIO68 = 0;
    GpioDataRegs.GPCSET.bit.GPIO68 = 1;

	EDIS;
}
