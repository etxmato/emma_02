/*
 *******************************************************************
 *** This software is copyright 2008 by Marcel van Tongeren      ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

#if !wxUSE_COMBOCTRL
    #error "Please set wxUSE_COMBOCTRL to 1 and rebuild the library."
#endif

#include "wx/xrc/xmlres.h"
#include "wx/spinctrl.h"

#include "main.h"
#include "guivip2.h"
#include "pixie.h"

#include "psave.h"

DEFINE_EVENT_TYPE(STATUS_BAR_PIXIE)

BEGIN_EVENT_TABLE(GuiVipII, XmlParser)
    EVT_COMMAND(wxID_ANY, STATUS_BAR_PIXIE, GuiVipII::pixieBarSize)

END_EVENT_TABLE()

GuiVipII::GuiVipII(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: XmlParser(title, pos, size, mode, dataDir, iniDir)
{
}

void GuiVipII::pixieBarSize(wxCommandEvent&WXUNUSED(event))
{
    p_Video[computerConfiguration.cdp1861Configuration.videoNumber]->pixieBarSize();
}

void GuiVipII::pixieBarSizeEvent()
{
    if (computerConfiguration.cdp1861Configuration.statusBarType != STATUSBAR_VIP2)
        return;

    wxCommandEvent event(STATUS_BAR_PIXIE, 810);
    event.SetEventObject(this);
    wxPostEvent(this, event);
}
