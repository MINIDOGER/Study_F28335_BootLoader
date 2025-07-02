#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"
// #include "Flash2833x_API_Library.h" //添加了flash操作的api和lib，还没有在cmd文件配置，参考https://blog.csdn.net/qq_17525633/article/details/128456010

#include "led.h"

void delay(void)
{
    Uint16 i;
	Uint32 j;
	for(i=0;i<32;i++)
		for (j = 0; j < 100000; j++);
}

void main(void)
{
	InitSysCtrl();
	// InitFlash();
	LED_Init();

	while (1)
	{
		LED1_TOGGLE;
		delay();
	}
}
