#include "FlashOperation.h"

ClassFlash BootFlash;

#pragma CODE_SECTION("ramfuncs")
void ClassFlash::Flash_Test(void)
{
    Uint16 Status = 0;
	FLASH_ST EraseStatus;
	volatile Uint16 Buffer[2] = {0x12cd, 0x342b};
	Uint16 *Flash_ptr = (Uint16 *)0x00330000; // Pointer to a location in flash
	Uint32 Length = 0x02; // Number of 16-bit values to be programmed
	FLASH_ST ProgStatus; // Status structure
	FLASH_ST VerifyStatus;

	EALLOW;
	Flash_CPUScaleFactor = SCALE_FACTOR;
	Flash_CallbackPtr = NULL;
	EDIS;

	Status = Flash_Erase(SECTORB,&EraseStatus);
	if(Status != STATUS_SUCCESS)
	{
		SCI.Msg[0] = 0x01;
		SCI.SendString(SCI.Msg, 1);
		return;
	}

	Status = Flash_Program(Flash_ptr,(Uint16 *)Buffer,Length,&ProgStatus);
	if(Status != STATUS_SUCCESS)
	{
		SCI.Msg[0] = 0x02;
		SCI.SendString(SCI.Msg, 1);
		return;
	}

	Status = Flash_Verify(Flash_ptr,(Uint16 *)Buffer,Length,&VerifyStatus);
	if(Status != STATUS_SUCCESS)
	{
		SCI.Msg[0] = 0x03;
		SCI.SendString(SCI.Msg, 1);
		return;
	}

	SCI.Msg[0] = (Uint8)Status;
	SCI.SendString(SCI.Msg, 1);
}

#pragma CODE_SECTION("ramfuncs")
Uint16 ClassFlash::MyFlashErase(void)
{
    Uint16 Status = 0;
    FLASH_ST EraseStatus;

	EALLOW;
	Flash_CPUScaleFactor = SCALE_FACTOR;
	Flash_CallbackPtr = NULL;
	EDIS;

    Status = Flash_Erase(SECTORC,&EraseStatus);
	if(Status != STATUS_SUCCESS)
	{
		SCI.Msg[0] = 0x01;
		SCI.SendString(SCI.Msg, 1);
		return Status;
	}

    return Status;
}

#pragma CODE_SECTION("ramfuncs")
Uint16 ClassFlash::MyFlashCode(Uint16* Code)
{
    Uint16 Status = 0;
    Uint32 temp = (Uint32)Code[3] << 16 | Code[4];
	Uint16 *Flash_ptr = (Uint16 *) temp; // Pointer to a location in flash
	Uint32 Length = (Uint32) Code[5]; // Number of 16-bit values to be programmed
	FLASH_ST ProgStatus; // Status structure
	FLASH_ST VerifyStatus;

	EALLOW;
	Flash_CPUScaleFactor = SCALE_FACTOR;
	Flash_CallbackPtr = NULL;
	EDIS;

	Status = Flash_Program(Flash_ptr,&Code[6],Length,&ProgStatus);
	if(Status != STATUS_SUCCESS)
	{
		SCI.Msg[0] = 0x02;
		SCI.SendString(SCI.Msg, 1);
		return Status;
	}

	Status = Flash_Verify(Flash_ptr,&Code[6],Length,&VerifyStatus);
	if(Status != STATUS_SUCCESS)
	{
		SCI.Msg[0] = 0x03;
		SCI.SendString(SCI.Msg, 1);
		return Status;
	}

    return Status;
}
