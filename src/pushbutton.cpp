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

#include "pushbutton.h"
#include <wx/event.h>

DEFINE_EVENT_TYPE(wxEVT_ButtonDownEvent) 
DEFINE_EVENT_TYPE(wxEVT_ButtonUpEvent) 
 
BEGIN_EVENT_TABLE(PushBitmapButton, wxButton)
  EVT_LEFT_DOWN(PushBitmapButton::onLeftMouseDown)
  EVT_LEFT_UP(PushBitmapButton::onLeftMouseUp)
END_EVENT_TABLE()

PushBitmapButton::PushBitmapButton(wxWindow* parent, wxWindowID id, const wxBitmap& bitmap, const wxPoint& pos, const wxSize& size, long style)
: wxBitmapButton(parent, id, bitmap, pos, size, style)
{
}

PushBitmapButton::~PushBitmapButton()
{
}

void PushBitmapButton::onLeftMouseDown(wxMouseEvent& pushEvent)
{
    wxCommandEvent event(wxEVT_ButtonDownEvent, GetId());
    wxPostEvent(GetParent(), event);
    pushEvent.Skip(true);
}

void PushBitmapButton::onLeftMouseUp(wxMouseEvent& pushEvent)
{
    wxCommandEvent event(wxEVT_ButtonUpEvent, GetId());
    wxPostEvent(GetParent(), event);
    pushEvent.Skip(true);
}

BEGIN_EVENT_TABLE(PushButton, wxButton) 
  EVT_LEFT_DOWN(PushButton::onLeftMouseDown) 
  EVT_LEFT_UP(PushButton::onLeftMouseUp) 
END_EVENT_TABLE() 

PushButton::PushButton(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos, const wxSize& size, long style)
: wxButton(parent, id, label, pos, size, style) 
{
} 

PushButton::~PushButton()
{
} 

void PushButton::onLeftMouseDown(wxMouseEvent& pushEvent)
{ 
    wxCommandEvent event(wxEVT_ButtonDownEvent, GetId()); 
    wxPostEvent(GetParent(), event); 
    pushEvent.Skip(true); 
} 

void PushButton::onLeftMouseUp(wxMouseEvent& pushEvent)
{ 
    wxCommandEvent event(wxEVT_ButtonUpEvent, GetId()); 
    wxPostEvent(GetParent(), event); 
    pushEvent.Skip(true); 
} 
 
