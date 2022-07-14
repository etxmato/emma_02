#ifndef LEDMODULE_H
#define LEDMODULE_H

#include "led.h"

class LedModuleScreen: public Panel
{
public:
    LedModuleScreen(wxWindow *parent, const wxSize& size);
    ~LedModuleScreen();

    void init(int computerType);
    void onPaint(wxPaintEvent&event);
private:
};

class LedModule : public wxFrame
{
public:
    LedModule(const wxString& title, const wxPoint& pos, const wxSize& size, int computerType);
    ~LedModule();

    void onClose(wxCloseEvent&WXUNUSED(event));

    void configure(ElfPortConfiguration elfPortConf);
    void write(Byte value); 
    void ledTimeout();
    void setLedMs(long ms);
    Byte getKey(Byte vtOut);
    void refreshPanel();

private:
    class LedModuleScreen *ledModuleScreenPointer;

    DECLARE_EVENT_TABLE()
};

#endif  // LEDMODULE_H
