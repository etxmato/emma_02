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

#include "main.h"
#include "fred.h"

#define CHIP8_PC 5
#define TAPE_MODE_PROGRAM 0x10
#define TAPE_MODE_DIRECT 0x20
#define TAPE_MODE_WRITE 0x40

#define INP_MODE_NONE 0
#define INP_MODE_KEYPAD 1
#define INP_MODE_TAPE_PROGRAM 2
#define INP_MODE_TAPE_DIRECT 3
#define INP_MODE_TAPE_RUN 4

FredScreen::FredScreen(wxWindow *parent, const wxSize& size)
: Panel(parent, size)
{
}

FredScreen::~FredScreen()
{
#if defined (__WXMAC__)
    delete osx_text_runButtonPointer;
    delete osx_text_resetButtonPointer;
#else
    delete text_runButtonPointer;
    delete text_resetButtonPointer;
#endif

    delete readSwitchButton;
    delete cardSwitchButton;
    delete powerSwitchButton;
    
    for (int i=0; i<8; i++)
    {
        delete ledPointer[i];
    }
}

void FredScreen::init()
{
    keyStart_ = 0;
    keyEnd_ = 0;
    lastKey_ = 0;
    forceUpperCase_ = p_Main->getUpperCase(ELF);
    
    wxClientDC dc(this);
    
#if defined(__linux__)
    wxFont defaultFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    SetFont(defaultFont);
#endif
#if defined(__WXMAC__)
    wxFont defaultFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    SetFont(defaultFont);
#endif

#if defined (__WXMAC__)
    osx_text_resetButtonPointer = new HexButton(dc, COSMICOS_HEX_BUTTON, 35, 60, "");
    osx_text_runButtonPointer = new HexButton(dc, COSMICOS_HEX_BUTTON, 85, 60, "");
#else
    text_resetButtonPointer = new wxButton(this, 5, "", wxPoint(20, 60), wxSize(25, 25), 0, wxDefaultValidator, "ResetButton");
    text_resetButtonPointer->SetToolTip("Reset");
    text_runButtonPointer = new wxButton(this, 1, "", wxPoint(80, 60, wxSize(25, 25), 0, wxDefaultValidator, "RunButton");
    text_runButtonPointer->SetToolTip("Go - RUN");
#endif

    for (int i=0; i<8; i++)
    {
        ledPointer[i] = new Led(dc, 24+34*(7-i), 15, ELFLED);
    }
    
    readSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(255, 255, 255), BUTTON_UP, 135, 60, "");
    cardSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(255, 255, 255), BUTTON_UP, 185, 60, "");
    powerSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(255, 255, 255), BUTTON_UP, 235, 60, "");

    stopLedPointer = new Led(dc, 55, 160, ELFLED);
    readyLedPointer = new Led(dc, 140, 160, ELFLED);
    errorLedPointer = new Led(dc, 225, 160, ELFLED);

    this->connectKeyEvent(this);
}

void FredScreen::onPaint(wxPaintEvent&WXUNUSED(event))
{
    wxPaintDC dc(this);
    
    dc.SetPen(*wxWHITE_PEN);
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.DrawRectangle(0, 0, 310, 180);
#if defined(__WXMAC__)
    wxFont defaultFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
#else
    wxFont defaultFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
#endif
    dc.SetFont(defaultFont);
    
    wxString number;
    dc.SetTextForeground(*wxBLACK);
    for (int i=0; i<8; i++)
    {
        number.Printf("%d", i);
//        dc.DrawText(number, 24+34*(7-i), 35);
    }
    
    dc.DrawText("RESET", 20, 88);
    dc.DrawText("RUN", 80, 88);
    dc.DrawText("READ", 125, 88);
    dc.DrawText("CARD", 175, 88);
    dc.DrawText("OFF", 232, 88);

    dc.DrawText("STOP", 42, 138);
    dc.DrawText("READY", 120, 138);
    dc.DrawText("ERROR", 205, 138);

    readSwitchButton->onPaint(dc);
    cardSwitchButton->onPaint(dc);
    powerSwitchButton->onPaint(dc);
    for (int i=0; i<8; i++)
    {
        ledPointer[i]->onPaint(dc);
    }
    stopLedPointer->onPaint(dc);
    readyLedPointer->onPaint(dc);
    errorLedPointer->onPaint(dc);
    
#if defined (__WXMAC__)
    osx_text_resetButtonPointer->onPaint(dc);
    osx_text_runButtonPointer->onPaint(dc);
#endif
}

void FredScreen::onMousePress(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);
    
    wxClientDC dc(this);
    
#if defined (__WXMAC__)
    if (osx_text_resetButtonPointer->onMousePress(dc, x, y))
        p_Computer->onReset();
    
    if (osx_text_runButtonPointer->onMousePress(dc, x, y))
        p_Computer->onRunButton();
#endif
}

void FredScreen::onMouseRelease(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);
    
    wxClientDC dc(this);
    
    if (readSwitchButton->onMouseRelease(dc, x, y))
        p_Main->stopComputer();
    if (cardSwitchButton->onMouseRelease(dc, x, y))
        p_Main->stopComputer();
    if (powerSwitchButton->onMouseRelease(dc, x, y))
        p_Main->stopComputer();
    
#if defined (__WXMAC__)
    osx_text_resetButtonPointer->onMouseRelease(dc, x, y);
    osx_text_runButtonPointer->onMouseRelease(dc, x, y);
#endif
}

BEGIN_EVENT_TABLE(Fred, wxFrame)
EVT_CLOSE (Fred::onClose)
END_EVENT_TABLE()

Fred::Fred(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    ef1State_ = 1;
    ef4State_ = 1;
    ef1StateTape_ = 1;
    
    displayType_ = 0;
	tapeRunSwitch_ = 0;
    tapeActivated_ = false;
	inpMode_ = INP_MODE_NONE;
    
    pulseCount_ = 0;
    totalLength_ = 0;
    pulseLength_ = 0;

    this->SetClientSize(size);
    
    fredScreenPointer = new FredScreen(this, size);
    fredScreenPointer->init();
}

Fred::~Fred()
{
    p_Main->setPixiePos(FRED, pixiePointer->GetPosition());
    pixiePointer->Destroy();
    
	p_Main->setMainPos(FRED, GetPosition());
    delete fredScreenPointer;
}

void Fred::onClose(wxCloseEvent&WXUNUSED(event) )
{
    p_Main->stopComputer();
}

void Fred::configureComputer()
{
    inType_[0] = FREDINP0;
    outType_[1] = FREDIOGROUP;
    outType_[2] = FREDIO2;
    outType_[3] = FREDIO3;
    outType_[4] = FREDIO4;
	efType_[1] = FREDEF1;
    efType_[2] = FREDEF2;
    efType_[4] = FREDEF4;
    
	p_Main->message("Configuring FRED");
    p_Main->message("	Output 1: set I/O group\n");
    
    p_Main->message("	I/O group 1: hex keypad");
    p_Main->message("	I/O group 2: TV");
    p_Main->message("	I/O group 3: tape");
    
    p_Main->message("");
    
    p_Main->message("Configuring hex keypad support");
    p_Main->message("	Output 2: 0 = keypad disabled, 1 = keypad enabled");
    p_Main->message("	Input 0: read byte");
    p_Main->message("	EF 1: data ready\n");
    
    p_Main->message("Configuring TV support");
    p_Main->message("	Output 2: display type 0 = TV off, 1 = 32x32, 2 = 64x16, 3 = 64x32\n");
    
    p_Main->message("Configuring tape support");
    p_Main->message("	Output 2: bit 4 = program mode, bit 5 = direct mode, bit 6 = write mode");
    p_Main->message("	Output 3: bit 0 = 1 - run tape, bit 1 = 1 - sound on, bit 2 = sound");
    p_Main->message("	Input 0: read byte");
    p_Main->message("	EF 1: data ready");
    p_Main->message("	EF 2: tape run/stop");
    p_Main->message("	EF 4: tape error\n");
    
    p_Main->getDefaultHexKeys(FRED, "FRED", "A", keyDefA1_, keyDefA2_, keyDefGameHexA_);

    if (p_Main->getConfigBool("/FRED/GameAuto", true))
        p_Main->loadKeyDefinition(p_Main->getRamFile(FRED), p_Main->getRamFile(FRED), keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");

    resetCpu();
}

void Fred::reDefineKeys(int hexKeyDefA1[], int hexKeyDefA2[])
{
    for (int i=0; i<16; i++)
    {
        keyDefA1_[i] = hexKeyDefA1[i];
        keyDefA2_[i] = hexKeyDefA2[i];
    }
}

void Fred::initComputer()
{
    Show(p_Main->getUseVelfControlWindows());
    setClear(1);
    setWait(1);
    
    for (int i=0; i<16; i++)
    {
        keyState_[i] = 0;
    }
    ioGroup_ = 0;
}

void Fred::keyDown(int keycode)
{
#if defined (__WXMAC__)
//    if (ef1State_ == 0) // This is to avoid multiple key presses on OSX
//        return;
#endif

    for (int i=0; i<16; i++)
    {
        if (keycode == keyDefA1_[i])
        {
            ef1State_ = 0;
            keyValue_ = i;
//            switches_ = ((switches_ << 4) & 0xf0) | i;
        }
        if (keycode == keyDefA2_[i])
        {
            ef1State_ = 0;
            keyValue_ = i;
//            switches_ = ((switches_ << 4) & 0xf0) | i;
        }
    }
}

void Fred::keyUp(int WXUNUSED(keycode))
{
    ef1State_ = 1;
}

Byte Fred::ef(int flag)
{
	switch(efType_[flag])
	{
		case 0:
			return 1;
		break;

        case FREDEF1:
            return ef1();
        break;
            
		case FREDEF2:
			return ef2();
		break;

		case FREDEF4:
			return ef4();
		break;

		default:
			return 1;
	}
}

Byte Fred::ef1()
{
	switch (inpMode_)
	{
		case INP_MODE_KEYPAD:
			return ef1State_;
		break;

		case INP_MODE_TAPE_PROGRAM:
			return ef1StateTape_;
		break;

        default:
            return 1;
    }
}

Byte Fred::ef2()
{
    if (tapeRunSwitch_&1)
        return 0;
    else
        return 1;
}

Byte Fred::ef4()
{
    return ef4State_;
}

Byte Fred::in(Byte port, Word WXUNUSED(address))
{
	Byte ret = 255;

	switch(inType_[port])
	{
        case FREDINP0:
            ret = 255;
			switch (inpMode_)
			{
				case INP_MODE_NONE:
					ret = 255;
				break;

				case INP_MODE_KEYPAD:
					ret = keyValue_;
                    ef1State_ = 1;
				break;

				case INP_MODE_TAPE_PROGRAM:
					ret = tapeInput_ & 0xff;
					ef1StateTape_ = 1;
				break;
			}
        break;
    }
	inValues_[port] = ret;
	return ret;
}

void Fred::out(Byte port, Word WXUNUSED(address), Byte value)
{
	outValues_[port] = value;

	switch(outType_[port])
	{
		case 0:
			return;
		break;

        case FREDIOGROUP:
            ioGroup_ = value;
        break;
            
        case FREDIO2:
            switch (ioGroup_)
            {
                case IO_GRP_FRED_KEYPAD:
                    if (value  == 1)
                        inpMode_ = INP_MODE_KEYPAD;
                    else
                        inpMode_ = INP_MODE_NONE;
                break;
                    
                case IO_GRP_FRED_TV:
                    if (value == 0)
                        pixiePointer->outPixie();
                    else
                        pixiePointer->inPixie();
                    displayType_ = value;
                break;
                    
                case IO_GRP_FRED_TAPE:
					if ((value & 0x10) == 0x10)
	                    inpMode_ = INP_MODE_TAPE_PROGRAM;

					if ((value & 0x20) == 0x20)
	                    inpMode_ = INP_MODE_TAPE_DIRECT;

					if (value == 0)
                        inpMode_ = INP_MODE_NONE;

					if (inpMode_ == INP_MODE_TAPE_DIRECT || inpMode_ == INP_MODE_TAPE_PROGRAM)
					{
						pulseLength_ = 0;
						lastSample_ = 0;
						pulseCount_ = 0;
						tapeInput_ = 0;
						polarity_ = 0;
						totalLength_ = 0;
						silenceCount_ = 0;
						bitNumber_ = -1;
                        if (tapeActivated_)
                            p_Computer->restartTapeLoad();
                        else
                            p_Main->startCassetteLoad();
                        tapeActivated_ = true;
						tapeRunSwitch_ = 0x3;
					}
                break;
            }
        break;

        case FREDIO3:
            if ((value&1) != (tapeRunSwitch_&1))
            {
                if ((value&1) == 1)
                {
                    if (tapeActivated_)
                        p_Computer->restartTapeLoad();
                    else
                        p_Main->startCassetteLoad();
                    
                    tapeActivated_ = true;
                    pulseCount_ = 0;
                    totalLength_ = 0;
                    pulseLength_ = 0;
                }
                else
                    p_Computer->pauseTape();
            }
            
            if ((value&2) != (tapeRunSwitch_&2))
            {
                if ((value&2) == 2)
                    p_Computer->setVolume(p_Main->getVolume(FRED));
                else
                {
                    if ((value&4) != 4)
                        p_Computer->setVolume(0);
                }
            }
            
            if ((value&4) != (tapeRunSwitch_&4))
            {
                psaveAmplitudeChange((value>>2)&1);
            }

            tapeRunSwitch_ = value;
        break;
	}
}

void Fred::cycle(int type)
{
	switch(cycleType_[type])
	{
		case 0:
			return;
		break;

		case PIXIECYCLE:
			pixiePointer->cyclePixieFred(displayType_);
		break;
	}
}

void Fred::onRunButton()
{
    setClear(1);
    setWait(1);
    p_Main->eventUpdateTitle();
    p_Main->startTime();
}
                                         
void Fred::startComputer()
{
    double zoom = p_Main->getZoom();
    double scale = p_Main->getScale();
    pixiePointer = new Pixie( "FRED", p_Main->getPixiePos(FRED), wxSize(64*zoom, 128*zoom), zoom, scale, FRED);
    p_Video = pixiePointer;

    resetPressed_ = false;

	p_Main->setSwName("");
    
    ramMask_ = ((2<<p_Main->getRamType(FRED))<<10)-1;
    
    defineMemoryType(0x0, ramMask_, RAM);
    for (int i=ramMask_+1; i<0xff00; i+=(ramMask_+1))
        defineMemoryType(i, i+ramMask_, MAPPEDRAM);
    
    initRam(0x0, ramMask_);
    p_Main->assDefault("fred", 0, ramMask_);

    readProgram(p_Main->getRamDir(FRED), p_Main->getRamFile(FRED), RAM, 0, NONAME);
    
    if (mainMemory_[0] == 0 && mainMemory_[0x2a] == 0xF8 && mainMemory_[0x100] == 0 && mainMemory_[0x210] == 0x52)
    {
        chip8baseVar_ = 0x100;
        chip8mainLoop_ = 0x13B;
        chip8type_ = CHIPFEL1;
    }
    if (chip8type_ != CHIP_NONE)
        p_Main->defineFelCommands_(chip8type_);

	pixiePointer->configurePixieFred();
	pixiePointer->initPixie();
	pixiePointer->setZoom(zoom);
	pixiePointer->Show(true);
	setWait(1);
	setClear(0);
	setWait(1);
	setClear(1);

	p_Main->updateTitle();

	cpuCycles_ = 0;
	p_Main->startTime();

	threadPointer->Run();
    if (mainMemory_[0] == 0)
        p_Computer->dmaOut(); // skip over IDL instruction, must be a RCA FRED COSMAC 1801 Game System
}

void Fred::writeMemDataType(Word address, Byte type)
{
	switch (memoryType_[address/256])
	{
		case RAM:
        case ROM:
			if (mainMemoryDataType_[address] != type)
			{
				p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
				mainMemoryDataType_[address] = type;
			}
		break;
            
		case MAPPEDRAM:
			address = address & 0x7ff;
			if (mainMemoryDataType_[address] != type)
			{
				p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
				mainMemoryDataType_[address] = type;
			}
		break;
	}
}

Byte Fred::readMemDataType(Word address)
{
    switch (memoryType_[address/256])
	{
		case RAM:
		case ROM:
			return mainMemoryDataType_[address];
		break;

        case MAPPEDRAM:
            address = address & 0x7ff;
			return mainMemoryDataType_[address];
		break;
	}
	return MEM_TYPE_UNDEFINED;
}

Byte Fred::readMem(Word addr)
{
	address_ = addr;

	switch (memoryType_[addr/256])
	{
		case UNDEFINED:
			return 255;
		break;

        case MAPPEDRAM:
			addr = addr & 0x7ff;
		break;
 	}
    return mainMemory_[addr];
}

void Fred::writeMem(Word addr, Byte value, bool writeRom)
{
	address_ = addr;

	switch (memoryType_[addr/256])
	{
		case RAM:
			if (mainMemory_[addr]==value)
				return;
			mainMemory_[addr]=value;
			if (addr>= memoryStart_ && addr<(memoryStart_+256))
				p_Main->updateDebugMemory(addr);
			p_Main->updateAssTabCheck(addr);
		break;
            
		case MAPPEDRAM:
			addr = addr & 0x7ff;
			if (mainMemory_[addr]==value)
				return;
			mainMemory_[addr]=value;
			if (addr>= memoryStart_ && addr<(memoryStart_+256))
				p_Main->updateDebugMemory(addr);
			p_Main->updateAssTabCheck(addr);
		break;

		default:
			if (writeRom)
				mainMemory_[addr]=value;
		break;
	}
}

void Fred::cpuInstruction()
{
	if (cpuMode_ == RUN)
	{
		if (steps_ != 0)
		{
			cycle0_=0;
			machineCycle();
			if (cycle0_ == 0) machineCycle();
			if (cycle0_ == 0 && steps_ != 0)
			{
				cpuCycle();
				cpuCycles_ += 2;
			}
			if (debugMode_)
				p_Main->showInstructionTrace();
		}
		else
			soundCycle();
        
        playSaveLoad();

		if (resetPressed_)
		{
			resetCpu();
			resetPressed_ = false;
			tapeRunSwitch_ = 0;
            tapeActivated_ = false;

            if (mainMemory_[0] == 0 && mainMemory_[0x2a] == 0xF8 && mainMemory_[0x100] == 0 && mainMemory_[0x210] == 0x52)
            {
                chip8baseVar_ = 0x100;
                chip8mainLoop_ = 0x13B;
                chip8type_ = CHIPFEL1;
            }
            
            setWait(1);
			setClear(0);
			setWait(1);
			setClear(1);
			pixiePointer->initPixie();
            ioGroup_ = 0;
            if (mainMemory_[0] == 0)
                p_Computer->dmaOut(); // skip over IDL instruction, must be a RCA FRED COSMAC 1801 Game System
		}
		if (debugMode_)
			p_Main->cycleDebug();
        if (chip8type_ != CHIP_NONE)
            p_Main->cycleFredDebug();
	}
	else
	{
		pixiePointer->initPixie();
		cpuCycles_ = 0;
		p_Main->startTime();
	}
}

void Fred::onReset()
{
	resetPressed_ = true;
}

void Fred::cassetteFred(short val)
{
    if (inpMode_ != INP_MODE_TAPE_DIRECT && inpMode_ != INP_MODE_TAPE_PROGRAM && (tapeRunSwitch_&1) != 1)
		return;

	wxString message;

	int difference;
	if (val < lastSample_)
		difference = lastSample_ - val;
	else
		difference = val - lastSample_;

	if (difference < 300)
		silenceCount_++;
	else
		silenceCount_ = 0;

	pulseLength_++;
	if (lastSample_ <= 0)
	{
		if (val > 0 && silenceCount_ == 0) 
		{
			pulseCount_++;
	//		message.Printf("zero, p-count %d, p-length %d", pulseCount_, pulseLength_);
	//		p_Main->eventShowTextMessage(message);
			totalLength_ += pulseLength_; 
            pulseLength_ = 0;
		}
	}
	else
	{
		if (val < 0 && silenceCount_ == 0) 
		{
			pulseCount_++;
		//	message.Printf("zero, p-count %d, p-length %d", pulseCount_, pulseLength_);
		//	p_Main->eventShowTextMessage(message);
			totalLength_ += pulseLength_;
            pulseLength_ = 0;
		}
	}

    if (pulseCount_ > 4000 && silenceCount_ > 10)
	{
    //    if ((totalLength_ / pulseCount_) < 10)
    //    {
            p_Computer->pauseTape();
            tapeRunSwitch_ = tapeRunSwitch_ & 2;
     //   }
    }

	if (pulseCount_ > 1 && silenceCount_ > 10)
    {
        if (bitNumber_ == 8)
        {
            if (pulseCount_ > 6 && (polarity_ & 1) != 1)
			{
                ef4State_ = 0;
                message.Printf("Polarity issue at %04X", scratchpadRegister_[0]);
                p_Main->eventShowTextMessage(message);
                bitNumber_ = 0;
                polarity_ = 0;
                tapeInput_ = 0;
                if (inpMode_ == INP_MODE_TAPE_DIRECT)
					dmaIn(tapeInput_);
         //       stopTape();
			}

            else
            {
                if (inpMode_ == INP_MODE_TAPE_DIRECT)
                    dmaIn(tapeInput_);
            }
            if  (inpMode_ == INP_MODE_TAPE_PROGRAM)
                ef1StateTape_ = 0;
        }
        else
        {
            if (bitNumber_ != -1)
            {
                if (pulseCount_ > 6)
                    tapeInput_ = (1 << bitNumber_) | tapeInput_;
            }
            else
            {
                if (pulseCount_ <= 6)
                {
                    message.Printf("Start bit != 1, one bit skipped at %04X", scratchpadRegister_[0]);
                    p_Main->eventShowTextMessage(message);
                    bitNumber_++;
           //         stopTape();
                }
                
            }
            if (pulseCount_ > 6)
                polarity_++;
        }
   //     message.Printf("bit%d, p-count %d, p-length %d, total %d, byte %02X", bitNumber_, pulseCount_, pulseLength_, totalLength_, tapeInput_);
  //      p_Main->eventShowTextMessage(message);

		if (bitNumber_ == 8)
		{
	//		message.Printf("BYTE-result %02X", tapeInput_);
	//		p_Main->eventShowTextMessage(message);
            polarity_ = 0;
            tapeInput_ = 0;
            bitNumber_ = -1;
		}
		else
            bitNumber_++;

		totalLength_ = 0;

		pulseCount_ = 0;
        
    }

    lastSample_ = val;
}

void Fred::cassetteFred(char val)
{
}

void Fred::finishStopTape()
{
    tapeRunSwitch_ = tapeRunSwitch_ & 2;
    tapeActivated_ = false;
}

void Fred::moveWindows()
{
    pixiePointer->Move(p_Main->getPixiePos(FRED));
}

void Fred::updateTitle(wxString Title)
{
    pixiePointer->SetTitle("FRED"+Title);
}

