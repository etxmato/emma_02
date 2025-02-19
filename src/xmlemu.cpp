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

#include "wx/wfstream.h"
#include "wx/zipstrm.h"
#include "wx/datstrm.h"
#include <math.h>

#include "main.h"
#include "pushbutton.h"

#include "xmlemu.h"

#define OFF    1

#define PRIDLE 0
#define PRW_START 1
#define PRW_STOP 2
#define PRBUSY 3

#if defined (__WXMAC__)
BEGIN_EVENT_TABLE(Computer, wxFrame)
    EVT_CLOSE (Computer::onClose)
    EVT_TIMER(900, Computer::OnRtcTimer)
    EVT_TIMER(901, Computer::OnCdp1878TimerA)
    EVT_TIMER(902, Computer::OnCdp1878TimerB)

END_EVENT_TABLE()
#else
BEGIN_EVENT_TABLE(Computer, wxFrame)
    EVT_CLOSE (Computer::onClose)

    EVT_COMMAND(0, wxEVT_ButtonDownEvent, Computer::onNumberKeyDown)
    EVT_COMMAND(1, wxEVT_ButtonDownEvent, Computer::onNumberKeyDown)
    EVT_COMMAND(2, wxEVT_ButtonDownEvent, Computer::onNumberKeyDown)
    EVT_COMMAND(3, wxEVT_ButtonDownEvent, Computer::onNumberKeyDown)
    EVT_COMMAND(4, wxEVT_ButtonDownEvent, Computer::onNumberKeyDown)
    EVT_COMMAND(5, wxEVT_ButtonDownEvent, Computer::onNumberKeyDown)
    EVT_COMMAND(6, wxEVT_ButtonDownEvent, Computer::onNumberKeyDown)
    EVT_COMMAND(7, wxEVT_ButtonDownEvent, Computer::onNumberKeyDown)
    EVT_COMMAND(8, wxEVT_ButtonDownEvent, Computer::onNumberKeyDown)
    EVT_COMMAND(9, wxEVT_ButtonDownEvent, Computer::onNumberKeyDown)
    EVT_COMMAND(10, wxEVT_ButtonDownEvent, Computer::onNumberKeyDown)
    EVT_COMMAND(11, wxEVT_ButtonDownEvent, Computer::onNumberKeyDown)
    EVT_COMMAND(12, wxEVT_ButtonDownEvent, Computer::onNumberKeyDown)
    EVT_COMMAND(13, wxEVT_ButtonDownEvent, Computer::onNumberKeyDown)
    EVT_COMMAND(14, wxEVT_ButtonDownEvent, Computer::onNumberKeyDown)
    EVT_COMMAND(15, wxEVT_ButtonDownEvent, Computer::onNumberKeyDown)

    EVT_COMMAND(0, wxEVT_ButtonUpEvent, Computer::onNumberKeyUp)
    EVT_COMMAND(1, wxEVT_ButtonUpEvent, Computer::onNumberKeyUp)
    EVT_COMMAND(2, wxEVT_ButtonUpEvent, Computer::onNumberKeyUp)
    EVT_COMMAND(3, wxEVT_ButtonUpEvent, Computer::onNumberKeyUp)
    EVT_COMMAND(4, wxEVT_ButtonUpEvent, Computer::onNumberKeyUp)
    EVT_COMMAND(5, wxEVT_ButtonUpEvent, Computer::onNumberKeyUp)
    EVT_COMMAND(6, wxEVT_ButtonUpEvent, Computer::onNumberKeyUp)
    EVT_COMMAND(7, wxEVT_ButtonUpEvent, Computer::onNumberKeyUp)
    EVT_COMMAND(8, wxEVT_ButtonUpEvent, Computer::onNumberKeyUp)
    EVT_COMMAND(9, wxEVT_ButtonUpEvent, Computer::onNumberKeyUp)
    EVT_COMMAND(10, wxEVT_ButtonUpEvent, Computer::onNumberKeyUp)
    EVT_COMMAND(11, wxEVT_ButtonUpEvent, Computer::onNumberKeyUp)
    EVT_COMMAND(12, wxEVT_ButtonUpEvent, Computer::onNumberKeyUp)
    EVT_COMMAND(13, wxEVT_ButtonUpEvent, Computer::onNumberKeyUp)
    EVT_COMMAND(14, wxEVT_ButtonUpEvent, Computer::onNumberKeyUp)
    EVT_COMMAND(15, wxEVT_ButtonUpEvent, Computer::onNumberKeyUp)

    EVT_COMMAND(20, wxEVT_ButtonDownEvent, Computer::onButtonPress)
    EVT_COMMAND(20, wxEVT_ButtonUpEvent, Computer::onButtonRelease)
    EVT_BUTTON(21, Computer::onRunButtonPress)
    EVT_BUTTON(22, Computer::onMpButton)
    EVT_BUTTON(23, Computer::onRunButtonPress0)
    EVT_BUTTON(24, Computer::onLoadButton)
    EVT_BUTTON(25, Computer::onPause)
    EVT_BUTTON(26, Computer::onSingleStep)
    EVT_BUTTON(27, Computer::onResetButton)
    EVT_BUTTON(28, Computer::onMonitor)
    EVT_BUTTON(29, Computer::onStopButton)
    EVT_BUTTON(30, Computer::onReadButton)
    EVT_BUTTON(31, Computer::onNanoMonitor)
    EVT_BUTTON(32, Computer::onCardButtonSwitch)
    EVT_BUTTON(33, Computer::onPowerButton)
    EVT_BUTTON(34, Computer::onMpSuperButton)
    EVT_BUTTON(35, Computer::onWaitButton)
    EVT_BUTTON(36, Computer::onClearButton)
//    EVT_BUTTON(37, Computer::onClearRunButton)
//    EVT_BUTTON(38, Computer::onClearResetButton)
    EVT_BUTTON(39, Computer::onRamButton)
    EVT_COMMAND(40, wxEVT_ButtonDownEvent, Computer::onSwitchButtonPress)
    EVT_COMMAND(40, wxEVT_ButtonUpEvent, Computer::onButtonRelease)

    EVT_BUTTON(0x50, Computer::onMpButtonMulti)
    EVT_BUTTON(0x51, Computer::onMpButtonMulti)
    EVT_BUTTON(0x52, Computer::onMpButtonMulti)
    EVT_BUTTON(0x53, Computer::onMpButtonMulti)

    EVT_BUTTON(0x60, Computer::onEmsButton)
    EVT_BUTTON(0x61, Computer::onEmsButton)

    EVT_BUTTON(0x80, Computer::onDataSwitch)
    EVT_BUTTON(0x81, Computer::onDataSwitch)
    EVT_BUTTON(0x82, Computer::onDataSwitch)
    EVT_BUTTON(0x83, Computer::onDataSwitch)
    EVT_BUTTON(0x84, Computer::onDataSwitch)
    EVT_BUTTON(0x85, Computer::onDataSwitch)
    EVT_BUTTON(0x86, Computer::onDataSwitch)
    EVT_BUTTON(0x87, Computer::onDataSwitch)

    EVT_BUTTON(0x90, Computer::onDataSwitch)
    EVT_BUTTON(0x91, Computer::onDataSwitch)
    EVT_BUTTON(0x92, Computer::onDataSwitch)
    EVT_BUTTON(0x93, Computer::onDataSwitch)

    EVT_BUTTON(0x100, Computer::onCardButton)
    EVT_BUTTON(0x101, Computer::onCardButton)
    EVT_BUTTON(0x102, Computer::onCardButton)
    EVT_BUTTON(0x103, Computer::onCardButton)
    EVT_BUTTON(0x104, Computer::onCardButton)
    EVT_BUTTON(0x105, Computer::onCardButton)
    EVT_BUTTON(0x106, Computer::onCardButton)
    EVT_BUTTON(0x107, Computer::onCardButton)
    EVT_BUTTON(0x108, Computer::onCardButton)
    EVT_BUTTON(0x109, Computer::onCardButton)
    EVT_BUTTON(0x10A, Computer::onCardButton)
    EVT_BUTTON(0x10B, Computer::onCardButton)
    EVT_BUTTON(0x10C, Computer::onCardButton)
    EVT_BUTTON(0x10D, Computer::onCardButton)
    EVT_BUTTON(0x10E, Computer::onCardButton)
    EVT_BUTTON(0x10F, Computer::onCardButton)

    EVT_BUTTON(0x110, Computer::onCardButton)
    EVT_BUTTON(0x111, Computer::onCardButton)
    EVT_BUTTON(0x112, Computer::onCardButton)
    EVT_BUTTON(0x113, Computer::onCardButton)
    EVT_BUTTON(0x114, Computer::onCardButton)
    EVT_BUTTON(0x115, Computer::onCardButton)
    EVT_BUTTON(0x116, Computer::onCardButton)
    EVT_BUTTON(0x117, Computer::onCardButton)
    EVT_BUTTON(0x118, Computer::onCardButton)
    EVT_BUTTON(0x119, Computer::onCardButton)
    EVT_BUTTON(0x11A, Computer::onCardButton)
    EVT_BUTTON(0x11B, Computer::onCardButton)
    EVT_BUTTON(0x11C, Computer::onCardButton)
    EVT_BUTTON(0x11D, Computer::onCardButton)
    EVT_BUTTON(0x11E, Computer::onCardButton)
    EVT_BUTTON(0x11F, Computer::onCardButton)

    EVT_BUTTON(0x120, Computer::onCardButton)
    EVT_BUTTON(0x121, Computer::onCardButton)
    EVT_BUTTON(0x122, Computer::onCardButton)
    EVT_BUTTON(0x123, Computer::onCardButton)
    EVT_BUTTON(0x124, Computer::onCardButton)
    EVT_BUTTON(0x125, Computer::onCardButton)
    EVT_BUTTON(0x126, Computer::onCardButton)
    EVT_BUTTON(0x127, Computer::onCardButton)
    EVT_BUTTON(0x128, Computer::onCardButton)
    EVT_BUTTON(0x129, Computer::onCardButton)
    EVT_BUTTON(0x12A, Computer::onCardButton)
    EVT_BUTTON(0x12B, Computer::onCardButton)
    EVT_BUTTON(0x12C, Computer::onCardButton)
    EVT_BUTTON(0x12D, Computer::onCardButton)
    EVT_BUTTON(0x12E, Computer::onCardButton)
    EVT_BUTTON(0x12F, Computer::onCardButton)

    EVT_BUTTON(0x130, Computer::onCardButton)
    EVT_BUTTON(0x131, Computer::onCardButton)
    EVT_BUTTON(0x132, Computer::onCardButton)
    EVT_BUTTON(0x133, Computer::onCardButton)
    EVT_BUTTON(0x134, Computer::onCardButton)
    EVT_BUTTON(0x135, Computer::onCardButton)
    EVT_BUTTON(0x136, Computer::onCardButton)
    EVT_BUTTON(0x137, Computer::onCardButton)
    EVT_BUTTON(0x138, Computer::onCardButton)
    EVT_BUTTON(0x139, Computer::onCardButton)
    EVT_BUTTON(0x13A, Computer::onCardButton)
    EVT_BUTTON(0x13B, Computer::onCardButton)
    EVT_BUTTON(0x13C, Computer::onCardButton)
    EVT_BUTTON(0x13D, Computer::onCardButton)
    EVT_BUTTON(0x13E, Computer::onCardButton)
    EVT_BUTTON(0x13F, Computer::onCardButton)

    EVT_BUTTON(0x140, Computer::onCardButton)
    EVT_BUTTON(0x141, Computer::onCardButton)
    EVT_BUTTON(0x142, Computer::onCardButton)
    EVT_BUTTON(0x143, Computer::onCardButton)
    EVT_BUTTON(0x144, Computer::onCardButton)
    EVT_BUTTON(0x145, Computer::onCardButton)
    EVT_BUTTON(0x146, Computer::onCardButton)
    EVT_BUTTON(0x147, Computer::onCardButton)
    EVT_BUTTON(0x148, Computer::onCardButton)
    EVT_BUTTON(0x149, Computer::onCardButton)
    EVT_BUTTON(0x14A, Computer::onCardButton)
    EVT_BUTTON(0x14B, Computer::onCardButton)
    EVT_BUTTON(0x14C, Computer::onCardButton)
    EVT_BUTTON(0x14D, Computer::onCardButton)
    EVT_BUTTON(0x14E, Computer::onCardButton)
    EVT_BUTTON(0x14F, Computer::onCardButton)

    EVT_BUTTON(0x150, Computer::onCardButton)
    EVT_BUTTON(0x151, Computer::onCardButton)
    EVT_BUTTON(0x152, Computer::onCardButton)
    EVT_BUTTON(0x153, Computer::onCardButton)
    EVT_BUTTON(0x154, Computer::onCardButton)
    EVT_BUTTON(0x155, Computer::onCardButton)
    EVT_BUTTON(0x156, Computer::onCardButton)
    EVT_BUTTON(0x157, Computer::onCardButton)
    EVT_BUTTON(0x158, Computer::onCardButton)
    EVT_BUTTON(0x159, Computer::onCardButton)
    EVT_BUTTON(0x15A, Computer::onCardButton)
    EVT_BUTTON(0x15B, Computer::onCardButton)
    EVT_BUTTON(0x15C, Computer::onCardButton)
    EVT_BUTTON(0x15D, Computer::onCardButton)
    EVT_BUTTON(0x15E, Computer::onCardButton)
    EVT_BUTTON(0x15F, Computer::onCardButton)

    EVT_BUTTON(0x160, Computer::onCardButton)
    EVT_BUTTON(0x161, Computer::onCardButton)
    EVT_BUTTON(0x162, Computer::onCardButton)
    EVT_BUTTON(0x163, Computer::onCardButton)
    EVT_BUTTON(0x164, Computer::onCardButton)
    EVT_BUTTON(0x165, Computer::onCardButton)
    EVT_BUTTON(0x166, Computer::onCardButton)
    EVT_BUTTON(0x167, Computer::onCardButton)
    EVT_BUTTON(0x168, Computer::onCardButton)
    EVT_BUTTON(0x169, Computer::onCardButton)
    EVT_BUTTON(0x16A, Computer::onCardButton)
    EVT_BUTTON(0x16B, Computer::onCardButton)
    EVT_BUTTON(0x16C, Computer::onCardButton)
    EVT_BUTTON(0x16D, Computer::onCardButton)
    EVT_BUTTON(0x16E, Computer::onCardButton)
    EVT_BUTTON(0x16F, Computer::onCardButton)

    EVT_BUTTON(0x170, Computer::onCardButton)
    EVT_BUTTON(0x171, Computer::onCardButton)
    EVT_BUTTON(0x172, Computer::onCardButton)
    EVT_BUTTON(0x173, Computer::onCardButton)
    EVT_BUTTON(0x174, Computer::onCardButton)
    EVT_BUTTON(0x175, Computer::onCardButton)
    EVT_BUTTON(0x176, Computer::onCardButton)
    EVT_BUTTON(0x177, Computer::onCardButton)
    EVT_BUTTON(0x178, Computer::onCardButton)
    EVT_BUTTON(0x179, Computer::onCardButton)
    EVT_BUTTON(0x17A, Computer::onCardButton)
    EVT_BUTTON(0x17B, Computer::onCardButton)
    EVT_BUTTON(0x17C, Computer::onCardButton)
    EVT_BUTTON(0x17D, Computer::onCardButton)
    EVT_BUTTON(0x17E, Computer::onCardButton)
    EVT_BUTTON(0x17F, Computer::onCardButton)

    EVT_BUTTON(0x180, Computer::onCardButton)
    EVT_BUTTON(0x181, Computer::onCardButton)
    EVT_BUTTON(0x182, Computer::onCardButton)
    EVT_BUTTON(0x183, Computer::onCardButton)
    EVT_BUTTON(0x184, Computer::onCardButton)
    EVT_BUTTON(0x185, Computer::onCardButton)
    EVT_BUTTON(0x186, Computer::onCardButton)
    EVT_BUTTON(0x187, Computer::onCardButton)
    EVT_BUTTON(0x188, Computer::onCardButton)
    EVT_BUTTON(0x189, Computer::onCardButton)
    EVT_BUTTON(0x18A, Computer::onCardButton)
    EVT_BUTTON(0x18B, Computer::onCardButton)
    EVT_BUTTON(0x18C, Computer::onCardButton)
    EVT_BUTTON(0x18D, Computer::onCardButton)
    EVT_BUTTON(0x18E, Computer::onCardButton)
    EVT_BUTTON(0x18F, Computer::onCardButton)

    EVT_BUTTON(0x190, Computer::onCardButton)
    EVT_BUTTON(0x191, Computer::onCardButton)
    EVT_BUTTON(0x192, Computer::onCardButton)
    EVT_BUTTON(0x193, Computer::onCardButton)
    EVT_BUTTON(0x194, Computer::onCardButton)
    EVT_BUTTON(0x195, Computer::onCardButton)
    EVT_BUTTON(0x196, Computer::onCardButton)
    EVT_BUTTON(0x197, Computer::onCardButton)
    EVT_BUTTON(0x198, Computer::onCardButton)
    EVT_BUTTON(0x199, Computer::onCardButton)
    EVT_BUTTON(0x19A, Computer::onCardButton)
    EVT_BUTTON(0x19B, Computer::onCardButton)
    EVT_BUTTON(0x19C, Computer::onCardButton)
    EVT_BUTTON(0x19D, Computer::onCardButton)
    EVT_BUTTON(0x19E, Computer::onCardButton)
    EVT_BUTTON(0x19F, Computer::onCardButton)

    EVT_BUTTON(0x1A0, Computer::onCardButton)
    EVT_BUTTON(0x1A1, Computer::onCardButton)
    EVT_BUTTON(0x1A2, Computer::onCardButton)
    EVT_BUTTON(0x1A3, Computer::onCardButton)
    EVT_BUTTON(0x1A4, Computer::onCardButton)
    EVT_BUTTON(0x1A5, Computer::onCardButton)
    EVT_BUTTON(0x1A6, Computer::onCardButton)
    EVT_BUTTON(0x1A7, Computer::onCardButton)
    EVT_BUTTON(0x1A8, Computer::onCardButton)
    EVT_BUTTON(0x1A9, Computer::onCardButton)
    EVT_BUTTON(0x1AA, Computer::onCardButton)
    EVT_BUTTON(0x1AB, Computer::onCardButton)
    EVT_BUTTON(0x1AC, Computer::onCardButton)
    EVT_BUTTON(0x1AD, Computer::onCardButton)
    EVT_BUTTON(0x1AE, Computer::onCardButton)
    EVT_BUTTON(0x1AF, Computer::onCardButton)

    EVT_BUTTON(0x1B0, Computer::onCardButton)
    EVT_BUTTON(0x1B1, Computer::onCardButton)
    EVT_BUTTON(0x1B2, Computer::onCardButton)
    EVT_BUTTON(0x1B3, Computer::onCardButton)
    EVT_BUTTON(0x1B4, Computer::onCardButton)
    EVT_BUTTON(0x1B5, Computer::onCardButton)
    EVT_BUTTON(0x1B6, Computer::onCardButton)
    EVT_BUTTON(0x1B7, Computer::onCardButton)
    EVT_BUTTON(0x1B8, Computer::onCardButton)
    EVT_BUTTON(0x1B9, Computer::onCardButton)
    EVT_BUTTON(0x1BA, Computer::onCardButton)
    EVT_BUTTON(0x1BB, Computer::onCardButton)
    EVT_BUTTON(0x1BC, Computer::onCardButton)
    EVT_BUTTON(0x1BD, Computer::onCardButton)
    EVT_BUTTON(0x1BE, Computer::onCardButton)
    EVT_BUTTON(0x1BF, Computer::onCardButton)

    EVT_BUTTON(0x1C0, Computer::onCardButton)
    EVT_BUTTON(0x1C1, Computer::onCardButton)
    EVT_BUTTON(0x1C2, Computer::onCardButton)
    EVT_BUTTON(0x1C3, Computer::onCardButton)
    EVT_BUTTON(0x1C4, Computer::onCardButton)
    EVT_BUTTON(0x1C5, Computer::onCardButton)
    EVT_BUTTON(0x1C6, Computer::onCardButton)
    EVT_BUTTON(0x1C7, Computer::onCardButton)
    EVT_BUTTON(0x1C8, Computer::onCardButton)
    EVT_BUTTON(0x1C9, Computer::onCardButton)
    EVT_BUTTON(0x1CA, Computer::onCardButton)
    EVT_BUTTON(0x1CB, Computer::onCardButton)
    EVT_BUTTON(0x1CC, Computer::onCardButton)
    EVT_BUTTON(0x1CD, Computer::onCardButton)
    EVT_BUTTON(0x1CE, Computer::onCardButton)
    EVT_BUTTON(0x1CF, Computer::onCardButton)

    EVT_BUTTON(0x1D0, Computer::onCardButton)
    EVT_BUTTON(0x1D1, Computer::onCardButton)
    EVT_BUTTON(0x1D2, Computer::onCardButton)
    EVT_BUTTON(0x1D3, Computer::onCardButton)
    EVT_BUTTON(0x1D4, Computer::onCardButton)
    EVT_BUTTON(0x1D5, Computer::onCardButton)
    EVT_BUTTON(0x1D6, Computer::onCardButton)
    EVT_BUTTON(0x1D7, Computer::onCardButton)
    EVT_BUTTON(0x1D8, Computer::onCardButton)
    EVT_BUTTON(0x1D9, Computer::onCardButton)
    EVT_BUTTON(0x1DA, Computer::onCardButton)
    EVT_BUTTON(0x1DB, Computer::onCardButton)
    EVT_BUTTON(0x1DC, Computer::onCardButton)
    EVT_BUTTON(0x1DD, Computer::onCardButton)
    EVT_BUTTON(0x1DE, Computer::onCardButton)
    EVT_BUTTON(0x1DF, Computer::onCardButton)

    EVT_BUTTON(0x1E0, Computer::onCardButton)
    EVT_BUTTON(0x1E1, Computer::onCardButton)
    EVT_BUTTON(0x1E2, Computer::onCardButton)
    EVT_BUTTON(0x1E3, Computer::onCardButton)
    EVT_BUTTON(0x1E4, Computer::onCardButton)
    EVT_BUTTON(0x1E5, Computer::onCardButton)
    EVT_BUTTON(0x1E6, Computer::onCardButton)
    EVT_BUTTON(0x1E7, Computer::onCardButton)
    EVT_BUTTON(0x1E8, Computer::onCardButton)
    EVT_BUTTON(0x1E9, Computer::onCardButton)
    EVT_BUTTON(0x1EA, Computer::onCardButton)
    EVT_BUTTON(0x1EB, Computer::onCardButton)
    EVT_BUTTON(0x1EC, Computer::onCardButton)
    EVT_BUTTON(0x1ED, Computer::onCardButton)
    EVT_BUTTON(0x1EE, Computer::onCardButton)
    EVT_BUTTON(0x1EF, Computer::onCardButton)

    EVT_BUTTON(0x1F0, Computer::onCardButton)
    EVT_BUTTON(0x1F1, Computer::onCardButton)
    EVT_BUTTON(0x1F2, Computer::onCardButton)
    EVT_BUTTON(0x1F3, Computer::onCardButton)
    EVT_BUTTON(0x1F4, Computer::onCardButton)
    EVT_BUTTON(0x1F5, Computer::onCardButton)
    EVT_BUTTON(0x1F6, Computer::onCardButton)
    EVT_BUTTON(0x1F7, Computer::onCardButton)
    EVT_BUTTON(0x1F8, Computer::onCardButton)
    EVT_BUTTON(0x1F9, Computer::onCardButton)
    EVT_BUTTON(0x1FA, Computer::onCardButton)
    EVT_BUTTON(0x1FB, Computer::onCardButton)
    EVT_BUTTON(0x1FC, Computer::onCardButton)
    EVT_BUTTON(0x1FD, Computer::onCardButton)
    EVT_BUTTON(0x1FE, Computer::onCardButton)
    EVT_BUTTON(0x1FF, Computer::onCardButton)

    EVT_TIMER(900, Computer::OnRtcTimer)
    EVT_TIMER(901, Computer::OnCdp1878TimerA)
    EVT_TIMER(902, Computer::OnCdp1878TimerB)

END_EVENT_TABLE()
#endif

Computer::Computer(const wxString& title, double clock, int tempo, ComputerConfiguration computerConfig)
: wxFrame((wxFrame *)NULL, -1, title)
{
    currentComputerConfiguration = computerConfig;
    computerClockSpeed_ = clock;
    title_ = title;

#ifndef __WXMAC__
    SetIcon(wxICON(app_icon));
#endif
  
    p_Main->stopTerminal();

//    this->SetClientSize(size);

    cycleValue_ = -1;
    cycleSize_ = -1;
            
    //remove...??
            cycleSize_ = (int)(1000 / ((1/1.75) * 8)); // ~1000 Hz on 1.75 CPU
            cycleValue_ = cycleSize_;
//    multiTilCycleSize_ = (int)(1000 / ((1/clock) * 8)); // ~1000 Hz on 1.75 CPU

    multiTilCycleSize_ = currentComputerConfiguration.multiSegDisplayConfiguration.cycleValue;
    multiTilCycleValue_ = multiTilCycleSize_;

    powerButtonState_ = true;
    
    if (currentComputerConfiguration.mainFrontPanelConfiguration.powerButtonDefined)
        powerButtonState_ = p_Main->getConfigBool("Computer/PowerButtonState", true);

    numberOfFrontPanels_ = 0;
    thumbSwitchValue_ = -1;

    rtcTimerPointer = new wxTimer(this, 900);
    computerTimerPointer[0] = new wxTimer(this, 901);
    computerTimerPointer[1] = new wxTimer(this, 902);

    runningGame_ = "";
    bitKeypadValue_ = 0;

    gaugeValue_ = 0;
    lastTapeInputInt32_ = 0;
    lastTapeInputInt16_ = 0;
    lastTapeInputChar_ = 0;
    maxTapeInputInt32_ = 0;
    maxTapeInputInt16_ = 0;
    maxTapeInputChar_ = 0;
    saveStarted_ = false;
    loadStarted_ = false;
    numberOfCdp1877Instances_ = 0;

    soundTempoCycleSize_ = (int) (((clock * 1000000) / 8) / tempo);
    vipIIRunCycleSize_ = (int) (((clock * 800000) / 8) ) * 2;

    chip8baseVar_ = 0xef0;
    chip8mainLoop_ = 0x1b;
    for (int i = 0; i<16; i++)
        chip8Register[i] = -1;
    pseudoLoaded_ = false;

    ctrlvTextCharNum_ = 0;
    memoryStart_ = 0;

    inKey1_ = -1;
    inKey2_ = -1;

    threadPointer = new RunComputer();
    if ( threadPointer->Create() != wxTHREAD_NO_ERROR )
    {
        p_Main->message("Can't create thread");
    }
    threadPointer->SetPriority(WXTHREAD_MAX_PRIORITY);
}

Computer::~Computer()
{
    p_Main->turboOff();
    p_Main->batchConvertStop();
    saveRtc();
    if (nvramDetails.size() > 0)
        saveNvRam();
    for (int counter=0; counter<2 ; counter++)
    {
        computerTimerPointer[counter]->Stop();
        delete computerTimerPointer[counter];
    }
    rtcTimerPointer->Stop();
    delete rtcTimerPointer;
    if (currentComputerConfiguration.coinConfiguration.defined)
    {
        p_Main->setCoinPos(coinPointer->GetPosition());
        coinPointer->Destroy();
    }
    if (currentComputerConfiguration.cdp1861Configuration.defined)
    {
        p_Main->setPixiePos(pixiePointer->GetPosition());
        pixiePointer->Destroy();
    }
    if (currentComputerConfiguration.cdp1864Configuration.defined)
    {
        p_Main->setCdp1864Pos(cdp1864Pointer->GetPosition());
        cdp1864Pointer->Destroy();
    }
    if (currentComputerConfiguration.studio4VideoConfiguration.defined)
    {
        p_Main->setSt4Pos(st4VideoPointer->GetPosition());
        st4VideoPointer->Destroy();
    }
    if (currentComputerConfiguration.vip2KVideoConfiguration.defined)
    {
        p_Main->setVip2KPos(vip2KVideoPointer->GetPosition());
        vip2KVideoPointer->Destroy();
    }
    if (currentComputerConfiguration.fredVideoConfiguration.defined)
    {
        p_Main->setFredPos(fredVideoPointer->GetPosition());
        fredVideoPointer->Destroy();
    }
    if (currentComputerConfiguration.tmsConfiguration.defined)
    {
        p_Main->setTmsPos(tmsPointer->GetPosition());
        tmsPointer->Destroy();
    }
    if (currentComputerConfiguration.mc6845Configuration.defined)
    {
        p_Main->set6845Pos(mc6845Pointer->GetPosition());
        mc6845Pointer->Destroy();
    }
    if (currentComputerConfiguration.mc6847Configuration.defined)
    {
        p_Main->set6847Pos(mc6847Pointer->GetPosition());
        mc6847Pointer->Destroy();
    }
    if (currentComputerConfiguration.i8275Configuration.defined)
    {
        p_Main->set8275Pos(i8275Pointer->GetPosition());
        i8275Pointer->Destroy();
    }
    if (currentComputerConfiguration.vis1870Configuration.defined)
    {
        p_Main->setV1870Pos(vis1870Pointer->GetPosition());
        vis1870Pointer->Destroy();
    }
    if (currentComputerConfiguration.sn76430NConfiguration.defined)
    {
        p_Main->setV1870Pos(sn76430nPointer->GetPosition());
        sn76430nPointer->Destroy();
    }
    for (int num=0; num<numberOfCdp1851Frames_; num++)
    {
        p_Main->setCdp1851Pos(cdp1851FramePointer[num]->GetPosition(), num);
        cdp1851FramePointer[num]->Destroy();
    }
    for (int num=0; num<numberOfCdp1852Frames_; num++)
    {
        p_Main->setCdp1852Pos(cdp1852FramePointer[num]->GetPosition(), num);
        cdp1852FramePointer[num]->Destroy();
    }
    for (int num=0; num<numberOfCdp1854Instances_ ; num++)
        delete cdp1854InstancePointer[num];
    for (int num=0; num<numberOfCdp1877Instances_ ; num++)
        delete cdp1877InstancePointer[num];
    for (int num=0; num<numberOfCd4536b_; num++)
        delete cd4536bPointer[num];
    if (currentComputerConfiguration.videoTerminalConfiguration.type != VTNONE)
    {
        p_Main->setVtPos(vtPointer->GetPosition());
        vtPointer->Destroy();
    }
    for (int pad=0; pad<=lastBitKeyPad_; pad++)
    {
        if (currentComputerConfiguration.bitKeypadConfiguration[pad].defined)
            delete bitkeypadPointer[pad];
    }
    if (currentComputerConfiguration.cvKeypadConfiguration.defined)
        delete cvkeypadPointer;
    if (currentComputerConfiguration.etiKeypadConfiguration.defined)
        delete etikeypadPointer;
    if (currentComputerConfiguration.fredKeypadConfiguration.defined)
        delete fredkeypadPointer;
    for (int pad=0; pad<lastLatchKeyPad_; pad++)
    {
        if (currentComputerConfiguration.keyLatchConfiguration[pad].defined)
            delete latchKeyPointer[pad];
    }
    if (currentComputerConfiguration.matrixKeyboardConfiguration.defined)
        delete matrixKeyboardPointer;
    if (currentComputerConfiguration.videoTerminalConfiguration.external || currentComputerConfiguration.videoTerminalConfiguration.loop_back)
        delete p_Serial;
    if (currentComputerConfiguration.basicPrinterConfiguration.defined || currentComputerConfiguration.qSerialPrinterConfiguration.defined || currentComputerConfiguration.centronicsPrinterConfiguration.defined)
    {
        p_Printer->closeFrames();
        delete p_Printer;
    }
    if (currentComputerConfiguration.parallelPrinterConfiguration.defined)
    {
        p_PrinterParallel->closeFrames();
        delete p_PrinterParallel;
    }
    if (currentComputerConfiguration.serialPrinterConfiguration.defined)
    {
        p_PrinterSerial->closeFrames();
        delete p_PrinterSerial;
    }
    if (currentComputerConfiguration.thermalPrinterConfiguration.defined)
    {
        p_PrinterThermal->closeFrames();
        delete p_PrinterThermal;
    }

    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
    {
        p_Main->setFrontPanelPos(panelPointer[frontPanel]->GetPosition(), frontPanel);
        delete panelPointer[frontPanel];
    }
    p_Main->writeXmlWindowConfig();
}

void Computer::onClose(wxCloseEvent&WXUNUSED(event) )
{
    resumeComputer();
    p_Main->stopComputer();
}

void Computer::resumeComputer()
{    
    if (!powerButtonState_ && currentComputerConfiguration.mainFrontPanelConfiguration.powerButtonDefined)
    {
        threadPointer->Resume();
        powerButtonState_ = true;
    }
}

void Computer::charEvent(int keycode)
{
    if (p_Vt100[UART1] != NULL)
    {
        if (vtPointer->IsActive())
            return;
    }
    if (currentComputerConfiguration.asciiKeyboardConfiguration.defined)
        charEventKeyboard(keycode);
    if (currentComputerConfiguration.cdp1871Configuration.defined)
        charEventKeyb1871(keycode);
    if (currentComputerConfiguration.matrixKeyboardConfiguration.defined)
        matrixKeyboardPointer->charEvent(keycode);
    if (currentComputerConfiguration.gpioPs2KeyboardConfiguration.defined)
        charEventPs2gpio(keycode);
}

bool Computer::keyDownPressed(int key)
{
    if (currentComputerConfiguration.runPressType == RUN_TYPE_UC1800)
    {
        if (key == inKey2_)
        {
            onRunButtonPress(false);
            return true;
        }
    }
    
    onHexKeyDown(key);
    if (currentComputerConfiguration.gpioPs2KeyboardConfiguration.jp4)
    {
        switch(key)
        {
            case WXK_NUMPAD0:
            case WXK_NUMPAD1:
            case WXK_NUMPAD2:
            case WXK_NUMPAD3:
            case WXK_NUMPAD4:
            case WXK_NUMPAD5:
            case WXK_NUMPAD6:
            case WXK_NUMPAD7:
            case WXK_NUMPAD8:
            case WXK_NUMPAD9:
            case WXK_NUMPAD_DECIMAL:
            case WXK_NUMPAD_ADD:
            case WXK_NUMPAD_DIVIDE:
            case WXK_NUMPAD_MULTIPLY:
            case WXK_NUMPAD_SUBTRACT:
            case WXK_NUMPAD_ENTER:
                charEventPs2gpio(key);
                return true;
            break;
        }
    }
    if (currentComputerConfiguration.ps2KeyboardConfiguration.defined)
    {
        keyDownPs2(key);
            return true;
    }
    return false;
}

bool Computer::keyDownExtended(int keycode, wxKeyEvent& event)
{
    if (currentComputerConfiguration.efButtonsConfiguration.defined)
    {
        for (int ef=1; ef <=4; ef++)
        {
            if (currentComputerConfiguration.efButtonsConfiguration.key[ef].defined)
            {
                if (currentComputerConfiguration.efButtonsConfiguration.key[ef].mod)
                {
                    efKeyValue[ef] = currentComputerConfiguration.efButtonsConfiguration.keyPressed ^ 1;
                    if (currentComputerConfiguration.efButtonsConfiguration.key[ef].value == event.GetModifiers())
                        efKeyValue[ef] = currentComputerConfiguration.efButtonsConfiguration.keyPressed;
                }
                else
                {
                    efKeyValue[ef] = currentComputerConfiguration.efButtonsConfiguration.keyPressed ^ 1;
                    if (currentComputerConfiguration.efButtonsConfiguration.key[ef].value == keycode)
                        efKeyValue[ef] = currentComputerConfiguration.efButtonsConfiguration.keyPressed;
                }
            }
        }
    }
    for (std::vector<EfButton>::iterator efButton = currentComputerConfiguration.efButtonsConfiguration.efButton.begin (); efButton != currentComputerConfiguration.efButtonsConfiguration.efButton.end (); ++efButton)
    {
        if (efButton->value == keycode)
        {
            for (std::vector<int>::iterator efFlag = efButton->flag.begin (); efFlag != efButton->flag.end (); ++efFlag)
            {
                efKeyValue[*efFlag] = currentComputerConfiguration.efButtonsConfiguration.keyPressed;
            }
        }
    }
    for (int pad=0; pad<=lastBitKeyPad_; pad++)
    {
        if (currentComputerConfiguration.bitKeypadConfiguration[pad].defined)
            bitkeypadPointer[pad]->keyDown(keycode);
    }
    if (currentComputerConfiguration.cvKeypadConfiguration.defined)
        cvkeypadPointer->keyDown(keycode, event);
    if (currentComputerConfiguration.etiKeypadConfiguration.defined)
        etikeypadPointer->keyDown(keycode, event);
    for (int pad=0; pad<lastLatchKeyPad_; pad++)
    {
        if (currentComputerConfiguration.keyLatchConfiguration[pad].defined)
            latchKeyPointer[pad]->keyDown(keycode, event);
    }
    if (currentComputerConfiguration.matrixKeyboardConfiguration.defined)
        matrixKeyboardPointer->keyDownExtended(keycode, event);
    if (currentComputerConfiguration.cdp1871Configuration.defined)
        return keyDownExtended1871(keycode, event);
    return false;
}

bool Computer::keyUpReleased(int key)
{
    if (currentComputerConfiguration.matrixKeyboardConfiguration.defined)
        matrixKeyboardPointer->keyUpExtended(key);
    return false;

}

bool Computer::keyUpReleased(int key, wxKeyEvent& event)
{
    if (currentComputerConfiguration.runPressType == RUN_TYPE_UC1800)
    {
        if (key == inKey1_)
        {
            onInButtonRelease();
            return true;
        }
        if (key == inKey2_)
        {
            onRun();
            return true;
        }
    }
    
    if (key == inKey1_ || key == inKey2_)
    {
        onInButtonRelease();
        return true;
    }
    
    onHexKeyUp(key);
    if (currentComputerConfiguration.asciiKeyboardConfiguration.defined)
        keyboardUp();
    if (currentComputerConfiguration.ps2KeyboardConfiguration.defined)
    {
        keyUpPs2(key);
        return true;
    }
    if (currentComputerConfiguration.cdp1871Configuration.defined)
        keyUp1871(key);
    if (currentComputerConfiguration.efButtonsConfiguration.defined)
    {
        for (int ef=1; ef <=4; ef++)
        {
            if (currentComputerConfiguration.efButtonsConfiguration.key[ef].defined)
            {
                if (currentComputerConfiguration.efButtonsConfiguration.key[ef].resetInp == -1)
                {
                    if (currentComputerConfiguration.efButtonsConfiguration.key[ef].mod)
                    {
                        efKeyValue[ef] = currentComputerConfiguration.efButtonsConfiguration.keyPressed ^ 1;
                        if (currentComputerConfiguration.efButtonsConfiguration.key[ef].value == event.GetModifiers())
                            efKeyValue[ef] = currentComputerConfiguration.efButtonsConfiguration.keyPressed;
                    }
                    else
                    {
                        if (currentComputerConfiguration.efButtonsConfiguration.key[ef].value == key)
                            efKeyValue[ef] = currentComputerConfiguration.efButtonsConfiguration.keyPressed ^ 1;
                    }
                }
            }
        }
    }
    for (std::vector<EfButton>::iterator efButton = currentComputerConfiguration.efButtonsConfiguration.efButton.begin (); efButton != currentComputerConfiguration.efButtonsConfiguration.efButton.end (); ++efButton)
    {
        if (efButton->value == key)
        {
            for (std::vector<int>::iterator efFlag = efButton->flag.begin (); efFlag != efButton->flag.end (); ++efFlag)
            {
                efKeyValue[*efFlag] = currentComputerConfiguration.efButtonsConfiguration.keyPressed ^ 1;
            }
        }
    }
    for (int pad=0; pad<=lastBitKeyPad_; pad++)
    {
        if (currentComputerConfiguration.bitKeypadConfiguration[pad].defined)
            bitkeypadPointer[pad]->keyUp(key);
    }
    if (currentComputerConfiguration.cvKeypadConfiguration.defined)
       cvkeypadPointer->keyUp(key, event);
    if (currentComputerConfiguration.etiKeypadConfiguration.defined)
        etikeypadPointer->keyUp(key, event);
    for (int pad=0; pad<lastLatchKeyPad_; pad++)
    {
        if (currentComputerConfiguration.keyLatchConfiguration[pad].defined)
            latchKeyPointer[pad]->keyUp(key, event);
    }
    if (currentComputerConfiguration.matrixKeyboardConfiguration.defined)
        matrixKeyboardPointer->keyUpExtended(key);
    return false;
}

void Computer::onButtonRelease(wxCommandEvent& event)
{
    onInButtonRelease();
    event.Skip();
}

void Computer::onButtonPress(wxCommandEvent& event)
{
    onInButtonPress(IN_BUTTON_PUSH);
    event.Skip();
}

void Computer::onSwitchButtonPress(wxCommandEvent& event)
{
    onInButtonPress(IN_BUTTON_SWITCH);
    event.Skip();
}

void Computer::onInButtonPress(bool switchButton)
{
    inPressed_ = true;
    if (currentComputerConfiguration.etiKeypadConfiguration.defined)
        etikeypadPointer->onInButtonPress();
    
    if (cpuMode_ == LOAD)
    {
        Byte value = getData(switchButton);
        if (singleStateStep_)
        {
            singleStateStep_ = false;
            dmaIn(value);
            showData(value);
            singleStateStep_ = true;
        }
        else
        {
            showData(dmaIn(value));
        }
        if (currentComputerConfiguration.ledDisplayConfiguration.followButton)
        {
            for (int i=0; i<8; i++)
            {
                dataSwitchState_[i] = 0;
                for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
                    panelPointer[frontPanel]->setLed(i,0);
            }
        }
    }
    else
    {
        inbuttonEfState_ = 0;
        if (singleStateStep_)
        {
            setClear(1);
            setWait(1);
            p_Main->eventUpdateTitle();
        }
    }
    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
        panelPointer[frontPanel]->inSetState(BUTTON_DOWN);
}

void Computer::onInButtonRelease()
{
    if (currentComputerConfiguration.etiKeypadConfiguration.defined)
        etikeypadPointer->onInButtonRelease();

    inPressed_ = false;
    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
        panelPointer[frontPanel]->inSetState(BUTTON_UP);
    inbuttonEfState_ = 1;
}

void Computer::configureComputer()
{
    wxString ioGroup;
    
    setEfType(1, UNDEFINED_EF1);
    setEfType(2, UNDEFINED_EF2);
    setEfType(3, UNDEFINED_EF3);
    setEfType(4, UNDEFINED_EF4);

    wxString printBuffer, efResetMessage, efMessage;

    p_Main->message("Configuring " + p_Main->getRunningComputerText());
    
    if (currentComputerConfiguration.ioGroupConfiguration.defined)
    {
        printBuffer.Printf("	Output %d: set I/O group", currentComputerConfiguration.ioGroupConfiguration.output.portNumber[0]);
        p_Main->message(printBuffer);
        if (currentComputerConfiguration.ioGroupConfiguration.output.portNumber[0] != -1)
        {
            printBuffer.Printf("	Input %d: read I/O group", currentComputerConfiguration.ioGroupConfiguration.input.portNumber[0]);
            p_Main->message(printBuffer);
        }
    }

    if (currentComputerConfiguration.flipflopConfiguration.defined)
    {
        setOutType(&currentComputerConfiguration.flipflopConfiguration.ioGroupVector, currentComputerConfiguration.flipflopConfiguration.output, "flipflop flag register");
        setEfType(&currentComputerConfiguration.flipflopConfiguration.ioGroupVector, currentComputerConfiguration.flipflopConfiguration.ef, "selected I/O group");
    }

    if (currentComputerConfiguration.monitorConfiguration.defined)
    {
        setEfType(&currentComputerConfiguration.monitorConfiguration.ioGroupVector, currentComputerConfiguration.monitorConfiguration.ef, "monitor start");
    }

    if (currentComputerConfiguration.efButtonsConfiguration.defined)
    {
        for (int ef=1; ef<=4; ef++)
        {
            if (currentComputerConfiguration.efButtonsConfiguration.key[ef].defined)
            {
                efResetMessage = "";
                if (currentComputerConfiguration.efButtonsConfiguration.key[ef].resetInp != -1)
                    efResetMessage.Printf(" (reset with input %d)", currentComputerConfiguration.efButtonsConfiguration.key[ef].resetInp);
                if (currentComputerConfiguration.efButtonsConfiguration.key[ef].mod)
                {
                    printBuffer.Printf("	EF %d: %d when ", ef, currentComputerConfiguration.efButtonsConfiguration.keyPressed);
                    p_Main->message(printBuffer + currentComputerConfiguration.efButtonsConfiguration.key[ef].modString + " is pressed" + efResetMessage);
                }
                else
                {
                    printBuffer.Printf("	EF %d: %d when key code %d is pressed", ef, currentComputerConfiguration.efButtonsConfiguration.keyPressed, currentComputerConfiguration.efButtonsConfiguration.key[ef].value);
                    p_Main->message(printBuffer + efResetMessage);
                }
            }
        }
        for (std::vector<EfButton>::iterator efButton = currentComputerConfiguration.efButtonsConfiguration.efButton.begin (); efButton != currentComputerConfiguration.efButtonsConfiguration.efButton.end (); ++efButton)
        {
            printBuffer = "";
            for (std::vector<int>::iterator efFlag = efButton->flag.begin (); efFlag != efButton->flag.end (); ++efFlag)
            {
                efMessage.Printf("%d", *efFlag);
                if (printBuffer.Len() != 0)
                    printBuffer = printBuffer + ", "+ efMessage;
                else
                    printBuffer = efMessage;
            }
            printBuffer = "	EF " + printBuffer;
            efMessage.Printf(": %d when key code %d is pressed", currentComputerConfiguration.efButtonsConfiguration.keyPressed, efButton->value);
            p_Main->message(printBuffer + efMessage);
        }
    }

    p_Main->message("");

    if (currentComputerConfiguration.bootstrapConfiguration.defined)
    {
        printBuffer.Printf("Bootstrap address: %04X", currentComputerConfiguration.bootstrapConfiguration.address);
        if (currentComputerConfiguration.bootstrapConfiguration.type == BOOTSTRAPTIMER_ONIN)
            printBuffer += " (when IN pressed)";
        p_Main->configureMessage(&currentComputerConfiguration.bootstrapConfiguration.ioGroupVector, printBuffer);
        
        switch (currentComputerConfiguration.bootstrapConfiguration.type)
        {
            case BOOTSTRAPREAD:
                printBuffer.Printf("	Read address >= %04X: release bootstrap", currentComputerConfiguration.bootstrapConfiguration.address);
                p_Main->message(printBuffer);
            break;

            case BOOTSTRAPTIMER:
            case BOOTSTRAPTIMER_ONIN:
                printBuffer.Printf("	Execute %d CPU Instructions: release bootstrap", currentComputerConfiguration.bootstrapConfiguration.timer);
                p_Main->message(printBuffer);
            break;
                
            case BOOTSTRAPOUT:
                setOutType(&currentComputerConfiguration.bootstrapConfiguration.ioGroupVector, currentComputerConfiguration.bootstrapConfiguration.output, "release bootstrap");
            break;

            case BOOTSTRAPQ:
                p_Main->message("	Q Pulse: release bootstrap");
            break;

            case BOOTSTRAPIN:
                setInType(&currentComputerConfiguration.bootstrapConfiguration.ioGroupVector, currentComputerConfiguration.bootstrapConfiguration.input, "release bootstrap");
            break;

            case BOOTSTRAPIO:
                setOutType(&currentComputerConfiguration.bootstrapConfiguration.ioGroupVector, currentComputerConfiguration.bootstrapConfiguration.output, "release bootstrap");
                setInType(&currentComputerConfiguration.bootstrapConfiguration.ioGroupVector, currentComputerConfiguration.bootstrapConfiguration.input, "release bootstrap");
            break;

            case BOOTSTRAPRUN:
                printBuffer.Printf("	Set bootstrap if RUN is pressed on startup");
                p_Main->message(printBuffer);
                printBuffer.Printf("	Read address >= %04X: release bootstrap", currentComputerConfiguration.bootstrapConfiguration.address);
                p_Main->message(printBuffer);
            break;
        }
        p_Main->message("");
    }
    
    hexModemOnStart = currentComputerConfiguration.videoTerminalConfiguration.hexModem_defined;

    if (currentComputerConfiguration.hexDisplayConfiguration.defined)
    {
        p_Main->configureMessage(&currentComputerConfiguration.hexDisplayConfiguration.ioGroupVector, "Frontpanel");

        setOutType(&currentComputerConfiguration.hexDisplayConfiguration.ioGroupVector, currentComputerConfiguration.hexDisplayConfiguration.output, "til display output");

        setInType(&currentComputerConfiguration.hexDisplayConfiguration.ioGroupVector, currentComputerConfiguration.hexDisplayConfiguration.input, "data input");
        
        setEfType(&currentComputerConfiguration.hexDisplayConfiguration.ioGroupVector, currentComputerConfiguration.hexDisplayConfiguration.ef, "0 when hex button pressed");

        setEfType(&currentComputerConfiguration.hexDisplayConfiguration.ioGroupVector, currentComputerConfiguration.hexDisplayConfiguration.inEf, "0 when in button pressed");

        setEfType(&currentComputerConfiguration.hexDisplayConfiguration.ioGroupVector, currentComputerConfiguration.hexDisplayConfiguration.inEfElf, "0 when in button pressed");

        setEfType(&currentComputerConfiguration.hexDisplayConfiguration.ioGroupVector, currentComputerConfiguration.hexDisplayConfiguration.startEf, "0 when start button pressed");

        setOutType(&currentComputerConfiguration.hexDisplayConfiguration.ioGroupVector, currentComputerConfiguration.multiSegDisplayConfiguration.segOutput, "7 segment display");
        setInType(&currentComputerConfiguration.hexDisplayConfiguration.ioGroupVector, currentComputerConfiguration.multiSegDisplayConfiguration.segInput, "reset 7 segment");
        setEfType(&currentComputerConfiguration.hexDisplayConfiguration.ioGroupVector, currentComputerConfiguration.multiSegDisplayConfiguration.segEf, "0 display start");

        if (currentComputerConfiguration.multiSegDisplayConfiguration.tilFontFile != "")
            p_Computer->readIntelFile(currentComputerConfiguration.multiSegDisplayConfiguration.tilFontDirectory + currentComputerConfiguration.multiSegDisplayConfiguration.tilFontFile, &tilFontMemory);

        setCycleType(CYCLE_TYPE_SEVEN_SEGMENT, MULTI_TIL_DISPLAY_CYCLE);

        p_Main->message("");
    }

    if (currentComputerConfiguration.emsMemoryConfiguration.size() != 0)
    {
        int emsConfigNumber = 0, rangeNumber;
        for (std::vector<EmsMemoryConfiguration>::iterator emsConfig = currentComputerConfiguration.emsMemoryConfiguration.begin (); emsConfig != currentComputerConfiguration.emsMemoryConfiguration.end (); ++emsConfig)
        {
            printBuffer = "EMS Memory";
            if (currentComputerConfiguration.emsMemoryConfiguration.size() > 1)
                printBuffer.Printf("EMS Memory %d", emsConfigNumber);
            p_Main->configureMessage(&emsConfig->ioGroupVector, printBuffer);
                
            if (emsConfig->output.portNumber[0] == -1)
            {
                if (emsConfig->outputStart != 0 || emsConfig->outputEnd != 0)
                {
                    printBuffer.Printf("	Address %04X-%04X: EMS-512KB page select", emsConfig->outputStart, emsConfig->outputEnd);
                    p_Main->message(printBuffer);
                }
            }
            else
            {
                setOutType(&emsConfig->ioGroupVector, emsConfig->output, EMS_PAGE_SELECT_OUT, "EMS-512KB page select");
            }
            
            if (emsConfig->range.size() == 1)
            {
                printBuffer.Printf("	EMS Page address range: %04X-%04X", emsConfig->range[0].start, emsConfig->range[0].end);
                p_Main->message(printBuffer);
            }
            else
            {
                rangeNumber = 1;
                for (std::vector<WordRange>::iterator range = emsConfig->range.begin (); range != emsConfig->range.end (); ++range)
                {
                    printBuffer.Printf("	EMS Page address range %d: %04X-%04X", rangeNumber, range->start, range->end);
                    p_Main->message(printBuffer);
                    rangeNumber++;
                }
            }

            p_Main->message("");
            emsConfigNumber++;
        }
    }

    resetCpu();
}

void Computer::setPrinterEf()
{
    printerEfState_ = 0;
}

void Computer::reLoadKeyDefinition(wxString fileName)
{
    runningGame_ = fileName;
    if (p_Main->getConfigBool("/Xml/GameAuto", true))
        p_Main->loadKeyDefinition("", fileName, keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_);
}

void Computer::reDefineKeysA(int hexKeyDefA1[], int hexKeyDefA2[])
{
    for (int i=0; i<16; i++)
    {
        keyDefA1_[i] = hexKeyDefA1[i];
        keyDefA2_[i] = hexKeyDefA2[i];
    }
}

void Computer::reDefineKeysB(int hexKeyDefB1[], int hexKeyDefB2[])
{
    for (int i=0; i<16; i++)
    {
        keyDefB1_[i] = hexKeyDefB1[i];
        keyDefB2_[i] = hexKeyDefB2[i];
    }
}

void Computer::initComputer()
{
//    Show(p_Main->showFrontPanel());
    for (std::vector<FrontPanelConfiguration>::iterator frontPanel = currentComputerConfiguration.frontPanelConfiguration.begin (); frontPanel != currentComputerConfiguration.frontPanelConfiguration.end (); ++frontPanel)
    {
        panelPointer.resize(numberOfFrontPanels_+1);
        
        panelPointer[numberOfFrontPanels_] = new PanelFrame(title_, frontPanel->pos, frontPanel->size);
        panelPointer[numberOfFrontPanels_]->init(frontPanel->guiItemConfiguration, frontPanel->size, frontPanel->picInterrupt);
        numberOfFrontPanels_++;
    }

    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
        panelPointer[frontPanel]->Show(p_Main->showFrontPanel());

    waitButtonState_ = 0;
    mpButtonState_ = 0;
    mpSuperButtonActive_ = false;
    for (int i=0; i<4; i++)
        mpButtonState[i] = false;
    loadButtonState_ = 1;
    runButtonState_ = 0;
    nvRamDisable_ = currentComputerConfiguration.nvRamConfiguration.disable;
    endSave_ = currentComputerConfiguration.addressLocationConfiguration.code_start;

    switches_ = 0;
    inbuttonEfState_ = 1;
    nextNybble_ = 'H';

    monitor_ = false;

    for (int i=0; i<8; i++)  dataSwitchState_[i]=0;
    for (int i=0; i<4; i++)  efSwitchState_[i]=0;
    for (int ef=1; ef <=4; ef++)
    {
        efKeyValue[ef] = 1;
        if (currentComputerConfiguration.efButtonsConfiguration.key[ef].defined)
            efKeyValue[ef] = currentComputerConfiguration.efButtonsConfiguration.keyPressed ^ 1;
    }
    
    bootstrap_ = 0;

    pulseCountStopTone_ = 2000;
    tapeFormat56_ = false;
    tapeFormatFixed_ = false;
    toneTime_ = 0;

    pulseCount_ = 0;
    tapeRecording_ = false;
    tapeActivated_ = false;
    tapeEnd_ = false;
    startBytes_ = 2;
    zeroWaveCounter_ = -1;
    tapedataReady_ = 1;
    runPressed_ = false;

    tapeRunSwitch_ = 0x2;
    tapeError_ = 1;
    inpMode_ = INP_MODE_NONE;
    cardSwitchOn_ = false;
    readSwitchOn_ = false;
    colourMask1862_ = 0xff;
    colourLatch_ = false;
    
    cdp1854Vt100Connection_ = -1;
    cdp1854Ut58Connection_ = -1;

    if (currentComputerConfiguration.runPressType == RUN_TYPE_UC1800)
        runButtonState_ = 1;
    
    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
    {
        panelPointer[frontPanel]->setNibbleLed(0, 0);
        panelPointer[frontPanel]->setNibbleLed(1, 1);
        panelPointer[frontPanel]->setPowerLed(0);
    }

    soundTempoCycleValue_ = soundTempoCycleSize_;
    vp550IntOn_ = false;
    printLatch_ = 0;
    updateQLed_ = false;
    updateTapeLed_ = false;

    cycleKeyOn_ = false;
    cycleFlashOn_ = false;
    runPressedAtStartup_ = false;

    specifiedSoftwareLoaded_ = true;
    emsButton_[0] = 0;
    emsButton_[1] = 0;

    adiChannel_ = 0;
    adiValue_ = 0;
    adOutputFunction_ = AD_CONVERTOR_SELECT;
    nanoMonitor_ = 0;
    addressLatchCounter_ = 0;

    resetComputer();
    resetTape();

    segValue_ = -1;
}

void Computer::resetComputer()
{
    inPressed_ = false;
    cassetteEf_ = 0;
    oldCassetteEf_ = 1;

    printerEfState_ = 1;
    hexEfState_ = 1;
    elfRunState_ = RESETSTATE;

    ioGroup_ = -1;
    if (currentComputerConfiguration.ioGroupConfiguration.defined)
        ioGroup_ = 0;
    qState_ = 0;
    
    lastMode_ = UNDEFINDEDMODE;
    
    thermalPrinting_ = false;
    thermalEF_ = 0;
    selectedMap_ = 0;
    if (currentComputerConfiguration.mcrConfiguration.maxMapNumber_ > 0)
    {
        mainMemory_[currentComputerConfiguration.mcrConfiguration.output.portNumber[0]] = (mainMemory_[currentComputerConfiguration.mcrConfiguration.output.portNumber[0]] & (currentComputerConfiguration.mcrConfiguration.output.mask ^ 0xff)) | selectedMap_;
    }

    intCounter_ = currentComputerConfiguration.interruptConfiguration.cycleValue;

    if (currentComputerConfiguration.dmaConfiguration.startOnFirstQ || currentComputerConfiguration.dmaConfiguration.startOnFirstOut != 0)
        dmaCounter_ = -1;
    else
        dmaCounter_ = currentComputerConfiguration.dmaConfiguration.cycleValue;
    
    systemTime_ = wxDateTime::Now();
    xmlComputerTime_ = wxDateTime::Now();
    nvramWriteProtected_ = currentComputerConfiguration.nvRamMpConfiguration.defined;

    printerStatus_ = PRIDLE;
    batchInProgress_ = false;

    if (currentComputerConfiguration.hwTapeConfiguration.format == TAPE_FORMAT_CV)
        resetTape();

    if (currentComputerConfiguration.gpioPs2KeyboardConfiguration.defined)
        resetPs2gpio();
    
    if (hexModemOnStart)
    {
        currentComputerConfiguration.videoTerminalConfiguration.hexModem_defined = hexModemOnStart;
        currentComputerConfiguration.videoTerminalConfiguration.xModem_defined = false;
        p_Main->eventSetButtonLabel ("XmodemButtonXml", "HEX");

    }
}

Byte Computer::ef(int flag)
{
    bool groupFound;

    if (currentComputerConfiguration.efButtonsConfiguration.defined)
    {
        if (currentComputerConfiguration.efButtonsConfiguration.key[flag].defined)
            return efKeyValue[flag];
    }
    
    if (currentComputerConfiguration.hexDisplayConfiguration.defined)
    {
        groupFound = false;
        
        if (currentComputerConfiguration.mc6845Configuration.ioGroupVector.size() == 0)
            groupFound = true;
        else
        {
            for (std::vector<int>::iterator ioGroupIterator = currentComputerConfiguration.hexDisplayConfiguration.ioGroupVector.begin (); ioGroupIterator != currentComputerConfiguration.hexDisplayConfiguration.ioGroupVector.end (); ++ioGroupIterator)
            {
                if (*ioGroupIterator == ioGroup_)
                    groupFound = true;
            }
        }
        if (groupFound || currentComputerConfiguration.hexDisplayConfiguration.ef.excludeIoGroup)
        {
            if (flag == currentComputerConfiguration.hexDisplayConfiguration.ef.flagNumber)
                return hexEfState_;
    
            if (flag == currentComputerConfiguration.hexDisplayConfiguration.inEf.flagNumber)
            {
                if (inbuttonEfState_ == 0)
                    return inbuttonEfState_;
            }

            if (flag == currentComputerConfiguration.hexDisplayConfiguration.inEfElf.flagNumber)
            {
                if (inPressed_ == true)
                    return 0;
            }

            if (flag == currentComputerConfiguration.hexDisplayConfiguration.startEf.flagNumber)
                return runButtonState_;
            
        }
    }

    if (currentComputerConfiguration.vis1870Configuration.defined)
    {
        groupFound = false;
        
        if (currentComputerConfiguration.vis1870Configuration.ioGroupVector.size() == 0)
            groupFound = true;
        else
        {
            for (std::vector<int>::iterator ioGroupIterator = currentComputerConfiguration.vis1870Configuration.ioGroupVector.begin (); ioGroupIterator != currentComputerConfiguration.vis1870Configuration.ioGroupVector.end (); ++ioGroupIterator)
            {
                if (*ioGroupIterator == ioGroup_)
                    groupFound = true;
            }
        }
        if (groupFound || currentComputerConfiguration.vis1870Configuration.ef.excludeIoGroup)
        {
            if (currentComputerConfiguration.vis1870Configuration.useVideoModeEf && currentComputerConfiguration.vis1870Configuration.videoModeEf == flag)
            {
                if (currentComputerConfiguration.vis1870Configuration.videoMode == NTSC)
                    return 0;
            }
            if (currentComputerConfiguration.vis1870Configuration.ef.flagNumber == flag)
            {
                return vis1870Pointer->ef1_1870();
            }
        }
    }
    
    if (currentComputerConfiguration.matrixKeyboardConfiguration.defined)
    {
        groupFound = false;
        
        if (currentComputerConfiguration.matrixKeyboardConfiguration.ioGroupVector.size() == 0)
            groupFound = true;
        else
        {
            for (std::vector<int>::iterator ioGroupIterator = currentComputerConfiguration.matrixKeyboardConfiguration.ioGroupVector.begin (); ioGroupIterator != currentComputerConfiguration.matrixKeyboardConfiguration.ioGroupVector.end (); ++ioGroupIterator)
            {
                if (*ioGroupIterator == ioGroup_)
                    groupFound = true;
            }
        }
        if (groupFound)
        {
            int returnValue = matrixKeyboardPointer->efKey(flag);
            if (returnValue != -1)
                return returnValue&0xff;
        }
    }
        
    if (currentComputerConfiguration.cdp1861Configuration.defined)
    {
        groupFound = false;
        
        if (currentComputerConfiguration.cdp1861Configuration.ioGroupVector.size() == 0)
            groupFound = true;
        else
        {
            for (std::vector<int>::iterator ioGroupIterator = currentComputerConfiguration.cdp1861Configuration.ioGroupVector.begin (); ioGroupIterator != currentComputerConfiguration.cdp1861Configuration.ioGroupVector.end (); ++ioGroupIterator)
            {
                if (*ioGroupIterator == ioGroup_)
                    groupFound = true;
            }
        }
        if (groupFound || currentComputerConfiguration.cdp1861Configuration.ef.excludeIoGroup)
        {
            if (currentComputerConfiguration.cdp1861Configuration.ef.flagNumber == flag)
            {
                if (pixiePointer->arePixieGraphicsOn() || !currentComputerConfiguration.cdp1861Configuration.efScreenOn)
                    return pixiePointer->efPixie();
            }
        }
    }

    if (currentComputerConfiguration.cdp1864Configuration.defined)
    {
        if (currentComputerConfiguration.cdp1864Configuration.ef == flag)
        {
            if (cdp1864Pointer->arePixieGraphicsOn() || !currentComputerConfiguration.cdp1864Configuration.screenOn)
                return cdp1864Pointer->efPixie();
        }
    }

    if (currentComputerConfiguration.studio4VideoConfiguration.defined)
    {
        groupFound = false;
        
        if (currentComputerConfiguration.studio4VideoConfiguration.ioGroupVector.size() == 0)
            groupFound = true;
        else
        {
            for (std::vector<int>::iterator ioGroupIterator = currentComputerConfiguration.studio4VideoConfiguration.ioGroupVector.begin (); ioGroupIterator != currentComputerConfiguration.studio4VideoConfiguration.ioGroupVector.end (); ++ioGroupIterator)
            {
                if (*ioGroupIterator == ioGroup_)
                    groupFound = true;
            }
        }
        if (groupFound || currentComputerConfiguration.studio4VideoConfiguration.ef.excludeIoGroup)
        {
            if (currentComputerConfiguration.studio4VideoConfiguration.ef.flagNumber == flag)
                return st4VideoPointer->efPixie();
        }
    }
    
    if (currentComputerConfiguration.vip2KVideoConfiguration.defined)
    {
        if (currentComputerConfiguration.vip2KVideoConfiguration.ef.flagNumber == flag)
        {
            if (vip2KVideoPointer->arePixieGraphicsOn())
                return vip2KVideoPointer->efPixie();
        }
    }

    switch(efType_[qState_][ioGroup_+1][flag])
    {
        case FLIPFLOP_EF:
            return ioGroup_ & 0x1;
        break;
            
        case SUPERBOARD_EF:
            if (!nvramWriteProtected_)
                return usb_ef();
            else
                return cassetteEf_;
        break;

        case FDC1770_EF:
            if ((fdcRegisterSelect_&0x10) == 0x10)
                return ef1770();
            else
                return cassetteEf_;
        break;

        case FDC1793_EF:
            return ef1793();
        break;

        case UPD765_EF:
            return efInterrupt();
        break;

        case THERMAL_PRINTER_EF:
            if (thermalPrinting_)
            {
                if (p_Main->getThermalEf())
                    return 1;
                else
                {
                    thermalEF_ = !thermalEF_;
                    return thermalEF_;
                }
            }
            else
                return cassetteEf_;
        break;

        case BASIC_Q_PRINTER_EF:
            if (printerStatus_ != PRIDLE)
                return true;
            else
                return cassetteEf_;
        break;

        case CENTRONICS_PRINTER_EF:
            return true;
        break;

        case BASIC_PRINTER_EF:
            return printerEfState_;
        break;

        case RTC_EF:
            return efRtcCdp1879();
        break;

        case MDU_EF:
            return cdp1855InstancePointer->readStatus();
        break;

        case MC6845_EF:
            if (!isLoading())
                return mc6845Pointer->ef6845();
            else
                return cassetteEf_;
        break;

        case TAPE_EF:
            if (tapeFinished_ > 0)
            {
                if ((tapeFinished_ & 0xff) == 0)
                    cassetteEf_ = !cassetteEf_;
                tapeFinished_--;
                if (tapeFinished_ == 0)
                    cassetteEf_ = 0;
            }
            return cassetteEf_;
        break;

        case TAPE_RUNNING_EF:
            return !(isSaving()|isLoading());
        break;
            
        case LATCH_KEYBOARD_EF:
        case LATCH_KEYPAD_EF1:
        case LATCH_KEYPAD_EF2:
            return latchKeyPointer[efType_[qState_][ioGroup_+1][flag] - LATCH_KEYBOARD_EF]->ef();
        break;

        case PS2_GPIO_EF:
            return efPs2gpio();
        break;

        case TAPE_CV_DATA_READY_EF:
            return tapedataReady_;
        break;

        case TAPE_CV_BUFFER_EMPTY_EF:
            return tapeHwReadyToReceive_;
        break;

        case CV_KEYPAD_DATA_READY_EF:
            return cvkeypadPointer->ef();
        break;

        case ETI_KEYPAD_EF:
            return etikeypadPointer->ef();
        break;

        case KEY_TAPE_FRED_EF:
            if (inpMode_ == INP_MODE_TAPE_PROGRAM)
                return tapedataReady_;

            if (currentComputerConfiguration.fredKeypadConfiguration.defined)
                return fredkeypadPointer->efHexFireA();

            return 1;
        break;

        case TAPE_CONTROL_FRED_EF:
            if (tapeRunSwitch_&1)
                return 0;
            else
                return 1;
        break;

        case KEY_FRED_COIN_ARCADE_EF:
            return fredkeypadPointer->efFireB();
        break;
            
        case TAPE_ERROR_FRED_EF:
            if (currentComputerConfiguration.fredKeypadConfiguration.defined)
            {
                if (tapeError_ == 0 || fredkeypadPointer->efCoin() == 0)
                    return 0;
                else
                    return 1;
            }
            else
            {
                if (tapeError_ == 0)
                    return 0;
                else
                    return 1;
            }
        break;

        case CDP1851_A_EF:
            return cdp1851FramePointer[efItemNumber_[qState_][ioGroup_+1][flag]]->getEfState(1);
        break;

        case CDP1851_B_EF:
            return cdp1851FramePointer[efItemNumber_[qState_][ioGroup_+1][flag]]->getEfState(2);
        break;

        case CDP1851_IRQ:
            return cdp1851FramePointer[efItemNumber_[qState_][ioGroup_+1][flag]]->getIrqState();
        break;

        case CDP1852_EF:
            return cdp1852FramePointer[efItemNumber_[qState_][ioGroup_+1][flag]]->getEfState();
        break;

        case TIMER_EF:
            return cdp1878InstancePointer[efItemNumber_[qState_][ioGroup_+1][flag]]->efInterrupt();
        break;
            
        case CD4536B_EF:
            return cd4536bPointer[efItemNumber_[qState_][ioGroup_+1][flag]]->ef();
        break;

        case ASCII_KEYBOARD_EF:
            return efKeyboard();
        break;

        case 0:
            return 1;
        break;

        case TMS_INTERRUPT_EF:
            return tmsPointer->readEf();
        break;
            
        case PS2_KEYBOARD_EF:
            return efPs2();
        break;

        case VIDEO_TERMINAL_EF:
            if (isLoading() && (currentComputerConfiguration.swTapeConfiguration.ef.flagNumber == flag || currentComputerConfiguration.hwTapeConfiguration.ef.flagNumber == flag))
                return cassetteEf_;
            else
                return vtPointer->ef();
        break;

        case UART1854_EF:
            if (isLoading() && (currentComputerConfiguration.swTapeConfiguration.ef.flagNumber == flag || currentComputerConfiguration.hwTapeConfiguration.ef.flagNumber == flag))
                return cassetteEf_;
            else
                return cdp1854InstancePointer[inItemNumber_[qState_][ioGroup_+1][flag]]->efSerialDataInput();
        break;

        case VIDEO_TERMINAL_EF_INTERRUPT:
            if (isLoading() && (currentComputerConfiguration.swTapeConfiguration.ef.flagNumber == flag || currentComputerConfiguration.hwTapeConfiguration.ef.flagNumber == flag))
                return cassetteEf_;
            else
                return vtPointer->efInterrupt();
        break;

        case UART1854_EF_INTERRUPT:
            if (isLoading() && (currentComputerConfiguration.swTapeConfiguration.ef.flagNumber == flag || currentComputerConfiguration.hwTapeConfiguration.ef.flagNumber == flag))
                return cassetteEf_;
            else
                return cdp1854InstancePointer[inItemNumber_[qState_][ioGroup_+1][flag]]->efInterrupt();
        break;
            
        case EXTERNAL_VIDEO_TERMINAL_EF:
            if (isLoading() && (currentComputerConfiguration.swTapeConfiguration.ef.flagNumber == flag || currentComputerConfiguration.hwTapeConfiguration.ef.flagNumber == flag))
                return cassetteEf_;
            else
                return p_Serial->ef();
        break;
 
        case EXTERNAL_VIDEO_TERMINAL_EF_INTERRUPT:
            if (isLoading() && (currentComputerConfiguration.swTapeConfiguration.ef.flagNumber == flag || currentComputerConfiguration.hwTapeConfiguration.ef.flagNumber == flag))
                return cassetteEf_;
            else
                return p_Serial->efInterrupt();
        break;

        case I8275_VERTICAL_EF:
            return i8275Pointer->frameEf8275();
        break;

        case I8275_HORIZONTAL_EF:
            return i8275Pointer->rowEf8275();
        break;

        case CDP1871_REPEAT_EF:
            return efKeybRepeat1871();
        break;

        case MULTI_TIL_DISPLAY_EF:
            if (segNumber_ == currentComputerConfiguration.multiSegDisplayConfiguration.multiTilNumber)
                return 1;
            else
                return 0;
        break;

        case CDP1871_EF:
            return efKeyb1871();
        break;

        case UNDEFINED_EF1:
            return currentComputerConfiguration.defaultEfConfiguration.ef1;
        break;

        case UNDEFINED_EF2:
            return currentComputerConfiguration.defaultEfConfiguration.ef2;
        break;

        case UNDEFINED_EF3:
            return currentComputerConfiguration.defaultEfConfiguration.ef3;
        break;

        case UNDEFINED_EF4:
            return currentComputerConfiguration.defaultEfConfiguration.ef4;
        break;
                        
        case NANO_MONITOR_EF:
            if (nanoMonitor_ > 0)
            {
                nanoMonitor_--;
                return 1;
            }
            else
                return 0;
        break;

        default:
            return 1;
    }
}

Byte Computer::in(Byte port, Word address)
{
    Byte ret = 255;

    if (currentComputerConfiguration.ioGroupConfiguration.defined)
    {
        if  (currentComputerConfiguration.ioGroupConfiguration.input.portNumber[0] == port)
            ret = ioGroup_ & currentComputerConfiguration.ioGroupConfiguration.input.mask;
    }

    if (currentComputerConfiguration.bootstrapConfiguration.defined)
    {
        if  (currentComputerConfiguration.bootstrapConfiguration.input.qValue == qState_ || currentComputerConfiguration.bootstrapConfiguration.input.qValue == -1)
        {
            for (std::vector<int>::iterator portNumber = currentComputerConfiguration.bootstrapConfiguration.input.portNumber.begin (); portNumber != currentComputerConfiguration.bootstrapConfiguration.input.portNumber.end (); ++portNumber)
            {
                if (port == *portNumber)
                    bootstrap_ = 0;
            }
        }
    }
    
    if (currentComputerConfiguration.efButtonsConfiguration.defined)
    {
        for (int ef=1; ef <=4; ef++)
        {
            if (currentComputerConfiguration.efButtonsConfiguration.key[ef].defined)
            {
                if (port == currentComputerConfiguration.efButtonsConfiguration.key[ef].resetInp)
                    efKeyValue[ef] = currentComputerConfiguration.efButtonsConfiguration.keyPressed ^ 1;
            }
        }
    }
        
    switch (inType_[qState_][ioGroup_+1][port])
    {
        case VT_UART1854_READ_RECEIVER_IN:
            ret = vtPointer->uartIn();
        break;

        case UART1854_READ_RECEIVER_IN:
            ret = cdp1854InstancePointer[inItemNumber_[qState_][ioGroup_+1][port]]->readReceiverHoldingRegister_();
        break;

        case EXTERNAL_VT_UART1854_READ_RECEIVER_IN:
            ret = p_Serial->uartIn();
        break;

        case VT_UART1854_READ_STATUS_IN:
            ret = vtPointer->uartStatus();
        break;

        case UART1854_READ_STATUS_IN:
            ret = cdp1854InstancePointer[inItemNumber_[qState_][ioGroup_+1][port]]->readStatusRegister();
        break;

        case EXTERNAL_VT_UART1854_READ_STATUS_IN:
            ret = p_Serial->uartStatus();
        break;

        case UART16450_READ_REGISTER_IN:
            ret = vtPointer->uart16450In();
        break;

        case EXTERNAL_UART16450_READ_REGISTER_IN:
            ret = p_Serial->uart16450In();
        break;

        case EXTERNAL_UART16450_READ_STATUS_IN:
        case UART16450_READ_STATUS_IN:
            ret = 255;
        break;

        case IDE_READ_PORT_IN:
            ret = inIde();
        break;

        case RTC_READ_PORT_IN:
            ret = inRtcDs12788();
        break;

        case AY_3_8912_DATA:
            ret = ay_3_8912InstancePointer->readData();
        break;

        case MDU_X:
            return cdp1855InstancePointer->readX();
        break;

        case MDU_Y:
            return cdp1855InstancePointer->readY();
        break;

        case MDU_Z:
            return cdp1855InstancePointer->readZ();
        break;

        case MDU_STATUS:
            return cdp1855InstancePointer->readStatus();
        break;

        case IDE_READ_STATUS_IN:
            ret = readIdeStatus();
        break;

        case FDC1770_READ_IN:
            ret = readRegister1770();
            inValues_[port] = ret;
            return ret;
        break;

        case FDC1770_INTRQ_IN:
            ret = intrqStatus1770();
            inValues_[port] = ret;
            return ret;
        break;
            
        case FDC1793_READ_IN:
            ret = in1793();
            inValues_[port] = ret;
            return ret;
        break;

        case UPD765_READ_STATUS_IN:
            return inputMasterStatus();
        break;

        case UPD765_READ_COMMAND_IN:
            return inputCommandStatus();
        break;

        case PARALLEL_PRINTER_IN:
            ret = p_PrinterParallel->inParallel();
        break;

        case SERIAL_PRINTER_IN:
            ret = p_PrinterSerial->inSerial();
        break;

        case THERMAL_PRINTER_IN:
            ret = p_PrinterThermal->inThermal();
        break;
            
        case NVRAM_MEMORY_PROTECT_IN:
            nvramWriteProtected_ = !nvramWriteProtected_;
            if (nvramWriteProtected_)
                return 1;
            else
                return 0;
        break;

        case SUPERBOARD_UART_IN:
            ret = usbIn();
        break;

        case SUPERBOARD_RESET_EF_IN:
            ret = usbIn6();
        break;

        case BIT_KEYPAD_IN_0:
        case BIT_KEYPAD_IN_1:
        case BIT_KEYPAD_IN_2:
        case BIT_KEYPAD_IN_3:
        case BIT_KEYPAD_IN_4:
        case BIT_KEYPAD_IN_5:
        case BIT_KEYPAD_IN_6:
            ret = bitkeypadPointer[inType_[qState_][ioGroup_+1][port]-BIT_KEYPAD_IN_0]->in();
            if (bitKeypadValue_ != 0)
            {
                if ( (ret != 0xff && currentComputerConfiguration.bitKeypadConfiguration[inType_[qState_][ioGroup_+1][port]-BIT_KEYPAD_IN_0].bitKeyPressed == 0) || (ret != 0 && currentComputerConfiguration.bitKeypadConfiguration[inType_[qState_][ioGroup_+1][port]-BIT_KEYPAD_IN_0].bitKeyPressed == 1) )
                {
                    for (int pad=0; pad<=lastBitKeyPad_; pad++)
                        bitkeypadPointer[pad]->keysUp();
                    for (int ef=1; ef<5; ef++)
                    {
                        if (currentComputerConfiguration.bitKeypadEf[ef])
                            efKeyValue[ef] = 1;
                    }
                    bitKeypadValue_ = 0;
                }
            }
        break;

        case CV_KEYPAD_READ_DATA_IN:
            ret = cvkeypadPointer->in();
        break;

        case ETI_KEYPAD_IN:
            ret = etikeypadPointer->in(address);
        break;

        case MATRIX_KEYBOARD_IN:
            if (currentComputerConfiguration.matrixKeyboardConfiguration.useAddress)
                ret = matrixKeyboardPointer->in(address);
            else
                ret = matrixKeyboardPointer->in();
        break;

        case PS2_GPIO_READ_DATA_IN:
            ret = inPs2gpio();
        break;

        case CDP1864_ENABLE_IN:
            ret = cdp1864Pointer->inPixie();
        break;

        case CDP1864_DISABLE_IN:
            ret = 255;
            cdp1864Pointer->outPixie();
        break;

        case VIP2K_VIDEO_ENABLE_IN:
            ret = vip2KVideoPointer->inPixie();
        break;

        case VIP2K_VIDEO_DISABLE_OUT:
            ret = 255;
            vip2KVideoPointer->outPixie();
        break;

        case TAPE_CV_READ_DATA_IN:
//            p_Main->startHwLoad();
            ret = lastTapeInpt_;
            tapedataReady_ = 1;
        break;

        case KEY_TAPE_FRED_READ_DATA_IN:
            ret = 255;
            switch (inpMode_)
            {
                case INP_MODE_NONE:
                    ret = 255;
                break;

                case INP_MODE_KEYPAD:
                    ret = fredkeypadPointer->inHex();
                break;

                case INP_MODE_TAPE_PROGRAM:
                    ret = lastTapeInpt_;
                    tapedataReady_ = 1;
                break;
            }
        break;

        case KEY_FRED_COIN_ARCADE_IN:
            ret = fredkeypadPointer->inCoin();
        break;
            
        case CDP1851_READ_A_OUT:
            return cdp1851FramePointer[inItemNumber_[qState_][ioGroup_+1][port]]->readPortA();
        break;

        case CDP1851_READ_B_OUT:
            return cdp1851FramePointer[inItemNumber_[qState_][ioGroup_+1][port]]->readPortB();
        break;

        case CDP1851_READ_STATUS_IN:
            return cdp1851FramePointer[inItemNumber_[qState_][ioGroup_+1][port]]->readStatusRegister();
        break;

        case CDP1852_READ_IN:
            return cdp1852FramePointer[inItemNumber_[qState_][ioGroup_+1][port]]->readPort();
        break;

        case TIMER_INTERRUPT:
            return cdp1878InstancePointer[inItemNumber_[qState_][ioGroup_+1][port]]->readInterrupt();
        break;

        case TIMER_COUNTER_HIGH_A:
        case TIMER_COUNTER_HIGH_B:
            return cdp1878InstancePointer[inItemNumber_[qState_][ioGroup_+1][port]]->readCounterHigh(inType_[qState_][ioGroup_+1][port]-TIMER_COUNTER_HIGH_A);
        break;

        case TIMER_COUNTER_LOW_A:
        case TIMER_COUNTER_LOW_B:
            return cdp1878InstancePointer[inItemNumber_[qState_][ioGroup_+1][port]]->readCounterLow(inType_[qState_][ioGroup_+1][port]-TIMER_COUNTER_LOW_A);
        break;

        // Folowing I/O is not adapted to ioGroups
        case TMS_DATA_PORT_OUT:
            ret = tmsPointer->readVRAM();
        break;

        case TMS_REGISTER_PORT_OUT:
            ret = tmsPointer->readDataPort();
        break;

        case CDP1861_ENABLE_IN:
            ret = pixiePointer->inPixie();
        break;

        case CDP1861_DISABLE_OUT:
            ret = 255;
            pixiePointer->outPixie();
        break;

        case I8275_READ_PARAMETER_IN:
            ret = i8275Pointer->pRegRead();
        break;

        case I8275_READ_STATUS_IN:
            ret = i8275Pointer->sRegRead();
        break;

        case ASCII_KEYBOARD_IN:
            ret = inKeyboard();
        break;

        case PS2_KEYBOARD_IN:
            ret = inPs2();
        break;

        case HEX_KEY_IN:
            inPressed_ = false;
            ret = getData(false);
        break;
            
        case SWITCHES_IN_DIRECT:
            inPressed_ = false;
            if (thumbSwitchValue_ != -1)
                ret = thumbSwitchValue_;
            else
                ret = getData(true);
        break;

        case PORT_EXTENDER_IN:
            ret = inPortExtender();
        break;

        case CDP1871_IN:
            ret = inKeyb1871();
        break;

        case COMX_DIAGNOSTIC_BOARD_IN1:
            if (diagRomActive_)
            {
                diagRomActive_ = false;
                if (currentComputerConfiguration.vis1870Configuration.defined)
                    p_Main->eventUpdateLedStatus(diagRomActive_, 1);
            }
            ret = getDiagInput();
        break;
            
        case COMX_DIAGNOSTIC_BOARD_IN2:
            ret = 0;
            if (currentComputerConfiguration.diagnosticBoardConfiguration.checksum == 0)
                ret = 2;
            if (dmaCounter_ == -100)
                ret = ret ^ 4;
            if (currentComputerConfiguration.diagnosticBoardConfiguration.factory == 1)
                ret = ret ^ 8;
            // bit 1 ROM checksum
            // bit 2 IDEN itself
            // bit 3 keyboard debounce, repeat and IDEN
        break;
            
        case DIP_IN:
            ret = inDip();
        break;

        case AD_CONVERTOR_IN:
            ret = 0;
            switch (adInputNumber_)
            {
                case 1:
                    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
                    {
                        if (panelPointer[frontPanel]->isAdiDefined())
                            adiValue_ = panelPointer[frontPanel]->getAdi(adiChannel_);
                    }
                break;
                case 2:
                    if (adiValue_ < 0)
                        ret = 10;
                    else
                        ret = int(adiValue_/100) & 0xf;
                break;
                case 4:
                    if (adiValue_ < 0)
                        ret = int(-adiValue_-int(-adiValue_/10)*10) & 0xf;
                    else
                        ret = int(adiValue_-int(adiValue_/10)*10) & 0xf;
                break;
                case 6:
                    if (adiValue_ < 0)
                        ret = int( (-adiValue_-int(-adiValue_/100)*100) / 10) & 0xf;
                    else
                        ret = int( (adiValue_-int(adiValue_/100)*100) / 10) & 0xf;
                break;
            }
            if (adiValue_ < -99)
                ret = 10;
            if (adiValue_ > 999)
                ret = 11;
            adInputNumber_++;
        break;
                        
        case MULTI_TIL_DISPLAY_IN:
            segNumber_ = 0;
            ret = 255;
        break;
    }
   
    Byte mask = currentComputerConfiguration.ioMask[inType_[qState_][ioGroup_+1][port]];
    if (mask != 0xff)
        ret &= mask;
    inValues_[port] = ret;
    return ret;
}

Byte Computer::inDip()
{
    return currentComputerConfiguration.dipConfiguration.value;
}

Byte Computer::getData(bool switchButton)
{
    if (switchButton)
    {
        return(dataSwitchState_[7]?128:0) +(dataSwitchState_[6]?64:0) +(dataSwitchState_[5]?32:0) +(dataSwitchState_[4]?16:0) +(dataSwitchState_[3]?8:0) +(dataSwitchState_[2]?4:0) +(dataSwitchState_[1]?2:0) +(dataSwitchState_[0]?1:0);
    }
    else
        return switches_;
}

void Computer::out(Byte port, Word address, Byte value)
{
    Byte mask = currentComputerConfiguration.ioMask[outType_[qState_][ioGroup_+1][port]];
    if (mask != 0xff)
        value &= mask;
    outValues_[port] = value;
    bool groupFound;

    if (currentComputerConfiguration.ioGroupConfiguration.defined)
    {
        if  (currentComputerConfiguration.ioGroupConfiguration.output.portNumber[0] == port)
            ioGroup_ = value & currentComputerConfiguration.ioGroupConfiguration.output.mask;
    }

    if (currentComputerConfiguration.bootstrapConfiguration.defined)
    {
        groupFound = false;
        
        if (currentComputerConfiguration.bootstrapConfiguration.ioGroupVector.size() == 0)
            groupFound = true;
        else
        {
            for (std::vector<int>::iterator ioGroupIterator = currentComputerConfiguration.bootstrapConfiguration.ioGroupVector.begin (); ioGroupIterator != currentComputerConfiguration.bootstrapConfiguration.ioGroupVector.end (); ++ioGroupIterator)
            {
                if (*ioGroupIterator == ioGroup_)
                    groupFound = true;
            }
        }
        if (groupFound)
        {
            if  (currentComputerConfiguration.bootstrapConfiguration.output.qValue == qState_ || currentComputerConfiguration.bootstrapConfiguration.output.qValue == -1)
            {
                for (std::vector<int>::iterator portNumber = currentComputerConfiguration.bootstrapConfiguration.output.portNumber.begin (); portNumber != currentComputerConfiguration.bootstrapConfiguration.output.portNumber.end (); ++portNumber)
                {
                    if (port == *portNumber)
                        bootstrap_ = 0;
                }
            }
        }
    }

    if  (currentComputerConfiguration.dmaConfiguration.defined)
        if (currentComputerConfiguration.dmaConfiguration.startOnFirstOut == port && dmaCounter_ == -1)
            dmaCounter_ = currentComputerConfiguration.dmaConfiguration.cycleValue;

    if (currentComputerConfiguration.centronicsPrinterConfiguration.defined)
    {
        groupFound = false;
        
        if (currentComputerConfiguration.centronicsPrinterConfiguration.ioGroupVector.size() == 0)
            groupFound = true;
        else
        {
            for (std::vector<int>::iterator ioGroupIterator = currentComputerConfiguration.centronicsPrinterConfiguration.ioGroupVector.begin (); ioGroupIterator != currentComputerConfiguration.centronicsPrinterConfiguration.ioGroupVector.end (); ++ioGroupIterator)
            {
                if (*ioGroupIterator == ioGroup_)
                    groupFound = true;
            }
        }
        if (groupFound)
        {
            if  (currentComputerConfiguration.centronicsPrinterConfiguration.output.qValue == qState_ || currentComputerConfiguration.centronicsPrinterConfiguration.output.qValue == -1)
            {
                for (std::vector<int>::iterator portNumber = currentComputerConfiguration.centronicsPrinterConfiguration.output.portNumber.begin (); portNumber != currentComputerConfiguration.centronicsPrinterConfiguration.output.portNumber.end (); ++portNumber)
                {
                    if (port == *portNumber)
                    {
                        if (value != 0)
                            printLatch_ = value;
                    }
                }
            }
        }
    }
    
    switch (outType_[qState_][ioGroup_+1][port])
    {
        case FLIPFLOP_OUT:
            if (value > 3)
                value = value;
            for (int number=0; number<4; number++)
            {
                switch (currentComputerConfiguration.flipflopConfiguration.function[number])
                {
                    case FLIPFLOP_IOGROUP:
                        switch (value & 0x3)
                        {
                            case 1:
                                ioGroup_ = 0;
                            break;
                            case 2:
                                ioGroup_ = 1;
                            break;
                            case 3:
                                ioGroup_ = ioGroup_ ^ 0x1;
                            break;
                        }
                    break;
                        
                    case FLIPFLOP_VIS_MODE:
                        vis1870Pointer->setVideoMemMode(value & 0x3);
                    break;

                    case FLIPFLOP_RS232_CTS:
                        if (cdp1854Vt100Connection_ != -1)
                            cdp1854InstancePointer[cdp1854Vt100Connection_]->uartCts(value & 0x3);
                        else
                        {
                            if (currentComputerConfiguration.videoTerminalConfiguration.type != VTNONE)
                                vtPointer->uartCts(value & 0x3);
                        }
                    break;

                    case FLIPFLOP_VIS_PCB:
                        vis1870Pointer->setPcbOut(value & 0x3);
                    break;
                }
                value = value >> 2;
            }
        break;
            
        case VT_UART1854_LOAD_TRANSMITTER_OUT:
             vtPointer->uartOut(value);
        break;

        case UART1854_LOAD_TRANSMITTER_OUT:
            cdp1854InstancePointer[inItemNumber_[qState_][ioGroup_+1][port]]->writeTransmitterHoldingRegister(value);
        break;

        case EXTERNAL_VT_UART1854_LOAD_TRANSMITTER_OUT:
             p_Serial->uartOut(value);
        break;

        case VT_UART1854_LOAD_CONTROL_OUT:
            vtPointer->uartControl(value);
        break;
            
        case UART1854_LOAD_CONTROL_OUT:
            cdp1854InstancePointer[inItemNumber_[qState_][ioGroup_+1][port]]->writeControlRegister(value);
        break;
            
        case EXTERNAL_VT_UART1854_LOAD_CONTROL_OUT:
            p_Serial->uartControl(value);
        break;

        case UART16450_CONTROL_OUT:
             vtPointer->selectUart16450Register(value);
        break;

        case EXTERNAL_UART16450_CONTROL_OUT:
            p_Serial->selectUart16450Register(value);
        break;

        case UART16450_WRITE_REGISTER_OUT:
            vtPointer->uart16450Out(value);
        break;
            
        case EXTERNAL_UART16450_WRITE_REGISTER_OUT:
            p_Serial->uart16450Out(value);
        break;

        case IDE_SELECT_PORT_OUT:
            selectIdeRegister(value);
        break;

        case IDE_WRITE_PORT_OUT:
            outIde(value);
        break;

        case RTC_SELECT_PORT_OUT:
            selectRtcRegisterDs12788(value);
        break;

        case RTC_WRITE_PORT_OUT:
            outRtcDs12788(value);
        break;

        case AY_3_8912_REGISTER_ADDRESS_1:
        case AY_3_8912_REGISTER_ADDRESS_2:
            ay_3_8912InstancePointer->writeRegister(outType_[qState_][ioGroup_+1][port]-AY_3_8912_REGISTER_ADDRESS_1, value);
        break;

        case AY_3_8912_DATA:
            ay_3_8912InstancePointer->writeData(value);
        break;

        case MDU_X:
            cdp1855InstancePointer->writeX(value);
        break;

        case MDU_Y:
            cdp1855InstancePointer->writeY(value);
        break;

        case MDU_Z:
            cdp1855InstancePointer->writeZ(value);
        break;

        case MDU_CONTROL:
            cdp1855InstancePointer->writeControl(value);
        break;

        case VIS1870_OUT2:
            vis1870Pointer->setInterruptEnable(value == 1);
        break;
            
        case VIS1870_INT_RESET:
            requestInterrupt(INTERRUPT_TYPE_VIS, false, currentComputerConfiguration.vis1870Configuration.picInterrupt);
        break;

        case VIS1870_OUT3:
            vis1870Pointer->out3_1870(value);
        break;
            
        case VIS1870_OUT4:
            outValues_[port] = address;
            vis1870Pointer->out4_1870(address);
        break;
            
        case VIS1870_OUT5:
            outValues_[port] = address;
            vis1870Pointer->out5_1870(address);
        break;
            
        case VIS1870_OUT6:
            outValues_[port] = address;
            vis1870Pointer->out6_1870(address);
        break;
            
        case VIS1870_OUT7:
            outValues_[port] = address;
            vis1870Pointer->out7_1870(address);
        break;

        case VIS1870_TELMAC_REGISTER_OUT:
            vismacRegisterLatch_ = value;
        break;
            
        case VIS1870_TELMAC_DATA_OUT:
            switch (vismacRegisterLatch_)
            {
                case 0x20:
                    vis1870Pointer->out2_1870(value);
                break;
                    
                case 0x30:
                    vis1870Pointer->out3_1870(value);
                break;
                
                case 0x40:
                    outValues_[port] = address;
                    vis1870Pointer->out4_1870(address);
                break;
                    
                case 0x50:
                    outValues_[port] = address;
                    vis1870Pointer->out5_1870(address);
                break;
                    
                case 0x60:
                    outValues_[port] = address;
                    vis1870Pointer->out6_1870(address);
                break;
                    
                case 0x70:
                    outValues_[port] = address;
                    vis1870Pointer->out7_1870(address);
                break;
            }
        break;

        case FDC1770_SELECT_OUT:
            selectRegister1770(value);
        break;

        case FDC1770_WRITE_OUT:
            writeRegister1770(value);
        break;

        case FDC1793_SELECT_OUT:
            selectRegister1793(value);
        break;

        case FDC1793_WRITE_OUT:
            writeRegister1793(value);
        break;

        case UPD765_DMA_CONTROL_OUT:
            outputDmaControl(value);
        break;

        case UPD765_DMA_COUNT_OUT:
            outputDmaCounter(value);
        break;

        case UPD765_WRITE_COMMAND_OUT:
            outputCommand(value);
        break;

        case PARALLEL_PRINTER_OUT:
            p_Main->eventPrintParallel(value);
        break;

        case SERIAL_PRINTER_OUT:
            p_Main->eventPrintSerial(value);
        break;

        case THERMAL_PRINTER_OUT:
            thermalPrinting_ = true;
            p_Main->eventPrintThermal(value, flipFlopQ_);
        break;

        case BASIC_PRINTER_OUT:
            if ((value & 0xfc) != 0)
                p_Printer->printerOut(value);
        break;

        case SUPERBOARD_UART_OUT:
            usbOutNoEfCheck(value);
        break;

        case MATRIX_KEYBOARD_OUT:
            matrixKeyboardPointer->setRow(value);
        break;

        case LATCH_KEYBOARD_OUT:
        case LATCH_KEYPAD_OUT1:
        case LATCH_KEYPAD_OUT2:
            latchKeyPointer[outType_[qState_][ioGroup_+1][port] - LATCH_KEYBOARD_OUT]->out(value);
        break;

        case LATCH_KEYPAD_DOUBLE_OUT:
            if (currentComputerConfiguration.keyLatchConfiguration[1].defined)
                latchKeyPointer[1]->out(value);
            if (currentComputerConfiguration.keyLatchConfiguration[2].defined)
                latchKeyPointer[2]->out(value);
        break;

        case PS2_GPIO_WRITE_CONTROL_OUT:
            writeGpioControlRegister(value);
        break;

        case TAPE_CV_WRITE_DATA_OUT:
       //     p_Main->startHwSave();
       //     tapeRecording_ = true;
            outSaveTapeHw(value);
        break;

        case TAPE_ON_OFF_OUT:
            if (value == 1)
            {
                p_Main->startCassetteLoad(0);
                return;
            }
            if (value == 2)
            {
                p_Main->startCassetteSave(0);
                return;
            }
            if (currentComputerConfiguration.swTapeConfiguration.stopDelay <= 0 || !isSaving())
                p_Main->stopCassette();
            else
            {
                if (stopTapeCounter_ == 0)
                    stopTapeCounter_ = (currentComputerConfiguration.swTapeConfiguration.stopDelay * sampleRate_) / 1000;
            }
        break;

        case TAPE_MICRO_OUT:
            tapeIo(value);
        break;

        case TAPE_Q_OUT:
            psaveAmplitudeChange(value&1);
        break;

        case BIT_SOUND_OUT:
            startQTone(0, value == mask);
            startQTone(1, value == mask);
        break;

        case CDP1862_BACKGROUND_OUT:
            pixiePointer->outPixieBackGround();
        break;

        case CDP1864_BACKGROUND_OUT:
            cdp1864Pointer->outPixieBackGround();
        break;

        case STUDIOIV_VIDEO_OUT:
            st4VideoPointer->outPixieStudioIV(value);
        break;

        case STUDIOIV_VIDEO_DMA_ENABLE_OUT:
            st4VideoPointer->dmaEnable();
        break;
            
        case CDP1864_COLORRAM_OUT:
            address = ((address >> 1) & 0xf8) + (address & 0x7);
            colorMemory1864_[address] = value;
            if (address >= memoryStart_ && address <(memoryStart_+256))
                p_Main->updateDebugMemory(address);
        break;
           
        case CDP1862_COLORRAM_OUT:
            address = ((address >> 1) & 0xf8) + (address & 0x7);
            colorMemory1862_[address] = value;
            if (address >= memoryStart_ && address <(memoryStart_+256))
                p_Main->updateDebugMemory(address);
        break;

        case CDP1863_TONE_LATCH_OUT:
        case CDP1864_TONE_LATCH_OUT:
            if (currentComputerConfiguration.cdp1864Configuration.colorLatch)
                colourLatch_ = (value & 1) == 1;
            if (value == 0)  value = 128;
            setTonePeriod(0, 32 *(value + 1), false);
            setTonePeriod(1, 32 *(value + 1), false);
        break;

        case CDP1863_TONE_SWITCH_OUT1:
        case CDP1863_TONE_SWITCH_OUT2:
            if (value != 0)
            {
                startTone(0, true);
                startTone(1, true);
            }
            else
            {
                startTone(0, false);
                startTone(1, false);
            }
        break;

        case VIP2K_VIDEO_ENABLE_IN:
            vip2KVideoPointer->inPixie();
        break;

        case VIP2K_VIDEO_DISABLE_OUT:
            vip2KVideoPointer->outPixie();
        break;

        case FRED_VIDEO_TYPE_OUT:
            if (value == 0)
                fredVideoPointer->outPixie();
            else
                fredVideoPointer->inPixie();
            fredVideoPointer->setDisplayType(value);
        break;

        case KEY_FRED_MODE_OUT:
            if (value  == 1)
                inpMode_ = INP_MODE_KEYPAD;
            if (value  == 2)
                inpMode_ = INP_MODE_KEY_DIRECT;
            fredkeypadPointer->setInputMode(inpMode_);
        break;

        case TAPE_MODE_FRED_OUT:
            if ((value & 0x10) == 0x10 && !tapeEnd_)
            {
                inpMode_ = INP_MODE_TAPE_PROGRAM;
                startLoad(0, false);
            }

            if ((value & 0x20) == 0x20 && !tapeEnd_)
            {
                inpMode_ = INP_MODE_TAPE_DIRECT;
                startLoad(0, false);
            }

            if ((value & 0x40) == 0x40)
            {
                inpMode_ = INP_MODE_NONE;
                if (tapeActivated_)
                {
                    p_Computer->stopTape();
                    tapeActivated_ = false;
                }
                if (!tapeRecording_)
                    p_Main->startCassetteSave(0);
                
                tapeRecording_ = true;
            }
            
            if (value == 0)
                inpMode_ = INP_MODE_NONE;
            
            fredkeypadPointer->setInputMode(inpMode_);
        break;

        case TAPE_SOUND_FRED_OUT:
            if ((value&1) != (tapeRunSwitch_&1) && !tapeRecording_ && (value&4) != 4)
            {
                if ((value&1) == 1)
                    startLoad(0, false);
                else
                {
                    if (tapeActivated_)
                    {
                        p_Computer->pauseTape();
                        p_Main->turboOff();
                    }
                }
            }
            
            if ((value&2) != (tapeRunSwitch_&2))
            {
                if ((value&2) == 2)
                    p_Computer->setVolume(p_Main->getVolume());
                else
                {
                    if ((value&4) != 4)
                        p_Computer->setVolume(0);
                }
            }
            
            if ((value&4) != (tapeRunSwitch_&4))
            {
                psaveAmplitudeChange((value>>2)&1);
                zeroWaveCounter_ = 7;
            }

            tapeRunSwitch_ = value;
        break;

        case ETI_KEYPAD_OUT:
            if (etikeypadPointer->out(address, value) && currentComputerConfiguration.cdp1864Configuration.defined)
                cdp1864Pointer->initiateColour(true);
        break;

        case CDP1851_WRITE_A_OUT:
            cdp1851FramePointer[outItemNumber_[qState_][ioGroup_+1][port]]->writePortA(value);
            cdp1851FramePointer[outItemNumber_[qState_][ioGroup_+1][port]]->refreshLeds();
        break;

        case CDP1851_WRITE_B_OUT:
            cdp1851FramePointer[outItemNumber_[qState_][ioGroup_+1][port]]->writePortB(value);
            cdp1851FramePointer[outItemNumber_[qState_][ioGroup_+1][port]]->refreshLeds();
        break;

        case CDP1851_WRITE_CONTROL_OUT:
            cdp1851FramePointer[outItemNumber_[qState_][ioGroup_+1][port]]->writeControlRegister(value);
            cdp1851FramePointer[outItemNumber_[qState_][ioGroup_+1][port]]->refreshLeds();
        break;

        case CDP1852_WRITE_OUT:
            cdp1852FramePointer[outItemNumber_[qState_][ioGroup_+1][port]]->writePort(value);
            cdp1852FramePointer[outItemNumber_[qState_][ioGroup_+1][port]]->refreshLeds();
        break;

        case TIMER_CONTROL_A:
        case TIMER_CONTROL_B:
            cdp1878InstancePointer[outItemNumber_[qState_][ioGroup_+1][port]]->writeControl(outType_[qState_][ioGroup_+1][port]-TIMER_CONTROL_A, value);
        break;

        case TIMER_COUNTER_HIGH_A:
        case TIMER_COUNTER_HIGH_B:
            cdp1878InstancePointer[outItemNumber_[qState_][ioGroup_+1][port]]->writeCounterHigh(outType_[qState_][ioGroup_+1][port]-TIMER_COUNTER_HIGH_A, value);
        break;

        case TIMER_COUNTER_LOW_A:
        case TIMER_COUNTER_LOW_B:
            cdp1878InstancePointer[outItemNumber_[qState_][ioGroup_+1][port]]->writeCounterLow(outType_[qState_][ioGroup_+1][port]-TIMER_COUNTER_LOW_A, value);
        break;

        case CD4536B_WRITE_OUT:
            cd4536bPointer[outItemNumber_[qState_][ioGroup_+1][port]]->writeControl(value);
        break;

            // Folowing I/O is not adapted to ioGroups
        case MEMORY_SLOT_OUT:
            slotOut(value);
        break;
            
        case TMS_REGISTER_PORT_OUT:
            tmsPointer->modeHighOut(value);
        break;

        case TMS_DATA_PORT_OUT:
            tmsPointer->writeVRAM(value);
        break;

        case COIN_VIDEO_ENABLE_OUT:
            coinPointer->inPixie();
        break;

        case CDP1861_DISABLE_OUT:
            pixiePointer->outPixie();
        break;

        case CDP1861_ENABLE_IN:
            pixiePointer->inPixie();
        break;

        case MC6847_OUT:
            mc6847Pointer->outMc6847(value);
        break;

        case I8275_WRITE_PARAMETER_OUT:
            i8275Pointer->pRegWrite(value);
        break;

        case I8275_WRITE_COMMAND_OUT:
            i8275Pointer->cRegWrite(value);
        break;

        case VIDEO_TERMINAL_OUT:
            vtPointer->out(value);
        break;

        case EXTERNAL_VIDEO_TERMINAL_OUT:
            p_Serial->out(value);
        break;

        case PS2_KEYBOARD_OUT:
            outPs2(value);
        break;

        case HEX_DISPLAY_OUT:
            inPressed_ = false;
            showData(value);
        break;

        case PORT_EXTENDER_SELECT_OUT:
            selectPortExtender(value);
        break;

        case PORT_EXTENDER_WRITE_OUT:
            outPortExtender(value);
        break;

        case EMS_PAGE_SELECT_OUT:
            for (std::vector<EmsMemoryConfiguration>::iterator emsConfig = currentComputerConfiguration.emsMemoryConfiguration.begin (); emsConfig != currentComputerConfiguration.emsMemoryConfiguration.end (); ++emsConfig)
            {
                for (std::vector<int>::iterator portNumber = emsConfig->output.portNumber.begin (); portNumber != emsConfig->output.portNumber.end (); ++portNumber)
                {
                    if (port == *portNumber)
                    {
                        emsConfig->page = value & emsConfig->output.mask;
                        p_Main->updateSlotInfo();
                    }
                }
            }
        break;

        case COMX_DIAGNOSTIC_BOARD_OUT:
            if (currentComputerConfiguration.diagnosticBoardConfiguration.factory == 0)
                return;
            diagOut(value);
        break;

        case LED_CIDELSA_OUT:
            p_Video[currentComputerConfiguration.vis1870Configuration.videoNumber]->updateStatusLed ((value&0x80) == 0x80, 0);
            p_Video[currentComputerConfiguration.vis1870Configuration.videoNumber]->updateStatusLed ((value&0x40) == 0x40, 1);
            p_Video[currentComputerConfiguration.vis1870Configuration.videoNumber]->updateStatusLed ((value&0x8) == 0x8, 2);
            p_Video[currentComputerConfiguration.vis1870Configuration.videoNumber]->updateStatusLed ((value&0x10) == 0x10, 3);
            p_Video[currentComputerConfiguration.vis1870Configuration.videoNumber]->updateStatusLed ((value&0x20) == 0x20, 4);
        break;

        case MULTI_TIL_DISPLAY_OUT:
            segValue_ = value;
        break;

        case AD_CONVERTOR_OUT:
            switch (adOutputFunction_)
            {
                case AD_CONVERTOR_SELECT:
                    if (value < 15)
                    {
                        adOutputFunction_ = AD_CONVERTOR_SELECT;
                        adInputNumber_ = 1;
                        adiChannel_ = value;
                    }
                    if (value == 15)
                        adOutputFunction_ = AD_CONVERTOR_PRINT;
                break;

                case AD_CONVERTOR_PRINT:
                    adOutputFunction_ = AD_CONVERTOR_SELECT;
                    p_Printer->printerOut(value);
                break;
            }
        break;

    }
}

void Computer::printOutPecom(int q)
{
    if (!currentComputerConfiguration.qSerialPrinterConfiguration.defined)
        return;
    
    cassetteEf_ = false;
    switch (printerStatus_)
    {
        case PRIDLE:
            if (q == 0)
                printerStatus_ = PRW_START;
        break;

        case PRW_START:
            if (q)
            {
                printerStatus_ = PRBUSY;
                p_Printer->startPecomChar();
                printBit_ = 0;
                printValue_ = 0;
            }
        break;

        case PRBUSY:
            if (q == 0)
                printValue_ += (1 <<(printBit_));
            if (printBit_ == 8)
            {
                p_Main->eventPrintPecom(printValue_);
                printerStatus_ = PRIDLE;
                if (currentComputerConfiguration.matrixKeyboardConfiguration.defined)
                    matrixKeyboardPointer->clearReturn();
            }
            printBit_++;
            break;
    }
}

void Computer::onXmlF4(bool WXUNUSED(forceStart))
{
    if (currentComputerConfiguration.basicPrinterConfiguration.defined)
    {
        p_Printer->onF4();
        return;
    }

    if (currentComputerConfiguration.parallelPrinterConfiguration.defined)
    {
        p_PrinterParallel->onF4Parallel();
        return;
    }
    
    if (currentComputerConfiguration.serialPrinterConfiguration.defined)
    {
        p_PrinterSerial->onF4Serial();
        return;
    }

    if (currentComputerConfiguration.qSerialPrinterConfiguration.defined)
    {
        p_Printer->onF4();
        return;
    }

    if (currentComputerConfiguration.thermalPrinterConfiguration.defined)
    {
        p_PrinterThermal->onF4Thermal();
        return;
    }
 
    p_Printer->onF4();
}

void Computer::slotOut(Byte value)
{
    Byte temp = slotShift(value, currentComputerConfiguration.slotConfiguration.slotOutputShift) & currentComputerConfiguration.slotConfiguration.slotOutputMask;
        
    if (currentComputerConfiguration.slotConfiguration.slotOutputNumber)
    {
        if (selectedSlot_ != temp)
        {
            p_Main->expLedOffEvent();
            selectedSlot_ = temp;
            p_Main->expLedOnEvent();
        }
    }
    else
    {
        Byte newSelectedSlot = 0;
        if (temp != 0)
        {
            while ((temp & 1) == 0)
            {
                newSelectedSlot++;
                temp = temp >> 1;
            }
        }
        if (selectedSlot_ != newSelectedSlot)
        {
            p_Main->expLedOffEvent();
            selectedSlot_ = newSelectedSlot;
            p_Main->expLedOnEvent();
        }
    }
    if (currentComputerConfiguration.slotConfiguration.useIoGroup)
        ioGroup_ = selectedSlot_;

    temp = slotShift(value, currentComputerConfiguration.slotConfiguration.bankOutputShift) & currentComputerConfiguration.slotConfiguration.bankOutputMask;

    if (currentComputerConfiguration.slotConfiguration.bankOutputNumber)
    {
        selectedBank_ = temp;
    }
    else
    {
        if (temp != 0)
        {
            selectedBank_ = 0;
            while ((temp & 1) == 0)
            {
                selectedBank_++;
                temp = temp >> 1;
            }
        }
    }
    p_Main->updateSlotInfo();
}

Byte Computer::slotShift(Byte value, int shift)
{
    Byte circleBit;
    while (shift > 0)
    {
        circleBit = (value & 1) << 7;
        value = (value >> 1) | circleBit;
        shift--;
    }
    while (shift < 0)
    {
        circleBit = (value & 0x80) >> 7;
        value = (value << 1) | circleBit;
        shift++;
    }
    return value;
}

void Computer::showData(Byte val)
{
    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
        panelPointer[frontPanel]->showData(val);
    
    if (currentComputerConfiguration.multiSegDisplayConfiguration.tilFontFile != "" && wait_ == 0)
    {
        for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
        {
            panelPointer[frontPanel]->showMulti(0, val);
            panelPointer[frontPanel]->showMulti(1, val);
            panelPointer[frontPanel]->showMulti(2, val);
            panelPointer[frontPanel]->showMulti(3, val);
            panelPointer[frontPanel]->showMulti(4, val);
            panelPointer[frontPanel]->showMulti(5, val);
        }
    }

    for (int i=0; i<8; i++)
    {
        for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
            panelPointer[frontPanel]->setLed(i, val&1);
        val = val>>1;
    }
}

void Computer::showCycleData(Byte val)
{
    if (singleStateStep_)
        showData(val);
    
    if (currentComputerConfiguration.ledDisplayConfiguration.showDataOnCycle)
    {
        for (int i=0; i<8; i++)
        {
            for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
                panelPointer[frontPanel]->setLed(i, val&1);
            val = val >> 1;
        }
        if (currentComputerConfiguration.ledDisplayConfiguration.datatil[0] && currentComputerConfiguration.ledDisplayConfiguration.datatil[1])
            for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
                panelPointer[frontPanel]->showData(val);
    }
}

void Computer::showCycleAddress(Word val)
{
    if (currentComputerConfiguration.ledDisplayConfiguration.showAddressOnCycle)
    {
        for (int i=8; i<24; i++)
        {
            for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
                panelPointer[frontPanel]->setLed(i, val&1);
            val = val >> 1;
        }
        for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
            panelPointer[frontPanel]->showAddress(val);
    }
}

void Computer::cycle(int type)
{
    switch(cycleType_[type])
    {
        case 0:
            return;
        break;

        case ASCII_KEYBOARD_CYCLE:
            cycleKeyboard();
        break;

        case RTC_CYCLE:
            cycleRtcCdp1879(systemTime_, xmlComputerTime_);
        break;

        case MDU_CYCLE:
            cdp1855InstancePointer->cycle();
        break;

        case KEY_FRED_CYCLE:
            fredkeypadPointer->cycle();
        break;

        case PS2_GPIO_CYCLE:
            cyclePs2gpio();
        break;

        case BIT_KEYPAD_CYCLE:
            cycleBitKeyPad();
        break;

        case PS2_KEYBOARD_CYCLE:
            cyclePs2();
        break;

        case COIN_VIDEO_CYCLE:
            coinPointer->cyclePixieCoinArcade();
        break;
            
        case CDP1861_CYCLE:
            pixiePointer->cyclePixie();
        break;
            
        case CDP1862_CYCLE:
            pixiePointer->cyclePixie();
        break;
            
        case CDP1864_CYCLE:
            cdp1864Pointer->cyclePixieCdp1864();
        break;

        case STUDIOIV_VIDEO_CYCLE:
            st4VideoPointer->cyclePixie();
        break;
            
        case VIP2K_VIDEO_CYCLE:
            vip2KVideoPointer->cyclePixie();
        break;

        case FRED_VIDEO_CYCLE:
            fredVideoPointer->cyclePixie();
        break;

        case TMS_CYCLE:
            tmsPointer->cycleTms();
        break;

        case MC6847_CYCLE:
            mc6847Pointer->cycle6847();
        break;

        case MC6845_BLINK_CYCLE:
            mc6845Pointer->blink6845();
        break;

        case MC6845_CYCLE:
            mc6845Pointer->cycle6845();
        break;

        case SN76430N_CYCLE:
            sn76430nPointer->cycle();
        break;

        case I8275_CYCLE:
            i8275Pointer->cycle8275();
        break;

        case VIDEO_TERMINAL_CYCLE:
            vtPointer->cycleVt();
        break;

        case UART1854_CYCLE:
            for (int num=0; num<numberOfCdp1854Instances_ ; num++)
                cdp1854InstancePointer[num]->cycle();
        break;

        case EXTERNAL_VIDEO_TERMINAL_CYCLE:
            p_Serial->cycleVt();
        break;

        case FDC_CYCLE:
            cycleFdc();
        break;

        case UPD765_CYCLE:
            cycleUpd765();
        break;

        case IDE_CYCLE:
            cycleIde();
        break;

        case LED_CYCLE:
            cycleLed();
            for (int num=0; num<numberOfCdp1851Frames_; num++)
                cdp1851FramePointer[num]->interruptCycle();
        break;

        case VIS1870_CYCLE:
            vis1870Pointer->cycle1870();
        break;
            
        case VIS1870_BLINK_CYCLE:
            vis1870Pointer->blink1870();
        break;

        case CDP1871_CYCLE:
            cycleKeyb1871();
        break;

        case THERMAL_PRINTER_CYCLE:
            p_PrinterThermal->cycleThermal();
        break;
            
        case DRAM_DMA_CYCLE:
            cycleDma();
        break;
            
        case INTERRUPT_CLOCK_CYCLE:
            cycleInt();
        break;
            
        case LATCH_KEYPAD_CYCLE:
            for (int pad=0; pad<lastLatchKeyPad_; pad++)
            {
                if (currentComputerConfiguration.keyLatchConfiguration[pad].defined)
                    latchKeyPointer[pad]->cycleKeybLatch();
            }
        break;

        case MULTI_TIL_DISPLAY_CYCLE:
            multiTilCycleValue_--;
            if (multiTilCycleValue_ <= 0)
            {
                if (segValue_ != -1)
                {
                    if (segNumber_ <= currentComputerConfiguration.multiSegDisplayConfiguration.multiTilNumber)
                        for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
                            panelPointer[frontPanel]->showMulti(segNumber_, segValue_);
                    segNumber_++;
                    if (segNumber_ > currentComputerConfiguration.multiSegDisplayConfiguration.multiTilNumber && currentComputerConfiguration.multiSegDisplayConfiguration.segInput.portNumber[0] == -1)
                        segNumber_ = 0;
                    segValue_ = -1;
                }
                multiTilCycleValue_ = multiTilCycleSize_;
            }
        break;

        case SOUND_SUPER_VP550_CYCLE:
            cycleVP550();
        break;

        case CD4536B_CYCLE:
            cycleCd();
        break;
    }
}

void Computer::cycleCd()
{
    for (int num=0; num<numberOfCd4536b_ ; num++)
        cd4536bPointer[num]->cycle();
}

void Computer::cycleVP550()
{
    if (vp550IntOn_)
    {
        soundTempoCycleValue_--;
        if (soundTempoCycleValue_ <= 0)
        {
            p_Computer->interrupt();
            soundTempoCycleValue_ = soundTempoCycleSize_;
        }
    }
}

void Computer::cycleBitKeyPad()
{
    bool keyFound = false;
    
    if (getCtrlvCharNum() != 0 && bitKeypadValue_ == 0)
    {
        if (checkKeyInputAddress(scratchpadRegister_[programCounter_]))
        {
            bitKeypadValue_ = getCtrlvChar();
            
 //           if (bitKeypadValue_ >= 'a' && bitKeypadValue_ <= 'z')
  //              bitKeypadValue_ = bitKeypadValue_ - 32;

            if (bitKeypadValue_ != 0)
            {
                for (int ef=1; ef<5; ef++)
                {
                    if (currentComputerConfiguration.bitKeypadEf[ef])
                        efKeyValue[ef] = 1;
                }
                for (int pad=0; pad<=lastBitKeyPad_; pad++)
                {
                    bitkeypadPointer[pad]->keysUp();
                    if (bitkeypadPointer[pad]->keyDownCtrlV(bitKeypadValue_))
                        keyFound = true;
                }
                if (!keyFound)
                    bitKeypadValue_ = 0;
            }
        }
    }
}

void Computer::cycleDma()
{
    if (flipFlopQ_)
    {
        if (dmaCounter_ == -1)
            dmaCounter_ = currentComputerConfiguration.dmaConfiguration.cycleValue;
    }

    if (dmaCounter_ > 0)
    {
        dmaCounter_--;
        if (dmaCounter_ <= 0)
        {
            dmaOut();
            dmaCounter_ = currentComputerConfiguration.dmaConfiguration.cycleValue;
            if (!diagDmaLedOn_ && currentComputerConfiguration.diagnosticBoardConfiguration.defined)
            {
                diagDmaLedOn_ = true;
                if (currentComputerConfiguration.vis1870Configuration.defined)
                    p_Main->eventUpdateLedStatus(diagDmaLedOn_, 2);
            }
        }
    }
}

void Computer::cycleInt()
{
    if (intCounter_ >= 0)
    {
        intCounter_--;
        if (intCounter_ == 0)
        {
            interrupt();
            intCounter_ = currentComputerConfiguration.interruptConfiguration.cycleValue;
        }
    }
}

void Computer::picInterruptRequest(int type, bool state, int picNumber)
{
    int instance = picNumber >> 3;
    if (instance < numberOfCdp1877Instances_)
        cdp1877InstancePointer[instance]->requestInterrupt(type, state, picNumber & 0x7);
}

void Computer::cycleLed()
{
    if (cycleValue_ > 0)
    {
        cycleValue_ --;
        if (cycleValue_ <= 0)
        {
            cycleValue_ = cycleSize_;
            rtcRam_[0xc] |= 0x40;
            p_Main->updateDebugMemory(0xc);
        }
    }
    if (ledCycleValue_ > 0)
    {
        ledCycleValue_ --;
        if (ledCycleValue_ <= 0)
        {
            ledCycleValue_ = ledCycleSize_;
            for (int num=0; num<numberOfCdp1851Frames_; num++)
                cdp1851FramePointer[num]->ledTimeout();
            for (int num=0; num<numberOfCdp1852Frames_; num++)
                cdp1852FramePointer[num]->ledTimeout();
            for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
                panelPointer[frontPanel]->ledTimeout();
            if (updateQLed_)
            {
                p_Main->eventUpdateVipIILedStatus(BAR_LED_Q, qState_ == 1);
                updateQLed_ = false;
            }
            if (updateTapeLed_)
            {
                p_Main->eventUpdateVipIILedStatus(BAR_LED_TAPE, cassetteEf_ != 0);
                updateTapeLed_ = false;
            }
        }
        if (goCycleValue_ > 0)
        {
            goCycleValue_ --;
            if (goCycleValue_ <= 0)
            {
                goCycleValue_ = goCycleSize_;
                wxMouseState mouseState = wxGetMouseState ();
                
                if (mouseState.LeftIsDown())
                    setWait(1);
                else
                    goCycleValue_ = -1;
            }
        }
    }
    if (cycleKeyOn_)
    {
        flashCycleValue_--;
        if (flashCycleValue_ <= 0)
        {
            if (runReleased_)
            {
                runReleased_ = false;
                cycleKeyOn_ = false;
                onRun();
            }
            else
            {
                flashState_ = false;
                cycleFlashOn_ = true;
                cycleKeyOn_ = false;
                flashCycleValue_ = vipIIRunCycleSize_;
            }
        }
    }
    if (cycleFlashOn_)
    {
        flashCycleValue_--;
        if (flashCycleValue_ <= 0)
        {
            p_Main->eventUpdateVipIILedStatus(BAR_LED_RUN, flashState_);
            flashState_ = !flashState_;
            flashCycleValue_ = vipIIRunCycleSize_/5;
        }
    }
}

void Computer::setGoTimer()
{
    goCycleValue_ = goCycleSize_;
}

void Computer::setMode()
{
    if (clear_ == 0 && wait_==1)
    {
        cpuMode_ = RESET;
        if (cpuType_ > CPU1801)
            clearResetPressed_ = true;
    }
    if (clear_ == 1 && wait_==1) cpuMode_ = RUN;
    if (clear_ == 0 && wait_==0) cpuMode_ = LOAD;
    if (clear_ == 1 && wait_==0) cpuMode_ = PAUSE;

    if (cpuMode_ != RUN)
    {
        for (int video=0; video<currentComputerConfiguration.numberOfVideoTypes_; video++)
        {
            if (p_Video[video] != NULL)
                p_Video[video]->reset();
        }
    }

    if (clear_ == 0)
    {
        for (int num=0; num<numberOfCdp1851Frames_; num++)
            cdp1851FramePointer[num]->reset();
        for (int num=0; num<numberOfCdp1852Frames_; num++)
            cdp1852FramePointer[num]->reset();
    }

    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
    {
        panelPointer[frontPanel]->setStateLed(RUNLED, clear_ & wait_);
        panelPointer[frontPanel]->setStateLed(CLEARLED, clear_ ^ 1);
        panelPointer[frontPanel]->setStateLed(WAITLED, wait_ ^ 1);
    }
}

void Computer::showState(int state)
{
    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
    {
        panelPointer[frontPanel]->setStateLed(SC0LED, (state+1)&1);
        panelPointer[frontPanel]->setStateLed(SC1LED, ((state+1)&2)>>1);
        panelPointer[frontPanel]->setStateLed(DMALED, 0);
        panelPointer[frontPanel]->setStateLed(INTERRUPTLED, 0);
    }
}

void Computer::showDmaLed()
{
    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
        panelPointer[frontPanel]->setStateLed(DMALED, 1);
}

void Computer::showIntLed()
{
    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
        panelPointer[frontPanel]->setStateLed(INTERRUPTLED, 1);
}

void Computer::showStatusLed(int led, int status)
{
    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
        panelPointer[frontPanel]->setStateLed(led, status);
}

void Computer::updateStatusBarLedStatus(bool status, int led)
{
    if (currentComputerConfiguration.cdp1861Configuration.statusBarType == STATUSBAR_VIP2)
    {
        if (currentComputerConfiguration.cdp1861Configuration.defined)
            pixiePointer->updateVipLedStatus(status, led);
        if (currentComputerConfiguration.cdp1864Configuration.defined)
            cdp1864Pointer->updateVipLedStatus(status, led);
    }
}

void Computer::autoBoot()
{
    runButtonState_ = 1;
    waitButtonState_ = 1;
    setClear(runButtonState_);
    
    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
    {
        panelPointer[frontPanel]->clearSetState(BUTTON_UP);
        panelPointer[frontPanel]->setReadyLed(0);
        panelPointer[frontPanel]->setStopLed(0);
        if (currentComputerConfiguration.mainFrontPanelConfiguration.waitButtonDefined)
        {
            panelPointer[frontPanel]->waitSetState(BUTTON_UP);
            setWait(waitButtonState_);
        }
        if (cpuMode_ == RESET)
            panelPointer[frontPanel]->showAddress(0);
           
        panelPointer[frontPanel]->runSetState(BUTTON_UP);
    }

    if (currentComputerConfiguration.autoBootConfiguration.dmaOutOnBoot)
        dmaOut();

    if (currentComputerConfiguration.autoBootConfiguration.dmaOutOnBootIfMem0is0 && readMemDebug(0) == 0)
        dmaOut();

    segNumber_ = 0;
}

void Computer::switchQ(int value)
{
    if (qState_ != value)
    {
        if (currentComputerConfiguration.cdp1861Configuration.statusBarType == STATUSBAR_VIP2)
        {
            if (ledTimeMs_ == 0)
                p_Main->eventUpdateVipIILedStatus(BAR_LED_Q, value == 1);
            else
                updateQLed_ = true;
        }
    }

    if (currentComputerConfiguration.hwTapeConfiguration.format == TAPE_FORMAT_CV && currentComputerConfiguration.hwTapeConfiguration.defined && qState_ != value)
    {
        qState_ = value;
        if (qState_ == 0)
        {
            if (tapeActivated_ || tapeRecording_)
            {
                p_Main->turboOff();
                if (currentComputerConfiguration.hwTapeConfiguration.stopDelay <= 0 || tapeRecording_)
                    p_Computer->pauseTape();
                else
                    pauseTapeCounter_ = (currentComputerConfiguration.hwTapeConfiguration.stopDelay * sampleRate_) / 1000;
            }
        }
        else
        {
            pauseTapeCounter_ = 0;
            if (p_Main->getHwTapeState() == HW_TAPE_STATE_REC)
            {
                if (tapeRecording_)
                    restartHwTapeSave(TAPE_RECORD);
                else
                {
                    tapeRecording_ = p_Main->startSaveCont(0, tapeCounter_);
                }
            }
            else
                startLoad(0, false);
        }
    }

    if (currentComputerConfiguration.centronicsPrinterConfiguration.defined)
    {
        if (value == 0 && qState_ == 1 && printLatch_ != 0)
        {
            p_Printer->printerOut(printLatch_);
            printLatch_ = 0;
        }
    }

    qState_ = value;
    
    if (currentComputerConfiguration.bootstrapConfiguration.type == BOOTSTRAPQ && qState_ == 0)
        bootstrap_ = 0;
    
    qLedStatus_ = (qLedStatus_ & 2) | (value & 1);
    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
    {
        panelPointer[frontPanel]->setQLed(qLedStatus_);
        for (int i=0; i<MAX_DATA_TIL; i++)
        {
            if (currentComputerConfiguration.ledDisplayConfiguration.datatilDpQ[i])
                panelPointer[frontPanel]->showDp313Italic(value == 1, i);
        }
    }

    if (currentComputerConfiguration.videoTerminalConfiguration.type != VTNONE)
        vtPointer->switchQ(value);

    if (currentComputerConfiguration.videoTerminalConfiguration.external || currentComputerConfiguration.videoTerminalConfiguration.loop_back)
        p_Serial->switchQ(value);
    
    if (currentComputerConfiguration.diagnosticBoardConfiguration.defined)
    {
        if (currentComputerConfiguration.diagnosticBoardConfiguration.cassetteCables == 1)
            cassetteEf_ = value;
    }
}

int Computer::getMpButtonState()
{
    return mpButtonState_;
}

void Computer::onWaitButton(wxCommandEvent&WXUNUSED(event))
{
    onWaitButton();
}

void Computer::onWaitButton()
{
    if (waitButtonState_)
        waitButtonState_ = 0;
    else
        waitButtonState_ = 1;

    setWait(waitButtonState_);
}

void Computer::onStopButton(wxCommandEvent&WXUNUSED(event))
{
    p_Main->stopComputer();
}

void Computer::onPowerButton(wxCommandEvent&WXUNUSED(event))
{
    onPowerButton();
}

void Computer::onPowerButton()
{
    powerButtonState_ = !powerButtonState_;
    p_Main->setConfigBool("Computer/PowerButtonState", powerButtonState_);
    if (powerButtonState_)
        powerOn();
    else
        powerOff();
}

void Computer::powerOff()
{
    threadPointer->Pause();
    
    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
    {
        panelPointer[frontPanel]->setNibbleLed(0, 0);
        panelPointer[frontPanel]->setNibbleLed(1, 0);
        panelPointer[frontPanel]->setPowerLed(1);
        panelPointer[frontPanel]->turnOff313Italic(true);
        panelPointer[frontPanel]->ledTimeout();
    }
}

void Computer::powerOn()
{
    threadPointer->Resume();
    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
    {
        panelPointer[frontPanel]->turnOff313Italic(false);
        panelPointer[frontPanel]->ledTimeout();
    }
    initComputer();
    setClear(0);
    setWait(1);
}

void Computer::runPressed()
{
    if (currentComputerConfiguration.bootstrapConfiguration.type == BOOTSTRAPRUN)
    {
        if (cycleKeyOn_ || cycleFlashOn_ || vipMode_)
            return;

        flashCycleValue_ = vipIIRunCycleSize_;
        runReleased_ = false;
        if (cpuMode_ != RESET)
        {
            cycleKeyOn_ = true;
            cycleFlashOn_ = false;
        }
    }
}

void Computer::onRunButtonPress(wxCommandEvent&WXUNUSED(event))
{
    onRunButtonPress(false);
}

void Computer::onRunButtonPress0(wxCommandEvent&WXUNUSED(event))
{
    onRunButtonPress(true);
}

void Computer::onRunButtonPress(bool run0)
{
    if (currentComputerConfiguration.autoBootConfiguration.defined)
        scratchpadRegister_[0] = currentComputerConfiguration.autoBootConfiguration.address;
    if (cardSwitchOn_ || readSwitchOn_)
    {
        showData(dmaOut());
        
        for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
            panelPointer[frontPanel]->setReadyLed(1);
    }
    else
    {
        for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
        {
            panelPointer[frontPanel]->setReadyLed(0);
            panelPointer[frontPanel]->setStopLed(0);
        }

        singleStateStep_ = false;

 /*       if (currentComputerConfiguration.runPressType != RUN_TYPE_MICROTUTOR || cpuMode_ != LOAD)
        {
            if (currentComputerConfiguration.autoBootConfiguration.dmaOutOnBoot)
                dmaOut();

            if (currentComputerConfiguration.autoBootConfiguration.dmaOutOnBootIfMem0is0 && readMemDebug(0) == 0)
                dmaOut();
        }*/

        if (!run0)
        {
            switch (currentComputerConfiguration.bootstrapConfiguration.type)
            {
                case BOOTSTRAPTIMER:
                    addressLatchCounter_ = currentComputerConfiguration.bootstrapConfiguration.timer;
                    bootstrap_ = currentComputerConfiguration.bootstrapConfiguration.address;
                break;
                    
                case BOOTSTRAPTIMER_ONIN:
                break;

                default:
                    if (currentComputerConfiguration.bootstrapConfiguration.defined)
                        bootstrap_ = currentComputerConfiguration.bootstrapConfiguration.address;
                break;
            }
        }

        switch (currentComputerConfiguration.runPressType)
        {
            case RUN_TYPE_BOOT:
                scratchpadRegister_[0] = currentComputerConfiguration.autoBootConfiguration.address;
                if (run0)
                    scratchpadRegister_[0] = 0;
                else
                    scratchpadRegister_[0] = currentComputerConfiguration.autoBootConfiguration.address;
                
                dmaOnBoot();
                setClear(1);
                setWait(1);
            break;

            case RUN_TYPE_VELF:
                dmaOnBoot();
                runPressed_ = true;
                for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
                {
                    if (cpuMode_ == RUN)
                        panelPointer[frontPanel]->runSetState(BUTTON_DOWN);
                    else
                        panelPointer[frontPanel]->runSetState(BUTTON_UP);
                }
            break;
                
            case RUN_TYPE_UC1800:
                dmaOnBoot();
                runButtonState_ = 0;
                onRun();
                for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
                    panelPointer[frontPanel]->runSetState(BUTTON_DOWN);
            break;

            case RUN_TYPE_MICROTUTOR:
                dmaOnBoot();
                if (cpuMode_ != LOAD)
                {
                    setClear(1);
                    setWait(1);
                    p_Main->startTime();
                }
                else
                    showData(dmaOut());
                for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
                    panelPointer[frontPanel]->runSetState(BUTTON_DOWN);
            break;

            case RUN_TYPE_RESET:
                if (!singleStateStep_)
                    resetCpu();
                if (run0)
                    scratchpadRegister_[0] = 0;
                else
                    scratchpadRegister_[0] = currentComputerConfiguration.autoBootConfiguration.address;
                dmaOnBoot();
                setClear(1);
                setWait(1);
            break;
                
            case RUN_TYPE_SWITCH:
                if (runButtonState_)
                {
                    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
                        panelPointer[frontPanel]->runSetState(BUTTON_DOWN);
                    runButtonState_ = 0;
                }
                else
                {
                    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
                        panelPointer[frontPanel]->runSetState(BUTTON_UP);
                    runButtonState_ = 1;
                    p_Main->startTime();
                }
                
                dmaOnBoot();
                setClear(runButtonState_);
            break;
                
            default:
                dmaOnBoot();
                setClear(1);
                setWait(1);
            break;
        }
        
        if (cpuMode_ != RUN)
            resetEffectiveClock();

        p_Main->eventUpdateTitle();
    }
}

void Computer::dmaOnBoot()
{
    if (currentComputerConfiguration.runPressType != RUN_TYPE_MICROTUTOR || cpuMode_ != LOAD)
    {
        if (currentComputerConfiguration.autoBootConfiguration.dmaOutOnBoot)
            dmaOut();

        if (currentComputerConfiguration.autoBootConfiguration.dmaOutOnBootIfMem0is0 && readMemDebug(0) == 0)
            dmaOut();
    }
}

void Computer::onRunButtonRelease()
{
    runButtonState_ = 1;
    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
        panelPointer[frontPanel]->runSetState(BUTTON_UP);
}

void Computer::onRun()
{
    if (cycleFlashOn_)
    {
        p_Main->eventUpdateVipIILedStatus(BAR_LED_RUN, true);
        cycleFlashOn_ = false;
        return;
    }
    if (cycleKeyOn_ && cpuMode_ != RESET)
    {
        runReleased_ = true;
        return;
    }

    switch (currentComputerConfiguration.bootstrapConfiguration.type)
    {
        case BOOTSTRAPTIMER:
            addressLatchCounter_ = currentComputerConfiguration.bootstrapConfiguration.timer;
            bootstrap_ = currentComputerConfiguration.bootstrapConfiguration.address;
        break;

        case BOOTSTRAPTIMER_ONIN:
        break;

        default:
            if (currentComputerConfiguration.bootstrapConfiguration.defined)
                bootstrap_ = currentComputerConfiguration.bootstrapConfiguration.address;
        break;
    }

    stopTape();
    pseudoType_ = p_Main->getPseudoDefinition(&chip8baseVar_, &chip8mainLoop_, &chip8register12bit_, &pseudoLoaded_);

    switch (currentComputerConfiguration.runPressType)
    {
        case RUN_TYPE_SWITCH:
            if (runButtonState_)
            {
                for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
                    panelPointer[frontPanel]->runSetState(BUTTON_DOWN);
                runButtonState_ = 0;
                if (currentComputerConfiguration.i8275Configuration.defined)
                    i8275Pointer->cRegWrite(0x40);
            }
            else
            {
                for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
                    panelPointer[frontPanel]->runSetState(BUTTON_UP);
                runButtonState_ = 1;
                p_Main->startTime();
                resetComputer();
            }
            
            setClear(runButtonState_);
            p_Main->eventUpdateTitle();
            return;
        break;

        case RUN_TYPE_UC1800:
            if (singleStateStep_)
                return;
            
            if (cpuMode_ != RUN)
                resetEffectiveClock();

            if (cpuMode_ != LOAD)
            {
                setClear(1);
                setWait(1);
                p_Main->eventUpdateTitle();
            }
            return;
        break;
            
        case RUN_TYPE_MICROTUTOR:
            if (cpuMode_ != LOAD)
            {
                setClear(1);
                setWait(1);
                p_Main->eventUpdateTitle();
                p_Main->startTime();
            }
            else
                showData(dmaOut());
        break;
    }
    if (!currentComputerConfiguration.autoBootConfiguration.defined || !currentComputerConfiguration.f12reset)
    {
        if (getClear()==0)
        {
            setClear(1);
            setWait(1);
            p_Main->eventUpdateTitle();
            resetEffectiveClock();
        }
        else
        {
            setClear(0);
            setWait(1);
            segNumber_ = 0;
            if (currentComputerConfiguration.cdp1864Configuration.defined)
                cdp1864Pointer->setPixieGraphics(false);
            if (mpSuperButtonActive_)
                mpButtonState_ = 0;
            monitor_ = false;
            p_Main->eventUpdateTitle();
        }
    }
    else
        onReset();
    
    if (runPressedAtStartup_)
    {
        runPressedAtStartup_ = false;
        cycleKeyOn_ = false;
        cycleFlashOn_ = false;
        runReleased_ = false;
    }
    if (cpuMode_ == RESET)
        for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
            panelPointer[frontPanel]->showAddress(0);
}

void Computer::onPause(wxCommandEvent&WXUNUSED(event))
{
    onPause();
}

void Computer::onPause()
{
    if (currentComputerConfiguration.stepPressType == STEP_TYPE_COSMICOS)
    {
        singleStateStep_ = true;
        setClear(1);
        setWait(0);
        if (currentComputerConfiguration.cdp1864Configuration.defined)
            cdp1864Pointer->setPixieGraphics(false);
    }
    else
    {
        setWait(0);
        if (mpSuperButtonActive_)
            mpButtonState_ = 0;
    }
    p_Main->eventUpdateTitle();
}

void Computer::onMpButton(wxCommandEvent&WXUNUSED(event))
{
    onMpButton();
}

void Computer::onMpSuperButton(wxCommandEvent&WXUNUSED(event))
{
    onMpButton();
}

void Computer::onMpButtonMulti(wxCommandEvent&event)
{
    onMpButton(event.GetId()-0x50);
}

void Computer::onMpButton()
{
    if (mpButtonState_ == 1)
        mpButtonState_ = 0;
    else
        mpButtonState_ = 1;
}

void Computer::onMpSuperButton()
{
    mpButtonState_ = 1;
    mpSuperButtonActive_ = true;
}

void Computer::onMpButton(int buttonNumber)
{
    buttonNumber &= 0x3;
    mpButtonState[buttonNumber] = !mpButtonState[buttonNumber];
}

void Computer::onEmsButton(int buttonNumber, bool up)
{
    resetPressed_ = true;

    buttonNumber &= 0x1;
    for (std::vector<EmsMemoryConfiguration>::iterator emsConfig = currentComputerConfiguration.emsMemoryConfiguration.begin (); emsConfig != currentComputerConfiguration.emsMemoryConfiguration.end (); ++emsConfig)
    {
        if (emsConfig->output.portNumber[0] == -1 && emsConfig->outputStart == 0 && emsConfig->outputEnd == 0)
        {
            if (up)
                emsButton_[buttonNumber] = (emsButton_[buttonNumber] + 1) &0xf;
            else
                emsButton_[buttonNumber] = (emsButton_[buttonNumber] - 1) &0xf;

            if (buttonNumber == 0)
            {
                emsConfig->page &= 0xf0;
                emsConfig->page |= emsButton_[buttonNumber];
            }
            else
            {
                emsConfig->page &= 0xf;
                emsConfig->page |= (emsButton_[buttonNumber] << 4);
            }
        }
    }
    
    if (currentComputerConfiguration.multicartEmsNumber_ != -1)
        setMultiCartGame();
}

void Computer::setMultiCartGame()
{
    wxString game = "";
    Byte character;
    
    size_t address = 0xa00 | (currentComputerConfiguration.emsMemoryConfiguration[currentComputerConfiguration.multicartEmsNumber_].page << currentComputerConfiguration.emsMemoryConfiguration[currentComputerConfiguration.multicartEmsNumber_].maskBits);
    
    if (emsMemory_[currentComputerConfiguration.multicartEmsNumber_].mainMem[address] < 32)
    {
        game = p_Main->getMultiCartGame((currentComputerConfiguration.emsMemoryConfiguration[currentComputerConfiguration.multicartEmsNumber_].page & 0xf)>>4, currentComputerConfiguration.emsMemoryConfiguration[currentComputerConfiguration.multicartEmsNumber_].page & 0xf);
    }
    else
    {
        while (emsMemory_[currentComputerConfiguration.multicartEmsNumber_].mainMem[address] >= 32 && emsMemory_[currentComputerConfiguration.multicartEmsNumber_].mainMem[address] < 0x80)
        {
            character = emsMemory_[currentComputerConfiguration.multicartEmsNumber_].mainMem[address++];
            game.Append(character);
        }
    }
    
    p_Main->loadKeyDefinition("", game, keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_);

    if (game.Find('.'))
        game = game.BeforeFirst('.');

    if (currentComputerConfiguration.keyLatchConfiguration[1].defined)
        latchKeyPointer[1]->reDefineHexKeys(keyDefA1_, keyDefA2_, simDefA2_);
    if (currentComputerConfiguration.keyLatchConfiguration[2].defined)
        latchKeyPointer[2]->reDefineHexKeys(keyDefB1_, keyDefB2_, simDefB2_);

    p_Main->setSwName(game);
    p_Main->eventUpdateTitle();
}

void Computer::onEmsButton(wxCommandEvent&event)
{
    onEmsButton(event.GetId()-0x60, true);
}

void Computer::onRamButton(wxCommandEvent&WXUNUSED(event))
{
    onRamButton();
}

void Computer::onRamButton()
{
    nvRamDisable_ = !nvRamDisable_;
    p_Main->setNvRamDisable(nvRamDisable_);
}

void Computer::onMonitor(wxCommandEvent&WXUNUSED(event))
{
    onMonitor();
}

void Computer::onMonitor()
{
    if (currentComputerConfiguration.giantBoardConfiguration.defined)
    {
        setClear(1);
        setWait(1);
        scratchpadRegister_[0] = currentComputerConfiguration.giantBoardConfiguration.base;
    }
    else monitor_ = true;
}

void Computer::onNanoMonitor(wxCommandEvent&WXUNUSED(event))
{
    onNanoMonitor();
}

void Computer::onNanoMonitor()
{
    resetPressed_ = true;
    nanoMonitor_ = 40;
}

void Computer::onLoadButton(wxCommandEvent&WXUNUSED(event))
{
    onLoadButton(true);
}

void Computer::onLoadButton(bool pushButton)
{
    if (pushButton)
    {
        setClear(0);
        setWait(0);
        segNumber_ = 0;
    }
    else
    {
        if (!loadButtonState_)
        {
            loadButtonState_ = 1;
            bootstrap_ = 0;
        }
        else
            loadButtonState_ = 0;
        setWait(loadButtonState_);
    }
    if (currentComputerConfiguration.cdp1864Configuration.defined)
    {
        cdp1864Pointer->outPixie();
        cdp1864Pointer->setPixieGraphics(false);
    }
    if (currentComputerConfiguration.i8275Configuration.defined)
        i8275Pointer->cRegWrite(0x40);
    if (cpuMode_ == RESET)
        for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
            panelPointer[frontPanel]->showAddress(0);
}

void Computer::onSingleStep(wxCommandEvent&WXUNUSED(event))
{
    onSingleStep();
}

void Computer::onSingleStep()
{
    if (currentComputerConfiguration.stepPressType == STEP_TYPE_COSMICOS)
    {
        setWait(1);
    }
    else
    {
        singleStateStep_ = !singleStateStep_;
        if (singleStateStep_)
            setLedMsTemp(0);
        else
            setLedMs(ledTimeMs_);
    }
}

void Computer::onResetButton(wxCommandEvent&WXUNUSED(event))
{
    onResetButton();
}

void Computer::onResetButton()
{
    if (currentComputerConfiguration.resetPressType == RESET_TYPE_FULL)
        onReset();
    else
    {
        segNumber_ = 0;
        singleStateStep_ = false;
        setClear(0);
        setWait(1);

        if (currentComputerConfiguration.cdp1864Configuration.defined)
            cdp1864Pointer->setPixieGraphics(false);
        monitor_ = false;
        if (mpSuperButtonActive_)
            mpButtonState_ = 0;

        if (cpuMode_ == RESET)
            for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
                panelPointer[frontPanel]->showAddress(0);

        lastMode_ = UNDEFINDEDMODE;
    }
}
/* Not needed on OSX, maybe on Windows or linux for UC1800?
void Computer::onResetButtonPress()
{
    onResetButton();
    panelPointer->resetSetState(BUTTON_DOWN);
}

void Computer::onResetButtonRelease()
{
    panelPointer->resetSetState(BUTTON_UP);
}*/

void Computer::onReadButton(wxCommandEvent&WXUNUSED(event))
{
    onReadButton();
}

void Computer::onReadButton()
{
    readSwitchOn_ = !readSwitchOn_;
    tapeEnd_ = false;
    
    updateCardReadStatus();
}

void Computer::onCardButtonSwitch(wxCommandEvent&WXUNUSED(event))
{
    onCardButtonSwitch();
}

void Computer::onCardButtonSwitch()
{
    cardSwitchOn_ = !cardSwitchOn_;
    
    updateCardReadStatus();
}

void Computer::updateCardReadStatus()
{
    if (!readSwitchOn_)
    {
        if (inpMode_ == INP_MODE_TAPE_DIRECT)
        {
            inpMode_ = INP_MODE_NONE;
            tapeRunSwitch_ = 0;
            p_Computer->pauseTape();
            p_Main->turboOff();
        
            setClear(0);
            setWait(1);
        
            for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
            {
                panelPointer[frontPanel]->setReadyLed(1);
                panelPointer[frontPanel]->setStopLed(1);
            }
        }
        if (inpMode_ == INP_MODE_KEY_DIRECT)
            inpMode_ = INP_MODE_NONE;
    }
    
    if (readSwitchOn_ && !cardSwitchOn_)
    {
        if (inpMode_ == INP_MODE_NONE)
        {
            inpMode_ = INP_MODE_TAPE_DIRECT;
            startLoad(0, false);
        
            setClear(0);
            setWait(0);
        
            for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
            {
                panelPointer[frontPanel]->setReadyLed(1);
                panelPointer[frontPanel]->setStopLed(0);
            }
        }
        else
        {
            inpMode_ = INP_MODE_TAPE_DIRECT;
            p_Main->turboOn();
            p_Computer->restartTapeLoad(TAPE_PLAY);
        }
    }
    
    if (readSwitchOn_ && cardSwitchOn_)
    {
        if (inpMode_ == INP_MODE_TAPE_DIRECT)
        {
            tapeRunSwitch_ = 0;
            p_Computer->pauseTape();
            p_Main->turboOff();
        }
        
        inpMode_ = INP_MODE_KEY_DIRECT;
    }
}

void Computer::dataSwitch(int i)
{
    if (dataSwitchState_[i])
        dataSwitchState_[i] = 0;
    else
        dataSwitchState_[i] = 1;

    if (currentComputerConfiguration.ledDisplayConfiguration.followSwitches || currentComputerConfiguration.ledDisplayConfiguration.followButton)
        for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
            panelPointer[frontPanel]->setLed(i,dataSwitchState_[i]);
}

void Computer::efSwitch(int i)
{
    if (efSwitchState_[i])
    {
        efSwitchState_[i] = 0;
    }
    else
    {
        efSwitchState_[i] = 1;
    }
    setEf(i+1, 1-efSwitchState_[i]);
}

void Computer::onNumberKeyDown(wxCommandEvent&event)
{
    int id = event.GetId();
    onNumberKeyDown(id);
}

void Computer::onNumberKeyDown(int id)
{
    if (currentComputerConfiguration.mainFrontPanelConfiguration.nibbleLeds)
    {
        hexEfState_ = 0;
        switches_ = (nextNybble_ == 'H')?(switches_&15)+(id<<4):(switches_&0xf0)+id;
        if (nextNybble_ == 'H')
        {
            nextNybble_ = 'L';
            for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
            {
                panelPointer[frontPanel]->setNibbleLed(1, 0);
                panelPointer[frontPanel]->setNibbleLed(0, 1);
            }
        }
        else
        {
            nextNybble_ = 'H';
            for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
            {
                panelPointer[frontPanel]->setNibbleLed(1, 1);
                panelPointer[frontPanel]->setNibbleLed(0, 0);
            }
        }
    }
    else
    {
        hexEfState_ = 0;
        switches_ = ((switches_ << 4) & 0xf0) | id;
    }
}

void Computer::onNumberKeyUp(wxCommandEvent&WXUNUSED(event))
{
    hexEfState_ = 1;
}

void Computer::onNumberKeyUp()
{
    hexEfState_ = 1;
}

void Computer::onEfKeyUp(int ef)
{
    efKeyValue[ef] = currentComputerConfiguration.efButtonsConfiguration.keyPressed ^ 1;
}

void Computer::onEfKeyDown(int ef)
{
    efKeyValue[ef] = currentComputerConfiguration.efButtonsConfiguration.keyPressed;
}

void Computer::onHexKeyDown(int keycode)
{
#if defined (__WXMAC__)
    if (hexEfState_ == 0) // This is to avoid multiple key presses on OSX
        return;
#endif
    
    if (currentComputerConfiguration.fredKeypadConfiguration.defined)
    {
        fredkeypadPointer->keyDown(keycode);
        return;
    }
    

    for (int i=0; i<16; i++)
    {
        if (keycode == keyDefA1_[i])
        {
            if (currentComputerConfiguration.keyLatchConfiguration[1].defined)
                latchKeyPointer[1]->keyDown(i);
            if (currentComputerConfiguration.mainFrontPanelConfiguration.nibbleLeds)
            {
                hexEfState_ = 0;
                switches_ = (nextNybble_ == 'H')?(switches_&15)+(i<<4):(switches_&0xf0)+i;
                if (nextNybble_ == 'H')
                {
                    nextNybble_ = 'L';
                    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
                    {
                        panelPointer[frontPanel]->setNibbleLed(1, 0);
                        panelPointer[frontPanel]->setNibbleLed(0, 1);
                    }
                }
                else
                {
                    nextNybble_ = 'H';
                    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
                    {
                        panelPointer[frontPanel]->setNibbleLed(1, 1);
                        panelPointer[frontPanel]->setNibbleLed(0, 0);
                    }
                }
            }
            else
            {
                hexEfState_ = 0;
                switches_ = ((switches_ << 4) & 0xf0) | i;
            }
        }
        if (keycode == keyDefA2_[i])
        {
            if (currentComputerConfiguration.keyLatchConfiguration[1].defined)
                latchKeyPointer[1]->keyDown(i);
            if (currentComputerConfiguration.mainFrontPanelConfiguration.nibbleLeds)
            {
                hexEfState_ = 0;
                switches_ = (nextNybble_ == 'H')?(switches_&15)+(i<<4):(switches_&0xf0)+i;
                if (nextNybble_ == 'H')
                {
                    nextNybble_ = 'L';
                    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
                    {
                        panelPointer[frontPanel]->setNibbleLed(1, 0);
                        panelPointer[frontPanel]->setNibbleLed(0, 1);
                    }
                }
                else
                {
                    nextNybble_ = 'H';
                    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
                    {
                        panelPointer[frontPanel]->setNibbleLed(1, 1);
                        panelPointer[frontPanel]->setNibbleLed(0, 0);
                    }
                }
            }
            else
            {
                hexEfState_ = 0;
                switches_ = ((switches_ << 4) & 0xf0) | i;
            }
        }
        if (keycode == keyDefB1_[i])
        {
            if (currentComputerConfiguration.keyLatchConfiguration[2].defined)
                latchKeyPointer[2]->keyDown(i);
        }
        if (keycode == keyDefB2_[i])
        {
            if (currentComputerConfiguration.keyLatchConfiguration[2].defined)
                latchKeyPointer[2]->keyDown(i);
        }
    }
}

void Computer::onHexDown(int hex)
{
    hexEfState_ = 0;
    switches_ = ((switches_ << 4) & 0xf0) | hex;
}

void Computer::onHexKeyUp(int keycode)
{
    if (currentComputerConfiguration.fredKeypadConfiguration.defined)
    {
        fredkeypadPointer->keyUp(keycode);
        return;
    }

    if (currentComputerConfiguration.keyLatchConfiguration[1].defined)
    {
        for (int i=0; i<16; i++)
        {
            if (keycode == keyDefA1_[i] || keycode == keyDefA2_[i])
                latchKeyPointer[1]->keyUp(i);
        }
    }
    if (currentComputerConfiguration.keyLatchConfiguration[2].defined)
    {
        for (int i=0; i<16; i++)
        {
            if (keycode == keyDefB1_[i] || keycode == keyDefB2_[i])
                latchKeyPointer[2]->keyUp(i);
        }
    }

    if (currentComputerConfiguration.useHexKeyboard)
    {
        for (int i=0; i<5; i++)
        {
            if (keyDefGameHexA_[i] <= 15)
            {
                if (keycode == keyDefA2_[keyDefGameHexA_[i]])
                    keyboardUp();
            }
        }
    }

    hexEfState_ = 1;
}

void Computer::onClearButton(wxCommandEvent&WXUNUSED(event))
{
    onClearButton();
}

void Computer::onClearButton()
{
    if (currentComputerConfiguration.ledDisplayConfiguration.followButton)
    {
        for (int i=0; i<8; i++)
        {
            dataSwitchState_[i] = 0;
            for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
                panelPointer[frontPanel]->setLed(i,0);
        }
    }
}
      
void Computer::onClearResetButtonPress()
{
    setClear(0);
    clearResetPressed_ = true;

    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
        panelPointer[frontPanel]->clearResetSetState(BUTTON_DOWN);
    p_Main->eventUpdateTitle();
}

void Computer::onClearButtonRelease()
{
    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
        panelPointer[frontPanel]->clearResetSetState(BUTTON_UP);
}

void Computer::onClearSwitch()
{
    if (inbuttonEfState_ == 0 && runButtonState_ == 0 && currentComputerConfiguration.bootstrapConfiguration.type == BOOTSTRAPTIMER_ONIN)
    {
        addressLatchCounter_ = currentComputerConfiguration.bootstrapConfiguration.timer;
        bootstrap_ = currentComputerConfiguration.bootstrapConfiguration.address;
    }
    if (runButtonState_)
        runButtonState_ = 0;
    else
        runButtonState_ = 1;

    setClear(runButtonState_);
    p_Main->eventUpdateTitle();
}

void Computer::startComputer()
{
    for (std::vector<AssemblerConfiguration>::iterator assemblerInfo = currentComputerConfiguration.assemblerConfiguration.begin (); assemblerInfo != currentComputerConfiguration.assemblerConfiguration.end (); ++assemblerInfo)
    {
        p_Main->assDefault(assemblerInfo->directoryName, assemblerInfo->fileName, assemblerInfo->code_start, assemblerInfo->code_end, assemblerInfo->end);
    }
    configureMemory();

    resetPressed_ = false;
    clearResetPressed_ = false;
    fAndMBasicRunning_ = false;

    p_Main->enableDebugGuiMemory();

    configureTapeExtensions(); // tape extensions need to be defined first so tape EF can be overwritten by other EF functions
    configurePrinterExtensions(); // printer extensions need to be defined second so printer EF can be overwritten by other EF functions
    configureExtensions();
    configureVideoExtensions();
    configureV1870Extension();
    configureKeyboardExtensions();
    configureDiskExtensions();
    configureSoundExtensions();
    
    if (currentComputerConfiguration.asciiKeyboardConfiguration.defined)
        startElfKeyFile("Xml");
    if (currentComputerConfiguration.cdp1871Configuration.defined)
        start1871KeyFile();
    for (int pad=0; pad<lastLatchKeyPad_; pad++)
    {
        if (currentComputerConfiguration.keyLatchConfiguration[pad].defined)
            latchKeyPointer[pad]->startKeyFile();
    }
    if (currentComputerConfiguration.matrixKeyboardConfiguration.defined)
        matrixKeyboardPointer->startKeyFile();
    if (currentComputerConfiguration.gpioPs2KeyboardConfiguration.defined)
        startPs2gpioKeyFile();

    if (currentComputerConfiguration.videoTerminalConfiguration.type != VTNONE)
        setEf(currentComputerConfiguration.videoTerminalConfiguration.ef.flagNumber, 0);

    address_ = 0;

    cpuCycles_ = 0;
    instructionCounter_= 0;
    p_Main->startTime();

    day_ = xmlComputerTime_.GetDay();
    month_ = xmlComputerTime_.GetMonth();
    year_ = xmlComputerTime_.GetYear();

    loadRtc();
    rtcCycle_ = 4;
    rtcTimerPointer->Start(250, wxTIMER_CONTINUOUS);

    ledTimeMs_ = currentComputerConfiguration.ledTimeMs_;

    qLedStatus_ = 0;
    if (currentComputerConfiguration.videoTerminalConfiguration.type != VTNONE)
        qLedStatus_ = (1 ^ vtPointer->ef()) << 1;
    if (currentComputerConfiguration.videoTerminalConfiguration.external || currentComputerConfiguration.videoTerminalConfiguration.loop_back)
        qLedStatus_ = (1 ^ p_Serial->ef()) << 1;
    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
    {
        panelPointer[frontPanel]->setLedMs(ledTimeMs_);
        panelPointer[frontPanel]->setErrorLed(0);
        panelPointer[frontPanel]->setQLed(qLedStatus_);
        panelPointer[frontPanel]->showAddress(address_);
    }

    if (currentComputerConfiguration.autoBootConfiguration.defined)
    {
        scratchpadRegister_[0] = currentComputerConfiguration.autoBootConfiguration.address;
        autoBoot();
    }
    else
    {
        if (currentComputerConfiguration.giantBoardConfiguration.defined)
        {
            scratchpadRegister_[0] = currentComputerConfiguration.giantBoardConfiguration.base;
            autoBoot();
        }
    }

    if (currentComputerConfiguration.multicartEmsNumber_ != -1)
        setMultiCartGame();
    else
    {
        p_Main->setSwName("");
        p_Main->updateTitle();
    }

    if (ledTimeMs_ == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((computerClockSpeed_ * 1000000) / 8) / 1000) * ledTimeMs_;
    ledCycleValue_ = ledCycleSize_;
    
    for (int num=0; num<numberOfCdp1851Frames_; num++)
    {
        cdp1851FramePointer[num]->setLedMs(ledTimeMs_);
        cdp1851FramePointer[num]->Show(currentComputerConfiguration.cdp1851Configuration[num].windowOpen);
    }
    for (int num=0; num<numberOfCdp1852Frames_; num++)
    {
        cdp1852FramePointer[num]->setLedMs(ledTimeMs_);
        cdp1852FramePointer[num]->Show(currentComputerConfiguration.cdp1852Configuration[num].windowOpen);
    }
    goCycleSize_ = (((computerClockSpeed_ * 1000000) / 8) / 1000) * 500;
    goCycleValue_ = -1;

    pseudoType_ = p_Main->getPseudoDefinition(&chip8baseVar_, &chip8mainLoop_, &chip8register12bit_, &pseudoLoaded_);

    setForceUpperCase(currentComputerConfiguration.forceUpperCase);

    vipMode_ = false;
    switch (currentComputerConfiguration.bootstrapConfiguration.type)
    {
        case BOOTSTRAPRUN:
            runPressedAtStartup_ = p_Main->runPressed();
            if (runPressedAtStartup_)
            {
                bootstrap_ = currentComputerConfiguration.bootstrapConfiguration.address;
                slotOut(2);
                vipMode_ = true;
            }
            else
            {
                bootstrap_ = 0;
                slotOut(1);
            }
        break;

        case BOOTSTRAPTIMER:
            addressLatchCounter_ = currentComputerConfiguration.bootstrapConfiguration.timer;
            bootstrap_ = currentComputerConfiguration.bootstrapConfiguration.address;
        break;

        case BOOTSTRAPTIMER_ONIN:
        break;
    
        default:
            if (currentComputerConfiguration.bootstrapConfiguration.defined)
                bootstrap_ = currentComputerConfiguration.bootstrapConfiguration.address;
        break;
    }

    switch (currentComputerConfiguration.vis1870Configuration.statusBarType)
    {
        case STATUSBAR_COMX:
        case STATUSBAR_DIAG:
            if (currentComputerConfiguration.vis1870Configuration.defined)
            {
                vis1870Pointer->reDrawBar();
                vis1870Pointer->updateComxExpansionLed(true);
            }
        break;
    }
    
    if (currentComputerConfiguration.cdp1861Configuration.statusBarType == STATUSBAR_VIP2)
    {
        lastMode_ = cpuMode_;
        if (currentComputerConfiguration.cdp1861Configuration.defined)
            pixiePointer->reDrawBar();
        if (currentComputerConfiguration.cdp1864Configuration.defined)
            cdp1864Pointer->reDrawBar();
    }

    if (p_Main->getClearRamState()  && currentComputerConfiguration.nvRamConfiguration.defined)
    {
        for (size_t i=0; i<nvramDetails.size(); i++)
        {
            for (long address = nvramDetails[i].start; address <= nvramDetails[i].end; address++)
                mainMemory_[address] = 0;
        }
        p_Main->resetClearRamState();
    }
    if (p_Main->getClearRtcState() && currentComputerConfiguration.rtcDs12887Configuration.defined)
    {
        for (int i = 0; i<128; i++)
        {
            rtcRam_[i] = 0;
            p_Main->updateDebugMemory(i);
        }
        rtcRam_[0xa] = 0x20;
        rtcRam_[0xb] = 0x6;
        rtcRam_[0xc] = 0;
        rtcRam_[0xd] = 0x80;
        currentComputerConfiguration.clearRtc = false;
        p_Main->resetClearRtcState();
    }

    if (currentComputerConfiguration.useSplashScreen_)
    {
        if (p_Video[VIDEOMAIN] != NULL)
            p_Video[VIDEOMAIN]->splashScreen();
        else
        {
            if (p_Vt100[UART1] != NULL)
                p_Vt100[UART1]->splashScreen();
        }
    }
    threadPointer->Run();
    
    if (!powerButtonState_)
    {
        for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
            panelPointer[frontPanel]->powerSetState(BUTTON_DOWN);
        powerOff();
    }

    if (currentComputerConfiguration.hwTapeConfiguration.startOnReset)
    {
        tapeActivated_ =  p_Main->startCassetteLoad(0);
        if (tapeActivated_)
            tapeRunSwitch_ = tapeRunSwitch_ | 1;
    }

    setCycleType(CYCLE_TYPE_COMPUTER, LED_CYCLE);
}

void Computer::loadRomRam(size_t configNumber)
{
    if (currentComputerConfiguration.memoryConfiguration[configNumber].filename != "")
    {
        if (currentComputerConfiguration.memoryConfiguration[configNumber].verifyFileExist)
            p_Main->checkAndReInstallFile(currentComputerConfiguration.memoryConfiguration[configNumber].dirname + currentComputerConfiguration.memoryConfiguration[configNumber].filename, currentComputerConfiguration.memoryConfiguration[configNumber].filename);

        if (currentComputerConfiguration.memoryConfiguration[configNumber].filename.Right(4) == ".st2")
            readSt2Program(currentComputerConfiguration.memoryConfiguration[configNumber].dirname, currentComputerConfiguration.memoryConfiguration[configNumber].filename, NOCHANGE);
        else
            readProgram(currentComputerConfiguration.memoryConfiguration[configNumber].dirname, currentComputerConfiguration.memoryConfiguration[configNumber].filename, NOCHANGE,  currentComputerConfiguration.memoryConfiguration[configNumber].start, currentComputerConfiguration.memoryConfiguration[configNumber].loadOffSet, NONAME); 
        // type & 0xff causes loading ROM to end up without congif number in the higher 8 bit.
    }
    if (currentComputerConfiguration.memoryConfiguration[configNumber].filename2 != "")
    {
        if (currentComputerConfiguration.memoryConfiguration[configNumber].verifyFileExist)
            p_Main->checkAndReInstallFile(currentComputerConfiguration.memoryConfiguration[configNumber].dirname + currentComputerConfiguration.memoryConfiguration[configNumber].filename2, currentComputerConfiguration.memoryConfiguration[configNumber].filename2);

        readProgram(currentComputerConfiguration.memoryConfiguration[configNumber].dirname, currentComputerConfiguration.memoryConfiguration[configNumber].filename2, NOCHANGE,  currentComputerConfiguration.memoryConfiguration[configNumber].start, NONAME); 
        // type & 0xff causes loading ROM to end up without congif number in the higher 8 bit.
    }
}

void Computer::writeMemDataType(Word address, Byte type)
{
    size_t number = (memoryType_[address / 256] >> 8);
    address = (address | bootstrap_) & currentComputerConfiguration.memoryMask;

    int memNumber;
    switch (memoryType_[address/256]&0xff)
    {
        case EMSMEMORY:
            switch (emsMemory_[number].memoryType_[((address - currentComputerConfiguration.emsMemoryConfiguration[number].startAddress) |(currentComputerConfiguration.emsMemoryConfiguration[number].page << currentComputerConfiguration.emsMemoryConfiguration[number].maskBits))/256])
            {
                case ROM:
                case RAM:
                    if (emsMemory_[number].dataType_[(long) ((address - currentComputerConfiguration.emsMemoryConfiguration[number].startAddress) |(currentComputerConfiguration.emsMemoryConfiguration[number].page << currentComputerConfiguration.emsMemoryConfiguration[number].maskBits))] != type)
                    {
                        p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                        emsMemory_[number].dataType_[(long) ((address - currentComputerConfiguration.emsMemoryConfiguration[number].startAddress) |(currentComputerConfiguration.emsMemoryConfiguration[number].page << currentComputerConfiguration.emsMemoryConfiguration[number].maskBits))] = type;
                    }
                    increaseExecutedEms(number, (long) ((address - currentComputerConfiguration.emsMemoryConfiguration[number].startAddress) |(currentComputerConfiguration.emsMemoryConfiguration[number].page << currentComputerConfiguration.emsMemoryConfiguration[number].maskBits)), type);
                break;
            }
        break;
    
        case SLOTMEM:
            switch (slotMemoryType_[selectedSlot_][((selectedBank_&currentComputerConfiguration.slotConfiguration.slotInfo[selectedSlot_].bankOutputMaskInSlot)*slotSize_[selectedSlot_] + address-currentComputerConfiguration.slotConfiguration.start)/256])
            {
                case ROM:
                case RAM:
                    if (slotMemoryDataType_[selectedSlot_][(selectedBank_&currentComputerConfiguration.slotConfiguration.slotInfo[selectedSlot_].bankOutputMaskInSlot)*slotSize_[selectedSlot_] + address-currentComputerConfiguration.slotConfiguration.start] != type)
                    {
                        p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                        slotMemoryDataType_[selectedSlot_][(selectedBank_&currentComputerConfiguration.slotConfiguration.slotInfo[selectedSlot_].bankOutputMaskInSlot)*slotSize_[selectedSlot_] + address-currentComputerConfiguration.slotConfiguration.start] = type;
                    }
                    increaseExecutedSlotMemory(selectedSlot_, (selectedBank_&currentComputerConfiguration.slotConfiguration.slotInfo[selectedSlot_].bankOutputMaskInSlot)*slotSize_[selectedSlot_] + address-currentComputerConfiguration.slotConfiguration.start, type);
                break;
            }
        break;

        case MCRMEM:
            switch (mcrMemoryType_[selectedMap_][address/256])
            {
                case ROM:
                case ROM_MAP1:
                case ROM_MAP2:
                case ROM_MAP3:
                case ROM_MAP4:
                case ROM_MAP5:
                case ROM_MAP6:
                case ROM_MAP7:
                    memNumber = mcrMemoryType_[selectedMap_][address/256]-ROM;
                    if (mcrMemoryDataType_[memNumber][address &(mcrSize_[memNumber]-1)] != type)
                    {
                        p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                        mcrMemoryDataType_[memNumber][address &(mcrSize_[memNumber]-1)]  = type;
                    }
                    increaseExecutedMcrMemory(memNumber, address &(mcrSize_[memNumber]-1), type);
                break;
                    
                case RAM:
                case RAM_MAP1:
                case RAM_MAP2:
                case RAM_MAP3:
                case RAM_MAP4:
                case RAM_MAP5:
                case RAM_MAP6:
                case RAM_MAP7:
                    memNumber = mcrMemoryType_[selectedMap_][address/256]-RAM;
                    if (mcrMemoryDataType_[memNumber][address &(mcrSize_[memNumber]-1)] != type)
                    {
                        p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                        mcrMemoryDataType_[memNumber][address &(mcrSize_[memNumber]-1)]  = type;
                    }
                    increaseExecutedMcrMemory(memNumber, address &(mcrSize_[memNumber]-1), type);
                break;

                case NVRAM:
                case NVRAM_MAP1:
                case NVRAM_MAP2:
                case NVRAM_MAP3:
                case NVRAM_MAP4:
                case NVRAM_MAP5:
                case NVRAM_MAP6:
                case NVRAM_MAP7:
                    memNumber = mcrMemoryType_[selectedMap_][address/256]-NVRAM;
                    if (mcrMemoryDataType_[memNumber][address &(mcrSize_[memNumber]-1)] != type)
                    {
                        p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                        mcrMemoryDataType_[memNumber][address &(mcrSize_[memNumber]-1)]  = type;
                    }
                    increaseExecutedMcrMemory(memNumber, address &(mcrSize_[memNumber]-1), type);
                break;
            }
        break;

        case MAPPEDROM:
        case ROM:
        case DIAGROM:
            if (currentComputerConfiguration.giantBoardConfiguration.defined)
            {
                if (address >= currentComputerConfiguration.giantBoardConfiguration.base)
                    address = (address & 0xff) | currentComputerConfiguration.giantBoardConfiguration.base;
            }
            else
            {
                if (currentComputerConfiguration.memoryConfiguration[number].useMemMask)
                    address = (address & currentComputerConfiguration.memoryConfiguration[number].memMask) | (currentComputerConfiguration.memoryConfiguration[number].start&0xff00);
            }
            
            if (mainMemoryDataType_[address] != type)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryDataType_[address] = type;
            }
            increaseExecutedMainMemory(address, type);
        break;

        case TESTCARTRIDGEROM:
            if (testCartRomDataType_[address] != type)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                testCartRomDataType_[address] = type;
            }
            increaseExecutedTestCartRom(address, type);
        break;
            
        case MAPPEDRAM:
        case RAM:
        case NVRAM:
        case SN76430NRAM:
        case MC6847RAM:
            if (currentComputerConfiguration.memoryConfiguration[number].useMemMask)
            {
                address = (address & currentComputerConfiguration.memoryConfiguration[number].memMask) | (currentComputerConfiguration.memoryConfiguration[number].start&0xff00);
            }
            
            if (mainMemoryDataType_[address] != type)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryDataType_[address] = type;
            }
            increaseExecutedMainMemory(address, type);
        break;

        case PAGER:
            switch (pagerMemoryType_[((getPager(address>>currentComputerConfiguration.memoryMapperConfiguration.maskBits) << currentComputerConfiguration.memoryMapperConfiguration.maskBits) |(address &currentComputerConfiguration.memoryMapperConfiguration.mask))/256])
            {
                case ROM:
                case RAM:
                    if (pagerMemoryDataType_[(getPager(address>>currentComputerConfiguration.memoryMapperConfiguration.maskBits) << currentComputerConfiguration.memoryMapperConfiguration.maskBits) |(address &currentComputerConfiguration.memoryMapperConfiguration.mask)] != type)
                    {
                        p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                        pagerMemoryDataType_[(getPager(address>>currentComputerConfiguration.memoryMapperConfiguration.maskBits) << currentComputerConfiguration.memoryMapperConfiguration.maskBits) |(address &currentComputerConfiguration.memoryMapperConfiguration.mask)] = type;
                    }
                    increaseExecutedMainMemory((getPager(address>>currentComputerConfiguration.memoryMapperConfiguration.maskBits) << currentComputerConfiguration.memoryMapperConfiguration.maskBits) |(address &currentComputerConfiguration.memoryMapperConfiguration.mask), type);
                break;
            }
        break;

        case PRAM1870:
            if (currentComputerConfiguration.vis1870Configuration.pageMemIsRom)
            {
                if (mainMemoryDataType_[address] != type)
                {
                    p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                    mainMemoryDataType_[address] = type;
                }
            }
            increaseExecutedMainMemory(address, type);
        break;
    }
}

Byte Computer::readMemDataType(Word address, uint64_t* executed)
{
    address = (address | bootstrap_) & currentComputerConfiguration.memoryMask;
    size_t number = (memoryType_[address / 256] >> 8);

    for (std::vector<MemoryPartConfiguration>::iterator ramPartConfigIterator = currentComputerConfiguration.memoryRamPartConfiguration.begin (); ramPartConfigIterator != currentComputerConfiguration.memoryRamPartConfiguration.end (); ++ramPartConfigIterator)
    {
        if (address >= ramPartConfigIterator->start && address <= ramPartConfigIterator->end)
                return mainMemoryDataType_[address];
    }

    int memNumber;
    switch (memoryType_[address/256]&0xff)
    {
        case EMSMEMORY:
            switch (emsMemory_[number].memoryType_[((address - currentComputerConfiguration.emsMemoryConfiguration[number].startAddress) |(currentComputerConfiguration.emsMemoryConfiguration[number].page << currentComputerConfiguration.emsMemoryConfiguration[number].maskBits))/256])
            {
                case ROM:
                case RAM:
                    if (profilerCounter_ != PROFILER_OFF)
                        *executed = emsMemory_[number].executed_[(long) ((address - currentComputerConfiguration.emsMemoryConfiguration[number].startAddress) |(currentComputerConfiguration.emsMemoryConfiguration[number].page << currentComputerConfiguration.emsMemoryConfiguration[number].maskBits))];
                    return emsMemory_[number].dataType_[(long) ((address - currentComputerConfiguration.emsMemoryConfiguration[number].startAddress) |(currentComputerConfiguration.emsMemoryConfiguration[number].page << currentComputerConfiguration.emsMemoryConfiguration[number].maskBits))];
                break;
            }
        break;

        case SLOTMEM:
            switch (slotMemoryType_[selectedSlot_][((selectedBank_&currentComputerConfiguration.slotConfiguration.slotInfo[selectedSlot_].bankOutputMaskInSlot)*slotSize_[selectedSlot_] + address-currentComputerConfiguration.slotConfiguration.start)/256])
            {
                case ROM:
                case RAM:
                    if (profilerCounter_ != PROFILER_OFF)
                        *executed = slotMemoryExecuted_[selectedSlot_][(selectedBank_&currentComputerConfiguration.slotConfiguration.slotInfo[selectedSlot_].bankOutputMaskInSlot)*slotSize_[selectedSlot_] + address-currentComputerConfiguration.slotConfiguration.start];
                    return slotMemoryDataType_[selectedSlot_][(selectedBank_&currentComputerConfiguration.slotConfiguration.slotInfo[selectedSlot_].bankOutputMaskInSlot)*slotSize_[selectedSlot_] + address-currentComputerConfiguration.slotConfiguration.start];
                break;
            }
        break;

        case MCRMEM:
            switch (mcrMemoryType_[selectedMap_][address/256])
            {
                case ROM:
                case ROM_MAP1:
                case ROM_MAP2:
                case ROM_MAP3:
                case ROM_MAP4:
                case ROM_MAP5:
                case ROM_MAP6:
                case ROM_MAP7:
                    memNumber = mcrMemoryType_[selectedMap_][address/256]-ROM;
                    if (profilerCounter_ != PROFILER_OFF)
                        *executed = mcrMemoryExecuted_[memNumber][address &(mcrSize_[memNumber]-1)];
                    return mcrMemoryDataType_[memNumber][address &(mcrSize_[memNumber]-1)];
                break;
                    
                case RAM:
                case RAM_MAP1:
                case RAM_MAP2:
                case RAM_MAP3:
                case RAM_MAP4:
                case RAM_MAP5:
                case RAM_MAP6:
                case RAM_MAP7:
                    memNumber = mcrMemoryType_[selectedMap_][address/256]-RAM;
                    if (profilerCounter_ != PROFILER_OFF)
                        *executed = mcrMemoryExecuted_[memNumber][address &(mcrSize_[memNumber]-1)];
                    return mcrMemoryDataType_[memNumber][address &(mcrSize_[memNumber]-1)];
                break;

                case NVRAM:
                case NVRAM_MAP1:
                case NVRAM_MAP2:
                case NVRAM_MAP3:
                case NVRAM_MAP4:
                case NVRAM_MAP5:
                case NVRAM_MAP6:
                case NVRAM_MAP7:
                    memNumber = mcrMemoryType_[selectedMap_][address/256]-NVRAM;
                    if (profilerCounter_ != PROFILER_OFF)
                        *executed = mcrMemoryExecuted_[memNumber][address &(mcrSize_[memNumber]-1)];
                    return mcrMemoryDataType_[memNumber][address &(mcrSize_[memNumber]-1)];
                break;
            }
        break;
            
        case MAPPEDROM:
        case ROM:
        case DIAGROM:
            if (currentComputerConfiguration.giantBoardConfiguration.defined)
            {
                if (address >= currentComputerConfiguration.giantBoardConfiguration.base)
                    address = (address & 0xff) | currentComputerConfiguration.giantBoardConfiguration.base;
            }
            else
            {
                if (currentComputerConfiguration.memoryConfiguration[number].useMemMask)
                    address = (address & currentComputerConfiguration.memoryConfiguration[number].memMask) | (currentComputerConfiguration.memoryConfiguration[number].start&0xff00);
            }

            if (profilerCounter_ != PROFILER_OFF)
                *executed = mainMemoryExecuted_[address];
            return mainMemoryDataType_[address];
        break;

        case MAPPEDRAM:
        case RAM:
        case NVRAM:
        case SN76430NRAM:
        case MC6847RAM:
            if (currentComputerConfiguration.memoryConfiguration[number].useMemMask)
            {
                address = (address & currentComputerConfiguration.memoryConfiguration[number].memMask) | (currentComputerConfiguration.memoryConfiguration[number].start&0xff00);
            }

            if (profilerCounter_ != PROFILER_OFF)
                *executed = mainMemoryExecuted_[address];
            return mainMemoryDataType_[address];
        break;

        case TESTCARTRIDGEROM:
            if (profilerCounter_ != PROFILER_OFF)
                *executed = testCartRomExecuted_[address];
            return testCartRomDataType_[address];
        break;

        case PAGER:
            switch (pagerMemoryType_[((getPager(address>>currentComputerConfiguration.memoryMapperConfiguration.maskBits) << currentComputerConfiguration.memoryMapperConfiguration.maskBits) |(address &currentComputerConfiguration.memoryMapperConfiguration.mask))/256])
            {
                case ROM:
                case RAM:
                    if (profilerCounter_ != PROFILER_OFF)
                        *executed = pagerMemoryExecuted_[(getPager(address>>currentComputerConfiguration.memoryMapperConfiguration.maskBits) << currentComputerConfiguration.memoryMapperConfiguration.maskBits) |(address &currentComputerConfiguration.memoryMapperConfiguration.mask)];
                    return pagerMemoryDataType_[(getPager(address>>currentComputerConfiguration.memoryMapperConfiguration.maskBits) << currentComputerConfiguration.memoryMapperConfiguration.maskBits) |(address &currentComputerConfiguration.memoryMapperConfiguration.mask)];
                break;
            }
        break;

        case PRAM1870:
            if (currentComputerConfiguration.vis1870Configuration.pageMemIsRom)
            {
                if (profilerCounter_ != PROFILER_OFF)
                    *executed = mainMemoryExecuted_[address];
                return mainMemoryDataType_ [address];
            }
            else
                return MEM_TYPE_UNDEFINED;
        break;
    }
    return MEM_TYPE_UNDEFINED;
}

Byte Computer::readMem(Word address)
{
    address_ = address;

    if (address >= currentComputerConfiguration.bootstrapConfiguration.address && (currentComputerConfiguration.bootstrapConfiguration.type == BOOTSTRAPREAD || currentComputerConfiguration.bootstrapConfiguration.type == BOOTSTRAPRUN))
        bootstrap_ = 0;

    address = address | bootstrap_;

    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
        panelPointer[frontPanel]->showAddress(address_);

    return readMemDebug(address, READ_FUNCTION_NOT_DEBUG);
}

Byte Computer::readMemDebug(Word address, int function)
{
    address = address & currentComputerConfiguration.memoryMask;

    wxDateTime systemNow;
    wxDateTime now;
    wxTimeSpan timeDiff;
    Byte value;
    int year;
    Byte high, low, rtcControl;
    bool groupFound;

    Byte minimon[] = { 0xf8, 0xff, 0xa1, 0xe1, 0x6c, 0x64, 0xa3, 0x21,
        0x6c, 0x64, 0x3f, 0x07, 0x37, 0x0c, 0x3a, 0x11,
        0xd3, 0xe3, 0xf6, 0x33, 0x17, 0x7b, 0x6c, 0x64,
        0x23, 0x3f, 0x13, 0x37, 0x1b, 0x13, 0x30, 0x13 };
    
    for (std::vector<MemoryPartConfiguration>::iterator copyConfigIterator = currentComputerConfiguration.memoryCopyConfiguration.begin (); copyConfigIterator != currentComputerConfiguration.memoryCopyConfiguration.end (); ++copyConfigIterator)
    {
        if (address >= copyConfigIterator->start && address <= copyConfigIterator->end)
            if (copyConfigIterator->slot == selectedSlot_ || copyConfigIterator->slot == -1)
                return (readMemDebug(address + copyConfigIterator->copy));
    }
    
    for (std::vector<MemoryPartConfiguration>::iterator ramPartConfigIterator = currentComputerConfiguration.memoryRamPartConfiguration.begin (); ramPartConfigIterator != currentComputerConfiguration.memoryRamPartConfiguration.end (); ++ramPartConfigIterator)
    {
        if (address >= ramPartConfigIterator->start && address <= ramPartConfigIterator->end)
                return mainMemory_[address];
    }

    if (currentComputerConfiguration.mc6845Configuration.defined)
    {
        groupFound = false;
        
        if (currentComputerConfiguration.mc6845Configuration.ioGroupVector.size() == 0)
            groupFound = true;
        else
        {
            for (std::vector<int>::iterator ioGroupIterator = currentComputerConfiguration.mc6845Configuration.ioGroupVector.begin (); ioGroupIterator != currentComputerConfiguration.mc6845Configuration.ioGroupVector.end (); ++ioGroupIterator)
            {
                if (*ioGroupIterator == ioGroup_)
                    groupFound = true;
            }
        }
        if (groupFound)
        {
            if ((address&currentComputerConfiguration.mc6845Configuration.dataMask) == currentComputerConfiguration.mc6845Configuration.data)
                return mc6845Pointer->readDataDirect6845(address);
            
            if (address >=currentComputerConfiguration.mc6845Configuration.startRam && address <= currentComputerConfiguration.mc6845Configuration.endRam)
                return mc6845Pointer->read6845(address & currentComputerConfiguration.mc6845Configuration.ramMask);
        }
    }

    if (currentComputerConfiguration.matrixKeyboardConfiguration.defined)
    {
        if (currentComputerConfiguration.matrixKeyboardConfiguration.input.addressMode)
        {
            if (address == currentComputerConfiguration.matrixKeyboardConfiguration.input.portNumber[0])
                return matrixKeyboardPointer->in();
        }
    }

    if (currentComputerConfiguration.rtcCdp1879Configuration.defined)
    {
        if (ioGroupCdp1879(ioGroup_))
        {
            if (address == currentComputerConfiguration.rtcCdp1879Configuration.control)
            {
                mainMemory_[address] = readRtcStatusCdp1879();
                return mainMemory_[address];
            }
            if (address >= currentComputerConfiguration.rtcCdp1879Configuration.second && address <= currentComputerConfiguration.rtcCdp1879Configuration.month)
            {
                mainMemory_[address] = readRtcCdp1879(address, mainMemory_[address], systemTime_, xmlComputerTime_);
                return mainMemory_[address];
            }
        }
    }

    for (int instance=0; instance<numberOfCdp1877Instances_; instance++)
    {
        if (cdp1877InstancePointer[instance]->ioGroupCdp1877(ioGroup_))
        {
            int picValue = cdp1877InstancePointer[instance]->devRead(address, function);
            if (picValue != -1)
                return picValue&0xff;
        }
    }

    if (address == currentComputerConfiguration.mcrConfiguration.bbat.portNumber[0])
    {
        mainMemory_[address] = (mainMemory_[address] & (currentComputerConfiguration.mcrConfiguration.bbat.mask ^ 0xff)) | currentComputerConfiguration.mcrConfiguration.bbat.mask;
    }

    if (currentComputerConfiguration.rtcM48t58Configuration.defined)
    {
        groupFound = false;
        
        if (currentComputerConfiguration.rtcM48t58Configuration.ioGroupVector.size() == 0)
            groupFound = true;
        else
        {
            for (std::vector<int>::iterator ioGroupIterator = currentComputerConfiguration.rtcM48t58Configuration.ioGroupVector.begin (); ioGroupIterator != currentComputerConfiguration.rtcM48t58Configuration.ioGroupVector.end (); ++ioGroupIterator)
            {
                if (*ioGroupIterator == ioGroup_)
                    groupFound = true;
            }
        }
        if (groupFound)
        {
            if (address > currentComputerConfiguration.rtcM48t58Configuration.control && address <= currentComputerConfiguration.rtcM48t58Configuration.year)
            {
                if ((mainMemory_[currentComputerConfiguration.rtcM48t58Configuration.control]&0x40) != 0x40)
                {
                    rtcControl = 0;
                    systemNow = wxDateTime::Now();
                    timeDiff = systemNow.Subtract(systemTime_);

                    now = xmlComputerTime_;
                    now.Add(timeDiff);
                    
                    value = 0;
                    if (address == currentComputerConfiguration.rtcM48t58Configuration.second)
                    {
                        value = now.GetSecond();
                        rtcControl = mainMemory_[address] & 0x80;
                    }
                    if (address == currentComputerConfiguration.rtcM48t58Configuration.minute)
                    {
                        value = now.GetMinute();
                    }
                    if (address == currentComputerConfiguration.rtcM48t58Configuration.hour)
                    {
                        value = now.GetHour();
                    }
                    if (address == currentComputerConfiguration.rtcM48t58Configuration.day)
                    {
                        value = now.GetWeekDay();
                        rtcControl = mainMemory_[address] & 0x70;
                    }
                    if (address == currentComputerConfiguration.rtcM48t58Configuration.date)
                    {
                        value = now.GetDay();
                        rtcControl = mainMemory_[address] & 0xc0;
                    }
                    if (address == currentComputerConfiguration.rtcM48t58Configuration.month)
                    {
                        value = now.GetMonth()+1;
                    }
                    if (address == currentComputerConfiguration.rtcM48t58Configuration.year)
                    {
                        year = now.GetYear();
                        value = year % 100;
                    }

                    high = (int)(value/10)*16;
                    low = value - (int)(value/10)*10;
                    mainMemory_[address] = high + low + rtcControl;
                }
                return mainMemory_[address];
            }
        }
    }

    if (currentComputerConfiguration.adConvertorConfiguration.defined)
    {
        if (currentComputerConfiguration.adConvertorConfiguration.addressStart != -1 && currentComputerConfiguration.adConvertorConfiguration.addressEnd != -1)
        {
            if (address >= currentComputerConfiguration.adConvertorConfiguration.addressStart && address <= currentComputerConfiguration.adConvertorConfiguration.addressEnd)
                for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
                {
                    if (panelPointer[frontPanel]->isAdsDefined())
                        return panelPointer[frontPanel]->getAds(address & 0xf)/20;
                }
        }
    }

    size_t number = (memoryType_[address / 256] >> 8);
    
    int memNumber = 0;
    switch (memoryType_[address/256]&0xff)
    {
        case EMSMEMORY:
            switch (emsMemory_[number].memoryType_[((address - currentComputerConfiguration.emsMemoryConfiguration[number].startAddress) |(currentComputerConfiguration.emsMemoryConfiguration[number].page << currentComputerConfiguration.emsMemoryConfiguration[number].maskBits))/256])
            {
                case UNDEFINED:
                    return 255;
                break;
                    
                case ROM:
                case RAM:
                    return emsMemory_[number].mainMem[(long) ((address - currentComputerConfiguration.emsMemoryConfiguration[number].startAddress) |(currentComputerConfiguration.emsMemoryConfiguration[number].page << currentComputerConfiguration.emsMemoryConfiguration[number].maskBits))];
                break;
                    
                default:
                    return 255;
                break;
            }
        break;
            
        case SLOTMEM:
            switch (slotMemoryType_[selectedSlot_][((selectedBank_&currentComputerConfiguration.slotConfiguration.slotInfo[selectedSlot_].bankOutputMaskInSlot)*slotSize_[selectedSlot_] + address-currentComputerConfiguration.slotConfiguration.start)/256])
            {
                case ROM:
                case RAM:
                    return slotMemory_[selectedSlot_][(selectedBank_&currentComputerConfiguration.slotConfiguration.slotInfo[selectedSlot_].bankOutputMaskInSlot)*slotSize_[selectedSlot_] + address-currentComputerConfiguration.slotConfiguration.start];
                break;
                    
                default:
                    return 255;
                break;
            }
        break;

        case MCRMEM:
            if (address == currentComputerConfiguration.mcrConfiguration.output.portNumber[0])
                return mainMemory_[address];
            if (address >= currentComputerConfiguration.mcrConfiguration.ioStart && address <= currentComputerConfiguration.mcrConfiguration.ioEnd)
            {
                mainMemory_[address] = 0xff;
                return 0xff;
            }
            switch (mcrMemoryType_[selectedMap_][address/256])
            {
                case ROM:
                case ROM_MAP1:
                case ROM_MAP2:
                case ROM_MAP3:
                case ROM_MAP4:
                case ROM_MAP5:
                case ROM_MAP6:
                case ROM_MAP7:
                    memNumber = mcrMemoryType_[selectedMap_][address/256]-ROM;
                    return mcrMemory_[memNumber][address &(mcrSize_[memNumber]-1)];
                break;

                case RAM:
                case RAM_MAP1:
                case RAM_MAP2:
                case RAM_MAP3:
                case RAM_MAP4:
                case RAM_MAP5:
                case RAM_MAP6:
                case RAM_MAP7:
                    memNumber = mcrMemoryType_[selectedMap_][address/256]-RAM;
                    return mcrMemory_[memNumber][address &(mcrSize_[memNumber]-1)];
                break;

                case NVRAM:
                case NVRAM_MAP1:
                case NVRAM_MAP2:
                case NVRAM_MAP3:
                case NVRAM_MAP4:
                case NVRAM_MAP5:
                case NVRAM_MAP6:
                case NVRAM_MAP7:
                    memNumber = mcrMemoryType_[selectedMap_][address/256]-NVRAM;
                    return mcrMemory_[memNumber][address &(mcrSize_[memNumber]-1)];
                break;

                default:
                    return 255;
                break;
            }
        break;

        case UNDEFINED:
            return 255;
        break;

        case ROM:
        case MAPPEDROM:
            if (address <32 && monitor_) return minimon[address];
            
            if (currentComputerConfiguration.giantBoardConfiguration.defined)
            {
                if (address >= currentComputerConfiguration.giantBoardConfiguration.base)
                    address = (address & 0xff) | currentComputerConfiguration.giantBoardConfiguration.base;
            }
            else
            {
                if (currentComputerConfiguration.memoryConfiguration[number].useMemMask)
                    address = (address & currentComputerConfiguration.memoryConfiguration[number].memMask) | (currentComputerConfiguration.memoryConfiguration[number].start&0xff00);
            }
            
            return mainMemory_[address];
        break;

        case TESTCARTRIDGEROM:
            return testCartRom_[address];
        break;

        case DIAGROM:
            if (diagRomActive_)
                return diagRomReplacement_[address];
            else
                return mainMemory_[address];
        break;
            
        case MAPPEDRAM:
        case RAM:
        case NVRAM:
            if (address <32 && monitor_) return minimon[address];
            
            if (currentComputerConfiguration.memoryConfiguration[number].useMemMask)
            {
                address = (address & currentComputerConfiguration.memoryConfiguration[number].memMask) | (currentComputerConfiguration.memoryConfiguration[number].start&0xff00);
            }
            
            return mainMemory_[address];
        break;

        break;
            
        case COLOURRAM1864:
            if ((address & currentComputerConfiguration.cdp1864Configuration.ramMask) < 0x40)
                return colorMemory1864_[address&currentComputerConfiguration.cdp1864Configuration.ramMask];
            else
                return 255;
        break;

        case COLOURRAMST4:
            address = (address&0xf) +  ((address&0x3c0) >> 2);
            return colorMemory1864_[address] & 0xf;
        break;

        case SN76430NRAM:
            return mainMemory_[address];
        break;

        case MC6847RAM:
            return mc6847Pointer->read6847(address);
        break;

        case PRAM1870:
            groupFound = false;
            
            if (currentComputerConfiguration.vis1870Configuration.ioGroupVector.size() == 0)
                groupFound = true;
            else
            {
                for (std::vector<int>::iterator ioGroupIterator = currentComputerConfiguration.vis1870Configuration.ioGroupVector.begin (); ioGroupIterator != currentComputerConfiguration.vis1870Configuration.ioGroupVector.end (); ++ioGroupIterator)
                {
                    if (*ioGroupIterator == ioGroup_)
                        groupFound = true;
                }
            }
            if (groupFound)
                return vis1870Pointer->readPram(address);
            else
                return mainMemory_[address];
        break;
            
        case CRAM1870:
            groupFound = false;
            
            if (currentComputerConfiguration.vis1870Configuration.ioGroupVector.size() == 0)
                groupFound = true;
            else
            {
                for (std::vector<int>::iterator ioGroupIterator = currentComputerConfiguration.vis1870Configuration.ioGroupVector.begin (); ioGroupIterator != currentComputerConfiguration.vis1870Configuration.ioGroupVector.end (); ++ioGroupIterator)
                {
                    if (*ioGroupIterator == ioGroup_)
                        groupFound = true;
                }
            }
            if (groupFound)
                return vis1870Pointer->readCram(address);
            else
                return mainMemory_[address];
        break;
            
        case PAGER:
            switch (pagerMemoryType_[((getPager(address>>currentComputerConfiguration.memoryMapperConfiguration.maskBits) << currentComputerConfiguration.memoryMapperConfiguration.maskBits) |(address &currentComputerConfiguration.memoryMapperConfiguration.mask))/256])
            {
                case UNDEFINED:
                    return 255;
                break;

                case ROM:
                case RAM:
                    if (address <32 && monitor_) return minimon[address];
                    return pagerMemory_[(getPager(address>>currentComputerConfiguration.memoryMapperConfiguration.maskBits) << currentComputerConfiguration.memoryMapperConfiguration.maskBits) |(address &currentComputerConfiguration.memoryMapperConfiguration.mask)];
                break;

                default:
                    return 255;
                break;
            }
        break;
            
        default:
            return 255;
        break;
    }
}

void Computer::writeMem(Word address, Byte value, bool writeRom)
{
    address_ = address;

    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
        panelPointer[frontPanel]->showAddress(address_);
    writeMemDebug(address, value, writeRom);
}

void Computer::writeMemDebug(Word address, Byte value, bool writeRom)
{
    address = address & currentComputerConfiguration.memoryMask;

    wxDateTime systemNow;
    wxDateTime now;
    wxTimeSpan timeDiff;
    Byte high, low;
    wxString printBuffer;
    bool groupFound;

    address = address | bootstrap_;
    size_t number = (memoryType_[address / 256] >> 8);
    
    if (address > endSave_)
        endSave_ = address;

    for (std::vector<MemoryPartConfiguration>::iterator ramPartConfigIterator = currentComputerConfiguration.memoryRamPartConfiguration.begin (); ramPartConfigIterator != currentComputerConfiguration.memoryRamPartConfiguration.end (); ++ramPartConfigIterator)
    {
        if (address >= ramPartConfigIterator->start && address <= ramPartConfigIterator->end)
            mainMemory_[address]=value;
    }

    if (currentComputerConfiguration.rtcCdp1879Configuration.defined)
    {
        if (ioGroupCdp1879(ioGroup_))
        {
            if (address == currentComputerConfiguration.rtcCdp1879Configuration.freeze)
            {
                unfreezeTimeCdp1879();
                return;
            }
            if (address == currentComputerConfiguration.rtcCdp1879Configuration.control)
            {
                writeRtcControlCdp1879(value);
                return;
            }
            if (address >= currentComputerConfiguration.rtcCdp1879Configuration.second && address <= currentComputerConfiguration.rtcCdp1879Configuration.hour)
            {
                mainMemory_[address] = value;
                xmlComputerTime_ = writeRtcTimeCdp1879(address, value, systemTime_, xmlComputerTime_);
                return;
            }
            if (address >= currentComputerConfiguration.rtcCdp1879Configuration.date && address <= currentComputerConfiguration.rtcCdp1879Configuration.month)
            {
                mainMemory_[address] = value;
                xmlComputerTime_ = writeRtcDateCdp1879(address, value, systemTime_, xmlComputerTime_);
                return;
            }
        }
    }

    if (currentComputerConfiguration.rtcM48t58Configuration.defined)
    {
        groupFound = false;
        
        if (currentComputerConfiguration.rtcM48t58Configuration.ioGroupVector.size() == 0)
            groupFound = true;
        else
        {
            for (std::vector<int>::iterator ioGroupIterator = currentComputerConfiguration.rtcM48t58Configuration.ioGroupVector.begin (); ioGroupIterator != currentComputerConfiguration.rtcM48t58Configuration.ioGroupVector.end (); ++ioGroupIterator)
            {
                if (*ioGroupIterator == ioGroup_)
                    groupFound = true;
            }
        }
        if (groupFound)
        {
            if (address == currentComputerConfiguration.rtcM48t58Configuration.control)
            {
                if (value == 0)
                {
                    systemNow = wxDateTime::Now();
                    timeDiff = systemNow.Subtract(systemTime_);

                    now = xmlComputerTime_;
                    now.Add(timeDiff);

                    now.SetDay(1);
                    now.SetYear(year_);
                    now.SetMonth(wxDateTime::Month(month_));
                    now.SetDay(day_);

                    xmlComputerTime_ = now.Subtract(timeDiff);
                }
            }
            if (address > currentComputerConfiguration.rtcM48t58Configuration.control && address != currentComputerConfiguration.rtcM48t58Configuration.day && address <= currentComputerConfiguration.rtcM48t58Configuration.year)
            {
                if (nvramWriteProtected_ || (currentComputerConfiguration.nvRamMpConfiguration.followMpSwitch && mpButtonState_ == 1))
                    return;
                if ((mainMemory_[currentComputerConfiguration.rtcM48t58Configuration.control]&0x80) == 0x80)
                {
                    mainMemory_[address]=value;
                    
                    if (address == currentComputerConfiguration.rtcM48t58Configuration.second)
                        value &= 0x7f;
                    if (address == currentComputerConfiguration.rtcM48t58Configuration.minute)
                        value &= 0x7f;
                    if (address == currentComputerConfiguration.rtcM48t58Configuration.hour)
                        value &= 0x3f;
                    if (address == currentComputerConfiguration.rtcM48t58Configuration.date)
                        value &= 0x3f;
                    if (address == currentComputerConfiguration.rtcM48t58Configuration.month)
                        value &= 0x1f;

                    high = ((value&0xf0)/16)*10;
                    low = value&0xf;
                    value = high + low;

                    systemNow = wxDateTime::Now();
                    timeDiff = systemNow.Subtract(systemTime_);

                    now = xmlComputerTime_;
                    now.Add(timeDiff);

                    if (address == currentComputerConfiguration.rtcM48t58Configuration.second)
                        now.SetSecond(value);
                    if (address == currentComputerConfiguration.rtcM48t58Configuration.minute)
                        now.SetMinute(value);
                    if (address == currentComputerConfiguration.rtcM48t58Configuration.hour)
                        now.SetHour(value);
                    if (address == currentComputerConfiguration.rtcM48t58Configuration.date)
                        day_=value;
                    if (address == currentComputerConfiguration.rtcM48t58Configuration.month)
                        month_=value-1;
                    if (address == currentComputerConfiguration.rtcM48t58Configuration.year)
                        year_=value+2000;

                    xmlComputerTime_ = now.Subtract(timeDiff);
                    return;
                }
            }
        }
    }

    for (std::vector<EmsMemoryConfiguration>::iterator emsConfig = currentComputerConfiguration.emsMemoryConfiguration.begin (); emsConfig != currentComputerConfiguration.emsMemoryConfiguration.end (); ++emsConfig)
    {
        if (emsConfig->output.portNumber[0] == -1)
        {
            if (emsConfig->outputStart != 0 || emsConfig->outputEnd != 0)
            {
                if (address>=emsConfig->outputStart && address <=emsConfig->outputEnd)
                {
                    emsConfig->page = value & emsConfig->output.mask;
                }
            }
        }
    }
    
    for (int instance=0; instance<numberOfCdp1877Instances_; instance++)
    {
        if (cdp1877InstancePointer[instance]->ioGroupCdp1877(ioGroup_))
        {
            cdp1877InstancePointer[instance]->devWrite(address, value);
        }
    }

    if (currentComputerConfiguration.mc6847Configuration.outputMode == 1 && currentComputerConfiguration.mc6847Configuration.defined)
    {
        bool groupFound = false;
        
        if (currentComputerConfiguration.mc6847Configuration.ioGroupVector.size() == 0)
            groupFound = true;
        else
        {
            for (std::vector<int>::iterator ioGroupIterator = currentComputerConfiguration.mc6847Configuration.ioGroupVector.begin (); ioGroupIterator != currentComputerConfiguration.mc6847Configuration.ioGroupVector.end (); ++ioGroupIterator)
            {
                if (*ioGroupIterator == ioGroup_)
                    groupFound = true;
            }
        }
        if (groupFound)
        {
            if (address>=currentComputerConfiguration.mc6847Configuration.outputStart && address <=currentComputerConfiguration.mc6847Configuration.outputEnd)
            mc6847Pointer->outMc6847(value);
        }
    }

    if (currentComputerConfiguration.mc6845Configuration.defined)
    {
        bool groupFound = false;
        
        if (currentComputerConfiguration.mc6845Configuration.ioGroupVector.size() == 0)
            groupFound = true;
        else
        {
            for (std::vector<int>::iterator ioGroupIterator = currentComputerConfiguration.mc6845Configuration.ioGroupVector.begin (); ioGroupIterator != currentComputerConfiguration.mc6845Configuration.ioGroupVector.end (); ++ioGroupIterator)
            {
                if (*ioGroupIterator == ioGroup_)
                    groupFound = true;
            }
        }
        if (groupFound)
        {
            if ((address&currentComputerConfiguration.mc6845Configuration.dataMask) == currentComputerConfiguration.mc6845Configuration.data)
            {
                mc6845Pointer->writeData6845(value);
                return;
            }

            if ((address&currentComputerConfiguration.mc6845Configuration.addressMask) == currentComputerConfiguration.mc6845Configuration.address)
            {
                mc6845Pointer->writeRegister6845(value);
                return;
            }

            if (address >=currentComputerConfiguration.mc6845Configuration.startRam && address <= currentComputerConfiguration.mc6845Configuration.endRam)
            {
                mc6845Pointer->write6845(address & currentComputerConfiguration.mc6845Configuration.ramMask, value);
                return;
            }
        }
    }

    if (currentComputerConfiguration.matrixKeyboardConfiguration.output.addressMode)
    {
        if (address == 0xe001)
        {
        }

        if (address == currentComputerConfiguration.matrixKeyboardConfiguration.output.portNumber[0])
        {
            matrixKeyboardPointer->setRow(value);
            return;
        }
    }

    if (currentComputerConfiguration.cdp1864Configuration.colorLatch)
    {
        if (!writeRom)
        {
            if (colourLatch_ && (address >= currentComputerConfiguration.cdp1864Configuration.startRam) && (address <= currentComputerConfiguration.cdp1864Configuration.endRam))
            {
                colorMemory1864_[address&currentComputerConfiguration.cdp1864Configuration.ramMask] = value & currentComputerConfiguration.ioMask[CDP1864_COLORRAM_OUT];
                if ((address&currentComputerConfiguration.cdp1864Configuration.ramMask) >= memoryStart_ && (address&currentComputerConfiguration.cdp1864Configuration.ramMask) <(memoryStart_+256))
                    p_Main->updateDebugMemory(address&currentComputerConfiguration.cdp1864Configuration.ramMask);
                if (address >= memoryStart_ && address<(memoryStart_ + 256))
                    p_Main->updateDebugMemory(address);
                p_Main->updateAssTabCheck(address);
                return;
            }
        }
    }

    if (currentComputerConfiguration.soundConfiguration.type == SOUND_SUPER_VP551)
    {
        switch (address)
        {
            case 0x4001:
                frequencySuper(2, value);
            return;
            case 0x4002:
                frequencySuper(3, value);
            return;
            case 0x4003:
                if (value & 0x4)
                    octaveSuper(3, value);
                else
                    octaveSuper(2, value);
            return;
            case 0x4010:
                amplitudeSuper(2, value);
            return;
            case 0x4020:
                amplitudeSuper(3, value);
            return;
            case 0x4030:
                vp550IntOn_ = (value == 1);
            return;
        }
    }
    
    int memNumber = 0;
    switch (memoryType_[address/256]&0xff)
    {
        case EMSMEMORY:
            switch (emsMemory_[number].memoryType_[((address - currentComputerConfiguration.emsMemoryConfiguration[number].startAddress) |(currentComputerConfiguration.emsMemoryConfiguration[number].page << currentComputerConfiguration.emsMemoryConfiguration[number].maskBits))/256])
            {
                case UNDEFINED:
                case ROM:
                    if (writeRom)
                        emsMemory_[number].mainMem[(long) ((address - currentComputerConfiguration.emsMemoryConfiguration[number].startAddress) |(currentComputerConfiguration.emsMemoryConfiguration[number].page << currentComputerConfiguration.emsMemoryConfiguration[number].maskBits))] = value;
                break;
                    
                case RAM:
                    if (!getMpButtonState())
                    {
                        emsMemory_[number].mainMem[(long) ((address - currentComputerConfiguration.emsMemoryConfiguration[number].startAddress) |(currentComputerConfiguration.emsMemoryConfiguration[number].page << currentComputerConfiguration.emsMemoryConfiguration[number].maskBits))] = value;
                        if (address >= memoryStart_ && address<(memoryStart_ + 256))
                            p_Main->updateDebugMemory(address);
                        p_Main->updateAssTabCheck(address);
                    }
                break;
            }
        break;
       
        case SLOTMEM:
            switch (slotMemoryType_[selectedSlot_][((selectedBank_&currentComputerConfiguration.slotConfiguration.slotInfo[selectedSlot_].bankOutputMaskInSlot)*slotSize_[selectedSlot_] + address-currentComputerConfiguration.slotConfiguration.start)/256])
            {
                case ROM:
                    if (writeRom)
                        slotMemory_[selectedSlot_][(selectedBank_&currentComputerConfiguration.slotConfiguration.slotInfo[selectedSlot_].bankOutputMaskInSlot)*slotSize_[selectedSlot_] + address-currentComputerConfiguration.slotConfiguration.start] = value;
                break;

                case RAM:
                    slotMemory_[selectedSlot_][(selectedBank_&currentComputerConfiguration.slotConfiguration.slotInfo[selectedSlot_].bankOutputMaskInSlot)*slotSize_[selectedSlot_] + address-currentComputerConfiguration.slotConfiguration.start] = value;
                break;
            }
        break;

        case MCRMEM:
            if (address == currentComputerConfiguration.mcrConfiguration.output.portNumber[0])
            {
                selectedMap_ = value & 0x7;
                mainMemory_[address] = (mainMemory_[address] & (currentComputerConfiguration.mcrConfiguration.output.mask ^ 0xff)) | selectedMap_;
            }
            if (address >= currentComputerConfiguration.mcrConfiguration.ioStart && address <= currentComputerConfiguration.mcrConfiguration.ioEnd)
                return;
            switch (mcrMemoryType_[selectedMap_][address/256])
            {
                case ROM:
                case ROM_MAP1:
                case ROM_MAP2:
                case ROM_MAP3:
                case ROM_MAP4:
                case ROM_MAP5:
                case ROM_MAP6:
                case ROM_MAP7:
                    memNumber = mcrMemoryType_[selectedMap_][address/256]-ROM;
                    if (writeRom)
                        mcrMemory_[memNumber][address &(mcrSize_[memNumber]-1)] = value;
                break;

                case RAM:
                case RAM_MAP1:
                case RAM_MAP2:
                case RAM_MAP3:
                case RAM_MAP4:
                case RAM_MAP5:
                case RAM_MAP6:
                case RAM_MAP7:
                    memNumber = mcrMemoryType_[selectedMap_][address/256]-RAM;
                    mcrMemory_[memNumber][address &(mcrSize_[memNumber]-1)] = value;
                break;

                case NVRAM:
                case NVRAM_MAP1:
                case NVRAM_MAP2:
                case NVRAM_MAP3:
                case NVRAM_MAP4:
                case NVRAM_MAP5:
                case NVRAM_MAP6:
                case NVRAM_MAP7:
                    memNumber = mcrMemoryType_[selectedMap_][address/256]-NVRAM;
                    mcrMemory_[memNumber][address &(mcrSize_[memNumber]-1)] = value;
                break;
            }
        break;

        case MC6847RAM:
            mc6847Pointer->write(address, value);
            mainMemory_[address] = value;
            if (address >= memoryStart_ && address<(memoryStart_ + 256))
                p_Main->updateDebugMemory(address);
        break;

        case COLOURRAM:
            if ((address >= 0xc000) && (address < 0xd000))
                colourMask1862_ = 0xe7;
            else
                colourMask1862_ = 0xff;
            colorMemory1862_[address&colourMask1862_] = value & 0xf;
            if ((address&colourMask1862_) >= memoryStart_ && (address&colourMask1862_) <(memoryStart_+256))
                p_Main->updateDebugMemory(address&colourMask1862_);
            if (address >= memoryStart_ && address<(memoryStart_ + 256))
                p_Main->updateDebugMemory(address);
            p_Main->updateAssTabCheck(address);
            useColour(colourMask1862_);
        break;

        case COLOURRAM1864:
            if ((address & currentComputerConfiguration.cdp1864Configuration.ramMask) < 0x40)
            {
                colorMemory1864_[address & currentComputerConfiguration.cdp1864Configuration.ramMask] = value & currentComputerConfiguration.ioMask[CDP1864_COLORRAM_OUT];
                if ((address & currentComputerConfiguration.cdp1864Configuration.ramMask) >= memoryStart_ && (address & currentComputerConfiguration.cdp1864Configuration.ramMask) < (memoryStart_ + 256))
                    p_Main->updateDebugMemory(address & currentComputerConfiguration.cdp1864Configuration.ramMask);
                if (address >= memoryStart_ && address < (memoryStart_ + 256))
                    p_Main->updateDebugMemory(address);
                p_Main->updateAssTabCheck(address);
                useColour(7);
            }
        break;
            
        case COLOURRAMST4:
            address = (address&0xf) +  ((address&0x3c0) >> 2);
            colorMemory1864_[address] = value & 0xf;
            if ((address) >= memoryStart_ && (address) < (memoryStart_ + 256))
                p_Main->updateDebugMemory(address);
            if (address >= memoryStart_ && address < (memoryStart_ + 256))
                p_Main->updateDebugMemory(address);
            p_Main->updateAssTabCheck(address);
            useColour(7);
        break;

        case SN76430NRAM:
            mainMemory_[address] = value;
            sn76430nPointer->writeRam(address, value);
        break;

        case UNDEFINED:
            if (writeRom)
                mainMemory_[address]=value;
        break;

        case MAPPEDROM:
        case ROM:
            if (address < 32 && monitor_) return;

            if (currentComputerConfiguration.giantBoardConfiguration.defined)
            {
                if (address >= currentComputerConfiguration.giantBoardConfiguration.base)
                    address = (address & 0xff) | currentComputerConfiguration.giantBoardConfiguration.base;
            }
            else
            {
                if (currentComputerConfiguration.memoryConfiguration[number].useMemMask)
                    address = (address & currentComputerConfiguration.memoryConfiguration[number].memMask) | (currentComputerConfiguration.memoryConfiguration[number].start&0xff00);
            }

            if (writeRom)
                mainMemory_[address]=value;
            else
            {
                if (currentComputerConfiguration.soundConfiguration.type == SOUND_SUPER_VP550 || currentComputerConfiguration.soundConfiguration.type == SOUND_SUPER_VP551)
                {
                    switch (address)
                    {
                        case 0x8001:
                            frequencySuper(0, value);
                        return;
                        case 0x8002:
                            frequencySuper(1, value);
                        return;
                        case 0x8003:
                            if (value & 0x4)
                                octaveSuper(1, value);
                            else
                                octaveSuper(0, value);
                        return;
                        case 0x8010:
                            amplitudeSuper(0, value);
                        return;
                        case 0x8020:
                            amplitudeSuper(1, value);
                        return;
                        case 0x8030:
                            vp550IntOn_ = (value == 1);
                        return;
                    }
                }
            }
        break;

        case TESTCARTRIDGEROM:
            if (writeRom)
                testCartRom_[address] = value;
        break;

        case DIAGROM:
            if (writeRom)
            {
                if (diagRomActive_)
                    diagRomReplacement_[address]=value;
                else
                    mainMemory_[address]=value;
            }
        break;
            
        case NVRAM:
            if (nvramWriteProtected_ || (currentComputerConfiguration.nvRamMpConfiguration.followMpSwitch && mpButtonState_ == 1))
                return;

            if (currentComputerConfiguration.memoryConfiguration[number].useMemMask)
            {
                
                address = (address & currentComputerConfiguration.memoryConfiguration[number].memMask) | (currentComputerConfiguration.memoryConfiguration[number].start&0xff00);
            }

            if (mainMemory_[address]==value)
                return;
            mainMemory_[address]=value;
            if (currentComputerConfiguration.memoryConfiguration[number].useMemMask)
            {
                if (address >= (memoryStart_ & currentComputerConfiguration.memoryConfiguration[number].memMask) && address<((memoryStart_ & currentComputerConfiguration.memoryConfiguration[number].memMask) | 256))
                    p_Main->updateDebugMemory(address);
            }
            else
            {
                if (address >= memoryStart_ && address<(memoryStart_ + 256))
                    p_Main->updateDebugMemory(address);
            }
            p_Main->updateAssTabCheck(address);
        break;

        case MAPPEDRAM:
        case RAM:
            if (!getMpButtonState() && !mpButtonState[(address>>10)&0x3])
            {
                if (address < 32 && monitor_) return;
                
                if (currentComputerConfiguration.memoryConfiguration[number].useMemMask)
                {
                    
                    address = (address & currentComputerConfiguration.memoryConfiguration[number].memMask) | (currentComputerConfiguration.memoryConfiguration[number].start&0xff00);
                }
                
                if (mainMemory_[address]==value)
                    return;
                mainMemory_[address]=value;
                if (currentComputerConfiguration.memoryConfiguration[number].useMemMask)
                {
                    if (address >= (memoryStart_ & currentComputerConfiguration.memoryConfiguration[number].memMask) && address<((memoryStart_ & currentComputerConfiguration.memoryConfiguration[number].memMask) | 256))
                        p_Main->updateDebugMemory(address);
                }
                else
                {
                    if (address >= memoryStart_ && address<(memoryStart_ + 256))
                        p_Main->updateDebugMemory(address);
                }
                p_Main->updateAssTabCheck(address);
            }
            
            for (std::vector<WriteAddress>::iterator i = currentComputerConfiguration.addressLocationConfiguration.writeAddress.begin (); i != currentComputerConfiguration.addressLocationConfiguration.writeAddress.end (); ++i)
            {
                if (address == i->address && value == i->value)
                {
                    switch (i->function)
                    {
                        case WRITE_ADDRESS_DEBUG:
                            printBuffer.Printf("Exec address: %04X, write address: %04X, value: %02X", scratchpadRegister_[programCounter_], i->address, i->value);
                            p_Main->eventShowTextMessage(printBuffer);
                        break;
                    }
                }
            }

            switch (address)
            {
                case 0x428b:
                    switch(value)
                    {
                        case 0x14:
//                            stop6845();
//                            if (fAndMBasicRunning_ && comxKeyFileOpened_)
//                                keyboardEf3_ = 0;
                            if (currentComputerConfiguration.parallelPrinterConfiguration.defined)
                            {
                                p_Main->eventPrintParallelFinished();
                            }
                            if (currentComputerConfiguration.serialPrinterConfiguration.defined)
                            {
                                p_Main->eventPrintSerialFinished();
                            }
         //                   if (currentComputerConfiguration.thermalPrinterConfiguration.defined)
         //                   {
         //                       p_Main->eventPrintThermalFinished();
         //                   }
                        break;
                    }
                break;

/*                case 0x41a3:
                    if (value == 0 && comxKeyFileOpened_ && !fAndMBasicRunning_ && comxRunState_ != LISTSTATE)
                    {
                        keyboardEf3_ = 0;
                    }
                break;*/
            }
        break;

        case PAGER:
            switch (pagerMemoryType_[((getPager(address>>currentComputerConfiguration.memoryMapperConfiguration.maskBits) << currentComputerConfiguration.memoryMapperConfiguration.maskBits) |(address &currentComputerConfiguration.memoryMapperConfiguration.mask))/256])
            {
                case UNDEFINED:
                case ROM:
                    if (address < 32 && monitor_) return;
                    if (writeRom)
                        pagerMemory_[(getPager(address>>currentComputerConfiguration.memoryMapperConfiguration.maskBits) << currentComputerConfiguration.memoryMapperConfiguration.maskBits) |(address &currentComputerConfiguration.memoryMapperConfiguration.mask)] = value;
                break;

                case RAM:
                    if (address < 32 && monitor_) return;
                    if (!getMpButtonState())
                        pagerMemory_[(getPager(address>>currentComputerConfiguration.memoryMapperConfiguration.maskBits) << currentComputerConfiguration.memoryMapperConfiguration.maskBits) |(address &currentComputerConfiguration.memoryMapperConfiguration.mask)] = value;
                    if (address >= memoryStart_ && address<(memoryStart_ + 256))
                        p_Main->updateDebugMemory(address);
                    p_Main->updateAssTabCheck(address);
                break;
            }
        break;

        case PRAM1870:
            if (writeRom && currentComputerConfiguration.vis1870Configuration.pageMemIsRom)
                mainMemory_[address]=value;
            else
            {
                groupFound = false;
                
                if (currentComputerConfiguration.vis1870Configuration.ioGroupVector.size() == 0)
                    groupFound = true;
                else
                {
                    for (std::vector<int>::iterator ioGroupIterator = currentComputerConfiguration.vis1870Configuration.ioGroupVector.begin (); ioGroupIterator != currentComputerConfiguration.vis1870Configuration.ioGroupVector.end (); ++ioGroupIterator)
                    {
                        if (*ioGroupIterator == ioGroup_)
                            groupFound = true;
                    }
                }
                if (groupFound || currentComputerConfiguration.vis1870Configuration.pageMemExcludeIoGroup)
                {
                    address = vis1870Pointer->writePram(address, value);
                    
                    if (address>= memoryStart_ && address<(memoryStart_+256))
                        p_Main->updateDebugMemory(address);
                }
                else
                {
                    if (ramGroupAtV1870_)
                    {
                        if (mainMemory_[address]==value)
                            return;
                        mainMemory_[address]=value;
                        if (address>= memoryStart_ && address<(memoryStart_+256))
                            p_Main->updateDebugMemory(address);
                        p_Main->updateAssTabCheck(address);
                    }
                }
            }
        break;
            
        case CRAM1870:
            groupFound = false;
            
            if (currentComputerConfiguration.vis1870Configuration.ioGroupVector.size() == 0)
                groupFound = true;
            else
            {
                for (std::vector<int>::iterator ioGroupIterator = currentComputerConfiguration.vis1870Configuration.ioGroupVector.begin (); ioGroupIterator != currentComputerConfiguration.vis1870Configuration.ioGroupVector.end (); ++ioGroupIterator)
                {
                    if (*ioGroupIterator == ioGroup_)
                        groupFound = true;
                }
            }
            if (groupFound || currentComputerConfiguration.vis1870Configuration.charMemExcludeIoGroup)
            {
                address = vis1870Pointer->writeCram(address, value);

            //    if (address>= memoryStart_ && address<(memoryStart_+256))
            //        p_Main->updateDebugMemory(address);
            }
            else
            {
                if (ramGroupAtV1870_)
                {
                    if (mainMemory_[address]==value)
                        return;
                    mainMemory_[address]=value;
                    if (address>= memoryStart_ && address<(memoryStart_+256))
                        p_Main->updateDebugMemory(address);
                    p_Main->updateAssTabCheck(address);
                }
            }
        break;
    }
}

void Computer::cpuInstruction()
{
    if (clearResetPressed_)
    {
        resetCpu();
        clearResetPressed_ = false;
    }
    if (cpuMode_ != lastMode_)
    {
        for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
        {
            for (int led=LOADLED; led<=RUNLED; led++)
                panelPointer[frontPanel]->setStateLed(led, 0);
            panelPointer[frontPanel]->setStateLed(cpuMode_, 1);
        }
        if (cpuMode_ == RUN)
            p_Main->eventUpdateVipIILedStatus(BAR_LED_RUN, true);
        else
            p_Main->eventUpdateVipIILedStatus(BAR_LED_RUN, false);
        lastMode_ = cpuMode_;
    }
    if (cpuMode_ == RUN)
    {
        if (tapeRecording_ && zeroWaveCounter_ >= 0)
        {
            zeroWaveCounter_--;
            if (zeroWaveCounter_ == -1)
                psaveAmplitudeZero();
        }
        if (addressLatchCounter_ > 0)
        {
            addressLatchCounter_--;
            if (addressLatchCounter_ == 0)
                bootstrap_ = 0;
        }
        cpuCycleStep();
        if (runPressed_)
        {
            setClear(clear_^1);
            p_Main->eventUpdateTitle();
            runPressed_ = false;
        }
        for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
        {
            if (idle_ || cpuMode_ != RUN)
                panelPointer[frontPanel]->setStateLed(RUNLED, 0);
            else
                panelPointer[frontPanel]->setStateLed(RUNLED, 1);
        }
    }
    else
    {
        if (runPressed_)
        {
            setClear(clear_^1);
            p_Main->eventUpdateTitle();
            if (clear_ == 1)
            {
                resetEffectiveClock();
                if (currentComputerConfiguration.cdp1861Configuration.defined)
                    pixiePointer->initPixie();
            }
            runPressed_ = false;
        }
        if (currentComputerConfiguration.runPressType != RUN_TYPE_VELF)
        {
            cycleLed();
            cpuCycles_ = 0;
            instructionCounter_= 0;

            machineCycle();
            machineCycle();
            cpuCycles_ += 2;
            playSaveLoad();

            if (resetPressed_)
                resetPressed();
            
            p_Main->startTime();
        }
        if (cpuMode_ == LOAD && currentComputerConfiguration.ledDisplayConfiguration.showDataOnLoad)
        {
            showData(readMem(address_));
            ledCycleValue_ = 1;
            threadPointer->Sleep(1);
        }
        if (cpuMode_ == PAUSE && currentComputerConfiguration.multiSegDisplayConfiguration.tilFontFile != "")
        {
            showData(readMem(address_));
            ledCycleValue_ = 1;
            threadPointer->Sleep(1);
        }
    }
    if (interruptRequested_ && cpuState_ == STATE_FETCH_1)
        p_Computer->interrupt();

    if (interruptEnable_ && clear_ == 1 && (cpuState_ == STATE_FETCH_1 || (cpuState_ == STATE_EXECUTE_1 && instructionCode_ == 0)))
    {
        if (numberOfCdp1877Instances_ > 0)
        {
            for (int instance=0; instance<numberOfCdp1877Instances_; instance++)
            {
                if (cdp1877InstancePointer[instance]->readMien() && cdp1877InstancePointer[instance]->findInterrupt() != -1)
                    interrupt();
            }
        }
        else
        {
            bool interruptRequest = false;
            
            for (int type=0; type<INTERRUPT_TYPE_MAX; type++)
            {
                if (interruptRequested[type] && interruptRequestedCounter[type] > 0)
                    interruptRequestedCounter[type]--;
                
                if (interruptRequested[type] && interruptRequestedCounter[type] == 0)
                {
                    interruptRequest |= interruptRequested[type];
                    interruptRequestedCounter[type] = 0;
                }
            }
            if (interruptRequest)
            {
                if (interrupt())
                {
                    if (currentComputerConfiguration.vis1870Configuration.defined)
                        if (currentComputerConfiguration.vis1870Configuration.outputInterruptReset.portNumber[0] == -1)
                            interruptRequested[INTERRUPT_TYPE_VIS] = false;
                    interruptRequested[INTERRUPT_TYPE_INPUT] = false;
                    if (currentComputerConfiguration.i8275Configuration.defined)
                    {
                        interruptRequested[INTERRUPT_TYPE_I8275_4] = false;
                        i8275Pointer->setRowEf8275(1);
                    }
                }
            }
        }
    }
}

void Computer::resetPressed()
{
    switch (currentComputerConfiguration.bootstrapConfiguration.type)
    {
        case BOOTSTRAPRUN:
            runPressedAtStartup_ = p_Main->runPressed();
            if (runPressedAtStartup_)
            {
                bootstrap_ = currentComputerConfiguration.bootstrapConfiguration.address;
                slotOut(2);
                vipMode_ = true;
            }
            else
            {
                bootstrap_ = 0;
                slotOut(1);
                vipMode_ = false;
            }
        break;

        case BOOTSTRAPTIMER_ONIN:
        break;
    
        case BOOTSTRAPTIMER:
            addressLatchCounter_ = currentComputerConfiguration.bootstrapConfiguration.timer;
            bootstrap_ = currentComputerConfiguration.bootstrapConfiguration.address;
        break;

        default:
            if (currentComputerConfiguration.bootstrapConfiguration.defined)
                bootstrap_ = currentComputerConfiguration.bootstrapConfiguration.address;
        break;
    }
    
    p_Main->stopTerminal();
    terminalStop();

    resetCpu();
    resetComputer();
    if (currentComputerConfiguration.i8275Configuration.defined)
        i8275Pointer->cRegWrite(0x40);
    if (currentComputerConfiguration.vis1870Configuration.defined)
    {
        vis1870Pointer->init1870();
        if (currentComputerConfiguration.vis1870Configuration.statusBarType != STATUSBAR_NONE)
            p_Main->v1870BarSizeEvent();

        if (currentComputerConfiguration.diagnosticBoardConfiguration.defined)
        {
            diagRomActive_ = currentComputerConfiguration.diagnosticBoardConfiguration.active;
            p_Main->eventUpdateLedStatus(diagRomActive_, 1);
            diagDmaLedOn_ = false;
            p_Main->eventUpdateLedStatus(diagDmaLedOn_, 2);
            p_Main->eventUpdateLedStatus(false, 5);
        }

    }
    for (int pad=0; pad<lastLatchKeyPad_; pad++)
    {
        if (currentComputerConfiguration.keyLatchConfiguration[pad].defined)
            latchKeyPointer[pad]->resetKeybLatch();
    }
    if (currentComputerConfiguration.matrixKeyboardConfiguration.defined)
        matrixKeyboardPointer->resetKeyboard();

    showCycleData(0);
    if (currentComputerConfiguration.autoBootConfiguration.defined)
    {
        scratchpadRegister_[0] = currentComputerConfiguration.autoBootConfiguration.address;
        autoBoot();
    }
    else
    {
        if (currentComputerConfiguration.giantBoardConfiguration.defined)
        {
            scratchpadRegister_[0] = currentComputerConfiguration.giantBoardConfiguration.base;
            autoBoot();
        }
        else
        {
            setClear(0);
            setWait(1);
        }
    }
    resetPressed_ = false;
    currentComputerConfiguration.diagnosticBoardConfiguration.active = p_Main->isDiagOn();

    elfRunState_ = RESETSTATE;

    if (currentComputerConfiguration.multicartEmsNumber_ == -1)
    {
        p_Main->setSwName("");
        p_Main->eventUpdateTitle();
    }
    
    if (currentComputerConfiguration.asciiKeyboardConfiguration.defined)
        startElfKeyFile("Xml");
    if (currentComputerConfiguration.cdp1871Configuration.defined)
        start1871KeyFile();
    for (int pad=0; pad<lastLatchKeyPad_; pad++)
    {
        if (currentComputerConfiguration.keyLatchConfiguration[pad].defined)
            latchKeyPointer[pad]->startKeyFile();
    }
    if (currentComputerConfiguration.matrixKeyboardConfiguration.defined)
        matrixKeyboardPointer->startKeyFile();
    if (currentComputerConfiguration.gpioPs2KeyboardConfiguration.defined)
        startPs2gpioKeyFile();
    
//    currentComputerConfiguration = p_Main->getConfiguration();
//      above line was to get the latest configuration probably only for clear RAM and clear RTC values, however above resets everything also other settings like EMS pages etc. As such I now just fetch the clearRam and clearRtc values from the GUI config.
    if (p_Main->getClearRamState()  && currentComputerConfiguration.nvRamConfiguration.defined)
    {
        for (size_t i=0; i<nvramDetails.size(); i++)
        {
            for (long address = nvramDetails[i].start; address <= nvramDetails[i].end; address++)
                mainMemory_[address] = 0;
        }
        p_Main->resetClearRamState();
    }
    if (p_Main->getClearRtcState() && currentComputerConfiguration.rtcDs12887Configuration.defined)
    {
        for (int i = 0; i<128; i++)
        {
            rtcRam_[i] = 0;
            p_Main->updateDebugMemory(i);
        }
        rtcRam_[0xa] = 0x20;
        rtcRam_[0xb] = 0x6;
        rtcRam_[0xc] = 0;
        rtcRam_[0xd] = 0x80;
        currentComputerConfiguration.clearRtc = false;
        p_Main->resetClearRtcState();
    }
    if (currentComputerConfiguration.fredVideoConfiguration.defined)
        fredVideoPointer->initPixie();

    if (currentComputerConfiguration.hwTapeConfiguration.startOnReset)
        startLoad(0, false);

    if (currentComputerConfiguration.videoTerminalConfiguration.type != VTNONE)
        qLedStatus_ = (1 ^ vtPointer->ef()) << 1;
    if (currentComputerConfiguration.videoTerminalConfiguration.external || currentComputerConfiguration.videoTerminalConfiguration.loop_back)
        qLedStatus_ = (1 ^ p_Serial->ef()) << 1;
    
    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
    {
        panelPointer[frontPanel]->setErrorLed(0);
        panelPointer[frontPanel]->setReadyLed(1);
        panelPointer[frontPanel]->setStopLed(1);
        
        for (int i=0; i<8; i++)
            panelPointer[frontPanel]->setLed(i,0);
        
        panelPointer[frontPanel]->setQLed(qLedStatus_);
    }
}

void Computer::showDataLeds(Byte value)
{
    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
    {
        for (int i=0; i<8; i++)
        {
            panelPointer[frontPanel]->setLed(i, value&1);
            value = value >> 1;
        }
        panelPointer[frontPanel]->setReadyLed(1);
    }
}

void Computer::configureMemory()
{
    for (std::vector<MemoryConfiguration>::iterator memConfig = currentComputerConfiguration.memoryConfiguration.begin (); memConfig != currentComputerConfiguration.memoryConfiguration.end (); ++memConfig)
    {
        switch (memConfig->type & 0xff)
        {
            case ROM:
                if (memConfig->mcr)
                {
                    memConfig->mcrMemNumber = numberOfMcrs_;
                    allocMcrMemory(memConfig->end + 1, memConfig->type);
                    
                }
                else
                {
                    defineMemoryType(memConfig->start, memConfig->end, memConfig->type);

                    if (memConfig->useMemMask)
                    {
                        defineMemoryType(memConfig->start + memConfig->memMask + 1, memConfig->end, MAPPEDROM + (memConfig->type & 0xff00));
                    }

                    for (std::vector<Word>::iterator address = memConfig->mappingList.begin (); address != memConfig->mappingList.end (); ++address)
                    {
                        defineMemoryType(*address, *address+memConfig->end-memConfig->start, MAPPEDROM + (memConfig->type & 0xff00));
                    }
                }
            break;
                
            case DIAGROM:
                defineMemoryType(memConfig->start, memConfig->end, memConfig->type);

                diagRomActive_ = true;
            break;
                                
            case RAM:
                if (memConfig->mcr)
                {
                    memConfig->mcrMemNumber = numberOfMcrs_;
                    allocMcrMemory(memConfig->end + 1, memConfig->type);
                }
                else
                {
                    defineMemoryType(memConfig->start, memConfig->end, memConfig->type);
                    
                    if (memConfig->useMemMask)
                    {
                        defineMemoryType(memConfig->start + memConfig->memMask + 1, memConfig->end, MAPPEDRAM + (memConfig->type & 0xff00));
                    }
                    
                    for (std::vector<Word>::iterator address = memConfig->mappingList.begin (); address != memConfig->mappingList.end (); ++address)
                    {
                        defineMemoryType(*address, *address+memConfig->end-memConfig->start, MAPPEDRAM + (memConfig->type & 0xff00));
                    }

                    initRam(memConfig->start, memConfig->end);
                }
            break;

            case NVRAM:
                if (memConfig->mcr)
                {
                    memConfig->mcrMemNumber = numberOfMcrs_;
                    allocMcrMemory(memConfig->end + 1, memConfig->type);
                }
                else
                {
                    defineMemoryType(memConfig->start, memConfig->end, memConfig->type);
        
                    if (memConfig->useMemMask)
                    {
                        defineMemoryType(memConfig->start + memConfig->memMask + 1, memConfig->end, MAPPEDRAM + (memConfig->type & 0xff00));
                    }

                    for (std::vector<Word>::iterator address = memConfig->mappingList.begin (); address != memConfig->mappingList.end (); ++address)
                    {
                        defineMemoryType(*address, *address+memConfig->end-memConfig->start, MAPPEDRAM + (memConfig->type & 0xff00));
                    }

                    initRam(memConfig->start, memConfig->end);
                }
            break;

            case PAGER:
                configurePortExt(currentComputerConfiguration.memoryMapperConfiguration);
                
                allocPagerMemory(memConfig->start, memConfig->end);
                definePortExtForPager(currentComputerConfiguration.memoryMapperConfiguration.startPort, currentComputerConfiguration.memoryMapperConfiguration.endPort);

                defineMemoryType(memConfig->start, memConfig->end, PAGER);
            break;
        }
    }

    size_t emsNumber = 0;
    
    for (std::vector<EmsMemoryConfiguration>::iterator emsConfig = currentComputerConfiguration.emsMemoryConfiguration.begin (); emsConfig != currentComputerConfiguration.emsMemoryConfiguration.end (); ++emsConfig)
    {        
        emsMemory_.resize(emsNumber+1);
                
        if (emsConfig->emsType == ROM)
        {
            readRomMapperBinFile(emsNumber, emsConfig->dirname+emsConfig->filename);
        }
        else
            allocEmsMemorySegment(emsNumber);

        for (std::vector<WordRange>::iterator range = emsConfig->range.begin (); range != emsConfig->range.end (); ++range)
        {
            if (range->start != range->end)
                defineMemoryType(range->start, range->end, emsConfig->type);
        }

        emsNumber++;
    }
    
    if (currentComputerConfiguration.giantBoardConfiguration.defined)
        defineMemoryType(0xF000, 0xFFFF, ROM);
    
    for (std::vector<MemoryPartConfiguration>::iterator ramPartConfigIterator = currentComputerConfiguration.memoryRamPartConfiguration.begin (); ramPartConfigIterator != currentComputerConfiguration.memoryRamPartConfiguration.end (); ++ramPartConfigIterator)
    {
        defineMemoryType(ramPartConfigIterator->start, ramPartConfigIterator->end, PARTRAM);
    }

    if (currentComputerConfiguration.slotConfiguration.maxSlotNumber_ > 0)
    {
        defineMemoryType(currentComputerConfiguration.slotConfiguration.start, currentComputerConfiguration.slotConfiguration.end, SLOTMEM);
        
        for (int slot=0; slot<currentComputerConfiguration.slotConfiguration.maxSlotNumber_; slot++)
        {
            allocSlotMemory();
            selectedBank_ = 0;
            selectedSlot_ = slot;
            
            if (currentComputerConfiguration.slotConfiguration.slotInfo[slot].maxBankNumber_ > 1)
            {
                for (int bank=0; bank<currentComputerConfiguration.slotConfiguration.slotInfo[slot].maxBankNumber_; bank++)
                {
                    selectedBank_ = bank;
                    
                    defineXmlBankMemoryType(slot, bank, 0, slotSize_[slot]-1, currentComputerConfiguration.slotConfiguration.slotInfo[slot].bankInfo[bank].type);

                    if (currentComputerConfiguration.slotConfiguration.slotInfo[slot].bankInfo[bank].filename != "")
                        readProgram(currentComputerConfiguration.slotConfiguration.slotInfo[slot].bankInfo[bank].dirname, currentComputerConfiguration.slotConfiguration.slotInfo[slot].bankInfo[bank].filename, SLOTMEM,  currentComputerConfiguration.slotConfiguration.start, NONAME);
                }
            }
            else
            {
                defineXmlSlotMemoryType(slot, 0, slotSize_[slot]-1, currentComputerConfiguration.slotConfiguration.slotInfo[slot].type);
                
                if (currentComputerConfiguration.slotConfiguration.slotInfo[slot].filename != "")
                    readProgram(currentComputerConfiguration.slotConfiguration.slotInfo[slot].dirname, currentComputerConfiguration.slotConfiguration.slotInfo[slot].filename, SLOTMEM,  currentComputerConfiguration.slotConfiguration.start, NONAME);
                
                for (int ramPart = 0; ramPart<4; ramPart++)
                {
                    if (currentComputerConfiguration.slotConfiguration.slotInfo[slot].ramStart[ramPart] != 0)
                    {
                        defineXmlSlotMemoryType(slot, currentComputerConfiguration.slotConfiguration.slotInfo[slot].ramStart[ramPart]-currentComputerConfiguration.slotConfiguration.start, currentComputerConfiguration.slotConfiguration.slotInfo[slot].ramEnd[ramPart]-currentComputerConfiguration.slotConfiguration.start, RAM);
                    }
                }
            }
        }
        if (currentComputerConfiguration.slotConfiguration.useIoGroup)
            ioGroup_ = selectedSlot_;
        setOutType(currentComputerConfiguration.slotConfiguration.output, MEMORY_SLOT_OUT);
    }
    selectedSlot_ = 0;
    selectedBank_ = 0;

    if (currentComputerConfiguration.mcrConfiguration.maxMapNumber_ > 0)
    {
        defineMemoryType(currentComputerConfiguration.mcrConfiguration.start, currentComputerConfiguration.mcrConfiguration.end, MCRMEM);

        for (int map=0; map<currentComputerConfiguration.mcrConfiguration.maxMapNumber_; map++)
        {
            if (currentComputerConfiguration.mcrConfiguration.mapInfo[map].defined)
            {
                for (std::vector<MapMemInfo>::iterator mapInfoIterator = currentComputerConfiguration.mcrConfiguration.mapInfo[map].mapMemInfo.begin (); mapInfoIterator != currentComputerConfiguration.mcrConfiguration.mapInfo[map].mapMemInfo.end (); ++mapInfoIterator)
                {
                    allocMcrMapMemory(mapInfoIterator->type);
                    defineMcrMapMemoryType(map, mapInfoIterator->start, mapInfoIterator->end, mapInfoIterator->type + (mapInfoIterator->memNumber & 0x7));
                }
            }
        }
    }
    selectedMap_ = 0;

    size_t memConfNumber = 2;
    while (memConfNumber < currentComputerConfiguration.memoryConfiguration.size())
    {
        switch (currentComputerConfiguration.memoryConfiguration[memConfNumber].type & 0xff)
        {
            case ROM:
            case DIAGROM:
            case RAM:
                loadRomRam(memConfNumber);
            break;

            case NVRAM:
                loadNvRam(memConfNumber);
            break;
        }
        memConfNumber++;
    }
    if ((currentComputerConfiguration.memoryConfiguration[p_Main->getRomRamButton0()].type & 0xff) == NVRAM)
        loadNvRam(p_Main->getRomRamButton0());
    else
    {
        if ((currentComputerConfiguration.memoryConfiguration[p_Main->getRomRamButton0()].type & 0xff) != UNDEFINED)
            loadRomRam(p_Main->getRomRamButton0());
    }
    if ((currentComputerConfiguration.memoryConfiguration[p_Main->getRomRamButton1()].type & 0xff) == NVRAM)
        loadNvRam(p_Main->getRomRamButton1());
    else
    {
        if ((currentComputerConfiguration.memoryConfiguration[p_Main->getRomRamButton1()].type & 0xff) != UNDEFINED)
            loadRomRam(p_Main->getRomRamButton1());
    }
}

void Computer::configureExtensions()
{
    wxString fileName, fileName2, message;
    wxString ioGroup;
    wxString printBuffer;

    inKey1_ = p_Main->getDefaultInKey1("Xml");
    inKey2_ = p_Main->getDefaultInKey2("Xml");
    p_Main->getDefaultHexKeys(XML, p_Main->getComputerString(), "A", keyDefA1_, keyDefA2_, keyDefGameHexA_);
    p_Main->getDefaultHexKeys(XML, p_Main->getComputerString(), "B", keyDefB1_, keyDefB2_, keyDefGameHexB_);

    if (p_Main->getConfigBool("/Xml/GameAuto", true))
        p_Main->loadKeyDefinition(currentComputerConfiguration.memoryConfiguration[0].filename, currentComputerConfiguration.memoryConfiguration[1].filename, keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_);

    if (currentComputerConfiguration.videoTerminalConfiguration.type != VTNONE)
    {
        double zoom = p_Main->getZoomVt();
        if (currentComputerConfiguration.videoTerminalConfiguration.type == VT52)
            vtPointer = new Vt100(p_Main->getRunningComputerText() + " - VT 52", p_Main->getVtPos(), wxSize(800*zoom, 500*zoom), zoom, computerClockSpeed_, currentComputerConfiguration, UART1);
        else
            vtPointer = new Vt100(p_Main->getRunningComputerText() + " - VT 100", p_Main->getVtPos(), wxSize(800*zoom, 500*zoom), zoom, computerClockSpeed_, currentComputerConfiguration, UART1);
        p_Vt100[UART1] = vtPointer;
        vtPointer->configure(currentComputerConfiguration.videoTerminalConfiguration,  currentComputerConfiguration.addressLocationConfiguration, currentComputerConfiguration.saveCommand_);
        vtPointer->Show(currentComputerConfiguration.videoTerminalConfiguration.show);
        vtPointer->drawScreen();
    }

    if (currentComputerConfiguration.videoTerminalConfiguration.external)
    {
        p_Serial = new Serial(XML, computerClockSpeed_, currentComputerConfiguration);
        p_Serial->configure(currentComputerConfiguration.videoTerminalConfiguration.baudR, currentComputerConfiguration.videoTerminalConfiguration.baudT, currentComputerConfiguration.videoTerminalConfiguration);
    }

    if (currentComputerConfiguration.videoTerminalConfiguration.loop_back)
    {
        p_Serial = new Serial(XML, computerClockSpeed_, currentComputerConfiguration);
        p_Serial->configure(currentComputerConfiguration.videoTerminalConfiguration.baudR, currentComputerConfiguration.videoTerminalConfiguration.baudT, currentComputerConfiguration.videoTerminalConfiguration);
    }

    if (currentComputerConfiguration.rtcM48t58Configuration.defined)
    {
        message.Printf("RTC M48T58 at address %04X-%04X\n", currentComputerConfiguration.rtcM48t58Configuration.control, currentComputerConfiguration.rtcM48t58Configuration.year);
        p_Main->configureMessage(0, message);
    }

    if (currentComputerConfiguration.rtcCdp1879Configuration.defined)
        configureRtcCdp1879(currentComputerConfiguration.rtcCdp1879Configuration, computerClockSpeed_);

    if (currentComputerConfiguration.rtcDs12887Configuration.defined)
        configureRtcDs12788(currentComputerConfiguration.rtcDs12887Configuration);

    if (currentComputerConfiguration.dipConfiguration.defined)
    {
        p_Main->configureMessage(&currentComputerConfiguration.dipConfiguration.ioGroupVector, "DIP switch");
        
        message.Printf("hex value: %02X", currentComputerConfiguration.dipConfiguration.value);
        setInType(&currentComputerConfiguration.dipConfiguration.ioGroupVector, currentComputerConfiguration.dipConfiguration.input, message);

        p_Main->message("");
    }
    
    if (currentComputerConfiguration.adConvertorConfiguration.defined)
    {
        p_Main->configureMessage(&currentComputerConfiguration.adConvertorConfiguration.ioGroupVector, "A/D Convertor & Printer");
        setInType(&currentComputerConfiguration.adConvertorConfiguration.ioGroupVector, currentComputerConfiguration.adConvertorConfiguration.input, "AD/I Input (channel 0 to E)");
        setOutType(&currentComputerConfiguration.adConvertorConfiguration.ioGroupVector, currentComputerConfiguration.adConvertorConfiguration.output, "select channel/printer data (channel F)");

        if (currentComputerConfiguration.adConvertorConfiguration.addressStart != -1 && currentComputerConfiguration.adConvertorConfiguration.addressEnd != -1)
        {
            message.Printf("	@%04X-@%04X: AD/S Input (channel 0 to F)", currentComputerConfiguration.adConvertorConfiguration.addressStart, currentComputerConfiguration.adConvertorConfiguration.addressEnd);
            p_Main->message(message);
        }
        p_Main->message("");
    }

    if (currentComputerConfiguration.dmaConfiguration.defined)
        setCycleType(CYCLE_TYPE_DRAM_DMA, DRAM_DMA_CYCLE);

    if (currentComputerConfiguration.interruptConfiguration.defined)
        setCycleType(CYCLE_TYPE_INTERRUPT_CLOCK, INTERRUPT_CLOCK_CYCLE);

    if (currentComputerConfiguration.nvRamMpConfiguration.defined)
    {
        p_Main->configureMessage(&currentComputerConfiguration.nvRamMpConfiguration.ioGroupVector, "NVRAM Memory Protect");

        setInType(&currentComputerConfiguration.nvRamMpConfiguration.ioGroupVector, currentComputerConfiguration.nvRamMpConfiguration.input, "switch memory protect");

        p_Main->message("");
    }

    if (currentComputerConfiguration.superBoardConfiguration.defined)
    {
        p_Main->configureMessage(&currentComputerConfiguration.superBoardConfiguration.ioGroupVector, "USB for COMX Super Board");
        setOutType(&currentComputerConfiguration.superBoardConfiguration.ioGroupVector, currentComputerConfiguration.superBoardConfiguration.output, "UART out");
        setInType(&currentComputerConfiguration.superBoardConfiguration.ioGroupVector, currentComputerConfiguration.superBoardConfiguration.input5, "UART in");
        setInType(&currentComputerConfiguration.superBoardConfiguration.ioGroupVector, currentComputerConfiguration.superBoardConfiguration.input6, "reset ef");
        setEfType(&currentComputerConfiguration.superBoardConfiguration.ioGroupVector, currentComputerConfiguration.superBoardConfiguration.ef, "USB Idle");

        p_Main->message("");
    }
    
    diagDmaLedOn_ = false;
    if (currentComputerConfiguration.diagnosticBoardConfiguration.defined)
    {
        p_Main->configureMessage(&currentComputerConfiguration.diagnosticBoardConfiguration.ioGroupVector, "Diagnose Card");
        setOutType(&currentComputerConfiguration.diagnosticBoardConfiguration.ioGroupVector, currentComputerConfiguration.diagnosticBoardConfiguration.output, "automated keyboard test");
        setInType(&currentComputerConfiguration.diagnosticBoardConfiguration.ioGroupVector, currentComputerConfiguration.diagnosticBoardConfiguration.input1, "bit 1 debounce, bit 2 Step, bit 6 Abort, bit 7 Repeat");
        setInType(&currentComputerConfiguration.diagnosticBoardConfiguration.ioGroupVector, currentComputerConfiguration.diagnosticBoardConfiguration.input2, "bit 1 ROM Checksum, bit 2 IDEN, bit 3 Factory unit");
        
        p_Main->message("");

        diagRomActive_ = currentComputerConfiguration.diagnosticBoardConfiguration.active;

        if (currentComputerConfiguration.vis1870Configuration.defined)
        {
            p_Main->eventUpdateLedStatus(diagRomActive_, 1);
            p_Main->eventUpdateLedStatus(diagDmaLedOn_, 2);
        }
    }
    
    if (currentComputerConfiguration.cdp1855Configuration.defined)
    {
        cdp1855InstancePointer = new Cdp1855Instance();
        cdp1855InstancePointer->configureCdp1855(currentComputerConfiguration.cdp1855Configuration);
    }

    if (currentComputerConfiguration.ay_3_8912Configuration.defined)
    {
        ay_3_8912InstancePointer = new AY_3_8912Instance();
        ay_3_8912InstancePointer->configureAY_3_8912(currentComputerConfiguration.ay_3_8912Configuration);
    }

    cdp1877InstancePointer.clear();
    numberOfCdp1877Instances_ = 0;
    for (std::vector<Cdp1877Configuration>::iterator cdp1877 = currentComputerConfiguration.cdp1877Configuration.begin (); cdp1877 != currentComputerConfiguration.cdp1877Configuration.end (); ++cdp1877)
    {
        cdp1877InstancePointer.resize(numberOfCdp1877Instances_+1);
        cdp1877InstancePointer[numberOfCdp1877Instances_] = new Cdp1877Instance(numberOfCdp1877Instances_);
        cdp1877InstancePointer[numberOfCdp1877Instances_]->configureCdp1877(*cdp1877);
        numberOfCdp1877Instances_++;
    }

    cdp1878InstancePointer.clear();
    numberOfCdp1878Instances_ = 0;
    for (std::vector<Cdp1878Configuration>::iterator cdp1878 = currentComputerConfiguration.cdp1878Configuration.begin (); cdp1878 != currentComputerConfiguration.cdp1878Configuration.end (); ++cdp1878)
    {
        cdp1878InstancePointer.resize(numberOfCdp1878Instances_+1);
        cdp1878InstancePointer[numberOfCdp1878Instances_] = new Cdp1878Instance(numberOfCdp1878Instances_);
        cdp1878InstancePointer[numberOfCdp1878Instances_]->configureCdp1878(*cdp1878);
        if (numberOfCdp1878Instances_ == 0)
        {
            computerTimerPointer[0]->Start((double)(1000/cdp1878->clockA), wxTIMER_CONTINUOUS);
            computerTimerPointer[1]->Start((double)(1000/cdp1878->clockB), wxTIMER_CONTINUOUS);
        }
        numberOfCdp1878Instances_++;
    }

    cdp1851FramePointer.clear();
    numberOfCdp1851Frames_ = 0;
    for (std::vector<Cdp1851Configuration>::iterator cdp1851 = currentComputerConfiguration.cdp1851Configuration.begin (); cdp1851 != currentComputerConfiguration.cdp1851Configuration.end (); ++cdp1851)
    {
        cdp1851FramePointer.resize(numberOfCdp1851Frames_+1);
#if defined (__WXMAC__) || (__linux__)
        cdp1851FramePointer[numberOfCdp1851Frames_] = new PioFrame("CDP1851 PIO", cdp1851->pos, wxSize(310, 180), numberOfCdp1851Frames_, *cdp1851);
#else
        cdp1851FramePointer[numberOfCdp1851Frames_] = new PioFrame("CDP1851 PIO", cdp1851->pos, wxSize(329, 180), numberOfCdp1851Frames_, *cdp1851);
#endif
        
        p_Main->configureMessage(&cdp1851->ioGroupVector, "CDP1851 PIO");
        setOutType(&cdp1851->ioGroupVector, cdp1851->writePortA, "write to port A", numberOfCdp1851Frames_);
        setOutType(&cdp1851->ioGroupVector, cdp1851->writePortB, "write to port B", numberOfCdp1851Frames_);
        setInType(&cdp1851->ioGroupVector, cdp1851->readPortA, "read port A", numberOfCdp1851Frames_);
        setInType(&cdp1851->ioGroupVector, cdp1851->readPortB, "read port B", numberOfCdp1851Frames_);
        setOutType(&cdp1851->ioGroupVector, cdp1851->writeControl, "write control register", numberOfCdp1851Frames_);
        setInType(&cdp1851->ioGroupVector, cdp1851->readStatus, "read status", numberOfCdp1851Frames_);
        setEfType(&cdp1851->ioGroupVector, cdp1851->efaRdy, "ARDY", numberOfCdp1851Frames_);
        setEfType(&cdp1851->ioGroupVector, cdp1851->efbRdy, "BRDY", numberOfCdp1851Frames_);
        setEfType(&cdp1851->ioGroupVector, cdp1851->efIrq, "IRQ", numberOfCdp1851Frames_);

        p_Main->message("");

        cdp1851FramePointer[numberOfCdp1851Frames_]->reset();
        numberOfCdp1851Frames_++;
    }

    cdp1852FramePointer.clear();
    numberOfCdp1852Frames_ = 0;
    for (std::vector<Cdp1852Configuration>::iterator cdp1852 = currentComputerConfiguration.cdp1852Configuration.begin (); cdp1852 != currentComputerConfiguration.cdp1852Configuration.end (); ++cdp1852)
    {
        cdp1852FramePointer.resize(numberOfCdp1852Frames_+1);
#if defined (__WXMAC__) || (__linux__)
        cdp1852FramePointer[numberOfCdp1852Frames_] = new Cdp1852Frame("CDP1852", cdp1852->pos, wxSize(310, 180), numberOfCdp1852Frames_);
#else
        cdp1852FramePointer[numberOfCdp1852Frames_] = new Cdp1852Frame("CDP1852", cdp1852->pos, wxSize(329, 180), numberOfCdp1852Frames_);
#endif
        
        p_Main->configureMessage(&cdp1852->ioGroupVector, "CDP1852");
        setOutType(&cdp1852->ioGroupVector, cdp1852->writePort, "write to port", numberOfCdp1852Frames_);
        setInType(&cdp1852->ioGroupVector, cdp1852->readPort, "read port", numberOfCdp1852Frames_);
        setEfType(&cdp1852->ioGroupVector, cdp1852->efStb, numberOfCdp1852Frames_, "STB");

        p_Main->message("");

        cdp1852FramePointer[numberOfCdp1852Frames_]->reset();
        numberOfCdp1852Frames_++;
    }

    cdp1854InstancePointer.clear();
    numberOfCdp1854Instances_ = 0;
    for (std::vector<Cdp1854Configuration>::iterator cdp1854 = currentComputerConfiguration.cdp1854Configuration.begin(); cdp1854 != currentComputerConfiguration.cdp1854Configuration.end(); ++cdp1854)
    {
        cdp1854InstancePointer.resize(numberOfCdp1854Instances_+1);
        cdp1854InstancePointer[numberOfCdp1854Instances_] = new Cdp1854Instance(numberOfCdp1854Instances_);
        
        cdp1854InstancePointer[numberOfCdp1854Instances_]->configureCdp1854(*cdp1854, computerClockSpeed_, currentComputerConfiguration.cdp1854Configuration.size());
    
        switch (cdp1854->connection)
        {
            case UART_CONNECTION_TU58:
                cdp1854Ut58Connection_ = numberOfCdp1854Instances_;
            break;

            case UART_CONNECTION_VT1802:
            break;

            case UART_CONNECTION_VIS1802:
                
            break;

            case UART_CONNECTION_VT100:
                cdp1854Vt100Connection_ = numberOfCdp1854Instances_;
            break;

        }
        numberOfCdp1854Instances_++;
    }

    cd4536bPointer.clear();
    numberOfCd4536b_ = 0;
    for (std::vector<Cd4536bConfiguration>::iterator cd4536bIo = currentComputerConfiguration.cd4536bConfiguration.begin (); cd4536bIo != currentComputerConfiguration.cd4536bConfiguration.end (); ++cd4536bIo)
    {
        cd4536bPointer.resize(numberOfCd4536b_+1);
        cd4536bPointer[numberOfCd4536b_] = new Cd4536b();
        
        cd4536bPointer[numberOfCd4536b_]->Configure(*cd4536bIo, numberOfCd4536b_);

        numberOfCd4536b_++;
    }
}

void Computer::configureVideoExtensions()
{
    if (currentComputerConfiguration.coinConfiguration.defined)
    {
        double zoom = p_Main->getZoom(currentComputerConfiguration.coinConfiguration.videoNumber);
        coinPointer = new Pixie(p_Main->getRunningComputerText() + " - Coin Video", p_Main->getCoinPos(), wxSize(64*zoom*currentComputerConfiguration.coinConfiguration.xScale, 128*zoom), zoom, currentComputerConfiguration.coinConfiguration.xScale, currentComputerConfiguration.coinConfiguration.videoNumber, VIDEOCOIN, currentComputerConfiguration.cdp1861Configuration, currentComputerConfiguration.cdp1862Configuration, currentComputerConfiguration.bootstrapConfiguration.type);
        p_Video[currentComputerConfiguration.coinConfiguration.videoNumber] = coinPointer;
        coinPointer->configurePixieCoinArcade(currentComputerConfiguration.coinConfiguration);
        coinPointer->initPixie();
        coinPointer->setZoom(zoom);
        coinPointer->Show(true);
    }

    if (currentComputerConfiguration.cdp1861Configuration.defined)
    {
        double zoom = p_Main->getZoom(currentComputerConfiguration.cdp1861Configuration.videoNumber);
        pixiePointer = new Pixie(p_Main->getRunningComputerText() + " - Pixie", p_Main->getPixiePos(), wxSize(64*zoom*currentComputerConfiguration.cdp1861Configuration.xScale, 128*zoom), zoom, currentComputerConfiguration.cdp1861Configuration.xScale, currentComputerConfiguration.cdp1861Configuration.videoNumber, VIDEOXMLPIXIE, currentComputerConfiguration.cdp1861Configuration, currentComputerConfiguration.cdp1862Configuration, currentComputerConfiguration.bootstrapConfiguration.type);
        p_Video[currentComputerConfiguration.cdp1861Configuration.videoNumber] = pixiePointer;
        
        switch (currentComputerConfiguration.cdp1861Configuration.doubleScreenIo)
        {
            case PIXIE_IN_OUT:
                pixiePointer->configurePixie();
            break;

            case PIXIE_IN_IN:
                pixiePointer->configurePixieIn();
            break;

            case PIXIE_OUT_OUT:
                pixiePointer->configurePixieOut();
            break;

            case PIXIE_DOUBLE:
                pixiePointer->configurePixieSuper();
            break;
        }
        if (currentComputerConfiguration.cdp1862Configuration.defined)
        {
            if (currentComputerConfiguration.cdp1862Configuration.startRam != -1 && currentComputerConfiguration.cdp1862Configuration.endRam != -1)
                defineMemoryType(currentComputerConfiguration.cdp1862Configuration.startRam, currentComputerConfiguration.cdp1862Configuration.endRam, COLOURRAM);
            pixiePointer->configureCdp1862(currentComputerConfiguration.autoBootConfiguration.defined);
        }
        pixiePointer->initPixie();
        pixiePointer->setZoom(zoom);
        pixiePointer->Show(true);
    }

    if (currentComputerConfiguration.cdp1864Configuration.defined)
    {
        double zoom = p_Main->getZoom(currentComputerConfiguration.cdp1864Configuration.videoNumber);
        cdp1864Pointer = new Pixie(p_Main->getRunningComputerText() + " - CDP1864", p_Main->getCdp1864Pos(), wxSize(64*zoom*currentComputerConfiguration.cdp1864Configuration.xScale, 192*zoom), zoom, currentComputerConfiguration.cdp1864Configuration.xScale, currentComputerConfiguration.cdp1864Configuration.videoNumber, VIDEOXML1864, currentComputerConfiguration.cdp1861Configuration, currentComputerConfiguration.cdp1862Configuration, currentComputerConfiguration.bootstrapConfiguration.type);
        p_Video[currentComputerConfiguration.cdp1864Configuration.videoNumber] = cdp1864Pointer;
        if (!currentComputerConfiguration.cdp1864Configuration.colorLatch)
            if (currentComputerConfiguration.cdp1864Configuration.startRam != -1 && currentComputerConfiguration.cdp1864Configuration.endRam != -1)
                defineMemoryType(currentComputerConfiguration.cdp1864Configuration.startRam, currentComputerConfiguration.cdp1864Configuration.endRam, COLOURRAM1864);
        cdp1864Pointer->configureCdp1864(currentComputerConfiguration.cdp1864Configuration);
        cdp1864Pointer->initPixie();
        cdp1864Pointer->setZoom(zoom);
        cdp1864Pointer->Show(true);
    }
    else
    {
        if (currentComputerConfiguration.soundConfiguration.type == SOUND_1863_1864 || currentComputerConfiguration.soundConfiguration.type == SOUND_1863_NOQ)
            configureCdp1863();
    }

    if (currentComputerConfiguration.studio4VideoConfiguration.defined)
    {
        double zoom = p_Main->getZoom(currentComputerConfiguration.studio4VideoConfiguration.videoNumber);
        st4VideoPointer = new PixieStudioIV(p_Main->getRunningComputerText(), p_Main->getSt4Pos(), wxSize(64*zoom*currentComputerConfiguration.studio4VideoConfiguration.xScale, 192*zoom), zoom, currentComputerConfiguration.studio4VideoConfiguration.xScale, currentComputerConfiguration.studio4VideoConfiguration.videoNumber, VIDEOSTUDIOIV, currentComputerConfiguration.cdp1861Configuration, currentComputerConfiguration.cdp1862Configuration, currentComputerConfiguration.bootstrapConfiguration.type);
        p_Video[currentComputerConfiguration.studio4VideoConfiguration.videoNumber] = st4VideoPointer;
        if (currentComputerConfiguration.studio4VideoConfiguration.startRam != -1 && currentComputerConfiguration.studio4VideoConfiguration.endRam != -1)
            defineMemoryType(currentComputerConfiguration.studio4VideoConfiguration.startRam, currentComputerConfiguration.studio4VideoConfiguration.endRam, COLOURRAMST4);
        st4VideoPointer->configureSt4(currentComputerConfiguration.studio4VideoConfiguration);
        st4VideoPointer->initPixie();
        st4VideoPointer->setZoom(zoom);
        st4VideoPointer->Show(true);
    }

    if (currentComputerConfiguration.vip2KVideoConfiguration.defined)
    {
        double zoom = p_Main->getZoom(currentComputerConfiguration.vip2KVideoConfiguration.videoNumber);
        vip2KVideoPointer = new PixieVip2K(p_Main->getRunningComputerText(), p_Main->getVip2KPos(), wxSize(64*zoom*currentComputerConfiguration.vip2KVideoConfiguration.xScale, 192*zoom), zoom, currentComputerConfiguration.vip2KVideoConfiguration.xScale, currentComputerConfiguration.vip2KVideoConfiguration.videoNumber, VIDEOVIP2K, currentComputerConfiguration.cdp1861Configuration, currentComputerConfiguration.cdp1862Configuration, currentComputerConfiguration.bootstrapConfiguration.type);
        p_Video[currentComputerConfiguration.vip2KVideoConfiguration.videoNumber] = vip2KVideoPointer;
        p_Computer->readIntelFile(currentComputerConfiguration.vip2KVideoConfiguration.sequencerDirectory + currentComputerConfiguration.vip2KVideoConfiguration.sequencerFile, &sequencerMemory, 2048);
        vip2KVideoPointer->configureVip2K(currentComputerConfiguration.vip2KVideoConfiguration);
        vip2KVideoPointer->initPixie();
        vip2KVideoPointer->setZoom(zoom);
        vip2KVideoPointer->Show(true);
    }

    if (currentComputerConfiguration.fredVideoConfiguration.defined)
    {
        double zoom = p_Main->getZoom(currentComputerConfiguration.fredVideoConfiguration.videoNumber);
        fredVideoPointer = new PixieFred(p_Main->getRunningComputerText(), p_Main->getFredPos(), wxSize(192*zoom*currentComputerConfiguration.fredVideoConfiguration.xScale, 128*zoom), zoom, currentComputerConfiguration.fredVideoConfiguration.xScale, currentComputerConfiguration.fredVideoConfiguration.videoNumber, VIDEOFRED, currentComputerConfiguration.cdp1861Configuration, currentComputerConfiguration.cdp1862Configuration, currentComputerConfiguration.bootstrapConfiguration.type);
        p_Video[currentComputerConfiguration.fredVideoConfiguration.videoNumber] = fredVideoPointer;
        fredVideoPointer->configureFredVideo(currentComputerConfiguration.fredVideoConfiguration);
        fredVideoPointer->initPixie();
        fredVideoPointer->setZoom(zoom);
        fredVideoPointer->Show(true);
    }
    
    if (currentComputerConfiguration.mc6845Configuration.defined)
    {
        double zoom = p_Main->getZoom(currentComputerConfiguration.mc6845Configuration.videoNumber);
        mc6845Pointer = new MC6845(p_Main->getRunningComputerText() + " - MC6845", p_Main->get6845Pos(), wxSize(currentComputerConfiguration.mc6845Configuration.screenSize.x*currentComputerConfiguration.mc6845Configuration.charSize.x*zoom, currentComputerConfiguration.mc6845Configuration.screenSize.y*(currentComputerConfiguration.mc6845Configuration.charSize.y+1)*zoom), zoom, computerClockSpeed_, currentComputerConfiguration.mc6845Configuration);
        p_Video[currentComputerConfiguration.mc6845Configuration.videoNumber] = mc6845Pointer;
        mc6845Pointer->configure6845();
        mc6845Pointer->init6845();
        mc6845Pointer->Show(true);
        currentComputerConfiguration.mc6845Configuration.defined = true;
    }

    if (currentComputerConfiguration.i8275Configuration.defined)
    {
        double zoom = p_Main->getZoom(currentComputerConfiguration.i8275Configuration.videoNumber);
        i8275Pointer = new i8275(p_Main->getRunningComputerText() + " - Intel 8275", p_Main->get8275Pos(), wxSize(80*8*zoom, 24*10*2*zoom), zoom, computerClockSpeed_, currentComputerConfiguration.i8275Configuration);
        p_Video[currentComputerConfiguration.i8275Configuration.videoNumber] = i8275Pointer;
        i8275Pointer->configure8275();
        i8275Pointer->init8275();
        i8275Pointer->Show(true);
    }

    if (currentComputerConfiguration.mc6847Configuration.defined)
    {
        double zoom = p_Main->getZoom(currentComputerConfiguration.mc6847Configuration.videoNumber);
        mc6847Pointer = new mc6847(p_Main->getRunningComputerText() + " - MC6847", p_Main->get6847Pos(), wxSize(32*8*zoom, currentComputerConfiguration.mc6847Configuration.screenHeight*zoom), zoom, computerClockSpeed_, currentComputerConfiguration.mc6847Configuration);
        p_Video[currentComputerConfiguration.mc6847Configuration.videoNumber] = mc6847Pointer;
        mc6847Pointer->configure();
        mc6847Pointer->init6847();
        mc6847Pointer->Show(true);
    }

    if (currentComputerConfiguration.tmsConfiguration.defined)
    {
        double zoom = p_Main->getZoom(currentComputerConfiguration.tmsConfiguration.videoNumber);
        tmsPointer = new Tms9918(p_Main->getRunningComputerText() + " - TMS 9918", p_Main->getTmsPos(), wxSize(320*zoom,240*zoom), zoom, computerClockSpeed_, currentComputerConfiguration.tmsConfiguration);
        p_Video[currentComputerConfiguration.tmsConfiguration.videoNumber] = tmsPointer;
        tmsPointer->configure();
        tmsPointer->Show(true);
    }
    
    if (currentComputerConfiguration.sn76430NConfiguration.defined)
    {
        double zoom = p_Main->getZoom(currentComputerConfiguration.sn76430NConfiguration.videoNumber);
        sn76430nPointer = new SN76430N(p_Main->getRunningComputerText() + " - SN76430N", p_Main->getSN76430NPos(), wxSize(128*zoom,96*zoom), zoom, computerClockSpeed_, 3.58, currentComputerConfiguration.sn76430NConfiguration);
        p_Video[currentComputerConfiguration.sn76430NConfiguration.videoNumber] = sn76430nPointer;
        sn76430nPointer->configure();
        sn76430nPointer->init();
        sn76430nPointer->Show(true);
        
        defineMemoryType(0x800, 0xFFF, SN76430NRAM);
    }
}

void Computer::configureV1870Extension()
{
    if (currentComputerConfiguration.vis1870Configuration.defined)
    {
        double zoom = p_Main->getZoom(currentComputerConfiguration.vis1870Configuration.videoNumber);
        vis1870Pointer = new VIS1870(p_Main->getRunningComputerText() + " - VIS 1870", p_Main->getV1870Pos(), wxSize(240*zoom,216*zoom), zoom, computerClockSpeed_, currentComputerConfiguration.vis1870Configuration);
        p_Video[currentComputerConfiguration.vis1870Configuration.videoNumber] = vis1870Pointer;
        
        bool loadRom = vis1870Pointer->configure1870();
        vis1870Pointer->init1870();
        if (loadRom)
        {
            if (currentComputerConfiguration.vis1870Configuration.charRomIsTmcFormat)
                vis1870Pointer->readChargenFileTmc(p_Main->getCharRomDir(), p_Main->getCharRomFile());
            else
                vis1870Pointer->readChargenFile(p_Main->getCharRomDir(), p_Main->getCharRomFile());
        }
             
        ramGroupAtV1870_ = false;
        if ((memoryType_[0xf400/256]&0xff) == RAM)
            ramGroupAtV1870_ = true;
            
        defineMemoryType(0xf400, 0xf7ff, CRAM1870);
        defineMemoryType(0xf800, 0xffff, PRAM1870);
        vis1870Pointer->Show(true);
    }
}

void Computer::configureKeyboardExtensions()
{
    lastBitKeyPad_ = 0;
    for (int pad=0; pad<MAX_BITKEYPADS; pad++)
    {
        if (currentComputerConfiguration.bitKeypadConfiguration[pad].defined)
        {
            if (pad == 0)
                p_Main->message("Configuring keypad / keyboard");
            
            bitkeypadPointer[pad] = new BitKeypad();
            bitkeypadPointer[pad]->configure(currentComputerConfiguration.bitKeypadConfiguration[pad]);
            lastBitKeyPad_ = pad;
        }
    }
    if (currentComputerConfiguration.bitKeypadConfiguration[lastBitKeyPad_].defined)
    {
        p_Computer->setCycleType(CYCLE_TYPE_KEYBOARD, BIT_KEYPAD_CYCLE);
        bitKeypadValue_ = 0;
        p_Main->message("");
    }
    
    if (currentComputerConfiguration.cvKeypadConfiguration.defined)
    {
        cvkeypadPointer = new CvKeypad();
        cvkeypadPointer->configure(currentComputerConfiguration.cvKeypadConfiguration);
    }

    if (currentComputerConfiguration.etiKeypadConfiguration.defined)
    {
        etikeypadPointer = new EtiKeypad();
        etikeypadPointer->configure(currentComputerConfiguration.etiKeypadConfiguration, keyDefA1_, keyDefA2_);
    }

    lastLatchKeyPad_ = 0;
    if (currentComputerConfiguration.keyLatchConfiguration[0].defined)
    {
        latchKeyPointer[0]= new KeybLatch;
        latchKeyPointer[0]->configure(currentComputerConfiguration.keyLatchConfiguration[0], currentComputerConfiguration.addressLocationConfiguration, "keyboard", currentComputerConfiguration.saveCommand_);
        lastLatchKeyPad_ = 1;
    }

    for (int pad=1; pad<MAX_LATCHKEYPADS; pad++)
    {
        if (currentComputerConfiguration.keyLatchConfiguration[pad].defined)
        {
            latchKeyPointer[pad] = new KeybLatch;
            latchKeyPointer[pad]->configure(currentComputerConfiguration.keyLatchConfiguration[pad], currentComputerConfiguration.addressLocationConfiguration, "keypad" + currentComputerConfiguration.keyLatchConfiguration[pad].padNumberStr, currentComputerConfiguration.saveCommand_);
            lastLatchKeyPad_ = pad+1;
            
            if (currentComputerConfiguration.keyLatchConfiguration[pad].loadAutoConfig)
            {
                if (pad == 1)
                    latchKeyPointer[pad]->reDefineHexKeys(keyDefA1_, keyDefA2_, simDefA2_);
                if (pad == 2)
                    latchKeyPointer[pad]->reDefineHexKeys(keyDefB1_, keyDefB2_, simDefB2_);
            }
        }
    }

    if (currentComputerConfiguration.matrixKeyboardConfiguration.defined)
    {
        matrixKeyboardPointer = new KeybMatrix;
        matrixKeyboardPointer->configure(currentComputerConfiguration.matrixKeyboardConfiguration, currentComputerConfiguration.saveCommand_);
    }

    if (currentComputerConfiguration.fredKeypadConfiguration.defined)
    {
        fredkeypadPointer = new KeypadFred();
        fredkeypadPointer->configure(currentComputerConfiguration.fredKeypadConfiguration, keyDefA1_, keyDefA2_);
    }

    if (currentComputerConfiguration.asciiKeyboardConfiguration.defined)
        configureKeyboard(currentComputerConfiguration.asciiKeyboardConfiguration, currentComputerConfiguration.addressLocationConfiguration, currentComputerConfiguration.saveCommand_);

    if (currentComputerConfiguration.cdp1871Configuration.defined)
        configureKeyb1871(currentComputerConfiguration.cdp1871Configuration, currentComputerConfiguration.addressLocationConfiguration, currentComputerConfiguration.saveCommand_);

    if (currentComputerConfiguration.ps2KeyboardConfiguration.defined)
        configurePs2(currentComputerConfiguration.ps2KeyboardConfiguration);
    
    if (currentComputerConfiguration.gpioPs2KeyboardConfiguration.defined)
        configurePs2gpio(currentComputerConfiguration.forceUpperCase, currentComputerConfiguration.gpioPs2KeyboardConfiguration);
}

void Computer::configureDiskExtensions()
{
    wxString fileName;
    
    if (currentComputerConfiguration.fdcConfiguration.wd1793_defined)
    {
        configure1793(currentComputerConfiguration.fdcConfiguration);
        resetFdc();

        for (int disk=0; disk<4; disk++)
        {
            fileName = p_Main->getFloppyFile(FDCTYPE_17XX, disk);
            if (fileName.Len() == 0)
                setFdcDiskname(disk+1, "");
            else
                setFdcDiskname(disk+1, p_Main->getFloppyDir(FDCTYPE_17XX, disk)+p_Main->getFloppyFile(FDCTYPE_17XX, disk));
        }
    }

    if (currentComputerConfiguration.fdcConfiguration.wd1770_defined)
    {
        configure1770(currentComputerConfiguration.fdcConfiguration);
        resetFdc();

        for (int disk=0; disk<4; disk++)
        {
            fileName = p_Main->getFloppyFile(FDCTYPE_17XX, disk);
            if (fileName.Len() == 0)
                setFdcDiskname(disk+1, "");
            else
                setFdcDiskname(disk+1, p_Main->getFloppyDir(FDCTYPE_17XX, disk)+p_Main->getFloppyFile(FDCTYPE_17XX, disk));
        }
    }

    if (currentComputerConfiguration.upd765Configuration.defined)
    {
        configureUpd765(currentComputerConfiguration.upd765Configuration);

        for (int disk=0; disk<4; disk++)
        {
            if (p_Main->getDirectoryMode(FDCTYPE_UPD765, disk))
                setUpdDiskname(disk+1, p_Main->getFloppyDirSwitched(FDCTYPE_UPD765, disk), "");
            else
            {
                fileName = p_Main->getFloppyFile(FDCTYPE_UPD765, disk);
                if (fileName.Len() == 0)
                    setUpdDiskname(disk+1, p_Main->getFloppyDir(FDCTYPE_UPD765, disk), "");
                else
                    setUpdDiskname(disk+1, p_Main->getFloppyDir(FDCTYPE_UPD765, disk), p_Main->getFloppyFile(FDCTYPE_UPD765, disk));
            }
        }
    }

    if (currentComputerConfiguration.ideConfiguration.defined)
    {
        configureIde(p_Main->getFloppyDir(FDCTYPE_TU58_IDE, 0) + p_Main->getFloppyFile(FDCTYPE_TU58_IDE, 0), p_Main->getFloppyDir(FDCTYPE_TU58_IDE, 1) + p_Main->getFloppyFile(FDCTYPE_TU58_IDE, 1), currentComputerConfiguration.ideConfiguration);
    }

    if (currentComputerConfiguration.tu58Configuration.defined)
    {
        configureTu58(currentComputerConfiguration.tu58Configuration);
    }
}

void Computer::configurePrinterExtensions()
{
    wxString ioGroup;
    wxString printBuffer;

    if (currentComputerConfiguration.basicPrinterConfiguration.defined)
    {
        p_Printer = new Printer();
        p_Printer->configureBasicPrinter(currentComputerConfiguration.basicPrinterConfiguration);
        p_Printer->init(p_Printer, PRINTER_BASIC);
    }

    if (currentComputerConfiguration.parallelPrinterConfiguration.defined)
    {
        p_PrinterParallel = new Printer();
        p_PrinterParallel->configureParallelPrinter(currentComputerConfiguration.parallelPrinterConfiguration);
        p_PrinterParallel->init(p_PrinterParallel, PRINTER_PARALLEL);
    }
    
    if (currentComputerConfiguration.serialPrinterConfiguration.defined)
    {
        p_PrinterSerial = new Printer();
        p_PrinterSerial->configureSerialPrinter(currentComputerConfiguration.serialPrinterConfiguration);
        p_PrinterSerial->init(p_PrinterSerial, PRINTER_SERIAL);
    }
    
    if (currentComputerConfiguration.qSerialPrinterConfiguration.defined)
    {
        p_Printer = new Printer();
        p_Printer->init(p_Printer, PRINTER_BASIC);
        p_Printer->configureQSerialPrinter(currentComputerConfiguration.qSerialPrinterConfiguration);
    }
    
    if (currentComputerConfiguration.thermalPrinterConfiguration.defined)
    {
        p_PrinterThermal = new Printer();
        p_PrinterThermal->configureThermalPrinter(currentComputerConfiguration.thermalPrinterConfiguration);
        p_PrinterThermal->init(p_PrinterThermal, COMXTHPRINTER);
    }

    if (currentComputerConfiguration.centronicsPrinterConfiguration.defined)
    {
        p_Printer = new Printer();
        p_Printer->init(p_Printer, PRINTER_BASIC);
        p_Printer->configureCentronicsPrinter(currentComputerConfiguration.centronicsPrinterConfiguration);
    }
}

void Computer::configureSoundExtensions()
{
    wxString ioGroup;
    wxString printBuffer;

    if (currentComputerConfiguration.soundConfiguration.type == SOUND_SUPER_VP550 || currentComputerConfiguration.soundConfiguration.type == SOUND_SUPER_VP551)
    {
        setCycleType(CYCLE_TYPE_SOUND, SOUND_SUPER_VP550_CYCLE);
    }
     
    setSoundType (currentComputerConfiguration.soundConfiguration.type);

    if (currentComputerConfiguration.bitSoundConfiguration.defined)
    {
        p_Main->configureMessage(&currentComputerConfiguration.bitSoundConfiguration.ioGroupVector, "Sound");
        
        printBuffer.Printf("using mask %d: tone wave", currentComputerConfiguration.bitSoundConfiguration.output.mask);
        setOutType(&currentComputerConfiguration.bitSoundConfiguration.ioGroupVector, currentComputerConfiguration.bitSoundConfiguration.output, printBuffer);
        
        p_Main->message("");
    }
}

void Computer::configureTapeExtensions()
{
    wxString ioGroup;
    wxString printBuffer;

    if (currentComputerConfiguration.swTapeConfiguration.defined)
    {
        p_Main->configureMessage(&currentComputerConfiguration.swTapeConfiguration.ioGroupVector, "Cassette");

        setOutType(&currentComputerConfiguration.swTapeConfiguration.ioGroupVector, currentComputerConfiguration.swTapeConfiguration.output, "motor");
        setOutType(&currentComputerConfiguration.swTapeConfiguration.ioGroupVector, currentComputerConfiguration.swTapeConfiguration.outputQ, "signal output (insead of Q)");
        setEfType(&currentComputerConfiguration.swTapeConfiguration.ioGroupVector, currentComputerConfiguration.swTapeConfiguration.ef, "signal input");
        setEfType(&currentComputerConfiguration.swTapeConfiguration.ioGroupVector, currentComputerConfiguration.swTapeConfiguration.efRun, "running");

        p_Main->message("");
    }

    if (currentComputerConfiguration.hwTapeConfiguration.defined)
    {
        if (currentComputerConfiguration.hwTapeConfiguration.format == TAPE_FORMAT_CV)
        {
            p_Main->configureMessage(&currentComputerConfiguration.hwTapeConfiguration.ioGroupVector, "CyberVision Cassette");
            setOutType(&currentComputerConfiguration.hwTapeConfiguration.ioGroupVector, currentComputerConfiguration.hwTapeConfiguration.output, "write data");
            setInType(&currentComputerConfiguration.hwTapeConfiguration.ioGroupVector, currentComputerConfiguration.hwTapeConfiguration.input, "read data");
            setEfType(&currentComputerConfiguration.hwTapeConfiguration.ioGroupVector, currentComputerConfiguration.hwTapeConfiguration.ef, "data ready");
            setEfType(&currentComputerConfiguration.hwTapeConfiguration.ioGroupVector, currentComputerConfiguration.hwTapeConfiguration.efBufferEmpty, "write buffer empty");
            
            p_Main->message("");

            p_Main->eventHwTapeStateChange(HW_TAPE_STATE_PLAY);
        }
        else
        {
            p_Main->configureMessage(&currentComputerConfiguration.hwTapeConfiguration.ioGroupVector, "FRED Cassette");
                        
            setOutType(&currentComputerConfiguration.hwTapeConfiguration.ioGroupVector, currentComputerConfiguration.hwTapeConfiguration.outputMode, "bit 4 = program mode, bit 5 = direct mode, bit 6 = write mode");
            setOutType(&currentComputerConfiguration.hwTapeConfiguration.ioGroupVector, currentComputerConfiguration.hwTapeConfiguration.outputSound, "bit 0 = 1 - run tape, bit 1 = 1 - sound on, bit 2 = sound");
            setInType(&currentComputerConfiguration.hwTapeConfiguration.ioGroupVector, currentComputerConfiguration.hwTapeConfiguration.input, "read data");
            
            setEfType(&currentComputerConfiguration.hwTapeConfiguration.ioGroupVector, currentComputerConfiguration.hwTapeConfiguration.ef, "data ready (if mode = 2)");
            setEfType(&currentComputerConfiguration.hwTapeConfiguration.ioGroupVector, currentComputerConfiguration.hwTapeConfiguration.efRun, "tape run/stop");
            setEfType(&currentComputerConfiguration.hwTapeConfiguration.ioGroupVector, currentComputerConfiguration.hwTapeConfiguration.efError, "tape error");

            p_Main->message("");
        }
    }
}

void Computer::configureCdp1863()
{
    wxString printBuffer = "";

    p_Main->configureMessage(&currentComputerConfiguration.cdp1863Configuration.ioGroupVector, "CDP 1863");
    
    p_Computer->setOutType(&currentComputerConfiguration.cdp1863Configuration.ioGroupVector, currentComputerConfiguration.cdp1863Configuration.toneLatch, "tone latch");
    p_Computer->setOutType(&currentComputerConfiguration.cdp1863Configuration.ioGroupVector, currentComputerConfiguration.cdp1863Configuration.toneSwitch1, "tone on/off");
    p_Computer->setOutType(&currentComputerConfiguration.cdp1863Configuration.ioGroupVector, currentComputerConfiguration.cdp1863Configuration.toneSwitch2, "tone on/off");

    p_Main->message("");

    p_Computer->setTonePeriod(0, 32 *(128 + 1), false);
    p_Computer->setTonePeriod(1, 32 *(128 + 1), false);
}

void Computer::moveWindows()
{
    if (currentComputerConfiguration.coinConfiguration.defined)
        coinPointer->Move(p_Main->getCoinPos());
    if (currentComputerConfiguration.cdp1861Configuration.defined)
        pixiePointer->Move(p_Main->getPixiePos());
    if (currentComputerConfiguration.cdp1864Configuration.defined)
        cdp1864Pointer->Move(p_Main->getCdp1864Pos());
    if (currentComputerConfiguration.studio4VideoConfiguration.defined)
        st4VideoPointer->Move(p_Main->getSt4Pos());
    if (currentComputerConfiguration.vip2KVideoConfiguration.defined)
        vip2KVideoPointer->Move(p_Main->getVip2KPos());
    if (currentComputerConfiguration.fredVideoConfiguration.defined)
        fredVideoPointer->Move(p_Main->getFredPos());
    if (currentComputerConfiguration.tmsConfiguration.defined)
        tmsPointer->Move(p_Main->getTmsPos());
    if (currentComputerConfiguration.mc6845Configuration.defined)
        mc6845Pointer->Move(p_Main->get6845Pos());
    if (currentComputerConfiguration.mc6847Configuration.defined)
        mc6847Pointer->Move(p_Main->get6847Pos());
    if (currentComputerConfiguration.i8275Configuration.defined)
        i8275Pointer->Move(p_Main->get8275Pos());
    if (currentComputerConfiguration.vis1870Configuration.defined)
        vis1870Pointer->Move(p_Main->getV1870Pos());
    if (currentComputerConfiguration.sn76430NConfiguration.defined)
        sn76430nPointer->Move(p_Main->getSN76430NPos());
    if (currentComputerConfiguration.videoTerminalConfiguration.type != VTNONE)
        vtPointer->Move(p_Main->getVtPos());
    for (int num=0; num<numberOfCdp1851Frames_; num++)
        cdp1851FramePointer[num]->Move(p_Main->getCdp1851Pos(num));
    for (int num=0; num<numberOfCdp1852Frames_; num++)
        cdp1852FramePointer[num]->Move(p_Main->getCdp1852Pos(num));
    for (int num=0; num<numberOfFrontPanels_; num++)
        panelPointer[num]->Move(p_Main->getFrontPanelPos(num));
}

void Computer::updateTitle(wxString Title)
{
    if (currentComputerConfiguration.coinConfiguration.defined)
        coinPointer->SetTitle(p_Main->getRunningComputerText() + " - Coin Video"+Title);
    if (currentComputerConfiguration.cdp1861Configuration.defined)
        pixiePointer->SetTitle(p_Main->getRunningComputerText() + " - Pixie"+Title);
    if (currentComputerConfiguration.cdp1864Configuration.defined)
        cdp1864Pointer->SetTitle(p_Main->getRunningComputerText() + " - CDP1864"+Title);
    if (currentComputerConfiguration.studio4VideoConfiguration.defined)
        st4VideoPointer->SetTitle(p_Main->getRunningComputerText() +Title);
    if (currentComputerConfiguration.vip2KVideoConfiguration.defined)
        vip2KVideoPointer->SetTitle(p_Main->getRunningComputerText() +Title);
    if (currentComputerConfiguration.fredVideoConfiguration.defined)
        fredVideoPointer->SetTitle(p_Main->getRunningComputerText() +Title);
    if (currentComputerConfiguration.tmsConfiguration.defined)
        tmsPointer->SetTitle(p_Main->getRunningComputerText() + " - TMS 9918"+Title);
    if (currentComputerConfiguration.mc6845Configuration.defined)
        mc6845Pointer->SetTitle(p_Main->getRunningComputerText() + " - MC6845"+Title);
    if (currentComputerConfiguration.mc6847Configuration.defined)
        mc6847Pointer->SetTitle(p_Main->getRunningComputerText() + " - MC6847"+Title);
    if (currentComputerConfiguration.i8275Configuration.defined)
        i8275Pointer->SetTitle(p_Main->getRunningComputerText() + " - Intel 8275"+Title);
    if (currentComputerConfiguration.vis1870Configuration.defined)
        vis1870Pointer->SetTitle(p_Main->getRunningComputerText() + " - VIS 1870"+Title);
    if (currentComputerConfiguration.sn76430NConfiguration.defined)
        sn76430nPointer->SetTitle(p_Main->getRunningComputerText() + " - SN76430N"+Title);
    if (currentComputerConfiguration.videoTerminalConfiguration.type == VT52)
        vtPointer->SetTitle(p_Main->getRunningComputerText() + " - VT 52"+Title);
    if (currentComputerConfiguration.videoTerminalConfiguration.type == VT100)
        vtPointer->SetTitle(p_Main->getRunningComputerText() + " - VT 100"+Title);
}

void Computer::releaseButtonOnScreen1851(HexButton* buttonPointer, int WXUNUSED(buttonType), int pioNumber)
{
    cdp1851FramePointer[pioNumber]->releaseButtonOnScreen(buttonPointer);
}

void Computer::releaseButtonOnScreen1852(HexButton* buttonPointer, int WXUNUSED(buttonType), int pioNumber)
{
    cdp1852FramePointer[pioNumber]->releaseButtonOnScreen(buttonPointer);
}

void Computer::showCdp1851(int pioNumber, bool state)
{
    cdp1851FramePointer[pioNumber]->Show(state);
    if (state)
        cdp1851FramePointer[pioNumber]->refreshLeds();
}

void Computer::showCdp1852(int pioNumber, bool state)
{
    cdp1852FramePointer[pioNumber]->Show(state);
    if (state)
        cdp1852FramePointer[pioNumber]->refreshLeds();
}

void Computer::removeCdp1851(int pioNumber)
{
    showCdp1851(pioNumber, false);
}
    
void Computer::removeCdp1852(int pioNumber)
{
    showCdp1852(pioNumber, false);
}
    
void Computer::setForceUpperCase(bool status)
{
    if (currentComputerConfiguration.videoTerminalConfiguration.type != VTNONE)
        vtPointer->setForceUCVt(status);
    if (currentComputerConfiguration.asciiKeyboardConfiguration.defined)
        setForceUpperCaseKeyboard(status);
    if (currentComputerConfiguration.gpioPs2KeyboardConfiguration.defined)
        setForceUpperCasePs2gpio(status);
}

void Computer::keyClear()
{
    if (currentComputerConfiguration.etiKeypadConfiguration.defined)
        etikeypadPointer->keyClear();
    if (currentComputerConfiguration.cdp1871Configuration.defined)
        keyClear1871();
}

void Computer::onReset()
{
    if (currentComputerConfiguration.diagnosticBoardConfiguration.defined)
        if (currentComputerConfiguration.vis1870Configuration.defined)
            p_Main->eventUpdateLedStatus(true, 5);
    resetPressed_ = true;
}

void Computer::sleepComputer(long ms)
{
    threadPointer->Sleep(ms);
}

Byte Computer::read8275CharRom(Word address)
{
    if (currentComputerConfiguration.i8275Configuration.defined)
        return i8275Pointer->read8275CharRom(address);
    else
        return 0;
}

void Computer::write8275CharRom(Word address, Byte value)
{
    if (currentComputerConfiguration.i8275Configuration.defined)
        i8275Pointer->write8275CharRom(address, value);
}

Byte Computer::read8275VideoRam(Word address)
{
    if (currentComputerConfiguration.i8275Configuration.defined)
        return i8275Pointer->read8275VideoRam(address);
    else
        return 0;
}

void Computer::write8275VideoRam(Word address, Byte value)
{
    if (currentComputerConfiguration.i8275Configuration.defined)
        i8275Pointer->write8275VideoRam(address, value);
}

Byte Computer::read1862ColorDirect(Word addr)
{
    return colorMemory1862_[addr&colourMask1862_] & currentComputerConfiguration.ioMask[CDP1862_COLORRAM_OUT];
}

void Computer::write1862ColorDirect(Word addr, Byte value)
{
    colorMemory1862_[addr&colourMask1862_] = value & currentComputerConfiguration.ioMask[CDP1862_COLORRAM_OUT];
}

Byte Computer::read1864ColorDirect(Word addr)
{
    return colorMemory1864_[addr] & currentComputerConfiguration.ioMask[CDP1864_COLORRAM_OUT];
}

void Computer::write1864ColorDirect(Word addr, Byte value)
{
    colorMemory1864_[addr] = value & currentComputerConfiguration.ioMask[CDP1864_COLORRAM_OUT];
}

Byte Computer::readSt4ColorDirect(Word addr)
{
    return colorMemory1864_[addr];
}

void Computer::writeSt4ColorDirect(Word addr, Byte value)
{
    colorMemory1864_[addr] = value;
}

Byte Computer::read6845CharRom(Word address)
{
    if (currentComputerConfiguration.mc6845Configuration.defined)
        return mc6845Pointer->read6845CharRom(address);
    else
        return 0;
}

void Computer::write6845CharRom(Word address, Byte value)
{
    if (currentComputerConfiguration.mc6845Configuration.defined)
        mc6845Pointer->write6845CharRom(address, value);
}

Byte Computer::read6847CharRom(Word address)
{
    if (currentComputerConfiguration.mc6847Configuration.defined)
        return mc6847Pointer->read6847CharRom(address);
    else
        return 0;
}

void Computer::write6847CharRom(Word address, Byte value)
{
    if (currentComputerConfiguration.mc6847Configuration.defined)
        mc6847Pointer->write6847CharRom(address, value);
}

int Computer::readDirect6847(Word address)
{
    if (currentComputerConfiguration.mc6847Configuration.defined)
        return mc6847Pointer->readDirect6847(address);
    else
        return 0;
}

void Computer::writeDirect6847(Word address, int value)
{
    if (currentComputerConfiguration.mc6847Configuration.defined)
        mc6847Pointer->writeDirect6847(address, value);
}

Word Computer::get6847RamMask()
{
     if (currentComputerConfiguration.mc6847Configuration.defined)
        return mc6847Pointer->get6847RamMask();
    else
        return 0;
}

void Computer::setLedMs(long ms)
{
    ledTimeMs_ = ms;
    setLedMsTemp(ms);
    for (int num=0; num<numberOfCdp1851Frames_; num++)
        cdp1851FramePointer[num]->setLedMs(ms);
    for (int num=0; num<numberOfCdp1852Frames_; num++)
        cdp1852FramePointer[num]->setLedMs(ms);
}

void Computer::setLedMsTemp(long ms)
{
    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
        panelPointer[frontPanel]->setLedMs(ms);
    if (ms == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((computerClockSpeed_ * 1000000) / 8) / 1000) * ms;
    ledCycleValue_ = ledCycleSize_;
}

Byte Computer::getKey(Byte vtOut)
{
    return panelPointer[PANEL_MAIN]->getKey(vtOut);
}

void Computer::activateMainWindow()
{
    bool maximize = IsMaximized();
    Iconize(false);
    Raise();
    Show(true);
    Maximize(maximize);
}

void Computer::releaseButtonOnScreen(HexButton* buttonPointer)
{
    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
        panelPointer[frontPanel]->releaseButtonOnScreen(buttonPointer);
}

void Computer::setGreenLed(int status)
{
    qLedStatus_ = (qLedStatus_ & 1) | ((status & 1) << 1);
    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
        panelPointer[frontPanel]->setQLed(qLedStatus_);
}

void Computer::refreshPanel()
{
    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
        panelPointer[frontPanel]->refreshPanel();
}

void Computer::showPanel()
{
    for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
        panelPointer[frontPanel]->Show(p_Main->showFrontPanel());
}

void Computer::OnCdp1878TimerA(wxTimerEvent&WXUNUSED(event))
{
    for (int counter=0; counter<numberOfCdp1878Instances_; counter++)
        cdp1878InstancePointer[counter]->timeOut(0);
}

void Computer::OnCdp1878TimerB(wxTimerEvent&WXUNUSED(event))
{
    for (int counter=0; counter<numberOfCdp1878Instances_; counter++)
        cdp1878InstancePointer[counter]->timeOut(1);
}

void Computer::OnRtcTimer(wxTimerEvent&WXUNUSED(event))
{
    rtcCycle_--;

    if (rtcCycle_ == 1)
        rtcRam_[0xa] |= 0x80;

    if (rtcCycle_ > 0)
        return;
    
    rtcRam_[0xa] &= 0x7f;
    rtcCycle_ = 4;

    wxDateTime now = wxDateTime::Now();
    
    writeRtc(0, now.GetSecond());
    writeRtc(2, now.GetMinute());
    writeRtc(4, now.GetHour());
    writeRtc(6, now.GetWeekDay());
    writeRtc(7, now.GetDay());
    writeRtc(8, now.GetMonth()+1);
    writeRtc(9, now.GetYear()-1972);

    rtcRam_[0xc] |= 0x10;
    p_Main->updateDebugMemory(0xa);
    p_Main->updateDebugMemory(0xc);
}

void Computer::writeRtc(int address, Byte value)
{
    if (address == 0xc || address == 0xd)
        return;
    
    if (address == 0xa)
        rtcRam_[address] = (rtcRam_[address] & 0x80) | (value & 0x7f);
    else
        rtcRam_[address] = value;
    p_Main->updateDebugMemory(address);
}

void Computer::cid1Bit8(bool set)
{
    if (currentComputerConfiguration.bitKeypadConfiguration[0].defined)
        bitkeypadPointer[0]->cid1Bit8(set);
}

void Computer::changeDiskName(int disk, wxString dirName, wxString fileName)
{
    if (currentComputerConfiguration.fdcConfiguration.wd1770_defined || currentComputerConfiguration.fdcConfiguration.wd1793_defined)
        setFdcDiskname(disk, dirName + fileName);
    if (currentComputerConfiguration.tu58Configuration.defined && disk > 1)
        setTu58Diskname(disk-2, dirName + fileName);
    if (currentComputerConfiguration.ideConfiguration.defined && disk < 2)
        setIdeDiskname(disk, dirName + fileName);
}

Byte Computer::readPramDirect(Word address)
{
    return vis1870Pointer->readPramDirect(address);
}

Byte Computer::readCramDirect(Word address)
{
    return vis1870Pointer->readCramDirect(address);
}

Byte Computer::readColourRamDirect(Word address)
{
    return vis1870Pointer->readColourRamDirect(address);
};

Byte Computer::readGraphicRamDirect(Word address)
{
    return vis1870Pointer->readGraphicRamDirect(address);
};

void Computer::writePramDirect(Word address, Byte value)
{
    vis1870Pointer->writePramDirect(address, value);
}

void Computer::writeCramDirect(Word address, Byte value)
{
    vis1870Pointer->writeCramDirect(address, value);
}

void Computer::writeColourRamDirect(Word address, Byte value)
{
    vis1870Pointer->writeColourRamDirect(address, value);
};

void Computer::writeGraphicRamDirect(Word address, Byte value)
{
    vis1870Pointer->writeGraphicRamDirect(address, value);
};

int Computer::getMaxLinesPerChar()
{
    return vis1870Pointer->getMaxLinesPerChar();
}

Word Computer::getPageMemorySize()
{
    return vis1870Pointer->getPageMemorySize();
}

Word Computer::getCharMemorySize()
{
    return vis1870Pointer->getCharMemorySize();
}

Word Computer::getGraphicMemorySize()
{
    return vis1870Pointer->getGraphicMemorySize();
}

Byte Computer::getPcbMask()
{
    return vis1870Pointer->getPcbMask();
};

void Computer::saveNvRam()
{
    if (nvRamDisable_)
        return;
    
    Byte value;
    wxFile outputFile;
    
    for (size_t i=0; i<nvramDetails.size(); i++)
    {
        if (nvramDetails[i].filename != "")
        {
            if (wxFile::Exists(nvramDetails[i].dirname+nvramDetails[i].filename))
                outputFile.Open(nvramDetails[i].dirname+nvramDetails[i].filename, wxFile::write);
            else
                outputFile.Create(nvramDetails[i].dirname+nvramDetails[i].filename);
            if (nvramDetails[i].mcr)
            {
                for (long address = nvramDetails[i].start; address <= nvramDetails[i].end; address++)
                {
                    value = mcrMemory_[nvramDetails[i].mcrMemNumber][address];
                    outputFile.Write(&value, 1);
                }
            }
            else
            {
                for (long address = nvramDetails[i].start; address <= nvramDetails[i].end; address++)
                {
                    value = mainMemory_[address];
                    outputFile.Write(&value, 1);
                }
            }
            outputFile.Close();
        }
    }
}

void Computer::loadNvRam(size_t configNumber)
{
    NvramDetails newNvram;

    newNvram.start = currentComputerConfiguration.memoryConfiguration[configNumber].start;
    newNvram.end = currentComputerConfiguration.memoryConfiguration[configNumber].end;
    newNvram.dirname = currentComputerConfiguration.memoryConfiguration[configNumber].dirname;
    newNvram.filename = currentComputerConfiguration.memoryConfiguration[configNumber].dumpFilename;
    newNvram.mcr = currentComputerConfiguration.memoryConfiguration[configNumber].mcr;
    newNvram.mcrMemNumber = currentComputerConfiguration.memoryConfiguration[configNumber].mcrMemNumber;
    nvramDetails.push_back(newNvram);

    if (nvRamDisable_)
        return;

    wxFFile inFile;
    size_t length = newNvram.end-newNvram.start+1;

    char* buffer = new char[length];

//    char buffer[length];

    if (wxFile::Exists(newNvram.dirname+newNvram.filename))
    {
        if (inFile.Open(newNvram.dirname+newNvram.filename, _("rb")))
        {
            length = inFile.Read(buffer, length);
            if (newNvram.mcr)
            {
                for (size_t i=0; i<length; i++)
                    mcrMemory_[newNvram.mcrMemNumber][i+newNvram.start] = (Byte)buffer[i];
            }
            else
            {
                for (size_t i=0; i<length; i++)
                    mainMemory_[i+newNvram.start] = (Byte)buffer[i];
            }
            inFile.Close();
        }
    }
    if (currentComputerConfiguration.memoryConfiguration[configNumber].filename != "")
    {
        if (currentComputerConfiguration.memoryConfiguration[configNumber].verifyFileExist)
            p_Main->checkAndReInstallFile(currentComputerConfiguration.memoryConfiguration[configNumber].dirname + currentComputerConfiguration.memoryConfiguration[configNumber].filename, currentComputerConfiguration.memoryConfiguration[configNumber].filename);

        readProgram(currentComputerConfiguration.memoryConfiguration[configNumber].dirname, currentComputerConfiguration.memoryConfiguration[configNumber].filename, currentComputerConfiguration.memoryConfiguration[configNumber].type,  currentComputerConfiguration.memoryConfiguration[configNumber].start, NONAME);
    }

    delete[] buffer;
}

void Computer::checkComputerFunction()
{
//#if defined (__linux__)
//    if (currentComputerConfiguration.locationTriggerConfiguration[scratchpadRegister_[programCounter_]].index[0] == 0xff)
//#else
    if (currentComputerConfiguration.locationTriggerConfiguration[scratchpadRegister_[programCounter_]].index[0] == -1)
//#endif
        return;

// ***   for (char trigger : currentComputerConfiguration.locationTriggerConfiguration[scratchpadRegister_[programCounter_]].index)
	for (std::vector<signed char>::iterator trigger = currentComputerConfiguration.locationTriggerConfiguration[scratchpadRegister_[programCounter_]].index.begin (); trigger != currentComputerConfiguration.locationTriggerConfiguration[scratchpadRegister_[programCounter_]].index.end (); ++trigger)
    {
        if (currentComputerConfiguration.addressLocationConfiguration.locationInfo[*trigger].checkAddressInfo.size() == 0)
        {
            checkRegFunction(*trigger);
        }
        else if (currentComputerConfiguration.addressLocationConfiguration.locationInfo[*trigger].checkAddressInfo.size() == 1)
        {
            if (readMemDebug(currentComputerConfiguration.addressLocationConfiguration.locationInfo[*trigger].checkAddressInfo[0].checkAddress) == currentComputerConfiguration.addressLocationConfiguration.locationInfo[*trigger].checkAddressInfo[0].checkValue)
                executeFunction(currentComputerConfiguration.addressLocationConfiguration.locationInfo[*trigger].type, currentComputerConfiguration.addressLocationConfiguration.locationInfo[*trigger].additionalAddress);
        }
        else
        {
            bool execute = true;
//#if defined (__WXMAC__)
//            for (CheckAddressInfo addressInfo : currentComputerConfiguration.addressLocationConfiguration.locationInfo[*trigger].checkAddressInfo)
//            {
//                if (readMemDebug(addressInfo.checkAddress) != addressInfo.checkValue)
//                    execute = false;
//            }
//#else
            for (std::vector<CheckAddressInfo>::iterator addressInfo = currentComputerConfiguration.addressLocationConfiguration.locationInfo[*trigger].checkAddressInfo.begin (); addressInfo != currentComputerConfiguration.addressLocationConfiguration.locationInfo[*trigger].checkAddressInfo.end (); ++addressInfo)
            {
                if (readMemDebug(addressInfo->checkAddress) != addressInfo->checkValue)
                    execute = false;
            }
//#endif
            if (execute)
                executeFunction(currentComputerConfiguration.addressLocationConfiguration.locationInfo[*trigger].type, currentComputerConfiguration.addressLocationConfiguration.locationInfo[*trigger].additionalAddress);
        }
    }
}

void Computer::checkRegFunction(char trigger)
{
    if (currentComputerConfiguration.addressLocationConfiguration.locationInfo[trigger].checkRegInfo.size() == 0)
    {
        executeFunction(currentComputerConfiguration.addressLocationConfiguration.locationInfo[trigger].type, currentComputerConfiguration.addressLocationConfiguration.locationInfo[trigger].additionalAddress);
    }
    else if (currentComputerConfiguration.addressLocationConfiguration.locationInfo[trigger].checkRegInfo.size() == 1)
    {
        if (scratchpadRegister_[currentComputerConfiguration.addressLocationConfiguration.locationInfo[trigger].checkRegInfo[0].checkReg] == currentComputerConfiguration.addressLocationConfiguration.locationInfo[trigger].checkRegInfo[0].checkValue)
            executeFunction(currentComputerConfiguration.addressLocationConfiguration.locationInfo[trigger].type, currentComputerConfiguration.addressLocationConfiguration.locationInfo[trigger].additionalAddress);
    }
    else
    {
        bool execute = true;
        for (std::vector<CheckRegInfo>::iterator regInfo = currentComputerConfiguration.addressLocationConfiguration.locationInfo[trigger].checkRegInfo.begin (); regInfo != currentComputerConfiguration.addressLocationConfiguration.locationInfo[trigger].checkRegInfo.end (); ++regInfo)
        {
            if (scratchpadRegister_[regInfo->checkReg] != regInfo->checkValue)
                execute = false;
        }
        if (execute)
            executeFunction(currentComputerConfiguration.addressLocationConfiguration.locationInfo[trigger].type, currentComputerConfiguration.addressLocationConfiguration.locationInfo[trigger].additionalAddress);
    }
}

void Computer::executeFunction(int function, Word additionalAddress)
{
    switch (function)
    {
        case INFO_RESET_STATE:
            elfRunState_ = RESETSTATE;
        break;

        case INFO_UT_STATE:
            elfRunState_ = UTSTATE;
        break;

        case INFO_BASIC_STATE_CW:
            elfRunState_ = BASICSTATECW;
        break;

        case INFO_BASIC_STATE:
            elfRunState_ = BASICSTATE;
        break;

        case INFO_RUN_STATE:
            elfRunState_ = RUNSTATE;
        break;

        case INFO_STOP_CAS:
            if (currentComputerConfiguration.swTapeConfiguration.stopDelay <= 0 || !isSaving())
                p_Main->stopCassette();
            else
            {
                if (stopTapeCounter_ == 0)
                    stopTapeCounter_ = (currentComputerConfiguration.swTapeConfiguration.stopDelay * sampleRate_) / 1000;
            }
        break;

        case INFO_STOP_CAS_MICRO:
            if (saveStarted_)
            {
                stopPausedSave();
                saveStarted_ = false;
            }
            if (loadStarted_)
            {
                stopPausedLoad();
                loadStarted_ = false;
            }
        break;

        case INFO_START_CAS_SAVE:
            if (currentComputerConfiguration.addressLocationConfiguration.code_start_high != -1 && currentComputerConfiguration.addressLocationConfiguration.code_start_low != -1)
            {
                writeMem(currentComputerConfiguration.addressLocationConfiguration.code_start_high, (currentComputerConfiguration.addressLocationConfiguration.code_start>>8)&0xff, false);
                writeMem(currentComputerConfiguration.addressLocationConfiguration.code_start_low, currentComputerConfiguration.addressLocationConfiguration.code_start&0xff, false);
                p_Main->eventSaveStart(currentComputerConfiguration.addressLocationConfiguration.code_start);
                p_Main->eventSetLocation(true);
            }
            if (currentComputerConfiguration.addressLocationConfiguration.code_end_high != -1 && currentComputerConfiguration.addressLocationConfiguration.code_end_low != -1)
            {
                writeMem(currentComputerConfiguration.addressLocationConfiguration.code_end_high, (endSave_>>8)&0xff, false);
                writeMem(currentComputerConfiguration.addressLocationConfiguration.code_end_low, endSave_&0xff, false);
                p_Main->eventSaveEnd(endSave_);
                p_Main->eventSetLocation(true);
            }
            p_Main->startCassetteSave(0);
        break;

        case INFO_START_CAS_LOAD:
            if (currentComputerConfiguration.addressLocationConfiguration.code_start_high != -1 && currentComputerConfiguration.addressLocationConfiguration.code_start_low != -1)
            {
                writeMem(currentComputerConfiguration.addressLocationConfiguration.code_start_high, (currentComputerConfiguration.addressLocationConfiguration.code_start>>8)&0xff, false);
                writeMem(currentComputerConfiguration.addressLocationConfiguration.code_start_low, currentComputerConfiguration.addressLocationConfiguration.code_start&0xff, false);
                p_Main->eventSaveStart(currentComputerConfiguration.addressLocationConfiguration.code_start);
                p_Main->eventSetLocation(true);
            }
            p_Main->setSwName ("");
            p_Main->eventUpdateTitle();
            p_Main->startCassetteLoad(0);
        break;

        case INFO_START_CAS_DLOAD:
            p_Main->startCassetteLoad(0);
        break;

        case INFO_START_THERMAL:
            thermalPrinting_ = true;
        break;

        case INFO_STOP_THERMAL:
            thermalPrinting_ = false;
        break;
            
        case INFO_START_FM_BASIC:
            if (!fAndMBasicRunning_)
            {
                fAndMBasicRunning_ = true;
                p_Main->eventSetFandMBasicGui();
            }
        break;
            
        case INFO_STOP_FM_BASIC:
            if (fAndMBasicRunning_)
            {
                fAndMBasicRunning_ = false;
                p_Main->eventSetFandMBasicGui();
            }
        break;
            
        case INFO_SET_DOS_FILENAME:
            setDosFileName();
        break;

        case INFO_BATCH_START_SAVE:
            if (p_Main->isBatchConvertActive() && !batchInProgress_)
                batchConvert();
        break;
            
        case INFO_BATCH_END_SAVE:
            batchInProgress_ = false;
        break;

        case INFO_START_XMODEM_SAVE:
            interruptRequestedCounter[INTERRUPT_TYPE_UART] = 2;
            p_Main->startAutoTerminalSave(TERM_XMODEM_SAVE);
        break;

        case INFO_START_XMODEM_LOAD:
            interruptRequestedCounter[INTERRUPT_TYPE_UART] = 2;
            p_Main->startAutoTerminalLoad(TERM_XMODEM_LOAD);
        break;
            
        case INFO_START_YMODEM_SAVE:
            interruptRequestedCounter[INTERRUPT_TYPE_UART] = 2;
            p_Main->startYsTerminalSave(TERM_YMODEM_SAVE);
        break;
            
        case INFO_START_HEXMODEM_SAVE:
            interruptRequestedCounter[INTERRUPT_TYPE_UART] = 2;
            p_Main->startAutoTerminalSave(TERM_HEX);
        break;

        case INFO_START_HEXMODEM_LOAD:
            interruptRequestedCounter[INTERRUPT_TYPE_UART] = 2;
            p_Main->startAutoTerminalLoad(TERM_HEX);
        break;
            
        case INFO_START_BINMODEM_SAVE:
            interruptRequestedCounter[INTERRUPT_TYPE_UART] = 2;
            p_Main->startAutoTerminalSave(TERM_BIN);
        break;

        case INFO_START_BINMODEM_LOAD:
            interruptRequestedCounter[INTERRUPT_TYPE_UART] = 2;
            p_Main->startAutoTerminalLoad(TERM_BIN);
        break;
            
        case INFO_STOP_MODEM:
            p_Main->stopAutoTerminal();
        break;

        case INFO_CORRECT_CAPS:
            for (int pad=0; pad<lastLatchKeyPad_; pad++)
            {
                if (currentComputerConfiguration.keyLatchConfiguration[pad].defined)
                    latchKeyPointer[pad]->checkCaps();
            }
            if (currentComputerConfiguration.matrixKeyboardConfiguration.defined)
                matrixKeyboardPointer->checkCaps();
        break;
            
        case INFO_ELFOS_BOOT:
            if (currentComputerConfiguration.videoTerminalConfiguration.hexModem_defined)
            {
                currentComputerConfiguration.videoTerminalConfiguration.hexModem_defined = false;
                currentComputerConfiguration.videoTerminalConfiguration.xModem_defined = true;
                p_Main->eventSetButtonLabel ("XmodemButtonXml", "XMODEM");
            }
        break;
            
        case INFO_START_CHIP8:
            chip8mainLoop_ = additionalAddress;
            activateElfOsChip8();
        break;

        case INFO_CV_TAPE_PLAY:
            if (p_Main->getHwTapeState() == HW_TAPE_STATE_RW)
                return;
            if (readMem(scratchpadRegister_[programCounter_]) == 0x7b)
                p_Main->eventHwTapeStateChange(HW_TAPE_STATE_PLAY);
        break;
            
        case INFO_CV_TAPE_FF:
            if (p_Main->getHwTapeState() == HW_TAPE_STATE_RW)
                return;
            if (readMem(scratchpadRegister_[programCounter_]) == 0x7b)
                p_Main->eventHwTapeStateChange(HW_TAPE_STATE_FF);
        break;
            
        case INFO_CV_TAPE_REC:
            if (p_Main->getHwTapeState() == HW_TAPE_STATE_RW)
                return;
            if (readMem(scratchpadRegister_[programCounter_]) == 0x7b)
                p_Main->eventHwTapeStateChange(HW_TAPE_STATE_REC);
        break;

        case INFO_FRED_TAPE_REC:
            if (tapeActivated_)
            {
                p_Computer->stopTape();
                tapeActivated_ = false;
            }
            if (!tapeRecording_)
                p_Main->startCassetteSave(0);
        
            tapeRecording_ = true;
        break;
            
        case INFO_CHECK_SW:
            specifiedSoftwareLoaded_ = true;
        break;
            
        case INFO_STUDIO_BUILD_IN_GAME:
            if (specifiedSoftwareLoaded_)
            {
                switch (additionalAddress)
                {
                    case 1:
                        p_Main->loadKeyDefinition("", "studio2buildin1", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_);
                    break;

                    case 2:
                        p_Main->loadKeyDefinition("", "studio2buildin2", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_);
                    break;

                    case 3:
                        p_Main->loadKeyDefinition("", "studio2buildin3", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_);
                    break;

                    case 4:
                        p_Main->loadKeyDefinition("", "studio2buildin4", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_);
                    break;

                    case 5:
                        p_Main->loadKeyDefinition("", "studio2buildin5", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_);
                    break;
                }
                if (currentComputerConfiguration.keyLatchConfiguration[1].defined)
                    latchKeyPointer[1]->reDefineHexKeys(keyDefA1_, keyDefA2_, simDefA2_);
                if (currentComputerConfiguration.keyLatchConfiguration[2].defined)
                    latchKeyPointer[2]->reDefineHexKeys(keyDefB1_, keyDefB2_, simDefB2_);
            }
        break;
            
        case INFO_VICTORY_BUILD_IN_GAME:
            if (specifiedSoftwareLoaded_)
            {
                switch (additionalAddress)
                {
                    case 1:
                        p_Main->loadKeyDefinition("", "victorygrandpack1", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_);
                    break;

                    case 2:
                        p_Main->loadKeyDefinition("", "victorygrandpack2", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_);
                    break;

                    case 3:
                        p_Main->loadKeyDefinition("", "victorygrandpack3", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_);
                    break;

                    case 4:
                        p_Main->loadKeyDefinition("", "victorygrandpack4", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_);
                    break;

                    case 5:
                        p_Main->loadKeyDefinition("", "victorygrandpack5", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_);
                    break;
                }
                if (currentComputerConfiguration.keyLatchConfiguration[1].defined)
                    latchKeyPointer[1]->reDefineHexKeys(keyDefA1_, keyDefA2_, simDefA2_);
                if (currentComputerConfiguration.keyLatchConfiguration[2].defined)
                    latchKeyPointer[2]->reDefineHexKeys(keyDefB1_, keyDefB2_, simDefB2_);
            }
        break;

        case INFO_VISICOM_BUILD_IN_GAME:
            if (specifiedSoftwareLoaded_)
            {
                switch (additionalAddress)
                {
                    case 1:
                        p_Main->loadKeyDefinition("", "visicombuildin1", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_);
                    break;

                    case 2:
                        p_Main->loadKeyDefinition("", "visicombuildin2", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_);
                    break;

                    case 3:
                        p_Main->loadKeyDefinition("", "visicombuildin3", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_);
                    break;

                    case 4:
                        p_Main->loadKeyDefinition("", "visicombuildin4", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_);
                    break;

                    case 7:
                        p_Main->loadKeyDefinition("", "visicombuildin7", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_);
                    break;
                }
                if (currentComputerConfiguration.keyLatchConfiguration[1].defined)
                    latchKeyPointer[1]->reDefineHexKeys(keyDefA1_, keyDefA2_, simDefA2_);
                if (currentComputerConfiguration.keyLatchConfiguration[2].defined)
                    latchKeyPointer[2]->reDefineHexKeys(keyDefB1_, keyDefB2_, simDefB2_);
            }
        break;

        case INFO_SET_PSEUDO_AM4KBAS2020:
            pseudoType_ = "AM4KBAS2020";
            chip8mainLoop_ = additionalAddress;
            p_Main->forcePseudoDefinition(pseudoType_, "am2020bas.syntax", "AM4KBAS 2020");
        break;

        case INFO_SET_PSEUDO_AM4KBAS1978:
            pseudoType_ = "AM4KBAS1978";
            chip8mainLoop_ = additionalAddress;
            p_Main->forcePseudoDefinition(pseudoType_, "am4kbas.syntax", "AM4KBAS 1978");
        break;

    }
}

bool Computer::checkKeyInputAddress(Word address)
{
    for (std::vector<Word>::iterator i = currentComputerConfiguration.addressLocationConfiguration.keyInputAddress.begin (); i != currentComputerConfiguration.addressLocationConfiguration.keyInputAddress.end (); ++i)
    {
        if (address == *i)
            return true;
    }
    return false;
}

bool Computer::checkKeyInputAddress()
{
    return checkKeyInputAddress(scratchpadRegister_[programCounter_]);
}

void Computer::startComputerRun(bool load)
{
    if (currentComputerConfiguration.asciiKeyboardConfiguration.defined)
        startElfRun(load, currentComputerConfiguration.stateCommand_[elfRunState_]);
    if (currentComputerConfiguration.cdp1871Configuration.defined)
        start1871Run(load, currentComputerConfiguration.stateCommand_[elfRunState_]);
    for (int pad=0; pad<lastLatchKeyPad_; pad++)
    {
        if (currentComputerConfiguration.keyLatchConfiguration[pad].defined)
            latchKeyPointer[pad]->startLatchRun(load, currentComputerConfiguration.stateCommand_[elfRunState_]);
    }
    if (currentComputerConfiguration.matrixKeyboardConfiguration.defined)
        matrixKeyboardPointer->startRun(load, currentComputerConfiguration.stateCommand_[elfRunState_]);

    if (currentComputerConfiguration.videoTerminalConfiguration.type != VTNONE)
    {
        if (cpuMode_ != RUN)
        {
            onInButtonPress(currentComputerConfiguration.mainFrontPanelConfiguration.dataSwitchButtons);
            onRun();
            vtPointer->startXmlRun(load, currentComputerConfiguration.stateCommand_[RESETSTATE]);
        }
        else
            vtPointer->startXmlRun(load, currentComputerConfiguration.stateCommand_[elfRunState_]);
    }
}

void Computer::ctrlvText(wxString text)
{
    ctrlvTextStr_ = text;
    ctrlvTextCharNum_ = 1;
}

void Computer::ctrlvTextXml(wxString text)
{
    if (currentComputerConfiguration.asciiKeyboardConfiguration.defined)
        startCtrlV(text);
    if (currentComputerConfiguration.cdp1871Configuration.defined)
        start1871CtrlV(text);
    for (int pad=0; pad<lastLatchKeyPad_; pad++)
    {
        if (currentComputerConfiguration.keyLatchConfiguration[pad].defined)
            latchKeyPointer[pad]->startCtrlV(text);
    }
    if (currentComputerConfiguration.matrixKeyboardConfiguration.defined)
        matrixKeyboardPointer->startCtrlV(text);
    if (currentComputerConfiguration.gpioPs2KeyboardConfiguration.defined)
        p_Computer->ctrlvText(text);

    if (currentComputerConfiguration.videoTerminalConfiguration.type != VTNONE)
        p_Computer->ctrlvText(text);
}

int Computer::getCtrlvChar()
{
    return getCtrlvChar(true);
}

int Computer::getCtrlvChar(bool increase)
{
    int character = 0;
    
    if (ctrlvTextCharNum_ <= ctrlvTextStr_.Len())
    {
        character = ctrlvTextStr_.GetChar(ctrlvTextCharNum_ - 1);
        if (increase)
            ctrlvTextCharNum_++;
    }
    else
        ctrlvTextCharNum_ = 0;
    
    return character;
}

int Computer::getCtrlvCharTmc()
{
    int character = 0;
    
    if (ctrlvTextCharNum_ <= (ctrlvTextStr_.Len() + 3))
    {
        character = ctrlvTextStr_.GetChar(ctrlvTextCharNum_ - 4);
        ctrlvTextCharNum_++;
    }
    else
        ctrlvTextCharNum_ = 0;
    
    return character;
}

void Computer::ctrlvTextCharNumPlusOne()
{
    ctrlvTextCharNum_++;
}

bool Computer::isComputerRunning()
{
    if (elfRunState_ == RUNSTATE)
        return true;
    else
        return false;
}

bool Computer::isFAndMBasicRunning()
{
    return fAndMBasicRunning_;
}

void Computer::terminalSave(wxString fileName, int protocol)
{
    if (currentComputerConfiguration.videoTerminalConfiguration.type != VTNONE)
        vtPointer->terminalSaveVt(fileName, protocol);
}

void Computer::terminalYsSave(wxString fileName, int protocol)
{
    if (currentComputerConfiguration.videoTerminalConfiguration.type != VTNONE)
        vtPointer->terminalYsSaveVt(fileName, protocol);
}

void Computer::terminalLoad(wxString filePath, wxString fileName, int protocol)
{
    if (currentComputerConfiguration.videoTerminalConfiguration.type != VTNONE)
        vtPointer->terminalLoadVt(filePath, protocol);
}

void Computer::terminalStop()
{
    if (currentComputerConfiguration.videoTerminalConfiguration.type != VTNONE)
        vtPointer->terminalStopVt();
}

void Computer::setDivider(Byte value)
{
    const int freq [16] = {0, 256, 128, 8192, 4096, 2048, 1024, 512, 256, 128, 64, 32, 16, 8, 4, 2};

    if (value == 0)
        cycleSize_ = -1;
    else
        cycleSize_ = (int) (((computerClockSpeed_ * 1000000) / 8) / freq [value]);
    cycleValue_ = cycleSize_;
}

void Computer::thrStatusVt100(bool data)
{
    vtPointer->thrStatusUart16450(data);
}

void Computer::thrStatusSerial(bool data)
{
    p_Serial->thrStatusUart16450(data);
}

void Computer::saveRtc()
{
    if (!currentComputerConfiguration.rtcDs12887Configuration.defined)
        return;

    Byte value;
    wxFile outputFile;

    wxString fileName = p_Main->getMainDir()+"rtcdump.bin";

    if (wxFile::Exists(fileName))
        outputFile.Open(fileName, wxFile::write);
    else
        outputFile.Create(fileName);
    for (int address = 0; address < 128; address++)
    {
        value = rtcRam_[address];
        outputFile.Write(&value, 1);
    }
    outputFile.Close();
}

void Computer::loadRtc()
{
    currentComputerConfiguration = p_Main->getConfiguration();

    if (!currentComputerConfiguration.rtcDs12887Configuration.defined)
        return;

    if (currentComputerConfiguration.clearRtc)
    {
        for (int i = 0; i<128; i++)
        {
            rtcRam_[i] = 0;
            p_Main->updateDebugMemory(i);
        }
        rtcRam_[0xa] = 0x20;
        rtcRam_[0xb] = 0x6;
        rtcRam_[0xc] = 0;
        rtcRam_[0xd] = 0x80;
        currentComputerConfiguration.clearRtc = false;
        p_Main->setConfiguration(currentComputerConfiguration);
        p_Main->eventSetCheckBox("XmlClearRtc", false);
        return;
    }
    
    wxFFile inFile;
    size_t length;
    char buffer[128];

    if (wxFile::Exists(p_Main->getMainDir()+"rtcdump.bin"))
    {
        if (inFile.Open(p_Main->getMainDir()+"rtcdump.bin", _("rb")))
        {
            length = inFile.Read(buffer, 128);
            for (size_t i=0; i<length; i++)
            {
                rtcRam_[i] = (Byte)buffer[i];
                p_Main->updateDebugMemory(i);
            }
            inFile.Close();
        }
    }
}

Byte Computer::readDirectRtc(Word address)
{
    return rtcRam_[address];
}

void Computer::writeDirectRtc(Word address, Byte value)
{
    rtcRam_[address] = value;
}

void Computer::setDosFileName()
{
    wxString name;
    name = "";
    Word address = 0xbc03;
    while ((mainMemory_[address] != 0xff) && (address < 0xbc15))
    {
        name = name + (char)mainMemory_[address];
        address++;
    }
    if (name[0] == 0 && name[1] == 0)
        return;

    p_Main->eventSetSwName(name);
    p_Main->setPloadFileName(name + p_Main->getRunningComputerPloadExtension());
    if (mainMemory_[0xbc17] == 2)
    {
        Word saveStart, saveEnd, saveExec;

        saveStart = mainMemory_[0xbc29] << 8;
        saveEnd = saveStart + (mainMemory_[0xbc25] << 8) + mainMemory_[0xbc26] - 4;
        saveExec = mainMemory_[0xbc29] << 8;
        p_Main->eventSetLocation(true, saveStart, saveEnd, saveExec);
    }
    else
        p_Main->eventSetLocation(false, 0, 0, 0);
}

void Computer::setSelectedSlot(int slot)
{
    if (slot < currentComputerConfiguration.slotConfiguration.maxSlotNumber_)
        selectedSlot_ = slot;
}

void Computer::setSelectedBank(int bank)
{
    if (bank < currentComputerConfiguration.slotConfiguration.slotInfo[selectedSlot_].maxBankNumber_)
        selectedBank_ = bank;
}

void Computer::cidelsaStatusBarDown(int keycode)
{
    switch(keycode)
    {
        case 'A':        // Coin shute A
        case 'B':        // Coin shute B
            for (int ef=1; ef <=4; ef++)
            {
                if (currentComputerConfiguration.efButtonsConfiguration.key[ef].defined)
                {
                    if (currentComputerConfiguration.efButtonsConfiguration.key[ef].value == keycode)
                        efKeyValue[ef] = 0;
                }
            }
        break;

        case '1':            // 1 Player
        case '2':            // 2 Players
            if (currentComputerConfiguration.bitKeypadConfiguration[0].defined)
                bitkeypadPointer[0]->keyDown(keycode);
        break;

        case WXK_SPACE:        // Fire
            if (currentComputerConfiguration.bitKeypadConfiguration[0].defined)
                bitkeypadPointer[0]->keyDown(keycode);
            if (currentComputerConfiguration.vis1870Configuration.defined)
                vis1870Pointer->focus();
        break;
    }
}

void Computer::cidelsaStatusBarUp(int keycode)
{
    switch(keycode)
    {
        case 'B':        // Coin shute B
        case 'A':        // Coin shute A
            for (int ef=1; ef <=4; ef++)
            {
                if (currentComputerConfiguration.efButtonsConfiguration.key[ef].defined)
                {
                    if (currentComputerConfiguration.efButtonsConfiguration.key[ef].value == keycode)
                        efKeyValue[ef] = 1;
                }
            }
            if (currentComputerConfiguration.vis1870Configuration.defined)
                vis1870Pointer->focus();
        break;

        case '1':            // 1 Player
        case '2':            // 2 Players
        case WXK_SPACE:        // Fire
            if (currentComputerConfiguration.bitKeypadConfiguration[0].defined)
                bitkeypadPointer[0]->keyUp(keycode);
            if (currentComputerConfiguration.vis1870Configuration.defined)
                vis1870Pointer->focus();
        break;
    }
}

void Computer::setBatchFileNumber(int number)
{
    batchFileNumber_ = number;
    batchInProgress_ = false;
        
    p_Main->eventSetConvertState(false);
}

void Computer::batchConvert()
{
    wxString memAccessExtension = "." + p_Main->getRunningComputerPloadExtension();

    if (getBatchFile(memAccessExtension))
    {
        batchInProgress_ = true;
        
        p_Main->setPloadFullFileName(p_Main->getBatchPath(batchFileNumber_+1) + p_Main->getBatchFile(batchFileNumber_+1));
        
        p_Main->pload();
        
        wxString filename = p_Main->getBatchFile(batchFileNumber_+1);
        filename = filename.Left(filename.Len()-memAccessExtension.Len()) + ".wav";
        
        p_Main->setWaveDir(p_Main->getBatchPath(batchFileNumber_+1));
        p_Main->setWaveFile(filename);
        
        startComputerRun(false);
//        load_ = false;
//        commandText_ = "psave";
//        comxRunCommand_ = 3;
    }
}

bool Computer::getBatchFile(wxString memAccessExtension)
{
    wxString extension = "xxxxx";
    wxString filename, path;
    bool result = true;
    
    while (extension != memAccessExtension && batchFileNumber_ >= 0)
    {
        filename = p_Main->getBatchFile(batchFileNumber_);
//        path = p_Main->getBatchPath(batchFileNumber_);
        extension = filename.Right(memAccessExtension.Len());
        batchFileNumber_--;
    }
    
    if (extension != memAccessExtension )
    {
        result = false;
        p_Main->batchConvertStop();
        p_Main->eventSetConvertState(true);
    }

    return result;
}

void Computer::closeKeyFile()
{
    if (currentComputerConfiguration.asciiKeyboardConfiguration.defined)
        closeElfKeyFile();
    if (currentComputerConfiguration.cdp1871Configuration.defined)
        close1871KeyFile();
    for (int pad=0; pad<lastLatchKeyPad_; pad++)
    {
        if (currentComputerConfiguration.keyLatchConfiguration[pad].defined)
            latchKeyPointer[pad]->closeKeyFile();
    }
    if (currentComputerConfiguration.matrixKeyboardConfiguration.defined)
        matrixKeyboardPointer->closeKeyFile();
    if (currentComputerConfiguration.gpioPs2KeyboardConfiguration.defined)
        closePs2gpioKeyFile();
}

void Computer::resetV1870VideoModeEf()
{
    if (p_Computer->getFlipFlopQ() || (currentComputerConfiguration.videoTerminalConfiguration.reverseQ == 1))
        currentComputerConfiguration.vis1870Configuration.useVideoModeEf = false;
}

void Computer::activateElfOsChip8()
{
    pseudoType_ = p_Main->getPseudoDefinition(&chip8baseVar_, &chip8mainLoop_, &chip8register12bit_, &pseudoLoaded_);
    
    Word address = scratchpadRegister_[0xA];
    
    fetchFileName(address, 100);
}

void Computer::fetchFileName(Word address, size_t length)
{
    wxString fileName = "";
    Byte MemoryValue = readMem(address++);
    
    while (MemoryValue != 0 && length > 0)
    {
        if (MemoryValue >= 32 && MemoryValue < 128)
            fileName += wxString::FromAscii(MemoryValue);
        MemoryValue = readMem(address++);
        length--;
    }
    fileName.MakeLower();
    fileName.Trim();
    
    reLoadKeyDefinition(fileName);
    p_Main->eventSetSwName(fileName);
}

void Computer::startLoad(int tapeNumber, bool button)
{
    if ((tapeRunSwitch_&1) & !button)
        return;
    
    lastSampleInt16_ = 0;
    lastSampleChar_ = 0;
    lastSampleInt32_ = 0;
    pulseCount_ = 0;
    tapeInput_ = 0;
    polarity_ = 0;
    silenceCount_ = 0;
    bitNumber_ = -1;
    tapeError_ = 1;

    pulseCountStopTone_ = 2000;
    tapeFormat56_ = false;
    tapeFormatFixed_ = false;
    toneTime_ = 0;
    pauseTapeCounter_ = 0;

    switch (currentComputerConfiguration.hwTapeConfiguration.format)
    {
        case TAPE_FORMAT_CV:
            if (tapeActivated_ || tapeRecording_)
            {
                p_Main->turboOn();
                
                switch (p_Main->getHwTapeState())
                {
                    case HW_TAPE_STATE_FF:
                        p_Computer->forwardTape(TAPE_FF);
                    break;

                    case HW_TAPE_STATE_RW:
                        p_Computer->rewindTape(TAPE_RW);
                    break;

                    default:
                        p_Computer->restartTapeLoad(TAPE_PLAY);
                    break;
                }
            }
            else
            {
                if (button)
                    tapeActivated_ = p_Main->startLoad(tapeNumber);
                else
                    tapeActivated_ = p_Main->startCassetteLoad(tapeNumber);
                
                if (!tapeActivated_)
                    p_Main->eventHwTapeStateChange(HW_TAPE_STATE_OFF);
            }
        break;
            
        default:
            if (tapeActivated_)
            {
                p_Main->turboOn();
                p_Computer->restartTapeLoad(TAPE_PLAY);
            }
            else
            {
                if (button)
                    tapeActivated_ = p_Main->startLoad(0);
                else
                    tapeActivated_ = p_Main->startCassetteLoad(0);
            }
            
            tapeRunSwitch_ = tapeRunSwitch_ | 1;
        break;
    }
}

void Computer::cassette(wxInt32 val)
{
    if (conversionTypeWav_ == 0)
    {
        if (val <= 0)
        {
            cassetteEf_ = tapePolarity_; // 0
            maxTapeInputInt32_ = 0;
        }
        else
        {
            cassetteEf_ = !tapePolarity_; //1
            if (val > lastTapeInputInt32_)
                maxTapeInputInt32_ = val;
            else
                gaugeValue_ = maxTapeInputInt32_ / 5;
        }
    }
    else
    {
        if (cassetteEf_ != tapePolarity_)
        {
            if (val > lastTapeInputInt32_)
                maxTapeInputInt32_ = val;
            else
            {
                gaugeValue_ = maxTapeInputInt32_ / 5;
                cassetteEf_ = tapePolarity_; // 0
            }
        }
        else
        {
            if (val < lastTapeInputInt32_)
                maxTapeInputInt32_ = -val;
            else
            {
                gaugeValue_ = maxTapeInputInt32_ / 5;
                cassetteEf_ = !tapePolarity_; //1
            }
        }
    }
    lastTapeInputInt32_ = val;
    
    if (cassetteEf_ != oldCassetteEf_)
    {
        oldCassetteEf_ = cassetteEf_;

        if (ledTimeMs_ == 0)
            p_Main->eventUpdateVipIILedStatus(BAR_LED_TAPE, cassetteEf_ != 0);
        else
            updateTapeLed_ = true;
    }
}

void Computer::cassette(wxInt16 val)
{
    if (conversionTypeWav_ == 0)
    {
        if (val <= 0)
        {
            cassetteEf_ = tapePolarity_; // 0
            maxTapeInputInt16_ = 0;
        }
        else
        {
            cassetteEf_ = !tapePolarity_; //1
            if (val > lastTapeInputInt16_)
                maxTapeInputInt16_ = val;
            else
                gaugeValue_ = maxTapeInputInt16_ / 5;
        }
    }
    else
    {
        if (cassetteEf_ != tapePolarity_)
        {
            if (val > lastTapeInputInt16_)
                maxTapeInputInt16_ = val;
            else
            {
                gaugeValue_ = maxTapeInputInt16_ / 5;
                cassetteEf_ = tapePolarity_; // 0
            }
        }
        else
        {
            if (val < lastTapeInputInt16_)
                maxTapeInputInt16_ = -val;
            else
            {
                gaugeValue_ = maxTapeInputInt16_ / 5;
                cassetteEf_ = !tapePolarity_; //1
            }
        }
    }
    lastTapeInputInt16_ = val;

    if (cassetteEf_ != oldCassetteEf_)
    {
        oldCassetteEf_ = cassetteEf_;

        if (ledTimeMs_ == 0)
            p_Main->eventUpdateVipIILedStatus(BAR_LED_TAPE, cassetteEf_ != 0);
        else
            updateTapeLed_ = true;
    }
}

void Computer::cassette(char val)
{
    if (conversionTypeWav_ == 0)
    {
        if (val < 0)
        {
            cassetteEf_ = tapePolarity_; // 0
            maxTapeInputChar_ = 0;
        }
        else
        {
            cassetteEf_ = !tapePolarity_; //1
            if (val > lastTapeInputChar_)
                maxTapeInputChar_ = val;
            else
                gaugeValue_ = maxTapeInputChar_ * 45;
        }
    }
    else
    {
        if (cassetteEf_ != tapePolarity_)
        {
            if (val > lastTapeInputChar_)
                maxTapeInputChar_ = val;
            else
            {
                gaugeValue_ = maxTapeInputChar_ * 45;
                cassetteEf_ = tapePolarity_; // 0
            }
        }
        else
        {
            if (val < lastTapeInputChar_)
                maxTapeInputChar_ = -val;
            else
            {
                gaugeValue_ = maxTapeInputChar_ * 45;
                cassetteEf_ = !tapePolarity_; //1
            }
        }
    }
    lastTapeInputChar_ = val;

    if (cassetteEf_ != oldCassetteEf_)
    {
        oldCassetteEf_ = cassetteEf_;

        if (ledTimeMs_ == 0)
            p_Main->eventUpdateVipIILedStatus(BAR_LED_TAPE, cassetteEf_ != 0);
        else
            updateTapeLed_ = true;
    }
}

void Computer::realCassette(short val)
{
    if (conversionType_ == 0)
    {
        if (val <= 0)
        {
            cassetteEf_ = tapePolarity_; // 0
            maxTapeInputInt16_ = 0;
        }
        else
        {
            cassetteEf_ = !tapePolarity_; //1
            if (val > lastTapeInputInt16_)
                maxTapeInputInt16_ = val;
            else
                gaugeValue_ = maxTapeInputInt16_;
        }
    }
    else
    {
        if (cassetteEf_ != tapePolarity_)
        {
            if (val > lastTapeInputInt16_)
                maxTapeInputInt16_ = val;
            else
            {
                gaugeValue_ = maxTapeInputInt16_;
                cassetteEf_ = tapePolarity_; // 0
            }
        }
        else
        {
            if (val < lastTapeInputInt16_)
                maxTapeInputInt16_ = -val;
            else
            {
                gaugeValue_ = maxTapeInputInt16_;
                cassetteEf_ = !tapePolarity_; //1
            }
        }
    }
    lastTapeInputInt16_ = val;
}

void Computer::cassetteXmlHw(wxInt32 val, long size)
{
    if ((tapeRunSwitch_&1) != 1 && currentComputerConfiguration.hwTapeConfiguration.format != TAPE_FORMAT_CV)
        return;
    
    wxInt32 tape_threshold;
    if (useXmlThreshold_)
        tape_threshold = currentComputerConfiguration.hwTapeConfiguration.threshold24Bit;
    else
        tape_threshold = threshold24_;

    stepCassetteCounter(size);
    
    if (!currentComputerConfiguration.hwTapeConfiguration.defined)
        return;
    
    wxInt64 difference;
    if (val < lastSampleInt32_)
        difference = lastSampleInt32_ - val;
    else
        difference = val - lastSampleInt32_;
    
    if (difference < tape_threshold)
        silenceCount_++;
    else
    {
        silenceCount_ = 0;
        toneTime_++;
    }
    
    switch (currentComputerConfiguration.hwTapeConfiguration.format)
    {
        case TAPE_FORMAT_AUTO:
        case TAPE_FORMAT_PM:
        case TAPE_FORMAT_56:
            if (lastSampleInt32_ <= 0)
            {
                if (val > 0 && silenceCount_ == 0)
                    pulseCount_++;
            }
            else
            {
                if (val < 0 && silenceCount_ == 0)
                    pulseCount_++;
            }
            
            if (pulseCount_ > pulseCountStopTone_ && silenceCount_ > 10 && currentComputerConfiguration.hwTapeConfiguration.stopTone)
            {
                p_Computer->pauseTape();
                p_Main->turboOff();
                tapeRunSwitch_ = tapeRunSwitch_ & 2;
            }
        break;
            
        case TAPE_FORMAT_CV:
            if (pauseTapeCounter_ > 0)
            {
                pauseTapeCounter_--;
                if (pauseTapeCounter_ == 0)
                    pauseTape();
            }

            if (lastSampleInt32_ <= 0)
            {
                if (val > 0 && silenceCount_ == 0)
                {
                    pulseCount_++;
                    cassetteCyberVision();
                }
            }
            if (silenceCount_ > 10)
                startBytes_ = 2;
        break;
    }

    if (!tapeFormatFixed_)
    {
        switch (currentComputerConfiguration.hwTapeConfiguration.format)
        {
            case TAPE_FORMAT_PM:
                tapeFormatFixed_ = true;
                cassettePm();
            break;
                
            case TAPE_FORMAT_56:
                pulseCountStopTone_ = 2000;
                tapeFormatFixed_ = true;
                tapeFormat56_ = true;
                cassette56();
            break;
                
            case TAPE_FORMAT_CV:
            break;

            default:
                if (pulseCount_ > 50 && pulseCount_ < 200 && silenceCount_ > 10)
                { // 5.2 & 6.2 tone format, if no tone is detected between 50 & 200 pulses at the start it is PM System format
                    pulseCountStopTone_ = 2000;
                    tapeFormatFixed_ = true;
                    tapeFormat56_ = true;
    //                if (computerType_ == FRED1)
    //                    p_Main->eventSetStaticTextValue("CurrentTapeFormatTextFRED1", "-> 5.2/6.2 Tone");
    //                else
    //                    p_Main->eventSetStaticTextValue("CurrentTapeFormatTextFRED1_5", "-> 5.2/6.2 Tone");
                }
                if (tapeFormat56_)
                    cassette56();
                else
                    cassettePm();
            break;
        }
    }
    else
    {
        if (tapeFormat56_)
            cassette56();
        else
            cassettePm();
    }
    
    lastSampleInt32_ = val;
}

void Computer::cassetteXmlHw(wxInt16 val, long size)
{
    if ((tapeRunSwitch_&1) != 1 && currentComputerConfiguration.hwTapeConfiguration.format != TAPE_FORMAT_CV)
        return;
    
    wxInt16 tape_threshold;
    if (useXmlThreshold_)
        tape_threshold = currentComputerConfiguration.hwTapeConfiguration.threshold16Bit;
    else
        tape_threshold = threshold16_;

    stepCassetteCounter(size);
    
    if (!currentComputerConfiguration.hwTapeConfiguration.defined)
        return;
    
    wxInt32 difference;
    if (val < lastSampleInt16_)
        difference = lastSampleInt16_ - val;
    else
        difference = val - lastSampleInt16_;
    
    if (difference < tape_threshold)
        silenceCount_++;
    else
    {
        silenceCount_ = 0;
        toneTime_++;
    }
    
    switch (currentComputerConfiguration.hwTapeConfiguration.format)
    {
        case TAPE_FORMAT_AUTO:
        case TAPE_FORMAT_PM:
        case TAPE_FORMAT_56:
            if (lastSampleInt16_ <= 0)
            {
                if (val > 0 && silenceCount_ == 0)
                    pulseCount_++;
            }
            else
            {
                if (val < 0 && silenceCount_ == 0)
                    pulseCount_++;
            }
            
            if (pulseCount_ > pulseCountStopTone_ && silenceCount_ > 10 && currentComputerConfiguration.hwTapeConfiguration.stopTone)
            {
                p_Computer->pauseTape();
                p_Main->turboOff();
                tapeRunSwitch_ = tapeRunSwitch_ & 2;
            }
        break;
            
        case TAPE_FORMAT_CV:
            if (pauseTapeCounter_ > 0)
            {
                pauseTapeCounter_--;
                if (pauseTapeCounter_ == 0)
                    pauseTape();
            }

            if (lastSampleInt16_ <= 0)
            {
                if (val > 0 && silenceCount_ == 0)
                {
                    pulseCount_++;
                    cassetteCyberVision();
                }
            }
            if (silenceCount_ > 10)
                startBytes_ = 2;
        break;
    }

    if (!tapeFormatFixed_)
    {
        switch (currentComputerConfiguration.hwTapeConfiguration.format)
        {
            case TAPE_FORMAT_PM:
                tapeFormatFixed_ = true;
                cassettePm();
            break;
                
            case TAPE_FORMAT_56:
                pulseCountStopTone_ = 2000;
                tapeFormatFixed_ = true;
                tapeFormat56_ = true;
                cassette56();
            break;
                
            case TAPE_FORMAT_CV:
            break;

            default:
                if (pulseCount_ > 50 && pulseCount_ < 200 && silenceCount_ > 10)
                { // 5.2 & 6.2 tone format, if no tone is detected between 50 & 200 pulses at the start it is PM System format
                    pulseCountStopTone_ = 2000;
                    tapeFormatFixed_ = true;
                    tapeFormat56_ = true;
    //                if (computerType_ == FRED1)
    //                    p_Main->eventSetStaticTextValue("CurrentTapeFormatTextFRED1", "-> 5.2/6.2 Tone");
    //                else
    //                    p_Main->eventSetStaticTextValue("CurrentTapeFormatTextFRED1_5", "-> 5.2/6.2 Tone");
                }
                if (tapeFormat56_)
                    cassette56();
                else
                    cassettePm();
            break;
        }
    }
    else
    {
        if (tapeFormat56_)
            cassette56();
        else
            cassettePm();
    }

    lastSampleInt16_ = val;
}

void Computer::cassetteXmlHw(char val, long size)
{
    if ((tapeRunSwitch_&1) != 1 && currentComputerConfiguration.hwTapeConfiguration.format != TAPE_FORMAT_CV)
        return;
    
    char tape_threshold;
    if (useXmlThreshold_)
        tape_threshold = currentComputerConfiguration.hwTapeConfiguration.threshold8Bit;
    else
        tape_threshold = threshold8_;
        
    stepCassetteCounter(size);

    if (!currentComputerConfiguration.hwTapeConfiguration.defined)
        return;

    int difference;
    if (val < lastSampleChar_)
        difference = lastSampleChar_ - val;
    else
        difference = val - lastSampleChar_;
    
    if (difference < tape_threshold)
        silenceCount_++;
    else
    {
        silenceCount_ = 0;
        toneTime_++;
    }

    switch (currentComputerConfiguration.hwTapeConfiguration.format)
    {
        case TAPE_FORMAT_AUTO:
        case TAPE_FORMAT_PM:
        case TAPE_FORMAT_56:
            if (lastSampleChar_ <= 0)
            {
                if (val > 0 && silenceCount_ == 0)
                    pulseCount_++;
            }
            else
            {
                if (val < 0 && silenceCount_ == 0)
                    pulseCount_++;
            }
            
            if (pulseCount_ > pulseCountStopTone_ && silenceCount_ > 10 && currentComputerConfiguration.hwTapeConfiguration.stopTone)
            {
                p_Computer->pauseTape();
                p_Main->turboOff();
                tapeRunSwitch_ = tapeRunSwitch_ & 2;
            }
        break;
            
        case TAPE_FORMAT_CV:
            if (pauseTapeCounter_ > 0)
            {
                pauseTapeCounter_--;
                if (pauseTapeCounter_ == 0)
                    pauseTape();
            }

            if (lastSampleChar_ <= 0)
            {
                if (val > 0 && silenceCount_ == 0)
                {
                    pulseCount_++;
                    cassetteCyberVision();
                }
            }
            if (silenceCount_ > 10)
                startBytes_ = 2;
        break;
    }

    if (!tapeFormatFixed_)
    {
        switch (currentComputerConfiguration.hwTapeConfiguration.format)
        {
            case TAPE_FORMAT_PM:
                tapeFormatFixed_ = true;
                cassettePm();
            break;
                
            case TAPE_FORMAT_56:
                pulseCountStopTone_ = 2000;
                tapeFormatFixed_ = true;
                tapeFormat56_ = true;
                cassette56();
            break;
                
            case TAPE_FORMAT_CV:
            break;

            default:
                if (pulseCount_ > 50 && pulseCount_ < 200 && silenceCount_ > 10)
                { // 5.2 & 6.2 tone format, if no tone is detected between 50 & 200 pulses at the start it is PM System format
                    pulseCountStopTone_ = 2000;
                    tapeFormatFixed_ = true;
                    tapeFormat56_ = true;
    //                if (computerType_ == FRED1)
    //                    p_Main->eventSetStaticTextValue("CurrentTapeFormatTextFRED1", "-> 5.2/6.2 Tone");
    //                else
    //                    p_Main->eventSetStaticTextValue("CurrentTapeFormatTextFRED1_5", "-> 5.2/6.2 Tone");
                }
                if (tapeFormat56_)
                    cassette56();
                else
                    cassettePm();
            break;
        }
    }
    else
    {
        if (tapeFormat56_)
            cassette56();
        else
            cassettePm();
    }

    lastSampleChar_ = val;
}

void Computer::stepCassetteCounter(long step)
{
    tapeCounterStep_+=step;
    if (tapeCounterStep_ < 0)
        tapeCounterStep_ = 0;
    
    int stepSize=1000;
    
    if (p_Main->isTurboOn())
        stepSize = 100;
        
    if (p_Main->getHwTapeState() == HW_TAPE_STATE_FF || p_Main->getHwTapeState() == HW_TAPE_STATE_RW)
        stepSize = 100;

    if (p_Main->isTurboOn() && (p_Main->getHwTapeState() == HW_TAPE_STATE_FF || p_Main->getHwTapeState() == HW_TAPE_STATE_RW))
        stepSize = 1;

    long long newPeriod = tapeCounterStep_*stepSize/sampleRate_;
    
    if (tapePeriod_ != newPeriod)
    {
        tapePeriod_ = newPeriod;
        int msec = tapePeriod_ % stepSize;
        int sec = (int) ((tapePeriod_/stepSize) % 60);
        int min = (int) ((tapePeriod_/stepSize) / 60);
        
        int numberOfSeconds = min*60+sec;
        int numberOfSecondsRewind = 30*60-numberOfSeconds;
        if (numberOfSecondsRewind < 0)
            numberOfSecondsRewind = 0;
        
        if (numberOfSeconds != lastSec_ && (p_Main->getHwTapeState() == HW_TAPE_STATE_FF || p_Main->getHwTapeState() == HW_TAPE_STATE_RW))
        {
            int cva = p_Main->getPsaveData(11);
            float cvb = (float) p_Main->getPsaveData(12)/100;

            remainingForwardSpeed_ += sqrt(cva*cva+2*cvb*numberOfSeconds);
            forwardSpeed_ = (int)remainingForwardSpeed_;
            remainingForwardSpeed_ -= forwardSpeed_;
            
            remainingRewindSpeed_ += sqrt(cva*cva+2*cvb*numberOfSecondsRewind);
            rewindSpeed_ = (int)remainingRewindSpeed_;
            remainingRewindSpeed_ -= rewindSpeed_;
        }

        switch (stepSize)
        {
            case 1:
                tapeCounter_.Printf("%02d:%02d     ",min,sec);
            break;

            case 100:
                tapeCounter_.Printf("%02d:%02d:%02d ",min,sec,msec);
            break;
                
            default:
                tapeCounter_.Printf("%02d:%02d:%03d",min,sec,msec);
            break;
        }
        p_Main->eventSetStaticTextValue("CasCounterXml", tapeCounter_);
    }

}

void Computer::cassetteCyberVision()
{
    float period = (float) (toneTime_ / pulseCount_) / sampleRate_;
    float freq = (float) 1 / period;
    
    if ((pulseCount_ == 1 && freq < currentComputerConfiguration.hwTapeConfiguration.frequencyBorder) || pulseCount_ == 2)  // && silenceCount_ > 10)
    {
        if (bitNumber_ == -1)
        {
            if (pulseCount_ == 1)
            {
                if (freq < currentComputerConfiguration.hwTapeConfiguration.frequencyBorder && currentComputerConfiguration.hwTapeConfiguration.startBit == 1)
                    bitNumber_++;
                if (freq >= currentComputerConfiguration.hwTapeConfiguration.frequencyBorder && currentComputerConfiguration.hwTapeConfiguration.startBit == 0)
                    bitNumber_++;
            }
        }
        else
        {
            if (currentComputerConfiguration.hwTapeConfiguration.stopBitIgnore)
            {
                if (bitNumber_ < currentComputerConfiguration.hwTapeConfiguration.dataBits)
                {
                    tapeInput_ = (tapeInput_ << 1) | (pulseCount_&1);
                    bitNumber_++;
                    
                    if (startBytes_ > 0 && tapeInput_ !=0)
                    {
                        tapeInput_ = 0;
                        bitNumber_ = -1;
                        startBytes_ = 2;
                    }
                }
                if (bitNumber_ == currentComputerConfiguration.hwTapeConfiguration.dataBits)
                {
                    lastTapeInpt_ = tapeInput_;
                    
                    if (startBytes_ > 0 && tapeInput_ == 0)
                        startBytes_--;
                    
                    tapedataReady_ = 0;
                    tapeInput_ = 0;
                    bitNumber_ = -1;
                }
            }
            else
            {
                if (bitNumber_ == currentComputerConfiguration.hwTapeConfiguration.dataBits)
                {
                    bool stopBit = false;;
                    if (freq < currentComputerConfiguration.hwTapeConfiguration.frequencyBorder && currentComputerConfiguration.hwTapeConfiguration.stopBit == 1)
                        stopBit = true;
                    if (freq >= currentComputerConfiguration.hwTapeConfiguration.frequencyBorder && currentComputerConfiguration.hwTapeConfiguration.stopBit == 0)
                        stopBit = true;

                    if (stopBit)
                    {
                        lastTapeInpt_ = tapeInput_;
                        
                        if (startBytes_ > 0 && tapeInput_ == 0)
                            startBytes_--;
                        
                        tapedataReady_ = 0;
                        tapeInput_ = 0;
                        bitNumber_ = -1;
                    }
                }
                else
                {
                    tapeInput_ = (tapeInput_ << 1) | (pulseCount_&1);
                    bitNumber_++;
                    
                    if (startBytes_ > 0 && tapeInput_ !=0)
                    {
                        tapeInput_ = 0;
                        bitNumber_ = -1;
                        startBytes_ = 2;
                    }
                }
            }
        }
        
        pulseCount_ = 0;
        toneTime_ = 0;
    }
}

void Computer::cassette56()
{
    float freq;
    wxString message;
    
    if (pulseCount_ > 50 && silenceCount_ > 10)
    {
        tapeFormatFixed_ = true;
        freq = (float) toneTime_ / (pulseCount_ - 1);
        if (bitNumber_ == currentComputerConfiguration.hwTapeConfiguration.dataBits)
        {
            if (freq <= currentComputerConfiguration.hwTapeConfiguration.frequencyBorder && (polarity_ & 1) != 1)
            {
                tapeError_ = 0;
                for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
                    panelPointer[frontPanel]->setErrorLed(1);
                
                if (inpMode_ == INP_MODE_TAPE_DIRECT)
                {
                    message.Printf("Polarity issue at %04X", scratchpadRegister_[0]);
                    p_Main->eventShowTextMessage(message);
                }
                if  (inpMode_ == INP_MODE_TAPE_PROGRAM)
                {
                    message.Printf("Polarity issue");
                    p_Main->eventShowTextMessage(message);
                }

                if (inpMode_ == INP_MODE_TAPE_DIRECT)
                    dmaIn(tapeInput_);
            }
            else
            {
                if (inpMode_ == INP_MODE_TAPE_DIRECT)
                    dmaIn(tapeInput_);
                if  (inpMode_ == INP_MODE_TAPE_PROGRAM)
                {
                    lastTapeInpt_ = tapeInput_;
                    tapedataReady_ = 0;
                }
            }
        }
        else
        {
            if (bitNumber_ != -1)
            {
                if (freq <= currentComputerConfiguration.hwTapeConfiguration.frequencyBorder)
                    tapeInput_ = (1 << bitNumber_) | tapeInput_;
            }
            if (freq <= currentComputerConfiguration.hwTapeConfiguration.frequencyBorder)
                polarity_++;
        }
        
        if (bitNumber_ == currentComputerConfiguration.hwTapeConfiguration.dataBits)
        {
            polarity_ = 0;
            tapeInput_ = 0;
            bitNumber_ = -1;
        }
        else
            bitNumber_++;
        
        pulseCount_ = 0;
        toneTime_ = 0;
    }
}

void Computer::cassettePm()
{
    wxString message;
    
    if (pulseCount_ > 1 && silenceCount_ > 10)
    {
        if (!tapeFormatFixed_)
        {
            if (currentComputerConfiguration.hwTapeConfiguration.format == TAPE_FORMAT_AUTO)
            {
//                if (computerType_ == FRED1)
//                    p_Main->eventSetStaticTextValue("CurrentTapeFormatTextFRED1", "-> PM System");
//                else
//                    p_Main->eventSetStaticTextValue("CurrentTapeFormatTextFRED1_5", "-> PM SYSTEM");
            }
            tapeFormatFixed_ = true;
        }
        if (bitNumber_ == currentComputerConfiguration.hwTapeConfiguration.dataBits)
        {
            if (pulseCount_ > 6 && (polarity_ & 1) != 1)
            {
                tapeError_ = 0;
                for (int frontPanel=0; frontPanel<numberOfFrontPanels_; frontPanel++)
                    panelPointer[frontPanel]->setErrorLed(1);
                
                if (inpMode_ == INP_MODE_TAPE_DIRECT)
                {
                    message.Printf("Polarity issue at %04X", scratchpadRegister_[0]);
                    p_Main->eventShowTextMessage(message);
                }
                if  (inpMode_ == INP_MODE_TAPE_PROGRAM)
                {
                    message.Printf("Polarity issue");
                    p_Main->eventShowTextMessage(message);
                }
                bitNumber_ = 0;
                polarity_ = 0;
                tapeInput_ = 0;
                if (inpMode_ == INP_MODE_TAPE_DIRECT)
                    dmaIn(tapeInput_);
            }
            else
            {
                if (inpMode_ == INP_MODE_TAPE_DIRECT)
                    dmaIn(tapeInput_);
                if  (inpMode_ == INP_MODE_TAPE_PROGRAM)
                {
                    lastTapeInpt_ = tapeInput_;
                    tapedataReady_ = 0;
                }
            }
        }
        else
        {
            if (bitNumber_ != -1)
            {
                if (pulseCount_ > 6)
                    tapeInput_ = (1 << bitNumber_) | tapeInput_;
            }
            if (pulseCount_ > 6)
                polarity_++;
        }
        
        if (bitNumber_ == currentComputerConfiguration.hwTapeConfiguration.dataBits)
        {
            polarity_ = 0;
            tapeInput_ = 0;
            bitNumber_ = -1;
        }
        else
            bitNumber_++;
        
        pulseCount_ = 0;
        toneTime_ = 0;
    }
}

void Computer::startRecording(int tapeNumber)
{
    tapeRecording_ = p_Main->startSaveCont(tapeNumber, tapeCounter_);
}

void Computer::finishStopTape()
{
    if (currentComputerConfiguration.addressLocationConfiguration.code_end_high != -1 && currentComputerConfiguration.addressLocationConfiguration.code_end_low != -1)
    {
        Word end = scratchpadRegister_[currentComputerConfiguration.addressLocationConfiguration.reg_end] + currentComputerConfiguration.addressLocationConfiguration.reg_end_adjust;
        
        writeMem(currentComputerConfiguration.addressLocationConfiguration.code_end_high, (end>>8)&0xff, false);
        writeMem(currentComputerConfiguration.addressLocationConfiguration.code_end_low, end&0xff, false);
        p_Main->eventSaveEnd(end);
        resetPressed_ = true;
    }
    inpMode_ = INP_MODE_NONE;
    tapeRunSwitch_ = tapeRunSwitch_ & 2;
    tapeActivated_ = false;
    tapeRecording_ = false;
    tapeEnd_ = true;
    cassetteEf_ = 0;
    tapeFinished_ = (currentComputerConfiguration.swTapeConfiguration.endDelay * sampleRate_) / 1000;
    p_Main->eventUpdateVipIILedStatus(BAR_LED_TAPE, cassetteEf_ != 0);
}

void Computer::resetTape()
{
    tapeCounterStep_ = 0;
    tapeCounter_ = "00:00:000";
    tapePeriod_ = 0;
    forwardSpeed_ = 18;
    remainingForwardSpeed_ = 0;
    rewindSpeed_ = 18;
    remainingRewindSpeed_ = 0;
    lastSec_ = -1;
    pauseTapeCounter_ = 0;

    if (currentComputerConfiguration.hwTapeConfiguration.defined)
    {
        p_Main->eventSetStaticTextValue("CasCounterXml", "00:00:000");

        if (currentComputerConfiguration.hwTapeConfiguration.format == TAPE_FORMAT_CV)
        {
            p_Computer->stopTape();
            p_Main->eventHwTapeStateChange(HW_TAPE_STATE_OFF);
            finishStopTape();
        }
    }
}

void Computer::tapeIo(Byte value)
{
    // 0x41 = TAPE 0, 0x81 = TAPE 1
    if (value == 0x41)
    {
        tapeNumber_ = "";
        if (!saveStarted_)
        {
            saveStarted_ = true;
            p_Main->startCassetteSave(0);
        }
        else
            restartTapeSave(TAPE_RECORD);
    }
    
    if (value == 0x81)
    {
        tapeNumber_ = "1";
        if (!saveStarted_)
        {
            saveStarted_ = true;
            p_Main->startCassetteSave(1);
        }
        else
            restartTapeSave(TAPE_RECORD1);
    }
    
    // 0x40 = TAPE 0, 0x80 = TAPE 1
    if (value == 0x40)
    {
        tapeNumber_ = "";
        if (!loadStarted_)
        {
            loadStarted_ = true;
            p_Main->startCassetteLoad(0);
        }
        else
            restartTapeLoad(TAPE_PLAY);
    }
    
    if (value == 0x80)
    {
        tapeNumber_ = "1";
        if (!loadStarted_)
        {
            loadStarted_ = true;
            p_Main->startCassetteLoad(1);
        }
        else
            restartTapeLoad(TAPE_PLAY1);
    }
    
    if (value == 0)
        pauseTape();
}

void Computer::onDataSwitch(wxCommandEvent&(event))
{
    dataSwitch(event.GetId()-0x80);
}

void Computer::onCardButton(wxCommandEvent&(event))
{
    cardButton(event.GetId()-0x100);
}

void Computer::cardButton(int cardValue)
{
    if (inpMode_ == INP_MODE_KEY_DIRECT)
        dmaIn(cardValue);
    else
    {
        if (currentComputerConfiguration.fredKeypadConfiguration.defined)
            fredkeypadPointer->setKeyPress(cardValue);
    }
}

void Computer::setTempo(int tempo)
{
    soundTempoCycleSize_ = (int) (((computerClockSpeed_ * 1000000) / 8) / tempo);
}

void Computer::onBackupYes(wxString dir, bool sub)
{
    deleteAllBackup(dir, sub);
}

void Computer::setEfKeyValue(int ef, Byte value)
{
    efKeyValue[ef] = value;
}

void Computer::showChip8Registers()
{
    int reg = chip8baseVar_;
    int newValue;

    for (int i=0; i<16; i++)
    {
        if (chip8register12bit_)
        {
            newValue = (p_Computer->readMem(reg++) << 8);
            newValue += p_Computer->readMem(reg++);
        }
        else
            newValue = p_Computer->readMem(reg++);
        if (newValue != chip8Register[i])
        {
            p_Main->showChip8Register(i, newValue, chip8register12bit_);
            chip8Register[i] = newValue;
        }
    }
}

void Computer::reDefineInKey(int inKey1, int inKey2)
{
    inKey1_ = inKey1;
    inKey2_ = inKey2;
}

void Computer::writeDebugFile(wxString dir, wxString name, Word start, Word end)
{
    uint64_t executed;
    wxFileName FullPath = wxFileName(dir+name, wxPATH_NATIVE);
    dir = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    name = FullPath.GetName() + ".debug";

    wxFFileOutputStream out(dir+name);
    wxZipOutputStream zip(out);
    wxDataOutputStream data(zip);

    zip.PutNextEntry(FullPath.GetName()+".deb");

    for (size_t i=start; i<=end; i++)
        data << p_Computer->readMemDataType(i, &executed);

    zip.PutNextEntry(FullPath.GetName()+".lab");
    
    for (size_t i=start; i<=end; i++)
        data << p_Computer->readMemLabelType(i);
 
    zip.Close();
}

void Computer::readDebugFile(wxString dir, wxString name, wxString number, Word start)
{
    using namespace std;

    wxFileName FullPath = wxFileName(dir+name, wxPATH_NATIVE);
    dir = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    name = FullPath.GetName() + ".debug" + number;
    Word startAddress;
    
    if (wxFile::Exists(dir+name))
    {
        auto_ptr<wxZipEntry> entry;

        wxFFileInputStream in(dir+name);
        wxZipInputStream zip(in);

        while (static_cast<void>(entry.reset(zip.GetNextEntry())), entry.get() != NULL)
        {
            // access meta-data
            name = entry->GetName();
            Byte nextByte = 0;
            
            startAddress = start;
            if (name.Right(3) == "deb")
            {
                while (!zip.Eof())
                {
                    nextByte = zip.GetC();
     //               if (nextByte >= 70 && nextByte <=86)
       //                 nextByte += 100;
                    if (nextByte != 0xff)
                        p_Computer->writeMemDataType(startAddress, nextByte);
                    startAddress++;
                }
                zip.CloseEntry();
            }
            if (name.Right(3) == "lab")
            {
                while (!zip.Eof())
                {
                    nextByte = zip.GetC();
                    if (nextByte != 0xff)
                        p_Computer->writeMemLabelType(startAddress, nextByte);
                    startAddress++;
                }
                zip.CloseEntry();
            }
        }
    }
}

void Computer::switchBootStrap()
{
    if (currentComputerConfiguration.bootstrapConfiguration.address != 0)
    {
        currentComputerConfiguration.bootstrapConfiguration.address = 0;
        bootstrap_ = 0;
    }
    else
    {
        currentComputerConfiguration.bootstrapConfiguration.address = 0x8000;
        bootstrap_ = 0x8000;
    }
}

Byte  Computer::getTilHexFont(Word address, int segNumber)
{
    if (currentComputerConfiguration.multiSegDisplayConfiguration.multitilAddressLineHigh != -1)
        address |= 1 << currentComputerConfiguration.multiSegDisplayConfiguration.multitilAddressLineHigh;
    if (currentComputerConfiguration.multiSegDisplayConfiguration.multitilAddressLineWait != -1)
        address |= (wait_ & 1) << currentComputerConfiguration.multiSegDisplayConfiguration.multitilAddressLineWait;
    if (currentComputerConfiguration.multiSegDisplayConfiguration.multitilAddressLine[segNumber] != -1)
        address |= (1 << currentComputerConfiguration.multiSegDisplayConfiguration.multitilAddressLine[segNumber]);
    if (currentComputerConfiguration.multiSegDisplayConfiguration.tilFontFile != "")
    {
        if (currentComputerConfiguration.multiSegDisplayConfiguration.reversePolarity)
            return tilFontMemory.data[address & tilFontMemory.mask] ^ 0xff;
        else
            return tilFontMemory.data[address & tilFontMemory.mask];
    }
    else
        return address;
}

bool Computer::serialDataOutput(int connection, Byte transmitterHoldingRegister, int uartNumber)
{
    switch (connection)
    {
        case UART_CONNECTION_TU58:
            if (currentComputerConfiguration.tu58Configuration.defined)
                RxFromHost(transmitterHoldingRegister, uartNumber);
        break;

        case UART_CONNECTION_VT1802:
        break;

        case UART_CONNECTION_VIS1802:
        break;

        case UART_CONNECTION_VT100:
            if (currentComputerConfiguration.videoTerminalConfiguration.type != VTNONE)
                vtPointer->serialDataOutput(transmitterHoldingRegister);
        break;
    }
    return false;
}

void Computer::sendSerialBreakComputer(int connection, bool fBreak)
{
    switch (connection)
    {
        case UART_CONNECTION_TU58:
            SendSerialBreak(fBreak);
        break;

        case UART_CONNECTION_VT1802:
        break;

        case UART_CONNECTION_VIS1802:
        break;

        case UART_CONNECTION_VT100:
        break;
    }
}

Byte Computer::readReceiverHoldingRegister(int uartNumber)
{
    uint8_t returnValue = 0;
/*    if (currentComputerConfiguration.videoTerminalConfiguration.type != VTNONE)
        return vtPointer->readReceiverHoldingRegister();
    else
        return 0;*/
    if (cdp1854Vt100Connection_ == uartNumber)
        return vtPointer->readReceiverHoldingRegister();
    if (cdp1854Ut58Connection_ == uartNumber)
    {
        TxToHost(returnValue, uartNumber);
        return returnValue;
    }
    return 0;
}

void Computer::setSendPacket(bool status)
{
    if (cdp1854Vt100Connection_ != -1)
        cdp1854InstancePointer[cdp1854Vt100Connection_]->setSendPacket(status);
}

void Computer::setTerminalLoad(bool status)
{
    if (cdp1854Vt100Connection_ != -1)
        cdp1854InstancePointer[cdp1854Vt100Connection_]->setTerminalLoad(status);
}

void Computer::setTerminalSave(bool status)
{
    if (cdp1854Vt100Connection_ != -1)
        cdp1854InstancePointer[cdp1854Vt100Connection_]->setTerminalSave(status);
}

void Computer::dataAvailable(int uartNumber)
{
    if (currentComputerConfiguration.videoTerminalConfiguration.type != VTNONE && uartNumber == 0)
        p_Vt100[uartNumber]->dataAvailable();
    if (cdp1854Vt100Connection_ == uartNumber)
        cdp1854InstancePointer[cdp1854Vt100Connection_]->dataAvailable();
    if (cdp1854Ut58Connection_ == uartNumber)
        cdp1854InstancePointer[cdp1854Ut58Connection_]->dataAvailable();
}

void Computer::dataAvailable(Byte data, int uartNumber)
{
    if (currentComputerConfiguration.videoTerminalConfiguration.type != VTNONE && uartNumber == 0)
        p_Vt100[uartNumber]->dataAvailable(data);
    if (cdp1854Vt100Connection_ == uartNumber)
        cdp1854InstancePointer[cdp1854Vt100Connection_]->dataAvailable(data);
    if (cdp1854Ut58Connection_ == uartNumber)
        cdp1854InstancePointer[cdp1854Ut58Connection_]->dataAvailable(data);
}

void Computer::dataAvailableUart(bool data, int uartNumber)
{
    if (currentComputerConfiguration.videoTerminalConfiguration.type != VTNONE && uartNumber == 0)
        p_Vt100[uartNumber]->dataAvailableUart(data);
    if (cdp1854Vt100Connection_ == uartNumber)
        cdp1854InstancePointer[cdp1854Vt100Connection_]->dataAvailableUart(data);
    if (cdp1854Ut58Connection_ == uartNumber)
        cdp1854InstancePointer[cdp1854Ut58Connection_]->dataAvailableUart(data);
}

