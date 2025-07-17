#ifndef TIMER_H
#define TIMER_H

#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

/**
 * 定时器中断服务函数
 * 用于处理定时器0的中断事件
 */
interrupt void TIM0_ISR(void);

/**
 * 定时器标志位联合体定义
 * 用于表示不同时间基准的定时器状态标志
 */
typedef union
{
    unsigned int u16Timer;
    struct
    {
        unsigned int Timer1s        :1; // 1秒定时标志位
        unsigned int Timer500ms     :1; // 500毫秒定时标志位
        unsigned int reserve        :14; // 保留位
    }bit;
}TimerBase;

/**
 * 定时器管理类
 * 提供定时器初始化、标志位更新等功能
 */
class ClassTimer
{
    private:
    unsigned int CntBak500ms; // 500ms定时器计数备份值
    unsigned int CntBak1s; // 1s定时器计数备份值

    public:
    Uint8 TimeCnt; // 时间计数器
    unsigned int clk100ms; // 100ms时钟计数器
    TimerBase TimeBaseFlag; // 时间基准标志位联合体

    void InitTimer(float Freq, float Period); // 定时器初始化函数
    void FlagTimer(void); // 定时标志更新函数
};

extern ClassTimer Timer; // 全局定时器实例

#endif
