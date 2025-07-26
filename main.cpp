#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

#include "Flash2833x_API_Config.h"
#include "Flash2833x_API_Library.h" //添加了flash操作的api和lib，还没有在cmd文件配置，参考https://blog.csdn.net/qq_17525633/article/details/128456010

#include "Debug_LED.h"
#include "SCI.h"
#include "Timer.h"
#include "FlashOperation.h"

/**
 * @brief 系统主函数
 *
 * @param 无
 * @return 无
 */
void main(void)
{
    // 系统基础初始化
    InitSysCtrl();
    InitGpio();

    // PIE控制模块初始化
    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();

	MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
	MemCopy(&Flash28_API_LoadStart, &Flash28_API_LoadEnd, &Flash28_API_RunStart);
	InitFlash();

    // 外设初始化
    InitDebugLED();
    SCI.InitSCI();
    SCI.Msg[0] = 0x11;
    SCI.SendString(SCI.Msg, 1);
    Timer.InitTimer(150, 100000);

    // BootFlash.Flash_Test();

    // 主循环
    while(1)
    {
        Timer.FlagTimer();
        // 每秒翻转LED
        if(Timer.TimeBaseFlag.bit.Timer1s == 1)
        {
            DEBUG_LED_Toggle();
            Timer.TimeCnt++;
        }

        // 串口数据上传任务
        SCI.UpDataTask();

        if(Timer.TimeCnt == 0x0A && SCI.UpData == 0)
        {

        }
    }
}
