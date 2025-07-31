#include "SCI.h"

ClassSCI SCI;

/**
 * @brief 初始化SCI（串行通信接口）模块
 * @param 无
 * @return 无
 */
void ClassSCI::InitSCI(void)
{
    // 调用初始化值设置函数
    InitValue();

    // 计算并设置SCI的波特率参数
    Uint16 DataTemp;
    DataTemp = 37500000/(8*115200)-1;
    
    // 初始化SCI的GPIO引脚
    InitSciGpio();

    // 允许访问受保护的寄存器
    EALLOW;

    // 使能SCI模块的时钟
    SysCtrlRegs.PCLKCR0.bit.SCIAENCLK = 1;

    // 配置发送FIFO
    SciaRegs.SCIFFTX.all = 0xE04F;

    // 配置接收FIFO并重置接收FIFO
    SciaRegs.SCIFFRX.all = 0x0061;
    SciaRegs.SCIFFRX.bit.RXFIFORESET = 1;

    // 清除FIFO控制寄存器
    SciaRegs.SCIFFCT.all = 0x0;

    // 配置SCI通信格式（8位数据，无校验，1位停止位）
    SciaRegs.SCICCR.all = 0x0007;

    // 配置SCI控制寄存器1（使能接收和发送）
    SciaRegs.SCICTL1.all = 0x0003;

    // 配置SCI控制寄存器2（使能发送和接收中断）
    SciaRegs.SCICTL2.all = 0x0003;
    SciaRegs.SCICTL2.bit.TXINTENA = 1;
    SciaRegs.SCICTL2.bit.RXBKINTENA = 1;

    // 设置SCI的波特率寄存器（高8位和低8位）
    SciaRegs.SCIHBAUD = DataTemp >> 8;
    SciaRegs.SCILBAUD = DataTemp & 0xFF;

    // 重新启用SCI模块的发送和接收功能
    SciaRegs.SCICTL1.all = 0x0023;

    // 使能全局中断9（对应SCI中断）
    IER |= M_INT9;

    // 使能SCI接收中断
    PieCtrlRegs.PIEIER9.bit.INTx1=1;

    // 注册SCI接收中断服务函数
    PieVectTable.SCIRXINTA = &SCIARX_ISR;

    // 禁止访问受保护的寄存器
    EDIS;
}

/**
 * @brief 初始化SCI模块的数据缓冲区和相关状态变量
 * @param 无
 * @return 无
 */
void ClassSCI::InitValue(void)
{
    // 初始化数据缓冲区内容为0
    memset(DataBuff.Data, 0, sizeof(DataBuff.Data));
    // 重置数据计数器
    DataBuff.DataCnt = 0;
    // 重置字节顺序标志
    DataBuff.IsLowByte = 0;
    // 设置目标数据长度（0x0107表示263字节）
    DataBuff.TargeCnt = 0x0107;
    // 清零包目标计数
    DataBuff.PackageTarge = 0;
    // 重置包计数器
    DataBuff.PackageCnt = 1;
    // 清零更新标志
    UpData = 0;
    // 清零消息标志
    memset(Msg, 0, sizeof(Msg));
    MsgLen = 0;
    // 清零特殊字符计数器
    NumFFRX = 0;
}

/**
 * @brief 发送一个字节数据到SCI（串行通信接口）发送缓冲区
 * 
 * 该函数会先检查发送FIFO缓冲区状态，如果当前缓冲区中的数据量
 * 小于16个字节，则将指定的数据写入发送缓冲区；否则，放弃发送
 * 
 * @param Byte 待发送的8位数据字节
 * 
 * @return Uint8 返回发送状态
 *         - 1 表示成功将数据写入发送缓冲区
 *         - 0 表示发送缓冲区已满，数据未被写入
 */
Uint8 ClassSCI::SendByte(Uint8 Byte)
{
    // 检查发送FIFO缓冲区状态
    if(ScibRegs.SCIFFTX.bit.TXFFST < 16)
    {
        // 将数据写入SCI发送缓冲区
        SciaRegs.SCITXBUF = Byte;
    }
    else
    {
        return 0;
    }
    return 1;
}

/**
 * @brief 向串口发送指定长度的字节数据
 * 
 * 该函数会循环发送数据缓冲区中的每个字节，直到：
 * 1. 所有字节都被成功发送
 * 2. 发送失败时立即停止发送
 * 
 * @param Data  待发送的数据缓冲区指针
 * @param Len   要发送的字节数
 */
void ClassSCI::SendString(Uint8* Data, Uint16 Len)
{
    for (Uint16 i = 0; i < Len; i++)
    {
        /**
         * 尝试发送单个字节
         * 如果发送失败则立即终止发送过程
         */
        if(!SendByte(Data[i]))
        {
            break;
        }
    }
}

/**
 * @brief 处理SCI通信的上行数据任务
 * 
 * 该函数主要负责处理串口通信中的数据接收、校验和响应生成。
 * 它会等待数据到达，解析数据包，并根据不同的命令执行相应操作。
 * 
 * @param 无
 * @return 无
 */
void ClassSCI::UpDataTask()
{
    if(UpData == 1)
    {
        Uint16 DataTemp = 0;
        Uint32 AddrTemp = 0;
        Msg[0] = 0xCD;
        SendString(Msg, 1);
        Timer.TimeCnt = 0;
        while(UpData/*Timer.TimeCnt < 0x14*/)
        {
            Timer.FlagTimer();
            if(Timer.TimeBaseFlag.bit.Timer500ms == 1)
            {
                DEBUG_LED_Toggle();
                Timer.TimeCnt++;
            }
            if(SciaRegs.SCIFFRX.bit.RXFFST > 0)
            {
                /**
                 * 清除定时器计数器，读取接收缓冲区中的数据量，
                 * 然后循环读取每个数据字节并进行处理。
                 * 数据处理包括低字节和高字节的组合。
                 */
                Timer.TimeCnt = 0;
                NumFFRX = SciaRegs.SCIFFRX.bit.RXFFST;
                for(Uint8 i = 0; i < NumFFRX; i++)
                {
                    if(DataBuff.IsLowByte)
                    {
                        DataBuff.Data[DataBuff.DataCnt] = SciaRegs.SCIRXBUF.bit.RXDT | DataTemp << 8;
                        DataBuff.IsLowByte = 0;
                        DataBuff.DataCnt++;
                    }
                    else
                    {
                        DataTemp = SciaRegs.SCIRXBUF.bit.RXDT;
                        DataBuff.IsLowByte = 1;
                    }
                }
                NumFFRX = 0;

                if(DataBuff.DataCnt == 6 && DataBuff.IsLowByte == 0)
                {
                    // 总元素数 = 包头(6) + 数据部分((len+1)/2) + 校验和(1)
                    DataBuff.TargeCnt = 7 + (DataBuff.Data[5] + 1) / 2;
                }
            }

            if(DataBuff.DataCnt >= DataBuff.TargeCnt)
            {
                Msg[0] = ReceptOK;
                MsgLen = 1;
                /**
                    * 检查数据包的第一个字节是否为预期设备标识符，
                    * 第二个字节是否为有效功能码。
                    */
                if(DataBuff.Data[0] != 0x01)
                {
                    Msg[0] = ErrorDevice;
                }
                else if(DataBuff.Data[1] != 0xCDFF && DataBuff.Data[1] != 0xCDDA && DataBuff.Data[1] != 0xCDF0)
                {
                    Msg[0] = ErrorFun;
                }
                else if(General.CheckSum_MINI(&DataBuff.Data[0], DataBuff.TargeCnt - 1) != DataBuff.Data[DataBuff.TargeCnt - 1])
                {
                    Msg[0] = ErrorCheck;
                }
                else
                {
                    /**
                    * 根据接收到的不同命令码执行不同的操作：
                    * - 0xCDFF: 结束命令，初始化变量
                    * - 0xCDF0: 开始命令，设置目标包数量并重置数据缓冲区
                    * - 0xCDDA: 数据接收，检查包序号、地址范围和校验和
                    */
                    switch(DataBuff.Data[1])
                    {
                        case 0xCDFF://结束命令
                        if(DataBuff.PackageTarge == DataBuff.PackageCnt - 1)
                        {
                            Msg[0] = ReceptOK;
                            UpData = 0;
                        }
                        else
                        {
                            Msg[0] = ErrorPack;
                        }
                        break;

                        case 0xCDF0://开始命令
                        DataBuff.PackageTarge = DataBuff.Data[2];
                        DataBuff.PackageCnt = 1;
                        DINT;
                        if(BootFlash.MyFlashErase() == 0)
                        {
                            Msg[0] = ReceptOK;
                        }
                        else
                        {
                            Msg[0] = ErrorFlash;
                        }
                        EINT;
                        break;

                        case 0xCDDA://数据接收
                        AddrTemp = (Uint32)DataBuff.Data[3] << 16 | DataBuff.Data[4];
                        if(DataBuff.Data[2] != DataBuff.PackageCnt)
                        {
                            Msg[0] = ErrorPack;
                        }
                        else if(AddrTemp < AddrMin || AddrTemp + (DataBuff.Data[5] - 1) > AddrMax) //需要修改为地址范围
                        {
                            Msg[0] = ErrorAddr;
                        }
                        break;

                        default:
                        Msg[0] = ErrorEnd;
                        break;
                    
                    }
                }


                
                if(DataBuff.Data[1] == 0xCDDA && Msg[0] == ReceptOK)
                {
                    DINT;
                    if(BootFlash.MyFlashCode(DataBuff.Data) == 0)
                    {
                        DataBuff.PackageCnt++;
                    }
                    else
                    {
                        Msg[0] = ErrorCode;
                    }
                    EINT;
                }
                
                SendString(Msg, MsgLen);

                /**
                    * 清空数据缓冲区，并重新设置目标数据长度。
                    */
                memset(DataBuff.Data, 0, sizeof(DataBuff.Data));
                DataBuff.DataCnt = 0;
                DataBuff.IsLowByte = 0;
                DataBuff.TargeCnt = 0x0107;
                memset(Msg, 0, sizeof(Msg));
                MsgLen = 0;
            }
        }
        /**
         * 在完成一次完整的数据处理后，调用InitValue函数重置相关变量。
         */
        InitValue();
        SciaRegs.SCIFFRX.bit.RXFFIENA = 1;
    }
}

/**
 * @brief SCI接收中断服务函数
 *
 * 处理串口数据接收流程，实现特定协议帧解析
 * 
 * @param 无
 * @return 无
 */
interrupt void SCIARX_ISR(void)
{
    Uint8 DataTemp = SciaRegs.SCIRXBUF.bit.RXDT;
    switch (SCI.DataBuff.DataCnt)
    {
        /**
         * 帧头同步阶段：
         * 检查第一个字节是否为帧起始标识0x01
         * 如果匹配则保存数据并进入下一接收状态
         */
        case 0:
        if(DataTemp == 0x01)
        {
            SCI.DataBuff.Data[SCI.DataBuff.DataCnt] = DataTemp;
            SCI.DataBuff.DataCnt++;
        }
        break;

        /**
         * 协议标识验证阶段：
         * 检查第二个字节是否为协议标识0xFF
         * 验证失败则重置接收状态
         */
        case 1:
        if(DataTemp == 0xFF)
        {
            SCI.DataBuff.Data[SCI.DataBuff.DataCnt] = DataTemp;
            SCI.DataBuff.DataCnt++;
        }
        else
        {
            SCI.DataBuff.DataCnt = 0;
        }
        break;

        /**
         * 数据校验阶段：
         * 1. 接收第六个字节（CRC校验值的一部分）
         * 2. 计算前4字节的CRC16校验值
         * 3. 校验通过则升级标志置1并重置接收缓冲区
         * 4. 校验失败则仅重置接收缓冲区
         */
        case 5:
        SCI.DataBuff.Data[SCI.DataBuff.DataCnt] = DataTemp;
        if(General.CRC16Modbus(SCI.DataBuff.Data, 4) == (SCI.DataBuff.Data[4] << 8 | SCI.DataBuff.Data[5]))
        {
            SciaRegs.SCIFFRX.bit.RXFFIENA = 0;
            SCI.InitValue();
            SCI.UpData = 1;
        }
        else
        {
            SCI.InitValue();
        }

        break;

        /**
         * 通用数据接收阶段：
         * 存储接收到的中间数据字节
         * 递增数据计数器
         */
        default:
        SCI.DataBuff.Data[SCI.DataBuff.DataCnt] = DataTemp;
        SCI.DataBuff.DataCnt++;
        break;
    }

    // 清除接收中断标志
    SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1;
    // 向PIE控制器发送中断应答
    PieCtrlRegs.PIEACK.bit.ACK9 = 1;
}

