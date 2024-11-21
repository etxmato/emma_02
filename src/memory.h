#ifndef MEMORY_H
#define MEMORY_H

#if defined (__linux__)
#include <stdint.h>
#endif

#ifdef __WXMSW__
typedef unsigned __int64 uint64_t;
#endif

#ifndef UINT64_MAX
#define UINT64_MAX   _UI64_MAX
#endif

#include "computerconfig.h"

class EmsMemory
{
public:
    Byte* mainMem;
    Byte* dataType_;
    Byte* labelType_;
    Byte* memoryType_;
    uint64_t* executed_;
};

class MemoryDefinition
{
public:
    vector<Byte> data;
    long mask;
};

class Memory
{
public:
    Memory();
    ~Memory();

    void defineMemoryType(long start, long end, int type); 
    void defineMemoryType(long address, int type); 
    void defineXmlBankMemoryType(int slot, int bank, long start, long end, int type);
    void defineXmlBankMemoryType(int slot, int bank, long address, int type);
    Byte getXmlBankMemoryType(int slot, int bank, long address);
    void defineXmlSlotMemoryType(int slot, long start, long end, int type);
    void defineXmlSlotMemoryType(int slot, long address, int type);
    Byte getXmlSlotMemoryType(int slot, long address);
    void defineExpansionMemoryType(int slot, long start, long end, int type);
    void defineExpansionMemoryType(int slot, long address, int type); 
    void defineBankMemoryType(int bank, long address, int type); 
    void defineEpromBankMemoryType(int bank, long address, int type); 
    void defineMultiCartMemoryType(long address, int type);
    void definePagerMemoryType(long address, int type);
    void defineEmsMemoryType(size_t emsNumber, long address, int type);
    int getPagerMemoryType(int address);
    void clearDebugMemory();
    void allocPagerMemory();
    void allocPagerMemory(Word start, Word end);
    void allocPagerMemoryCommon();
    void allocComxExpansionMemory();
    void allocSlotMemory();
    wxFileOffset allocRomMapperMemory(size_t number, wxFileOffset length);
    void allocEmsMemorySegment(size_t number);
    size_t allocMultiCartMemory(size_t memorySize);
    void allocTestCartMemory();
    void setEmsPage(size_t number, Byte value);
    void setEmsPage(std::vector<EmsMemoryConfiguration>::iterator emsConfig, Byte value);
    void setPager(int page, Byte value);
    void initRam(long start, long end);
    void initCpuRam();

    virtual Byte readMem(Word address) = 0;
    virtual void writeMem(Word address, Byte value, bool writeRom) = 0;
    virtual Byte readMemDebug(Word address) = 0;
    virtual void writeMemDebug(Word address, Byte value, bool writeRom) = 0;

    Byte readSequencerRom(Word address);
    void writeSequencerRom(Word address, Byte value);
    Byte readSequencer(Word address);
    Word readSequencerAddress(Word address);
    void writeSequencer(Word address, Byte value);
    

    Byte getRam(long address) {return mainMemory_[address];};
    void setRam(long address, Byte value) {mainMemory_[address] = value;};
    Byte getEmsPage(size_t emsNumber);
    Byte getEmsPage(std::vector<EmsMemoryConfiguration>::iterator emsConfig);
    Byte getPager(int port) {return pager_[port];};
    int getMemoryType(int i);
    int getExpansionMemoryType(int slot, int i) {return expansionMemoryType_[slot*32 + i];};
    int getBankMemoryType(int bank, int i) {return bankMemoryType_[bank*32 + i];};
    int getEpromBankMemoryType(int bank, int i) {return epromBankMemoryType_[bank*32 + i];};
    Byte getDynamicByte(Word address);
    Word getDynamicWord(Word address);
    void setDynamicRandomByte();
    wxString getMultiCartGame();

    int getEmsMemoryType(int i, size_t emsNumber) {return emsMemory_[emsNumber].memoryType_[((i - currentComputerConfiguration.emsMemoryConfiguration[emsNumber].startAddress) |(currentComputerConfiguration.emsMemoryConfiguration[emsNumber].page << currentComputerConfiguration.emsMemoryConfiguration[emsNumber].maskBits))/256];};

protected: 
    ComputerConfiguration currentComputerConfiguration;

    Word address_;
    int profilerCounter_;
    Byte* mainMemory_;
    Byte* mainMemoryDataType_;
    Byte* mainMemoryLabelType_;
    int memoryType_[256];
    Byte* expansionMemoryType_;
    int bankMemoryType_[128];
    int epromBankMemoryType_[160];
    int superBankMemoryType_[512];
    int multiCartMemoryType_[4096];
    int testCartMemoryType_[256];
    Byte* pagerMemory_;
    Byte* pagerMemoryDataType_;
    Byte* pagerMemoryLabelType_;
    Byte* pagerMemoryType_;
    int colorMemory1862_[255];
    int colorMemory1864_[1024];
    Byte mc6845ram_[16383];
    Byte mc6845CharRom_[2048];
    Byte diagRomReplacement_[4096];
    
    MemoryDefinition sequencerMemory;
    MemoryDefinition tilFontMemory;

    Byte* expansionRom_;
    Byte* expansionRomDataType_;
    Byte* expansionRomLabelType_;
    Byte* expansionRam_;
    Byte* expansionRamDataType_;
    Byte* expansionRamLabelType_;
    Byte* expansionEprom_;
    Byte* expansionEpromDataType_;
    Byte* expansionEpromLabelType_;
    Byte* expansionSuper_;
    Byte* expansionSuperDataType_;
    Byte* expansionSuperLabelType_;
    Byte* cpuRam_;
    Byte* cpuRamDataType_;
    Byte* cpuRamLabelType_;
    
    vector<Byte*> slotMemory_;
    vector<Byte*> slotMemoryDataType_;
    vector<Byte*> slotMemoryLabelType_;
    vector<Byte*> slotMemoryType_;
    vector<uint64_t*> slotMemoryExecuted_;
    int numberOfSlots_;

    Byte* multiCartRom_;
    Byte* multiCartRomDataType_;
    Byte* multiCartRomLabelType_;
    Byte* testCartRom_;
    Byte* testCartRomDataType_;
    Byte* testCartRomLabelType_;
    
    vector<EmsMemory> emsMemory_;
    wxUint32 emsSize_;
    
    vector<Word> slotSize_;
    vector<wxUint32> slotMemorySize_;
    wxUint32 slotMemSize_;

    Byte multiCartMsb_;
    Byte multiCartLsb_;

    uint64_t* mainMemoryExecuted_;
    uint64_t* expansionRomExecuted_;
    uint64_t* expansionRamExecuted_;
    uint64_t* expansionEpromExecuted_;
    uint64_t* expansionSuperExecuted_;
    uint64_t* cpuRamExecuted_;
    uint64_t* multiCartRomExecuted_;
    uint64_t* testCartRomExecuted_;
    uint64_t* pagerMemoryExecuted_;

    size_t multiCartMask_;
    
    bool multiCartMemoryDefined_;
    bool testCartMemoryDefined_;
    bool pagerDefined_;
    bool emsRamDefined_;
    bool emsRomDefined_;
    bool comxExpansionMemoryDefined_;
    bool slotMemoryDefined_;

    wxUint32 pagerSize_;

private:
    int pager_[256];
    
    Word pagerStart_;
    Word pagerEnd_;
    
    Word randomAddress_;

};

#endif  // MEMORY_H
