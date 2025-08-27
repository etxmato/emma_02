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
#include "cdp1851.h"

Cdp1851Printer::Cdp1851Printer(int pioNumber, Cdp1851Configuration cdp1851Configuration, Cdp1851PrinterConfiguration cdp1851PrinterConfiguration, double clock)
{
    pioNumber_ = pioNumber;
    cdp1851Configuration_ = cdp1851Configuration;
    cdp1851PrinterConfiguration_ = cdp1851PrinterConfiguration;
    
    select_in_ = 1;
    error_ = 1;
    paper_out_ = 0;
    auto_feed_ = false;

    printerBusy_ =  0;
    printerAck_ = 0;
    busyCycleValue_ = -1;
    ackCycleValue_ = -1;
    
    busyCycleSize_ = (((clock * 1000000) / 8) / 1000) * cdp1851PrinterConfiguration_.busyCycleTime;
    ackCycleSize_ = (((clock * 1000000) / 8) / 1000) * cdp1851PrinterConfiguration_.ackCycleTime;
}

Cdp1851Printer::~Cdp1851Printer()
{
}

void Cdp1851Printer::init(Cdp1851Instance *cdp1851InstancePointer)
{
    cdp1851InstancePointer_ = cdp1851InstancePointer;
    
    p_Printer = new Printer();
    p_Printer->init(p_Printer, PRINTER_BASIC);
}

void Cdp1851Printer::cycle()
{
    if (busyCycleValue_ > 0)
    {
        busyCycleValue_--;
        if (busyCycleValue_ == 0)
        {
            ackCycleValue_ = ackCycleSize_;
            printerAck_ = 0x20;
        }
    }
    if (ackCycleValue_ > 0)
    {
        ackCycleValue_--;
        if (ackCycleValue_ == 0)
        {
            printerBusy_ = 0;
            printerAck_ = 0;
        }
    }
}

void Cdp1851Printer::initPort(int port, Byte value)
{
    portValue[port] = value;
}

void Cdp1851Printer::writePort(int port, Byte value)
{
    portValue[port] = value;
    
    switch (cdp1851PrinterConfiguration_.portFunction[port])
    {
        case CDP1851_PRINTER_FUNC_LATCH:
            printLatch_ = value;
        break;

        case CDP1851_PRINTER_FUNC_IO:
            initIo(port, value);
            selectOutIo(port, value);
       break;
    }
}

Byte Cdp1851Printer::readPort(int port)
{
    switch (cdp1851PrinterConfiguration_.portFunction[port])
    {
        case CDP1851_PRINTER_FUNC_IO:
            portValue[port] = selectInIo(port, portValue[port]);
            portValue[port] = errorIo(port, portValue[port]);
            portValue[port] = paperOutIo(port, portValue[port]);
        break;
    }

    return portValue[port];
}

void Cdp1851Printer::rdyControl(int port, Byte value, int rdyMode)
{
    int rdy = (value & 0x10) >> 4;
    
    if (rdyMode == 1)
    {
        cdp1851InstancePointer_->setPioEfState(port, rdy);

        switch (cdp1851PrinterConfiguration_.portRdyFunction[port])
        {
            case CDP1851_PRINTER_FUNC_INIT:
                init(rdy);
            break;
                
            case CDP1851_PRINTER_FUNC_SELECT_OUT:
                selectOut(rdy);
            break;
                
            case CDP1851_PRINTER_FUNC_STROBE:
                strobe(rdy);
            break;

            case CDP1851_PRINTER_FUNC_AUTO_LF:
                autoLf(rdy);
            break;
        }
    }
}

void Cdp1851Printer::strobeControl(int port, Byte value, int strobeMode)
{
    int strobeValue = (value & 0x20) >> 5;
    
    if (strobeMode == 1)
    {
        cdp1851InstancePointer_->setPioEfState(port, strobeValue);

        switch (cdp1851PrinterConfiguration_.portStrobeFunction[port])
        {
            case CDP1851_PRINTER_FUNC_INIT:
                init(strobeValue);
            break;
                
            case CDP1851_PRINTER_FUNC_SELECT_OUT:
                selectOut(strobeValue);
            break;
                
            case CDP1851_PRINTER_FUNC_STROBE:
                strobe(strobeValue);
            break;

            case CDP1851_PRINTER_FUNC_AUTO_LF:
                autoLf(strobeValue);
            break;
        }
    }
}

void Cdp1851Printer::initIo(int port, Byte value)
{
    if (port != cdp1851PrinterConfiguration_.init.portNumber)
        return;
    
    int initValue = (value & (1 << cdp1851PrinterConfiguration_.init.bitNumber)) >> cdp1851PrinterConfiguration_.init.bitNumber;
    
    init(initValue);
}

void Cdp1851Printer::selectOutIo(int port, Byte value)
{
    if (port != cdp1851PrinterConfiguration_.selectOut.portNumber)
        return;

    int selectOutValue = (value & (1 << cdp1851PrinterConfiguration_.selectOut.bitNumber)) >> cdp1851PrinterConfiguration_.selectOut.bitNumber;
    
    selectOut(selectOutValue);
}

void Cdp1851Printer::strobeIo(int port, Byte value)
{
    if (port != cdp1851PrinterConfiguration_.strobe.portNumber)
        return;

    int strobeValue = (value & (1 << cdp1851PrinterConfiguration_.strobe.bitNumber)) >> cdp1851PrinterConfiguration_.strobe.bitNumber;
    
    strobe(strobeValue);
}

void Cdp1851Printer::autoLfIo(int port, Byte value)
{
    if (port != cdp1851PrinterConfiguration_.autoLf.portNumber)
        return;

    int autoLfValue = (value & (1 << cdp1851PrinterConfiguration_.autoLf.bitNumber)) >> cdp1851PrinterConfiguration_.autoLf.bitNumber;
    
    autoLf(autoLfValue);
}

Byte Cdp1851Printer::selectInIo(int port, Byte value)
{
    if (port != cdp1851PrinterConfiguration_.selectIn.portNumber)
        return 0;

    Byte mask = (1 << cdp1851PrinterConfiguration_.selectIn.bitNumber) ^ 0xff;
    value &= mask;
    value |= select_in_ << cdp1851PrinterConfiguration_.selectIn.bitNumber;
    return value;
}

Byte Cdp1851Printer::errorIo(int port, Byte value)
{
    if (port != cdp1851PrinterConfiguration_.error.portNumber)
        return 0;

    Byte mask = (1 << cdp1851PrinterConfiguration_.error.bitNumber) ^ 0xff;
    value &= mask;
    value |= error_ << cdp1851PrinterConfiguration_.error.bitNumber;
    return value;
}

Byte Cdp1851Printer::paperOutIo(int port, Byte value)
{
    if (port != cdp1851PrinterConfiguration_.paperOut.portNumber)
        return 0;

    Byte mask = (1 << cdp1851PrinterConfiguration_.paperOut.bitNumber) ^ 0xff;
    value &= mask;
    value |= paper_out_ << cdp1851PrinterConfiguration_.paperOut.bitNumber;
    return value;
}

void Cdp1851Printer::init(Byte value)
{
    if (value == 0)
    {
        printerBusy_ =  0;
        printerAck_ = 0;
        busyCycleValue_ = -1;
        ackCycleValue_ = -1;
    }
}

void Cdp1851Printer::selectOut(Byte value)
{
    if (value == 0)
    {
    }
}

void Cdp1851Printer::strobe(Byte value)
{
    if (value == 1)
    {
        p_Printer->printerOut(printLatch_);
        printerBusy_ = 0x80;
        busyCycleValue_ = busyCycleSize_;
    }
}

void Cdp1851Printer::autoLf(Byte value)
{
    auto_feed_ = (value == 0);
}

Byte Cdp1851Printer::readStatusRegister(Byte pioStatus)
{
    return pioStatus | printerBusy_ | printerAck_;
}


Cdp1851Screen::Cdp1851Screen(wxWindow *parent, const wxSize& size, int pioNumber, Cdp1851Configuration cdp1851Configuration)
: Panel(parent, size)
{
    pioNumber_ = pioNumber;
    cdp1851Configuration_ = cdp1851Configuration;
        
//    this->SetClientSize(size);
}

Cdp1851Screen::~Cdp1851Screen()
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

void Cdp1851Screen::init(Cdp1851Instance *cdp1851InstancePointer)
{
    cdp1851InstancePointer_ = cdp1851InstancePointer;
    wxClientDC dc(this);
    
    for (int i=0; i<8; i++)
    {
        ledPointer[i] = new Led(dc, 33+23*(7-i), 30, LED_SMALL_RED_DISABLE);
        updateLed_[i]=true;
        setLed(i, 2);
        dataSwitchButton[i] = new SwitchButton(dc, SWITCH_BUTTON_VERTICAL_PIO, wxColour(255, 255, 255), BUTTON_DOWN, 27+23*(7-i), 50, "");
        ioSwitchState_[i] = 0;
        ioSwitchEnabled_[i] = true;
    }
    for (int i=8; i<16; i++)
    {
        ledPointer[i] = new Led(dc, 33+23*(15-i), 90, LED_SMALL_RED_DISABLE);
        updateLed_[i]=true;
        setLed(i, 2);
        dataSwitchButton[i] = new SwitchButton(dc, SWITCH_BUTTON_VERTICAL_PIO, wxColour(255, 255, 255), BUTTON_DOWN, 27+23*(15-i), 110, "");
        ioSwitchState_[i] = 0;
        ioSwitchEnabled_[i] = true;
    }
    for (int i=16; i<18; i++)
    {
        ledPointer[i] = new Led(dc, 229+37*(i-16), 30, LED_SMALL_RED_DISABLE);
        updateLed_[i]=true;
        setLed(i, 2);
        dataSwitchButton[i] = new SwitchButton(dc, SWITCH_BUTTON_VERTICAL_PIO, wxColour(255, 255, 255), BUTTON_DOWN, 223+37*(i-16), 50, "");
        ioSwitchState_[i] = 0;
        ioSwitchEnabled_[i] = false;
        dataSwitchButton[i]->enable(dc, false);
    }
    for (int i=18; i<20; i++)
    {
        ledPointer[i] = new Led(dc, 229+37*(i-18), 90, LED_SMALL_RED_DISABLE);
        updateLed_[i]=true;
        setLed(i, 2);
        dataSwitchButton[i] = new SwitchButton(dc, SWITCH_BUTTON_VERTICAL_PIO, wxColour(255, 255, 255), BUTTON_DOWN, 223+37*(i-18), 110, "");
        ioSwitchState_[i] = 0;
        ioSwitchEnabled_[i] = false;
        dataSwitchButton[i]->enable(dc, false);
    }
#if defined (__WXMAC__)
    osx_ardyButtonPointer = new HexButtonCdp1851(dc, PUSH_BUTTON_PIO, 55, 144, "A", pioNumber_);
    osx_brdyButtonPointer = new HexButtonCdp1851(dc, PUSH_BUTTON_PIO, 95, 144, "B", pioNumber_);
#else
    text_ardyButtonPointer = new wxButton(this, 1, "A", wxPoint(55, 144), wxSize(25, 25), 0, wxDefaultValidator, "ArdyButton");
    text_ardyButtonPointer->SetToolTip("ARDY");
    text_brdyButtonPointer = new wxButton(this, 2, "B", wxPoint(95, 144), wxSize(25, 25), 0, wxDefaultValidator, "BrdyButton");
    text_brdyButtonPointer->SetToolTip("BRDY");
#endif

    this->connectKeyEvent(this);
}

void Cdp1851Screen::onPaint(wxPaintEvent&WXUNUSED(event))
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

void Cdp1851Screen::onMousePress(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);
    
    wxClientDC dc(this);

    if (cdp1851InstancePointer_->getPioAInterruptEnabled())
    {
        if (osx_ardyButtonPointer->onMousePress(dc, x, y))
            onArdyButton();
    }

    if (cdp1851InstancePointer_->getPioBInterruptEnabled())
    {
        if (osx_brdyButtonPointer->onMousePress(dc, x, y))
            onBrdyButton();
    }
}

void Cdp1851Screen::onMouseRelease(wxMouseEvent&event)
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
    if (cdp1851InstancePointer_->getPioAInterruptEnabled())
        osx_ardyButtonPointer->onMouseRelease(dc, x, y);
    if (cdp1851InstancePointer_->getPioBInterruptEnabled())
        osx_brdyButtonPointer->onMouseRelease(dc, x, y);
#endif
}

void Cdp1851Screen::releaseButtonOnScreen(HexButton* buttonPoint)
{
    wxClientDC dc(this);

    buttonPoint->releaseButtonOnScreen(dc);
}

void Cdp1851Screen::onArdyButton()
{
    cdp1851InstancePointer_->onRdyA();
}

void Cdp1851Screen::onBrdyButton()
{
    cdp1851InstancePointer_->onRdyB();
}

void Cdp1851Screen::ioSwitch(int i)
{
    if (ioSwitchState_[i])
        ioSwitchState_[i] = 0;
    else
        ioSwitchState_[i] = 1;
    
    if (i<8)
    {
        if (cdp1851InstancePointer_->getPioAMode() == PIO_INPUT)
            cdp1851InstancePointer_->setOutputValueA(i, ioSwitchState_[i]);
        if (cdp1851InstancePointer_->getPioAMode() == PIO_BI_DRECT)
            cdp1851InstancePointer_->setOutputValueA(i, ioSwitchState_[i]);
        if (cdp1851InstancePointer_->getPioAMode() == PIO_BIT_PROG)
        {
            Byte bitmask = 1 << i;
            if ((cdp1851InstancePointer_->getPioAProgBits() & bitmask) == 0)
                cdp1851InstancePointer_->setOutputValueA(i, ioSwitchState_[i]);
        }
    }
    if (i>=8 && i<16)
    {
        if (cdp1851InstancePointer_->getPioBMode() == PIO_INPUT || cdp1851InstancePointer_->getPioBMode() == PIO_BI_DRECT)
            cdp1851InstancePointer_->setOutputValueB(i-8, ioSwitchState_[i]);
        if (cdp1851InstancePointer_->getPioBMode() == PIO_BIT_PROG)
        {
            Byte bitmask = 1 << (i-8);
            if ((cdp1851InstancePointer_->getPioBProgBits() & bitmask) == 0)
                cdp1851InstancePointer_->setOutputValueB(i-8, ioSwitchState_[i]);
        }
    }
}

void Cdp1851Screen::reset()
{
    wxClientDC dc(this);
    
    pioAInterruptButtonEnabled_ = false;
    pioBInterruptButtonEnabled_ = false;
    pioAInterruptButtonState_ = false;
    pioBInterruptButtonState_ = false;

#if defined (__WXMAC__)
    osx_ardyButtonPointer->enable(dc, false);
    osx_brdyButtonPointer->enable(dc, false);
#else
    text_ardyButtonPointer->Disable();
    text_brdyButtonPointer->Disable();
#endif
}

void Cdp1851Screen::clearA()
{
    wxClientDC dc(this);

    for (int i=0; i<8; i++)
    {
        dataSwitchButton[i]->setState(dc, 0);
        ioSwitchState_[i] = 0;
    }
}

void Cdp1851Screen::clearB()
{
    wxClientDC dc(this);
    
    for (int i=8; i<16; i++)
    {
        dataSwitchButton[i]->setState(dc, 0);
        ioSwitchState_[i] = 0;
    }
}

void Cdp1851Screen::clearStbRdy()
{
    wxClientDC dc(this);
    
    for (int i=16; i<20; i++)
    {
        dataSwitchButton[i]->setState(dc, 0);
        ioSwitchState_[i] = 0;
    }
}

void Cdp1851Screen::writeControlRegister(Byte value, int commandByteNumber)
{
    wxClientDC dc(this);
    Byte outputValue, inputValue;
    
    if (commandByteNumber == PIO_COMMAND_NONE)
    {
        switch (value & 0xc7)
        {
            case 0x3: // Set input
            case 0x7:
                if (value & 0x8)
                {   // Port A
                    outputValue = cdp1851InstancePointer_->getOutputValueA();
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
                    outputValue = cdp1851InstancePointer_->getOutputValueB();
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
                    outputValue = cdp1851InstancePointer_->getOutputValueA();
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
                    outputValue = cdp1851InstancePointer_->getOutputValueB();
                    for (int i=8; i<16; i++)
                    {
                        setLed(i, outputValue&1);
                        outputValue = outputValue >> 1;
                        dataSwitchButton[i]->enable(dc, false);
                        ioSwitchEnabled_[i] = false;
                    }
                }
            break;
                                
            case 0x83: // Set bi directional
            case 0x87:
                if (value & 0x8 && cdp1851InstancePointer_->getPioBMode() == PIO_BIT_PROG)
                {   // Port A & port B need to be set in bit programmable
                    outputValue = cdp1851InstancePointer_->getOutputValueA();
                    inputValue = cdp1851InstancePointer_->getInputValueA();
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
        }
        enableInterruptButton(dc);
        if ((value & 0x1) == 0) // STB and RDY Control
        {
            cdp1851InstancePointer_->readStatusRegister();
            switch (value & 0xe)
            {
                case 0x00: // Port A, output STB/RDY
                    if (cdp1851InstancePointer_->getPioRdyAMode() == PIO_OUTPUT)
                        setLed(PIO_RDY_A, (value & 0x10) >> 4);
                    if (cdp1851InstancePointer_->getPioStbAMode() == PIO_OUTPUT)
                        setLed(PIO_STB_A, (value & 0x20) >> 5);
                break;
                    
                case 0x02: // Port B, output STB/RDY
                    if (cdp1851InstancePointer_->getPioRdyBMode() == PIO_OUTPUT)
                        setLed(PIO_RDY_B, (value & 0x10) >> 4);
                    if (cdp1851InstancePointer_->getPioStbBMode() == PIO_OUTPUT)
                        setLed(PIO_STB_B, (value & 0x20) >> 5);
               break;
            }
        }
    }
}

void Cdp1851Screen::setProgBitsA(Byte value)
{
    wxClientDC dc(this);

    Byte outPutValue = cdp1851InstancePointer_->getOutputValueA();
    
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
}

void Cdp1851Screen::setProgBitsB(Byte value)
{
    wxClientDC dc(this);

    Byte outPutValue = cdp1851InstancePointer_->getOutputValueB();

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
}

void Cdp1851Screen::enableStbRdy(int port, Byte pioStatus, int pioStbMode, int pioRdyMode)
{
    if (port == 0)
        enableStbRdyA(pioStatus, pioStbMode, pioRdyMode);
    else
        enableStbRdyB(pioStatus, pioStbMode, pioRdyMode);
}

void Cdp1851Screen::enableStbRdyA(Byte pioStatus, int pioStbAMode, int pioRdyAMode)
{
    wxClientDC dc(this);

    Byte mode = ((pioStbAMode^1) << 1) & 0x3;
    if (mode == 0)
        mode = (pioStatus & 0x20) >> 5;
    setLed(PIO_STB_A, mode);
    dataSwitchButton[PIO_STB_A]->enable(dc, pioStbAMode == PIO_INPUT);
    ioSwitchEnabled_[PIO_STB_A] = (pioStbAMode == PIO_INPUT);
    dataSwitchButton[PIO_STB_A]->setState(dc, (pioStatus & 0x20) == 0x20);
    ioSwitchState_[PIO_STB_A] = (pioStatus & 0x20) >> 5;
    mode = ((pioRdyAMode^1) << 1) & 0x3;
    if (mode == 0)
        mode = (pioStatus & 0x10) >> 4;
    setLed(PIO_RDY_A, mode);
    dataSwitchButton[PIO_RDY_A]->enable(dc, pioRdyAMode == PIO_INPUT);
    ioSwitchEnabled_[PIO_RDY_A] = (pioRdyAMode == PIO_INPUT);
    dataSwitchButton[PIO_RDY_A]->setState(dc, (pioStatus & 0x10) == 0x10);
    ioSwitchState_[PIO_RDY_A] = (pioStatus & 0x10) >> 4;
}

void Cdp1851Screen::enableStbRdyB(Byte pioStatus, int pioStbBMode, int pioRdyBMode)
{
    wxClientDC dc(this);

    Byte mode = ((pioStbBMode^1) << 1) & 0x3;
    if (mode == 0)
        mode = (pioStatus & 0x80) >> 7;
    setLed(PIO_STB_B, mode);
    dataSwitchButton[PIO_STB_B]->enable(dc, pioStbBMode == PIO_INPUT);
    ioSwitchEnabled_[PIO_STB_B] = (pioStbBMode == PIO_INPUT);
    dataSwitchButton[PIO_STB_B]->setState(dc, (pioStatus & 0x80) == 0x80);
    ioSwitchState_[PIO_STB_B] = (pioStatus & 0x80) >> 7;
    mode = ((pioRdyBMode^1) << 1) & 0x3;
    if (mode == 0)
        mode = (pioStatus & 0x40) >> 6;
    setLed(PIO_RDY_B, mode);
    dataSwitchButton[PIO_RDY_B]->enable(dc, pioRdyBMode == PIO_INPUT);
    ioSwitchEnabled_[PIO_RDY_B] = (pioRdyBMode == PIO_INPUT);
    dataSwitchButton[PIO_RDY_B]->setState(dc, (pioStatus & 0x40) == 0x40);
    ioSwitchState_[PIO_RDY_B] = (pioStatus & 0x40) >> 6;
}

void Cdp1851Screen::disableStbRdyA(wxDC& dc)
{
    setLed(PIO_STB_A, 2);
    dataSwitchButton[PIO_STB_A]->enable(dc, false);
    ioSwitchEnabled_[PIO_STB_A] = false;
    setLed(PIO_RDY_A, 2);
    dataSwitchButton[PIO_RDY_A]->enable(dc, false);
    ioSwitchEnabled_[PIO_RDY_A] = false;
}

void Cdp1851Screen::disableStbRdyB(wxDC& dc)
{
    setLed(PIO_STB_B, 2);
    dataSwitchButton[PIO_STB_B]->enable(dc, false);
    ioSwitchEnabled_[PIO_STB_B] = false;
    setLed(PIO_RDY_B, 2);
    dataSwitchButton[PIO_RDY_B]->enable(dc, false);
    ioSwitchEnabled_[PIO_RDY_B] = false;
}

#if defined (__WXMAC__)
void Cdp1851Screen::enableInterruptButton(wxDC& dc)
#else
void Cdp1851Screen::enableInterruptButton(wxDC& WXUNUSED(dc))
#endif
{
    switch (cdp1851InstancePointer_->getPioBMode())
    {
        case PIO_OUTPUT:
        case PIO_INPUT:
            pioBInterruptButtonEnabled_ = cdp1851InstancePointer_->getPioBInterruptEnabled();
        break;

        case PIO_BIT_PROG:
            pioBInterruptButtonEnabled_ = false;
        break;
    }
    
    switch (cdp1851InstancePointer_->getPioAMode())
    {
        case PIO_OUTPUT:
        case PIO_INPUT:
            pioAInterruptButtonEnabled_ = cdp1851InstancePointer_->getPioAInterruptEnabled();
        break;
            
        case PIO_BIT_PROG:
            pioAInterruptButtonEnabled_ = false;
        break;
            
        case PIO_BI_DRECT:
            pioAInterruptButtonEnabled_ = cdp1851InstancePointer_->getPioAInterruptEnabled();
            pioBInterruptButtonEnabled_ = cdp1851InstancePointer_->getPioAInterruptEnabled();
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

void Cdp1851Screen::writePortA(Byte value)
{
    if (cdp1851InstancePointer_->getPioAMode() == PIO_OUTPUT || cdp1851InstancePointer_->getPioAMode() == PIO_BI_DRECT)
    {
        for (int i=0; i<8; i++)
        {
            setLed(i, value&1);
            value = value >> 1;
        }
    }
    if (cdp1851InstancePointer_->getPioAMode() == PIO_BIT_PROG)
    {
        Byte progBits = cdp1851InstancePointer_->getPioAProgBits();
        value &= progBits;
        for (int i=0; i<8; i++)
        {
            if (progBits&1)
                setLed(i, value&1);
            progBits = progBits >> 1;
            value = value >> 1;
        }
    }
}

void Cdp1851Screen::writePortB(Byte value)
{
    if (cdp1851InstancePointer_->getPioBMode() == PIO_OUTPUT || cdp1851InstancePointer_->getPioBMode() == PIO_BI_DRECT)
    {
        for (int i=8; i<16; i++)
        {
            setLed(i, value&1);
            value = value >> 1;
        }
    }
    if (cdp1851InstancePointer_->getPioBMode() == PIO_BIT_PROG)
    {
        Byte progBits = cdp1851InstancePointer_->getPioBProgBits();
        value &= progBits;
        for (int i=8; i<16; i++)
        {
            if (progBits&1)
                setLed(i, value&1);
            progBits = progBits >> 1;
            value = value >> 1;
        }
    }
}

Byte Cdp1851Screen::readStatusRegister(Byte pioStatus)
{
    if (cdp1851InstancePointer_->getPioRdyAMode() == PIO_INPUT && cdp1851InstancePointer_->getPioAMode() == PIO_BIT_PROG)
    {
        pioStatus |= ioSwitchState_[PIO_RDY_A] << 4;
    }
    if (cdp1851InstancePointer_->getPioStbAMode() == PIO_INPUT && cdp1851InstancePointer_->getPioAMode() == PIO_BIT_PROG)
    {
        pioStatus |= ioSwitchState_[PIO_STB_A] << 5;
    }
    if (cdp1851InstancePointer_->getPioRdyBMode() == PIO_INPUT && cdp1851InstancePointer_->getPioBMode() == PIO_BIT_PROG)
    {
        pioStatus |= ioSwitchState_[PIO_RDY_B] << 6;
    }
    if (cdp1851InstancePointer_->getPioStbBMode() == PIO_INPUT && cdp1851InstancePointer_->getPioBMode() == PIO_BIT_PROG)
    {
        pioStatus |= ioSwitchState_[PIO_STB_B] << 7;
    }
    return pioStatus;
}

void Cdp1851Screen::refreshLeds()
{
    wxClientDC dc(this);

    for (int i=0; i<20; i++)
        refreshLed(dc, i);
}

BEGIN_EVENT_TABLE(Cdp1851Instance, wxFrame)
    EVT_CLOSE (Cdp1851Instance::onClose)
    EVT_BUTTON(1, Cdp1851Instance::onArdyButton)
    EVT_BUTTON(2, Cdp1851Instance::onBrdyButton)
END_EVENT_TABLE()

Cdp1851Instance::Cdp1851Instance(const wxString& title, const wxPoint& pos, const wxSize& size, int pioNumber, Cdp1851Configuration cdp1851Configuration)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    pioNumber_ = pioNumber;
    cdp1851Configuration_ = cdp1851Configuration;

    pioScreenPointer = new Cdp1851Screen(this, size, pioNumber, cdp1851Configuration);
    this->SetClientSize(size);
#ifndef __WXMAC__
    SetIcon(wxICON(app_icon));
#endif
    init();
}

Cdp1851Instance::Cdp1851Instance(int pioNumber, Cdp1851Configuration cdp1851Configuration, Cdp1851PrinterConfiguration cdp1851PrinterConfiguration, double clock)
{
    pioNumber_ = pioNumber;
    cdp1851Configuration_ = cdp1851Configuration;
    
    if (cdp1851Configuration_.connection == PIO_CONNECTION_PRINTER)
        pioPrinterPointer = new Cdp1851Printer(pioNumber, cdp1851Configuration, cdp1851PrinterConfiguration, clock);
    
    init();
}

Cdp1851Instance::~Cdp1851Instance()
{
    if (cdp1851Configuration_.connection == PIO_CONNECTION_WINDOW)
        delete pioScreenPointer;
}

void Cdp1851Instance::init()
{
    switch (cdp1851Configuration_.connection)
    {
        case PIO_CONNECTION_WINDOW:
            pioScreenPointer->init(this);
        break;
            
        case PIO_CONNECTION_PRINTER:
            pioPrinterPointer->init(this);
            pioPrinterPointer->initPort(0, cdp1851Configuration_.initPortA);
            pioPrinterPointer->initPort(1, cdp1851Configuration_.initPortB);
        break;
    }

    outPutValueA_ = cdp1851Configuration_.initPortA;
    outPutValueB_ = cdp1851Configuration_.initPortB;
    inPutValueA_ = 0;
}

void Cdp1851Instance::writeControlRegister(Byte value)
{
    if (cdp1851Configuration_.connection == PIO_CONNECTION_WINDOW)
        pioScreenPointer->writeControlRegister(value, commandByteNumber_);
        
    if (commandByteNumber_ == PIO_COMMAND_NONE)
    {
        switch (value & 0xc7)
        {
            case 0x3: // Set input
            case 0x7:
                if (value & 0x8)				// Port A
                    modeSet[0] = PIO_INPUT;
                if (value & 0x10)				// Port B
                    modeSet[1] = PIO_INPUT;
            break;
                
            case 0x43: // Set output
            case 0x47:
                if (value & 0x8)				// Port A
                    modeSet[0] = PIO_OUTPUT;
                if (value & 0x10)				// Port B
                    modeSet[1] = PIO_OUTPUT;
            break;
                
            case 0xc3: // Set bit progammable
            case 0xc7:
                if ((value & 0x8) == 0x8)		// Port A
                {
                    modeSet[0] = PIO_BIT_PROG;
                    commandByteNumber_ = PIO_COMMAND_BITPROGA;
                    if (cdp1851Configuration_.connection == PIO_CONNECTION_WINDOW)
                        pioScreenPointer->enableStbRdyA(pioStatus_, strobeMode[0], rdyMode[0]);
                }
                if ((value & 0x10) == 0x10)		// Port B
                {
                    modeSet[1] = PIO_BIT_PROG;
                    commandByteNumber_ = PIO_COMMAND_BITPROGB;
                    if (cdp1851Configuration_.connection == PIO_CONNECTION_WINDOW)
                        pioScreenPointer->enableStbRdyB(pioStatus_, strobeMode[1], rdyMode[1]);
                }
                if ((value & 0x18) == 0x18)		// Port A+B
                {
                    commandByteNumber_ = PIO_COMMAND_BITPROGAB;
                    if (cdp1851Configuration_.connection == PIO_CONNECTION_WINDOW)
                    {
                        pioScreenPointer->enableStbRdyA(pioStatus_, strobeMode[0], rdyMode[0]);
                        pioScreenPointer->enableStbRdyB(pioStatus_, strobeMode[1], rdyMode[1]);
                    }
                }
            break;
                
            case 0x83: // Set bi directional
            case 0x87:
                if (value & 0x8 && modeSet[1] == PIO_BIT_PROG)
                    // Port A & port B needs to be set in bit programmable
                    modeSet[0] = PIO_BI_DRECT;
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
        if ((value & 0x1) == 0) // STB and RDY Control
        {
            readStatusRegister();
            strobeRdyControl(value);
        }
    }
    else
    {
        switch (commandByteNumber_)
        {
            case PIO_COMMAND_BITPROGA:
                setProgBitsA(value);
            break;
                
            case PIO_COMMAND_BITPROGB:
                setProgBitsB(value);
            break;

            case PIO_COMMAND_BITPROGAB:
                setProgBitsA(value);
                setProgBitsB(value);
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

void Cdp1851Instance::strobeRdyControl(Byte value)
{
    int port = (value & 0x2) >> 1;

    switch (value & 0xc)
    {
        case 0x04: // set RDY
            if (modeSet[port] == PIO_BIT_PROG && modeSet[0] != PIO_BI_DRECT)
            {
                rdyMode[port] = (value & 0x40) >> 6;
                if (cdp1851Configuration_.connection == PIO_CONNECTION_WINDOW)
                    pioScreenPointer->enableStbRdy(port, pioStatus_, strobeMode[port], rdyMode[port]);
            }
        break;

        case 0x08: // set STB
            if (modeSet[port] == PIO_BIT_PROG && modeSet[0] != PIO_BI_DRECT)
            {
                strobeMode[port] = (value & 0x80) >> 7;
                if (cdp1851Configuration_.connection == PIO_CONNECTION_WINDOW)
                    pioScreenPointer->enableStbRdy(port, pioStatus_, strobeMode[port], rdyMode[port]);
            }
        break;

        case 0x0C: // set RDY+STB
            if (modeSet[port] == PIO_BIT_PROG && modeSet[0] != PIO_BI_DRECT)
            {
                rdyMode[port] = (value & 0x40) >> 6;
                strobeMode[port] = (value & 0x80) >> 7;
                if (cdp1851Configuration_.connection == PIO_CONNECTION_WINDOW)
                    pioScreenPointer->enableStbRdy(port, pioStatus_, strobeMode[port], rdyMode[port]);
            }
        break;
    }
    
    if (cdp1851Configuration_.connection == PIO_CONNECTION_PRINTER)
    {
        pioPrinterPointer->rdyControl(port, value, rdyMode[port]);
        pioPrinterPointer->strobeControl(port, value, strobeMode[port]);
    }
}

void Cdp1851Instance::setProgBitsA(Byte value)
{
    if (cdp1851Configuration_.connection == PIO_CONNECTION_WINDOW)
        pioScreenPointer->setProgBitsA(value);
    pioAProgBits_ = value;
    commandByteNumber_ = PIO_COMMAND_NONE;
}

void Cdp1851Instance::setProgBitsB(Byte value)
{
    if (cdp1851Configuration_.connection == PIO_CONNECTION_WINDOW)
        pioScreenPointer->setProgBitsB(value);
    pioBProgBits_ = value;
    commandByteNumber_ = PIO_COMMAND_NONE;
}

void Cdp1851Instance::onRdyA()
{
    pioEfState[0] = 0;
   
    if (modeSet[0] == PIO_BI_DRECT)
        pioStatus_ |= 0x8;
    else
        pioStatus_ |= 0x2;

    if (pioAInterruptEnabled_)
        p_Computer->requestInterrupt();
}

void Cdp1851Instance::onRdyB()
{
    pioEfState[1] = 0;

    if (modeSet[0] == PIO_BI_DRECT)
        pioStatus_ |= 0x4;
    else
        pioStatus_ |= 0x1;

    if (pioBInterruptEnabled_)
        p_Computer->requestInterrupt();
}

void Cdp1851Instance::writePortA(Byte value)
{
    if (cdp1851Configuration_.connection == PIO_CONNECTION_WINDOW)
        pioScreenPointer->writePortA(value);
    
    if (modeSet[0] == PIO_OUTPUT || modeSet[0] == PIO_BI_DRECT)
    {
        outPutValueA_ = value;
        if (pioAInterruptEnabled_)
            p_Computer->requestInterrupt(INTERRUPT_TYPE_PIO_A, true, cdp1851Configuration_.picInterrupt);
    }
    if (modeSet[0] == PIO_BIT_PROG)
    {
        Byte progBits = pioAProgBits_;
        outPutValueA_ &= (progBits ^ 0xff);
        value &= progBits;
        outPutValueA_ |= value;
    }
    
    if (cdp1851Configuration_.connection == PIO_CONNECTION_PRINTER)
        pioPrinterPointer->writePort(0, value);
}

void Cdp1851Instance::writePortB(Byte value)
{
    if (cdp1851Configuration_.connection == PIO_CONNECTION_WINDOW)
        pioScreenPointer->writePortB(value);
    
    if (modeSet[1] == PIO_OUTPUT || modeSet[1] == PIO_BI_DRECT)
    {
        outPutValueB_ = value;
        if (pioBInterruptEnabled_)
            p_Computer->requestInterrupt(INTERRUPT_TYPE_PIO_A, true, cdp1851Configuration_.picInterrupt);
    }
    if (modeSet[1] == PIO_BIT_PROG)
    {
        Byte progBits = pioBProgBits_;
        outPutValueB_ &= (progBits ^ 0xff);
        value &= progBits;
        outPutValueB_ |= value;
    }
    
    if (cdp1851Configuration_.connection == PIO_CONNECTION_PRINTER)
        pioPrinterPointer->writePort(1, value);
}

Byte Cdp1851Instance::readPortA()
{
    pioEfState[0] = 1;
    pioStatus_ &= 0xFD;
    p_Computer->requestInterrupt(INTERRUPT_TYPE_PIO_A, false, cdp1851Configuration_.picInterrupt);

    if (cdp1851Configuration_.connection == PIO_CONNECTION_PRINTER)
        return pioPrinterPointer->readPort(0);
    else
    {
        if (modeSet[0] == PIO_BI_DRECT)
            return inPutValueA_;
        else
            return outPutValueA_;
    }
}

Byte Cdp1851Instance::readPortB()
{
    pioEfState[1] = 1;
    pioStatus_ &= 0xFE;
    p_Computer->requestInterrupt(INTERRUPT_TYPE_PIO_B, false, cdp1851Configuration_.picInterrupt);

    if (cdp1851Configuration_.connection == PIO_CONNECTION_PRINTER)
        return pioPrinterPointer->readPort(1);
    else
        return outPutValueB_;
}

Byte Cdp1851Instance::readStatusRegister()
{
    if (rdyMode[0] == PIO_INPUT && modeSet[0] == PIO_BIT_PROG)
    {
        pioStatus_ &= 0xef;
    }
    if (strobeMode[0] == PIO_INPUT && modeSet[0] == PIO_BIT_PROG)
    {
        pioStatus_ &= 0xdf;
    }
    if (rdyMode[1] == PIO_INPUT && modeSet[1] == PIO_BIT_PROG)
    {
        pioStatus_ &= 0xbf;
    }
    if (strobeMode[1] == PIO_INPUT && modeSet[1] == PIO_BIT_PROG)
    {
        pioStatus_ &= 0x7f;
    }
    if (modeSet[0] != PIO_BIT_PROG)
        pioStatus_ &= 0xcf;
    if (modeSet[1] != PIO_BIT_PROG)
        pioStatus_ &= 0x3f;
    
    if (cdp1851Configuration_.connection == PIO_CONNECTION_WINDOW)
        pioStatus_ = pioScreenPointer->readStatusRegister(pioStatus_);
    if (cdp1851Configuration_.connection == PIO_CONNECTION_PRINTER)
        pioStatus_ = pioPrinterPointer->readStatusRegister(pioStatus_);
    return pioStatus_;
}

void Cdp1851Instance::maskInterruptA()
{
    Byte unmaskedBits = outPutValueA_ & pioAInterruptMask_;
    switch (pioAInterruptLogical_)
    {
        case 0:     // Interrupt if any 'unmasked' bit is low
            if ((unmaskedBits ^ pioAInterruptMask_) != 0)
            {
                pioStatus_ |= 0x2;
                p_Computer->requestInterrupt(INTERRUPT_TYPE_PIO_A, true, cdp1851Configuration_.picInterrupt);
            }
        break;

        case 0x1:   // Interrupt if any 'unmasked' bit is high
            if ((unmaskedBits & pioAInterruptMask_) != 0)
            {
                pioStatus_ |= 0x2;
                p_Computer->requestInterrupt(INTERRUPT_TYPE_PIO_A, true, cdp1851Configuration_.picInterrupt);
            }
        break;

        case 0x2:   // Interrupt if all 'unmasked' bit are low
            if ((unmaskedBits & pioAInterruptMask_) == 0)
            {
                pioStatus_ |= 0x2;
                p_Computer->requestInterrupt(INTERRUPT_TYPE_PIO_A, true, cdp1851Configuration_.picInterrupt);
            }
        break;

        case 0x3:   // Interrupt if all 'unmasked' bit are high
            if ((unmaskedBits & pioAInterruptMask_) == pioAInterruptMask_)
            {
                pioStatus_ |= 0x2;
                p_Computer->requestInterrupt(INTERRUPT_TYPE_PIO_A, true, cdp1851Configuration_.picInterrupt);
            }
        break;
    }
}

void Cdp1851Instance::maskInterruptB()
{
    Byte unmaskedBits = outPutValueB_ & pioBInterruptMask_;
    switch (pioBInterruptLogical_)
    {
        case 0:     // Interrupt if any 'unmasked' bit is low
            if ((unmaskedBits ^ pioBInterruptMask_) != 0)
            {
                pioStatus_ |= 0x1;
                p_Computer->requestInterrupt(INTERRUPT_TYPE_PIO_B, true, cdp1851Configuration_.picInterrupt);
            }
        break;
            
        case 0x1:   // Interrupt if any 'unmasked' bit is high
            if ((unmaskedBits & pioBInterruptMask_) != 0)
            {
                pioStatus_ |= 0x1;
                p_Computer->requestInterrupt(INTERRUPT_TYPE_PIO_B, true, cdp1851Configuration_.picInterrupt);
            }
        break;
            
        case 0x2:   // Interrupt if all 'unmasked' bit are low
            if ((unmaskedBits & pioBInterruptMask_) == 0)
            {
                pioStatus_ |= 0x1;
                p_Computer->requestInterrupt(INTERRUPT_TYPE_PIO_B, true, cdp1851Configuration_.picInterrupt);
            }
        break;
            
        case 0x3:   // Interrupt if all 'unmasked' bit are high
            if ((unmaskedBits & pioBInterruptMask_) == pioBInterruptMask_)
            {
                pioStatus_ |= 0x1;
                p_Computer->requestInterrupt(INTERRUPT_TYPE_PIO_B, true, cdp1851Configuration_.picInterrupt);
            }
        break;
    }
}

Byte Cdp1851Instance::getEfState1()
{
    return pioEfState[0] ^ cdp1851Configuration_.efaRdy.reverse;
}

Byte Cdp1851Instance::getEfState2()
{
    return pioEfState[1] ^ cdp1851Configuration_.efbRdy.reverse;
}

Byte Cdp1851Instance::getIrqState()
{
    Byte irqState = 0;
    if (pioStatus_ & 1 || pioStatus_ & 2)
        irqState = 1;
    return irqState ^ cdp1851Configuration_.efIrq.reverse;
}

void Cdp1851Instance::reset()
{
    if (cdp1851Configuration_.connection == PIO_CONNECTION_WINDOW)
        pioScreenPointer->reset();

    modeSet[0] = PIO_INPUT;
    modeSet[1] = PIO_INPUT;
    strobeMode[0] = PIO_INPUT;
    strobeMode[1] = PIO_INPUT;
    rdyMode[0] = PIO_OUTPUT;
    rdyMode[1] = PIO_OUTPUT;
    pioAInterruptEnabled_ = false;
    pioBInterruptEnabled_ = false;
    pioAInterruptMask_ = 0;
    pioBInterruptMask_ = 0;
    pioStatus_ = 0;
    commandByteNumber_ = PIO_COMMAND_NONE;

    writeControlRegister(0x4b);

    clearStbRdy();
    clearA();
    clearB();
}

void Cdp1851Instance::ledTimeout()
{
    if (cdp1851Configuration_.connection == PIO_CONNECTION_WINDOW)
        pioScreenPointer->ledTimeout();
}

void Cdp1851Instance::interruptCycle()
{
    if (pioAInterruptEnabled_)
        maskInterruptA();
    
    if (pioBInterruptEnabled_)
        maskInterruptB();
    
    if (cdp1851Configuration_.connection == PIO_CONNECTION_PRINTER)
        pioPrinterPointer->cycle();
}

void Cdp1851Instance::setLedMs(long ms)
{
    if (cdp1851Configuration_.connection == PIO_CONNECTION_WINDOW)
        pioScreenPointer->setLedMs(ms);
}

void Cdp1851Instance::refreshLeds()
{
    if (cdp1851Configuration_.connection == PIO_CONNECTION_WINDOW)
        pioScreenPointer->refreshLeds();
}

void Cdp1851Instance::onClose(wxCloseEvent&WXUNUSED(event))
{
    p_Computer->removeCdp1851(pioNumber_);
}

void Cdp1851Instance::onArdyButton(wxCommandEvent&WXUNUSED(event))
{
    if (cdp1851Configuration_.connection == PIO_CONNECTION_WINDOW)
        pioScreenPointer->onArdyButton();
};

void Cdp1851Instance::onBrdyButton(wxCommandEvent&WXUNUSED(event)) 
{
    if (cdp1851Configuration_.connection == PIO_CONNECTION_WINDOW)
        pioScreenPointer->onBrdyButton();
};

void Cdp1851Instance::releaseButtonOnScreen(HexButton* buttonPoint)
{
    if (cdp1851Configuration_.connection == PIO_CONNECTION_WINDOW)
        pioScreenPointer->releaseButtonOnScreen(buttonPoint);
}

void Cdp1851Instance::refreshPanel()
{
    if (cdp1851Configuration_.connection == PIO_CONNECTION_WINDOW)
        pioScreenPointer->refreshPanel();
}

void Cdp1851Instance::clearA()
{
    if (cdp1851Configuration_.connection == PIO_CONNECTION_WINDOW)
        pioScreenPointer->clearA();
    
    outPutValueA_ = cdp1851Configuration_.initPortA;
    inPutValueA_ = cdp1851Configuration_.initPortA;
}

void Cdp1851Instance::clearB()
{
    if (cdp1851Configuration_.connection == PIO_CONNECTION_WINDOW)
        pioScreenPointer->clearB();

    outPutValueB_ = cdp1851Configuration_.initPortB;
}

void Cdp1851Instance::clearStbRdy()
{
    if (cdp1851Configuration_.connection == PIO_CONNECTION_WINDOW)
        pioScreenPointer->clearStbRdy();

    pioStatus_ &= 0x0f;
}

void Cdp1851Instance::setOutputValueA(int bit, int value)
{
    Byte bitmask = (1 << bit) ^ 0xff;
  
    outPutValueA_ &= bitmask;
    outPutValueA_ |= value << bit;
}

void Cdp1851Instance::setOutputValueB(int bit, int value)
{
    Byte bitmask = (1 << bit) ^ 0xff;
  
    outPutValueB_ &= bitmask;
    outPutValueB_ |= value << bit;
}

