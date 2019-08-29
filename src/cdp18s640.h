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
    Cdp18s640Screen(wxWindow *parent, const wxSize& size);
    ~Cdp18s640Screen();
    
    void init();
    void onPaint(wxPaintEvent&event);
    void onMousePress(wxMouseEvent& event);
    void onMouseRelease(wxMouseEvent& event);
    void releaseButtonOnScreen(HexButton* buttonPointer);
private:
};

#endif  // CDP18S640_H
