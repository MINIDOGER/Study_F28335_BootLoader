#include "SCI.h"
#include "General.h"

ClassSCI SCI;

void ClassSCI::InitSCI(void)
{
    DataBuff.DataCnt = 0;
    UpData = 0;

    Uint16 temp;
    temp = 37500000/(8*115200)-1;
    
    InitSciGpio();

    EALLOW;

    SysCtrlRegs.PCLKCR0.bit.SCIAENCLK = 1;

    SciaRegs.SCIFFTX.all = 0xE04F;
    SciaRegs.SCIFFRX.bit.RXFIFORESET = 0;
    SciaRegs.SCIFFRX.all = 0x2061;
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
        if(General.CRC16Modbus(SCI.DataBuff.Data, 4) == 0xE831)
        {
            SciaRegs.SCIFFRX.bit.RXFFIENA = 0;
            SCI.DataBuff.DataCnt = 0;
            SCI.UpData = 1;
        }
        else
        {
            SCI.DataBuff.DataCnt = 0;
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

