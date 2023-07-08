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

#if defined(__linux__)
#include "app_icon.xpm"
#endif

#include "main.h"
#include "pio.h"

PioScreen::PioScreen(wxWindow *parent, const wxSize& size, int pioNumber, int tilType)
: Panel(parent, size, tilType)
{
    pioNumber_ = pioNumber;
//    this->SetClientSize(size);
}

PioScreen::~PioScreen()
{
    for (int i=0; i<20; i++)
    {
        delete ledPointer[i];
        delete dataSwitchButton[i];
    }
#if defined (__WXMAC__)
    delete osx_ardyButtonPointer;
    delete osx_brdyButtonPointer;
#else
    delete text_ardyButtonPointer;
    delete text_brdyButtonPointer;
#endif
}

void PioScreen::init()
{
    wxClientDC dc(this);
    
    for (int i=0; i<8; i++)
    {
        ledPointer[i] = new Led(dc, 33+23*(7-i), 30, PIOLED);
        updateLed_[i]=true;
        setLed(i, 2);
        dataSwitchButton[i] = new SwitchButton(dc, PIO_VERTICAL_BUTTON, wxColour(255, 255, 255), BUTTON_DOWN, 27+23*(7-i), 50, "");
        ioSwitchState_[i] = 0;
        ioSwitchEnabled_[i] = true;
    }
    for (int i=8; i<16; i++)
    {
        ledPointer[i] = new Led(dc, 33+23*(15-i), 90, PIOLED);
        updateLed_[i]=true;
        setLed(i, 2);
        dataSwitchButton[i] = new SwitchButton(dc, PIO_VERTICAL_BUTTON, wxColour(255, 255, 255), BUTTON_DOWN, 27+23*(15-i), 110, "");
        ioSwitchState_[i] = 0;
        ioSwitchEnabled_[i] = true;
    }
    for (int i=16; i<18; i++)
    {
        ledPointer[i] = new Led(dc, 229+37*(i-16), 30, PIOLED);
        updateLed_[i]=true;
        setLed(i, 2);
        dataSwitchButton[i] = new SwitchButton(dc, PIO_VERTICAL_BUTTON, wxColour(255, 255, 255), BUTTON_DOWN, 223+37*(i-16), 50, "");
        ioSwitchState_[i] = 0;
        ioSwitchEnabled_[i] = false;
        dataSwitchButton[i]->enable(dc, false);
    }
    for (int i=18; i<20; i++)
    {
        ledPointer[i] = new Led(dc, 229+37*(i-18), 90, PIOLED);
        updateLed_[i]=true;
        setLed(i, 2);
        dataSwitchButton[i] = new SwitchButton(dc, PIO_VERTICAL_BUTTON, wxColour(255, 255, 255), BUTTON_DOWN, 223+37*(i-18), 110, "");
        ioSwitchState_[i] = 0;
        ioSwitchEnabled_[i] = false;
        dataSwitchButton[i]->enable(dc, false);
    }
    outPutValueA_ = 0;
    outPutValueB_ = 0;
    inPutValueA_ = 0;

#if defined (__WXMAC__)
    osx_ardyButtonPointer = new HexButton2(dc, PIO_HEX_BUTTON, 55, 144, "A", pioNumber_);
    osx_brdyButtonPointer = new HexButton2(dc, PIO_HEX_BUTTON, 95, 144, "B", pioNumber_);
#else
    text_ardyButtonPointer = new wxButton(this, 1, "A", wxPoint(55, 144), wxSize(25, 25), 0, wxDefaultValidator, "ArdyButton");
    text_ardyButtonPointer->SetToolTip("ARDY");
    text_brdyButtonPointer = new wxButton(this, 2, "B", wxPoint(95, 144), wxSize(25, 25), 0, wxDefaultValidator, "BrdyButton");
    text_brdyButtonPointer->SetToolTip("BRDY");
#endif

    this->connectKeyEvent(this);
}

void PioScreen::onPaint(wxPaintEvent&WXUNUSED(event))
{
    wxPaintDC dc(this);
    wxColourDatabase colour;
    wxString number;

    dc.SetPen(*wxWHITE_PEN);
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.DrawRectangle(0, 0, 329, 219);
#if defined(__WXMAC__)
    wxFont defaultFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
#else
    wxFont defaultFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
#endif

    dc.SetTextForeground(*wxBLACK);
    dc.SetFont(defaultFont);
    
    for (int i=0; i<8; i++)
    {
        number.Printf("%d", i);
        dc.DrawText(number, 33+23*(7-i), 6);
    }
    dc.DrawText("STB", 219, 6);
    dc.DrawText("RDY", 256, 6);
    dc.DrawText("A:", 10, 26);
    dc.DrawText("B:", 10, 86);
    dc.DrawText("INT:", 10, 146);

    dc.SetTextForeground(colour.Find("GREY"));
    
#if defined (__WXMAC__)
    rePaintLeds(dc);
#endif
    
    for (int i=0; i<20; i++)
    {
        ledPointer[i]->onPaint(dc);
        dataSwitchButton[i]->onPaint(dc);
    }

#if defined (__WXMAC__)
    osx_ardyButtonPointer->onPaint(dc);
    osx_brdyButtonPointer->onPaint(dc);
#endif
}

void PioScreen::onMousePress(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);
    
    wxClientDC dc(this);

    if (pioAInterruptEnabled_)
    {
        if (osx_ardyButtonPointer->onMousePress(dc, x, y))
            onArdyButton();
    }

    if (pioBInterruptEnabled_)
    {
        if (osx_brdyButtonPointer->onMousePress(dc, x, y))
            onBrdyButton();
    }
}

void PioScreen::onMouseRelease(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);
    
    wxClientDC dc(this);
    
    for (int i=0; i<20; i++)
    {
        if (ioSwitchEnabled_[i])
        {
            if (dataSwitchButton[i]->onMouseRelease(dc, x, y))
                ioSwitch(i);
        }
    }
#if defined (__WXMAC__)
    if (pioAInterruptEnabled_)
        osx_ardyButtonPointer->onMouseRelease(dc, x, y);
    if (pioBInterruptEnabled_)
        osx_brdyButtonPointer->onMouseRelease(dc, x, y);
#endif
}

void PioScreen::releaseButtonOnScreen(HexButton* buttonPoint)
{
    wxClientDC dc(this);

    buttonPoint->releaseButtonOnScreen(dc);
}

void PioScreen::interruptCycle()
{
    if (pioAInterruptEnabled_)
        maskInterruptA();
    
    if (pioBInterruptEnabled_)
        maskInterruptB();
}

void PioScreen::onArdyButton()
{
    p_Computer->setEfState(pioNumber_, 1, 0);
   
    if (pioAMode_ == PIO_BI_DRECT)
        pioStatus_ |= 0x8;
    else
        pioStatus_ |= 0x2;

    if (pioAInterruptEnabled_)
        p_Computer->requestInterrupt();
}

void PioScreen::onBrdyButton()
{
    p_Computer->setEfState(pioNumber_, 2, 0);
    
    if (pioAMode_ == PIO_BI_DRECT)
        pioStatus_ |= 0x4;
    else
        pioStatus_ |= 0x1;

    if (pioBInterruptEnabled_)
        p_Computer->requestInterrupt();
}

void PioScreen::ioSwitch(int i)
{
    if (ioSwitchState_[i])
        ioSwitchState_[i] = 0;
    else
        ioSwitchState_[i] = 1;
    
    if (i<8)
    {
        Byte bitmask1 = (1 << i) ^ 0xff;
        if (pioAMode_ == PIO_INPUT)
        {
            outPutValueA_ &= bitmask1;
            outPutValueA_ |=  ioSwitchState_[i] << i;
        }
        if (pioAMode_ == PIO_BI_DRECT)
        {
            inPutValueA_ &= bitmask1;
            inPutValueA_ |=  ioSwitchState_[i] << i;
        }
        if (pioAMode_ == PIO_BIT_PROG)
        {
            Byte bitmask2 = 1 << i;
            if ((pioAProgBits_ & bitmask2) == 0)
            {
                outPutValueA_ &= bitmask1;
                outPutValueA_ |=  ioSwitchState_[i] << i;
            }
        }
    }
    if (i>=8 && i<16)
    {
        Byte bitmask1 = (1 << (i-8)) ^ 0xff;
        if (pioBMode_ == PIO_INPUT || pioBMode_ == PIO_BI_DRECT)
        {
            outPutValueB_ &= bitmask1;
            outPutValueB_ |=  ioSwitchState_[i] << (i-8);
        }
        if (pioBMode_ == PIO_BIT_PROG)
        {
            Byte bitmask2 = 1 << (i-8);
            if ((pioBProgBits_ & bitmask2) == 0)
            {
                outPutValueB_ &= bitmask1;
                outPutValueB_ |=  ioSwitchState_[i] << (i-8);
            }
        }
    }
}

void PioScreen::reset()
{
    wxClientDC dc(this);
    
    pioAMode_ = PIO_INPUT;
    pioBMode_ = PIO_INPUT;
    pioStbAMode_ = PIO_INPUT;
    pioStbBMode_ = PIO_INPUT;
    pioRdyAMode_ = PIO_OUTPUT;
    pioRdyBMode_ = PIO_OUTPUT;
    pioAInterruptEnabled_ = false;
    pioBInterruptEnabled_ = false;
    pioAInterruptButtonEnabled_ = false;
    pioBInterruptButtonEnabled_ = false;
    pioAInterruptButtonState_ = false;
    pioBInterruptButtonState_ = false;
    pioAInterruptMask_ = 0;
    pioBInterruptMask_ = 0;
    pioStatus_ = 0;
    commandByteNumber_ = PIO_COMMAND_NONE;
    writeControlRegister(0x1b);

    clearA();
    clearB();
    clearStbRdy();

#if defined (__WXMAC__)
    osx_ardyButtonPointer->enable(dc, false);
    osx_brdyButtonPointer->enable(dc, false);
#else
    text_ardyButtonPointer->Disable();
    text_brdyButtonPointer->Disable();
#endif
}

void PioScreen::clearA()
{
    wxClientDC dc(this);

    outPutValueA_ = 0;
    inPutValueA_ = 0;
    for (int i=0; i<8; i++)
    {
        dataSwitchButton[i]->setState(dc, 0);
        ioSwitchState_[i] = 0;
    }
}

void PioScreen::clearB()
{
    wxClientDC dc(this);
    
    outPutValueB_ = 0;
    for (int i=8; i<16; i++)
    {
        dataSwitchButton[i]->setState(dc, 0);
        ioSwitchState_[i] = 0;
    }
}

void PioScreen::clearStbRdy()
{
    wxClientDC dc(this);
    
    for (int i=16; i<20; i++)
    {
        dataSwitchButton[i]->setState(dc, 0);
        ioSwitchState_[i] = 0;
    }
    pioStatus_ &= 0x0f;
}

void PioScreen::writeControlRegister(Byte value)
{
    wxClientDC dc(this);
    Byte outputValue, inputValue;
    
    if (commandByteNumber_ == PIO_COMMAND_NONE)
    {
        switch (value & 0xc7)
        {
            case 0x3: // Set input
            case 0x7:
                if (value & 0x8)
                {   // Port A
                    pioAMode_ = PIO_INPUT;
                    outputValue = outPutValueA_;
                    for (int i=0; i<8; i++)
                    {
                        setLed(i, 2);
                        dataSwitchButton[i]->enable(dc, true);
                        ioSwitchEnabled_[i] = true;
                        dataSwitchButton[i]->setState(dc, (outputValue&1) == 1);
                        ioSwitchState_[i] = outputValue&1;
                        outputValue = outputValue >> 1;
                   }
                }
                if (value & 0x10)
                {   // Port B
                    pioBMode_ = PIO_INPUT;
                    outputValue = outPutValueB_;
                    for (int i=8; i<16; i++)
                    {
                        setLed(i, 2);
                        dataSwitchButton[i]->enable(dc, true);
                        ioSwitchEnabled_[i] = true;
                        dataSwitchButton[i]->setState(dc, (outputValue&1) == 1);
                        ioSwitchState_[i] = outputValue&1;
                        outputValue = outputValue >> 1;
                    }
                }
            break;
                
            case 0x43: // Set output
            case 0x47:
                if (value & 0x8)
                {   // Port A
                    pioAMode_ = PIO_OUTPUT;
                    outputValue = outPutValueA_;
                    for (int i=0; i<8; i++)
                    {
                        setLed(i, outputValue&1);
                        outputValue = outputValue >> 1;
                        dataSwitchButton[i]->enable(dc, false);
                        ioSwitchEnabled_[i] = false;
                    }
                }
                if (value & 0x10)
                {   // Port B
                    pioBMode_ = PIO_OUTPUT;
                    outputValue = outPutValueB_;
                    for (int i=8; i<16; i++)
                    {
                        setLed(i, outputValue&1);
                        outputValue = outputValue >> 1;
                        dataSwitchButton[i]->enable(dc, false);
                        ioSwitchEnabled_[i] = false;
                    }
                }
            break;
                
            case 0xc3: // Set bit progammable
            case 0xc7:
                if ((value & 0x8) == 0x8)  // Port A
                {
                    pioAMode_ = PIO_BIT_PROG;
                    commandByteNumber_ = PIO_COMMAND_BITPROGA;
                    enableStbRdyA(dc);
                }
                if ((value & 0x10) == 0x10) // Port B
                {
                    pioBMode_ = PIO_BIT_PROG;
                    commandByteNumber_ = PIO_COMMAND_BITPROGB;
                    enableStbRdyB(dc);
                }
                if ((value & 0x18) == 0x18) // Port A+B
                {
                    commandByteNumber_ = PIO_COMMAND_BITPROGAB;
                    enableStbRdyA(dc);
                    enableStbRdyB(dc);
                }
            break;
                
            case 0x83: // Set bi directional
            case 0x87:
                if (value & 0x8 && pioBMode_ == PIO_BIT_PROG)
                {   // Port A & port B needs to be set in bit programmable
                    pioAMode_ = PIO_BI_DRECT;
                    outputValue = outPutValueA_;
                    inputValue = inPutValueA_;
                    for (int i=0; i<8; i++)
                    {
                        setLed(i, outputValue&1);
                        dataSwitchButton[i]->enable(dc, true);
                        ioSwitchEnabled_[i] = true;
                        dataSwitchButton[i]->setState(dc, (inputValue&1) == 1);
                        ioSwitchState_[i] = inputValue&1;
                        outputValue = outputValue >> 1;
                        inputValue = inputValue >> 1;
                    }
                    disableStbRdyA(dc);
                    disableStbRdyB(dc);
                }
            break;

            case 0x05: // Set interrupt control
            case 0x45:
                if ((value & 0x10) == 0x10)
                {
                    commandByteNumber_ = PIO_COMMAND_INT;
                    pioInterruptCommand_ = value;
                }
                if ((pioInterruptCommand_ & 0x8) == 0)
                    pioAInterruptLogical_ = (value & 0x60) >> 5;
                else
                    pioBInterruptLogical_ = (value & 0x60) >> 5;
            break;

            case 0x01: // Set interrupt disable
            case 0x41:
                if ((value & 0x8) == 0)
                    pioAInterruptEnabled_ = false;
                else
                    pioBInterruptEnabled_ = false;
            break;
 
            case 0x81: // Set interrupt enable
            case 0xc1:
                if ((value & 0x8) == 0)
                    pioAInterruptEnabled_ = true;
                else
                    pioBInterruptEnabled_ = true;
            break;
        }
        enableInterruptButton(dc);
        if ((value & 0x1) == 0) // STB and RDY Control
        {
            readStatusRegister();
            switch (value & 0xe)
            {
                case 0x00: // Port A, output STB/RDY
                    if (pioRdyAMode_ == PIO_OUTPUT)
                    {
                        setLed(PIO_RDY_A, (value & 0x10) >> 4);
                        pioStatus_ &= 0xef;
                        pioStatus_ |= (value & 0x10);
                    }
                    if (pioStbAMode_ == PIO_OUTPUT)
                    {
                        setLed(PIO_STB_A, (value & 0x20) >> 5);
                        pioStatus_ &= 0xdf;
                        pioStatus_ |= (value & 0x20);
                    }
                break;
                    
                case 0x02: // Port B, output STB/RDY
                    if (pioRdyBMode_ == PIO_OUTPUT)
                    {
                        setLed(PIO_RDY_B, (value & 0x10) >> 4);
                        pioStatus_ &= 0xbf;
                        pioStatus_ |= ((value & 0x10) << 2);
                    }
                    if (pioStbBMode_ == PIO_OUTPUT)
                    {
                        setLed(PIO_STB_B, (value & 0x20) >> 5);
                        pioStatus_ &= 0x7f;
                        pioStatus_ |= ((value & 0x20) << 2);
                    }
               break;

                case 0x04: // Port A, set RDY
                    if (pioAMode_ == PIO_BIT_PROG && pioAMode_ != PIO_BI_DRECT)
                    {
                        pioRdyAMode_ = (value & 0x40) >> 6;
                        enableStbRdyA(dc);
                    }
                break;

                case 0x06: // Port B, set RDY
                    if (pioBMode_ == PIO_BIT_PROG && pioAMode_ != PIO_BI_DRECT)
                    {
                        pioRdyBMode_ = (value & 0x40) >> 6;
                        enableStbRdyB(dc);
                    }
                break;

                case 0x08: // Port A, set STB
                    if (pioAMode_ == PIO_BIT_PROG && pioAMode_ != PIO_BI_DRECT)
                    {
                        pioStbAMode_ = (value & 0x80) >> 7;
                        enableStbRdyA(dc);
                    }
                break;

                case 0x0A: // Port B, set STB
                    if (pioBMode_ == PIO_BIT_PROG && pioAMode_ != PIO_BI_DRECT)
                    {
                        pioStbBMode_ = (value & 0x80) >> 7;
                        enableStbRdyB(dc);
                    }
                break;

                case 0x0C: // Port A, set RDY+STB
                    if (pioAMode_ == PIO_BIT_PROG && pioAMode_ != PIO_BI_DRECT)
                    {
                        pioRdyAMode_ = (value & 0x40) >> 6;
                        pioStbAMode_ = (value & 0x80) >> 7;
                        enableStbRdyA(dc);
                    }
                break;

                case 0x0E: // Port B, set RDY+STB
                    if (pioBMode_ == PIO_BIT_PROG && pioAMode_ != PIO_BI_DRECT)
                    {
                        pioRdyBMode_ = (value & 0x40) >> 6;
                        pioStbBMode_ = (value & 0x80) >> 7;
                        enableStbRdyB(dc);
                    }
                break;
            }
        }
    }
    else
    {
        switch (commandByteNumber_)
        {
            case PIO_COMMAND_BITPROGA:
                setProgBitsA(dc, value);
            break;
                
            case PIO_COMMAND_BITPROGB:
                setProgBitsB(dc, value);
            break;

            case PIO_COMMAND_BITPROGAB:
                setProgBitsA(dc, value);
                setProgBitsB(dc, value);
            break;
                
            case PIO_COMMAND_INT:
                if ((pioInterruptCommand_ & 0x8) == 0)
                    pioAInterruptMask_ = value ^ 0xff;
                else
                    pioBInterruptMask_ = value ^ 0xff;
                commandByteNumber_ = PIO_COMMAND_NONE;
            break;
        }
    }
}

void PioScreen::setProgBitsA(wxDC& dc, Byte value)
{
    pioAProgBits_ = value;
    Byte outPutValue = outPutValueA_;
    
    for (int i=0; i<8; i++)
    {
        if (value&1)
        {
            setLed(i, outPutValue&1);
            dataSwitchButton[i]->enable(dc, false);
            ioSwitchEnabled_[i] = false;
        }
        else
        {
            setLed(i, 2);
            dataSwitchButton[i]->enable(dc, true);
            ioSwitchEnabled_[i] = true;
            dataSwitchButton[i]->setState(dc, (outPutValue&1) == 1);
            ioSwitchState_[i] = outPutValue&1;
        }
        outPutValue = outPutValue >> 1;
        value = value >> 1;
    }
    commandByteNumber_ = PIO_COMMAND_NONE;
}

void PioScreen::setProgBitsB(wxDC& dc, Byte value)
{
    pioBProgBits_ = value;
    Byte outPutValue = outPutValueB_;

    for (int i=8; i<16; i++)
    {
        if (value&1)
        {
            setLed(i, outPutValue&1);
            dataSwitchButton[i]->enable(dc, false);
            ioSwitchEnabled_[i] = false;
        }
        else
        {
            setLed(i, 2);
            dataSwitchButton[i]->enable(dc, true);
            ioSwitchEnabled_[i] = true;
            dataSwitchButton[i]->setState(dc, (outPutValue&1) == 1);
            ioSwitchState_[i] = outPutValue&1;
        }
        outPutValue = outPutValue >> 1;
        value = value >> 1;
    }
    commandByteNumber_ = PIO_COMMAND_NONE;
}

void PioScreen::enableStbRdyA(wxDC& dc)
{
    Byte mode = ((pioStbAMode_^1) << 1) & 0x3;
    if (mode == 0)
        mode = (pioStatus_ & 0x20) >> 5;
    setLed(PIO_STB_A, mode);
    dataSwitchButton[PIO_STB_A]->enable(dc, pioStbAMode_ == PIO_INPUT);
    ioSwitchEnabled_[PIO_STB_A] = (pioStbAMode_ == PIO_INPUT);
    dataSwitchButton[PIO_STB_A]->setState(dc, (pioStatus_ & 0x20) == 0x20);
    ioSwitchState_[PIO_STB_A] = (pioStatus_ & 0x20) >> 5;
    mode = ((pioRdyAMode_^1) << 1) & 0x3;
    if (mode == 0)
        mode = (pioStatus_ & 0x10) >> 4;
    setLed(PIO_RDY_A, mode);
    dataSwitchButton[PIO_RDY_A]->enable(dc, pioRdyAMode_ == PIO_INPUT);
    ioSwitchEnabled_[PIO_RDY_A] = (pioRdyAMode_ == PIO_INPUT);
    dataSwitchButton[PIO_RDY_A]->setState(dc, (pioStatus_ & 0x10) == 0x10);
    ioSwitchState_[PIO_RDY_A] = (pioStatus_ & 0x10) >> 4;
}

void PioScreen::enableStbRdyB(wxDC& dc)
{
    Byte mode = ((pioStbBMode_^1) << 1) & 0x3;
    if (mode == 0)
        mode = (pioStatus_ & 0x80) >> 7;
    setLed(PIO_STB_B, mode);
    dataSwitchButton[PIO_STB_B]->enable(dc, pioStbBMode_ == PIO_INPUT);
    ioSwitchEnabled_[PIO_STB_B] = (pioStbBMode_ == PIO_INPUT);
    dataSwitchButton[PIO_STB_B]->setState(dc, (pioStatus_ & 0x80) == 0x80);
    ioSwitchState_[PIO_STB_B] = (pioStatus_ & 0x80) >> 7;
    mode = ((pioRdyBMode_^1) << 1) & 0x3;
    if (mode == 0)
        mode = (pioStatus_ & 0x40) >> 6;
    setLed(PIO_RDY_B, mode);
    dataSwitchButton[PIO_RDY_B]->enable(dc, pioRdyBMode_ == PIO_INPUT);
    ioSwitchEnabled_[PIO_RDY_B] = (pioRdyBMode_ == PIO_INPUT);
    dataSwitchButton[PIO_RDY_B]->setState(dc, (pioStatus_ & 0x40) == 0x40);
    ioSwitchState_[PIO_RDY_B] = (pioStatus_ & 0x40) >> 6;
}

void PioScreen::disableStbRdyA(wxDC& dc)
{
    setLed(PIO_STB_A, 2);
    dataSwitchButton[PIO_STB_A]->enable(dc, false);
    ioSwitchEnabled_[PIO_STB_A] = false;
    setLed(PIO_RDY_A, 2);
    dataSwitchButton[PIO_RDY_A]->enable(dc, false);
    ioSwitchEnabled_[PIO_RDY_A] = false;
}

void PioScreen::disableStbRdyB(wxDC& dc)
{
    setLed(PIO_STB_B, 2);
    dataSwitchButton[PIO_STB_B]->enable(dc, false);
    ioSwitchEnabled_[PIO_STB_B] = false;
    setLed(PIO_RDY_B, 2);
    dataSwitchButton[PIO_RDY_B]->enable(dc, false);
    ioSwitchEnabled_[PIO_RDY_B] = false;
}

#if defined (__WXMAC__)
void PioScreen::enableInterruptButton(wxDC& dc)
#else
void PioScreen::enableInterruptButton(wxDC& WXUNUSED(dc))
#endif
{
    switch (pioBMode_)
    {
        case PIO_OUTPUT:
        case PIO_INPUT:
            pioBInterruptButtonEnabled_ = pioBInterruptEnabled_;
        break;

        case PIO_BIT_PROG:
            pioBInterruptButtonEnabled_ = false;
        break;
    }
    
    switch (pioAMode_)
    {
        case PIO_OUTPUT:
        case PIO_INPUT:
            pioAInterruptButtonEnabled_ = pioAInterruptEnabled_;
        break;
            
        case PIO_BIT_PROG:
            pioAInterruptButtonEnabled_ = false;
        break;
            
        case PIO_BI_DRECT:
            pioAInterruptButtonEnabled_ = pioAInterruptEnabled_;
            pioBInterruptButtonEnabled_ = pioAInterruptEnabled_;
        break;
    }

    if (pioAInterruptButtonEnabled_ != pioAInterruptButtonState_)
    {
#if defined (__WXMAC__)
        osx_ardyButtonPointer->enable(dc, pioAInterruptButtonEnabled_);
#else
        if (pioAInterruptButtonEnabled_)
            text_ardyButtonPointer->Enable();
        else
            text_ardyButtonPointer->Disable();
#endif
        pioAInterruptButtonState_ = pioAInterruptButtonEnabled_;
    }

    if (pioBInterruptButtonEnabled_ != pioBInterruptButtonState_)
    {
#if defined (__WXMAC__)
        osx_brdyButtonPointer->enable(dc, pioBInterruptButtonEnabled_);
#else
        if (pioBInterruptButtonEnabled_)
            text_brdyButtonPointer->Enable();
        else
            text_brdyButtonPointer->Disable();
#endif
        pioBInterruptButtonState_ = pioBInterruptButtonEnabled_;
    }
}

void PioScreen::writePortA(Byte value)
{
    if (pioAMode_ == PIO_OUTPUT || pioAMode_ == PIO_BI_DRECT)
    {
        outPutValueA_ = value;
        for (int i=0; i<8; i++)
        {
            setLed(i, value&1);
            value = value >> 1;
        }
        if (pioAInterruptEnabled_)
            p_Computer->interrupt();
    }
    if (pioAMode_ == PIO_BIT_PROG)
    {
        Byte progBits = pioAProgBits_;
        outPutValueA_ &= (progBits ^ 0xff);
        value &= progBits;
        outPutValueA_ |= value;
        for (int i=0; i<8; i++)
        {
            if (progBits&1)
                setLed(i, value&1);
            progBits = progBits >> 1;
            value = value >> 1;
        }
    }
}

void PioScreen::writePortB(Byte value)
{
    if (pioBMode_ == PIO_OUTPUT || pioBMode_ == PIO_BI_DRECT)
    {
        outPutValueB_ = value;
        for (int i=8; i<16; i++)
        {
            setLed(i, value&1);
            value = value >> 1;
        }
        if (pioBInterruptEnabled_)
            p_Computer->interrupt();
    }
    if (pioBMode_ == PIO_BIT_PROG)
    {
        Byte progBits = pioBProgBits_;
        outPutValueB_ &= (progBits ^ 0xff);
        value &= progBits;
        outPutValueB_ |= value;
        for (int i=8; i<16; i++)
        {
            if (progBits&1)
                setLed(i, value&1);
            progBits = progBits >> 1;
            value = value >> 1;
        }
    }
}

Byte PioScreen::readPortA()
{
    p_Computer->setEfState(pioNumber_, 1, 1);
    pioStatus_ &= 0xFD;
 
    if (pioAMode_ == PIO_BI_DRECT)
        return inPutValueA_;
    else
        return outPutValueA_;
}

Byte PioScreen::readPortB()
{
    p_Computer->setEfState(pioNumber_, 2, 1);
    pioStatus_ &= 0xFE;

    return outPutValueB_;
}

void PioScreen::maskInterruptA()
{
    Byte unmaskedBits = outPutValueA_ & pioAInterruptMask_;
    switch (pioAInterruptLogical_)
    {
        case 0:     // Interrupt if any 'unmasked' bit is low
            if ((unmaskedBits ^ pioAInterruptMask_) != 0)
                p_Computer->interrupt();
        break;

        case 0x1:   // Interrupt if any 'unmasked' bit is high
            if ((unmaskedBits & pioAInterruptMask_) != 0)
                p_Computer->interrupt();
        break;

        case 0x2:   // Interrupt if all 'unmasked' bit are low
            if ((unmaskedBits & pioAInterruptMask_) == 0)
                p_Computer->interrupt();
        break;

        case 0x3:   // Interrupt if all 'unmasked' bit are high
            if ((unmaskedBits & pioAInterruptMask_) == pioAInterruptMask_)
                p_Computer->interrupt();
        break;
    }
}

void PioScreen::maskInterruptB()
{
    Byte unmaskedBits = outPutValueB_ & pioBInterruptMask_;
    switch (pioBInterruptLogical_)
    {
        case 0:     // Interrupt if any 'unmasked' bit is low
            if ((unmaskedBits ^ pioBInterruptMask_) != 0)
                p_Computer->interrupt();
        break;
            
        case 0x1:   // Interrupt if any 'unmasked' bit is high
            if ((unmaskedBits & pioBInterruptMask_) != 0)
                p_Computer->interrupt();
        break;
            
        case 0x2:   // Interrupt if all 'unmasked' bit are low
            if ((unmaskedBits & pioBInterruptMask_) == 0)
                p_Computer->interrupt();
        break;
            
        case 0x3:   // Interrupt if all 'unmasked' bit are high
            if ((unmaskedBits & pioBInterruptMask_) == pioBInterruptMask_)
                p_Computer->interrupt();
        break;
    }
}

Byte PioScreen::readStatusRegister()
{
    if (pioRdyAMode_ == PIO_INPUT && pioAMode_ == PIO_BIT_PROG)
    {
        pioStatus_ &= 0xef;
        pioStatus_ |= ioSwitchState_[PIO_RDY_A] << 4;
    }
    if (pioStbAMode_ == PIO_INPUT && pioAMode_ == PIO_BIT_PROG)
    {
        pioStatus_ &= 0xdf;
        pioStatus_ |= ioSwitchState_[PIO_STB_A] << 5;
    }
    if (pioRdyBMode_ == PIO_INPUT && pioBMode_ == PIO_BIT_PROG)
    {
        pioStatus_ &= 0xbf;
        pioStatus_ |= ioSwitchState_[PIO_RDY_B] << 6;
    }
    if (pioStbBMode_ == PIO_INPUT && pioBMode_ == PIO_BIT_PROG)
    {
        pioStatus_ &= 0x7f;
        pioStatus_ |= ioSwitchState_[PIO_STB_B] << 7;
    }
    if (pioAMode_ != PIO_BIT_PROG)
        pioStatus_ &= 0xcf;
    if (pioBMode_ != PIO_BIT_PROG)
        pioStatus_ &= 0x3f;
    return pioStatus_;
}

void PioScreen::refreshLeds()
{
    wxClientDC dc(this);

    for (int i=0; i<20; i++)
        refreshLed(dc, i);
}

BEGIN_EVENT_TABLE(PioFrame, wxFrame)
    EVT_CLOSE (PioFrame::onClose)
    EVT_BUTTON(1, PioFrame::onArdyButton)
    EVT_BUTTON(2, PioFrame::onBrdyButton)
END_EVENT_TABLE()

PioFrame::PioFrame(const wxString& title, const wxPoint& pos, const wxSize& size, int pioNumber)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    pioNumber_ = pioNumber;
    
    pioScreenPointer = new PioScreen(this, size, pioNumber, TILNONE);
    pioScreenPointer->init();
    
    this->SetClientSize(size);
    
#ifndef __WXMAC__
    SetIcon(wxICON(app_icon));
#endif
}

PioFrame::~PioFrame()
{
    delete pioScreenPointer;
}

void PioFrame::onClose(wxCloseEvent&WXUNUSED(event))
{
    p_Computer->removePio(pioNumber_);
}

void PioFrame::onArdyButton(wxCommandEvent&WXUNUSED(event))
{
    pioScreenPointer->onArdyButton();
};

void PioFrame::onBrdyButton(wxCommandEvent&WXUNUSED(event)) 
{
    pioScreenPointer->onBrdyButton();
};

void PioFrame::refreshPanel()
{
    pioScreenPointer->refreshPanel();
}
