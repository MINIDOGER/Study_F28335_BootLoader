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

