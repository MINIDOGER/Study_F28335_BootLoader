#include "Timer.h"

ClassTimer Timer;

/**
 * @brief 初始化定时器
 *
 * 该函数完成以下功能：
 * 1. 初始化各类时间计数器变量
 * 2. 配置CPU定时器0的寄存器
 * 3. 设置定时器中断使能和中断向量
 *
 * @param Freq 系统主频，用于计算定时器基准时钟
 * @param Period 定时周期，单位us
 */
void ClassTimer::InitTimer(float Freq, float Period)
{
    /**
     * 初始化各类时间计数器变量
     * 包含100ms时钟标志、500ms和1秒的计数备份以及通用时间计数器
     */
    clk100ms = 0;
    CntBak500ms = 0;
    CntBak1s = 0;
    TimeCnt = 0x00;

    /**
     * 配置CPU定时器0寄存器
     * 包含以下关键配置：
     * - 启用定时器时钟
     * - 设置最大周期值
     * - 初始化定时器控制寄存器
     */
    EALLOW;

    SysCtrlRegs.PCLKCR3.bit.CPUTIMER0ENCLK = 1; // CPU Timer 0

    // InitCpuTimers();//初始化三个计时器
    CpuTimer0.RegsAddr = &CpuTimer0Regs;
    CpuTimer0Regs.PRD.all  = 0xFFFFFFFF;
    CpuTimer0Regs.TPR.all  = 0;
    CpuTimer0Regs.TPRH.all = 0;
    CpuTimer0Regs.TCR.bit.TSS = 1;
    CpuTimer0Regs.TCR.bit.TRB = 1;
    CpuTimer0.InterruptCount = 0;

    ConfigCpuTimer(&CpuTimer0, Freq, Period);
    CpuTimer0Regs.TCR.bit.TSS = 0;

    /**
     * 定时器中断配置
     * 完成以下中断相关配置：
     * - 设置中断服务函数
     * - 使能全局中断
     * - 使能调试中断
     */
    IER |= M_INT1;
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1; 
    PieVectTable.TINT0 = &TIM0_ISR;
    
    EDIS;

    EINT;   //使能INTM中断
    ERTM;   //使能DBGM中断
}

/**
 * @brief 更新定时器标志位
 * 
 * 该函数用于更新基于系统时钟的定时器标志位，包括500ms和1s的定时器标志。
 * 函数会检查当前计数与备份计数的差值，当达到设定阈值时更新标志位并更新备份计数。
 * 
 * @param 无
 * @return 无
 */
void ClassTimer::FlagTimer(void)
{
    unsigned int temp;
    
    // 检查并更新500ms定时器标志
    TimeBaseFlag.bit.Timer500ms = 0;
    temp = clk100ms - CntBak500ms;
    if(temp >= 5)
    {
        TimeBaseFlag.bit.Timer500ms = 1;
        CntBak500ms += 5;
    }

    // 检查并更新1s定时器标志
    TimeBaseFlag.bit.Timer1s = 0;
    temp = clk100ms - CntBak1s;
    if(temp >= 10)
    {
        TimeBaseFlag.bit.Timer1s = 1;
        CntBak1s += 10;
    }
}

/**
 * @brief 定时器0中断服务例程
 *
 * 此函数为定时器0的中断处理程序，用于递增100毫秒时钟计数器，
 * 并向中断控制器发送中断应答信号以确认中断已被处理。
 *
 * @param 无
 * @return 无
 */
interrupt void TIM0_ISR(void)
{
    // 递增100毫秒时钟计数器
    Timer.clk100ms++;

    // 允许访问受保护的寄存器
    EALLOW;

    // 向中断控制器发送中断应答信号，确认中断已被处理
    PieCtrlRegs.PIEACK.bit.ACK1 = 1;

    // 禁止访问受保护的寄存器
    EDIS;
}
