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
#include "pushbutton.h"
#include "keypad.h"

KeypadScreen::KeypadScreen(wxWindow *parent, const wxSize& size, int tilType)
: Panel(parent, size, tilType)
{
}

KeypadScreen::~KeypadScreen()
{
    delete mainBitmapPointer;
    for (int i=0;i<2;i++) 
    {
        delete ledPointer[i];
    }
#if defined (__WXMAC__)
    delete osx_push_inButtonPointer;
    for (int i=0;i<16;i++)
        delete osx_buttonPointer[i];
#else
    delete push_inButtonPointer;
    for (int i=0;i<16;i++)
        delete buttonPointer[i];
#endif
}

void KeypadScreen::init(int computerType)
{
    keyStart_ = 0;
    keyEnd_ = 0;
    lastKey_ = 0;
    forceUpperCase_ = p_Main->getUpperCase();

    wxClientDC dc(this);
    wxString buttonText;
    int x, y;

    mainBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/hexpad.png", wxBITMAP_TYPE_PNG);

    for (int i=0;i<2;i++) 
    {
        ledPointer[i] = new Led(dc, 20+96*(1-i),174, LED_SMALL_RED);
        updateLed_[i] = true;
    }
    ledPointer[1]->setStatus(dc, 1);

#if defined (__WXMAC__)
    osx_push_inButtonPointer = new HexButton(dc, PUSH_BUTTON, 8, 11, "IN");
    for (int i=0; i<16; i++)
    {
        buttonText.Printf("%01X", i);
        x = 8+(i&0x3)*32;
        y = 139 -(int)i/4*32;
        osx_buttonPointer[i] = new HexButton(dc, PUSH_BUTTON, x, y, buttonText);
    }
#else
    push_inButtonPointer = new PushButton(this, 20, "IN", wxPoint(8, 11), wxSize(30, 30), 0);
    for (int i=0;i<16;i++)
    {
        buttonText.Printf("%01X", i);
        x = 8+(i&0x3)*32;
        y = 139 -(int)i/4*32;
        buttonPointer[i] = new PushButton(this, i, buttonText, wxPoint(x, y), wxSize(30, 30), 0);
    }
#endif
    this->connectKeyEvent(this);
}

void KeypadScreen::onPaint(wxPaintEvent&WXUNUSED(event))
{
    wxPaintDC dc(this);
    dc.DrawBitmap(*mainBitmapPointer, 0, 0);

    for (int i=0;i<2;i++) 
    {
        ledPointer[i]->onPaint(dc);
    }
#if defined (__WXMAC__)
    rePaintLeds(dc);
    osx_push_inButtonPointer->onPaint(dc);
    for (int i = 0; i<16; i++)
        osx_buttonPointer[i]->onPaint(dc);
#endif
}

void KeypadScreen::onMouseRelease(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);
    
    wxClientDC dc(this);
    
#if defined (__WXMAC__)
    if (osx_push_inButtonPointer->onMouseRelease(dc, x, y))
        p_Computer->onInButtonRelease();
    for (int i = 0; i<16; i++)
        if (osx_buttonPointer[i]->onMouseRelease(dc, x, y))
            p_Computer->onNumberKeyUp();
#endif
}

void KeypadScreen::onMousePress(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);
    
    wxClientDC dc(this);
    
#if defined (__WXMAC__)
    if (osx_push_inButtonPointer->onMousePress(dc, x, y))
        p_Computer->onInButtonPress();
    
    for (int i = 0; i<16; i++)
    {
        if (osx_buttonPointer[i]->onMousePress(dc, x, y))
        {
            p_Computer->onNumberKeyDown(i);
        }
    }
#endif
}

void KeypadScreen::releaseButtonOnScreen(HexButton* buttonPoint)
{
    wxClientDC dc(this);
    
    buttonPoint->releaseButtonOnScreen(dc);
}

BEGIN_EVENT_TABLE(Keypad, wxFrame)
    EVT_CLOSE (Keypad::onClose)
    EVT_COMMAND(20, wxEVT_ButtonDownEvent, Keypad::onButtonPress)
    EVT_COMMAND(20, wxEVT_ButtonUpEvent, Keypad::onButtonRelease)
    EVT_COMMAND(0, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
    EVT_COMMAND(1, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
    EVT_COMMAND(2, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
    EVT_COMMAND(3, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
    EVT_COMMAND(4, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
    EVT_COMMAND(5, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
    EVT_COMMAND(6, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
    EVT_COMMAND(7, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
    EVT_COMMAND(8, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
    EVT_COMMAND(9, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
    EVT_COMMAND(10, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
    EVT_COMMAND(11, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
    EVT_COMMAND(12, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
    EVT_COMMAND(13, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
    EVT_COMMAND(14, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
    EVT_COMMAND(15, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
    EVT_COMMAND(0, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
    EVT_COMMAND(1, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
    EVT_COMMAND(2, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
    EVT_COMMAND(3, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
    EVT_COMMAND(4, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
    EVT_COMMAND(5, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
    EVT_COMMAND(6, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
    EVT_COMMAND(7, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
    EVT_COMMAND(8, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
    EVT_COMMAND(9, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
    EVT_COMMAND(10, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
    EVT_COMMAND(11, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
    EVT_COMMAND(12, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
    EVT_COMMAND(13, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
    EVT_COMMAND(14, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
    EVT_COMMAND(15, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
END_EVENT_TABLE()

Keypad::Keypad(const wxString& title, const wxPoint& pos, const wxSize& size, int computerType)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    computerType_ = computerType;

#ifndef __WXMAC__
    SetIcon(wxICON(app_icon));
#endif

    keypadScreenPointer = new KeypadScreen(this, size, TILNONE);
    keypadScreenPointer->init(computerType);
    keypadScreenPointer->setLed(1, 1);
    keypadScreenPointer->setLed(0, 0);

    keypadValue_ = 0;
    nextNybble_ = 'H';
    this->SetClientSize(size);
}

Keypad::~Keypad()
{
    delete keypadScreenPointer;
}

void Keypad::onClose(wxCloseEvent&WXUNUSED(event))
{
    p_Computer->removeElfHex();
    Destroy();
}

void Keypad::onNumberKeyDown(wxCommandEvent&event)
{
    int i = event.GetId();
    onNumberKeyDown(i);
}

void Keypad::onNumberKeyDown(int i)
{
    wxClientDC dc(this);
    
    keypadValue_= (nextNybble_ == 'H')?(keypadValue_&15)+(i<<4):(keypadValue_&0xf0)+i;
    if (nextNybble_ == 'H')
    {
        nextNybble_ = 'L';
        keypadScreenPointer->setLed(1, 0);
        keypadScreenPointer->setLed(0, 1);
    }
    else
    {
        nextNybble_ = 'H';
        keypadScreenPointer->setLed(1, 1);
        keypadScreenPointer->setLed(0, 0);
    }
    p_Computer->onHexDown(i);
}

void Keypad::onNumberDown(int hex)
{
    wxClientDC dc(this);

    keypadValue_= (nextNybble_ == 'H')?(keypadValue_&15)+(hex<<4):(keypadValue_&0xf0)+hex;
    if (nextNybble_ == 'H')
    {
        nextNybble_ = 'L';
        keypadScreenPointer->setLed(1, 0);
        keypadScreenPointer->setLed(0, 1);
    }
    else
    {
        nextNybble_ = 'H';
        keypadScreenPointer->setLed(1, 1);
        keypadScreenPointer->setLed(0, 0);
    }
}

void Keypad::onNumberKeyUp(wxCommandEvent&event)
{
    int i = event.GetId();
    p_Computer->onHexKeyUp(i);
}

void Keypad::onButtonRelease(wxCommandEvent& event)
{
    p_Computer->onInButtonRelease();
    event.Skip();
}

void Keypad::onButtonPress(wxCommandEvent& event)
{
    p_Computer->onInButtonPress(keypadValue_);
    event.Skip();
}

void Keypad::ledTimeout()
{
    keypadScreenPointer->ledTimeout();
}

void Keypad::setLedMs(long ms)
{
    keypadScreenPointer->setLedMs(ms);
}

Byte Keypad::getKey(Byte vtOut)
{
    return keypadScreenPointer->getKey(vtOut);
}

void Keypad::releaseButtonOnScreen(HexButton* buttonPointer)
{
    keypadScreenPointer->releaseButtonOnScreen(buttonPointer);
}

void Keypad::refreshPanel()
{
    keypadScreenPointer->refreshPanel();
}

EtiKeypad::EtiKeypad()
{
    keyClear();
    step_ = false;
}

void EtiKeypad::configure(IoConfiguration ioConf, int keyDefA1[], int keyDefA2[])
{
    for (int i=0; i<16; i++)
    {
        keyDefA1_[i] = keyDefA1[i];
        keyDefA2_[i] = keyDefA2[i];
    }
    ioConfiguration_ = ioConf;

    wxString printBuffer;
 
    wxString ioGroup = "";
    if (ioConfiguration_.etiKeypad.ioGroup != -1)
    {
        ioGroup.Printf(" on group %d", ioConfiguration_.etiKeypad.ioGroup);
    }
    
    p_Main->message("Configuring HUG1802 keypad" + ioGroup);

    printBuffer.Printf("	Output %d: write PIA", ioConfiguration_.etiKeypad.out);
    p_Main->message(printBuffer);

    printBuffer.Printf("	Input %d: read PIA", ioConfiguration_.etiKeypad.inp);
    p_Main->message(printBuffer);
 
    printBuffer.Printf("	Ef %d: step key\n", ioConfiguration_.etiKeypad.ef);
    p_Main->message(printBuffer);

    DataDirection_ = true;
}

void EtiKeypad::keyDown(int keycode,  wxKeyEvent& event)
{
    for (int i=0; i<16; i++)
    {
        if (keycode == keyDefA1_[i])
            eti660KeyState_[i] = true;
        if (keycode == keyDefA2_[i])
            eti660KeyState_[i] = true;
    }

    if (keycode == 43)
        step_ = true;

    if (keycode == WXK_NUMPAD_ADD)
        step_ = true;
}

void EtiKeypad::keyUp(int keycode, wxKeyEvent& WXUNUSED(event))
{
    for (int i=0; i<16; i++)
    {
        if (keycode == keyDefA1_[i])
            eti660KeyState_[i] = false;
        if (keycode == keyDefA2_[i])
            eti660KeyState_[i] = false;
    }

    if (keycode == 43)
        step_ = false;

    if (keycode == WXK_NUMPAD_ADD)
        step_ = false;
}

void EtiKeypad::onInButtonPress()
{
    step_ = true;
}

void EtiKeypad::onInButtonRelease()
{
    step_ = false;
}

Byte EtiKeypad::ef()
{
    return step_;
}

Byte EtiKeypad::in(Word address)
{
    Byte ret = 0xff;

    if ((address&0x3) == 0)
    {
        if (DataDirection_)    // Data Direction A
        {
//            p_Main->message("Data Direction Register: Input");
        }
        else                // Output Register
        {
            if ((outputKeyValue_&0x1) == 0)
            {
                if (eti660KeyState_[3])
                    ret &= 0x7f;
                if (eti660KeyState_[2])
                    ret &= 0xbf;
                if (eti660KeyState_[1])
                    ret &= 0xdf;
                if (eti660KeyState_[0])
                    ret &= 0xef;
            }

            if ((outputKeyValue_&0x2) == 0)
            {
                if (eti660KeyState_[7])
                    ret &= 0x7f;
                if (eti660KeyState_[6])
                    ret &= 0xbf;
                if (eti660KeyState_[5])
                    ret &= 0xdf;
                if (eti660KeyState_[4])
                    ret &= 0xef;
            }

            if ((outputKeyValue_&0x4) == 0)
            {
                if (eti660KeyState_[0xb])
                    ret &= 0x7f;
                if (eti660KeyState_[0xa])
                    ret &= 0xbf;
                if (eti660KeyState_[9])
                    ret &= 0xdf;
                if (eti660KeyState_[8])
                    ret &= 0xef;
            }

            if ((outputKeyValue_&0x8) == 0)
            {
                if (eti660KeyState_[0xf])
                    ret &= 0x7f;
                if (eti660KeyState_[0xe])
                    ret &= 0xbf;
                if (eti660KeyState_[0xd])
                    ret &= 0xdf;
                if (eti660KeyState_[0xc])
                    ret &= 0xef;
            }

            if ((outputKeyValue_&0x10) == 0)
            {
                if (eti660KeyState_[0xc])
                    ret &= 0xf7;
                if (eti660KeyState_[8])
                    ret &= 0xfb;
                if (eti660KeyState_[4])
                    ret &= 0xfd;
                if (eti660KeyState_[0])
                    ret &= 0xfe;
            }

            if ((outputKeyValue_&0x20) == 0)
            {
                if (eti660KeyState_[0xd])
                    ret &= 0xf7;
                if (eti660KeyState_[9])
                    ret &= 0xfb;
                if (eti660KeyState_[5])
                    ret &= 0xfd;
                if (eti660KeyState_[1])
                    ret &= 0xfe;
            }

            if ((outputKeyValue_&0x40) == 0)
            {
                if (eti660KeyState_[0xe])
                    ret &= 0xf7;
                if (eti660KeyState_[0xa])
                    ret &= 0xfb;
                if (eti660KeyState_[6])
                    ret &= 0xfd;
                if (eti660KeyState_[2])
                    ret &= 0xfe;
            }

            if ((outputKeyValue_&0x80) == 0)
            {
                if (eti660KeyState_[0xf])
                    ret &= 0xf7;
                if (eti660KeyState_[0xb])
                    ret &= 0xfb;
                if (eti660KeyState_[7])
                    ret &= 0xfd;
                if (eti660KeyState_[3])
                    ret &= 0xfe;
            }

            ret &= inputKeyLatch_;
            ret |= (outputKeyValue_&outputKeyLatch_);
//            p_Main->eventMessageHex(ret);
        }
    }
//    else
//        p_Main->message("Other Input");
    return ret;
}

bool EtiKeypad::out(Word address, Byte value)
{
    bool returnValue = false;
    
    if ((address&0x3) == 1)    // Control register A
    {
        if ((value&0x4) == 0x4)
        {
//            p_Main->message("Data Direction Register Selected");
            returnValue = true;
            DataDirection_ = true;
        }
        else
        {
//            p_Main->message("Output Register Selected");
            DataDirection_ = false;
        }
//        p_Main->eventMessageHex(value);
    }

    if ((address&0x3) == 0)
    {
        if (DataDirection_)    // Data Direction A
        {
//            p_Main->message("Data Direction Register");
//            p_Main->eventMessageHex(value);
            outputKeyLatch_ = value;
            inputKeyLatch_ = value^0xff;
        }
        else                // Output Register A
        {
//            p_Main->message("Output Register");
            outputKeyValue_ = value&outputKeyLatch_;
            outputKeyValue_ |= inputKeyLatch_;
//            p_Main->eventMessageHex(outputKeyValue_);
        }
    }
    return returnValue;
}

void EtiKeypad::keyClear()
{
    for (int i=0; i<16; i++)
        eti660KeyState_[i] = false;
}
