#ifndef MEMBER_H
#define MEMBER_H

#include <wx/dcclient.h> 

#include "elf.h"
#include "tms9918.h"
#include "pixie.h"
#include "mc6847.h"
#include "mc6845.h"
#include "i8275.h"
#include "keypad.h"
#include "ledmodule.h"
#include "printer.h"
#include "elfconfiguration.h"

class MemberScreen : public Panel
{
public:
    MemberScreen(wxWindow *parent, const wxSize& size);
    ~MemberScreen();

    void init(int front);
    void onPaint(wxPaintEvent&event);
    void onMousePress(wxMouseEvent& event);
    void onMouseRelease(wxMouseEvent& event);

private:
};

class Membership : public wxFrame,  public MainElf
{
public:
    Membership(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf, Conf computerConf);
    Membership() {};
    ~Membership();

    void onClose(wxCloseEvent&WXUNUSED(event));
    bool keyDownPressed(int keycode);
    bool keyUpReleased(int key, wxKeyEvent& event);

    void onRun();
    void onInButtonPress();
    void onInButtonPress(Byte value);
    void onInButtonRelease();

    void configureComputer();
    void initComputer();
    Byte ef(int flag);
    Byte in(Byte port, Word address);
    Byte getData();
    void out(Byte port, Word address, Byte value);
    void showData(Byte value);
    void cycle(int type);
    void cycleLed();

    void autoBoot();
    void switchQ(int value);
    int getMpButtonState();
    void onMpButton();
    void onWaitButton();
    void onClearButton();
    void dataSwitch(int i);

    void startComputer();
    void writeMemDataType(Word address, Byte type);
    Byte readMemDataType(Word address, uint64_t* executed);
    Byte readMem(Word address);
    void writeMem(Word address, Byte value, bool writeRom);
    Byte readMemDebug(Word address);
    void writeMemDebug(Word address, Byte value, bool writeRom);
    void cpuInstruction();
    void resetPressed();
    void configureExtensions();
    void moveWindows();
    void updateTitle(wxString Title);
    void setForceUpperCase(bool status);
    void onReset();
    void saveRam();
    void loadRam();
    void sleepComputer(long ms);
    void setLedMs(long ms);
    Byte getKey(Byte vtOut);
    void activateMainWindow();
    void setGreenLed(int status);

    void terminalSave(wxString fileName, int protocol);
    void terminalLoad(wxString filePath, wxString fileName, int protocol);
    void terminalStop();
    void refreshPanel();

private:
    class MemberScreen *memberScreenPointer;

    int ledCycleValue_;
    int ledCycleSize_;

    int waitButtonState_;
    int clearButtonState_;
    int mpButtonState_;
    int dataSwitchState_[8];
    int ef4SwitchState;
    bool inPressed_;

    double clockSpeed_;
    Word ramEnd_;

    int qLedStatus_;
    DECLARE_EVENT_TABLE()
};

#endif  // MEMBER_H
