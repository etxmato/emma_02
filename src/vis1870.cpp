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
    EVT_SET_FOCUS(VIS1870::onFocus)
    EVT_CLOSE (VIS1870::onClose)
    EVT_SIZE(VIS1870::onSize)
END_EVENT_TABLE()

VIS1870::VIS1870(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, int computerType, double clock, double videoClock, IoConfiguration ioConfiguration, int videoNumber)
: Video(title, pos, size)
{
    clock_ = clock;
    videoClock_ = videoClock;
    colourIndex_ = 0;
    ioConfiguration_ = ioConfiguration;
    videoNumber_ = videoNumber;

    v1870Configured_ = false;
    windowSize_ = size;

    computerType_ = computerType;
 
    defineColours(computerType_);

    backGround_ = 56;

    fullScreenSet_ = false;
    zoom_ = zoom;

    double intPart;
    zoomFraction_ = (modf(zoom_, &intPart) != 0);

    videoType_ = VIDEOXML1870;

    if (ioConfiguration_.v1870efRev)
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
    videoMode_ = p_Main->getVideoMode(computerType_);
    interruptEnabled_ = true;
    
    if (videoMode_ == PAL)
        linesPerCharacters_ = 9;
    else
        linesPerCharacters_ = 8;

    if (ioConfiguration_.v1870rotate)
    {
        videoHeight_ = 240;
        videoWidth_ = linesPerCharacters_*ioConfiguration_.v1870maxScreenLines;
    }
    else
    {
        videoWidth_ = 240;
        videoHeight_ = linesPerCharacters_*ioConfiguration_.v1870maxScreenLines;
    }

    switch (ioConfiguration_.statusBarType)
    {
        case STATUSBAR_COMX:
            comxStatusBarPointer = new ComxStatusBar(this);
            SetStatusBar(comxStatusBarPointer);
        break;

        case STATUSBAR_DIAG:
            diagStatusBarPointer = new DiagStatusBar(this);
            SetStatusBar(diagStatusBarPointer);
        break;

        case STATUSBAR_CIDELSA:
            cidelsaStatusBarPointer = new CidelsaStatusBar(this);
            SetStatusBar(cidelsaStatusBarPointer);
        break;
    }

    setCycle();

    videoScreenPointer = new VideoScreen(this, wxSize(videoWidth_, videoHeight_), zoom, computerType, videoNumber_);
    
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

    switch (ioConfiguration_.statusBarType)
    {
        case STATUSBAR_COMX:
            delete comxStatusBarPointer;
        break;

        case STATUSBAR_DIAG:
            delete diagStatusBarPointer;
        break;

        case STATUSBAR_CIDELSA:
            delete cidelsaStatusBarPointer;
        break;
    }
}

void VIS1870::onFocus(wxFocusEvent&WXUNUSED(event))
{
    if (computerType_ == TMC600)
    {
        p_Computer->checkCaps();
    }
}

void VIS1870::focus()
{
    this->SetFocus();
    videoScreenPointer->SetFocus();
}

bool VIS1870::configure1870(bool expansionRomLoaded, int expansionTypeCard0)
{
    v1870Configured_ = true;
    pageMemorySize_ = ioConfiguration_.v1870pageMemSize;
    charMemorySize_ = ioConfiguration_.v1870charMemSize;
    charMemoryIsRom_ = ioConfiguration_.v1870charMemRom;
    pcbMask_ = ioConfiguration_.v1870pcbMask;
    maxLinesPerCharacters_ = ioConfiguration_.v1870maxCharLines;
    romAddress_ = ioConfiguration_.v1870charRomStart;

    wxString message;
        
    wxString ioGroup = "";
    if (ioConfiguration_.v1870ioGroup != -1)
    {
        ioGroup.Printf(" on group %d", ioConfiguration_.v1870ioGroup);
    }
    p_Main->message("Configuring Video Interface System CDP 1869/1870" + ioGroup);
    
    if (ioConfiguration_.v1870outWrite == -1 && ioConfiguration_.v1870outSelect == -1)
        p_Main->message("    Output 3 to 7: VIS OUT 3 to 7");
    else
    {
        message.Printf("    Output %d: VIS data, output 7: VIS register select", ioConfiguration_.v1870outWrite, ioConfiguration_.v1870outSelect);
    }
    message.Printf("    EF %d: display/non display period", ioConfiguration_.v1870ef);
    p_Main->message(message);
    message.Printf("    Page RAM Size = %d KB", (pageMemorySize_+1)/0x400);
    p_Main->message(message);
    if (charMemoryIsRom_)
    {
        if (romAddress_ == 0)
            message.Printf("    Character ROM Size = %d KB", (charMemorySize_+1)/0x400);
        else
            message.Printf("    Character RAM+ROM Size = %d KB; RAM 0-%03X, ROM %03X-%03X", (charMemorySize_+1)/0x400, romAddress_-1, romAddress_, charMemorySize_);
    }
    else
        message.Printf("    Character RAM Size = %d KB", (charMemorySize_+1)/0x400);
    p_Main->message(message);
    
    message.Printf("    %d Characters with size: 6x%d\n", pcbMask_+1, linesPerCharacters_);
    p_Main->message(message);
  
    switch (ioConfiguration_.statusBarType)
    {
        case STATUSBAR_COMX:
            comxStatusBarPointer->initComxBar(expansionRomLoaded, expansionTypeCard0);
        break;

        case STATUSBAR_DIAG:
            diagStatusBarPointer->initDiagBar();
        break;

        case STATUSBAR_CIDELSA:
            cidelsaStatusBarPointer->initCidelsaBar();
        break;
    }
    return charMemoryIsRom_;
}

void VIS1870::v1870BarSize()
{
    switch (ioConfiguration_.statusBarType)
    {
        case STATUSBAR_COMX:
            comxStatusBarPointer->reDrawBar();
            updateExpansionLed(true);
        break;

        case STATUSBAR_DIAG:
            diagStatusBarPointer->reDrawBar();
        break;

        case STATUSBAR_CIDELSA:
            cidelsaStatusBarPointer->reDrawBar();
        break;
    }
}

void VIS1870::init1870()
{
    switch (p_Main->getCpuStartupVideoRam())
    {
        case STARTUP_ZEROED:
            for (int i=0; i<4096; i++) v1870pcb_[i] = 0;
            for (int i=0; i<4096; i++) vismacColorRam_[i] = 0;
            if (!charMemoryIsRom_)
                for (int i=0; i<4096; i++) characterMemory_[i] = 0;
            for (int i=0; i<4096; i++) pageMemory_[i] = 0;
        break;
            
        case STARTUP_RANDOM:
            for (int i=0; i<4096; i++) v1870pcb_[i] = rand() % 0x100;
            for (int i=0; i<4096; i++) vismacColorRam_[i] = rand() % 0x10;
            if (!charMemoryIsRom_)
                for (int i=0; i<4096; i++) characterMemory_[i] = rand() % 0x100;
            for (int i=0; i<4096; i++) pageMemory_[i] = rand() % 0x100;
        break;
            
        case STARTUP_DYNAMIC:
            p_Computer->setDynamicRandomByte();
            for (int i=0; i<4096; i++) v1870pcb_[i] = p_Computer->getDynamicByte(i);
            p_Computer->setDynamicRandomByte();
            for (int i=0; i<4096; i++) vismacColorRam_[i] = p_Computer->getDynamicByte(i);
            p_Computer->setDynamicRandomByte();
            if (!charMemoryIsRom_)
                for (int i=0; i<4096; i++) characterMemory_[i] = p_Computer->getDynamicByte(i);
            p_Computer->setDynamicRandomByte();
            for (int i=0; i<4096; i++) pageMemory_[i] = p_Computer->getDynamicByte(i);
        break;
    }
    
    charactersPerRow_ = 20;
    rowsPerScreen_ = 24;
    pixelHeight_ = 2;
    pixelWidth_ = 2;
    displayOff_ = 0;
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
    displayPeriodEnd_ = nonDisplayPeriodEnd_ - (displayPeriod_ / (pixelWidth_ * pixelHeight_));
}

Byte VIS1870::ef1_1870()
{
    return ef1Value_;
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
        displayOff_ = register3_ & 0x10;
        charactersPerRow_ = ((register3_ & 0x80) == 0x80) ? 40 : 20;
        backgroundColour_ = register3_ & 0x7;
        backGround_ = backgroundColour_ + 56;
        colourFormatControl_ = register3_ & 0x08;
        reDraw_ = true;
        newBackGround_ = true;
        if (computerType_ == TMC600)
        {
            if ((pixelHeight_ == 2) && (pixelWidth_ == 2))
                maxPageMemory_ = 960/2;
            else
                maxPageMemory_ = 960;
        }
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
        maxPageMemory_ = ((register5_ & 0x40) == 0x40) ? 1920 : (ioConfiguration_.v1870maxScreenLines*40);
        if ((linesPerCharacters_ == 16) || (linesPerCharacters_ == 9) || ioConfiguration_.v1870cmaMaskFixed)
            CmaMask_ = 0xf;
        else
            CmaMask_ = 0x7;

        pixelHeight_ = ((register5_ & 0x80) == 0x80) ? 1 : 2;
        displayPeriodEnd_ = nonDisplayPeriodEnd_ - (displayPeriod_ / (pixelWidth_ * pixelHeight_));
        rowsPerScreen_ = ((register5_ & 0x80) == 0x80) ? ioConfiguration_.v1870maxScreenLines : 12;
        
        if (computerType_ == TMC600)
        {
            if ((pixelHeight_ == 2) && (pixelWidth_ == 2))
                maxPageMemory_ = 960/4;
            else
                maxPageMemory_ = 960;
        }

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
            if (ioConfiguration_.v1870rotate)
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
    if (p_Computer->getFlipFlopQ())
        p_Computer->resetV1870VideoModeEf();

    cycleValue_ --;
    if (cycleValue_ == preDisplayPeriod_)
    {
        if (!displayOff_)
        {
            ef1Value_ = efDisplay_;
            if (ioConfiguration_.v1870interruptMode == INT_MODE1)
                p_Computer->interrupt();
            if (interruptEnabled_ && ioConfiguration_.v1870interruptMode == INT_MODE3)
                p_Computer->interrupt();
        }
    }
    if (cycleValue_ == nonDisplayPeriodEnd_)
    {
        if (!displayOff_)
            nonDisplay_ = false;
        if (changeScreenSize_)
        {
            wxSize size;
            if (wxIsMainThread())
                size = GetClientSize();
            else
                size = p_Main->eventGetClientSize(false, videoNumber_);

            p_Main->setMainSize(computerType_, size);

            changeScreenSize();
            if (!fullScreenSet_)
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
            if (ioConfiguration_.v1870interruptMode == INT_MODE2)
                p_Computer->interrupt();
            if (interruptEnabled_ && ioConfiguration_.v1870interruptMode == INT_MODE4)
                p_Computer->interrupt();
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
    if (!nonDisplay_)
        return -1;

    if (cmemAccessMode_)
        address = register6_ & pageMemoryMask_;
    else
        address &= pageMemoryMask_;

    address &= pageMemorySize_;

    pageMemory_[address] = v;
    vismacColorRam_[address] = vismacColorLatch_;

    if ((address <(charactersPerRow_ * rowsPerScreen_)) || (pixelWidth_ == 2) || (pixelHeight_ == 2) || (ioConfiguration_.v1870colorRamType == CR_CIDELSA))
    {
        int a = address - register7_;
        while(a < 0) a += maxPageMemory_;
        int x = (a % charactersPerRow_) * 6;
        int y = (a / charactersPerRow_) * linesPerCharacters_;
        drawCharacterAndBackground(x, y, v, address);
// ** address log, comment out next line
//        p_Main->assLog(v);
    }
    return address;
}

int VIS1870::writeCram(Word address, Byte v)
{
    if (!nonDisplay_)
        return -1;

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

    if (ioConfiguration_.v1870colorRamType == CR_CIDELSA)
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

Byte VIS1870::readPram(Word address)
{
    if (ioConfiguration_.v1870pageMemRom)
        return p_Computer->getRam(address);
    
    if (!nonDisplay_)
        return 0xff;

    if (cmemAccessMode_)
    {
        return pageMemory_[register6_ & pageMemoryMask_ & pageMemorySize_];
    }
    return pageMemory_[address & pageMemoryMask_ & pageMemorySize_];
}

Byte VIS1870::readCram(Word address)
{
    if (!nonDisplay_)
        return 0xff;

    Word ac;
    Byte ret, clr;

    if (cmemAccessMode_)
        ac = register6_ & pageMemoryMask_;
    else
        ac = address & pageMemoryMask_;

    address &= CmaMask_;
    if (ioConfiguration_.v1870colorRamType == CR_CIDELSA)
    {
        p_Computer->cid1Bit8((v1870pcb_[address+(pageMemory_[ac])*maxLinesPerCharacters_] == 0));
    }
    address += ((pageMemory_[ac]&pcbMask_) * maxLinesPerCharacters_);
    if (ioConfiguration_.v1870colorRamType == CR_TMC600)
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

Byte VIS1870::readColourRamDirect(Word address)
{
    switch (ioConfiguration_.v1870colorRamType)
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
    switch (ioConfiguration_.v1870colorRamType)
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

Byte VIS1870::readCramDirect(Word address)
{
    reDraw_ = true;
    switch (ioConfiguration_.v1870colorRamType)
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
    switch (ioConfiguration_.v1870colorRamType)
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
        if (ioConfiguration_.v1870rotate)
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
    int char_screen;
    wxCoord x,y;
    int address;

    setColour(colourIndex_+backGround_);
    drawRectangle(0, 0, videoWidth_ + 2*offsetX_, videoHeight_ + 2*offsetY_);

    if (displayOff_)
    {
        return;
    }
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

void VIS1870::drawCharacter(wxCoord x, wxCoord y, Byte v, int address)
{
    Byte pcb = (v & 0x80) ? 1 : 0;

    v &= pcbMask_;
    int a = v * maxLinesPerCharacters_;
    for (wxCoord i=y; i<y+linesPerCharacters_; i++)
    {
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
    if (ioConfiguration_.v1870colorRamType == CR_TMC600)
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
        if (ioConfiguration_.v1870rotate)
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
        if (ioConfiguration_.v1870rotate)
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
    if (ioConfiguration_.v1870rotate)
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
    switch (ioConfiguration_.statusBarType)
    {
        case STATUSBAR_COMX:
            comxStatusBarPointer->reDrawBar();
            updateExpansionLed(true);
        break;

        case STATUSBAR_DIAG:
            diagStatusBarPointer->reDrawBar();
            updateExpansionLed(true);
        break;

        case STATUSBAR_CIDELSA:
            cidelsaStatusBarPointer->reDrawBar();
        break;
    }
}

void VIS1870::updateExpansionLed(bool status)
{
    p_Main->eventUpdateComxLedStatus(p_Computer->getSelectedSlot(), 0, status);
}

void VIS1870::updateStatusLed(bool status)
{
    p_Main->eventUpdateComxLedStatus(p_Computer->getSelectedSlot(), 1, status);
}

void VIS1870::updateLedStatus(int card, int i, bool status)
{
    switch (ioConfiguration_.statusBarType)
    {
        case STATUSBAR_COMX:
            comxStatusBarPointer->updateLedStatus(card, i, status);
        break;

        case STATUSBAR_DIAG:
            diagStatusBarPointer->updateLedStatus(0, status);
        break;
    }
}

void VIS1870::updateDiagLedStatus(int led, bool status)
{
    if (ioConfiguration_.statusBarType == STATUSBAR_DIAG)
        diagStatusBarPointer->updateLedStatus(led, status);
}

void VIS1870::updateCidelsaLedStatus(int number, bool status)
{
    cidelsaStatusBarPointer->updateLedStatus(number, status);
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
    fieldTime = clockPeriod * 60 * 312;

    cycleSize_ = (int)(fieldTime / ((1/clock_) * 8));
    blinkSize_ = cycleSize_ * 25;

    displayPeriod_ = (int) ((float) cycleSize_ / 312 * videoHeight_);
    nonDisplayPeriodEnd_ = (int) ((float) cycleSize_ / 312 * 268);
    preDisplayPeriod_ = (int) ((float) cycleSize_ / 312 * 269);

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
    if (fullScreenSet)
        switch (ioConfiguration_.statusBarType)
        {
            case STATUSBAR_COMX:
            case STATUSBAR_DIAG:
            case STATUSBAR_CIDELSA:
                SetStatusBar(NULL);
            break;
        }
    else
    {
        switch (ioConfiguration_.statusBarType)
        {
            case STATUSBAR_COMX:
                SetStatusBar(comxStatusBarPointer);
            break;

            case STATUSBAR_DIAG:
                SetStatusBar(diagStatusBarPointer);
            break;

            case STATUSBAR_CIDELSA:
                SetStatusBar(cidelsaStatusBarPointer);
            break;
        }
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

