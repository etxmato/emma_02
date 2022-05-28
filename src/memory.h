#ifndef MEMORY_H
#define MEMORY_H

#if defined (__linux__)
#include <stdint.h>
#endif

#ifdef __WXMSW__
typedef unsigned __int64 uint64_t;
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

class Memory
{
public:
	Memory();
	~Memory();

	void defineMemoryType(long start, long end, int type); 
	void defineMemoryType(long address, int type); 
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
    wxFileOffset allocRomMapperMemory(size_t number, wxFileOffset length);
    void allocEmsMemorySegment(size_t number);
    size_t allocMultiCartMemory(size_t memorySize);
    void allocTestCartMemory();
    void setEmsPage(size_t number, Byte value);
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
    Byte getEmsPage(size_t emsNumber) {return computerConfiguration.emsConfig_[emsNumber].page;};
	Byte getPager(int port) {return pager_[port];};
	int getMemoryType(int i) {return memoryType_[i];};
	int getExpansionMemoryType(int slot, int i) {return expansionMemoryType_[slot*32 + i];};
	int getBankMemoryType(int bank, int i) {return bankMemoryType_[bank*32 + i];};
	int getEpromBankMemoryType(int bank, int i) {return epromBankMemoryType_[bank*32 + i];};
    Byte getDynamicByte(Word address);
    Word getDynamicWord(Word address);
    void setDynamicRandomByte();
    wxString getMultiCartGame();

    int getEmsMemoryType(int i, size_t emsNumber) {return emsMemory_[emsNumber].memoryType_[((i - computerConfiguration.emsConfig_[emsNumber].start) |(computerConfiguration.emsConfig_[emsNumber].page << computerConfiguration.emsConfig_[emsNumber].maskBits))/256];};

protected: 
    Conf computerConfiguration;

    Word address_;
    int profilerCounter_;
	Byte* mainMemory_;
    Byte* mainMemoryDataType_;
	Byte* mainMemoryLabelType_;
	int memoryType_[256];
	int expansionMemoryType_[128];
	int bankMemoryType_[128];
	int epromBankMemoryType_[160];
	int superBankMemoryType_[512];
    int multiCartMemoryType_[4096];
    int testCartMemoryType_[256];
    Byte* pagerMemory_;
    Byte* pagerMemoryDataType_;
    Byte* pagerMemoryLabelType_;
    Byte* pagerMemoryType_;
	int colorMemory1864_[1024];
	Byte mc6845ram_[2048];
	Byte mc6845CharRom_[2048];
    Byte diagRomReplacement_[4096];
	Byte sequencerMemory_[2048];
	
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

    Byte* multiCartRom_;
    Byte* multiCartRomDataType_;
	Byte* multiCartRomLabelType_;
    Byte* testCartRom_;
    Byte* testCartRomDataType_;
    Byte* testCartRomLabelType_;
    
    vector<EmsMemory> emsMemory_;
    wxUint32 emsSize_;
    
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

    wxUint32 pagerSize_;

private:
	int pager_[256];
    
    Word pagerStart_;
    Word pagerEnd_;
    
    Word randomAddress_;

};

#endif  // MEMORY_H
