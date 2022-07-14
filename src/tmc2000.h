#ifndef TELMAC2000_H
#define TELMAC2000_H

#include "cdp1802.h"
#include "pixie.h"

class Tmc2000 : public Cdp1802, public Pixie
{
public:
    Tmc2000(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType, Conf computerConf);
    ~Tmc2000();

    void configureComputer();
    void reDefineKeysA(int *, int *);
    void initComputer();
    void keyDown(int keycode);
    void keyUp(int keycode);

    void onRun();

    Byte ef(int flag);
    Byte ef3();
    Byte in(Byte port, Word address);
    void out(Byte port, Word address, Byte value);
    void outTMC2000(Byte value);
    void cycle(int type);

    void startComputer();
    void writeMemDataType(Word address, Byte type);
    Byte readMemDataType(Word address, uint64_t* executed);
    Byte readMem(Word address);
    void writeMem(Word address, Byte value, bool writeRom);
    Byte readMemDebug(Word address);
    void writeMemDebug(Word address, Byte value, bool writeRom);
    void cpuInstruction();
    void resetPressed();
    void onReset();
    void checkComputerFunction();

    Byte read1864ColorDirect(Word address);
    void write1864ColorDirect(Word address, Byte value);

private:
    Byte telmac2000KeyPort_;
    Byte telmac2000KeyState_[64];
    bool runPressed_;
    bool colorLatch_;

    int hexKeyDefA1_[64];
    int hexKeyDefA2_[16];
    int hexKeyDefB1_[16];
    int hexKeyDefB2_[16];
    int inKey1_;
    int inKey2_;

    bool simDefA2_;
    bool simDefB2_;

    int keyDefGameHexA_[5];
    int keyDefGameHexB_[5];
};

#endif  // TELMAC2000_H
