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

#define FORE 64
#define BACK 65

#if defined(__linux__)
#include "app_icon.xpm"
/*#include "comx_icon.xpm"
#include "telmac_icon.xpm"
#include "pecom_icon.xpm"
#include "cidelsa_icon.xpm"*/
#endif

#define EF_REFRESH 0
#define EF_NONDISPLAY 1
#define MC6845CHARW 8

#include "main.h"
#include "v1870.h"

BEGIN_EVENT_TABLE(V1870, wxFrame)
    EVT_SET_FOCUS(V1870::onFocus)
    EVT_CLOSE (V1870::onClose)
    EVT_SIZE(V1870::onSize)
END_EVENT_TABLE()

V1870::V1870(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, int computerType, double clock, int videoNumber)
: Video(title, pos, size)
{
    int regVal [16] = {129, 80, 102, 20, 34, 0, 24, 27, 0, 8, 72, 8, 0, 0, 0, 0};
    clock_ = clock;
    videoNumber_ = videoNumber;

    v1870Configured_ = false;
    windowSize_ = size;

    computerType_ = computerType;

    defineColours(COMX);
    if (computerType_!= COMX)
        defineColours(computerType_);

    backGround_ = 56;

    fullScreenSet_ = false;
    zoom_ = zoom;

    double intPart;
    zoomFraction_ = (modf(zoom_, &intPart) != 0);

    mc6845started_ = false;
    videoType_ = VIDEO1870;

    efNonDisplay_ = 1;
    efDisplay_ = 0;

#ifndef __WXMAC__
    SetIcon(wxICON(app_icon));
#endif

    offsetX_ = 0;
    offsetY_ = 0;
    
    pixelWidth_ = 2;
    pixelHeight_ = 2;
    videoMode_ = PAL;
    interruptMode_ = INT_MODE2;
    interruptEnabled_ = true;

    switch (computerType_)
    {
        case COMX:
            videoMode_ = p_Main->getVideoMode(COMX);
            interlaceGUI_ = p_Main->getInterlace(COMX);
            maxPageMemory_ = 960;
            if (videoMode_ == PAL)
            {
                videoWidth_ = 240;
                videoHeight_ = 216;
            }
            else
            {
                videoWidth_ = 240;
                videoHeight_ = 192;
            }
            if (p_Main->isDiagActive(COMX))
            {
                diagStatusBarPointer = new DiagStatusBar(this);
                SetStatusBar(diagStatusBarPointer);
            }
            else
            {
                comxStatusBarPointer = new ComxStatusBar(this);
                SetStatusBar(comxStatusBarPointer);
            }

            cursorAddress_ = 0;
            startAddress_ = 0;
            cursorOn_ = true;
            cursorBlink_ = true;
            cursorBlinkOn_ = false;
            cursorBlinkTime_ = 16;
            blink_ = cursorBlinkTime_;
            charLine_ = 0;
            rows_ = 0;
            scanLine_ = 0;
            for (int i=0; i<16; i++)
            {
                register_[i] = 0xff;
                writeRegister6845(i);
                writeData6845(regVal[i]);
            }
        break;

        case CIDELSA:
            if (cidelsaGame_ == DRACO)
                maxPageMemory_ = 1920;
            else
                maxPageMemory_ = 1000;
            videoWidth_ = 200;
            videoHeight_ = 240;
        break;

        case TMC600:
            maxPageMemory_ = 960;
            videoWidth_ = 240;
            videoHeight_ = 216;
        break;

        case PECOM:
            maxPageMemory_ = 960;
            videoWidth_ = 240;
            videoHeight_ = 216;
        break;

        case MICROBOARD:
            videoType_ = VIDEO1870;
            videoMode_ = p_Main->getVideoMode(computerType_);
            if (videoMode_ == PAL)
            {
                videoWidth_ = 240;
                videoHeight_ = 216;
            }
            else
            {
                videoWidth_ = 240;
                videoHeight_ = 192;
            }
        break;
            
        case XML:
            videoMode_ = p_Main->getVideoMode(computerType_);
            if (videoMode_ == PAL)
            {
                videoWidth_ = 240;
                videoHeight_ = 216;
            }
            else
            {
                videoWidth_ = 240;
                videoHeight_ = 192;
            }
        break;
    }

    setCycle();

    videoScreenPointer = new VideoScreen(this, wxSize(videoWidth_, videoHeight_), zoom, computerType, videoNumber_);
    this->SetClientSize((videoWidth_+2*borderX_[videoType_])*zoom_, (videoHeight_+2*borderY_[videoType_])*zoom_);
    this->SetBackgroundColour(wxColour(0, 0, 0));
    characterListPointer = NULL;
    characterListPointer6845 = NULL;
}

V1870::~V1870()
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
    switch (computerType_)
    {
        case COMX:
            if (p_Main->isDiagActive(COMX))
                delete diagStatusBarPointer;
            else
                delete comxStatusBarPointer;
        break;

        case CIDELSA:
            if (cidelsaGame_ != DRACO)
                delete cidelsaStatusBarPointer;
        break;
    }
    if (updateCharacter_)
    {
        while(characterListPointer != NULL)
        {
            temp = characterListPointer;
            characterListPointer = temp->nextCharacter;
            delete temp;
        }
    }
    if (updateCharacter6845_ > 0)
    {
        while(characterListPointer6845 != NULL)
        {
            temp = characterListPointer6845;
            characterListPointer6845 = temp->nextCharacter;
            delete temp;
        }
    }
}

void V1870::onFocus(wxFocusEvent&WXUNUSED(event))
{
    if (computerType_ == TMC600)
    {
        p_Computer->checkCaps();
    }
}

void V1870::focus()
{
    this->SetFocus();
    videoScreenPointer->SetFocus();
}

void V1870::configure1870Comx(bool expansionRomLoaded, int expansionTypeCard0)
{
    v1870Configured_ = true;
   maxLinesPerCharacters_ = 16;
    pageMemorySize_ = 0x3ff;
    charMemorySize_ = 0x7ff;
    pcbMask_ = 0x7f;
    charMemoryIsRom_ = false;

    if (p_Main->isDiagActive(COMX))
        diagStatusBarPointer->initDiagBar();
    else
        comxStatusBarPointer->initComxBar(expansionRomLoaded, expansionTypeCard0);

    screenCopyPointer = new wxBitmap(videoWidth_, videoHeight_);
    screenScrollCopyPointer = new wxBitmap(videoWidth_, videoHeight_);
    dcMemory.SelectObject(*screenCopyPointer);
    dcScroll.SelectObject(*screenScrollCopyPointer);

#if defined(__WXMAC__)
    gc = wxGraphicsContext::Create(dcMemory);
    gc->SetAntialiasMode(wxANTIALIAS_NONE);
#endif

    cycleType_[VIDEOCYCLE_V1870] = V1870CYCLE;
    efType_[0][0][1] = V1870EF;

    outType_[0][0][3] = V1870OUT3;
    outType_[0][0][4] = V1870OUT4;
    outType_[0][0][5] = V1870OUT5;
    outType_[0][0][6] = V1870OUT6;
    outType_[0][0][7] = V1870OUT7;
    p_Main->message("Configuring Video Interface System CDP 1869/1870");
    p_Main->message("    Output 3 to 7: VIS OUT 3 to 7");
    p_Main->message("    EF 1: display/non display period\n");
}

bool V1870::configure1870Microboard(int v1870group, int v1870pageMemSize, int videoMode, int interruptMode)
{
    v1870Configured_ = true;
    pageMemorySize_ = (0x400 << v1870pageMemSize) - 1;
    maxPageMemory_ = (v1870pageMemSize == 0) ? 960 : 1920;
    if (videoMode_ == PAL)
    {
        switch (videoMode)
        {
            case 1:
                pageMemorySize_ = 0x3ff;
                maxPageMemory_ = 960;
                charMemorySize_ = 0x3ff;
                charMemoryIsRom_ = false;
                pcbMask_ = 0x7f;
                maxLinesPerCharacters_ = 8;
            break;

            case 2:
                pageMemorySize_ = 0x3ff;
                maxPageMemory_ = 960;
                charMemorySize_ = 0x3ff;
                charMemoryIsRom_ = false;
                pcbMask_ = 0x7f;
                maxLinesPerCharacters_ = 16;
            break;
        }
    }
    else
    {
        switch (videoMode)
        {
            case 1:
                charMemorySize_ = 0x3ff;
                charMemoryIsRom_ = false;
                pcbMask_ = 0x7f;
                maxLinesPerCharacters_ = 8;
            break;
            
            case 2:
                charMemorySize_ = 0x7ff;
                charMemoryIsRom_ = false;
                pcbMask_ = 0xff;
                maxLinesPerCharacters_ = 8;
            break;

            case 3:
                charMemorySize_ = 0x7ff;
                charMemoryIsRom_ = false;
                pcbMask_ = 0x7f;
                maxLinesPerCharacters_ = 16;
            break;
            
            case 4:
                charMemorySize_ = 0x3ff;
                charMemoryIsRom_ = true;
                romAddress_ = 0x200;
                pcbMask_ = 0x7f;
                maxLinesPerCharacters_ = 8;
            break;

            case 5:
                charMemorySize_ = 0x3ff;
                charMemoryIsRom_ = true;
                romAddress_ = 0;
                pcbMask_ = 0x7f;
                maxLinesPerCharacters_ = 8;
            break;

            case 6:
                charMemorySize_ = 0x7ff;
                charMemoryIsRom_ = true;
                romAddress_ = 0;
                pcbMask_ = 0xff;
                maxLinesPerCharacters_ = 8;
            break;

            case 7:
                charMemorySize_ = 0x7ff;
                charMemoryIsRom_ = true;
                romAddress_ = 0;
                pcbMask_ = 0x7f;
                maxLinesPerCharacters_ = 16;
            break;
                
            case 8:
                charMemorySize_ = 0x7ff;
                charMemoryIsRom_ = true;
                romAddress_ = 0x400;
                pcbMask_ = 0xff;
                maxLinesPerCharacters_ = 8;
            break;

            case 9:
                charMemorySize_ = 0xfff;
                charMemoryIsRom_ = true;
                romAddress_ = 0x800;
                pcbMask_ = 0xff;
                maxLinesPerCharacters_ = 16;
            break;
        }
    }

    interruptMode_ = interruptMode;
    
    wxString message;
    
    screenCopyPointer = new wxBitmap(videoWidth_, videoHeight_);
    screenScrollCopyPointer = new wxBitmap(videoWidth_, videoHeight_);
    dcMemory.SelectObject(*screenCopyPointer);
    dcScroll.SelectObject(*screenScrollCopyPointer);
    
#if defined(__WXMAC__)
    gc = wxGraphicsContext::Create(dcMemory);
    gc->SetAntialiasMode(wxANTIALIAS_NONE);
#endif
    
    cycleType_[VIDEOCYCLE_V1870] = V1870CYCLE;
    message.Printf("Configuring Video Interface System CDP 1869/1870 on group %02X", v1870group);
    p_Main->message(message);
    
    p_Main->message("    Output 3 to 7: VIS OUT 3 to 7");
    p_Main->message("    EF 1: display/non display period");
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
    if (videoMode_ == PAL && maxLinesPerCharacters_ == 16 )
        message.Printf("    64 Characters with size: 6x%d", maxLinesPerCharacters_);
    else
        message.Printf("    %d Characters with size: 6x%d", pcbMask_+1, maxLinesPerCharacters_);
    p_Main->message(message);
    p_Main->message("\n");
    
    return charMemoryIsRom_;
}

void V1870::configure1870Cidelsa()
{
    v1870Configured_ = true;
    maxLinesPerCharacters_ = 8;
    pcbMask_ = 0xff;
    charMemorySize_ = 0x7ff;
    charMemoryIsRom_ = false;
    
    if (cidelsaGame_ != DRACO)
    {
        pageMemorySize_ = 0x3ff;
        cidelsaStatusBarPointer = new CidelsaStatusBar(this);
        SetStatusBar(cidelsaStatusBarPointer);
        cidelsaStatusBarPointer->initCidelsaBar();
        efNonDisplay_ = 0;
        efDisplay_ = 1;
    }
    else
    {
        pageMemorySize_ = 0x7ff;
        videoWidth_ = 216;
        reCycle_ = true;
        efNonDisplay_ = 1;
        efDisplay_ = 0;
    }
    this->SetClientSize((videoWidth_+2*borderX_[videoType_])*zoom_, (videoHeight_+2*borderY_[videoType_])*zoom_);

    screenCopyPointer = new wxBitmap(videoWidth_, videoHeight_);
    screenScrollCopyPointer = new wxBitmap(videoWidth_, videoHeight_);
    dcMemory.SelectObject(*screenCopyPointer);
    dcScroll.SelectObject(*screenScrollCopyPointer);

#if defined(__WXMAC__)
    gc = wxGraphicsContext::Create(dcMemory);
    gc->SetAntialiasMode(wxANTIALIAS_NONE);
#endif

    cycleType_[VIDEOCYCLE_V1870] = V1870CYCLE;
    efType_[0][0][1] = V1870EF;

    outType_[0][0][3] = V1870OUT3;
    outType_[0][0][4] = V1870OUT4;
    outType_[0][0][5] = V1870OUT5;
    outType_[0][0][6] = V1870OUT6;
    outType_[0][0][7] = V1870OUT7;
    p_Main->message("Configuring Video Interface System CDP 1869/1876");
    p_Main->message("    Output 3 to 7: VIS OUT 3 to 7");
    p_Main->message("    EF 1: display/non display period\n");
}

void V1870::configure1870Telmac()
{
    v1870Configured_ = true;
    maxLinesPerCharacters_ = 16;
    pageMemorySize_ = 0x3ff;
    charMemorySize_ = 0xfff;
    pcbMask_ = 0xff;
    charMemoryIsRom_ = true;

    screenCopyPointer = new wxBitmap(videoWidth_, videoHeight_);
    screenScrollCopyPointer = new wxBitmap(videoWidth_, videoHeight_);
    dcMemory.SelectObject(*screenCopyPointer);
    dcScroll.SelectObject(*screenScrollCopyPointer);

#if defined(__WXMAC__)
    gc = wxGraphicsContext::Create(dcMemory);
    gc->SetAntialiasMode(wxANTIALIAS_NONE);
#endif

    cycleType_[BLINKCYCLE_V1870] = V1870BLINK;
    cycleType_[VIDEOCYCLE_V1870] = V1870CYCLE;

    efType_[0][0][1] = V1870EF;

    outType_[0][0][5] = TELMACOUTDATA;
    outType_[0][0][7] = TELMACOUTREGISTER;
    p_Main->message("Configuring Video Interface System CDP 1869/1870");
    p_Main->message("    Output 5: VIS data, output 7: VIS register select");
    p_Main->message("    EF 1: display/non display period\n");
}

void V1870::configure1870Pecom()
{
    v1870Configured_ = true;
    maxLinesPerCharacters_ = 16;
    pageMemorySize_ = 0x3ff;
    charMemorySize_ = 0x7ff;
    pcbMask_ = 0x7f;
    charMemoryIsRom_ = false;

    screenCopyPointer = new wxBitmap(videoWidth_, videoHeight_);
    screenScrollCopyPointer = new wxBitmap(videoWidth_, videoHeight_);
    dcMemory.SelectObject(*screenCopyPointer);
    dcScroll.SelectObject(*screenScrollCopyPointer);

#if defined(__WXMAC__)
    gc = wxGraphicsContext::Create(dcMemory);
    gc->SetAntialiasMode(wxANTIALIAS_NONE);
#endif

    cycleType_[VIDEOCYCLE_V1870] = V1870CYCLE;

    outType_[0][0][3] = V1870OUT3;
    outType_[0][0][4] = V1870OUT4;
    outType_[0][0][5] = V1870OUT5;
    outType_[0][0][6] = V1870OUT6;
    outType_[0][0][7] = V1870OUT7;
    p_Main->message("Configuring Video Interface System CDP 1869/1870");
    p_Main->message("    Output 3 to 7: VIS OUT 3 to 7\n");
}

void V1870::v1870BarSize()
{
    if (computerType_ == COMX)
    {
        if (p_Main->isDiagActive(COMX))
            diagStatusBarPointer->reDrawBar();
        else
        {
            comxStatusBarPointer->reDrawBar();
            updateExpansionLed(true);
        }
    }
    if (computerType_ == CIDELSA && cidelsaGame_ != DRACO)
    {
        cidelsaStatusBarPointer->reDrawBar();
    }
}

void V1870::configure6845()
{
    if (mc6845started_)  return;
    mc6845started_ = true;
    backGround_ = BACK;
    videoType_ = VIDEO80COL;

    save_v1870X_ = videoWidth_;
    save_v1870Y_ = videoHeight_;
    videoWidth_ = charLine_*MC6845CHARW;
    videoHeight_ = rows_*scanLine_*2;

    setCycle();
    setCycleType(BLINKCYCLE_MC6845, MC6845BLINK);
    setCycleType(VIDEOCYCLE_MC6845, MC6845CYCLE);
    if (fullScreenSet_)
    {
        changeScreenSize();
    }
    else
    {
        p_Main->eventSetClientSize(p_Main->get6845Size(computerType_), CALL_CHANGE_SCREEN_SIZE, false, videoNumber_);
        
        p_Main->v1870BarSizeEvent();
    }
}

void V1870::stop6845()
{
    if (!mc6845started_)  return;
    mc6845started_ = false;
    backGround_ = backgroundColour_ + 56;
    videoType_ = VIDEO1870;

    videoWidth_ = save_v1870X_;
    videoHeight_ = save_v1870Y_;

    setCycle();
    setCycleType(BLINKCYCLE_MC6845, 0);
    setCycleType(VIDEOCYCLE_MC6845, 0);
    reDraw_ = true;
    if (fullScreenSet_)
    {
        changeScreenSize();
    }
    else
    {
        p_Main->eventSetClientSize(p_Main->getMainSize(computerType_), CALL_CHANGE_SCREEN_SIZE, false, videoNumber_);

        p_Main->v1870BarSizeEvent();
    }
}

void V1870::init1870()
{
    switch (p_Main->getCpuStartupVideoRam())
    {
        case STARTUP_ZEROED:
            for (int i=0; i<4096; i++) v1870pcb_[i] = 0;
            for (int i=0; i<4096; i++) vismacColorRam_[i] = 0;
            for (int i=0; i<4096; i++) characterMemory_[i] = 0;
            for (int i=0; i<4096; i++) pageMemory_[i] = 0;
        break;
            
        case STARTUP_RANDOM:
            for (int i=0; i<4096; i++) v1870pcb_[i] = rand() % 0x100;
            for (int i=0; i<4096; i++) vismacColorRam_[i] = rand() % 0x10;
            for (int i=0; i<4096; i++) characterMemory_[i] = rand() % 0x100;
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
            for (int i=0; i<4096; i++) pageMemory_[i] = p_Computer->getDynamicByte(i);
        break;
    }
    
    register3_ = 0;
    charactersPerRow_ = 20;
    pixelWidth_ = 2;
    displayOff_ = 0;
    backgroundColour_ = 0;
    colourFormatControl_ = 0;
    register4_ = 0;
    register5_ = 0;
    cmemAccessMode_ = 0;
    linesPerCharacters_ = 9;
    maxLinesPerCharacters_ = 16;
    pageMemoryMask_ = 0x3ff;
    CmaMask_ = 0xf;
    rowsPerScreen_ = 12;
    pixelHeight_ = 2;
    register6_ = 0;
    register7_ = 0;
    cycleValue_ = cycleSize_;
    cycleValue6845_ = cycleSize6845_;
    blinkValue_ = blinkSize_;
    blinkValue6845_ = blinkSize6845_;
    nonDisplay_ = true;
    ef1Value_ = efNonDisplay_;
    changeScreenSize_ = false;
    reDraw_ = true;
    reBlit_ = false;
    newBackGround_ = false;
    extraBackGround_ = false;
    updateCharacter_ = false;
    updateCharacter6845_ = 0;
    vismacBlink_ = false;
    displayPeriodEnd_ = nonDisplayPeriodEnd_ - (displayPeriod_ / (pixelWidth_ * pixelHeight_));
}

Byte V1870::ef1_1870()
{
    return ef1Value_;
}

void V1870::out2_1870(Byte value)
{
    vismacColorLatch_ = value;
//        p_Main->messageInt(vismacColorLatch_);
}

void V1870::out3_1870(Byte value)
{
    Byte old;

    old = register3_;
    register3_ = value;

//    p_Main->eventMessageHex(value);
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

void V1870::out4_1870(Word address)
{
    if (address != register4_)
    {
        register4_ = address;
        tone(register4_);
    }
}

void V1870::out5_1870(Word address)
{
    Word old;

    old = register5_;
    register5_ = address;

    if (((old & 0x8) != (register5_ & 0x8)) && (computerType_ == COMX))
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
            if ((register5_ & 0x28) == 8)
                videoHeight_ = 192;
            else
                videoHeight_ = 216;
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

    if ((old & 0xe8) != (register5_ & 0xe8))
    {
        linesPerCharacters_ = ((register5_ & 0x8) == 0x8) ? 8 : 9;
        linesPerCharacters_ = ((register5_ & 0x20) == 0x20) ? 16 : linesPerCharacters_;
        pageMemoryMask_ = ((register5_ & 0x40) == 0x40) ? 0x7ff : 0x3ff;
        if (computerType_ == CIDELSA)
            maxPageMemory_ = ((register5_ & 0x40) == 0x40) ? 1920 : 1000;
        else
            maxPageMemory_ = ((register5_ & 0x40) == 0x40) ? 1920 : 960;
        if ((linesPerCharacters_ == 16) || (linesPerCharacters_ == 9) || (computerType_ == COMX))
            CmaMask_ = 0xf;
        else
            CmaMask_ = 0x7;

        pixelHeight_ = ((register5_ & 0x80) == 0x80) ? 1 : 2;
        displayPeriodEnd_ = nonDisplayPeriodEnd_ - (displayPeriod_ / (pixelWidth_ * pixelHeight_));
        if (computerType_ == CIDELSA)
        {
            if (cidelsaGame_ == DRACO)
                rowsPerScreen_ = ((register5_ & 0x80) == 0x80) ? 27 : 12;
            else
                rowsPerScreen_ = ((register5_ & 0x80) == 0x80) ? 25 : 12;
        }
        else
            rowsPerScreen_ = ((register5_ & 0x80) == 0x80) ? 24 : 12;
        if (computerType_ == TMC600)
        {
            if ((pixelHeight_ == 2) && (pixelWidth_ == 2))
                maxPageMemory_ = 960/4;
            else
                maxPageMemory_ = 960;
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
        noise(register5_);
    //p_Main->messageInt(linesPerCharacters_);
    //p_Main->messageInt(rowsPerScreen_);
}

void V1870::out6_1870(Word address)
{
    register6_ = (address & 0x7ff);
}

void V1870::out7_1870(Word address)
{
    Word old;

    old = register7_;
    register7_ = (address & pageMemoryMask_ & 0x7fc);

    if (mc6845started_)  return;
    if ((pixelWidth_ == 1) && (pixelHeight_ == 1) && ((computerType_ == COMX) || (computerType_ == TMC600) || (computerType_ == PECOM) || (computerType_ == MICROBOARD) || (computerType_ == XML)))
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

void V1870::cycle1870()
{
    cycleValue_ --;
    if (cycleValue_ == preDisplayPeriod_)
    {
        if (!displayOff_)
        {
            ef1Value_ = efDisplay_;
            if (computerType_ == MICROBOARD || computerType_ == XML)
            {
                if (interruptMode_ == INT_MODE1)
                    p_Computer->interrupt();
                if (interruptEnabled_ && interruptMode_ == INT_MODE3)
                    p_Computer->interrupt();
            }
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

            if (mc6845started_)
                p_Main->set6845Size(computerType_, size);
            else
                p_Main->setMainSize(computerType_, size);

            changeScreenSize();
            if (!fullScreenSet_)
                p_Main->v1870BarSizeEvent();
            changeScreenSize_ = false;
        }
        if (!mc6845started_)  
            copyScreen();

/*        if (!realCassetteLoad_ && steps_ != 0)
        {
            playSound();
        }*/
    }
    if (cycleValue_ == displayPeriodEnd_)
    {
        ef1Value_ = efNonDisplay_;
        nonDisplay_ = true;
        videoSyncCount_++;
        switch (computerType_)
        {
            case COMX:
                if (!displayOff_)
                    p_Computer->interrupt();
            break;

            case XML:
            case MICROBOARD:
                if (!displayOff_)
                {
                    if (interruptMode_ == INT_MODE2)
                        p_Computer->interrupt();
                    if (interruptEnabled_ && interruptMode_ == INT_MODE4)
                        p_Computer->interrupt();
                }
            break;

            case CIDELSA:
                if (cidelsaGame_ != DRACO)
                    p_Computer->interrupt();
            break;
        }
    }
    if (cycleValue_ <= 0)
        cycleValue_ = cycleSize_;
}

void V1870::blink1870()
{
    blinkValue_ --;
    if (blinkValue_ == 0)
    {
        blinkValue_ = blinkSize_;
        vismacBlink_ = !vismacBlink_;
        reDraw_ = true;
        if (!mc6845started_)  
            copyScreen();
    }
}

void V1870::writePram(Word address, Byte v)
{
    if (!nonDisplay_ && computerType_ != CIDELSA)
        return;

    if (cmemAccessMode_)
        address = register6_ & pageMemoryMask_;
    else
    {
        if (!nonDisplay_)
            return;
        address &= pageMemoryMask_;
    }

    address &= pageMemorySize_;

    pageMemory_[address] = v;
    vismacColorRam_[address] = vismacColorLatch_;

    if (address>= memoryStart_ && address<(memoryStart_+256))
        p_Main->updateDebugMemory(address);

    if ((address <(charactersPerRow_ * rowsPerScreen_)) || (pixelWidth_ == 2) || (pixelHeight_ == 2) || (computerType_ == CIDELSA))
    {
        int a = address - register7_;//((register7_ / charactersPerRow_) * charactersPerRow_);
        while(a < 0) a += maxPageMemory_;
        int x = (a % charactersPerRow_) * 6;
        int y = (a / charactersPerRow_) * linesPerCharacters_;
        drawCharacterAndBackground(x, y, v, address);
// ** address log, comment out next line
        p_Main->assLog(v);
    }
}

void V1870::writeCram(Word address, Byte v)
{
    if (!nonDisplay_ && computerType_ != CIDELSA)
        return;

    Word ac;
    if (cmemAccessMode_)
        ac = register6_ & pageMemoryMask_;
    else
    {
        if (!nonDisplay_)
            return;
        ac = address & pageMemoryMask_;
    }
    
    address &= CmaMask_;
    address += ((pageMemory_[ac]&pcbMask_) * maxLinesPerCharacters_);
    
    if (charMemoryIsRom_ && address >= romAddress_)
        return;
    
    characterMemory_[address&charMemorySize_] = v;

    if (address>= memoryStart_ && address<(memoryStart_+256))
        p_Main->updateDebugMemory(address);

    if (computerType_ == CIDELSA)
        v1870pcb_[address] = (flipFlopQ_) ? 1 : 0;
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
    return;
}

Byte V1870::readPram(Word address)
{
    if (computerType_ == COMX)
    {
        if (isEpromBoardLoaded() || isSuperBoardLoaded())
        {
            return mainMemory_ [address];
        }
    }
    

    if (!nonDisplay_)
        return 0xff;

    if (cmemAccessMode_)
    {
        return pageMemory_[register6_ & pageMemoryMask_ & pageMemorySize_];
    }
    return pageMemory_[address & pageMemoryMask_ & pageMemorySize_];
}

Byte V1870::readCram(Word address)
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
    if (computerType_ == CIDELSA)
    {
        p_Computer->cid1Bit8((v1870pcb_[address+(pageMemory_[ac])*maxLinesPerCharacters_] == 0));
    }
    address += ((pageMemory_[ac]&pcbMask_) * maxLinesPerCharacters_);
    if (computerType_ == TMC600)
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

Byte V1870::readColourRamDirect(Word address)
{
    switch (computerType_)
    {
        case TMC600:
            return vismacColorRam_[address] & 0xf;
        break;
            
        case CIDELSA:
            return ((characterMemory_[address&charMemorySize_] & 0xc0) >> 6) | ((v1870pcb_[address] & 0x1) << 2);
        break;
            
        default:
            return 0;
        break;
    }
}

void V1870::writeColourRamDirect(Word address, Byte value)
{
    switch (computerType_)
    {
        case TMC600:
            vismacColorRam_[address] = value & 0xf;
        break;
            
        case CIDELSA:
            characterMemory_[address&charMemorySize_] = (characterMemory_[address&charMemorySize_] & 0x3f) | ((value << 6) & 0xc0);
            v1870pcb_[address] = (value & 0x4) >> 2;
        break;
    }
    reDraw_ = true;
}

void V1870::set1870(wxString Register, long value)
{
    if (Register == "3")
    {
        out3_1870(value);
    }
    if (Register == "4")
    {
        out4_1870(value);
    }
    if (Register == "5")
    {
        out5_1870(value);
    }
    if (Register == "6")
    {
        out6_1870(value);
    }
    if (Register == "7")
    {
        out7_1870(value);
    }
}

void V1870::copyScreen()
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
        drawExtraBackground(colour_[backGround_]);

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
        if (computerType_ == CIDELSA)
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

void V1870::drawScreen()
{
    int char_screen;
    wxCoord x,y;
    int address;

    setColour(backGround_);
    drawRectangle(0, 0, videoWidth_ + 2*offsetX_, videoHeight_ + 2*offsetY_);

    if (displayOff_)
    {
        return;
    }
    char_screen = charactersPerRow_ * rowsPerScreen_;
    address = register7_;
    //address = (address/charactersPerRow_) * charactersPerRow_; this stops base address to be != 0x28, like F804
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

void V1870::drawCharacter(wxCoord x, wxCoord y, Byte v, int address)
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

void V1870::drawCharacterAndBackground(wxCoord x, wxCoord y, Byte v, int address)
{
    Byte pcb = (v & 0x80) ? 1 : 0;
    if (displayOff_)
        return;
    v &= pcbMask_;
    int a = v * maxLinesPerCharacters_;
    for (wxCoord i=y; i<y+linesPerCharacters_; i++)
    {
        if (pcbMask_  == 0xff) pcb = v1870pcb_[a];
        setColourMutex(backGround_);
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

void V1870::drawLine(wxCoord x,wxCoord y,Byte v,Byte pcb, int address)
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
    if (computerType_ == TMC600)
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
     
    setColourMutex(clr);
    line_byte = v;
    for (wxCoord i=x; i<x+(6*pixelWidth_); i+=pixelWidth_)
    {
        if (line_byte & 128)
            drawPoint(i, y);
        line_byte <<= 1;
    }
}

void V1870::drawPoint(wxCoord x, wxCoord y)
{
    if ((pixelHeight_ == 1) &&(pixelWidth_ == 1))
    {
        if (computerType_ == CIDELSA)
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
        if (computerType_ == CIDELSA)
        {
            drawRectangleMutex(videoWidth_ - 1 -y + offsetX_, x + offsetY_, pixelHeight_, pixelWidth_);
        }
        else
        {

            drawRectangleMutex(x + offsetX_, y + offsetY_, pixelWidth_, pixelHeight_);
        }
    }
}

void V1870::drawBackgroundLine(wxCoord x, wxCoord y)
{
    if (computerType_ == CIDELSA)
    {
        drawRectangleMutex(videoWidth_ - 1 -y + offsetX_, x + offsetY_, pixelHeight_, 6*pixelWidth_);
    }
    else
    {
        drawRectangleMutex(x + offsetX_, y + offsetY_, 6*pixelWidth_, pixelHeight_);
    }
}

void V1870::reDrawBar()
{
    if (computerType_ == COMX)
    {
        if (p_Main->isDiagActive(COMX))
            diagStatusBarPointer->reDrawBar();
        else
            comxStatusBarPointer->reDrawBar();
        updateExpansionLed(true);
    }
    else if (computerType_ == CIDELSA)
        if (cidelsaGame_ != DRACO)
            cidelsaStatusBarPointer->reDrawBar();
}

void V1870::updateExpansionLed(bool status)
{
    p_Main->eventUpdateComxLedStatus(expansionSlot_, 0, status);
}

void V1870::updateStatusLed(bool status)
{
    p_Main->eventUpdateComxLedStatus(expansionSlot_, 1, status);
}

void V1870::updateLedStatus(int card, int i, bool status)
{
    if (p_Main->isDiagActive(COMX))
        diagStatusBarPointer->updateLedStatus(0, status);
    else
        comxStatusBarPointer->updateLedStatus(card, i, status);
}

void V1870::updateDiagLedStatus(int led, bool status)
{
    if (p_Main->isDiagActive(COMX))
        diagStatusBarPointer->updateLedStatus(led, status);
}

void V1870::updateCidelsaLedStatus(int number, bool status)
{
    cidelsaStatusBarPointer->updateLedStatus(number, status);
}

bool V1870::readChargenFileTmc(wxString romDir, wxString romFile)
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

bool V1870::readChargenFile(wxString romDir, wxString romFile)
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

Byte V1870::ef6845()
{
    return(cycleValue6845_ < nonDisplay6845_) ? 0 : 1;
}

void V1870::writeRegister6845(Byte value)
{
    registerIndex_ = value&0x1f;
}

Byte V1870::readData6845()
{
    Byte Ret = 0;

    switch(registerIndex_)
    {
        case 14:
            Ret = register_[registerIndex_];
        break;

        case 15:
            Ret = register_[registerIndex_];
        break;
    }
    return Ret;
}

void V1870::writeData6845(Byte value)
{
    switch(registerIndex_)
    {
        case 0:
            if ((value + 1) != Nht_)
            {
                Nht_= value + 1;
                reCycle_ = true;
            }
        break;

        case 1:
            if (value != charLine_)
            {
                charLine_ = value;
                if (mc6845started_)
                    videoWidth_ = charLine_*MC6845CHARW;
                reDraw_ = true;
                if (mc6845started_)
                    setScreenSize();
                reCycle_ = true;
            }
        break;

        case 4:
            if ((value + 1) != Nvt_)
            {
                Nvt_= value + 1;
                reCycle_ = true;
            }
        break;

        case 5:
            if (value != Nadj_)
            {
                Nadj_= value;
                reCycle_ = true;
            }
        break;

        case 6:
            if (value != rows_)
            {
                rows_ = value;
                if (mc6845started_)
                    videoHeight_ = rows_*scanLine_*2;
                reDraw_ = true;
                if (mc6845started_)
                    setScreenSize();
            }
        break;

        case 8:
            interlace_ = ((value&1) == 1);
            if (interlaceGUI_ == true)
                interlace_ = 1;

            if ((value&0x3) == 3)
                videoM_ = 1;
            else
                videoM_ = 2;

            reDraw_ = true;
        break;

        case 9:
            if (value != (scanLine_-1))
            {
                value &= 0x1f;
                scanLine_ = value + 1;
                if (mc6845started_)
                    videoHeight_ = rows_*scanLine_*2;
                reDraw_ = true;
                if (mc6845started_)
                    setScreenSize();
                reCycle_ = true;
            }
        break;

        case 10:
            if ((value&0x7f) != register_[registerIndex_])
            {
                value &= 0x7f;
                cursorStartLine_ = value & 0x1f;
                if (cursorStartLine_ > 15)  cursorStartLine_ = 15;
                switch(value & 0x60)
                {
                    case 0x00:
                        cursorOn_ = true;
                        cursorBlink_ = false;
                        cursorBlinkTime_ = 16;
                    break;

                    case 0x20:
                        cursorOn_ = false;
                    break;

                    case 0x40:
                        cursorOn_ = true;
                        cursorBlink_ = true;
                        cursorBlinkTime_ = 16;
                    break;

                    case 0x60:
                        cursorOn_ = true;
                        cursorBlink_ = true;
                        cursorBlinkTime_ = 32;
                    break;
                }
            }
        break;

        case 11:
            if ((value&0x1f) != register_[registerIndex_])
            {
                value &= 0x1f;
                cursorEndLine_ = value;
                if (cursorEndLine_ > 15)  cursorEndLine_ = 15;
            }
        break;

        case 12:
            if ((value&0x3f) != register_[registerIndex_])
            {
                value &= 0x3f;
                startAddress_ = (startAddress_ & 0xff) + (value << 8);
                reDraw_ = true;
            }
        break;

        case 13:
            if (value != register_[registerIndex_])
            {
                startAddress_ = (startAddress_ & 0x3f00) + value;
                reDraw_ = true;
            }
        break;

        case 14:
            if ((value&0x3f) != register_[registerIndex_])
            {
                value &= 0x3f;
                if (mc6845started_)
                    draw6845(cursorAddress_& 0x7ff, mc6845ram_[cursorAddress_& 0x7ff]);
                cursorAddress_ = (cursorAddress_ & 0xff) +(value << 8);
            }
        break;

        case 15:
            if (value != register_[registerIndex_])
            {
                if (mc6845started_)
                    draw6845(cursorAddress_& 0x7ff, mc6845ram_[cursorAddress_& 0x7ff]);
                cursorAddress_ = (cursorAddress_ & 0x3f00) + value;
            }
        break;
    }
    register_[registerIndex_] = value;
}

void V1870::cycle6845()
{
    cycleValue6845_--;
    if (cycleValue6845_ <= 0)
        cycleValue6845_ = cycleSize6845_;
}

void V1870::blink6845()
{
    blinkValue6845_--;
    if (blinkValue6845_ <= 0)
    {
        blinkValue6845_ = blinkSize6845_;
//#ifndef __WXMAC__
        copyScreen6845();
//#endif
        videoSyncCount_++;
        blink_--;
        if (blink_ <= 0)
        {
            blink_ = cursorBlinkTime_;
            if (cursorOn_)
            {
                if (cursorBlinkOn_ || !cursorBlink_)
                    cursorBlinkOn_ = false;
                else
                    cursorBlinkOn_ = true;
            }
        }
#ifndef __WXMAC__
        if (cursorOn_)
            drawCursor6845(cursorAddress_, cursorBlinkOn_);
#else
        p_Main->eventRefreshVideo(false, videoNumber_);
#endif
    }
}

Byte V1870::read6845(Word addr)
{
     return mc6845ram_[addr];
}

void V1870::write6845(Word addr, Byte value)
{
    draw6845(addr, value);
    p_Main->assLog(value);
}

Byte V1870::read6845CharRom(Word addr)
{
     return mc6845CharRom_[addr];
}

void V1870::write6845CharRom(Word addr, Byte value)
{
    mc6845CharRom_[addr] = value;
}

void V1870::copyScreen6845()
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
        drawScreen6845();

#if defined(__WXMAC__)
    if (reBlit_ || reDraw_)
    {
        p_Main->eventRefreshVideo(false, videoNumber_);
        reBlit_ = false;
        reDraw_ = false;
    }
#else
    if (extraBackGround_ && newBackGround_)
        drawExtraBackground(colour_[BACK]);

    CharacterList *temp;

    if (reBlit_ || reDraw_)
    {
        videoScreenPointer->blit(0, 0, videoWidth_+2*offsetX_, rows_*scanLine_*videoM_+2*offsetY_, &dcMemory, 0, 0);
        if (updateCharacter6845_ > 0 )
        {
            updateCharacter6845_ = 0;
            while(characterListPointer6845 != NULL)
            {
                temp = characterListPointer6845;
                characterListPointer6845 = temp->nextCharacter;
                delete temp;
            }
        }
        reBlit_ = false;
        reDraw_ = false;
    }
    if (updateCharacter6845_ > 0)
    {
        updateCharacter6845_ = 0;
        while(characterListPointer6845 != NULL)
        {
            videoScreenPointer->blit(offsetX_+(characterListPointer6845->x), offsetY_+(characterListPointer6845->y), MC6845CHARW, scanLine_*videoM_, &dcMemory, offsetX_+characterListPointer6845->x, offsetY_+characterListPointer6845->y);
            temp = characterListPointer6845;
            characterListPointer6845 = temp->nextCharacter;
            delete temp;
        }
    }
#endif
}

void V1870::drawScreen6845()
{
    int addr = startAddress_;
    setColour(BACK);
    drawRectangle(0, 0, videoWidth_ + 2*offsetX_, videoHeight_ + 2*offsetY_);
    for (int i=0; i<(charLine_*rows_); i++)
    {
        draw6845(addr, mc6845ram_[addr]);

        addr++;
        addr &= 0x7ff;
    }
}

void V1870::draw6845(Word addr, Byte value)
{
    mc6845ram_[addr] = value;

    if (p_Main->isZoomEventOngoing())
    {
        reDraw_ = true;
        return;
    }
    
    addr = (addr - startAddress_) & 0x7ff;

    int y = (addr/charLine_)*scanLine_*videoM_;
    int x = (addr%charLine_)*MC6845CHARW;
    drawCharacter6845(x, y, value);
}

void V1870::drawCharacter6845(wxCoord x, wxCoord y, Byte v)
{
    int line_byte, line;

    setColour(BACK);
    drawRectangle(x+offsetX_, y+offsetY_, MC6845CHARW, scanLine_*videoM_);
    setColour(FORE);

    if (v >= 0x80)
    {
        if (interlace_ & !(videoM_ == 1))
            drawRectangle(x+offsetX_, y+offsetY_+(scanLine_-1)*videoM_, MC6845CHARW, 2);
        else
            drawRectangle(x+offsetX_, y+offsetY_+(scanLine_-1)*videoM_, MC6845CHARW, 1);
    }

#if defined(__WXMAC__) || defined(__linux__)
    reBlit_ = true;
#else
    if (zoomFraction_)
        reBlit_ = true;
    else
    {
        CharacterList *temp = new CharacterList;
        temp->x = x;
        temp->y = y;
        temp->nextCharacter = characterListPointer6845;
        characterListPointer6845 = temp;
        updateCharacter6845_++;
//        if (updateCharacter6845_ > 40)
//            reBlit_ = true;
    }
#endif

    line = 0;
    for (wxCoord j=y; j<y+(scanLine_-1)*videoM_; j+=videoM_)
    {
        line_byte =    mc6845CharRom_[v*8+line];
        for (wxCoord i=x; i<x+MC6845CHARW; i++)
        {
            if (line_byte & 128)
            {
                if (interlace_ & !(videoM_ == 1))
                    drawRectangle(i+offsetX_, j+offsetY_, 1, videoM_);
                else
                    drawPoint(i+offsetX_, j+offsetY_);
            }
            line_byte <<= 1;
        }
        line++;
    }
}

void V1870::drawCursor6845(Word addr, bool status)
{
    Byte v;
    int line_byte, line;
    wxColour clr;

    addr = (addr - startAddress_) & 0x7ff;

    int y = (addr/charLine_)*scanLine_*videoM_;
    int x = (addr%charLine_)*MC6845CHARW;

    v = mc6845ram_[addr];
    line = cursorStartLine_;
    for (int yLine = y + cursorStartLine_*videoM_; yLine <= (y + cursorEndLine_*videoM_); yLine+=videoM_)
    {
        if (yLine == (y + (scanLine_-1)*videoM_))
        {
            if (v<0x80)
            {
                if (status)
                    clr = colour_[FORE];
                else
                    clr = colour_[backGround_];
            }
            else
            {
                if (status)
                    clr = colour_[backGround_];
                else
                    clr = colour_[FORE];
            }
            if (interlace_ & !(videoM_ == 1))
                videoScreenPointer->drawRectangle(clr, (x+offsetX_)*zoom_, (yLine+offsetY_)*zoom_, MC6845CHARW*zoom_, 2*zoom_);
            else
                videoScreenPointer->drawRectangle(clr, (x+offsetX_)*zoom_, (yLine+offsetY_)*zoom_, MC6845CHARW*zoom_, zoom_);
        }
        else
        {
            line_byte =    mc6845CharRom_[v*8+line];
            for (wxCoord i=x; i<x+MC6845CHARW; i++)
            {
                if (line_byte & 128)
                {
                    if (status)
                        clr = colour_[backGround_];
                    else
                        clr = colour_[FORE];
                }
                else
                {
                    if (status)
                        clr = colour_[FORE];
                    else
                        clr = colour_[backGround_];
                }
                if (interlace_ & !(videoM_ == 1))
                    videoScreenPointer->drawRectangle(clr, (i+offsetX_)*zoom_, (yLine+offsetY_)*zoom_, zoom_, 2*zoom_);
                else
                    videoScreenPointer->drawRectangle(clr, (i+offsetX_)*zoom_, (yLine+offsetY_)*zoom_, zoom_, zoom_);
                line_byte <<= 1;
            }
        }
        line++;
    }
#if defined(__linux__)
    this->Update();
#endif
}

void V1870::drawCursor6845(wxDC &dc, Word addr, bool status)
{
    Byte v;
    int line_byte, line;
    wxColour clr;

    addr = (addr - startAddress_) & 0x7ff;

    int y = (addr/charLine_)*scanLine_*videoM_;
    int x = (addr%charLine_)*MC6845CHARW;

    v = mc6845ram_[addr];
    line = cursorStartLine_;
    for (int yLine = y + cursorStartLine_*videoM_; yLine <= (y + cursorEndLine_*videoM_); yLine+=videoM_)
    {
        if (yLine == (y + (scanLine_-1)*videoM_))
        {
            if (v<0x80)
            {
                if (status)
                    clr = colour_[FORE];
                else
                    clr = colour_[backGround_];
            }
            else
            {
                if (status)
                    clr = colour_[backGround_];
                else
                    clr = colour_[FORE];
            }
            dc.SetBrush(wxBrush(clr));
            dc.SetPen(wxPen(clr));
            if (interlace_ & !(videoM_ == 1))
                dc.DrawRectangle(x+offsetX_, yLine+offsetY_, MC6845CHARW, 2);
            else
                dc.DrawRectangle(x+offsetX_, yLine+offsetY_, MC6845CHARW, 1);
        }
        else
        {
            line_byte =    mc6845CharRom_[v*8+line];
            for (wxCoord i=x; i<x+MC6845CHARW; i++)
            {
                if (line_byte & 128)
                {
                    if (status)
                        clr = colour_[backGround_];
                    else
                        clr = colour_[FORE];
                }
                else
                {
                    if (status)
                        clr = colour_[FORE];
                    else
                        clr = colour_[backGround_];
                }
                dc.SetBrush(wxBrush(clr));
                dc.SetPen(wxPen(clr));
                if (interlace_ & !(videoM_ == 1))
                    dc.DrawRectangle(i+offsetX_, yLine+offsetY_, 1, 2);
                else
                    dc.DrawRectangle(i+offsetX_, yLine+offsetY_, 1, 1);
                line_byte <<= 1;
            }
        }
        line++;
    }
#if defined(__linux__)
    this->Update();
#endif
}

void V1870::setInterlace(bool status)
{
    interlaceGUI_ = status;
    interlace_ = status;
    if (status)
        register_[8] |= 1;
    else
        register_[8] &= 0xfe;
    reDraw_ = true;
}

void V1870::setClock(double clock)
{
    clock_ = clock;
    reCycle_ = true;
}

void V1870::setCycle()
{
    float clockPeriod, scanLinePeriod, characterRowPeriod;
    int fieldTime;

    switch (computerType_)
    {
        case COMX:
            if (videoMode_ == PAL)
            {  // DOT = 5.626
                clockPeriod = (float)((1/5.626) * 6);
                fieldTime = clockPeriod * 60 * 312;

                cycleSize_ = (int)(fieldTime / ((1/clock_) * 8));

                if (mc6845started_)
                    displayPeriod_ = (int) ((float) cycleSize_ / 312 * save_v1870Y_);
                else
                    displayPeriod_ = (int) ((float) cycleSize_ / 312 * videoHeight_);
                nonDisplayPeriodEnd_ = (int) ((float) cycleSize_ / 312 * 268);
                preDisplayPeriod_ = (int) ((float) cycleSize_ / 312 * 269);
            }
            else
            {  // DOT = 5.67
                clockPeriod = (float)((1/5.67) * 6);
                fieldTime = clockPeriod * 60 * 262;

                cycleSize_ = (int)(fieldTime / ((1/clock_) * 8));

                if (mc6845started_)
                    displayPeriod_ = (int) ((float) cycleSize_ / 262 * save_v1870Y_);
                else
                    displayPeriod_ = (int) ((float) cycleSize_ / 262 * videoHeight_);
                nonDisplayPeriodEnd_ = (int) ((float) cycleSize_ / 262 * 226);
                preDisplayPeriod_ = (int) ((float) cycleSize_ / 262 * 227);
            }
            clockPeriod = (float)(1/14.318180 * 7); // DOT = 14.31818
            scanLinePeriod = (float) Nht_ * clockPeriod;
            characterRowPeriod = (float) (scanLine_ + 1) * scanLinePeriod;
            fieldTime = (int)((float) (Nvt_ * characterRowPeriod) + (Nadj_ * scanLinePeriod));

            cycleSize6845_ = (int)(scanLinePeriod / ((1/clock_) * 8));
            nonDisplay6845_ = cycleSize6845_ - (int)((charLine_ * clockPeriod) / ((1/clock_) * 8));
            blinkSize6845_ = (int)(fieldTime / ((1/clock_) * 8));

            cycleValue6845_ = cycleSize6845_;
            blinkValue6845_ = blinkSize6845_;
        break;

        case CIDELSA: 
            clockPeriod = (float)((1/5.626) * 6);
            fieldTime = clockPeriod * 60 * 312;

            cycleSize_ = (int)(fieldTime / ((1/clock_) * 8));
            blinkSize_ = cycleSize_ * 25;

            displayPeriod_ = (int) ((float) cycleSize_ / 312 * videoWidth_);
            nonDisplayPeriodEnd_ = (int) ((float) cycleSize_ / 312 * 268);
            preDisplayPeriod_ = (int) ((float) cycleSize_ / 312 * 269);
        break;

        case XML:
        case MICROBOARD:
        case TMC600:
        case PECOM:  // DOT = 5.626
            clockPeriod = (float)((1/5.626) * 6);
            fieldTime = clockPeriod * 60 * 312;

            cycleSize_ = (int)(fieldTime / ((1/clock_) * 8));
            blinkSize_ = cycleSize_ * 25;

            displayPeriod_ = (int) ((float) cycleSize_ / 312 * videoHeight_);
            nonDisplayPeriodEnd_ = (int) ((float) cycleSize_ / 312 * 268);
            preDisplayPeriod_ = (int) ((float) cycleSize_ / 312 * 269);
        break;

    }
    cycleValue_ = cycleSize_;
    blinkValue_ = blinkSize_;
    nonDisplay_ = true;
    ef1Value_ = efNonDisplay_;
    displayPeriodEnd_ = nonDisplayPeriodEnd_ - (displayPeriod_ / (pixelWidth_ * pixelHeight_));
    reCycle_ = false;
}

void V1870::setFullScreen(bool fullScreenSet)
{
    fullScreenSet_ = fullScreenSet;
#ifdef __WXMAC__
    if (fullScreenSet)
        SetStatusBar(NULL);
    else
    {
        if (computerType_ == COMX)
        {
            if (p_Main->isDiagActive(COMX))
                SetStatusBar(diagStatusBarPointer);
            else
                SetStatusBar(comxStatusBarPointer);
        }
        if (computerType_ == CIDELSA)
            SetStatusBar(cidelsaStatusBarPointer);
    }
#endif
    ShowFullScreen(fullScreenSet);
}

void V1870::onF3()
{
    fullScreenSet_ = !fullScreenSet_;
    p_Main->eventVideoSetFullScreen(fullScreenSet_, videoNumber_);
}

void V1870::reBlit(wxDC &dc)
{
    if (!v1870Configured_)
        return;

    if (!memoryDCvalid_)
        return;

    if (videoType_ != VIDEO80COL)
        dc.Blit(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_, &dcMemory, 0, 0);
    else
        dc.Blit(0, 0, videoWidth_+2*offsetX_, rows_*scanLine_*videoM_+2*offsetY_, &dcMemory, 0, 0);
    
    if (extraBackGround_ && newBackGround_)
    {
        wxSize size = wxGetDisplaySize();

        dc.SetBrush(wxBrush(colour_[backGround_]));
        dc.SetPen(wxPen(colour_[backGround_]));

        int xStart = (int)((2*offsetX_+videoWidth_)*zoom_*xZoomFactor_);
        dc.DrawRectangle(xStart, 0, size.x-xStart, size.y);

        int yStart = (int)((2*offsetY_+videoHeight_)*zoom_);
        dc.DrawRectangle(0, yStart, size.x, size.y-yStart);

        newBackGround_ = false;
    }
    if (videoType_ == VIDEO80COL)
        drawCursor6845(dc, cursorAddress_, cursorBlinkOn_);
}

