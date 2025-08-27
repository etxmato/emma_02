#ifndef CDP1851_H
#define CDP1851_H

#define PIO_INPUT 0
#define PIO_OUTPUT 1
#define PIO_BIT_PROG 2
#define PIO_BI_DRECT 3

#define PIO_COMMAND_NONE 0
#define PIO_COMMAND_BITPROGA 1
#define PIO_COMMAND_BITPROGB 2
#define PIO_COMMAND_BITPROGAB 3
#define PIO_COMMAND_INT 4

#define PIO_STB_A 16
#define PIO_RDY_A 17
#define PIO_STB_B 18
#define PIO_RDY_B 19

class Cdp1851Instance;

class Cdp1851Printer
{
public:
    Cdp1851Printer(int pioNumber, Cdp1851Configuration cdp1851Configuration, Cdp1851PrinterConfiguration cdp1851PrinterConfiguration, double clock);
    ~Cdp1851Printer();
    
    void init(Cdp1851Instance *cdp1851InstancePointer);
    void cycle();
    void initPort(int port, Byte value);
    void writePort(int port, Byte value);
    Byte readPort(int port);
    void rdyControl(int port, Byte value, int rdyMode);
    void strobeControl(int port, Byte value, int strobeMode);
    void initIo(int port, Byte value);
    void selectOutIo(int port, Byte value);
    void strobeIo(int port, Byte value);
    void autoLfIo(int port, Byte value);
    Byte selectInIo(int port, Byte value);
    Byte errorIo(int port, Byte value);
    Byte paperOutIo(int port, Byte value);
    Byte busyIo(int port, Byte value);
    Byte ackIo(int port, Byte value);
    void init(Byte value);
    void selectOut(Byte value);
    void strobe(Byte value);
    void autoLf(Byte value);
    Byte readStatusRegister(Byte pioStatus);
    Byte setMask(Byte mask, Byte active);
    Byte resetMask(Byte mask, Byte active);

private:
    int pioNumber_;
    Byte printLatch_;
    
    Byte printerBusyMask_;
    Byte printerAckMask_;
    Byte selectInMask_;
    Byte errorMask_;
    Byte paperOutMask_;

    Byte printerBusy_;
    Byte printerAck_;
    Byte select_in_;
    Byte error_;
    Byte paper_out_;
    
    bool auto_feed_;

    Cdp1851Configuration cdp1851Configuration_;
    Cdp1851PrinterConfiguration cdp1851PrinterConfiguration_;
    Cdp1851Instance *cdp1851InstancePointer_;
    
    Byte portValue[2];
    int busyCycleValue_;
    int busyCycleSize_;
    int ackCycleValue_;
    int ackCycleSize_;
};

class Cdp1851Screen : public Panel
{
public:
    Cdp1851Screen(wxWindow *parent, const wxSize& size, int pioNumber, Cdp1851Configuration cdp1851Configuration);
    ~Cdp1851Screen();
    
    void init(Cdp1851Instance *cdp1851InstancePointer);
    void onPaint(wxPaintEvent&event);
    void onMousePress(wxMouseEvent& event);
    void onMouseRelease(wxMouseEvent& event);
    void releaseButtonOnScreen(HexButton* buttonPointer);
    
    void onArdyButton();
    void onBrdyButton();
    
    void ioSwitch(int i);
    
    void setProgBitsA(Byte value);
    void setProgBitsB(Byte value);
    void enableStbRdy(int port, Byte pioStatus, int pioStbMode, int pioRdyMode);
    void enableStbRdyA(Byte pioStatus, int pioStbAMode, int pioRdyAMode);
    void enableStbRdyB(Byte pioStatus, int pioStbBMode, int pioRdyBMode);
    void disableStbRdyA(wxDC& dc);
    void disableStbRdyB(wxDC& dc);
    void enableInterruptButton(wxDC& dc);
    
    void reset();
    void clearA();
    void clearB();
    void clearStbRdy();
    void writeControlRegister(Byte value, int commandByteNumber);
    void writePortA(Byte value);
    void writePortB(Byte value);
    Byte readStatusRegister(Byte pioStatus);
    void refreshLeds();

private:
    int pioNumber_;
    Cdp1851Configuration cdp1851Configuration_;
    Cdp1851Instance *cdp1851InstancePointer_;

    Byte pioStrobe_;
    
    bool pioAInterruptButtonEnabled_;
    bool pioBInterruptButtonEnabled_;
    bool pioAInterruptButtonState_;
    bool pioBInterruptButtonState_;
    
    int ioSwitchState_[20];
    bool ioSwitchEnabled_[20];
};

class Cdp1851Instance : public wxFrame
{
public:
    Cdp1851Instance(const wxString& title, const wxPoint& pos, const wxSize& size, int pioNumber, Cdp1851Configuration cdp1851Configuration);
    Cdp1851Instance(int pioNumber, Cdp1851Configuration cdp1851Configuration, Cdp1851PrinterConfiguration cdp1851PrinterConfiguration, double clock);
    ~Cdp1851Instance();
  
    void onClose(wxCloseEvent& event);

    void init();
    void writeControlRegister(Byte value);
    void strobeRdyControl(Byte value);
    void setProgBitsA(Byte value);
    void setProgBitsB(Byte value);
    void onRdyA();
    void onRdyB();
    void writePortA(Byte value);
    void writePortB(Byte value);
    Byte readPortA();
    Byte readPortB();
    void maskInterruptA();
    void maskInterruptB();
    Byte readStatusRegister();
    void reset();
    void ledTimeout();
    void interruptCycle();
    void setLedMs(long ms);
    void refreshLeds();
    void onArdyButton(wxCommandEvent&event);
    void onBrdyButton(wxCommandEvent&event);
    void releaseButtonOnScreen(HexButton* buttonPoint);
    void refreshPanel();
    Byte getEfState1();
    Byte getEfState2();
    Byte getIrqState();
   
    void clearA();
    void clearB();
    void clearStbRdy();

    void setOutputValueA(int bit, int value);
    void setOutputValueB(int bit, int value);

    Byte getOutputValueA() {return outPutValueA_;};
    Byte getInputValueA() {return inPutValueA_;};
    Byte getOutputValueB() {return outPutValueB_;};
    Byte getPioAMode() {return modeSet[0];};
    Byte getPioBMode() {return modeSet[1];};
    Byte getPioStbAMode() {return strobeMode[0];};
    Byte getPioStbBMode() {return strobeMode[1];};
    Byte getPioRdyAMode() {return rdyMode[0];};
    Byte getPioRdyBMode() {return rdyMode[1];};
    Byte getPioAProgBits() {return pioAProgBits_;};
    Byte getPioBProgBits() {return pioBProgBits_;};
    void setPioEfState(int port, Byte state) {pioEfState[port] = state;};
    bool getPioAInterruptEnabled() {return pioAInterruptEnabled_;};
    bool getPioBInterruptEnabled() {return pioBInterruptEnabled_;};

    int connection_;
    
private:
    class Cdp1851Screen *pioScreenPointer;
    class Cdp1851Printer *pioPrinterPointer;

    int pioNumber_;
    Cdp1851Configuration cdp1851Configuration_;

    Byte outPutValueA_;
    Byte outPutValueB_;
    Byte inPutValueA_;
    Byte pioEfState[2];

    int modeSet[2];
    int strobeMode[2];
    int rdyMode[2];
    Byte pioAInterruptMask_;
    Byte pioBInterruptMask_;
    Byte pioStatus_;

    Byte pioAProgBits_;
    Byte pioBProgBits_;

    bool pioAInterruptEnabled_;
    bool pioBInterruptEnabled_;
    Byte pioInterruptCommand_;
    Byte pioAInterruptLogical_;
    Byte pioBInterruptLogical_;
    int commandByteNumber_;

    DECLARE_EVENT_TABLE()
};


#endif  // CDP1851
