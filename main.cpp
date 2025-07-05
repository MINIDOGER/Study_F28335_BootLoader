#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"
// #include "Flash2833x_API_Library.h" //添加了flash操作的api和lib，还没有在cmd文件配置，参考https://blog.csdn.net/qq_17525633/article/details/128456010

#include "Debug_LED.h"
#include "SCI.h"
#include "Timer.h"

Uint8 msg = 0x00;

void main(void)
{
	InitSysCtrl();
	InitGpio();

	InitPieCtrl();
	IER = 0x0000;
	IFR = 0x0000;
	InitPieVectTable();

	// InitFlash();

	InitDebugLED();
	SCI.InitSCI();
	Timer.InitTimer(150, 100000);

	while(1)
	{
		Timer.FlagTimer();
		if(Timer.TimeBaseFlag.bit.Timer1s == 1)
		{
			DEBUG_LED_Toggle();
		}

		if(SCI.UpData == 1)
		{
			msg = 0xCD;
			SCI.SendString(&msg, 1);

			// SciaRegs.SCIFFRX.bit.RXFFIENA = 1;
			// SCI.UpData = 1;
		}
	}
}
