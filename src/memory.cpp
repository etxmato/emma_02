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
    profilerCounter_ = p_Main->getProfilerCounter();
    
    mainMemory_ = (Byte*)malloc(65536);
    mainMemoryDataType_ = (Byte*)malloc(65536);
    mainMemoryLabelType_ = (Byte*)malloc(65536);
    cpuRam_ = (Byte*)malloc(255);
    cpuRamDataType_ = (Byte*)malloc(255);
    cpuRamLabelType_ = (Byte*)malloc(255);
    
    if (profilerCounter_ != PROFILER_OFF)
    {
        mainMemoryExecuted_ = (uint64_t*)malloc(65536*8);
        cpuRamExecuted_ = (uint64_t*)malloc(255*8);
    }
    for (int i=0; i<65536; i++)
	{
        mainMemory_[i] = 0;
        mainMemoryDataType_[i] = MEM_TYPE_DATA;
        if (profilerCounter_ != PROFILER_OFF)
            mainMemoryExecuted_[i] = 0;
        mainMemoryLabelType_[i] = LABEL_TYPE_NONE;
    }
    for (int i=0; i<255; i++)
    {
        cpuRam_[i] = 0;
        cpuRamDataType_[i] = MEM_TYPE_DATA;
        if (profilerCounter_ != PROFILER_OFF)
            cpuRamExecuted_[i] = 0;
        cpuRamLabelType_[i] = LABEL_TYPE_NONE;
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
    multiCartMemoryDefined_ = false;
    testCartMemoryDefined_ = false;
    pagerDefined_ = false;
    emsRamDefined_ = false;
    emsRomDefined_ = false;
}

Memory::~Memory()
{
	free(mainMemory_);
    free(mainMemoryDataType_);
    free(mainMemoryLabelType_);
    free(cpuRam_);
    free(cpuRamDataType_);
    free(cpuRamLabelType_);
    if (profilerCounter_ != PROFILER_OFF)
    {
        free(mainMemoryExecuted_);
        free(cpuRamExecuted_);
    }
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
        if (profilerCounter_ != PROFILER_OFF)
        {
            free(expansionRomExecuted_);
            free(expansionRamExecuted_);
            free(expansionEpromExecuted_);
            free(expansionSuperExecuted_);
        }
    }
    for (size_t emsNumber=0; emsNumber<computerConfiguration.emsConfigNumber_; emsNumber++)
    {
        free(emsMemory_[emsNumber].mainMem);
        free(emsMemory_[emsNumber].dataType_);
        free(emsMemory_[emsNumber].memoryType_);
        if (profilerCounter_ != PROFILER_OFF)
            free(emsMemory_[emsNumber].executed_);
        free(emsMemory_[emsNumber].labelType_);
    }
    if (multiCartMemoryDefined_)
    {
        free(multiCartRom_);
        free(multiCartRomDataType_);
        if (profilerCounter_ != PROFILER_OFF)
            free(multiCartRomExecuted_);
        free(multiCartRomLabelType_);
    }
    if (testCartMemoryDefined_)
    {
        free(testCartRom_);
        free(testCartRomDataType_);
        if (profilerCounter_ != PROFILER_OFF)
            free(testCartRomExecuted_);
        free(testCartRomLabelType_);
    }
    if (pagerDefined_)
    {
        free(pagerMemory_);
        free(pagerMemoryDataType_);
        free(pagerMemoryType_);
        if (profilerCounter_ != PROFILER_OFF)
            free(pagerMemoryExecuted_);
        free(pagerMemoryLabelType_);

    }
}

void Memory::clearDebugMemory()
{
	for (int i=0; i<65536; i++)
    {
        mainMemoryDataType_[i] = MEM_TYPE_DATA;
        if (profilerCounter_ != PROFILER_OFF)
            mainMemoryExecuted_[i] = 0;
        mainMemoryLabelType_[i] = LABEL_TYPE_NONE;
    }
    for (int i=0; i<255; i++)
    {
        cpuRamDataType_[i] = MEM_TYPE_DATA;
        if (profilerCounter_ != PROFILER_OFF)
            cpuRamExecuted_[i] = 0;
        cpuRamLabelType_[i] = LABEL_TYPE_NONE;
    }
	if (comxExpansionMemoryDefined_)
	{
		for (int i=0; i<32768; i++)
        {
            expansionRomDataType_[i] = MEM_TYPE_DATA;
            if (profilerCounter_ != PROFILER_OFF)
                expansionRomExecuted_[i] = 0;
            expansionRomLabelType_[i] = LABEL_TYPE_NONE;
        }
		for (int i=0; i<32768; i++)
        {
            expansionRamDataType_[i] = MEM_TYPE_DATA;
            if (profilerCounter_ != PROFILER_OFF)
                expansionRamExecuted_[i] = 0;
            expansionRamLabelType_[i] = LABEL_TYPE_NONE;
        }
        for (int i=0; i<40960; i++)
        {
            expansionEpromDataType_[i] = MEM_TYPE_DATA;
            if (profilerCounter_ != PROFILER_OFF)
                expansionEpromExecuted_[i] = 0;
            expansionEpromLabelType_[i] = LABEL_TYPE_NONE;
        }
        for (int i=0; i<131072; i++)
        {
            expansionSuperDataType_[i] = MEM_TYPE_DATA;
            if (profilerCounter_ != PROFILER_OFF)
                expansionSuperExecuted_[i] = 0;
            expansionSuperLabelType_[i] = LABEL_TYPE_NONE;
        }
    }
    for (size_t emsNumber=0; emsNumber<computerConfiguration.emsConfigNumber_; emsNumber++)
    {
        for (wxUint32 i=0; i<emsSize_; i++)
        {
            emsMemory_[emsNumber].dataType_[i] = MEM_TYPE_DATA;
            if (profilerCounter_ != PROFILER_OFF)
                emsMemory_[emsNumber].executed_[i] = 0;
            emsMemory_[emsNumber].labelType_[i] = LABEL_TYPE_NONE;
        }
    }
    if (multiCartMemoryDefined_)
    {
        for (wxUint32 i=0; i<1048576; i++)
        {
            multiCartRomDataType_[i] = MEM_TYPE_DATA;
            if (profilerCounter_ != PROFILER_OFF)
                multiCartRomExecuted_[i] = 0;
            multiCartRomLabelType_[i] = LABEL_TYPE_NONE;
        }
    }
    if (pagerDefined_)
    {
        for (wxUint32 i=0; i<pagerSize_; i++)
        {
            pagerMemoryDataType_[i] = MEM_TYPE_DATA;
            if (profilerCounter_ != PROFILER_OFF)
                pagerMemoryExecuted_[i] = 0;
            pagerMemoryLabelType_[i] = LABEL_TYPE_NONE;
        }
    }
}

void Memory::allocPagerMemory()
{
    computerConfiguration.startPort_ = 1;
    computerConfiguration.endPort_ = 15;
    computerConfiguration.pagerMaskBits_ = 12;
    pagerSize_ = 1048576;
    pagerStart_ = 0;
    pagerEnd_ = 0xFFFF;
    
    allocPagerMemoryCommon();
}
    
void Memory::allocPagerMemory(Word start, Word end)
{    
    pagerSize_ = (computerConfiguration.pagerMask_ + 1) * 256;
    pagerStart_ = start;
    pagerEnd_ = end;
    
    computerConfiguration.startPort_ = (int) pagerStart_ / (computerConfiguration.pagerMask_ + 1);
    computerConfiguration.endPort_ = (int) (pagerEnd_ + 1) / (computerConfiguration.pagerMask_ + 1);

    allocPagerMemoryCommon();
}

void Memory::allocPagerMemoryCommon()
{
    pagerMemory_ = (Byte*)malloc(pagerSize_);
    pagerMemoryDataType_ = (Byte*)malloc(pagerSize_);
    if (profilerCounter_ != PROFILER_OFF)
        pagerMemoryExecuted_ = (uint64_t*)malloc(pagerSize_*8);
    pagerMemoryLabelType_ = (Byte*)malloc(pagerSize_);
    pagerMemoryType_ = (Byte*)malloc(pagerSize_/256);
    pagerDefined_ = true;

    for (int i = computerConfiguration.startPort_; i<=computerConfiguration.endPort_; i++)
    {
        pager_[i] = i;
    }

    for (wxUint32 i = 0; i<(pagerSize_/256); i++) pagerMemoryType_[i] = RAM;

    switch (p_Main->getCpuStartupRam())
    {
        case STARTUP_ZEROED:
            for (wxUint32 i = 0; i<pagerSize_; i++)
            {
                pagerMemory_[i] = 0;
                pagerMemoryDataType_[i] = MEM_TYPE_DATA;
                if (profilerCounter_ != PROFILER_OFF)
                    pagerMemoryExecuted_[i] = 0;
                pagerMemoryLabelType_[i] = LABEL_TYPE_NONE;
            }
        break;
            
        case STARTUP_RANDOM:
            for (wxUint32 i = 0; i<pagerSize_; i++)
            {
                pagerMemory_[i] = rand() % 0x100;
                pagerMemoryDataType_[i] = MEM_TYPE_DATA;
                if (profilerCounter_ != PROFILER_OFF)
                    pagerMemoryExecuted_[i] = 0;
                pagerMemoryLabelType_[i] = LABEL_TYPE_NONE;
            }
        break;
            
        case STARTUP_DYNAMIC:
            setDynamicRandomByte();
            for (wxUint32 i = 0; i<pagerSize_; i++)
            {
                pagerMemory_[i] = getDynamicByte(i);
                pagerMemoryDataType_[i] = MEM_TYPE_DATA;
                if (profilerCounter_ != PROFILER_OFF)
                    pagerMemoryExecuted_[i] = 0;
                pagerMemoryLabelType_[i] = LABEL_TYPE_NONE;
            }
        break;
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
    if (profilerCounter_ != PROFILER_OFF)
    {
        expansionRomExecuted_ = (uint64_t*)malloc(32768*8);
        expansionRamExecuted_ = (uint64_t*)malloc(32768*8);
        expansionEpromExecuted_ = (uint64_t*)malloc(40960*8);
        expansionSuperExecuted_ = (uint64_t*)malloc(131072*8);
    }
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
        if (profilerCounter_ != PROFILER_OFF)
            expansionRomExecuted_[i] = 0;
        expansionRomLabelType_[i] = LABEL_TYPE_NONE;
    }
    switch (p_Main->getCpuStartupRam())
    {
        case STARTUP_ZEROED:
            for (int i=0; i<32768; i++)
            {
                expansionRam_[i] = 0;
                expansionRamDataType_[i] = MEM_TYPE_DATA;
                if (profilerCounter_ != PROFILER_OFF)
                    expansionRamExecuted_[i] = 0;
                expansionRamLabelType_[i] = LABEL_TYPE_NONE;
            }
            for (int i=65536; i<131072; i++)
            {
                expansionSuper_[i] = 0;
                expansionSuperDataType_[i] = MEM_TYPE_DATA;
                if (profilerCounter_ != PROFILER_OFF)
                    expansionSuperExecuted_[i] = 0;
                expansionSuperLabelType_[i] = LABEL_TYPE_NONE;
            }
        break;
            
        case STARTUP_RANDOM:
            for (int i=0; i<32768; i++)
            {
                expansionRam_[i] = rand() % 0x100;
                expansionRamDataType_[i] = MEM_TYPE_DATA;
                if (profilerCounter_ != PROFILER_OFF)
                    expansionRamExecuted_[i] = 0;
                expansionRamLabelType_[i] = LABEL_TYPE_NONE;
            }
            for (int i=65536; i<131072; i++)
            {
                expansionSuper_[i] = rand() % 0x100;
                expansionSuperDataType_[i] = MEM_TYPE_DATA;
                if (profilerCounter_ != PROFILER_OFF)
                    expansionSuperExecuted_[i] = 0;
                expansionSuperLabelType_[i] = LABEL_TYPE_NONE;
           }
       break;
            
        case STARTUP_DYNAMIC:
            setDynamicRandomByte();
            for (int i=0; i<32768; i++)
            {
                expansionRam_[i] = getDynamicByte(i);
                expansionRamDataType_[i] = MEM_TYPE_DATA;
                if (profilerCounter_ != PROFILER_OFF)
                    expansionRamExecuted_[i] = 0;
                expansionRamLabelType_[i] = LABEL_TYPE_NONE;
            }
            setDynamicRandomByte();
            for (int i=65536; i<131072; i++)
            {
                expansionSuper_[i] = getDynamicByte(i);
                expansionSuperDataType_[i] = MEM_TYPE_DATA;
                if (profilerCounter_ != PROFILER_OFF)
                    expansionSuperExecuted_[i] = 0;
                expansionSuperLabelType_[i] = LABEL_TYPE_NONE;
            }
        break;
    }
	for (int i=0; i<40960; i++)
	{
		expansionEprom_[i] = 0xff;
        expansionEpromDataType_[i] = MEM_TYPE_DATA;
        if (profilerCounter_ != PROFILER_OFF)
            expansionEpromExecuted_[i] = 0;
        expansionEpromLabelType_[i] = LABEL_TYPE_NONE;
	}
	for (int i=0; i<65536; i++)
	{
		expansionSuper_[i] = 0xff;
        expansionSuperDataType_[i] = MEM_TYPE_DATA;
        if (profilerCounter_ != PROFILER_OFF)
            expansionSuperExecuted_[i] = 0;
        expansionSuperLabelType_[i] = LABEL_TYPE_NONE;
	}
}

wxFileOffset Memory::allocRomMapperMemory(size_t emsNumber, wxFileOffset length)
{
    emsSize_ = (computerConfiguration.emsConfig_[emsNumber].mask + 1) * (computerConfiguration.emsConfig_[emsNumber].outputMask + 1);

    if (length > emsSize_)
        length = emsSize_;
    
    emsMemory_[emsNumber].mainMem = (Byte*)malloc((size_t)emsSize_);
    emsMemory_[emsNumber].dataType_ = (Byte*)malloc((size_t)emsSize_);
    if (profilerCounter_ != PROFILER_OFF)
        emsMemory_[emsNumber].executed_ = (uint64_t*)malloc((size_t)emsSize_*8);
    emsMemory_[emsNumber].labelType_ = (Byte*)malloc((size_t)emsSize_);
    emsMemory_[emsNumber].memoryType_ = (Byte*)malloc(emsSize_/256);
    computerConfiguration.emsConfig_[0].page = 0;

    for (wxUint32 i = 0; i<(emsSize_/256); i++) emsMemory_[emsNumber].memoryType_[i] = ROM;

    emsRomDefined_ = true;

    for (wxUint32 i = 0; i<emsSize_; i++)
    {
        emsMemory_[emsNumber].mainMem[i] = 0xff;
        emsMemory_[emsNumber].dataType_[i] = MEM_TYPE_DATA;
        if (profilerCounter_ != PROFILER_OFF)
            emsMemory_[emsNumber].executed_[i] = 0;
        emsMemory_[emsNumber].labelType_[i] = MEM_TYPE_DATA;
    }
    return length;
}

void Memory::allocEmsMemorySegment(size_t emsNumber)
{    
    emsSize_ = (computerConfiguration.emsConfig_[emsNumber].mask + 1) * (computerConfiguration.emsConfig_[emsNumber].outputMask + 1);
    
    emsMemory_[emsNumber].mainMem = (Byte*)malloc((size_t)emsSize_);
    emsMemory_[emsNumber].dataType_ = (Byte*)malloc((size_t)emsSize_);
    if (profilerCounter_ != PROFILER_OFF)
        emsMemory_[emsNumber].executed_ = (uint64_t*)malloc((size_t)emsSize_*8);
    emsMemory_[emsNumber].labelType_ = (Byte*)malloc((size_t)emsSize_);
    emsMemory_[emsNumber].memoryType_ = (Byte*)malloc(emsSize_/256);
    computerConfiguration.emsConfig_[emsNumber].page = 0;

    for (wxUint32 i = 0; i<(emsSize_/256); i++)
    {
// Some test code to check RAM / ROM type every 16K
//        if ((i & (0x4000/256)) == 0x4000/256)
//            emsMemory_[emsNumber].memoryType_[i] = ROM;
//        else
            emsMemory_[emsNumber].memoryType_[i] = RAM;
   }

    emsRamDefined_ = true;

    switch (p_Main->getCpuStartupRam())
    {
        case STARTUP_ZEROED:
            for (wxUint32 i=0; i<emsSize_; i++)
            {
                emsMemory_[emsNumber].mainMem[i] = 0;
                emsMemory_[emsNumber].dataType_[i] = MEM_TYPE_DATA;
                if (profilerCounter_ != PROFILER_OFF)
                    emsMemory_[emsNumber].executed_[i] = 0;
                emsMemory_[emsNumber].labelType_[i] = MEM_TYPE_DATA;
            }
        break;
            
        case STARTUP_RANDOM:
            for (wxUint32 i=0; i<emsSize_; i++)
            {
                emsMemory_[emsNumber].mainMem[i] = rand() % 0x100;
                emsMemory_[emsNumber].dataType_[i] = MEM_TYPE_DATA;
                if (profilerCounter_ != PROFILER_OFF)
                    emsMemory_[emsNumber].executed_[i] = 0;
                emsMemory_[emsNumber].labelType_[i] = LABEL_TYPE_NONE;
            }
        break;
            
        case STARTUP_DYNAMIC:
            setDynamicRandomByte();
            for (wxUint32 i=0; i<emsSize_; i++)
            {
                emsMemory_[emsNumber].mainMem[i] = getDynamicByte(i);
                emsMemory_[emsNumber].dataType_[i] = MEM_TYPE_DATA;
                if (profilerCounter_ != PROFILER_OFF)
                    emsMemory_[emsNumber].executed_[i] = 0;
                emsMemory_[emsNumber].labelType_[i] = LABEL_TYPE_NONE;
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
    if (profilerCounter_ != PROFILER_OFF)
        multiCartRomExecuted_ = (uint64_t*)malloc(memorySize*8);
    multiCartRomLabelType_ = (Byte*)malloc(memorySize);
    
    multiCartMemoryDefined_ = true;
    
    for (int i = 0; i<4096; i++) multiCartMemoryType_[i] = ROM;
    
    for (size_t i = 0; i<memorySize; i++)
    {
        multiCartRom_[i] = 0xff;
        multiCartRomDataType_[i] = MEM_TYPE_DATA;
        if (profilerCounter_ != PROFILER_OFF)
            multiCartRomExecuted_[i] = 0;
        multiCartRomLabelType_[i] = LABEL_TYPE_NONE;
    }
    return memorySize;
}

void Memory::allocTestCartMemory()
{
    testCartRom_ = (Byte*)malloc(0xFFFF);
    testCartRomDataType_ = (Byte*)malloc(0xFFFF);
    if (profilerCounter_ != PROFILER_OFF)
        testCartRomExecuted_ = (uint64_t*)malloc(0xFFFF*8);
    testCartRomLabelType_ = (Byte*)malloc(0xFFFF);
    
    testCartMemoryDefined_ = true;
    
    for (int i = 0; i<4096; i++) testCartMemoryType_[i] = ROM;
    
    for (size_t i = 0; i<0xFFFF; i++)
    {
        testCartRom_[i] = 0xff;
        testCartRomDataType_[i] = MEM_TYPE_DATA;
        if (profilerCounter_ != PROFILER_OFF)
            testCartRomExecuted_[i] = 0;
        testCartRomLabelType_[i] = LABEL_TYPE_NONE;
    }
}

void Memory::setEmsPage(size_t emsNumber, Byte value)
{
    computerConfiguration.emsConfig_[emsNumber].page = value & computerConfiguration.emsConfig_[emsNumber].outputMask;
	p_Main->updateSlotInfo();
}

void Memory::setPager(int page, Byte value)
{
    pager_[page] = value;
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

void Memory::initCpuRam()
{
    switch (p_Main->getCpuStartupRam())
    {
        case STARTUP_ZEROED:
            for (long i=0; i<=255; i++)
                cpuRam_[i] = 0;
        break;
            
        case STARTUP_RANDOM:
            for (long i=0; i<=255; i++)
                cpuRam_[i] = rand() % 0x100;
        break;
            
        case STARTUP_DYNAMIC:
            setDynamicRandomByte();
            for (long i=0; i<=255; i++)
                cpuRam_[i] = getDynamicByte(i);
        break;
    }
}

void Memory::defineMemoryType(long start, long end, int type)
{
    start = (start / 256) & 0xff;
    end = (end / 256) & 0xff;
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
    for (int emsNumber=0; emsNumber<(int)computerConfiguration.emsConfigNumber_; emsNumber++)
    {
        if (address >= computerConfiguration.emsConfig_[emsNumber].start && address <= computerConfiguration.emsConfig_[emsNumber].end)
        {
            defineEmsMemoryType(emsNumber, address, type);
            return;
        }
    }
    if (pagerDefined_)
    {
        definePagerMemoryType(address, type);
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

void Memory::defineMultiCartMemoryType(long address, int type)
{
    address = (address+multiCartLsb_*0x1000+multiCartMsb_*0x10000)/256;
    multiCartMemoryType_[address] = type;
}

void Memory::definePagerMemoryType(long address, int type)
{
    int pagerNumber = (int) (address / (computerConfiguration.pagerMask_ + 1));
    address = (pager_[pagerNumber] * (computerConfiguration.pagerMask_ + 1))/256;
    pagerMemoryType_[address] = type;
}

void Memory::defineEmsMemoryType(size_t emsNumber, long address, int type)
{
    address = (address - computerConfiguration.emsConfig_[emsNumber].start) / 256;
    emsMemory_[emsNumber].memoryType_[computerConfiguration.emsConfig_[emsNumber].page*((computerConfiguration.emsConfig_[emsNumber].mask + 1)/256)+address] = type;
}

int Memory::getPagerMemoryType(int address)
{
    int pagerNumber = (int) (address / (computerConfiguration.pagerMask_ + 1));
    address = (pager_[pagerNumber] * (computerConfiguration.pagerMask_ + 1))/256;

    return pagerMemoryType_[address];
};

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



