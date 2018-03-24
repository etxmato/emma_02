#ifndef MEMORY_H
#define MEMORY_H

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
	void defineEmsMemoryType(long address, int type); 
    void defineMultiCartMemoryType(long address, int type);
    void definePagerMemoryType(long address, int type);
    void defineRomMapperMemoryType(long address, int type);
    void clearDebugMemory();
	void allocPagerMemory();
	void allocRomMapperMemory(wxFileOffset length);
	void allocComxExpansionMemory();
    void allocEmsMemory();
    size_t allocMultiCartMemory(size_t memorySize);
    void allocTestCartMemory();
    void setEmsPage(Byte value);
    void setPager(int page, Byte value);
    void setRomMapper(Byte value);
	void initRam(long start, long end);
	
	virtual Byte readMem(Word addr) = 0;
	virtual void writeMem(Word addr, Byte value, bool writeRom) = 0; 

	Byte getRam(long address) {return mainMemory_[address];};
	void setRam(long address, Byte value) {mainMemory_[address] = value;};
	Byte getEms(long address) {return emsRam_[address];};
    Byte getEmsPage() {return emsPage_;};
    Byte getmaxNumberOfEmsPages() {return romMapperDefined_;};
	Byte getPager(int port) {return pager_[port];};
	int getMemoryType(int i) {return memoryType_[i];};
	int getExpansionMemoryType(int slot, int i) {return expansionMemoryType_[slot*32 + i];};
	int getBankMemoryType(int bank, int i) {return bankMemoryType_[bank*32 + i];};
	int getEpromBankMemoryType(int bank, int i) {return epromBankMemoryType_[bank*32 + i];};
	int getEmsMemoryType(int i) {return emsMemoryType_[emsPage_*64 + i];};
	int getPagerMemoryType(int i) {return pagerMemoryType_[pager_[(i>>4)&0xf] * 16 + (i&0xf)];};
    int getRomMapperMemoryType(int i) {return romMapperMemoryType_[emsPage_*128 + i];};
    bool isRomMapperDefined() {return romMapperDefined_;};
    Byte getDynamicByte(Word address);
    Word getDynamicWord(Word address);
    void setDynamicRandomByte();
    wxString getMultiCartGame();

protected: 
	Word address_;
	Byte* mainMemory_;
	Byte* mainMemoryDataType_;
	Byte* mainMemoryLabelType_;
	int memoryType_[256];
	int expansionMemoryType_[128];
	int bankMemoryType_[128];
	int epromBankMemoryType_[160];
	int superBankMemoryType_[512];
	int emsMemoryType_[2048];
    int multiCartMemoryType_[4096];
    int testCartMemoryType_[256];
    int pagerMemoryType_[4096];
	int romMapperMemoryType_[32768];
	int colorMemory1864_[1024];
	Byte mc6845ram_[2048];
	Byte mc6845CharRom_[2048];
    Byte diagRomReplacement_[4096];

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

    Byte* multiCartRom_;
	Byte* multiCartRomDataType_;
	Byte* multiCartRomLabelType_;
    Byte* testCartRom_;
    Byte* testCartRomDataType_;
    Byte* testCartRomLabelType_;
	Byte* emsRam_;
	Byte* emsRamDataType_;
	Byte* emsRamLabelType_;
	Byte emsPage_;
	Byte multiCartMsb_;
    Byte multiCartLsb_;
        
    size_t multiCartMask_;
    wxFileOffset romMapperSize_;
    Byte maxNumberOfPages_;
    
    bool multiCartMemoryDefined_;
    bool testCartMemoryDefined_;
    bool emsMemoryDefined_;
	bool pagerDefined_;
    bool romMapperDefined_;

private:
	bool comxExpansionMemoryDefined_;
	int pager_[16];
    
    Word randomAddress_;

};

#endif  // MEMORY_H
