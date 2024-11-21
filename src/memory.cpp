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
            for (int i=0; i<1024; i++)
            {
                colorMemory1862_[i] = 0;
                colorMemory1864_[i] = 0;
            }
            for (int i=0; i<16383; i++) mc6845ram_[i] = 0;
        break;
            
        case STARTUP_RANDOM:
            for (int i=0; i<1024; i++)
            {
                colorMemory1862_[i] = rand() % 0x100;
                colorMemory1864_[i] = rand() % 0x100;
            }
            for (int i=0; i<16383; i++) mc6845ram_[i] = rand() % 0x100;
        break;
            
        case STARTUP_DYNAMIC:
            setDynamicRandomByte();
            for (int i=0; i<1024; i++)
            {
                colorMemory1862_[i] = getDynamicByte(i);
                colorMemory1864_[i] = getDynamicByte(i);
            }
            setDynamicRandomByte();
            for (int i=0; i<16383; i++)
                mc6845ram_[i] = getDynamicByte(i);
        break;
    }
    
    sequencerMemory.data.resize(2048);
    for (int i=0; i<2040; i+=8)
    {
        sequencerMemory.data[i] = 0xb7;
        sequencerMemory.data[i+1] = 0xb7;
        sequencerMemory.data[i+2] = 0xa6;
        sequencerMemory.data[i+3] = 0xb7;
        sequencerMemory.data[i+4] = 0xb6;
        sequencerMemory.data[i+5] = 0xb7;
        sequencerMemory.data[i+6] = 0xb6;
        sequencerMemory.data[i+7] = 0xb7;
        sequencerMemory.data[i+8] = 0xb6;
    }

    comxExpansionMemoryDefined_ = false;
    slotMemoryDefined_ = false;
    multiCartMemoryDefined_ = false;
    testCartMemoryDefined_ = false;
    pagerDefined_ = false;
    emsRamDefined_ = false;
    emsRomDefined_ = false;
    numberOfSlots_ = 0;
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
        free(expansionMemoryType_);
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
    if (slotMemoryDefined_)
    {
        for (int slot=0; slot<numberOfSlots_; slot++)
        {
            free(slotMemory_[slot]);
            free(slotMemoryDataType_[slot]);
            free(slotMemoryLabelType_[slot]);
            if (profilerCounter_ != PROFILER_OFF)
                free(slotMemoryExecuted_[slot]);
        }
    }
    for (std::vector<EmsMemory>::iterator emsMemory = emsMemory_.begin (); emsMemory != emsMemory_.end (); ++emsMemory)
    {
        free(emsMemory->mainMem);
        free(emsMemory->dataType_);
        free(emsMemory->memoryType_);
        if (profilerCounter_ != PROFILER_OFF)
            free(emsMemory->executed_);
        free(emsMemory->labelType_);
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
    if (slotMemoryDefined_)
    {
        for (int slot=0; slot<numberOfSlots_; slot++)
        {
            for (int i=0; i<slotSize_[slot]; i++)
            {
                slotMemoryDataType_[slot][i] = MEM_TYPE_DATA;
                if (profilerCounter_ != PROFILER_OFF)
                    slotMemoryExecuted_[slot][i] = 0;
                slotMemoryLabelType_[slot][i] = LABEL_TYPE_NONE;
            }
        }
    }
    for (std::vector<EmsMemory>::iterator emsMemory = emsMemory_.begin (); emsMemory != emsMemory_.end (); ++emsMemory)
    {
        for (wxUint32 i=0; i<emsSize_; i++)
        {
            emsMemory->dataType_[i] = MEM_TYPE_DATA;
            if (profilerCounter_ != PROFILER_OFF)
                emsMemory->executed_[i] = 0;
            emsMemory->labelType_[i] = LABEL_TYPE_NONE;
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
    currentComputerConfiguration.memoryMapperConfiguration.startPort = 1;
    currentComputerConfiguration.memoryMapperConfiguration.endPort = 15;
    currentComputerConfiguration.memoryMapperConfiguration.maskBits = 12;
    pagerSize_ = 1048576;
    pagerStart_ = 0;
    pagerEnd_ = 0xFFFF;
    
    allocPagerMemoryCommon();
}
    
void Memory::allocPagerMemory(Word start, Word end)
{    
    pagerSize_ = (currentComputerConfiguration.memoryMapperConfiguration.mask + 1) * 256;
    pagerStart_ = start;
    pagerEnd_ = end;
    
    currentComputerConfiguration.memoryMapperConfiguration.startPort = (int) pagerStart_ / (currentComputerConfiguration.memoryMapperConfiguration.mask + 1);
    currentComputerConfiguration.memoryMapperConfiguration.endPort = (int) (pagerEnd_ + 1) / (currentComputerConfiguration.memoryMapperConfiguration.mask + 1);

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

    for (int i = currentComputerConfiguration.memoryMapperConfiguration.startPort; i<=currentComputerConfiguration.memoryMapperConfiguration.endPort; i++)
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
    expansionMemoryType_ = (Byte*)malloc(128);
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

/*    slotSize_ = currentComputerConfiguration.slotConfiguration.end - currentComputerConfiguration.slotConfiguration.start + 1;
    slotMemSize_ = slotSize_ * (int)currentComputerConfiguration.slotConfiguration.maxSlotNumber_;
    
    expansionRom_ = (Byte*)malloc(slotMemSize_);
    expansionRomDataType_ = (Byte*)malloc(slotMemSize_);
    expansionRomLabelType_ = (Byte*)malloc(slotMemSize_);
    expansionMemoryType_ = (Byte*)malloc(slotMemSize_/256);
    if (profilerCounter_ != PROFILER_OFF)
        expansionRomExecuted_ = (uint64_t*)malloc(slotMemSize_*8);

    for (int slot=0; slot<(int)currentComputerConfiguration.slotConfiguration.maxSlotNumber_; slot++)
    {
        for (int i=0; i<slotSize_/256; i++) expansionMemoryType_[i] = currentComputerConfiguration.slotConfiguration.slotInfo[slot].type;
    }

    for (int i=0; i<slotMemSize_; i++)
    {
        expansionRom_[i] = 0xff;
        expansionRomDataType_[i] = MEM_TYPE_DATA;
        if (profilerCounter_ != PROFILER_OFF)
            expansionRomExecuted_[i] = 0;
        expansionRomLabelType_[i] = LABEL_TYPE_NONE;
    }*/

void Memory::allocSlotMemory()
{
    slotMemory_.resize(numberOfSlots_+1);
    slotMemoryDataType_.resize(numberOfSlots_+1);
    slotMemoryLabelType_.resize(numberOfSlots_+1);
    slotMemoryType_.resize(numberOfSlots_+1);
    slotMemoryExecuted_.resize(numberOfSlots_+1);
    slotSize_.resize(numberOfSlots_+1);
    slotMemorySize_.resize(numberOfSlots_+1);

    slotSize_[numberOfSlots_] = currentComputerConfiguration.slotConfiguration.end - currentComputerConfiguration.slotConfiguration.start + 1;
    slotMemorySize_[numberOfSlots_] = slotSize_[numberOfSlots_] * (int)currentComputerConfiguration.slotConfiguration.slotInfo[numberOfSlots_].maxBankNumber_;

    slotMemory_[numberOfSlots_] = (Byte*)malloc(slotMemorySize_[numberOfSlots_]);
    slotMemoryDataType_[numberOfSlots_] = (Byte*)malloc(slotMemorySize_[numberOfSlots_]);
    slotMemoryLabelType_[numberOfSlots_] = (Byte*)malloc(slotMemorySize_[numberOfSlots_]);
    slotMemoryType_[numberOfSlots_] = (Byte*)malloc(slotMemorySize_[numberOfSlots_]/256);
    if (profilerCounter_ != PROFILER_OFF)
        slotMemoryExecuted_[numberOfSlots_] = (uint64_t*)malloc(slotMemorySize_[numberOfSlots_]*8);

    for (int i=0; i<slotSize_[numberOfSlots_]/256; i++) slotMemoryType_[numberOfSlots_][i] = currentComputerConfiguration.slotConfiguration.slotInfo[numberOfSlots_].type;

    for (wxUint32 i=0; i<slotMemorySize_[numberOfSlots_]; i++)
    {
        if (currentComputerConfiguration.slotConfiguration.slotInfo[numberOfSlots_].type == RAM)
        {
            switch (p_Main->getCpuStartupRam())
            {
                case STARTUP_ZEROED:
                    slotMemory_[numberOfSlots_][i] = 0;
                break;
                
                case STARTUP_RANDOM:
                    slotMemory_[numberOfSlots_][i] = rand() % 0x100;
                break;
                
                case STARTUP_DYNAMIC:
                    setDynamicRandomByte();
                    slotMemory_[numberOfSlots_][i] = getDynamicByte(i);
                break;
            }
        }
        else
            slotMemory_[numberOfSlots_][i] = 0xff;
        slotMemoryDataType_[numberOfSlots_][i] = MEM_TYPE_DATA;
        if (profilerCounter_ != PROFILER_OFF)
            slotMemoryExecuted_[numberOfSlots_][i] = 0;
        slotMemoryLabelType_[numberOfSlots_][i] = LABEL_TYPE_NONE;
    }
    numberOfSlots_++;
    slotMemoryDefined_ = true;
}

wxFileOffset Memory::allocRomMapperMemory(size_t emsNumber, wxFileOffset length)
{
    emsSize_ = (currentComputerConfiguration.emsMemoryConfiguration[emsNumber].mask + 1) * (currentComputerConfiguration.emsMemoryConfiguration[emsNumber].output.mask + 1);

    if (length > emsSize_)
        length = emsSize_;
    
    emsMemory_[emsNumber].mainMem = (Byte*)malloc((size_t)emsSize_);
    emsMemory_[emsNumber].dataType_ = (Byte*)malloc((size_t)emsSize_);
    if (profilerCounter_ != PROFILER_OFF)
        emsMemory_[emsNumber].executed_ = (uint64_t*)malloc((size_t)emsSize_*8);
    emsMemory_[emsNumber].labelType_ = (Byte*)malloc((size_t)emsSize_);
    emsMemory_[emsNumber].memoryType_ = (Byte*)malloc(emsSize_/256);
    currentComputerConfiguration.emsMemoryConfiguration[emsNumber].page = 0;

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
    emsSize_ = (currentComputerConfiguration.emsMemoryConfiguration[emsNumber].mask + 1) * (currentComputerConfiguration.emsMemoryConfiguration[emsNumber].output.mask + 1);
    
    emsMemory_[emsNumber].mainMem = (Byte*)malloc((size_t)emsSize_);
    emsMemory_[emsNumber].dataType_ = (Byte*)malloc((size_t)emsSize_);
    if (profilerCounter_ != PROFILER_OFF)
        emsMemory_[emsNumber].executed_ = (uint64_t*)malloc((size_t)emsSize_*8);
    emsMemory_[emsNumber].labelType_ = (Byte*)malloc((size_t)emsSize_);
    emsMemory_[emsNumber].memoryType_ = (Byte*)malloc(emsSize_/256);
    currentComputerConfiguration.emsMemoryConfiguration[emsNumber].page = 0;

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
    
    for (int i = 0; i<256; i++) testCartMemoryType_[i] = ROM;
    
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
    if (currentComputerConfiguration.emsMemoryConfiguration.size() != 0)
    {
        currentComputerConfiguration.emsMemoryConfiguration[emsNumber].page = value & currentComputerConfiguration.emsMemoryConfiguration[emsNumber].output.mask;
        p_Main->updateSlotInfo();
    }
}

void Memory::setEmsPage(std::vector<EmsMemoryConfiguration>::iterator emsConfig, Byte value)
{
    emsConfig->page = value & emsConfig->output.mask;
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
    size_t emsNumber = 0;
    for (std::vector<EmsMemoryConfiguration>::iterator emsConfig = currentComputerConfiguration.emsMemoryConfiguration.begin (); emsConfig != currentComputerConfiguration.emsMemoryConfiguration.end (); ++emsConfig)
    {
        for (std::vector<WordRange>::iterator range = emsConfig->range.begin (); range != emsConfig->range.end (); ++range)
        {
            if (address >= range->start && address <= range->end)
            {
                defineEmsMemoryType(emsNumber, address, type);
                return;
            }
        }
        emsNumber++;
    }
    if (pagerDefined_)
    {
        definePagerMemoryType(address, type);
        return;
    }
    memoryType_[address/256] = type;
} 

void Memory::defineXmlBankMemoryType(int slot, int bank, long start, long end, int type)
{
    int factor = slotSize_[slot]/256;
    start /= 256;
    end /= 256;
    for (long i=start; i<=end; i++)
    {
        slotMemoryType_[slot][bank*factor+i] = type;
    }
}

void Memory::defineXmlBankMemoryType(int slot, int bank, long address, int type)
{
    int factor = slotSize_[slot]/256;
    address /= 256;
    slotMemoryType_[slot][bank*factor+address] = type;
}

Byte Memory::getXmlBankMemoryType(int slot, int bank, long address)
{
    int factor = slotSize_[slot]/256;
    address /= 256;
    return slotMemoryType_[slot][bank*factor+address];
}

void Memory::defineXmlSlotMemoryType(int slot, long start, long end, int type)
{
    start /= 256;
    end /= 256;
    for (long i=start; i<=end; i++)
    {
        slotMemoryType_[slot][i] = type;
    }
}

void Memory::defineXmlSlotMemoryType(int slot, long address, int type)
{
    address /= 256;
    slotMemoryType_[slot][address] = type;
}

Byte Memory::getXmlSlotMemoryType(int slot, long address)
{
    if (!slotMemoryDefined_)
        return UNDEFINED;
    
    address /= 256;
    return slotMemoryType_[slot][address];
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
    int pagerNumber = (int) (address / (currentComputerConfiguration.memoryMapperConfiguration.mask + 1));
    address = (pager_[pagerNumber] * (currentComputerConfiguration.memoryMapperConfiguration.mask + 1))/256;
    pagerMemoryType_[address] = type;
}

void Memory::defineEmsMemoryType(size_t emsNumber, long address, int type)
{
    address = (address - currentComputerConfiguration.emsMemoryConfiguration[emsNumber].startAddress) / 256;
    emsMemory_[emsNumber].memoryType_[currentComputerConfiguration.emsMemoryConfiguration[emsNumber].page*((currentComputerConfiguration.emsMemoryConfiguration[emsNumber].mask + 1)/256)+address] = type;
}

int Memory::getPagerMemoryType(int address)
{
    int pagerNumber = (int) (address / (currentComputerConfiguration.memoryMapperConfiguration.mask + 1));
    address = (pager_[pagerNumber] * (currentComputerConfiguration.memoryMapperConfiguration.mask + 1))/256;

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
    return sequencerMemory.data[address];
}

void Memory::writeSequencerRom(Word address, Byte value)
{
    sequencerMemory.data[address] = value;
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

    return sequencerMemory.data[sequencerAddress];
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
    
    sequencerMemory.data[sequencerAddress] = value;
}

Byte Memory::getEmsPage(size_t emsNumber)
{
    if (currentComputerConfiguration.emsMemoryConfiguration.size() != 0)
        return currentComputerConfiguration.emsMemoryConfiguration[emsNumber].page;
    else
        return 0;
};

Byte Memory::getEmsPage(std::vector<EmsMemoryConfiguration>::iterator emsConfig)
{
    return emsConfig->page;
}

int Memory::getMemoryType(int i)
{
    return memoryType_[i];
}


