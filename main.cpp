#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"
// #include "Flash2833x_API_Library.h" //添加了flash操作的api和lib，还没有在cmd文件配置，参考https://blog.csdn.net/qq_17525633/article/details/128456010

#include "Debug_LED.h"
#include "SCI.h"
#include "Timer.h"

Uint8 TimeCnt = 0x00;
Uint8 Msg = 0x00;
Uint8 NumFFRX = 0x00;
Uint8 CodeData[10];

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
	Msg = 0x11;
	SCI.SendString(&Msg, 1);
	Timer.InitTimer(150, 100000);

	while(1)
	{
		Timer.FlagTimer();
		if(Timer.TimeBaseFlag.bit.Timer1s == 1)
		{
			DEBUG_LED_Toggle();
			TimeCnt++;
		}

		if(SCI.UpData == 1)
		{
			Msg = 0xCD;
			SCI.SendString(&Msg, 1);
			while(TimeCnt < 0x0A)
			{
				if(SciaRegs.SCIFFRX.bit.RXFFST > 0)
				{
					NumFFRX = ScicRegs.SCIFFRX.bit.RXFFST;
					for(Uint8 i = 0; i < NumFFRX; i++)
					{
						CodeData[i] = ScicRegs.SCIRXBUF.bit.RXDT;
					}
				}
			}

			// SciaRegs.SCIFFRX.bit.RXFFIENA = 1;
			SCI.UpData = 0;
		}

		if(TimeCnt == 0x0A && SCI.UpData == 0)
		{

		}
	}
}
