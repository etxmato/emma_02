#ifndef PIO_H
#define PIO_H

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

class PioScreen : public Panel
{
public:
    PioScreen(wxWindow *parent, const wxSize& size);
    ~PioScreen();
    
    void init();
    void onPaint(wxPaintEvent&event);
    void onMousePress(wxMouseEvent& event);
    void onMouseRelease(wxMouseEvent& event);
    void releaseButtonOnScreen(HexButton* buttonPointer);
   
    void interruptCycle();
    void onArdyButton();
    void onBrdyButton();

    void ioSwitch(int i);

    void setProgBitsA(wxDC& dc, Byte value);
    void setProgBitsB(wxDC& dc, Byte value);
    void enableStbRdyA(wxDC& dc);
    void enableStbRdyB(wxDC& dc);
    void disableStbRdyA(wxDC& dc);
    void disableStbRdyB(wxDC& dc);
    void enableInterruptButton(wxDC& dc);

    void reset();
    void clearA();
    void clearB();
    void clearStbRdy();
    void writeControlRegister(Byte value);
    void writePortA(Byte value);
    void writePortB(Byte value);
    Byte readPortA();
    Byte readPortB();
    Byte readStatusRegister();
    void maskInterruptA();
    void maskInterruptB();
    void refreshLeds();
    
private:
    int pioAMode_;
    int pioBMode_;
    int pioStbAMode_;
    int pioStbBMode_;
    int pioRdyAMode_;
    int pioRdyBMode_;
    bool pioAInterruptEnabled_;
    bool pioBInterruptEnabled_;
    Byte pioAProgBits_;
    Byte pioBProgBits_;
    Byte pioStrobe_;
    Byte pioAInterruptMask_;
    Byte pioBInterruptMask_;
    Byte pioInterruptCommand_;
    Byte pioAInterruptLogical_;
    Byte pioBInterruptLogical_;
    Byte pioStatus_;
    int commandByteNumber_;

    Byte outPutValueA_;
    Byte outPutValueB_;
    Byte inPutValueA_;

    bool pioAInterruptButtonEnabled_;
    bool pioBInterruptButtonEnabled_;
    bool pioAInterruptButtonState_;
    bool pioBInterruptButtonState_;

    int ioSwitchState_[20];
    bool ioSwitchEnabled_[20];
};

class PioFrame : public wxFrame
{
public:
    PioFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~PioFrame();
  
	void onClose(wxCloseEvent& event);

    void writeControlRegister(Byte value) {pioScreenPointer->writeControlRegister(value);};
    void writePortA(Byte value) {pioScreenPointer->writePortA(value);};
    void writePortB(Byte value) {pioScreenPointer->writePortB(value);};
    Byte readPortA() {return pioScreenPointer->readPortA();};
    Byte readPortB() {return pioScreenPointer->readPortB();};
    Byte readStatusRegister() {return pioScreenPointer->readStatusRegister();};
    void reset() {pioScreenPointer->reset();};
    void ledTimeout() {pioScreenPointer->ledTimeout();};
    void interruptCycle() {pioScreenPointer->interruptCycle();};
    void setLedMs(long ms) {pioScreenPointer->setLedMs(ms);};
    void refreshLeds() {pioScreenPointer->refreshLeds();};
    void onArdyButton(wxCommandEvent&event);
    void onBrdyButton(wxCommandEvent&event);
    void releaseButtonOnScreen(HexButton* buttonPoint) {pioScreenPointer->releaseButtonOnScreen(buttonPoint);};
    
private:
    class PioScreen *pioScreenPointer;

	DECLARE_EVENT_TABLE()
};


#endif  // PIO_H
