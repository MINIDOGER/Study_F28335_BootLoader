#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

#include "Flash2833x_API_Config.h"
#include "Flash2833x_API_Library.h" //添加了flash操作的api和lib，还没有在cmd文件配置，参考https://blog.csdn.net/qq_17525633/article/details/128456010

#include "Debug_LED.h"
#include "SCI.h"
#include "Timer.h"

void Flash_Test(void)
{
	Uint16 Status = 0;
	FLASH_ST EraseStatus;
	volatile Uint16 Buffer[2] = {0x12cd, 0x342b};
	Uint16 *Flash_ptr = (Uint16 *)0x00330000; // Pointer to a location in flash
	Uint32 Length = 0x02; // Number of 16-bit values to be programmed
	FLASH_ST ProgStatus; // Status structure
	FLASH_ST VerifyStatus;

	EALLOW;
	Flash_CPUScaleFactor = SCALE_FACTOR;
	Flash_CallbackPtr = NULL;
	EDIS;

	Status = Flash_Erase(SECTORB,&EraseStatus);
	if(Status != STATUS_SUCCESS)
	{
		SCI.Msg = 0x01;
		SCI.SendString(&SCI.Msg, 1);
		return;
	}

	Status = Flash_Program(Flash_ptr,(Uint16 *)Buffer,Length,&ProgStatus);
	if(Status != STATUS_SUCCESS)
	{
		SCI.Msg = 0x02;
		SCI.SendString(&SCI.Msg, 1);
		return;
	}

	Status = Flash_Verify(Flash_ptr,(Uint16 *)Buffer,Length,&VerifyStatus);
	if(Status != STATUS_SUCCESS)
	{
		SCI.Msg = 0x03;
		SCI.SendString(&SCI.Msg, 1);
		return;
	}

	SCI.Msg = (Uint8)Status;
	SCI.SendString(&SCI.Msg, 1);
}

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

	Flash_Test();

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
