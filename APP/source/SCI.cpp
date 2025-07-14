#include "SCI.h"

ClassSCI SCI;

void ClassSCI::InitSCI(void)
{
    InitValue();

    Uint16 temp;
    temp = 37500000/(8*115200)-1;
    
    InitSciGpio();

    EALLOW;

    SysCtrlRegs.PCLKCR0.bit.SCIAENCLK = 1;

    SciaRegs.SCIFFTX.all = 0xE04F;
    SciaRegs.SCIFFRX.all = 0x0061;
    SciaRegs.SCIFFRX.bit.RXFIFORESET = 1;
    SciaRegs.SCIFFCT.all = 0x0;

    SciaRegs.SCICCR.all = 0x0007;
    SciaRegs.SCICTL1.all = 0x0003;
    SciaRegs.SCICTL2.all = 0x0003;
    SciaRegs.SCICTL2.bit.TXINTENA = 1;
    SciaRegs.SCICTL2.bit.RXBKINTENA = 1;

    SciaRegs.SCIHBAUD = temp >> 8;
    SciaRegs.SCILBAUD = temp & 0xFF;

    SciaRegs.SCICTL1.all = 0x0023;

    IER |= M_INT9;
    PieCtrlRegs.PIEIER9.bit.INTx1=1;
    PieVectTable.SCIRXINTA = &SCIARX_ISR;

    EDIS;
}

void ClassSCI::InitValue(void)
{
    memset(DataBuff.Data, 0, sizeof(DataBuff.Data));
    DataBuff.DataCnt = 0;
    DataBuff.IsLowByte = 0;
    DataBuff.TargeCnt = 0x0107;
    DataBuff.PackageTarge = 0;
    DataBuff.PackageCnt = 1;
    UpData = 0;
    Msg = 0;
    NumFFRX = 0;
}

Uint8 ClassSCI::SendByte(Uint8 Byte)
{
    if(ScibRegs.SCIFFTX.bit.TXFFST < 16)
    {
        SciaRegs.SCITXBUF = Byte;
    }
    else
    {
        return 0;
    }
    return 1;
}

void ClassSCI::SendString(Uint8* Data, Uint16 Len)
{
    for (Uint16 i = 0; i < Len; i++)
    {
        if(!SendByte(Data[i]))
        {
            break;
        }
    }
}

void ClassSCI::UpDataTask()
{
    if(UpData == 1)
    {
        Uint16 temp = 0;
        Msg = 0xCD;
        SendString(&Msg, 1);
        Timer.TimeCnt = 0;
        while(Timer.TimeCnt < 0x14)
        {
            Timer.FlagTimer();
            if(Timer.TimeBaseFlag.bit.Timer500ms == 1)
            {
                DEBUG_LED_Toggle();
                Timer.TimeCnt++;
            }
            if(SciaRegs.SCIFFRX.bit.RXFFST > 0)
            {
                Timer.TimeCnt = 0;
                NumFFRX = SciaRegs.SCIFFRX.bit.RXFFST;
                for(Uint8 i = 0; i < NumFFRX; i++)
                {
                    if(DataBuff.IsLowByte)
                    {
                        DataBuff.Data[DataBuff.DataCnt] = SciaRegs.SCIRXBUF.bit.RXDT | temp << 8;
                        DataBuff.IsLowByte = 0;
                        DataBuff.DataCnt++;
                    }
                    else
                    {
                        temp = SciaRegs.SCIRXBUF.bit.RXDT;
                        DataBuff.IsLowByte = 1;
                    }
                }
                NumFFRX = 0;
                if(DataBuff.DataCnt == DataBuff.TargeCnt)
                {
                    if(DataBuff.Data[0] != 0x01)
                    {
                        Msg = ErrorDevice;
                    }
                    else if(DataBuff.Data[1] != 0xCDFF && DataBuff.Data[1] != 0xCDDA && DataBuff.Data[1] != 0xCDF0)
                    {
                        Msg = ErrorFun;
                    }

                    switch(DataBuff.Data[1])
                    {
                        case 0xCDFF://结束命令
                        InitValue();
                        break;

                        case 0xCDF0://开始命令
                        DataBuff.PackageTarge = DataBuff.Data[2];
                        DataBuff.PackageCnt = 1;
                        memset(DataBuff.Data, 0, sizeof(DataBuff.Data));
                        DataBuff.DataCnt = 0;
                        DataBuff.IsLowByte = 0;
                        DataBuff.TargeCnt = 0x0107;
                        Msg = 0;
                        break;

                        case 0xCDDA://数据接收
                        if(DataBuff.Data[2] != DataBuff.PackageCnt)
                        {
                            Msg = ErrorPack;
                        }
                        else if(((Uint32)DataBuff.Data[3] << 16) | DataBuff.Data[4] < AddrMin || (((Uint32)DataBuff.Data[3] << 16) | DataBuff.Data[4]) + (Uint32)DataBuff.Data[5] > AddrMax) //需要修改为地址范围
                        {
                            Msg = ErrorAddr;
                        }
                        else if(General.CheckSum_MINI(&DataBuff.Data[0], DataBuff.TargeCnt - 1) != DataBuff.Data[DataBuff.TargeCnt - 1])
                        {
                            Msg = ErrorCheck;
                        }
                        else
                        {
                            Msg = ReceptOK;
                        }
                        break;

                        default:
                        Msg = ErrorFun;
                        break;
                    
                    }
                    
                    SendString(&Msg, 1);
                    if(DataBuff.Data[1] == 0xCDDA && Msg == ReceptOK)
                    {
                        DataBuff.PackageCnt++;
                    }

                    memset(DataBuff.Data, 0, sizeof(DataBuff.Data));
                    DataBuff.DataCnt = 0;
                    DataBuff.IsLowByte = 0;
                    DataBuff.TargeCnt = 0x0107;
                    Msg = 0;
                }
                else if(DataBuff.DataCnt == 6)
                {
                    // 总元素数 = 包头(6) + 数据部分((len+1)/2) + 校验和(1)
                    DataBuff.TargeCnt = 7 + (DataBuff.Data[5] + 1) / 2;
                }
            }
        }
        InitValue();
        SciaRegs.SCIFFRX.bit.RXFFIENA = 1;
    }
}

interrupt void SCIARX_ISR(void)
{
    Uint8 temp = SciaRegs.SCIRXBUF.bit.RXDT;
    switch (SCI.DataBuff.DataCnt)
    {
        case 0:
        if(temp == 0x01)
        {
            SCI.DataBuff.Data[SCI.DataBuff.DataCnt] = temp;
            SCI.DataBuff.DataCnt++;
        }
        break;

        case 1:
        if(temp == 0xFF)
        {
            SCI.DataBuff.Data[SCI.DataBuff.DataCnt] = temp;
            SCI.DataBuff.DataCnt++;
        }
        else
        {
            SCI.DataBuff.DataCnt = 0;
        }
        break;

        case 5:
        SCI.DataBuff.Data[SCI.DataBuff.DataCnt] = temp;
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

        default:
        SCI.DataBuff.Data[SCI.DataBuff.DataCnt] = temp;
        SCI.DataBuff.DataCnt++;
        break;
    }

    SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1;
    PieCtrlRegs.PIEACK.bit.ACK9 = 1;
}

