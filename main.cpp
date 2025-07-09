#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

#include "Flash2833x_API_Config.h"
#include "Flash2833x_API_Library.h" //添加了flash操作的api和lib，还没有在cmd文件配置，参考https://blog.csdn.net/qq_17525633/article/details/128456010

#include "Debug_LED.h"
#include "SCI.h"
#include "Timer.h"

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
	SCI.Msg = 0x11;
	SCI.SendString(&SCI.Msg, 1);
	Timer.InitTimer(150, 100000);

	while(1)
	{
		Timer.FlagTimer();
		if(Timer.TimeBaseFlag.bit.Timer1s == 1)
		{
			DEBUG_LED_Toggle();
			Timer.TimeCnt++;
		}

		SCI.UpDataTask();

		if(Timer.TimeCnt == 0x0A && SCI.UpData == 0)
		{

		}
	}
}
