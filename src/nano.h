#ifndef NANO_H
#define NANO_H

#include "cdp1802.h"
#include "pixie.h"

class Nano : public Cdp1802, public Pixie
{
public:
    Nano(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType, Conf computerConf);
    ~Nano();

    void configureComputer();
    void reDefineKeysA(int *, int *);
    void initComputer();
    void keyDown(int keycode);
    void keyUp(int keycode);

    Byte ef(int flag);
    Byte ef3();
    Byte in(Byte port, Word address);
    void out(Byte port, Word address, Byte value);
    void outNano(Byte value);
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

private:
    Byte NanoKeyPort_;
    Byte NanoKeyState_[64];
    int tabPressed_;

    int hexKeyDefA1_[16];
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

#endif  // NANO_H
