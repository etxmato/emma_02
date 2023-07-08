#ifndef CDP18S640_H
#define CDP18S640_H

#include "cdp1802.h"
#include "pixie.h"
#include "keyboard.h"
#include "elfconfiguration.h"
#include "vt100.h"

#define SC0LED 0
#define SC1LED 1
#define WAITLED 2
#define CLEARLED 3

class Cdp18s640Screen : public Panel
{
public:
    Cdp18s640Screen(wxWindow *parent, const wxSize& size, int tilType);
    ~Cdp18s640Screen();
    
    void init();
    void onPaint(wxPaintEvent&event);
    void onMousePress(wxMouseEvent& event);
    void onMouseRelease(wxMouseEvent& event);
    void releaseButtonOnScreen(HexButton* buttonPointer);
private:
};

class Cdp18s640Frame : public wxFrame
{
public:
    Cdp18s640Frame(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~Cdp18s640Frame();
    
    void onClose(wxCloseEvent& event);
  
    void onRunButton(wxCommandEvent&event);
    void onRunPButton(wxCommandEvent&event);
    void onResetButton(wxCommandEvent&event);

    void init() {cdp18s640ScreenPointer->init();};
    void releaseButtonOnScreen(HexButton* buttonPoint) {cdp18s640ScreenPointer->releaseButtonOnScreen(buttonPoint);};

    void setQLed(int status) {cdp18s640ScreenPointer->setQLed(status);};
    void showDataTil313Italic(Byte value) {cdp18s640ScreenPointer->showData(value);};
    void showAddressTil313Italic(Word address) {cdp18s640ScreenPointer->showAddress(address);};
    void setStateLed(int i, int status) {cdp18s640ScreenPointer->setStateLed(i, status);};
    void setRunLed(int status) {cdp18s640ScreenPointer->setRunLed(status);};
    void ledTimeout() {cdp18s640ScreenPointer->ledTimeout();};
    void setLedMs(long ms) {cdp18s640ScreenPointer->setLedMs(ms);};
    void refreshPanel();

private:
    class Cdp18s640Screen *cdp18s640ScreenPointer;
    
    DECLARE_EVENT_TABLE()
};

#endif  // CDP18S640_H
