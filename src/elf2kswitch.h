#ifndef ELF2KSWITCH_H
#define ELF2KSWITCH_H

#include "elf2kdisk.h"

class Elf2KswitchScreen: public Panel
{
public:
    Elf2KswitchScreen(wxWindow *parent, const wxSize& size, int tilType);
    ~Elf2KswitchScreen();

    void init();
    void onPaint(wxPaintEvent&event);
    void onMousePress(wxMouseEvent& event);
    void onMouseRelease(wxMouseEvent& event);

private:
};

class Elf2Kswitch : public wxFrame 
{
public:
    Elf2Kswitch(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~Elf2Kswitch();

    void onClose(wxCloseEvent&WXUNUSED(event));

    void setRunButtonUp(int up);
    void setInButtonUp(bool up);
    void setLoadButtonUp(int up);
    void setMpButtonUp(int up);
    void setDataSwitchUp(int number, int up);

    Byte getKey(Byte vtOut);

private:
    class Elf2KswitchScreen *elf2KswitchScreenPointer;

    DECLARE_EVENT_TABLE()
};

#endif  // ELF2KSWITCH_H
