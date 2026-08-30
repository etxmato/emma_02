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

#if defined(__linux__)
#include "app_icon.xpm"
#endif

#define FOREGROUND 2
#define BACKGROUND 3

#define SCN2672_VSYNC 3
#define SCN2672_INTERLACED 1
#define SCN2672_NOT_INTERLACED 2

#define SCN2672_LIGHT_PEN 0
#define SCN2672_READY 1
#define SCN2672_SPLIT_SCREEN 2
#define SCN2672_LINE_ZERO 3
#define SCN2672_VBLANK 4
#define SCN2672_RDFLG 5

#include "main.h"
#include "scn2672.h"

BEGIN_EVENT_TABLE(Scn2672, wxFrame)
    EVT_CLOSE (Scn2672::onClose)
    EVT_SIZE(Scn2672::onSize)
END_EVENT_TABLE()

Scn2672::Scn2672(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double clock, Scn2672Configuration scn2672Configuration, Crt8002Configuration crt8002Configuration)
: Video(title, pos, size)
{
//    int regVal [11] = {0x38, 0x11, 0x11, 0xb4, scn2672Configuration_.screenSize.y-1, scn2672Configuration_.screenSize.x-1, 7, 0x20, 0x80, 0x10, 0x12};

    memoryDCBeingUsed_ = false;
    clock_ = clock;
    videoNumber_ = scn2672Configuration.videoNumber;
    scn2672Configuration_ = scn2672Configuration;
    crt8002Configuration_ = crt8002Configuration;

    xZoomFactor_ = scn2672Configuration.xScale;
    if (!p_Main->isFullScreenFloat())
        xZoomFactor_ = (int) xZoomFactor_;

    windowSize_ = size;
    colourIndex_ = 0;
    registerIndex_ = 0;
    displayStart_ = 0;
	displayEnd_ = 0;
    screenStart_ = 0;
    cursorAddress_ = 0;
    pointerAddress_ = 0;
    statusRegister_ = 0;
    interruptRegister_ = 0;
    displayBuffer_ = 0;
    doubleHeightCharacter_ = false;
    
    interruptReady_ = false;
    interruptSplitScreen_ = false;
    interruptLineZero_ = false;
    interruptVerticalBlank_ = false;

    currentY_ = 0;
    curentRamAddress_ = screenStart_;
    
    videoWidth_ = size.x;
    videoHeight_ = size.y;
    
    cursorOn_ = false;
    displayOn_ = false;
    setVideoConfiguration_ = false;
    initializationRegistersLoaded_ = false;

    videoType_ = VIDEOSCN2672;
    colourIndex_ = COL_SCN2672_FORE-2;

    interlace_ = SCN2672_INTERLACED;
    interlaceOR_ = p_Main->getInterlace();
    p_Main->setScn2672SelectorValue(SCN2672_SEL_INTERLACE, 1 | (p_Main->getInterlace() << 1), DO_NOT_SHOW_ANY_TRACE);

    readCharRomFile(p_Main->getCharRomDir(), p_Main->getCharRomFile());

    switch (p_Main->getCpuStartupVideoRam())
    {
        case STARTUP_ZEROED:
            for (int i=0; i<32768; i++)
            	scn2672ram_[i] = 0;
        break;
            
        case STARTUP_RANDOM:
            for (int i=0; i<32768; i++)
            	scn2672ram_[i] = rand() % 0x100;
        break;
            
        case STARTUP_DYNAMIC:
            p_Computer->setDynamicRandomByte();
            for (int i=0; i<32768; i++)
            	scn2672ram_[i] = p_Computer->getDynamicByte(i);
        break;
    }
    for (int x=0; x<256; x++) 
    {
    	for (int y=0; y<128; y++)
        {
            scn2672display_[x][y] = -1;
            scn2672displayAttribute_[x][y] = 0xff;
        }
    }
    
    fullScreenSet_ = false;
    zoom_ = zoom;

    double intPart;
    zoomFraction_ = (modf(zoom_, &intPart) != 0);

#ifndef __WXMAC__
    SetIcon(wxICON(app_icon));
#endif

    defineColours();

    offsetX_ = borderX_[videoType_];
    offsetY_ = borderY_[videoType_];

    screenCopyPointer = new wxBitmap(2*offsetX_+size.x, 2*offsetY_+size.y);
    dcMemory.SelectObject(*screenCopyPointer);

    // The software framebuffer is enabled on ALL platforms (see video.h/
    // video.cpp); render per-pixel drawing through it (one DrawBitmap per
    // frame instead of per-pixel draws). The graphics context is only created
    // on macOS, where the non-framebuffer fallback draw path needs it.
    // On Linux wxGraphicsContext::Create(dcMemory) creates a Cairo context
    // bound to the screenCopyPointer Pixmap; leaving it unguarded caused a
    // BadDrawable X error on exit when that Pixmap was freed out of order.
#if defined(__WXMAC__)
    gc = wxGraphicsContext::Create(dcMemory);
    gc->SetAntialiasMode(wxANTIALIAS_NONE);
#endif
    enableFramebufferMac();

    videoScreenPointer = new VideoScreen(this, size, zoom, videoNumber_, xZoomFactor_);
    cursorBlink_ = true;
    cursorBlinkOn_ = false;
    characterBlinkOn_ = false;
    characterBlinkTimeSize_ = 16;
    p_Main->setScn2672SelectorValue(SCN2672_SEL_CHAR_BLINK_RATE, false, DO_NOT_SHOW_ANY_TRACE);
    cursorBlinkTimeSize_ = 16;
    p_Main->setScn2672SelectorValue(SCN2672_SEL_CURSOR_BLINK_RATE, false, DO_NOT_SHOW_ANY_TRACE);
    activeCharactersPerRow_ = 40;
//    p_Main->setScn2672RegisterByte(SCN2672_CHAR_PER_ROW, (Byte)activeCharactersPerRow_, DO_NOT_SHOW_ANY_TRACE);
    characterRowsPerScreen_ = 24;
//    p_Main->setScn2672RegisterByte(SCN2672_SCREEN_ROWS, (Byte)characterRowsPerScreen_, DO_NOT_SHOW_ANY_TRACE);
    scanLinesPerCharacterRow_ = 17;
//    p_Main->setScn2672RegisterByte(SCN2672_SCANLINES, (Byte)scanLinesPerCharacterRow_, DO_NOT_SHOW_ANY_TRACE);
    p_Main->setScn2672RegisterByte(SCN2672_CHAR_WIDTH, (Byte)scn2672Configuration_.charSize.x, DO_NOT_SHOW_ANY_TRACE);
    underLineScanLine_ = 0;
//    p_Main->setScn2672RegisterByte(SCN2672_UNDERLINE_LINE, (Byte)underLineScanLine_, DO_NOT_SHOW_ANY_TRACE);

    cursorBlinkTimeValue_ = cursorBlinkTimeSize_;
    characterBlinkTimeValue_ = characterBlinkTimeSize_;

    backGround_ = BACKGROUND;
    
    for (int registerNummber=0; registerNummber<11; registerNummber++)
    {
        initializationRegisterLoaded[registerNummber] = false;
        initializationRegister[registerNummber] = 0;
    }

    zoom_ = zoom;
    this->SetClientSize((videoWidth_+2*borderX_[videoType_])*zoom_*xZoomFactor_, (videoHeight_+2*borderY_[videoType_])*zoom_);
    this->SetBackgroundColour(colour_[colourIndex_+backGround_]);

    setCycle();
}

Scn2672::~Scn2672()
{
    dcMemory.SelectObject(wxNullBitmap);
    delete screenCopyPointer;
    delete videoScreenPointer;
#if defined(__WXMAC__)
    delete gc;
#endif
}

void Scn2672::configureScn2672()
{
    p_Main->configureMessage(&scn2672Configuration_.ioGroupVector, "SCN2672 Programmable Video Timing Controller");

    wxString printBuffer;
    if (scn2672Configuration_.initializationRegister.portNumber[0] != -1)
    {
        printBuffer.Printf("	%04X: Write Initialization registers", scn2672Configuration_.initializationRegister.portNumber[0]);
        p_Main->message(printBuffer);
    }
    if (scn2672Configuration_.interrupt.portNumber[0] != -1)
    {
        printBuffer.Printf("	%04X: Read Interrupt register", scn2672Configuration_.interrupt.portNumber[0]);
        p_Main->message(printBuffer);
    }
    if (scn2672Configuration_.command.portNumber[0] != -1)
    {
        printBuffer.Printf("	%04X: Write Command register", scn2672Configuration_.command.portNumber[0]);
        p_Main->message(printBuffer);
    }
    if (scn2672Configuration_.status.portNumber[0] != -1)
    {
        printBuffer.Printf("	%04X: Read Status register", scn2672Configuration_.status.portNumber[0]);
        p_Main->message(printBuffer);
    }
    if (scn2672Configuration_.screenStart.portNumber[0] != -1)
    {
        printBuffer.Printf("	%04X/%04X: Read or write Screen start address lower/upper register", scn2672Configuration_.screenStart.portNumber[0], scn2672Configuration_.screenStart.portNumber[0]+1);
        p_Main->message(printBuffer);
    }
    if (scn2672Configuration_.cursor.portNumber[0] != -1)
    {
        printBuffer.Printf("	%04X/%04X: Read or write Cursor address lower/upper register", scn2672Configuration_.cursor.portNumber[0], scn2672Configuration_.cursor.portNumber[0]+1);
        p_Main->message(printBuffer);
    }
    if (scn2672Configuration_.cursor.portNumber[0] != -1)
    {
        printBuffer.Printf("	%04X/%04X: Write Display pointer address lower/upper register", scn2672Configuration_.pointer.portNumber[0], scn2672Configuration_.pointer.portNumber[0]+1);
        p_Main->message(printBuffer);
    }
    if (scn2672Configuration_.data.portNumber[0] != -1)
    {
        printBuffer.Printf("	%04X: Read or write Display buffer", scn2672Configuration_.data.portNumber[0]);
        p_Main->message(printBuffer);
    }

    p_Computer->setCycleType(CYCLE_TYPE_BLINK_SCN2672, SCN2672_BLINK_CYCLE);
    p_Computer->setCycleType(CYCLE_TYPE_VIDEO_SCN2672, SCN2672_CYCLE);

    p_Main->message("");
}

void Scn2672::configureCrt8002()
{
    p_Main->configureMessage(&crt8002Configuration_.ioGroupVector, "CRT8002-018 Video-Controller Video Generator VDAC");
    wxString printBuffer;

    IoPort attribute;
    IoPort attributeScreen1;

    if (crt8002Configuration_.attribute.portNumber[0] != -1)
    {
        printBuffer.Printf("	%04X: Read or write Attribute type", crt8002Configuration_.attribute.portNumber[0]);
        p_Main->message(printBuffer);
    }
    if (crt8002Configuration_.attributeScreen1.portNumber[0] != -1)
    {
        printBuffer.Printf("	%04X: Read Main (Screen 1) attribute type", crt8002Configuration_.attributeScreen1.portNumber[0]);
        p_Main->message(printBuffer);
    }

    if (crt8002Configuration_.reverse.bitNumber != -1)
        p_Main->setCrt8002RegisterNibble(CRT8002_REVERSE,crt8002Configuration_.reverse.bitNumber, DO_NOT_SHOW_ANY_TRACE);
    if (crt8002Configuration_.blink.bitNumber != -1)
        p_Main->setCrt8002RegisterNibble(CRT8002_BLINK, crt8002Configuration_.blink.bitNumber, DO_NOT_SHOW_ANY_TRACE);
    if (crt8002Configuration_.graphic_ms0.bitNumber != -1)
        p_Main->setCrt8002RegisterNibble(CRT8002_GRAPHIC_MS0, crt8002Configuration_.graphic_ms0.bitNumber, DO_NOT_SHOW_ANY_TRACE);
    if (crt8002Configuration_.graphic_ms1.bitNumber != -1)
        p_Main->setCrt8002RegisterNibble(CRT8002_GRAPHIC_MS1, crt8002Configuration_.graphic_ms1.bitNumber, DO_NOT_SHOW_ANY_TRACE);
    if (crt8002Configuration_.underline.bitNumber != -1)
        p_Main->setCrt8002RegisterNibble(CRT8002_UNDERLINE, crt8002Configuration_.underline.bitNumber, DO_NOT_SHOW_ANY_TRACE);
    if (crt8002Configuration_.strikeThru.bitNumber != -1)
        p_Main->setCrt8002RegisterNibble(CRT8002_STRIKE_THRU, crt8002Configuration_.strikeThru.bitNumber, DO_NOT_SHOW_ANY_TRACE);
    if (crt8002Configuration_.blank.bitNumber != -1)
        p_Main->setCrt8002RegisterNibble(CRT8002_BLANK, crt8002Configuration_.blank.bitNumber, DO_NOT_SHOW_ANY_TRACE);
    if (crt8002Configuration_.underlineLine1 != -1)
        p_Main->setCrt8002RegisterNibble(CRT8002_UNDERLINE_LINE1, crt8002Configuration_.underlineLine1, DO_NOT_SHOW_ANY_TRACE);
    if (crt8002Configuration_.underlineLine2 != -1)
        p_Main->setCrt8002RegisterNibble(CRT8002_UNDERLINE_LINE2, crt8002Configuration_.underlineLine2, DO_NOT_SHOW_ANY_TRACE);
    if (crt8002Configuration_.strikeThruLine1 != -1)
        p_Main->setCrt8002RegisterNibble(CRT8002_STRIKE_THRU_LINE1, crt8002Configuration_.strikeThruLine1, DO_NOT_SHOW_ANY_TRACE);
    if (crt8002Configuration_.strikeThruLine2 != -1)
        p_Main->setCrt8002RegisterNibble(CRT8002_STRIKE_THRU_LINE2, crt8002Configuration_.strikeThruLine2, DO_NOT_SHOW_ANY_TRACE);

    p_Main->message("");
}

void Scn2672::initScn2672()
{
    lineCycleValueScn2672_ = 0;
    characterLineCycleValueScn2672_ = characterLineCycleSizeScn2672_;
    verticalRetraceCycleValueScn2672_ = 0;

    cursorBlinkValueScn2672_ = cursorBlinkSizeScn2672_;
    characterBlinkValueScn2672_ = characterBlinkSizeScn2672_;
    reDraw_ = true;
    reDrawOnNextCycle_ = true;
    reBlit_ = false;
    extraBackGround_ = true;
    newBackGround_ = true;
    attributeType_ = 0;
    lastAttributeType_ = 0;
}

int Scn2672::writeInitializationRegisterScn2672(Byte value, int showTrace)
{
    writeInitializationRegisterScn2672(registerIndex_++, value, showTrace);
    return p_Main->setScn2672RegisterByte(SCN2672_INITIALIZATION_REGISTER, value, showTrace);
}

int Scn2672::writeInitializationRegisterScn2672(Byte registerIndex, Byte value, int showTrace)
{
    statusRegister_[SCN2672_SPLIT_SCREEN] = 0;
    interruptRegister_[SCN2672_SPLIT_SCREEN] = 0;

    if (registerIndex  > 10 || initializationRegister[registerIndex] == value)
        return showTrace;
        
	return writeRegisterScn2672(registerIndex, value, showTrace);
}

int Scn2672::writeRegisterScn2672(Byte registerIndex, Word value, int showTrace)
{
	if (registerIndex <= 10)
    {
    	initializationRegister[registerIndex] = value;
        checkIfInitializationRegistersAreSet(registerIndex);
    }

    int realInterlace;
    int scanLines;
    
    switch (registerIndex)
    {
        case SCN2672_R0:
            scanLines = ((value & 0x78) >> 3) + 1;
            if (interlace_ == SCN2672_INTERLACED)
                scanLines = scanLines*2 + 1;
            setVideoConfiguration_ = true;
            return p_Main->setScn2672RegisterByte(SCN2672_SCANLINES, scanLines, showTrace);
        break;
            
        case SCN2672_R1:
    		showTrace = p_Main->setScn2672SelectorValue(SCN2672_SEL_INTERLACE, ((initializationRegister[1] & 0x80) >> 7) | (p_Main->getInterlace() << 1), showTrace);
            setVideoConfiguration_ = true;
            equalizingConstant_ = (value & 0x7F) + 1;
            calculateHorizontalFrontPorch();
            return p_Main->setScn2672RegisterByte(SCN2672_EQUALIZING_CONSTANT, (Byte)equalizingConstant_, showTrace);
        break;

        case SCN2672_R2:
            horizontalBackPorch_ = ((value & 0x7) * 4) + 1;
            horizontalSyncWidth_ = (((value >> 3) & 0xF) + 1) * 2;
            calculateHorizontalFrontPorch();
            setCycle();
            showTrace = p_Main->setScn2672RegisterByte(SCN2672_HORIZONTAL_SYNC_WIDTH, (Byte)horizontalSyncWidth_, showTrace);
            return p_Main->setScn2672RegisterByte(SCN2672_HORIZONTAL_BACK_PORCH, (Byte)horizontalBackPorch_, showTrace);
        break;

        case SCN2672_R3:
        	verticalBackPorch_ = ((value & 0x1f) * 2) + 4;
			verticalFrontPorch_ = ((value >> 5) + 1) * 4;
            setCycle();
            showTrace = p_Main->setScn2672RegisterByte(SCN2672_VERTICAL_FRONT_PORCH, (Byte)verticalFrontPorch_, showTrace);
            return p_Main->setScn2672RegisterByte(SCN2672_VERTICAL_BACK_PORCH, (Byte)verticalBackPorch_, showTrace);
        break;

        case SCN2672_R4:
            setVideoConfiguration_ = true;
            if ((value & 0x80) == 0x80)
        		characterBlinkTimeSize_ = 32;
        	else 
        		characterBlinkTimeSize_ = 16;
            showTrace = p_Main->setScn2672SelectorValue(SCN2672_SEL_CHAR_BLINK_RATE, (value & 0x80) == 0x80, showTrace);
            return p_Main->setScn2672RegisterByte(SCN2672_SCREEN_ROWS, (value & 0x7f) + 1, showTrace);
        break;

        case SCN2672_R5:
            setVideoConfiguration_ = true;
            return p_Main->setScn2672RegisterByte(SCN2672_CHAR_PER_ROW, value + 1, showTrace);
        break;

        case SCN2672_R6:
            setVideoConfiguration_ = true;
            showTrace = p_Main->setScn2672RegisterNibble(SCN2672_FIRST_CURSOR_LINE, initializationRegister[6] >> 4, showTrace);
            return p_Main->setScn2672RegisterNibble(SCN2672_LAST_CURSOR_LINE, initializationRegister[6] & 0xf, showTrace);
        break;

        case SCN2672_R7:
            cursorBlink_ = ((value & 0x20) >> 5) & 1;
            showTrace = p_Main->setScn2672SelectorValue(SCN2672_SEL_CURSOR_BLINK, cursorBlink_, showTrace);
            cursorBlinkOn_ = true;
            underLineScanLine_ = value & 0xf;
            doubleHeightCharacter_ = (value >> 4) & 0x1;
            reDraw_ = true;
            return p_Main->setScn2672RegisterNibble(SCN2672_UNDERLINE_LINE, (Byte)underLineScanLine_, showTrace);
        break;

        case SCN2672_R8:
        	displayStart_ = (displayStart_ & 0xf00) + value;
            reDraw_ = true;
            return p_Main->setScn2672Register12Bit(SCN2672_DISPLAY_BUFFER_START, displayStart_, showTrace);
        break;

        case SCN2672_R9:
        	displayStart_ = (displayStart_ & 0xff) + ((value & 0xf) << 8);
            showTrace = p_Main->setScn2672Register12Bit(SCN2672_DISPLAY_BUFFER_START, displayStart_, showTrace);
            displayEnd_ = ((value >> 4) + 1) * 1024 - 1;
            reDraw_ = true;
            return p_Main->setScn2672RegisterWord(SCN2672_DISPLAY_BUFFER_END, displayEnd_, showTrace);
        break;

        case SCN2672_R10:
        	if ((value & 0x80) == 0x80)
        		cursorBlinkTimeSize_ = 32;
        	else 
        		cursorBlinkTimeSize_ = 16;
        	showTrace = p_Main->setScn2672SelectorValue(SCN2672_SEL_CURSOR_BLINK_RATE, (value & 0x80) == 0x80, showTrace);
        	splitScreenInterruptRow_ = value & 0x7f;
            return p_Main->setScn2672RegisterByte(SCN2672_SPLIT_SCREEN_INT_ROW, (Byte)splitScreenInterruptRow_, showTrace);
        break;
        
        case SCN2672_INITIALIZATION_REGISTER:	// Initialization register
        	writeInitializationRegisterScn2672((Byte)value, showTrace);
        break;

        case SCN2672_COMMAND:	// Command
        	writeCommandScn2672((Byte)value, showTrace);
        break;

        case SCN2672_SCREEN_START:	// Screen start
        	writeScreenStartScn2672(value, showTrace);
        break;

        case SCN2672_CURSOR:	// Cursor
        	writeCursorScn2672(value, showTrace);
        break;

        case SCN2672_POINTER:	// Pointer
        	writePointerScn2672(value, showTrace);
        break;

        case SCN2672_DATA:	// Data
        	writeDataScn2672((Byte)value, showTrace);
        break;

        case SCN2672_SCANLINES:	// Scanlines
        	writeScanlinesScn2672((Byte)value, showTrace);
        break;
        
        case SCN2672_CHAR_WIDTH:	// Char width
        	writeCharWidthScn2672((Byte)value, showTrace);
        break;
        
        case SCN2672_SCREEN_ROWS:	// Screen rows
        	writeScreenRowsScn2672((Byte)value, showTrace);
        break;
        
        case SCN2672_CHAR_PER_ROW:	// Char per Row
        	writeCharPerRowScn2672((Byte)value, showTrace);
        break;

        case SCN2672_FIRST_CURSOR_LINE:	// First cursor line
            realInterlace = (((initializationRegister[1] & 0x80) >> 7) + 1) ^ 3;
            if (realInterlace == SCN2672_NOT_INTERLACED && !scn2672Configuration_.videoMode)
                value = value/2;
        	initializationRegister[6] &= 0xf;
            initializationRegister[6] |= ((value&0xf) << 4);
            setVideoConfiguration_ = true;
        break;

        case SCN2672_LAST_CURSOR_LINE:	// Last cursor line
            realInterlace = (((initializationRegister[1] & 0x80) >> 7) + 1) ^ 3;
            if (realInterlace == SCN2672_NOT_INTERLACED && !scn2672Configuration_.videoMode)
                value = (value - 1) / 2;
        	initializationRegister[6] &= 0xf0;
        	initializationRegister[6] |= (value&0xf);
            setVideoConfiguration_ = true;
        break;

        case SCN2672_UNDERLINE_LINE:	// Underline position
            underLineScanLine_ = value & 0xf;
            initializationRegister[7] &= 0xf0;
        	initializationRegister[7] |= (underLineScanLine_);
            reDraw_ = true;
        break;

        case SCN2672_DISPLAY_BUFFER_START:
        	initializationRegister[8] = value & 0xff;
            initializationRegister[9] &= 0xf0;
        	initializationRegister[9] |= ((value & 0xf00) >> 8);
        	displayStart_ = value;
            reDraw_ = true;
        break;
            
        case SCN2672_DISPLAY_BUFFER_END:
            initializationRegister[9] &= 0xf0;
        	initializationRegister[9] |= ((((value + 1) / 1025) - 1) << 4);
        	displayStart_ = value;
            reDraw_ = true;
        break;
        
        case SCN2672_SPLIT_SCREEN_INT_ROW:
        	splitScreenInterruptRow_ = value & 0x7f;
        	initializationRegister[10] &= 0x80;
        	initializationRegister[10] |= splitScreenInterruptRow_;
        break;
    }
    return showTrace;
}

void Scn2672::checkIfInitializationRegistersAreSet(int currentRegister)
{
    if (initializationRegistersLoaded_)
        return;
    
    initializationRegisterLoaded[currentRegister] = true;
    initializationRegistersLoaded_ = true;
    for (int registerNummber=0; registerNummber<11; registerNummber++)
    {
        if (!initializationRegisterLoaded[registerNummber])
            initializationRegistersLoaded_ = false;
    }
}

int Scn2672::writeRegisterCrt8002(Byte registerIndex, Word value, int showTrace)
{
    switch (registerIndex)
    {
        case CRT8002_ATTRIBUTE:
        	writeAttribute(value, showTrace);
        break;
        case CRT8002_ATTRIBUTE_SCREEN1:
        	writeAttributeScreen1(value, showTrace);
        break;
        case CRT8002_REVERSE:
        	crt8002Configuration_.reverse.bitNumber = (value & 0x7);
        break;
        case CRT8002_BLINK:
        	crt8002Configuration_.blink.bitNumber = (value & 0x7);
        break;
        case CRT8002_GRAPHIC_MS0:
        	crt8002Configuration_.graphic_ms0.bitNumber = (value & 0x7);
        break;
        case CRT8002_GRAPHIC_MS1:
        	crt8002Configuration_.graphic_ms1.bitNumber = (value & 0x7);
        break;
        case CRT8002_UNDERLINE:
        	crt8002Configuration_.underline.bitNumber = (value & 0x7);
        break;
        case CRT8002_STRIKE_THRU:
        	crt8002Configuration_.strikeThru.bitNumber = (value & 0x7);
        break;
        case CRT8002_BLANK:
        	crt8002Configuration_.blank.bitNumber = (value & 0x7);
        break;
        case CRT8002_UNDERLINE_LINE1:
        	crt8002Configuration_.underlineLine1 = (value & 0x7);
        break;
        case CRT8002_UNDERLINE_LINE2:
        	crt8002Configuration_.underlineLine2 = (value & 0x7);
        break;
        case CRT8002_STRIKE_THRU_LINE1:
        	crt8002Configuration_.strikeThruLine1 = (value & 0x7);
        break;
        case CRT8002_STRIKE_THRU_LINE2:
        	crt8002Configuration_.strikeThruLine2 = (value & 0x7);
        break;
    }
    reDraw_ = true;
    return showTrace;
}        

void Scn2672::setCharacterAndCursurMode()
{
	scanLinesPerCharacterRow_ = ((initializationRegister[0] & 0x78) >> 3) + 1;
	interlace_ = (((initializationRegister[1] & 0x80) >> 7) + 1) ^ 3;
    cursorStartLine_ = initializationRegister[6] >> 4;
    cursorEndLine_ = initializationRegister[6] & 0xf;

    if (interlaceOR_ == true)
        setForcedInterlace();

    if (interlace_ == SCN2672_INTERLACED)
		scanLinesPerCharacterRow_ = scanLinesPerCharacterRow_*2 + 1;

    p_Main->setScn2672RegisterByte(SCN2672_SCANLINES, scanLinesPerCharacterRow_, DO_NOT_SHOW_ANY_TRACE);
}

void Scn2672::writeScanlinesScn2672(Byte value, int showTrace)
{
    Byte scanLines = value;
    if (interlace_ == SCN2672_INTERLACED)
        scanLines = (scanLines - 1) / 2;
    
    initializationRegister[0] &= 0x87;
    scanLines = (scanLines - 1) & 0xf;
    scanLines = scanLines << 3;
    initializationRegister[0] |= scanLines;

    setVideoConfiguration_ = true;
    p_Main->setScn2672RegisterByte(SCN2672_SCANLINES, value, showTrace);
}

void Scn2672::setVideoHeight()
{
    characterRowsPerScreen_ = (initializationRegister[4] & 0x7f) + 1;
    videoHeight_ = characterRowsPerScreen_*scanLinesPerCharacterRow_*interlace_;
}

void Scn2672::setVideoWidth()
{
    int currentActiveCharactersPerRow = activeCharactersPerRow_;
    activeCharactersPerRow_ = initializationRegister[5] + 1;
    
    if (currentActiveCharactersPerRow <= 40 && activeCharactersPerRow_ >= 70)
    {
         borderX_[videoType_] = borderX_[videoType_] * xZoomFactor_;
         xZoomFactor_ = xZoomFactor_/2;
         borderX_[videoType_] = borderX_[videoType_] / xZoomFactor_;
    }
    if (currentActiveCharactersPerRow >= 70 && activeCharactersPerRow_ <= 40)
    {
         borderX_[videoType_] = borderX_[videoType_] * xZoomFactor_;
         xZoomFactor_ = xZoomFactor_*2;
         borderX_[videoType_] = borderX_[videoType_] / xZoomFactor_;
    }
    videoScreenPointer->setScale(xZoomFactor_);
    calculateHorizontalFrontPorch();

    videoWidth_ = activeCharactersPerRow_*scn2672Configuration_.charSize.x;
}

void Scn2672::setInterlace(bool status)
{
    scanLinesPerCharacterRow_ = ((initializationRegister[0] & 0x78) >> 3) + 1;
    cursorStartLine_ = initializationRegister[6] >> 4;
    cursorEndLine_ = initializationRegister[6] & 0xf;

    if (status)
    {
        scanLinesPerCharacterRow_ = scanLinesPerCharacterRow_*2 + 1;
        setForcedInterlace();
    }
    else
        interlace_ = SCN2672_NOT_INTERLACED;

    videoHeight_ = characterRowsPerScreen_*scanLinesPerCharacterRow_*interlace_;

    reDraw_ = true;
    setCycle();
    setScreenSize();
    resetScreenCopyPointer();

    p_Main->setScn2672RegisterByte(SCN2672_SCANLINES, (Byte)scanLinesPerCharacterRow_, DO_NOT_SHOW_ANY_TRACE);
}

void Scn2672::setForcedInterlace()
{
    interlace_ = SCN2672_INTERLACED;
    int realInterlace = (((initializationRegister[1] & 0x80) >> 7) + 1) ^ 3;

    if (realInterlace == SCN2672_NOT_INTERLACED && !scn2672Configuration_.videoMode)
    {
        cursorStartLine_ = cursorStartLine_*2;
        cursorEndLine_ = cursorEndLine_*2 + 1;
    }
}

void Scn2672::calculateHorizontalFrontPorch()
{
    horizontalFrontPorch_ = ((equalizingConstant_ + 2 * horizontalSyncWidth_) * 2) - activeCharactersPerRow_ - horizontalSyncWidth_ - horizontalBackPorch_;
}

int Scn2672::writeCommandScn2672(Byte value, int showTrace)
{
    if ((value & 0xe0) == 0xa0)
        showTrace = delayedCommand(value, showTrace);
    else
        showTrace = instantaneousCommand(value, showTrace);
    
    statusRegister_[SCN2672_READY] = 1;
    if (interruptReady_)
    {
        interruptRegister_[SCN2672_READY] = 1;
        p_Computer->requestInterrupt(INTERRUPT_TYPE_SCN2672, true, scn2672Configuration_.picInterrupt);
    }

    return p_Main->setScn2672RegisterByte(SCN2672_COMMAND, value, showTrace);
}

int Scn2672::instantaneousCommand(Byte value, int showTrace)
{
    switch (value & 0xf0)
    {
        case 0:
            interruptReady_ = false;
            interruptSplitScreen_ = false;
            interruptLineZero_ = false;
            interruptVerticalBlank_ = false;
            
            statusRegister_[SCN2672_RDFLG] = 1;
            registerIndex_ = 0;
            cursorOn_ = false;
            displayOn_ = false;
        return showTrace;
            
        case 0x10:
            registerIndex_ = value & 0xf;
        return showTrace;

        case 0x20:
            if ((value & 0x8) == 0x8)
                return showTrace;
            else
                displayOn_ = ((value & 0x1) == 0x1);
        return showTrace;

        case 0x30:
            cursorOn_ = ((value & 0x1) == 0x1);
            cursorBlinkOn_ = true;
        return showTrace;
    }
    switch (value & 0xe0)
    {
        case 0x40: // reset interrupt/status
            statusRegister_[SCN2672_LIGHT_PEN] = 0;
            statusRegister_[SCN2672_READY] = 0;
            statusRegister_[SCN2672_SPLIT_SCREEN] = 0;
            statusRegister_[SCN2672_LINE_ZERO] = 0;
            statusRegister_[SCN2672_VBLANK] = 0;
            interruptRegister_ = 0;
        break;
            
        case 0x80: // disable interrupt
            if ((value & 0x2) == 0x2)
                interruptReady_ = false;
            if ((value & 0x4) == 0x4)
                interruptSplitScreen_ = false;
            if ((value & 0x8) == 0x8)
                interruptLineZero_ = false;
            if ((value & 0x10) == 0x10)
                interruptVerticalBlank_ = false;

        break;

        case 0x60: // enable interrupt
            if ((value & 0x2) == 0x2)
                interruptReady_ = true;
            if ((value & 0x4) == 0x4)
                interruptSplitScreen_ = true;
            if ((value & 0x8) == 0x8)
                interruptLineZero_ = true;
            if ((value & 0x10) == 0x10)
                interruptVerticalBlank_ = true;

        break;
    }
    return showTrace;
}

int Scn2672::delayedCommand(Byte value, int showTrace)
{
    switch (value)
    {
        case 0xA4: // Read at pointer address
            displayBuffer_ = scn2672ram_[pointerAddress_ & 0x7fff];
        break;

        case 0xA2: // write at pointer address
            scn2672ram_[pointerAddress_ & 0x7FFF] = displayBuffer_;
        break;

        case 0xA9: // increment cursor address
            cursorAddress_++;
            cursorAddress_ &= 0x7fff;
        break;

        case 0xAC: // read at cursor address
            displayBuffer_ = scn2672ram_[cursorAddress_ & 0x7fff];
        break;

        case 0xAA: // write at cursor address
            scn2672ram_[cursorAddress_ & 0x7fff] = displayBuffer_;
        break;

        case 0xAD: // read at cursor address++
            displayBuffer_ = scn2672ram_[cursorAddress_ & 0x7fff];
            cursorAddress_++;
            cursorAddress_ &= 0x7fff;
        break;

        case 0xAB: // write at cursor address++
            scn2672ram_[cursorAddress_ & 0x7fff] = displayBuffer_;
            cursorAddress_++;
            cursorAddress_ &= 0x7fff;
        break;

        case 0xBB: // write from cursor address to pointer address
            for (Word address = cursorAddress_; address <= pointerAddress_; address++)
                scn2672ram_[address & 0x7fff] = displayBuffer_;
            cursorAddress_ = pointerAddress_;
            reDraw_ = true;
        break;
            
        default:
            reDraw_ = true;
        break;
    }
    return showTrace;
}

int Scn2672::writeScreenStartScn2672(Word value, int showTrace)
{
    screenStart_ = value;
    curentRamAddress_ = screenStart_;
    reDraw_ = true;
    return p_Main->setScn2672RegisterWord(SCN2672_SCREEN_START, screenStart_, showTrace);
}

int Scn2672::writeScreenStartHighScn2672(Byte value, int showTrace)
{
    screenStart_ = (screenStart_ & 0xff) | (value << 8);
    curentRamAddress_ = screenStart_;
    return p_Main->setScn2672RegisterWord(SCN2672_SCREEN_START, screenStart_, showTrace);
}

int Scn2672::writeScreenStartLowScn2672(Byte value, int showTrace)
{
    screenStart_ = (screenStart_ & 0xff00) | value;
    curentRamAddress_ = screenStart_;
    return p_Main->setScn2672RegisterWord(SCN2672_SCREEN_START, screenStart_, showTrace);
}

Byte Scn2672::readScreenStartHighScn2672(int showTrace)
{
    p_Main->readScn2672Register(SCN2672_SCREEN_START, screenStart_ >> 8, showTrace);
    return screenStart_ >> 8;
}

Byte Scn2672::readScreenStartLowScn2672(int showTrace)
{
    p_Main->readScn2672Register(SCN2672_SCREEN_START, screenStart_ & 0xff, showTrace);
    return screenStart_ & 0xff;
}

int Scn2672::writeCursorScn2672(Word value, int showTrace)
{
    cursorAddress_ = value;
    return p_Main->setScn2672RegisterWord(SCN2672_CURSOR, cursorAddress_, showTrace);
}

int Scn2672::writeCursorHighScn2672(Byte value, int showTrace)
{
    cursorAddress_ = (cursorAddress_ & 0xff) | (value << 8);
    return p_Main->setScn2672RegisterWord(SCN2672_CURSOR, cursorAddress_, showTrace);
}

int Scn2672::writeCursorLowScn2672(Byte value, int showTrace)
{
    cursorAddress_ = (cursorAddress_ & 0xff00) | value;
    return p_Main->setScn2672RegisterWord(SCN2672_CURSOR, cursorAddress_, showTrace);
}

Byte Scn2672::readCursorHighScn2672(int showTrace)
{
    p_Main->readScn2672Register(SCN2672_CURSOR, cursorAddress_ >> 8, showTrace);
    return cursorAddress_ >> 8;
}

Byte Scn2672::readCursorLowScn2672(int showTrace)
{
    p_Main->readScn2672Register(SCN2672_CURSOR, cursorAddress_ & 0xff, showTrace);
    return cursorAddress_ & 0xff;
}

int Scn2672::writePointerScn2672(Word value, int showTrace)
{
    pointerAddress_ = value;
    return p_Main->setScn2672RegisterWord(SCN2672_POINTER, pointerAddress_, showTrace);
}

int Scn2672::writePointerHighScn2672(Byte value, int showTrace)
{
    pointerAddress_ = (pointerAddress_ & 0xff) | (value << 8);
    return p_Main->setScn2672RegisterWord(SCN2672_POINTER, pointerAddress_, showTrace);
}

int Scn2672::writePointerLowScn2672(Byte value, int showTrace)
{
    pointerAddress_ = (pointerAddress_ & 0xff00) | value;
    return p_Main->setScn2672RegisterWord(SCN2672_POINTER, pointerAddress_, showTrace);
}

Byte Scn2672::readDataScn2672(int showTrace)
{
    p_Main->readScn2672RegisterSetData(SCN2672_DATA, displayBuffer_, showTrace);
    return displayBuffer_;
}

void Scn2672::writeDataScn2672(Byte value, int showTrace)
{
    displayBuffer_ = value;
    p_Main->setScn2672RegisterByte(SCN2672_DATA, displayBuffer_, showTrace);
}

void Scn2672::writeCharWidthScn2672(Byte value, int showTrace)
{
    scn2672Configuration_.charSize.x = value;
    setVideoConfiguration_ = true;
    p_Main->setScn2672RegisterByte(SCN2672_CHAR_WIDTH, value, showTrace);
}

void Scn2672::writeScreenRowsScn2672(Byte value, int showTrace)
{
	initializationRegister[4] &= 0x80;
	initializationRegister[4] |= (value - 1);
    setVideoConfiguration_ = true;
    p_Main->setScn2672RegisterByte(SCN2672_SCREEN_ROWS, value, showTrace);
}

void Scn2672::writeCharPerRowScn2672(Byte value, int showTrace)
{
	initializationRegister[5] = (value - 1);
    setVideoConfiguration_ = true;
    p_Main->setScn2672RegisterByte(SCN2672_CHAR_PER_ROW, value, showTrace);
}

Byte Scn2672::readAttribute(int showTrace)
{
    p_Main->readCrt8002Register(CRT8002_ATTRIBUTE, displayBuffer_, showTrace);
    return attributeType_.to_ulong();
}

Byte Scn2672::readAttributeScreen1(int showTrace)
{
    p_Main->readCrt8002Register(CRT8002_ATTRIBUTE_SCREEN1, displayBuffer_, showTrace);
    return attributeTypeScreen1_;
}

void Scn2672::writeAttribute(Byte value, int showTrace)
{
    attributeType_ = value;
    showTrace = p_Main->setCrt8002Register(CRT8002_ATTRIBUTE, (Byte)attributeType_.to_ulong(), showTrace);
    
    if (attributeType_ == 0 || lastAttributeType_ == value)
        return;

    lastAttributeType_ = value;
}

void Scn2672::writeAttributeScreen1(Byte value, int showTrace)
{
    attributeTypeScreen1_ = value;
    showTrace = p_Main->setCrt8002Register(CRT8002_ATTRIBUTE_SCREEN1, (Byte)attributeTypeScreen1_, showTrace);
}

Byte Scn2672::readStatusScn2672(int showTrace)
{
     p_Main->readScn2672RegisterSetData(SCN2672_STATUS, statusRegister_.to_ulong(), showTrace);
     return statusRegister_.to_ulong();
}

Byte Scn2672::readInterruptScn2672(int showTrace)
{
     p_Main->readScn2672RegisterSetData(SCN2672_INTERRUPT, interruptRegister_.to_ulong(), showTrace);
     return interruptRegister_.to_ulong();
}

Byte Scn2672::readInterruptStatusRegister()
{
    if (interruptRegister_ != 0)
        return 0x10;
    else
        return 0;
}

void Scn2672::cycleScn2672()
{
    if (!initializationRegistersLoaded_)
        return;
    
    currentLine_ = (int)currentY_/ (scanLinesPerCharacterRow_ * interlace_);
    
    if (lineCycleValueScn2672_ > 0)
    {
        lineCycleValueScn2672_--;
        if (lineCycleValueScn2672_ == 0)
        {
            statusRegister_[SCN2672_LINE_ZERO] = 0;
            interruptRegister_[SCN2672_LINE_ZERO] = 0;
        }
    }
    
    if (characterLineCycleValueScn2672_ > 0)
    {
        characterLineCycleValueScn2672_--;
        if (characterLineCycleValueScn2672_ == 0)
        {
            if (currentLine_ == splitScreenInterruptRow_)
            {
                statusRegister_[SCN2672_SPLIT_SCREEN] = 1;
                attributeTypeScreen1_ = attributeType_.to_ulong();
                p_Main->setCrt8002Register(CRT8002_ATTRIBUTE_SCREEN1, attributeTypeScreen1_, DO_NOT_SHOW_ANY_TRACE);
                if (interruptSplitScreen_)
                {
                    interruptRegister_[SCN2672_SPLIT_SCREEN] = 1;
                    p_Computer->requestInterrupt(INTERRUPT_TYPE_SCN2672, true, scn2672Configuration_.picInterrupt);
                }
            }

            lineCycleValueScn2672_ = lineCycleSizeScn2672_;
            statusRegister_[SCN2672_LINE_ZERO] = 1;
            if (interruptLineZero_)
            {
                interruptRegister_[SCN2672_LINE_ZERO] = 1;
                p_Computer->requestInterrupt(INTERRUPT_TYPE_SCN2672, true, scn2672Configuration_.picInterrupt);
            }
            drawNextLine();
            if (currentLine_ < (characterRowsPerScreen_ - 1))
                characterLineCycleValueScn2672_ = characterLineCycleSizeScn2672_;
            else
            {
                verticalRetraceCycleValueScn2672_ = verticalRetraceCycleSizeScn2672_;
                
                statusRegister_[SCN2672_VBLANK] = 1;
                if (interruptVerticalBlank_)
                {
                    interruptRegister_[SCN2672_VBLANK] = 1;
                    p_Computer->requestInterrupt(INTERRUPT_TYPE_SCN2672, true, scn2672Configuration_.picInterrupt);
                }
            }
        }
    }
    if (verticalRetraceCycleValueScn2672_ > 0)
    {
        verticalRetraceCycleValueScn2672_--;
        if (verticalRetraceCycleValueScn2672_ == 0)
        {
            characterLineCycleValueScn2672_ = characterLineCycleSizeScn2672_;
            statusRegister_[SCN2672_VBLANK] = 0;
            interruptRegister_[SCN2672_VBLANK] = 0;
        }
    }
}

void Scn2672::drawNextLine()
{
    Word address = curentRamAddress_;
    drawLine();
    
    if (isEndScreen())
        setStartScreen();
    else
    {
        if (doubleHeightCharacter_)
        {
            curentRamAddress_ = address;
            drawLine();
            if (isEndScreen())
                setStartScreen();
        }
    }
}

void Scn2672::drawLine()
{
    int row;
    bool blink = false;
    
    if (crt8002Configuration_.blink.bitNumber != -1)
    {
        blink = attributeType_[crt8002Configuration_.blink.bitNumber] ^ crt8002Configuration_.blink.reversePolarity;
    }
    for (int x=0; x<(activeCharactersPerRow_*scn2672Configuration_.charSize.x); x+=scn2672Configuration_.charSize.x)
    {
        row = (int)x/scn2672Configuration_.charSize.x;

        if (scn2672display_[row&0xff][currentLine_&0x7f] != scn2672ram_[curentRamAddress_ & 0x7fff] || reDrawOnNextCycle_ ||
            ( ((scn2672ram_[curentRamAddress_ & 0x7fff] & 0x80) == 0x80) && blink) ||
            scn2672displayAttribute_[row&0xff][currentLine_&0x7f] != attributeType_.to_ulong())
            drawCharacterScn2672(x, currentY_, scn2672ram_[curentRamAddress_ & 0x7fff]);
        
        if (curentRamAddress_== cursorAddress_)
        {
            drawCursor(x);
            scn2672display_[row&0xff][currentLine_&0x7f] = -1;
        }
        else
        {
            scn2672display_[row&0xff][currentLine_&0x7f] = scn2672ram_[curentRamAddress_ & 0x7fff];
            scn2672displayAttribute_[row&0xff][currentLine_&0x7f] = attributeType_.to_ulong();
        }
        
        curentRamAddress_++;
    }
    if (curentRamAddress_ > displayEnd_)
        curentRamAddress_ = displayStart_;
    currentY_ += scanLinesPerCharacterRow_ * interlace_;
}

bool Scn2672::isEndScreen()
{
    return (currentY_ >= (characterRowsPerScreen_ * scanLinesPerCharacterRow_ * interlace_));
}

void Scn2672::setStartScreen()
{
    reDrawOnNextCycle_ = false;
    currentY_ = 0;
    copyScreen();
    if (reDraw_)
        reDrawOnNextCycle_ = true;
    reDraw_ = false;

    if (setVideoConfiguration_)
    {
        setCharacterAndCursurMode();
        setVideoWidth();
        setVideoHeight();
        setVideoConfiguration_ = false;
        
        setScreenSize();
        setCycle();
        resetScreenCopyPointer();
        reDrawOnNextCycle_ = true;
        reDraw_ = true;
    }
}

void Scn2672::blinkScn2672()
{
    cursorBlinkValueScn2672_--;
    if (cursorBlinkValueScn2672_ <= 0)
    {
        cursorBlinkValueScn2672_ = cursorBlinkSizeScn2672_;
        videoSyncCount_++;
        if (changeScreenSize_)
            changeScreenSize();
        copyScreen();
        cursorBlinkTimeValue_--;
        if (cursorBlinkTimeValue_ <= 0)
        {
            cursorBlinkTimeValue_ = cursorBlinkTimeSize_;
            if (cursorOn_ && cursorBlink_)
                cursorBlinkOn_ = !cursorBlinkOn_;
        }
    }
    characterBlinkValueScn2672_--;
    if (characterBlinkValueScn2672_ <= 0)
    {
        characterBlinkValueScn2672_ = characterBlinkSizeScn2672_;
        videoSyncCount_++;
        if (changeScreenSize_)
            changeScreenSize();
        copyScreen();
        characterBlinkTimeValue_--;
        if (characterBlinkTimeValue_ <= 0)
        {
            characterBlinkTimeValue_ = characterBlinkTimeSize_;
            characterBlinkOn_ = !characterBlinkOn_;
        }
    }
}

void Scn2672::setClock(double clock)
{
    clock_ = clock;
    setCycle();
}

void Scn2672::setCycle()
{   
    float clockPeriod = (float)(1/scn2672Configuration_.videoClock); 
    float scanLinePeriod = (float)(activeCharactersPerRow_ + horizontalFrontPorch_ + horizontalSyncWidth_ + horizontalBackPorch_) * clockPeriod;
    int fieldScanLines = (characterRowsPerScreen_ * scanLinesPerCharacterRow_) + verticalFrontPorch_ + verticalBackPorch_ + SCN2672_VSYNC;
    float fieldTime = (float)(fieldScanLines * scanLinePeriod);

    lineCycleSizeScn2672_ = (int)(scanLinePeriod / ((1/clock_) * 8));
    characterLineCycleSizeScn2672_ = lineCycleSizeScn2672_ * scanLinesPerCharacterRow_;
    nonDisplayScn2672_ = lineCycleSizeScn2672_ - (int)((activeCharactersPerRow_ * clockPeriod) / ((1/clock_) * 8));
    cursorBlinkSizeScn2672_ = (int)(fieldTime / ((1/clock_) * 8));
    characterBlinkSizeScn2672_ = cursorBlinkSizeScn2672_;

    lineCycleValueScn2672_ = 0;
    characterLineCycleValueScn2672_ = characterLineCycleSizeScn2672_;
    
    verticalRetraceCycleSizeScn2672_ = characterLineCycleSizeScn2672_ * (verticalBackPorch_ + verticalFrontPorch_ + SCN2672_VSYNC);
    verticalRetraceCycleValueScn2672_ = 0;
    cursorBlinkValueScn2672_ = cursorBlinkSizeScn2672_;
    characterBlinkValueScn2672_ = characterBlinkSizeScn2672_;
}

Byte Scn2672::readScn2672Ram(Word addr)
{
    return scn2672ram_[addr&0x7FFF];
}

void Scn2672::writeScn2672Ram(Word addr, Byte value)
{
    scn2672ram_[addr&0x7FFF] = value;
    p_Main->assLog(value);
}

Byte Scn2672::readScn2672CharRom(Word addr)
{
     return scn2672CharRom_[addr&SCN267_ROM_MASK];
}

void Scn2672::writeScn2672CharRom(Word addr, Byte value)
{
    scn2672CharRom_[addr&SCN267_ROM_MASK] = value;
    reDraw_ = true;
}

void Scn2672::copyScreen()
{
    if (p_Main->isZoomEventOngoing())
        return;

    updateReColour();

    if (reDraw_)
        drawOffsetBackground();
    
    // The software framebuffer is flushed into dcMemory identically on every
    // platform; only how dcMemory reaches the window differs. macOS posts an
    // async refresh (onPaint -> reBlit(dc), which also paints the extra
    // background); Windows/Linux draw the extra background and blit the client
    // DC directly from the emulation thread here.
#if defined(__WXMAC__)
    if (reBlit_ || reDraw_)
    {
        flushFramebufferMac();
        p_Main->eventRefreshVideo(false, videoNumber_);
        reBlit_ = false;
    }
#else
    if (extraBackGround_ && newBackGround_)
        drawExtraBackground(colour_[colourIndex_+backGround_]);

    if (reBlit_ || reDraw_)
    {
        flushFramebufferMac();
        videoScreenPointer->blit(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_, &dcMemory, 0, 0);
        reBlit_ = false;
    }
#endif
}

void Scn2672::drawOffsetBackground()
{
    setColour(colourIndex_+backGround_);
    drawRectangle(0, 0, videoWidth_ + 2*offsetX_, offsetY_);
    drawRectangle(0, videoHeight_ + offsetY_, videoWidth_ + 2*offsetX_, videoHeight_ + 2*offsetY_);
    drawRectangle(0, offsetY_, offsetX_, videoHeight_ + offsetY_);
    drawRectangle(videoWidth_ + offsetX_, offsetY_, videoWidth_ + 2*offsetX_, videoHeight_ + offsetY_);
}

void Scn2672::drawCursor(wxCoord x)
{
    int clr;

    if (cursorOn_)
    {
        if (cursorBlinkOn_)
            clr = colourIndex_+FOREGROUND;
        else
            clr = colourIndex_+backGround_;
        
        setColour(clr);
        int lineFactor, lineCounter;
        if (scn2672Configuration_.videoMode)
        {
            lineFactor = 1;
            lineCounter = 1;
        }
        else
        {
            lineFactor = 2;
            lineCounter = interlace_;
        }
        for (wxCoord y=(currentY_+cursorStartLine_*interlace_+offsetY_); y<=(currentY_+cursorEndLine_*interlace_+offsetY_); y+=interlace_)
            drawRectangle(x+offsetX_, y, scn2672Configuration_.charSize.x, 1);
    }
}

void Scn2672::drawCharacterScn2672(wxCoord x, wxCoord y, Byte character)
{
    int line_byte, line, characterLine, backGround, foreGround;
    bool attributeOn = ((character & 0x80) == 0x80);

    character &= 0x7f;
    
    foreGround = colourIndex_+FOREGROUND;
    backGround = colourIndex_+backGround_;

    if (crt8002Configuration_.reverse.bitNumber != -1)
    {
        if (attributeOn && (attributeType_[crt8002Configuration_.reverse.bitNumber] ^ crt8002Configuration_.reverse.reversePolarity))
        {
            backGround = colourIndex_+FOREGROUND;
            foreGround = colourIndex_+backGround_;
        }
    }
        
    setColour(backGround);
    drawRectangle(x+offsetX_, y+offsetY_, scn2672Configuration_.charSize.x, scanLinesPerCharacterRow_*interlace_);

    if (crt8002Configuration_.blink.bitNumber != -1)
        if (attributeOn && (attributeType_[crt8002Configuration_.blink.bitNumber] ^ crt8002Configuration_.blink.reversePolarity) && !characterBlinkOn_)
            return;
    
    setColour(foreGround);

    // Full-frame refresh on all platforms: the software framebuffer flushes
    // the whole plane each frame.
    reBlit_ = true;

    Byte graphic[16];
    Byte graphicCode = character;
    int blockHeight = 3;
    line = 0;
    
    Byte graphicsAttribute = 3;
    if (crt8002Configuration_.graphic_ms0.bitNumber != -1 && crt8002Configuration_.graphic_ms1.bitNumber != -1)
        graphicsAttribute = (attributeType_[crt8002Configuration_.graphic_ms0.bitNumber] ^ crt8002Configuration_.graphic_ms0.reversePolarity) + ((attributeType_[crt8002Configuration_.graphic_ms1.bitNumber]  ^ crt8002Configuration_.graphic_ms1.reversePolarity) << 1);
    if ((attributeOn && graphicsAttribute == 0))
    {
        for (int block = 0; block <3; block++)
        {
            line_byte = 0;
            if ((graphicCode & 1) == 1)
                line_byte = 0x7;
            if ((graphicCode & 0x10) == 0x10)
                line_byte |= 0x78;
            for (int lineNumber = 0; lineNumber <blockHeight; lineNumber++)
                graphic[block*3 + lineNumber] = line_byte;
            graphicCode = graphicCode >> 1;
            if (block == 2)
                blockHeight = 5;
        }
    }
    
    line = 0;
    characterLine = 0;
    for (wxCoord j=y; j<y+scanLinesPerCharacterRow_*interlace_; j+=interlace_)
    {
        if ((attributeOn && graphicsAttribute == 0))
            line_byte = graphic[characterLine];
        else
        {
            line_byte = scn2672CharRom_[character*SCN2672_ROM_LINES+characterLine+SCN2672_FIRST_ROM_LINE];
            if (attributeOn && (characterLine == crt8002Configuration_.underlineLine1 || characterLine == crt8002Configuration_.underlineLine2) && (attributeType_[crt8002Configuration_.underline.bitNumber] ^ crt8002Configuration_.underline.reversePolarity))
                line_byte = 0xff;
            if (attributeOn && (characterLine == crt8002Configuration_.strikeThruLine1 || characterLine == crt8002Configuration_.strikeThruLine2) && (attributeType_[crt8002Configuration_.strikeThru.bitNumber] ^ crt8002Configuration_.strikeThru.reversePolarity))
                line_byte = 0xff;
        }
        if (attributeOn && (attributeType_[crt8002Configuration_.blank.bitNumber] ^ crt8002Configuration_.blank.reversePolarity))
            line_byte = 0;
        if (scn2672Configuration_.charSize.x == 7)
            line_byte <<= 1;
        for (wxCoord i=x; i<x+scn2672Configuration_.charSize.x; i++)
        {
            if (line_byte & 128)
            	drawPoint(i+offsetX_, j+offsetY_);

            line_byte <<= 1;
        }
        if (interlace_ == SCN2672_NOT_INTERLACED || (line&1) == 1 || scn2672Configuration_.videoMode)
        	characterLine++;
        line++;
    }
}

bool Scn2672::readCharRomFile(wxString romDir, wxString romFile)
{
    wxFFile inFile;
    size_t length, number;
    char buffer[SCN267_ROM_MASK+1];

    if (romFile == "")
    {
        p_Main->errorMessage("No font filename specified");
        return false;
    }

    wxString fileName = romDir + romFile;

    if (inFile.Open(fileName, "rb"))
    {
        length = inFile.Read(buffer, SCN267_ROM_MASK+1);
        number = 0;
        for (size_t i=0; i<length; i++)
        {
            scn2672CharRom_[i&SCN267_ROM_MASK] = (Byte)buffer[i];
            number++;
        }
        inFile.Close();
        return true;
    }
    else
    {
        p_Main->errorMessage("Error reading " + fileName);
        return false;
    }
}

void Scn2672::setFullScreen(bool fullScreenSet)
{
    fullScreenSet_ = fullScreenSet;
    ShowFullScreen(fullScreenSet);
}

void Scn2672::onF3()
{
    fullScreenSet_ = !fullScreenSet_;
    p_Main->eventVideoSetFullScreen(fullScreenSet_, videoNumber_);
}

void Scn2672::reBlit(wxDC &dc)
{
    if (!memoryDCvalid_)
        return;
    
    memoryDCBeingUsed_ = true;
    
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
    memoryDCBeingUsed_ = false;
}

void Scn2672::resetScreenCopyPointer()
{
    if (videoHeight_ == 0 || videoWidth_ == 0)
        return;

    while (memoryDCBeingUsed_)
        wxThread::This()->Sleep(1);

    memoryDCvalid_ = false;
    dcMemory.SelectObject(wxNullBitmap);
    delete screenCopyPointer;
    
    screenCopyPointer = new wxBitmap(2*offsetX_+videoWidth_, 2*offsetY_+videoHeight_);
    dcMemory.SelectObject(*screenCopyPointer);

#ifdef __WXMAC__
    delete gc;
    gc = wxGraphicsContext::Create(dcMemory);
    gc->SetAntialiasMode(wxANTIALIAS_NONE);
#endif
    memoryDCvalid_ = true;
}

