/*
 *******************************************************************
 *** This software is copyright 2008 by Marcel van Tongeren      ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "main.h"
#include "memory.h"

Memory::Memory()
{
	mainMemory_ = (Byte*)malloc(65536);
    mainMemoryDataType_ = (Byte*)malloc(65536);
    mainMemoryLabelType_ = (Byte*)malloc(65536);
    for (int i=0; i<65536; i++)
	{
        mainMemory_[i] = 0;
        mainMemoryDataType_[i] = MEM_TYPE_DATA;
        mainMemoryLabelType_[i] = LABEL_TYPE_NONE;
    }
	for (int i=0; i<256; i++) memoryType_[i] = 0;

    switch (p_Main->getCpuStartupVideoRam())
    {
        case STARTUP_ZEROED:
            for (int i=0; i<1024; i++) colorMemory1864_[i] = 0;
            for (int i=0; i<2048; i++) mc6845ram_[i] = 0;
        break;
            
        case STARTUP_RANDOM:
            for (int i=0; i<1024; i++) colorMemory1864_[i] = rand() % 0x100;
            for (int i=0; i<2048; i++) mc6845ram_[i] = rand() % 0x100;
        break;
            
        case STARTUP_DYNAMIC:
            setDynamicRandomByte();
            for (int i=0; i<1024; i++)
                colorMemory1864_[i] = getDynamicByte(i);
            setDynamicRandomByte();
            for (int i=0; i<2048; i++)
                mc6845ram_[i] = getDynamicByte(i);
        break;
    }
    
	comxExpansionMemoryDefined_ = false;
    emsMemoryDefined_ = false;
    multiCartMemoryDefined_ = false;
    testCartMemoryDefined_ = false;
    pagerDefined_ = false;
    romMapperDefined_ = false;
}

Memory::~Memory()
{
	free(mainMemory_);
    free(mainMemoryDataType_);
    free(mainMemoryLabelType_);
    if (comxExpansionMemoryDefined_)
	{
		free(expansionRom_);	
        free(expansionRomDataType_);
        free(expansionRomLabelType_);
        free(expansionRam_);
        free(expansionRamDataType_);
        free(expansionRamLabelType_);
        free(expansionEprom_);
        free(expansionEpromDataType_);
        free(expansionEpromLabelType_);
        free(expansionSuper_);
        free(expansionSuperDataType_);
        free(expansionSuperLabelType_);
    }
    if (emsMemoryDefined_)
    {
        free(emsRam_);
        free(emsRamDataType_);
        free(emsRamLabelType_);
    }
    if (multiCartMemoryDefined_)
    {
        free(multiCartRom_);
        free(multiCartRomDataType_);
        free(multiCartRomLabelType_);
    }
    if (testCartMemoryDefined_)
    {
        free(testCartRom_);
        free(testCartRomDataType_);
        free(testCartRomLabelType_);
    }
    if (romMapperDefined_)
    {
        free(expansionRom_);
        free(expansionRomDataType_);
        free(expansionRomLabelType_);
    }
}

void Memory::clearDebugMemory()
{
	for (int i=0; i<65536; i++)
    {
        mainMemoryDataType_[i] = MEM_TYPE_DATA;
        mainMemoryLabelType_[i] = LABEL_TYPE_NONE;
    }
	if (comxExpansionMemoryDefined_)
	{
		for (int i=0; i<32768; i++)
        {
            expansionRomDataType_[i] = MEM_TYPE_DATA;
            expansionRomLabelType_[i] = LABEL_TYPE_NONE;
        }
		for (int i=0; i<32768; i++)
        {
            expansionRamDataType_[i] = MEM_TYPE_DATA;
            expansionRamLabelType_[i] = LABEL_TYPE_NONE;
        }
        for (int i=0; i<40960; i++)
        {
            expansionEpromDataType_[i] = MEM_TYPE_DATA;
            expansionEpromLabelType_[i] = LABEL_TYPE_NONE;
        }
        for (int i=0; i<131072; i++)
        {
            expansionSuperDataType_[i] = MEM_TYPE_DATA;
            expansionSuperLabelType_[i] = LABEL_TYPE_NONE;
        }
    }
    if (emsMemoryDefined_)
    {
        for (int i=0; i<524288; i++)
        {
            emsRamDataType_[i] = MEM_TYPE_DATA;
            emsRamLabelType_[i] = LABEL_TYPE_NONE;
        }
    }
    if (multiCartMemoryDefined_)
    {
        for (int i=0; i<1048576; i++)
        {
            multiCartRomDataType_[i] = MEM_TYPE_DATA;
            multiCartRomLabelType_[i] = LABEL_TYPE_NONE;
        }
    }
    if (romMapperDefined_)
    {
        for (int i=0; i<8388608; i++)
        {
            expansionRomDataType_[i] = MEM_TYPE_DATA;
            expansionRomLabelType_[i] = LABEL_TYPE_NONE;
        }
    }
}

void Memory::allocPagerMemory()
{
	mainMemory_ = (Byte*)realloc(mainMemory_, 1048576);
    mainMemoryDataType_ = (Byte*)realloc(mainMemoryDataType_, 1048576);
    mainMemoryLabelType_ = (Byte*)realloc(mainMemoryLabelType_, 1048576);
    pagerDefined_ = true;

	for (int i = 0; i<16; i++)
	{
		pager_[i] = i;
	}

	for (int i = 0; i<4096; i++) pagerMemoryType_[i] = RAM;

    switch (p_Main->getCpuStartupRam())
    {
        case STARTUP_ZEROED:
            for (int i = 0; i<1048576; i++)
            {
                mainMemory_[i] = 0;
                mainMemoryDataType_[i] = MEM_TYPE_DATA;
                mainMemoryLabelType_[i] = LABEL_TYPE_NONE;
            }
        break;
            
        case STARTUP_RANDOM:
            for (int i = 0; i<1048576; i++)
            {
                mainMemory_[i] = rand() % 0x100;
                mainMemoryDataType_[i] = MEM_TYPE_DATA;
                mainMemoryLabelType_[i] = LABEL_TYPE_NONE;
            }
        break;
            
        case STARTUP_DYNAMIC:
            setDynamicRandomByte();
            for (int i = 0; i<1048576; i++)
            {
                mainMemory_[i] = getDynamicByte(i);
                mainMemoryDataType_[i] = MEM_TYPE_DATA;
                mainMemoryLabelType_[i] = LABEL_TYPE_NONE;
            }
        break;
    }
}

void Memory::allocRomMapperMemory(wxFileOffset length)
{
    romMapperSize_ = length;

    maxNumberOfPages_ = (Byte) (romMapperSize_ / 32768);
    if (romMapperSize_ % 32768 != 0)
    {
        maxNumberOfPages_++;
        romMapperSize_ = maxNumberOfPages_ * 32768;
    }
 
    maxNumberOfPages_--;

	expansionRom_ = (Byte*)malloc((size_t)romMapperSize_);
    expansionRomDataType_ = (Byte*)malloc((size_t)romMapperSize_);
    expansionRomLabelType_ = (Byte*)malloc((size_t)romMapperSize_);
    romMapperDefined_ = true;

    emsPage_ = 0;

	for (int i = 0; i<32768; i++) romMapperMemoryType_[i] = ROM;

	for (int i = 0; i<romMapperSize_; i++)
	{
		expansionRom_[i] = 0xff;
		expansionRomDataType_[i] = MEM_TYPE_DATA;
        expansionRomLabelType_[i] = MEM_TYPE_DATA;
    }
}

void Memory::allocComxExpansionMemory()
{
	expansionRom_ = (Byte*)malloc(32768);
	expansionRomDataType_ = (Byte*)malloc(32768);
    expansionRomLabelType_ = (Byte*)malloc(32768);
    expansionRam_ = (Byte*)malloc(32768);
	expansionRamDataType_ = (Byte*)malloc(32768);
    expansionRamLabelType_ = (Byte*)malloc(32768);
    expansionEprom_ = (Byte*)malloc(40960);
    expansionEpromDataType_ = (Byte*)malloc(40960);
    expansionEpromLabelType_ = (Byte*)malloc(40960);
    expansionSuper_ = (Byte*)malloc(131072);
    expansionSuperDataType_ = (Byte*)malloc(131072);
    expansionSuperLabelType_ = (Byte*)malloc(131072);
    
	for (int i=0; i<128; i++) expansionMemoryType_[i] = 0;
	for (int i=0; i<128; i++) bankMemoryType_[i] = RAM;
	for (int i=0; i<160; i++) epromBankMemoryType_[i] = ROM;
	for (int i=0; i<256; i++) superBankMemoryType_[i] = ROM;
	for (int i=256; i<512; i++) superBankMemoryType_[i] = RAM;

	comxExpansionMemoryDefined_ = true;

	for (int i=0; i<32768; i++)
	{
		expansionRom_[i] = 0xff;
		expansionRomDataType_[i] = MEM_TYPE_DATA;
        expansionRomLabelType_[i] = LABEL_TYPE_NONE;
    }
    switch (p_Main->getCpuStartupRam())
    {
        case STARTUP_ZEROED:
            for (int i=0; i<32768; i++)
            {
                expansionRam_[i] = 0;
                expansionRamDataType_[i] = MEM_TYPE_DATA;
                expansionRamLabelType_[i] = LABEL_TYPE_NONE;
            }
            for (int i=65536; i<131072; i++)
            {
                expansionSuper_[i] = 0;
                expansionSuperDataType_[i] = MEM_TYPE_DATA;
                expansionSuperLabelType_[i] = LABEL_TYPE_NONE;
            }
        break;
            
        case STARTUP_RANDOM:
            for (int i=0; i<32768; i++)
            {
                expansionRam_[i] = rand() % 0x100;
                expansionRamDataType_[i] = MEM_TYPE_DATA;
                expansionRamLabelType_[i] = LABEL_TYPE_NONE;
            }
            for (int i=65536; i<131072; i++)
            {
                expansionSuper_[i] = rand() % 0x100;
                expansionSuperDataType_[i] = MEM_TYPE_DATA;
                expansionSuperLabelType_[i] = LABEL_TYPE_NONE;
           }
       break;
            
        case STARTUP_DYNAMIC:
            setDynamicRandomByte();
            for (int i=0; i<32768; i++)
            {
                expansionRam_[i] = getDynamicByte(i);
                expansionRamDataType_[i] = MEM_TYPE_DATA;
                expansionRamLabelType_[i] = LABEL_TYPE_NONE;
            }
            setDynamicRandomByte();
            for (int i=65536; i<131072; i++)
            {
                expansionSuper_[i] = getDynamicByte(i);
                expansionSuperDataType_[i] = MEM_TYPE_DATA;
                expansionSuperLabelType_[i] = LABEL_TYPE_NONE;
            }
        break;
    }
	for (int i=0; i<40960; i++)
	{
		expansionEprom_[i] = 0xff;
		expansionEpromDataType_[i] = MEM_TYPE_DATA;
        expansionEpromLabelType_[i] = LABEL_TYPE_NONE;
	}
	for (int i=0; i<65536; i++)
	{
		expansionSuper_[i] = 0xff;
		expansionSuperDataType_[i] = MEM_TYPE_DATA;
        expansionSuperLabelType_[i] = LABEL_TYPE_NONE;
	}
}

void Memory::allocEmsMemory()
{
	emsRam_ = (Byte*)malloc(524288);
	emsRamDataType_ = (Byte*)malloc(524288);
    emsRamLabelType_ = (Byte*)malloc(524288);
    emsPage_ = 0;

	for (int i=0; i<2048; i++) emsMemoryType_[i] = RAM;

	emsMemoryDefined_ = true;

    switch (p_Main->getCpuStartupRam())
    {
        case STARTUP_ZEROED:
            for (int i=0; i<524288; i++)
            {
                emsRam_[i] = 0;
                emsRamDataType_[i] = MEM_TYPE_DATA;
                emsRamLabelType_[i] = LABEL_TYPE_NONE;
            }
        break;
            
        case STARTUP_RANDOM:
            for (int i=0; i<524288; i++)
            {
                emsRam_[i] = rand() % 0x100;
                emsRamDataType_[i] = MEM_TYPE_DATA;
                emsRamLabelType_[i] = LABEL_TYPE_NONE;
            }
        break;
            
        case STARTUP_DYNAMIC:
            setDynamicRandomByte();
            for (int i=0; i<524288; i++)
            {
                emsRam_[i] = getDynamicByte(i);
                emsRamDataType_[i] = MEM_TYPE_DATA;
                emsRamLabelType_[i] = LABEL_TYPE_NONE;
            }
        break;
    }
}

size_t Memory::allocMultiCartMemory(size_t memorySize)
{
    if (memorySize < 1048576)
        multiCartMask_ = 0x7FFFF;
    else
        multiCartMask_ = 0xFFFFF;

    memorySize = multiCartMask_ + 1;
    
    multiCartRom_ = (Byte*)malloc(memorySize);
    multiCartRomDataType_ = (Byte*)malloc(memorySize);
    multiCartRomLabelType_ = (Byte*)malloc(memorySize);
    
    multiCartMemoryDefined_ = true;
    
    for (int i = 0; i<4096; i++) multiCartMemoryType_[i] = ROM;
    
    for (size_t i = 0; i<memorySize; i++)
    {
        multiCartRom_[i] = 0xff;
        multiCartRomDataType_[i] = MEM_TYPE_DATA;
        multiCartRomLabelType_[i] = LABEL_TYPE_NONE;
    }
    return memorySize;
}

void Memory::allocTestCartMemory()
{
    testCartRom_ = (Byte*)malloc(0xFFFF);
    testCartRomDataType_ = (Byte*)malloc(0xFFFF);
    testCartRomLabelType_ = (Byte*)malloc(0xFFFF);
    
    testCartMemoryDefined_ = true;
    
    for (int i = 0; i<4096; i++) testCartMemoryType_[i] = ROM;
    
    for (size_t i = 0; i<0xFFFF; i++)
    {
        testCartRom_[i] = 0xff;
        testCartRomDataType_[i] = MEM_TYPE_DATA;
        testCartRomLabelType_[i] = LABEL_TYPE_NONE;
    }
}

void Memory::setEmsPage(Byte value)
{
	emsPage_ = value & 0x1f;
	p_Main->updateSlotInfo();
}

void Memory::setPager(int page, Byte value)
{
    pager_[page] = value;
    p_Main->updateSlotInfo();
}

void Memory::setRomMapper(Byte value)
{
    emsPage_ = value;
    p_Main->updateSlotInfo();
}

void Memory::initRam(long start, long end)
{
    switch (p_Main->getCpuStartupRam())
    {
        case STARTUP_ZEROED:
            for (long i=start; i<=end; i++)
                mainMemory_[i] = 0;
        break;
            
        case STARTUP_RANDOM:
            for (long i=start; i<=end; i++)
                mainMemory_[i] = rand() % 0x100;
        break;
            
        case STARTUP_DYNAMIC:
            setDynamicRandomByte();
            for (long i=start; i<=end; i++)
                mainMemory_[i] = getDynamicByte(i);
        break;
	}
}

void Memory::defineMemoryType(long start, long end, int type) 
{
	start /= 256;
	end /= 256;
	for (long i=start; i<=end; i++)
	{
		memoryType_[i] = type;
	}
} 

void Memory::defineMemoryType(long address, int type) 
{
	if (multiCartMemoryDefined_ && (((address&0xfff) < 0x800) || ((address&0xfff) > 0xc00)))
	{
		defineMultiCartMemoryType(address, type);
		return;
	}
    if (emsMemoryDefined_ && (address >= 0x8000) && (address < 0xc000))
    {
        address /= 256;
        emsMemoryType_[emsPage_*64+(address&0x3f)] = type;
        return;
    }
    if (pagerDefined_)
    {
        definePagerMemoryType(address, type);
        return;
    }
    if (romMapperDefined_ && (address >= 0x8000) && (address <= 0xffff))
    {
        address /= 256;
        romMapperMemoryType_[emsPage_*128+address] = type;
        return;
    }
	memoryType_[address/256] = type;
} 

void Memory::defineExpansionMemoryType(int slot, long start, long end, int type) 
{
	start /= 256;
	end /= 256;
	for (long i=start; i<=end; i++)
	{
		expansionMemoryType_[slot*32+i] = type;
	}
} 

void Memory::defineExpansionMemoryType(int slot, long address, int type) 
{
	address /= 256;
	expansionMemoryType_[slot*32+address] = type;
} 

void Memory::defineBankMemoryType(int bank, long address, int type) 
{
	address /= 256;
	bankMemoryType_[bank*32+address] = type;
} 

void Memory::defineEpromBankMemoryType(int bank, long address, int type) 
{
	address /= 256;
	epromBankMemoryType_[bank*32+address] = type;
} 

void Memory::defineEmsMemoryType(long address, int type)
{
	address /= 256;
	emsMemoryType_[emsPage_*64+address] = type;
} 

void Memory::defineMultiCartMemoryType(long address, int type)
{
    address = (address+multiCartLsb_*0x1000+multiCartMsb_*0x10000)/256;
    multiCartMemoryType_[address] = type;
}

void Memory::definePagerMemoryType(long address, int type)
{
    address /= 256;
    pagerMemoryType_[pager_[(address>>4)&0xf] * 16 + (address&0xf)] = type;
}

void Memory::defineRomMapperMemoryType(long address, int type)
{
    address /= 256;
    romMapperMemoryType_[emsPage_*128+address] = type;
}

Byte Memory::getDynamicByte(Word address)
{
    if ((address & 0x7f) == randomAddress_)
        return rand() % 0x100;
    else
    {
        if (address&0x40)
            return 0;
        else
            return 0xFF;
    }
}

Word Memory::getDynamicWord(Word address)
{
    if ((address & 0x7f) == randomAddress_)
        return rand() % 0x10000;
    else
    {
        if (address&0x40)
            return 0;
        else
            return 0xFFFF;
    }
}

void Memory::setDynamicRandomByte()
{
    randomAddress_ = rand() % 0x80;
}

wxString Memory::getMultiCartGame()
{
    wxString game = "";
    Word address = 0xa00;
    Byte character;
    
    while (multiCartRom_[(address + multiCartLsb_ * 0x1000 + multiCartMsb_ * 0x10000)&multiCartMask_] >= 32 && multiCartRom_[(address + multiCartLsb_ * 0x1000 + multiCartMsb_ * 0x10000)&multiCartMask_] < 0x80)
    {
        character = multiCartRom_[((address++) + multiCartLsb_ * 0x1000 + multiCartMsb_ * 0x10000)&multiCartMask_];
        game.Append(character);
    }
    return game;
}

Byte Memory::readSequencerRom(Word address)
{
    return sequencerMemory_[address];
}

void Memory::writeSequencerRom(Word address, Byte value)
{
    sequencerMemory_[address] = value;
}

Byte Memory::readSequencer(Word address)
{
    Word sequencerAddress = (((address & 0x8) >> 1) & (address & 0x4)) ^ 0x4;
    sequencerAddress |= (address & 0x3);
    sequencerAddress |= ((address & 0x3e0) >> 2);
    sequencerAddress |= ((address & 0x7000) >> 4);
    
//    wxString traceText;
//    traceText.Printf("----  Sequencer = %04X - %04X", sequencerAddress, address);
//    p_Main->debugTrace(traceText);

    return sequencerMemory_[sequencerAddress];
}

Word Memory::readSequencerAddress(Word address)
{
    Word sequencerAddress = (((address & 0x8) >> 1) & (address & 0x4)) ^ 0x4;
    sequencerAddress |= (address & 0x3);
    sequencerAddress |= ((address & 0x3e0) >> 2);
    sequencerAddress |= ((address & 0x7000) >> 4);
    
    //    wxString traceText;
    //    traceText.Printf("----  Sequencer = %04X - %04X", sequencerAddress, address);
    //    p_Main->debugTrace(traceText);
    
    return sequencerAddress;
}

void Memory::writeSequencer(Word address, Byte value)
{
    Word sequencerAddress = (((address & 0x8) >> 1) & (address & 0x4)) ^ 0x4;
    sequencerAddress |= (address & 0x3);
    sequencerAddress |= ((address & 0x3e0) >> 2);
    sequencerAddress |= ((address & 0x7000) >> 4);
    
    sequencerMemory_[sequencerAddress] = value;
}



