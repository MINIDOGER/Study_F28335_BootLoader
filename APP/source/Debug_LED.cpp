#include "Debug_LED.h"

/**
 * @brief 初始化调试LED的GPIO配置
 * 
 * 该函数配置了GPIO68引脚，用于驱动调试LED。具体配置包括：
 * - 开启GPIO模块的时钟
 * - 设置GPIO68为通用IO模式
 * - 配置GPIO68为输出方向
 * - 禁止GPIO68的上拉/下拉电阻
 * - 初始状态将GPIO68置高（LED熄灭，取决于硬件设计）
 *
 * @param 无
 * @return 无
 */
void InitDebugLED(void)
{
    EALLOW;
    // 开启GPIO时钟
    SysCtrlRegs.PCLKCR3.bit.GPIOINENCLK = 1;

    /**
     * - 设置GPIO68为通用IO模式
     * - 配置GPIO68为输出方向
     * - 禁止GPIO68的上拉/下拉电阻
     * - 初始状态将GPIO68置高（LED熄灭，取决于硬件设计）
     */
    GpioCtrlRegs.GPCMUX1.bit.GPIO68 = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO68 = 1;
    GpioCtrlRegs.GPCPUD.bit.GPIO68 = 0;
    GpioDataRegs.GPCSET.bit.GPIO68 = 1;

    EDIS;
}
