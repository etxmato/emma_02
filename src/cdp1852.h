#ifndef CDP1852_H
#define CDP1852_H

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

class Cdp1852Screen : public Panel
{
public:
    Cdp1852Screen(wxWindow *parent, const wxSize& size);
    ~Cdp1852Screen();
    
    void init();
    void onPaint(wxPaintEvent&event);
    void onMousePress(wxMouseEvent& event);
    void onMouseRelease(wxMouseEvent& event);
    void releaseButtonOnScreen(HexButton* buttonPointer);
   
    void onStbButton();

    void ioSwitch(int i);

    void reset();
    void writePort(Byte value);
    Byte readPort();
    void refreshLeds();
    
private:
    Byte outPutValue_;
    Byte inPutValue_;

    int ioSwitchState_[8];
};

class Cdp1852Frame : public wxFrame
{
public:
    Cdp1852Frame(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~Cdp1852Frame();
  
    void onClose(wxCloseEvent& event);

    void writePort(Byte value) {cdp1852ScreenPointer->writePort(value);};
    Byte readPort() {return cdp1852ScreenPointer->readPort();};
    void reset() {cdp1852ScreenPointer->reset();};
    void ledTimeout() {cdp1852ScreenPointer->ledTimeout();};
    void setLedMs(long ms) {cdp1852ScreenPointer->setLedMs(ms);};
    void refreshLeds() {cdp1852ScreenPointer->refreshLeds();};
    void onStbButton(wxCommandEvent&event);
    void releaseButtonOnScreen(HexButton* buttonPoint) {cdp1852ScreenPointer->releaseButtonOnScreen(buttonPoint);};
    
private:
    class Cdp1852Screen *cdp1852ScreenPointer;

    DECLARE_EVENT_TABLE()
};


#endif  // CDP1852_H
