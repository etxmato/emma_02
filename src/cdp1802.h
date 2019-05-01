#ifndef CDP1802_H
#define CDP1802_H
 
#include <bitset>
using namespace std;

#include "iodevice.h"
#include "memory.h"
#include "sound.h"
#include "elfconfiguration.h"

#define LOAD 0
#define RESET 1
#define PAUSE 2
#define RUN 3
#define UNDEFINDEDMODE 5

class Cdp1802 : public IoDevice, public Memory, public Sound
{
public:
	Cdp1802();
	virtual ~Cdp1802() {};

	void initCpu(int computerType);
	void resetCpu();

	void machineCycle();
	virtual void cpuInstruction() = 0;

	void setClear(int value);
	int getClear() {return clear_;};
	void setWait(int value);
	int getWait() {return wait_;};
	void cpuCycle();
	void dmaIn(Byte value);
	Byte dmaOut();
	Byte pixieDmaOut(int *color);
	void visicomDmaOut(Byte *vram1, Byte *vram2);
	Byte pixieDmaOut();

	void interrupt();
	void pixieInterrupt();
	void setEf(int flag, int value);

	bool readIntelFile(wxString fileName, int memoryType, long end, bool showFilename);
	bool readIntelFile(wxString fileName, int memoryType, Word* lastAddress, long end, bool showFilename);
	bool readLstFile(wxString fileName, int memoryType, long end, bool showFilename);
	void saveIntelFile(wxString fileName, long start, long end);
	void saveBinFile(wxString fileName, long start, long end);
	bool readBinFile(wxString fileName, int memoryType, Word address, long end, bool showFilename, bool showAddressPopup, Word specifiedStartAddress);
    bool readBinFile(wxString fileName, int memoryType, Word address, Word* lastAddress, long end, bool showFilename);
    bool readRomMapperBinFile(wxString fileName);
    bool readMultiCartBinFile(wxString dirName, wxString fileName);
    void setAddress(bool showFilename, Word start, Word end);
	void checkLoadedSoftware();
	bool readProgram(wxString romDir, wxString rom, int memoryType, Word address, bool showFilename);
	bool readProgram(wxString romDir, wxString rom, int memoryType, Word address, Word* lastAddress, bool showFilename);
	bool readProgramCidelsa(wxString romDir, wxString rom, int memoryType, Word address, bool showFilename);
	bool readProgramTmc600(wxString romDir, wxString rom, int memoryType, Word address, bool showFilename);
	bool readProgramPecom(wxString romDir, wxString rom, int memoryType, Word address, bool showFilename);
	void readSt2Program(int computerType);
    bool readFile(wxString fileName, int memoryType, Word address, long end, bool showFilename);
    bool readFile(wxString fileName, int memoryType, Word address, long end, bool showFilename, bool showAddressPopup, Word specifiedStartAddress);
	bool readFile(wxString fileName, int memoryType, Word address, Word* lastAddress, long end, bool showFilename);

	Word getScratchpadRegister(int number) {return scratchpadRegister_[number];};
	void setScratchpadRegister(int number, Word scratchpadRegister) {scratchpadRegister_[number] = scratchpadRegister;};
	Byte getAccumulator() {return accumulator_;};
	void setAccumulator(Byte accumulator) {accumulator_ = accumulator;};
	Byte getDataFlag() {return dataFlag_;};
	void setDataFlag(Byte dataFlag) {dataFlag_ = dataFlag;};
	Byte getRegisterT() {return registerT_;};
	void setRegisterT(Byte registerT) {registerT_ = registerT;};
	Byte getFlipFlopQ() { return flipFlopQ_; };
	void setFlipFlopQ(Byte flipFlopQ) { flipFlopQ_ = flipFlopQ; };
	Byte isReadyToReceiveData(int ef) { return readyToReceiveData[ef]; };
	void setNotReadyToReceiveData(int ef) { readyToReceiveData[ef] = false; };
	Byte getProgramCounter() {return programCounter_;};
	void setProgramCounter(Byte programCounter) {programCounter_ = programCounter;};
	Byte getDataPointer() {return dataPointer_;};
	void setDataPointer(Byte dataPointer) {dataPointer_ = dataPointer;};
	Byte getInterruptEnable() {return interruptEnable_;};
	void setInterruptEnable(Byte interruptEnable) {interruptEnable_ = interruptEnable;};
	Byte getEfFlags() {return efFlags_;};
	void setCycle0() {cycle0_ = 1;};
	long getCpuCycles() {return cpuCycles_;};
	long getSteps(){return steps_;};
	void setSteps(long steps);
	void setTraceStatus(bool trace) {trace_ = trace;};
	void setDmaTraceStatus(bool traceDma) {traceDma_ = traceDma;};
	void setIntTraceStatus(bool traceInt) {traceInt_ = traceInt;};
	void setChip8IntTraceStatus(bool traceChip8Int) {traceChip8Int_ = traceChip8Int;};
	Word getOutValue(int number) {return outValues_[number];};
	void setOutValue(int number, Byte outValue) {outValues_[number] = outValue;};
	Byte getInValue(int number) {return inValues_[number];};
	void setInValue(int number, Byte inValue) {inValues_[number] = inValue;};
	void setDebugMode (bool debugModeNew, bool debugChip8ModeNew, bool trace, bool traceDma, bool traceInt, bool traceChip8Int);
	void debugTrace (wxString text);
	Byte getCpuMode() {return cpuMode_;};
	void useColour(int colourMask) {colourMask_ = colourMask;};
	Byte get1805Counter() {return counter_;};
	Byte get1805Ch() {return ch_;};
	Byte is1805CtrRunning() {return ctrRunning_;};
    bool getSkipTraceMode() {return skipTrace_;};

	void writeMemLabelType(Word address, Byte type);
	Byte readMemLabelType(Word address);

protected:
	Byte cycle0_;
	Byte cpuMode_;

	// Debug data
	long steps_;
	Word outValues_[8];
	Byte inValues_[8];
	bool debugMode_;
	bool chip8DebugMode_;

	// 1802 CPU Registers
	Word scratchpadRegister_[16];
	Byte programCounter_;
    Byte scrtProgramCounter_;

	int expansionSlot_;
	int ramBank_;
	int epromBank_;
	int ramSwitched_;
	int epromSlot_;
	int superSlot_;
    int fdcSlot_;
    int networkSlot_;

    ElfConfiguration elfConfiguration;
  
    Word ramStart_;
    int ramMask_;
    bool disableSystemRom_;
    int bootstrap_;
    Word addressLatch_;
    Word setLatch_;
    Word romMask_;
    
private:
	void setMode();
	void decCounter();
	void inst1805(wxString *tr);
	int colourMask_;
	bool readyToReceiveData[4];

	// 1802 CPU Registers
	Byte dataFlag_;
	Byte dataPointer_;
	Byte registerT_;
	Byte interruptEnable_;
	Byte efFlags_;
	Byte accumulator_;

	int clear_;
	int wait_;
	Byte idle_;
	bool trace_;
    bool skipTrace_;
	bool traceDma_;
	bool traceInt_;
	bool traceChip8Int_;
	int cpuType_;

	// 1805 stuff
	Byte tq_;
	Byte ci_;
	Byte xi_;
	Byte cie_;
	Byte xie_;
	Byte counter_;
	Byte ch_;
	Byte ctrMode_;
	Byte ctrRunning_;
	Byte ctrPre_;
};

#endif	// CDP1802_H
