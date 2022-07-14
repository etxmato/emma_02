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

#define STATE_FETCH 0
#define STATE_EXECUTE 1
#define STATE_DMA 2
#define STATE_INTERRUPT 3

#define STATE_FETCH_1 5 // This should always be 1 lower than STATE_EXECUTE_2_1805
#define STATE_FETCH_2 0
#define STATE_EXECUTE_1 1
#define STATE_EXECUTE_1_LBR 2
#define STATE_EXECUTE_2_LBR 3
#define STATE_EXECUTE_1_1805 4
#define STATE_EXECUTE_2_1805 6
#define STATE_EXECUTE_2_1805 6
#define STATE_EXECUTE_3_1805 7
#define STATE_EXECUTE_4_1805 8
#define STATE_EXECUTE_5_1805 9
#define STATE_EXECUTE_6_1805 10
#define STATE_EXECUTE_7_1805 11
#define STATE_EXECUTE_8_1805 12

#define INT_STD 0
#define INT_PIXIE 1

/*
#define DMA_IN 0
#define DMA_OUT 1
#define DMA_PIXIE 2
#define DMA_VISICOM 3
#define DMA_COLOR 4*/

class Cdp1802 : public IoDevice, public Memory, public Sound
{
public:
    Cdp1802();
    virtual ~Cdp1802() {};

    void initCpu(int computerType);
    void resetCpu();
    void resetEffectiveClock();

    void machineCycle();
    virtual void cpuInstruction() = 0;

    void setClear(int value);
    int getClear() {return clear_;};
    void setWait(int value);
    int getWait() {return wait_;};
    void cpuCycleStep();
    void singleStateStep();
    void cpuCycleFetch();
    void cpuCycleFetch2();
    void cpuCycleExecute1();
    void cpuCycleExecute1_LBR();
    void cpuCycleExecute2_LBR();
    void cpuCycleFinalize();
    void cpuCycleExecute1_1805();
    void cpuCycleExecute2_1805();
    void dmaIn(Byte value);
    Byte dmaOut();
    Byte pixieDmaOut(int *color);
    void visicomDmaOut(Byte *vram1, Byte *vram2);
    Byte pixieDmaOut();

    void interrupt();
    void requestInterrupt();
    void pixieInterrupt();
    void setEf(int flag, int value);

    bool readIntelFile(wxString fileName, int memoryType, long end, long inhibitStart, long inhibitEnd);
    bool readIntelFile(wxString fileName, int memoryType, long end, bool showFilename);
    bool readIntelFile(wxString fileName, int memoryType, Word* lastAddress, long end, bool showFilename);
    bool readLstFile(wxString fileName, int memoryType, long end, bool showFilename);
    void saveIntelFile(wxString fileName, long start, long end);
    void saveBinFile(wxString fileName, long start, long end);
    bool readBinFile(wxString fileName, int memoryType, Word start, long end, long inhibitStart, long inhibitEnd);
    bool readBinFile(wxString fileName, int memoryType, Word address, long end, bool showFilename, bool showAddressPopup, Word specifiedStartAddress);
    bool readBinFile(wxString fileName, int memoryType, Word address, Word* lastAddress, long end, bool showFilename);
    bool readRomMapperBinFile(size_t emsNumber, wxString fileName);
    bool readMultiCartBinFile(wxString dirName, wxString fileName);
    void setAddress(bool showFilename, Word start, Word end);
    void checkLoadedSoftware();
    bool readProgram(wxString romDir, wxString rom, int memoryType, Word address, bool showFilename);
    bool readProgram(wxString romDir, wxString rom, int memoryType, Word address, Word* lastAddress, bool showFilename);
    bool readProgramMicro(wxString romDir, wxString rom, int memoryType, Word address, long lastAddress, bool showFilename);
    bool readProgramMicro(wxString romDir, wxString rom, int memoryType1, int memoryType2, long address, long lastAddress, long inhibitstart, long inhibitEnd_);
    bool readProgramCidelsa(wxString romDir, wxString rom, int memoryType, Word address, bool showFilename);
    bool readProgramTmc600(wxString romDir, wxString rom, int memoryType, Word address, bool showFilename);
    bool readProgramPecom(wxString romDir, wxString rom, int memoryType, Word address, bool showFilename);
    void readSt2Program(int computerType, int memoryType);
    bool readFile(wxString fileName, int memoryType, Word address, long end, long inhibitStart, long inhibitEnd);
    bool readFile(wxString fileName, int memoryType, Word address, long end, bool showFilename);
    bool readFile(wxString fileName, int memoryType, Word address, long end, bool showFilename, bool showAddressPopup, Word specifiedStartAddress);
    bool readFile(wxString fileName, int memoryType, Word address, Word* lastAddress, long end, bool showFilename);

    Word getScratchpadRegister(int number) {return scratchpadRegister_[number];};
    void setScratchpadRegister(int number, Word scratchpadRegister) {scratchpadRegister_[number] = scratchpadRegister;};
    Word getAndIncRegister0() {return scratchpadRegister_[0]++;};
    Byte getAccumulator() {return accumulator_;};
    void setAccumulator(Byte accumulator) {accumulator_ = accumulator;};
    Byte getDataFlag() {return dataFlag_;};
    void setDataFlag(Byte dataFlag) {dataFlag_ = dataFlag;};
    Byte getRegisterT() {return registerT_;};
    void setRegisterT(Byte registerT) {registerT_ = registerT;};
    Byte getRegisterB() {return registerB_;};
    void setRegisterB(Byte registerB) {registerB_ = registerB;};
    Byte getCounterJamValue() {return ch_;};
    void setCounterJamValue(Byte ch) {ch_ = ch;};
    Byte getCounterTimer() {return counter_;};
    void setCounterTimer(Byte counter) {counter_ = counter;};
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
    Byte getCounterInterruptEnable() {return cie_;};
    void setCounterInterruptEnable(Byte cie) {cie_ = cie;};
    Byte getEfFlags() {return efFlags_;};
    void setCycle0() {cycle0_ = 1;};
    uint64_t getCpuCycles() {return cpuCycles_;};
    uint64_t getInstructionCounter() {return instructionCounter_;};
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

    virtual void setHeaderTitle(const wxString& title);
    virtual void updateTitle(wxString Title);

    void increaseExecutedMainMemory(long address, Byte type);
    void increaseExecutedExpansionRom(long address, Byte type);
    void increaseExecutedExpansionRam(Word address, Byte type);
    void increaseExecutedExpansionEprom(Word address, Byte type);
    void increaseExecutedExpansionSuper(Word address, Byte type);
    void increaseExecutedCpuRam(Word address, Byte type);
    void increaseExecutedMultiCartRom(Word address, Byte type);
    void increaseExecutedTestCartRom(Word address, Byte type);
    void increaseExecutedEms(size_t emsNumber, long address, Byte type);
    void clearProfiler();

protected:
    Byte cycle0_;
    Byte cpuMode_;
    Byte cpuState_;

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

    Byte idle_;
    int cpuType_;
    bool singleStateStep_;

    Byte bus_;
    Byte instructionCode_;
    wxString traceBuffer_;
    bool stopHiddenTrace_;
    bool startHiddenTrace_;

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
    
    bool interruptRequested_;
  
private:
    void setMode();
    void decCounter();
    int colourMask_;
    bool readyToReceiveData[4];

    // 1802 CPU Registers
    Byte dataFlag_;
    Byte dataPointer_;
    Byte registerB_;
    Byte registerT_;
    Byte interruptEnable_;
    Byte efFlags_;
    Byte accumulator_;

    int clear_;
    int wait_;
    bool trace_;
    bool skipTrace_;
    bool traceDma_;
    bool traceInt_;
    bool traceChip8Int_;
    
    int interruptType_;
    int dmaType_;

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

#endif    // CDP1802_H
