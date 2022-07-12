/*
 *******************************************************************
 *** This software is copyright 2008 by Marcel van Tongeren      ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 ***                                                             ***
 *** 1802 Code based on elf emulator by Michael H Riley with     ***
 *** copyright as below                                          ***
 *******************************************************************
*/
 
/*
 *******************************************************************
 *** This software is copyright 2006 by Michael H Riley          ***
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

#if defined(__linux__)
#include "app_icon.xpm"
#endif

#include "main.h"
#include "ledmodule.h"

LedModuleScreen::LedModuleScreen(wxWindow *parent, const wxSize& size)
: Panel(parent, size)
{
}

LedModuleScreen::~LedModuleScreen()
{
    delete mainBitmapPointer;
    for (int i=0;i<8;i++)
    {
        delete ledPointer[i];
    }
}

void LedModuleScreen::init(int computerType)
{
    keyStart_ = 0;
    keyEnd_ = 0;
    lastKey_ = 0;
    forceUpperCase_ = p_Main->getUpperCase(ELF);

    wxClientDC dc(this);

    mainBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/hexpad.png", wxBITMAP_TYPE_PNG);

    for (int i=0;i<8;i++)
    {
        ledPointer[i] = new Led(dc, 20+16*(7-i),16, computerType);
        updateLed_[i] = true;
    }

    this->connectKeyEvent(this);
}

void LedModuleScreen::onPaint(wxPaintEvent&WXUNUSED(event))
{
    wxPaintDC dc(this);
    dc.DrawBitmap(*mainBitmapPointer, 0, 0);

#if defined(__WXMAC__)
    rePaintLeds(dc);
#endif
        
    for (int i=0;i<8;i++)
    {
        ledPointer[i]->onPaint(dc);
    }
}

BEGIN_EVENT_TABLE(LedModule, wxFrame)
    EVT_CLOSE (LedModule::onClose)
END_EVENT_TABLE()

LedModule::LedModule(const wxString& title, const wxPoint& pos, const wxSize& size, int computerType)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
#ifndef __WXMAC__
    SetIcon(wxICON(app_icon));
#endif

    this->SetClientSize(size);

    ledModuleScreenPointer = new LedModuleScreen(this, size);
    ledModuleScreenPointer->init(computerType);
}

LedModule::~LedModule()
{
    delete ledModuleScreenPointer;
}

void LedModule::onClose(wxCloseEvent&WXUNUSED(event) )
{
    p_Computer->removeElfLedModule();
    Destroy();
}

void LedModule::configure(ElfPortConfiguration elfPortConf)
{
//    int output;

//    output = p_Main->getConfigItem("Elf/Led_Module_Output", 4l);
    p_Computer->setOutType(elfPortConf.led_Module_Output, LEDMODOUT);

    wxString printBuffer;
    p_Main->message("Configuring Led Module");

    printBuffer.Printf("    Output %d: write data\n", elfPortConf.led_Module_Output);
    p_Main->message(printBuffer);
}

void LedModule::write(Byte value)
{
    wxClientDC dc(this);

    for (int i=0;i<8;i++)
    {
        ledModuleScreenPointer->setLed(i, (value & 1) ? 1 : 0);
        value>>=1;
    }
}

void LedModule::ledTimeout()
{
    ledModuleScreenPointer->ledTimeout();
}

void LedModule::setLedMs(long ms)
{
    ledModuleScreenPointer->setLedMs(ms);
}

Byte LedModule::getKey(Byte vtOut)
{
    return ledModuleScreenPointer->getKey(vtOut);
}

void LedModule::refreshPanel()
{
    ledModuleScreenPointer->refreshPanel();
}
