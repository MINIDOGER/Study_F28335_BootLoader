#include "Timer.h"

ClassTimer Timer;

void ClassTimer::InitTimer(float Freq, float Period)
{
    clk100ms = 0;
    CntBak500ms = 0;
    CntBak1s = 0;

    EALLOW;
    SysCtrlRegs.PCLKCR3.bit.CPUTIMER0ENCLK = 1; // CPU Timer 0

    InitCpuTimers();//初始化三个计时器
    // CpuTimer0.RegsAddr = &CpuTimer0Regs;
    // CpuTimer0Regs.PRD.all  = 0xFFFFFFFF;
    // CpuTimer0Regs.TPR.all  = 0;
    // CpuTimer0Regs.TPRH.all = 0;
    // CpuTimer0Regs.TCR.bit.TSS = 1;
    // CpuTimer0Regs.TCR.bit.TRB = 1;
    // CpuTimer0.InterruptCount = 0;

    ConfigCpuTimer(&CpuTimer0, Freq, Period);
    CpuTimer0Regs.TCR.bit.TSS = 0;

    IER |= M_INT1;
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1; 
    PieVectTable.TINT0 = &TIM0_IRQn;
    
    EDIS;

    EINT;   //使能INTM中断
    ERTM;   //使能DBGM中断
}

void ClassTimer::FlagTimer(void)
{
    unsigned int temp;
    TimeBaseFlag.bit.Timer500ms = 0;
    temp = clk100ms - CntBak500ms;
    if(temp >= 5)
    {
        TimeBaseFlag.bit.Timer500ms = 1;
        CntBak500ms += 5;
    }

    TimeBaseFlag.bit.Timer1s = 0;
    temp = clk100ms - CntBak1s;
    if(temp >= 10)
    {
        TimeBaseFlag.bit.Timer1s = 1;
        CntBak1s += 10;
    }
}

interrupt void TIM0_IRQn(void)
{
    Timer.clk100ms++;

    EALLOW;
    PieCtrlRegs.PIEACK.bit.ACK1 = 1;
    EDIS;
}
