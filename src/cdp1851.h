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
    Cdp1851Printer(int pioNumber, Cdp1851Configuration cdp1851Configuration);
    ~Cdp1851Printer();

    void init(Cdp1851Instance *cdp1851InstancePointer);
    void writePortA(Byte value);
    void strobe(int pioStbMode);
    Byte readStatusRegister(Byte pioStatus);

private:
    int pioNumber_;
    Byte printLatch_;
    Byte printerBusy_;
    
    Cdp1851Configuration cdp1851Configuration_;
    Cdp1851Instance *cdp1851InstancePointer_;
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
    Cdp1851Instance(int pioNumber, Cdp1851Configuration cdp1851Configuration);
    ~Cdp1851Instance();
  
    void onClose(wxCloseEvent& event);

    void init();
    void writeControlRegister(Byte value);
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
    Byte getPioAMode() {return pioAMode_;};
    Byte getPioBMode() {return pioBMode_;};
    Byte getPioStbAMode() {return pioStbAMode_;};
    Byte getPioStbBMode() {return pioStbBMode_;};
    Byte getPioRdyAMode() {return pioRdyAMode_;};
    Byte getPioRdyBMode() {return pioRdyBMode_;};
    Byte getPioAProgBits() {return pioAProgBits_;};
    Byte getPioBProgBits() {return pioBProgBits_;};
    void setPioEfState1(bool state) {pioEfState1_ = state;};
    void setPioEfState2(bool state) {pioEfState2_ = state;};
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
    Byte pioEfState1_;
    Byte pioEfState2_;

    int pioAMode_;
    int pioBMode_;
    int pioStbAMode_;
    int pioStbBMode_;
    int pioRdyAMode_;
    int pioRdyBMode_;
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
