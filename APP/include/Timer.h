#ifndef TIMER_H
#define TIMER_H

#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

interrupt void TIM0_ISR(void);

typedef union
{
    unsigned int u16Timer;
    struct
    {
        unsigned int Timer1s        :1;
        unsigned int Timer500ms     :1;
        unsigned int reserve        :14;
    }bit;
}TimerBase;

class ClassTimer
{
    private:
    unsigned int CntBak500ms;
    unsigned int CntBak1s;

    public:
    Uint8 TimeCnt;
    unsigned int clk100ms;
    TimerBase TimeBaseFlag;

    void InitTimer(float Freq, float Period);
    void FlagTimer(void);
};

extern ClassTimer Timer;

#endif
