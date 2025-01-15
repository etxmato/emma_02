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

#include "wx/graphics.h"
#include "wx/dcgraph.h"

#if defined(__linux__)
#include "app_icon.xpm"
#endif

#define EF_REFRESH 0
#define EF_NONDISPLAY 1

#include "main.h"
#include "vis1870.h"

BEGIN_EVENT_TABLE(VIS1870, wxFrame)
    EVT_CLOSE (VIS1870::onClose)
    EVT_SIZE(VIS1870::onSize)
END_EVENT_TABLE()

VIS1870::VIS1870(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double clock, Vis1870Configuration vis1870Configuration)
: Video(title, pos, size)
{
    clock_ = clock;
    colourIndex_ = 0;
    videoClock_ = vis1870Configuration.videoClock;
    vis1870Configuration_ = vis1870Configuration;
    videoNumber_ = vis1870Configuration.videoNumber;
    pageMemoryMask_ = vis1870Configuration.pageMemSize;
    pageMemorySize_ = vis1870Configuration.pageMemSize;
    charMemorySize_ = vis1870Configuration.charMemSize;
    graphicMemorySize_ = vis1870Configuration.graphicMemSize;

    v1870Configured_ = false;
    windowSize_ = size;

    usePcbOut_ = false;
    graphicMode_ = false;
    pcbBit_ = 0;
    defineColours();

    backGround_ = 56;

    fullScreenSet_ = false;
    zoom_ = zoom;

    double intPart;
    zoomFraction_ = (modf(zoom_, &intPart) != 0);

    videoType_ = VIDEOXML1870;

    if (vis1870Configuration_.ef.reverse == 1)
    {
        efNonDisplay_ = 0;
        efDisplay_ = 1;
    }
    else
    {
        efNonDisplay_ = 1;
        efDisplay_ = 0;
    }
    
#ifndef __WXMAC__
    SetIcon(wxICON(app_icon));
#endif

    offsetX_ = 0;
    offsetY_ = 0;
    
    pixelWidth_ = 2;
    pixelHeight_ = 2;
    interruptEnabled_ = true;
    linesPerCharacters_ = vis1870Configuration_.charLines;

    if (vis1870Configuration_.videoMode == PAL)
    {
        linesPerScreen_ = 312;
        nonDisplayPeriodEndLine_ = 268;
        preDisplayPeriodLine_ = 269;
    }
    else
    {
        linesPerScreen_ = 262;
        nonDisplayPeriodEndLine_ = 226;
        preDisplayPeriodLine_ = 227;
    }

    if (vis1870Configuration_.rotateScreen)
    {
        videoHeight_ = 240;
        videoWidth_ = linesPerCharacters_*vis1870Configuration_.maxScreenLines;
    }
    else
    {
        videoWidth_ = 240;
        videoHeight_ = linesPerCharacters_*vis1870Configuration_.maxScreenLines;
    }

    switch (vis1870Configuration_.statusBarType)
    {
        case STATUSBAR_COMX:
            comxStatusBarPointer = new ComxStatusBar(this);
            statusBarPointer = comxStatusBarPointer;
            SetStatusBar(statusBarPointer);
        break;

        case STATUSBAR_DIAG:
            diagStatusBarPointer = new DiagStatusBar(this);
            statusBarPointer = diagStatusBarPointer;
            SetStatusBar(statusBarPointer);
        break;

        case STATUSBAR_CIDELSA:
            cidelsaStatusBarPointer = new CidelsaStatusBar(this);
            statusBarPointer = cidelsaStatusBarPointer;
            SetStatusBar(statusBarPointer);
        break;
    }

    setCycle();

    videoScreenPointer = new VideoScreen(this, wxSize(videoWidth_, videoHeight_), zoom, videoNumber_);
    
    screenCopyPointer = new wxBitmap(videoWidth_, videoHeight_);
    screenScrollCopyPointer = new wxBitmap(videoWidth_, videoHeight_);
    dcMemory.SelectObject(*screenCopyPointer);
    dcScroll.SelectObject(*screenScrollCopyPointer);
    
#if defined(__WXMAC__)
    gc = wxGraphicsContext::Create(dcMemory);
    gc->SetAntialiasMode(wxANTIALIAS_NONE);
#endif

    this->SetClientSize((videoWidth_+2*borderX_[videoType_])*zoom_, (videoHeight_+2*borderY_[videoType_])*zoom_);
    this->SetBackgroundColour(wxColour(0, 0, 0));
    characterListPointer = NULL;
}

VIS1870::~VIS1870()
{
    CharacterList *temp;

    dcMemory.SelectObject(wxNullBitmap);
    dcScroll.SelectObject(wxNullBitmap);
    delete videoScreenPointer;
    if (!v1870Configured_)
        return;
    delete screenCopyPointer;
    delete screenScrollCopyPointer;
#if defined(__WXMAC__)
    delete gc;
#endif
    if (updateCharacter_)
    {
        while(characterListPointer != NULL)
        {
            temp = characterListPointer;
            characterListPointer = temp->nextCharacter;
            delete temp;
        }
    }

    if (vis1870Configuration_.statusBarType != STATUSBAR_NONE)
        delete statusBarPointer;
}

void VIS1870::focus()
{
    this->SetFocus();
    videoScreenPointer->SetFocus();
}

bool VIS1870::configure1870()
{
    v1870Configured_ = true;
    pageMemorySize_ = vis1870Configuration_.pageMemSize;
    charMemorySize_ = vis1870Configuration_.charMemSize;
    graphicMemorySize_ = vis1870Configuration_.graphicMemSize;
    charMemoryIsRom_ = vis1870Configuration_.charMemIsRom;
    pcbMask_ = vis1870Configuration_.pcbMask;
    maxLinesPerCharacters_ = vis1870Configuration_.maxCharLines;
    romAddress_ = vis1870Configuration_.charRomStart;

    wxString message;
        
    p_Main->configureMessage(&vis1870Configuration_.ioGroupVector, "Video Interface System CDP 1869/1870");
    
    p_Computer->setOutType(&vis1870Configuration_.ioGroupVector, vis1870Configuration_.outputInterruptEnable, "Interrupt enable");
    p_Computer->setOutType(&vis1870Configuration_.ioGroupVector, vis1870Configuration_.outputInterruptReset, "Interrupt reset");
    if (vis1870Configuration_.outputWrite.portNumber[0] == -1 && vis1870Configuration_.outputSelect.portNumber[0] == -1)
    {
        p_Main->message("	Output 3 to 7: VIS OUT 3 to 7");
        p_Computer->setOutType(&vis1870Configuration_.ioGroupVector, 3, VIS1870_OUT3, 0);
        p_Computer->setOutType(&vis1870Configuration_.ioGroupVector, 4, VIS1870_OUT4, 0);
        p_Computer->setOutType(&vis1870Configuration_.ioGroupVector, 5, VIS1870_OUT5, 0);
        p_Computer->setOutType(&vis1870Configuration_.ioGroupVector, 6, VIS1870_OUT6, 0);
        p_Computer->setOutType(&vis1870Configuration_.ioGroupVector, 7, VIS1870_OUT7, 0);
    }
    else
    {
        p_Computer->setOutType(&vis1870Configuration_.ioGroupVector, vis1870Configuration_.outputWrite, "VIS data");
        p_Computer->setOutType(&vis1870Configuration_.ioGroupVector, vis1870Configuration_.outputSelect, "VIS register select");
    }
    p_Computer->setEfType(&vis1870Configuration_.ioGroupVector, vis1870Configuration_.ef, "display/non display period");
    p_Computer->setCycleType(CYCLE_TYPE_VIDEO_VIS1870, VIS1870_CYCLE);
    if (vis1870Configuration_.cursorBlink)
        p_Computer->setCycleType(CYCLE_TYPE_BLINK_VIS1870, VIS1870_BLINK_CYCLE);
    
    message.Printf("	Page RAM Size = %d KB", (pageMemorySize_+1)/0x400);
    p_Main->message(message);
    if (charMemoryIsRom_)
    {
        if (romAddress_ == 0)
            message.Printf("	Character ROM Size = %d KB", (charMemorySize_+1)/0x400);
        else
            message.Printf("	Character RAM+ROM Size = %d KB; RAM 0-%03X, ROM %03X-%03X", (charMemorySize_+1)/0x400, romAddress_-1, romAddress_, charMemorySize_);
    }
    else
        message.Printf("	Character RAM Size = %d KB", (charMemorySize_+1)/0x400);
    p_Main->message(message);

    if (graphicMemorySize_ != 0)
    {
        message.Printf("	Graphic RAM Size = %d KB", (graphicMemorySize_+1)/0x400);
        p_Main->message(message);
    }

//    message.Printf("	%d Characters with size: 6x%d\n", pcbMask_+1, linesPerCharacters_);
    if (maxLinesPerCharacters_ > linesPerCharacters_)
        message.Printf("	%d Characters with size: 6x%d (max 6x%d)\n", (charMemorySize_+1)/maxLinesPerCharacters_, linesPerCharacters_, maxLinesPerCharacters_);
    else
        message.Printf("	%d Characters with size: 6x%d\n", (charMemorySize_+1)/maxLinesPerCharacters_, linesPerCharacters_);
    p_Main->message(message);

    if (vis1870Configuration_.statusBarType != STATUSBAR_NONE)
    {
        statusBarPointer->init(vis1870Configuration_.expansionConfiguration_defined);
        statusBarPointer->configure(vis1870Configuration_.statusBarLedOut);
    }
    return charMemoryIsRom_;
}

void VIS1870::init1870()
{
    switch (p_Main->getCpuStartupVideoRam())
    {
        case STARTUP_ZEROED:
            for (int i=0; i<4096; i++) v1870pcb_[i] = 0;
            for (int i=0; i<4096; i++) vismacColorRam_[i] = 0;
            for (int i=0; i<4096; i++) characterMemory_[i] = 0;
            for (int i=0; i<8192; i++) graphicMemory_[i] = 0;
            for (int i=0; i<4096; i++) pageMemory_[i] = 0;
        break;
            
        case STARTUP_RANDOM:
            for (int i=0; i<4096; i++) v1870pcb_[i] = rand() % 0x100;
            for (int i=0; i<4096; i++) vismacColorRam_[i] = rand() % 0x10;
            for (int i=0; i<4096; i++) characterMemory_[i] = rand() % 0x100;
            for (int i=0; i<8192; i++) graphicMemory_[i] = rand() % 0x100;
            for (int i=0; i<4096; i++) pageMemory_[i] = rand() % 0x100;
        break;
            
        case STARTUP_DYNAMIC:
            p_Computer->setDynamicRandomByte();
            for (int i=0; i<4096; i++) v1870pcb_[i] = p_Computer->getDynamicByte(i);
            p_Computer->setDynamicRandomByte();
            for (int i=0; i<4096; i++) vismacColorRam_[i] = p_Computer->getDynamicByte(i);
            p_Computer->setDynamicRandomByte();
            for (int i=0; i<4096; i++) characterMemory_[i] = p_Computer->getDynamicByte(i);
            p_Computer->setDynamicRandomByte();
            for (int i=0; i<8192; i++) graphicMemory_[i] = p_Computer->getDynamicByte(i);
            p_Computer->setDynamicRandomByte();
            for (int i=0; i<4096; i++) pageMemory_[i] = p_Computer->getDynamicByte(i);
        break;
    }
    
    charactersPerRow_ = 20;
    rowsPerScreen_ = 24;
    pixelHeight_ = 2;
    pixelWidth_ = 2;
    displayOff_ = false;
    backgroundColour_ = 0;
    colourFormatControl_ = 0;
    cmemAccessMode_ = 0;
    CmaMask_ = 0xf;
    register3_ = 0;
    register4_ = 0;
    register5_ = 0;
    register6_ = 0;
    register7_ = 0;
    cycleValue_ = cycleSize_;
    blinkValue_ = blinkSize_;
    nonDisplay_ = true;
    ef1Value_ = efNonDisplay_;
    changeScreenSize_ = false;
    reDraw_ = true;
    reBlit_ = false;
    newBackGround_ = false;
    extraBackGround_ = false;
    updateCharacter_ = false;
    vismacBlink_ = false;
    graphicMode_ = false;
    pcbBit_ = 0;
    displayPeriodEnd_ = nonDisplayPeriodEnd_ - (displayPeriod_ / (pixelWidth_ * pixelHeight_));
}

Byte VIS1870::ef1_1870()
{
    return ef1Value_^vis1870Configuration_.ef.reverse;
}

void VIS1870::out2_1870(Byte value)
{
    vismacColorLatch_ = value;
}

void VIS1870::out3_1870(Byte value)
{
    Byte old;

    old = register3_;
    register3_ = value;

    if (old != register3_)
    {
        pixelWidth_ = ((register3_ & 0x80) == 0x80) ? 1 : 2;
        displayOff_ = ((register3_ & 0x10) == 0x10);
        if (displayOff_)  // If display is OFF make sure PREDISPLAY (nonDisplay_) is true so writing to P/CRAM is allowed
            nonDisplay_ = true;
        charactersPerRow_ = ((register3_ & 0x80) == 0x80) ? 40 : 20;
        backgroundColour_ = register3_ & 0x7;
        backGround_ = backgroundColour_ + 56;
        colourFormatControl_ = register3_ & 0x08;
        reDraw_ = true;
        newBackGround_ = true;

        maxPageMemory_ = ((register5_ & 0x40) == 0x40) ? 1920 : (rowsPerScreen_*charactersPerRow_);

        displayPeriodEnd_ = nonDisplayPeriodEnd_ - (displayPeriod_ / (pixelWidth_ * pixelHeight_));
    }
}

void VIS1870::out4_1870(Word address)
{
    if (address != register4_)
    {
        register4_ = address;
        p_Computer->tone(register4_);
    }
}

void VIS1870::out5_1870(Word address)
{
    Word old;

    old = register5_;
    register5_ = address;

    if ((old & 0xe8) != (register5_ & 0xe8))
    {
        linesPerCharacters_ = ((register5_ & 0x8) == 0x8) ? 8 : 9;
        linesPerCharacters_ = ((register5_ & 0x20) == 0x20) ? 16 : linesPerCharacters_;
        pageMemoryMask_ = ((register5_ & 0x40) == 0x40) ? 0x7ff : 0x3ff;
        if ((linesPerCharacters_ == 16) || (linesPerCharacters_ == 9) || vis1870Configuration_.cmaMaskFixed)
            CmaMask_ = 0xf;
        else
            CmaMask_ = 0x7;

        pixelHeight_ = ((register5_ & 0x80) == 0x80) ? 1 : 2;
        displayPeriodEnd_ = nonDisplayPeriodEnd_ - (displayPeriod_ / (pixelWidth_ * pixelHeight_));
        rowsPerScreen_ = ((register5_ & 0x80) == 0x80) ? vis1870Configuration_.maxScreenLines : 12;
        
        // maxPageMemory_ = ((register5_ & 0x40) == 0x40) ? 1920 : (vis1870Configuration_.maxScreenLines*40);
        // instead of above line I use charactersPerRow_ to get correct TMC600 handling - looks to be ok on COMX,
        // Cidelsa and Pecom as well. TMC expects a maxPageMemory_ = 960/4 when RES 1 is used with 12*20 character
        // screen. Same line as below is used in out3_1870.
        maxPageMemory_ = ((register5_ & 0x40) == 0x40) ? 1920 : (rowsPerScreen_*charactersPerRow_);

        reDraw_ = true;
    }

    if ((old & 0x8) != (register5_ & 0x8)) // && (computerType_ == COMX))
    {
        if (fullScreenSet_)
        {
            changeScreenSize();
        }
        else
        {
            dcMemory.SelectObject(wxNullBitmap);
            dcScroll.SelectObject(wxNullBitmap);
            delete screenCopyPointer;
            delete screenScrollCopyPointer;
            if (vis1870Configuration_.rotateScreen)
                videoWidth_ = linesPerCharacters_*rowsPerScreen_*pixelHeight_;
            else
                videoHeight_ = linesPerCharacters_*rowsPerScreen_*pixelHeight_;
            screenCopyPointer = new wxBitmap(2*offsetX_+videoWidth_, 2*offsetY_+videoHeight_);
            screenScrollCopyPointer = new wxBitmap(videoWidth_, videoHeight_);
            dcMemory.SelectObject(*screenCopyPointer);
            dcScroll.SelectObject(*screenScrollCopyPointer);

#ifdef __WXMAC__
            delete gc;
            gc = wxGraphicsContext::Create(dcMemory);
            gc->SetAntialiasMode(wxANTIALIAS_NONE);
#endif

            setScreenSize();
        }
        reDraw_ = true;
    }

    if (register5_ & 1)
    {
        cmemAccessMode_ = 1;
        register6_ = (address & 0x7ff);
    }
    else
    {
        cmemAccessMode_ = 0;
        register6_ = 0;
    }
    if ((old & 0xff00) != (register5_ & 0xff00))
        p_Computer->noise(register5_);
}

void VIS1870::out6_1870(Word address)
{
    register6_ = (address & 0x7ff);
}

void VIS1870::out7_1870(Word address)
{
    Word old;

    old = register7_;
    register7_ = (address & pageMemoryMask_ & 0x7fc);

    if (pixelWidth_ == 1 && pixelHeight_ == 1)
    {
        if (pageMemoryMask_ == 0x7ff)
            reDraw_ = true;
        else
        {
            if ((register7_ == (old+40)) || ((register7_ == 0) && (old == 920)))
            {
                dcScroll.Blit(0, 0, videoWidth_, videoHeight_-linesPerCharacters_, &dcMemory, offsetX_, linesPerCharacters_+offsetY_);
                dcScroll.Blit(0, videoHeight_-linesPerCharacters_, videoWidth_, linesPerCharacters_, &dcMemory, offsetX_, 0);
                dcMemory.Blit(offsetX_, offsetY_, videoWidth_, videoHeight_, &dcScroll, 0, 0);
                reBlit_ = true;
                return;
            }
            if ((register7_ == (old-40)) || ((register7_ == 920) && (old == 0)))
            {
                dcScroll.Blit(0, linesPerCharacters_, videoWidth_, videoHeight_-linesPerCharacters_, &dcMemory, offsetX_, offsetY_);
                dcScroll.Blit(0, 0, videoWidth_, linesPerCharacters_, &dcMemory, offsetX_, videoHeight_-linesPerCharacters_+offsetY_);
                dcMemory.Blit(offsetX_, offsetY_, videoWidth_, videoHeight_, &dcScroll, 0, 0);
                reBlit_ = true;
                return;
            }
        }
    }
    reDraw_ = true;
}

void VIS1870::cycle1870()
{
    p_Computer->resetV1870VideoModeEf();

    cycleValue_ --;
    if (cycleValue_ == preDisplayPeriod_)
    {
        if (!displayOff_)
        {
            ef1Value_ = efDisplay_;
            if (vis1870Configuration_.interruptMode == INT_MODE1)
                p_Computer->requestInterrupt(INTERRUPT_TYPE_VIS, true, vis1870Configuration_.picInterrupt);
            if (interruptEnabled_ && vis1870Configuration_.interruptMode == INT_MODE3)
                p_Computer->requestInterrupt(INTERRUPT_TYPE_VIS, true, vis1870Configuration_.picInterrupt);
        }
    }
    if (cycleValue_ == nonDisplayPeriodEnd_)
    {
        if (!displayOff_)
            nonDisplay_ = false;
        if (changeScreenSize_)
        {
            changeScreenSize();
            if (!fullScreenSet_)
                if (vis1870Configuration_.statusBarType != STATUSBAR_NONE)
                    p_Main->v1870BarSizeEvent();
            changeScreenSize_ = false;
        }

        copyScreen();
    }
    if (cycleValue_ == displayPeriodEnd_)
    {
        ef1Value_ = efNonDisplay_;
        nonDisplay_ = true;
        videoSyncCount_++;
        if (!displayOff_)
        {
            if (vis1870Configuration_.interruptMode == INT_MODE2)
                p_Computer->requestInterrupt(INTERRUPT_TYPE_VIS, true, vis1870Configuration_.picInterrupt);
            if (interruptEnabled_ && vis1870Configuration_.interruptMode == INT_MODE4)
                p_Computer->requestInterrupt(INTERRUPT_TYPE_VIS, true, vis1870Configuration_.picInterrupt);
        }
     }
    if (cycleValue_ <= 0)
        cycleValue_ = cycleSize_;
}

void VIS1870::blink1870()
{
    blinkValue_ --;
    if (blinkValue_ == 0)
    {
        blinkValue_ = blinkSize_;
        vismacBlink_ = !vismacBlink_;
        reDraw_ = true;

        copyScreen();
    }
}

int VIS1870::writePram(Word address, Byte v)
{
    if ((!nonDisplay_ && vis1870Configuration_.useBlockWrite))
        return -1;

    if (graphicMode_)
        return writeCramGraphic(address, v);

    if (cmemAccessMode_)
        address = register6_ & pageMemoryMask_;
    else
        address &= pageMemoryMask_;

    address &= pageMemorySize_;

    pageMemory_[address] = v;
    vismacColorRam_[address] = vismacColorLatch_;

    if ((address <(charactersPerRow_ * rowsPerScreen_)) || (pixelWidth_ == 2) || (pixelHeight_ == 2) || (vis1870Configuration_.colorRamType == CR_CIDELSA))
    {
        int a = address - register7_;
        while(a < 0) a += maxPageMemory_;
        int x = (a % charactersPerRow_) * 6;
        int y = (a / charactersPerRow_) * linesPerCharacters_;
        drawCharacterAndBackground(x, y, v, address);
        p_Main->assLog(v);
    }
    return address;
}

int VIS1870::writeCram(Word address, Byte v)
{
    if (!nonDisplay_ && vis1870Configuration_.useBlockWrite)
        return -1;
    
    if (graphicMode_)
        return writeCramGraphic(address, v);
    else
        return writeCramText(address, v);
}

int VIS1870::writeCramText(Word address, Byte v)
{
    Word ac;
    if (cmemAccessMode_)
        ac = register6_ & pageMemoryMask_;
    else
        ac = address & pageMemoryMask_;

    address &= CmaMask_;
    address += ((pageMemory_[ac]&pcbMask_) * maxLinesPerCharacters_);

    if (charMemoryIsRom_ && address >= romAddress_)
        return -1;
    
    characterMemory_[address&charMemorySize_] = v;

    if (vis1870Configuration_.colorRamType == CR_CIDELSA)
        v1870pcb_[address] = (p_Computer->getFlipFlopQ()) ? 1 : 0;
    for (int i=0; i<maxPageMemory_; i++)
    {
        if ((pageMemory_[i]&pcbMask_) == (pageMemory_[ac]&pcbMask_))
        {
            int a = i - ((register7_ / charactersPerRow_) * charactersPerRow_);
            while(a < 0) a += maxPageMemory_;
            int x = (a % charactersPerRow_) * 6;
            int y = (a / charactersPerRow_) * linesPerCharacters_;
            drawCharacterAndBackground(x, y, pageMemory_[i], i);
        }
    }
    return address;
}

int VIS1870::writeCramGraphic(Word address, Byte v)
{
    Word ac;
    if (cmemAccessMode_)
        ac = register6_ & graphicMemorySize_;
    else
        ac = address & graphicMemorySize_;
    
    address &= CmaMask_;
    int x = ac % charactersPerRow_;
    int y = ac / charactersPerRow_;
    address = x + (y * charactersPerRow_ * maxLinesPerCharacters_) + (address * charactersPerRow_);
    
    graphicMemory_[address&graphicMemorySize_] = v;
    
    y = address / charactersPerRow_;
    drawLine(x*6*pixelWidth_, y*pixelHeight_, v, pcbBit_, address);
    reBlit_ = true;
    
    return address;
}

Byte VIS1870::readPram(Word address)
{
    if (vis1870Configuration_.pageMemIsRom)
        return p_Computer->getMainMemory(address);
    
    if ((!nonDisplay_ && vis1870Configuration_.useBlockWrite))
        return 0xff;

    if (graphicMode_)
        return readCramGraphic(address);

    if (cmemAccessMode_)
    {
        return pageMemory_[register6_ & pageMemoryMask_ & pageMemorySize_];
    }
    return pageMemory_[address & pageMemoryMask_ & pageMemorySize_];
}

Byte VIS1870::readCram(Word address)
{
    if ((!nonDisplay_ && vis1870Configuration_.useBlockWrite))
        return 0xff;

    if (graphicMode_)
        return readCramGraphic(address);
    else
        return readCramText(address);
}

Byte VIS1870::readCramText(Word address)
{
    Byte ret, clr;

    Word ac;
    if (cmemAccessMode_)
        ac = register6_ & pageMemoryMask_;
    else
        ac = address & pageMemoryMask_;

    address &= CmaMask_;
    if (vis1870Configuration_.colorRamType == CR_CIDELSA)
    {
        p_Computer->cid1Bit8((v1870pcb_[address+(pageMemory_[ac])*maxLinesPerCharacters_] == 0));
    }
    address += ((pageMemory_[ac]&pcbMask_) * maxLinesPerCharacters_);

    if (vis1870Configuration_.colorRamType == CR_TMC600)
    {
        ret = characterMemory_[ac&charMemorySize_] & 0x3f;
        clr = vismacColorRam_[ac] &0x7;
        if (((vismacColorRam_[ac] & 0x8) == 0x8) && vismacBlink_)
        {
            clr = backgroundColour_;
        }
        ret |= ((clr & 0x2) << 6);
        return ret;
    }
    return characterMemory_[address&charMemorySize_];
}

Byte VIS1870::readCramGraphic(Word address)
{
    Word ac;
    if (cmemAccessMode_)
        ac = register6_ & graphicMemorySize_;
    else
        ac = address & graphicMemorySize_;

    address &= CmaMask_;
    int x = ac % charactersPerRow_;
    int y = ac / charactersPerRow_;
    address = x + (y * charactersPerRow_ * maxLinesPerCharacters_) + (address * charactersPerRow_);
    
    return graphicMemory_[address&graphicMemorySize_];
}

Byte VIS1870::readColourRamDirect(Word address)
{
    switch (vis1870Configuration_.colorRamType)
    {
        case CR_TMC600:
            return vismacColorRam_[address] & 0xf;
        break;
            
        case CR_CIDELSA:
            return ((characterMemory_[address&charMemorySize_] & 0xc0) >> 6) | ((v1870pcb_[address] & 0x1) << 2);
        break;
            
        default:
            return 0;
        break;
    }
}

void VIS1870::writeColourRamDirect(Word address, Byte value)
{
    switch (vis1870Configuration_.colorRamType)
    {
        case CR_TMC600:
            vismacColorRam_[address] = value & 0xf;
        break;
            
        case CR_CIDELSA:
            characterMemory_[address&charMemorySize_] = (characterMemory_[address&charMemorySize_] & 0x3f) | ((value << 6) & 0xc0);
            v1870pcb_[address] = (value & 0x4) >> 2;
        break;
    }
    reDraw_ = true;
}

Byte VIS1870::readGraphicRamDirect(Word address)
{
    return graphicMemory_[address&graphicMemorySize_];
}

void VIS1870::writeGraphicRamDirect(Word address, Byte value)
{
    graphicMemory_[address&graphicMemorySize_] = value;
    reDraw_ = true;
}

Byte VIS1870::readCramDirect(Word address)
{
    reDraw_ = true;
    switch (vis1870Configuration_.colorRamType)
    {
        case CR_TMC600:
        case CR_CIDELSA:
            return characterMemory_[address] & 0x3f;
        break;
                        
        default:
            return characterMemory_[address];
        break;
    }
}

void VIS1870::writeCramDirect(Word address, Byte value)
{
    switch (vis1870Configuration_.colorRamType)
    {
        case CR_TMC600:
            characterMemory_[address] = value & 0x3f;
        break;
            
        case CR_CIDELSA:
            characterMemory_[address] = (value & 0x3f) | (characterMemory_[address] & 0xc0);
        break;

        default:
            characterMemory_[address] = value;
        break;
    }
    reDraw_ = true;
}

void VIS1870::copyScreen()
{
    if (p_Main->isZoomEventOngoing())
        return;

    if (reColour_)
    {
        for (int i=0; i<numberOfColours_; i++)
        {
            colour_[i] = colourNew_[i];
            brushColour_[i] = brushColourNew_[i];
            penColour_[i] = penColourNew_[i];
        }
        for (int i=0; i<10; i++)
        {
            borderX_[i] = borderXNew_[i];
            borderY_[i] = borderYNew_[i];
        }
        setScreenSize();
        reDraw_ = true;
        reBlit_ = true;
        newBackGround_ = true;
        reColour_ = false;
    }

    if (reCycle_)
        setCycle();

    if (reDraw_)
        drawScreen();

#if defined(__WXMAC__)
    if (reBlit_ || reDraw_)
    {
        p_Main->eventRefreshVideo(false, videoNumber_);
        reBlit_ = false;
        reDraw_ = false;
    }
#else
    if (extraBackGround_ && newBackGround_)
        drawExtraBackground(colour_[colourIndex_+backGround_]);

    CharacterList *temp;

    if (reBlit_ || reDraw_)
    {
        videoScreenPointer->blit(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_, &dcMemory, 0, 0);
        reBlit_ = false;
        reDraw_ = false;
        if (updateCharacter_)
        {
            updateCharacter_ = false;
            while(characterListPointer != NULL)
            {
                temp = characterListPointer;
                characterListPointer = temp->nextCharacter;
                delete temp;
            }
        }
    }
    if (updateCharacter_)
    {
        updateCharacter_ = false;
        if (vis1870Configuration_.rotateScreen)
        {
            while(characterListPointer != NULL)
            {
                videoScreenPointer->blit(offsetX_+videoWidth_-linesPerCharacters_-characterListPointer->y, offsetY_+characterListPointer->x, linesPerCharacters_, 6, &dcMemory, offsetX_+videoWidth_-linesPerCharacters_-characterListPointer->y, offsetY_+characterListPointer->x);
                temp = characterListPointer;
                characterListPointer = temp->nextCharacter;
                delete temp;
            }
        }
        else
        {
            while(characterListPointer != NULL)
            {
                videoScreenPointer->blit(offsetX_+(characterListPointer->x*pixelWidth_), offsetY_+ (characterListPointer->y*pixelHeight_), 6*pixelWidth_, linesPerCharacters_*pixelHeight_, &dcMemory, offsetX_+characterListPointer->x*pixelWidth_, offsetY_+characterListPointer->y*pixelHeight_);
                temp = characterListPointer;
                characterListPointer = temp->nextCharacter;
                delete temp;
            }
        }
    }
#endif
}

void VIS1870::drawScreen()
{
    setColour(colourIndex_+backGround_);
    drawRectangle(0, 0, videoWidth_ + 2*offsetX_, videoHeight_ + 2*offsetY_);

    if (displayOff_)
    {
        return;
    }
    
    if (graphicMode_)
        drawGraphicScreen();
    else
        drawTextScreen();
}

void VIS1870::drawTextScreen()
{
    int char_screen;
    wxCoord x,y;
    int address;

    char_screen = charactersPerRow_ * rowsPerScreen_;
    address = register7_;

    while(address<0) address += maxPageMemory_;
    for (int i=0; i<char_screen; i++)
    {
        x = (i % charactersPerRow_) * 6;
        y = (i / charactersPerRow_) * linesPerCharacters_;
        drawCharacter(x, y, pageMemory_[address], address);
        address++;
        if (address>=maxPageMemory_) address = 0;
    }
}

void VIS1870::drawGraphicScreen()
{
    int char_Line_screen;
    wxCoord x,y;

    char_Line_screen = charactersPerRow_ * rowsPerScreen_ * linesPerCharacters_;

    for (int address=0; address<char_Line_screen; address++)
    {
        x = (address % charactersPerRow_) * 6;
        y = (address / charactersPerRow_);
        drawLine(x*pixelWidth_, y*pixelHeight_, graphicMemory_[address&graphicMemorySize_], pcbBit_, address);
    }
}

void VIS1870::drawCharacter(wxCoord x, wxCoord y, Byte v, int address)
{
    Byte pcb = (v & 0x80) ? 1 : 0;

    v &= pcbMask_;
    int a = v * maxLinesPerCharacters_;

    for (wxCoord i=y; i<y+linesPerCharacters_; i++)
    {
        if (usePcbOut_)
            pcb = pcbBit_;
        else
            if (pcbMask_  == 0xff) pcb = v1870pcb_[a];
    
        if (i==(y+8) && linesPerCharacters_ > maxLinesPerCharacters_)
            drawLine(x*pixelWidth_, i*pixelHeight_, 0, pcb, address);
        else
            drawLine(x*pixelWidth_, i*pixelHeight_, characterMemory_[a&charMemorySize_], pcb, address);
        a++;
    }
}

void VIS1870::drawCharacterAndBackground(wxCoord x, wxCoord y, Byte v, int address)
{
    Byte pcb = (v & 0x80) ? 1 : 0;
    if (displayOff_)
        return;
    v &= pcbMask_;
    int a = v * maxLinesPerCharacters_;

    for (wxCoord i=y; i<y+linesPerCharacters_; i++)
    {
        if (usePcbOut_)
            pcb = pcbBit_;
        else
            if (pcbMask_  == 0xff) pcb = v1870pcb_[a];
        setColourMutex(colourIndex_+backGround_);
        drawBackgroundLine(x*pixelWidth_, i*pixelHeight_);
        if (i==(y+8) && linesPerCharacters_ > maxLinesPerCharacters_)
            drawLine(x*pixelWidth_, i*pixelHeight_, 0, pcb, address);
        else
            drawLine(x*pixelWidth_, i*pixelHeight_, characterMemory_[a&charMemorySize_], pcb, address);
        a++;
    }
#if defined(__WXMAC__) || defined(__linux__)
    reBlit_ = true;
#else
    if (reBlit_)  return;
    if (zoomFraction_)
        reBlit_ = true;
    else
    {
        updateCharacter_ = true;
        CharacterList *temp = new CharacterList;
        temp->x = x;
        temp->y = y;
        temp->nextCharacter = characterListPointer;
        characterListPointer = temp;
    }
#endif
}

void VIS1870::drawLine(wxCoord x,wxCoord y,Byte v,Byte pcb, int address)
{
    int line_byte, clr = 0;

    switch(register3_ & 0x60)
    {
        case 0x00:
            if (v & 0x40) clr += 4;
            if (v & 0x80) clr += 2;
            if (pcb) clr += 1;
        break;
        case 0x20:
            if (v & 0x40) clr += 4;
            if (pcb) clr += 2;
            if (v & 0x80) clr += 1;
        break;
        case 0x40:
            if (pcb) clr += 4;
            if (v & 0x40) clr += 2;
            if (v & 0x80) clr += 1;
        break;
        case 0x60:
            if (pcb) clr += 4;
            if (v & 0x40) clr += 2;
            if (v & 0x80) clr += 1;
        break;
    }
    if (vis1870Configuration_.colorRamType == CR_TMC600)
    {
        clr = vismacColorRam_[address] &0x7;
        if (((vismacColorRam_[address] & 0x8) == 0x8) && vismacBlink_)
        {
            clr = backgroundColour_;
        }
    }
    v <<= 2;

     if (colourFormatControl_)
        clr = (clr*8) + backgroundColour_;
     else
          clr += 56;
     
    setColourMutex(colourIndex_+clr);
    line_byte = v;
    for (wxCoord i=x; i<x+(6*pixelWidth_); i+=pixelWidth_)
    {
        if (line_byte & 128)
            drawPoint(i, y);
        line_byte <<= 1;
    }
}

void VIS1870::drawPoint(wxCoord x, wxCoord y)
{
    if ((pixelHeight_ == 1) &&(pixelWidth_ == 1))
    {
        if (vis1870Configuration_.rotateScreen)
        {
            drawPointMutex(videoWidth_ - 1 - y + offsetX_, x + offsetY_);
        }
        else
        {
            drawPointMutex(x + offsetX_, y + offsetY_);
        }
    }
    else
    {
        if (vis1870Configuration_.rotateScreen)
        {
            drawRectangleMutex(videoWidth_ - 1 -y + offsetX_, x + offsetY_, pixelHeight_, pixelWidth_);
        }
        else
        {

            drawRectangleMutex(x + offsetX_, y + offsetY_, pixelWidth_, pixelHeight_);
        }
    }
}

void VIS1870::drawBackgroundLine(wxCoord x, wxCoord y)
{
    if (vis1870Configuration_.rotateScreen)
    {
        drawRectangleMutex(videoWidth_ - 1 -y + offsetX_, x + offsetY_, pixelHeight_, 6*pixelWidth_);
    }
    else
    {
        drawRectangleMutex(x + offsetX_, y + offsetY_, 6*pixelWidth_, pixelHeight_);
    }
}

void VIS1870::reDrawBar()
{
    if (vis1870Configuration_.statusBarType == STATUSBAR_NONE)
        return;
    
    statusBarPointer->reDrawBar();
    if (vis1870Configuration_.statusBarType == STATUSBAR_COMX)
        updateComxExpansionLed(true);
}

void VIS1870::updateComxExpansionLed(bool status)
{
    p_Main->eventUpdateLedStatus(status, p_Computer->getSelectedSlot(), 0);
}

void VIS1870::updateComxStatusLed(bool status)
{
    p_Main->eventUpdateLedStatus(status, p_Computer->getSelectedSlot(), 1);
}

void VIS1870::updateStatusLed(bool status, int card, int i)
{
    statusBarPointer->updateLedStatus(status, card, i);
}

bool VIS1870::readChargenFileTmc(wxString romDir, wxString romFile)
{
    wxFFile inFile;
    size_t length, number;
    char buffer[4096];

    wxString fileName = romDir + romFile;

    if (!wxFile::Exists(fileName))
    {   // Works correct, via p_Main->errorMessage it will NOT
        (void)wxMessageBox( "File " + fileName + " not found",
                            "Emma 02", wxICON_ERROR | wxOK );
        return false;
    }

    if (inFile.Open(fileName, "rb"))
    {
        length = inFile.Read(buffer, 4096);
        number = 0;
        for (size_t i=0; i<length; i+=16)
        {
            for (int j=0; j<8; j++)
            {
                characterMemory_[i+j] = (Byte)buffer[number];
                number++;
            }
        }
        for (size_t i=8; i<length; i+=16)
        {
            for (int j=0; j<8; j++)
            {
                characterMemory_[i+j] = (Byte)buffer[number];
                number++;
            }
        }
        inFile.Close();
        return true;
    }
    else
    {   // Works correct, via p_Main->errorMessage it will NOT
        (void)wxMessageBox( "Error reading " + fileName,
                            "Emma 02", wxICON_ERROR | wxOK );
        return false;
    }
}

bool VIS1870::readChargenFile(wxString romDir, wxString romFile)
{
    wxFFile inFile;
    size_t length;
    char buffer[4096];
    
    wxString fileName = romDir + romFile;
    
    if (!wxFile::Exists(fileName))
    {   // Works correct, via p_Main->errorMessage it will NOT
        (void)wxMessageBox( "File " + fileName + " not found",
                           "Emma 02", wxICON_ERROR | wxOK );
        return false;
    }
    
    if (inFile.Open(fileName, "rb"))
    {
        length = inFile.Read(buffer, 4096-romAddress_);
        for (size_t i=0; i<length; i++)
            characterMemory_[i+romAddress_] = (Byte)buffer[i];
        
        inFile.Close();
        return true;
    }
    else
    {   // Works correct, via p_Main->errorMessage it will NOT
        (void)wxMessageBox( "Error reading " + fileName,
                           "Emma 02", wxICON_ERROR | wxOK );
        return false;
    }
}

void VIS1870::setClock(double clock)
{
    clock_ = clock;
    reCycle_ = true;
}

void VIS1870::setCycle()
{
    float clockPeriod;
    int fieldTime;

    clockPeriod = (float)((1/videoClock_) * 6);
    fieldTime = clockPeriod * 60 * linesPerScreen_;

    cycleSize_ = (int)(fieldTime / ((1/clock_) * 8));
    blinkSize_ = cycleSize_ * 25;

    displayPeriod_ = (int) ((float) cycleSize_ / linesPerScreen_ * videoHeight_);
    nonDisplayPeriodEnd_ = (int) ((float) cycleSize_ / linesPerScreen_ * nonDisplayPeriodEndLine_);
    preDisplayPeriod_ = (int) ((float) cycleSize_ / linesPerScreen_ * preDisplayPeriodLine_);

    cycleValue_ = cycleSize_;
    blinkValue_ = blinkSize_;
    nonDisplay_ = true;
    ef1Value_ = efNonDisplay_;
    displayPeriodEnd_ = nonDisplayPeriodEnd_ - (displayPeriod_ / (pixelWidth_ * pixelHeight_));
    reCycle_ = false;
}

void VIS1870::setFullScreen(bool fullScreenSet)
{
    fullScreenSet_ = fullScreenSet;
#ifdef __WXMAC__
    if (vis1870Configuration_.statusBarType != STATUSBAR_NONE)
    {
        if (fullScreenSet)
            SetStatusBar(NULL);
        else
            SetStatusBar(statusBarPointer);
    }
#endif
    ShowFullScreen(fullScreenSet);
}

void VIS1870::onF3()
{
    fullScreenSet_ = !fullScreenSet_;
    p_Main->eventVideoSetFullScreen(fullScreenSet_, videoNumber_);
}

void VIS1870::reBlit(wxDC &dc)
{
    if (!v1870Configured_)
        return;

    if (!memoryDCvalid_)
        return;

    dc.Blit(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_, &dcMemory, 0, 0);

    if (extraBackGround_ && newBackGround_)
    {
        wxSize size = wxGetDisplaySize();

        dc.SetBrush(wxBrush(colour_[colourIndex_+backGround_]));
        dc.SetPen(wxPen(colour_[colourIndex_+backGround_]));

        int xStart = (int)((2*offsetX_+videoWidth_)*zoom_*xZoomFactor_);
        dc.DrawRectangle(xStart, 0, size.x-xStart, size.y);

        int yStart = (int)((2*offsetY_+videoHeight_)*zoom_);
        dc.DrawRectangle(0, yStart, size.x, size.y-yStart);

        newBackGround_ = false;
    }
}

void VIS1870::setPcbOut(Byte pcbBit)
{
    usePcbOut_ = true;
    
    switch (pcbBit)
    {
        case 1:
            pcbBit_ = 1;
        break;
        case 2:
            pcbBit_ = 0;
        break;
        case 3:
            pcbBit_ = pcbBit_ ^ 1;
        break;
    }
}

void VIS1870::setVideoMemMode(Byte mode)
{
    switch (mode)
    {
        case 1:
            graphicMode_ = true;
        break;
        case 2:
            graphicMode_ = false;
        break;
        case 3:
            graphicMode_ = !graphicMode_;
        break;
    }
}
