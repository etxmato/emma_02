////////////////////////////////////////////////////////////////////////////////
// xrc_compat.h - wxWidgets XRC id compatibility shim
////////////////////////////////////////////////////////////////////////////////
// Some wx builds provide no implicit wxString -> const char* conversion:
// wx < 3.2 never has it, and wx 3.2 built with wxUSE_STD_STRING_CONV_IN_WXSTRING
// (e.g. openSUSE) disables it as well. On those builds the stock XRCCTRL()
// macro (whose id goes through XRCID() -> DoGetXRCID(const char*)) rejects the
// dynamic wxString ids used throughout the GUI code. GetXRCID() takes a
// wxString and exists in every wx version we build against (3.0, 3.1 and 3.2),
// on every platform.
//
// XRCCTRL at these call sites is always a runtime window lookup, never an event
// table, so resolving the id through GetXRCID() is safe. XRCID() itself is left
// alone so event tables keep using its compile-time constant.
//
// Include this after wx/xrc/xmlres.h.
////////////////////////////////////////////////////////////////////////////////
#ifndef XRC_COMPAT_H
#define XRC_COMPAT_H

#include "wx/xrc/xmlres.h"

#if !wxCHECK_VERSION(3, 2, 0) || defined(wxUSE_STD_STRING_CONV_IN_WXSTRING)
    #undef XRCCTRL
    #define XRCCTRL(window, id, type) \
        (wxStaticCast((window).FindWindow(wxXmlResource::GetXRCID(id)), type))
#endif

#endif // XRC_COMPAT_H
