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

#include "super.h"

SuperScreen::SuperScreen(wxWindow *parent, const wxSize& size)
: Panel(parent, size)
{
}

SuperScreen::~SuperScreen()
{
	delete mainBitmapPointer;

#if defined (__WXMAC__)
    delete osx_push_inButtonPointer;
    delete osx_text_resetButtonPointer;
    delete osx_text_loadButtonPointer;
	delete osx_text_runButtonPointer;
	delete osx_pauseButtonPointer;
	delete osx_monitorButtonPointer;
	delete osx_stepButtonPointer;
	delete osx_text_mpButtonPointer;
	for (int i = 0; i<16; i++)
		delete osx_buttonPointer[i];
#else
    delete push_inButtonPointer;
    delete text_resetButtonPointer;
    delete text_loadButtonPointer;
    delete text_runButtonPointer;
    delete pauseButtonPointer;
    delete monitorButtonPointer;
    delete stepButtonPointer;
    delete text_mpButtonPointer;
	for (int i = 0; i<16; i++)
		delete buttonPointer[i];
#endif
	delete powerSwitchButton;

	for (int i=0; i<8; i++)
		delete ledPointer[i];
	for (int i=0; i<4; i++)
		delete addressPointer[i];
 	for (int i=0; i<2; i++)
		delete dataPointer[i];
	delete qLedPointer;
}

void SuperScreen::init()
{
	keyStart_ = 0;
	keyEnd_ = 0;
	lastKey_ = 0;
	forceUpperCase_ = p_Main->getUpperCase(SUPERELF);

	wxClientDC dc(this);
	wxString Number, buttonText;
	int x, y;

	mainBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/superelf.png", wxBITMAP_TYPE_PNG);

	wxSize buttonSize;

    int xPos = 308;
    int yPos = 231;
    int xSize = 30;
    int ySize = 30;

#if defined (__WXMAC__)
    osx_push_inButtonPointer = new HexButton(dc, ELF_HEX_BUTTON, xPos+5*(xSize+2), yPos+3*(ySize+2), "IN");
    osx_text_resetButtonPointer = new HexButton(dc, ELF_HEX_BUTTON, xPos+4*(xSize+2), yPos+ySize+2, "R");
    osx_text_loadButtonPointer = new HexButton(dc, ELF_HEX_BUTTON, xPos+4*(xSize+2), yPos, "L");
	osx_text_runButtonPointer = new HexButton(dc, ELF_HEX_BUTTON, xPos+4*(xSize+2), yPos+2*(ySize+2), "G");
	osx_pauseButtonPointer = new HexButton(dc, ELF_HEX_BUTTON, xPos+4*(xSize+2), yPos+3*(ySize+2), "W");
	osx_monitorButtonPointer = new HexButton(dc, ELF_HEX_BUTTON, xPos+5*(xSize+2), yPos, "M");
	osx_stepButtonPointer = new HexButton(dc, ELF_HEX_BUTTON, xPos+5*(xSize+2), yPos+(ySize+2), "S");
	osx_text_mpButtonPointer = new HexButton(dc, ELF_HEX_BUTTON, xPos+5*(xSize + 2), yPos+2*(ySize+2), "P");
	for (int i = 0; i<16; i++)
	{
		buttonText.Printf("%01X", i);
		x = xPos + (i & 0x3)*(xSize + 2);
		y = (yPos + 3 * (ySize + 2)) - (int)(i / 4 * (ySize + 2));
		osx_buttonPointer[i] = new HexButton(dc, ELF_HEX_BUTTON, x, y, buttonText);
	}
#else
    push_inButtonPointer = new PushButton(this, 20, "IN", wxPoint(xPos+5*(xSize+2), yPos+3*(ySize+2)), wxSize(xSize, ySize), 0);
    text_loadButtonPointer = new wxButton(this, 24, "L", wxPoint(xPos+4*(xSize+2), yPos), wxSize(xSize, ySize));
    text_loadButtonPointer->SetToolTip("Load");
    text_resetButtonPointer = new wxButton(this, 27, "R", wxPoint(xPos+4*(xSize+2), yPos+ySize+2), wxSize(xSize, ySize));
    text_resetButtonPointer->SetToolTip("Reset");
    text_runButtonPointer = new wxButton(this, 21, "G", wxPoint(xPos+4*(xSize+2), yPos+2*(ySize+2)), wxSize(xSize, ySize));
    text_runButtonPointer->SetToolTip("Go - RUN");
    pauseButtonPointer = new wxButton(this, 25, "W", wxPoint(xPos+4*(xSize+2), yPos+3*(ySize+2)), wxSize(xSize, ySize));
    pauseButtonPointer->SetToolTip("Wait");
    monitorButtonPointer = new wxButton(this, 28, "M", wxPoint(xPos+5*(xSize+2), yPos), wxSize(xSize, ySize));
    monitorButtonPointer->SetToolTip("enable Monitor");
    stepButtonPointer = new wxButton(this, 26, "S", wxPoint(xPos+5*(xSize+2), yPos+(ySize+2)), wxSize(xSize, ySize));
    stepButtonPointer->SetToolTip("Single Step");
    text_mpButtonPointer = new wxButton(this, 22, "P", wxPoint(xPos+5*(xSize+2), yPos+2*(ySize+2)), wxSize(xSize, ySize));
    text_mpButtonPointer->SetToolTip("memory Protect");
	for (int i = 0; i<16; i++)
	{
		buttonText.Printf("%01X", i);
		x = xPos + (i & 0x3)*(xSize + 2);
		y = (yPos + 3 * (ySize + 2)) - (int)(i / 4 * (ySize + 2));
		buttonPointer[i] = new PushButton(this, i, buttonText, wxPoint(x, y), wxSize(xSize, ySize), 0);
	}
#endif
    
    powerSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(0xbd, 0xb2, 0xa5), BUTTON_UP, 464, 42, "");

	for (int i=0; i<8; i++)
	{
		ledPointer[i] = new Led(dc, 284, 233+15*i, SUPERELFLED);
		updateLed_[i] = true;
	}
	for (int i=0; i<4; i++)
	{
		addressPointer[i] = new Til311();
		addressPointer[i]->init(dc, 304+i*28, 140);
	}
	updateAddress_ = true;
	for (int i=0; i<2; i++)
	{
		dataPointer[i] = new Til311();
		dataPointer[i]->init(dc, 434+i*28,140);
	}
	updateData_ = true;

	qLedPointer = new Led(dc, 284, 210, SUPERELFLED);
	updateQLed_ = true;
	this->connectKeyEvent(this);
}

void SuperScreen::onPaint(wxPaintEvent&WXUNUSED(event))
{
	wxPaintDC dc(this);
	dc.DrawBitmap(*mainBitmapPointer, 0, 0);

	addressPointer[3]->onPaint(dc);
	addressPointer[2]->onPaint(dc);
	addressPointer[1]->onPaint(dc);
	addressPointer[0]->onPaint(dc);
	dataPointer[1]->onPaint(dc);
	dataPointer[0]->onPaint(dc);
	qLedPointer->onPaint(dc);
	for (int i=0; i<8; i++)
		ledPointer[i]->onPaint(dc);
    powerSwitchButton->onPaint(dc);
#if defined (__WXMAC__)
    osx_push_inButtonPointer->onPaint(dc);
    osx_text_resetButtonPointer->onPaint(dc);
    osx_text_loadButtonPointer->onPaint(dc);
	osx_text_runButtonPointer->onPaint(dc);
	osx_pauseButtonPointer->onPaint(dc);
	osx_monitorButtonPointer->onPaint(dc);
	osx_stepButtonPointer->onPaint(dc);
	osx_text_mpButtonPointer->onPaint(dc);
	for (int i = 0; i<16; i++)
    osx_buttonPointer[i]->onPaint(dc);
#endif
}

void SuperScreen::onMouseRelease(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);
    
    wxClientDC dc(this);
    
    if (powerSwitchButton->onMouseRelease(dc, x, y))
        p_Main->stopComputer();
    
#if defined (__WXMAC__)
    if (osx_push_inButtonPointer->onMouseRelease(dc, x, y))
        p_Computer->onInButtonRelease();
    osx_text_resetButtonPointer->onMouseRelease(dc, x, y);
	osx_text_loadButtonPointer->onMouseRelease(dc, x, y);
	osx_text_runButtonPointer->onMouseRelease(dc, x, y);
	osx_pauseButtonPointer->onMouseRelease(dc, x, y);
	osx_monitorButtonPointer->onMouseRelease(dc, x, y);
	osx_stepButtonPointer->onMouseRelease(dc, x, y);
	osx_text_mpButtonPointer->onMouseRelease(dc, x, y);
	for (int i = 0; i<16; i++)
		osx_buttonPointer[i]->onMouseRelease(dc, x, y);
#endif
}

void SuperScreen::onMousePress(wxMouseEvent&event)
{
	int x, y;
	event.GetPosition(&x, &y);

	wxClientDC dc(this);

#if defined (__WXMAC__)
	if (osx_push_inButtonPointer->onMousePress(dc, x, y))
		p_Computer->onInButtonPress();

	if (osx_text_resetButtonPointer->onMousePress(dc, x, y))
		p_Computer->onResetButton();

	if (osx_text_loadButtonPointer->onMousePress(dc, x, y))
		p_Computer->onLoadButton();

	if (osx_text_runButtonPointer->onMousePress(dc, x, y))
		p_Computer->onRunButton();

	if (osx_pauseButtonPointer->onMousePress(dc, x, y))
		p_Computer->onPause();

	if (osx_monitorButtonPointer->onMousePress(dc, x, y))
		p_Computer->onMonitor();

	if (osx_stepButtonPointer->onMousePress(dc, x, y))
		p_Computer->onSingleStep();

	if (osx_text_mpButtonPointer->onMousePress(dc, x, y))
		p_Computer->onMpButton();

	for (int i = 0; i<16; i++)
	{
		if (osx_buttonPointer[i]->onMousePress(dc, x, y))
		{
			p_Computer->onNumberKeyDown(i);
		}
	}
#endif
}

void SuperScreen::releaseButtonOnScreen(HexButton* buttonPoint)
{
	wxClientDC dc(this);

	buttonPoint->releaseButtonOnScreen(dc);
}

BEGIN_EVENT_TABLE(Super, wxFrame)
	EVT_CLOSE (Super::onClose)
	EVT_COMMAND(20, wxEVT_ButtonDownEvent, Super::onButtonPress)
	EVT_COMMAND(20, wxEVT_ButtonUpEvent, Super::onButtonRelease)
	EVT_BUTTON(21, Super::onRunButton)
	EVT_BUTTON(22, Super::onMpButton)
	EVT_BUTTON(24, Super::onLoadButton)
	EVT_BUTTON(25, Super::onPause)
	EVT_BUTTON(26, Super::onSingleStep)
	EVT_BUTTON(27, Super::onResetButton)
	EVT_BUTTON(28, Super::onMonitor)
	EVT_COMMAND(0, wxEVT_ButtonDownEvent, Super::onNumberKeyDown)
	EVT_COMMAND(1, wxEVT_ButtonDownEvent, Super::onNumberKeyDown)
	EVT_COMMAND(2, wxEVT_ButtonDownEvent, Super::onNumberKeyDown)
	EVT_COMMAND(3, wxEVT_ButtonDownEvent, Super::onNumberKeyDown)
	EVT_COMMAND(4, wxEVT_ButtonDownEvent, Super::onNumberKeyDown)
	EVT_COMMAND(5, wxEVT_ButtonDownEvent, Super::onNumberKeyDown)
	EVT_COMMAND(6, wxEVT_ButtonDownEvent, Super::onNumberKeyDown)
	EVT_COMMAND(7, wxEVT_ButtonDownEvent, Super::onNumberKeyDown)
	EVT_COMMAND(8, wxEVT_ButtonDownEvent, Super::onNumberKeyDown)
	EVT_COMMAND(9, wxEVT_ButtonDownEvent, Super::onNumberKeyDown)
	EVT_COMMAND(10, wxEVT_ButtonDownEvent, Super::onNumberKeyDown)
	EVT_COMMAND(11, wxEVT_ButtonDownEvent, Super::onNumberKeyDown)
	EVT_COMMAND(12, wxEVT_ButtonDownEvent, Super::onNumberKeyDown)
	EVT_COMMAND(13, wxEVT_ButtonDownEvent, Super::onNumberKeyDown)
	EVT_COMMAND(14, wxEVT_ButtonDownEvent, Super::onNumberKeyDown)
	EVT_COMMAND(15, wxEVT_ButtonDownEvent, Super::onNumberKeyDown)
	EVT_COMMAND(0, wxEVT_ButtonUpEvent, Super::onNumberKeyUp)
	EVT_COMMAND(1, wxEVT_ButtonUpEvent, Super::onNumberKeyUp)
	EVT_COMMAND(2, wxEVT_ButtonUpEvent, Super::onNumberKeyUp)
	EVT_COMMAND(3, wxEVT_ButtonUpEvent, Super::onNumberKeyUp)
	EVT_COMMAND(4, wxEVT_ButtonUpEvent, Super::onNumberKeyUp)
	EVT_COMMAND(5, wxEVT_ButtonUpEvent, Super::onNumberKeyUp)
	EVT_COMMAND(6, wxEVT_ButtonUpEvent, Super::onNumberKeyUp)
	EVT_COMMAND(7, wxEVT_ButtonUpEvent, Super::onNumberKeyUp)
	EVT_COMMAND(8, wxEVT_ButtonUpEvent, Super::onNumberKeyUp)
	EVT_COMMAND(9, wxEVT_ButtonUpEvent, Super::onNumberKeyUp)
	EVT_COMMAND(10, wxEVT_ButtonUpEvent, Super::onNumberKeyUp)
	EVT_COMMAND(11, wxEVT_ButtonUpEvent, Super::onNumberKeyUp)
	EVT_COMMAND(12, wxEVT_ButtonUpEvent, Super::onNumberKeyUp)
	EVT_COMMAND(13, wxEVT_ButtonUpEvent, Super::onNumberKeyUp)
	EVT_COMMAND(14, wxEVT_ButtonUpEvent, Super::onNumberKeyUp)
	EVT_COMMAND(15, wxEVT_ButtonUpEvent, Super::onNumberKeyUp)
END_EVENT_TABLE()

Super::Super(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
	elfConfiguration = conf;
	wxClientDC dc(this);
	elfClockSpeed_ = clock;
    
#ifndef __WXMAC__
	SetIcon(wxICON(app_icon));
#endif

	superScreenPointer = new SuperScreen(this, size);
	superScreenPointer->init();

	this->SetClientSize(size);
	lastAddress_ = 0;

    runningGame_ = "";
}

Super::~Super()
{
	if (elfConfiguration.usePixie)
	{
		p_Main->setPixiePos(SUPERELF, pixiePointer->GetPosition());
		pixiePointer->Destroy();
	}
	if (elfConfiguration.useTMS9918)
	{
		p_Main->setTmsPos(SUPERELF, tmsPointer->GetPosition());
		tmsPointer->Destroy();
	}
	if (elfConfiguration.use6845||elfConfiguration.useS100)
	{
		p_Main->set6845Pos(SUPERELF, mc6845Pointer->GetPosition());
		mc6845Pointer->Destroy();
	}
	if (elfConfiguration.use6847)
	{
		p_Main->set6847Pos(SUPERELF, mc6847Pointer->GetPosition());
		mc6847Pointer->Destroy();
	}
	if (elfConfiguration.use8275)
	{
		p_Main->set8275Pos(SUPERELF, i8275Pointer->GetPosition());
		i8275Pointer->Destroy();
	}
	if (elfConfiguration.vtType != VTNONE)
	{
		p_Main->setVtPos(SUPERELF, vtPointer->GetPosition());
		vtPointer->Destroy();
	}
    if (elfConfiguration.vtExternal)
        delete p_Serial;
	if (p_Main->getPrinterStatus(SUPERELF))
	{
		p_Printer->closeFrames();
		delete p_Printer;
	}
	p_Main->setMainPos(SUPERELF, GetPosition());

	delete superScreenPointer;
}

void Super::onClose(wxCloseEvent&WXUNUSED(event) )
{
	p_Main->stopComputer();
}

void Super::charEvent(int keycode)
{
	if (elfConfiguration.useKeyboard)
		charEventKeyboard(keycode);
}

bool Super::keyDownPressed(int key)
{
	onHexKeyDown(key);
	if (elfConfiguration.UsePS2)
	{
		keyDownPs2(key);
		return true;
	}
	return false;
}

bool Super::keyUpReleased(int key)
{
	if (key == inKey1_ || key == inKey2_)
	{
		onInButtonRelease();
		return true;
	}
	onHexKeyUp(key);
//	if (elfConfiguration.useKeyboard)
//		keyboardUp();
	if (elfConfiguration.UsePS2)
	{
		keyUpPs2(key);
		return true;
	}
	return false;
}

void Super::onButtonRelease(wxCommandEvent& event)
{
	onInButtonRelease();
	event.Skip();
}

void Super::onButtonPress(wxCommandEvent& event)
{
	onInButtonPress();
	event.Skip();
}

void Super::onInButtonPress()
{
	if (cpuMode_ == LOAD)
	{
		Byte value = getData();
		dmaIn(value);
		showData(value);
	}
	else
	{
		ef4State_ = 0;
	}
}

void Super::onInButtonRelease()
{
	ef4State_ = 1;
}

void Super::configureComputer()
{
	efType_[1] = EF1UNDEFINED;
	efType_[2] = EF2UNDEFINED;
	efType_[3] = EF3UNDEFINED;
    setCycleType(COMPUTERCYCLE, LEDCYCLE);

//	int efPort;
	wxString printBuffer;

//	int input = p_Main->getConfigItem("/SuperElf/HexInput", 4l);
//	int output = p_Main->getConfigItem("/SuperElf/HexOutput", 4l);

	p_Main->message("Configuring Super Elf");
	printBuffer.Printf("	Output %d: display output, input %d: data input", elfConfiguration.elfPortConf.hexOutput, elfConfiguration.elfPortConf.hexInput);
	p_Main->message(printBuffer);

	p_Computer->setInType(elfConfiguration.elfPortConf.hexInput, SUPERIN);
	p_Computer->setOutType(elfConfiguration.elfPortConf.hexOutput, SUPEROUT);

    if (elfConfiguration.useRomMapper)
    {
//        output = p_Main->getConfigItem("/SuperElf/EmsOutput", 7l);
        printBuffer.Printf("	Output %d: rom mapper", elfConfiguration.elfPortConf.emsOutput);
        p_Computer->setOutType(elfConfiguration.elfPortConf.emsOutput, ROMMAPPEROUT);
        p_Main->message(printBuffer);
    }
    if (elfConfiguration.useEms)
    {
//        output = p_Main->getConfigItem("/SuperElf/EmsOutput", 7l);
        printBuffer.Printf("	Output %d: EMS-512KB", elfConfiguration.elfPortConf.emsOutput);
        p_Computer->setOutType(elfConfiguration.elfPortConf.emsOutput, EMSMAPPEROUT);
        p_Main->message(printBuffer);
    }
	if (elfConfiguration.useTape)
	{
//		efPort = p_Main->getConfigItem("/SuperElf/TapeEf", 2l);
//		efPort = p_Main->getConfigItem("/SuperElf/TapeEf", 3l);
		efType_[elfConfiguration.elfPortConf.tapeEf] = ELF2EF2;
		printBuffer.Printf("	EF %d: cassette in", elfConfiguration.elfPortConf.tapeEf);
		p_Main->message(printBuffer);
	}
	if (elfConfiguration.useHexKeyboardEf3)
	{
//		efPort = p_Main->getConfigItem("/SuperElf/HexEf", 3l);
		efType_[elfConfiguration.elfPortConf.hexEf] = SUPEREF3;
		printBuffer.Printf("	EF %d: 0 when hex button pressed", elfConfiguration.elfPortConf.hexEf);
		p_Main->message(printBuffer);
	}
	if (efType_[4] == 0)
	{
		efType_[4] = ELFINEF;
		p_Main->message("	EF 4: 0 when in button pressed");
	}
	p_Main->message("");

    inKey1_ = p_Main->getDefaultInKey1("SuperElf");
    inKey2_ = p_Main->getDefaultInKey2("SuperElf");
    p_Main->getDefaultHexKeys(SUPERELF, "SuperElf", "A", keyDefA1_, keyDefA2_, keyDefGameHexA_);


	if (p_Main->getConfigBool("/SuperElf/GameAuto", true))
		p_Main->loadKeyDefinition(p_Main->getRomFile(SUPERELF, MAINROM1), p_Main->getRomFile(SUPERELF, MAINROM2), keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
        
	resetCpu();
}

void Super::setPrinterEf()
{
    ef3State_ = 0;
}

void Super::reLoadKeyDefinition(wxString fileName)
{
    runningGame_ = fileName;
    if (p_Main->getConfigBool("/SuperElf/GameAuto", true))
        p_Main->loadKeyDefinition("", fileName, keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
}

void Super::reDefineKeysA(int hexKeyDefA1[], int hexKeyDefA2[])
{
    for (int i=0; i<16; i++)
    {
        keyDefA1_[i] = hexKeyDefA1[i];
        keyDefA2_[i] = hexKeyDefA2[i];
    }
}

void Super::initComputer()
{
	Show(p_Main->getUseElfControlWindows(SUPERELF));
	mpButtonState_ = 0;
	ef4State_ = 1;
	switches_ = 0;
	lastMode_ = UNDEFINDEDMODE;
	monitor_ = false;
 	singleStep_ = 0;
	ef3State_ = 1;
	ef4State_ = 1;
	elfRunState_ = RESETSTATE;
	cassetteEf_ = 0;
}

Byte Super::ef(int flag)
{
	switch(efType_[flag])
	{
		case 0:
			return 1;
		break;

		case PIXIEEF:
			return pixiePointer->efPixie();
		break;

		case KEYBRDEF:
			return efKeyboard();
		break;

		case PS2GPIOEF:
			return efPs2gpio();
		break;

		case PS2EF:
			return efPs2();
		break;

		case FDCEF:
			return ef1793();
		break;

		case VT100EF:
			return vtPointer->ef();
		break;

        case VTSERIALEF:
            return p_Serial->ef();
        break;
 
		case MC6847EF:
			return mc6845Pointer->ef6845();
		break;

		case I8275EF:
			return i8275Pointer->ef8275();
		break;

		case ELF2EF2:
			return cassetteEf_;
		break;

		case ELFINEF:
			return ef4();
		break;

		case VTINEF:
			if (ef4State_ == 0)
				return 0;
			else
				return vtPointer->ef();
		break;

		case EF1UNDEFINED:
			return elfConfiguration.elfPortConf.ef1default;
		break;

		case EF2UNDEFINED:
			return elfConfiguration.elfPortConf.ef2default;
		break;

		case EF3UNDEFINED:
			return elfConfiguration.elfPortConf.ef3default;
		break;

		case SUPEREF3:
		case ELFPRINTEREF:
			return ef3State_;
		break;

		default:
			return 1;
	}
}

Byte Super::ef4()
{
	return ef4State_;
}

Byte Super::in(Byte port, Word WXUNUSED(address))
{
	Byte ret;

	switch(inType_[port])
	{
		case 0:
			ret = 255;
		break;

		case PIXIEIN:
			ret = pixiePointer->inPixie();
		break;

		case I8275PREGREAD:
			ret = i8275Pointer->pRegRead();
		break;

		case I8275SREGREAD:
			ret = i8275Pointer->sRegRead();
		break;

		case KEYBRDIN:
			ret = inKeyboard();
		break;

		case PS2GPIOIN:
			ret = inPs2gpio();
		break;

		case PS2IN:
			ret = inPs2();
		break;

		case SUPERIN:
			ret = getData();
		break;

		case FDCIN:
			ret = in1793();
		break;

		case IDEIN:
			ret = inIde();
		break;

		case PORTEXTIN:
			ret = inPortExtender();
		break;

		case UARTIN:
			return vtPointer->uartIn();
		break;

		case UARTSTATUS:
			return vtPointer->uartStatus();
		break;

		default:
			ret = 255;
	}
	inValues_[port] = ret;
	return ret;
}

Byte Super::getData()
{
	return switches_;
}

void Super::out(Byte port, Word WXUNUSED(address), Byte value)
{
	outValues_[port] = value;

	switch(outType_[port])
	{
		case 0:
			return;
		break;

		case TMSHIGHOUT:
			tmsPointer->modeHighOut(value);
		break;

		case TMSLOWOUT:
			tmsPointer->modeLowOut(value);
		break;

		case PIXIEOUT:
			pixiePointer->outPixie();
		break;

		case MC6847OUT:
			mc6847Pointer->outMc6847(value);
		break;

		case I8275PREGWRITE:
			i8275Pointer->pRegWrite(value);
		break;

		case I8275CREGWRITE:
			i8275Pointer->cRegWrite(value);
		break;

		case VT100OUT:
			vtPointer->out(value);
		break;

		case PRINTEROUT:
//			p_Main->eventPrintDefault(value);
			if ((value & 0xfc) != 0)
				p_Printer->printerOut(value);
		break;

		case PS2OUT:
			outPs2(value);
		break;

		case SUPEROUT:
			showData(value);
		break;

		case FDCSELECTOUT:
			selectRegister1793(value);
		break;

		case FDCWRITEOUT:
			writeRegister1793(value);
		break;

		case IDESELECTOUT:
			selectIdeRegister(value);
		break;

		case IDEWRITEOUT:
			outIde(value);
		break;

		case PORTEXTSELECTOUT:
			selectPortExtender(value);
		break;

		case PORTEXTWRITEOUT:
			outPortExtender(value);
		break;

		case UARTOUT:
            vtPointer->uartOut(value);
		break;

		case UARTCONTROL:
            vtPointer->uartControl(value);
		break;

        case ROMMAPPEROUT:
            setRomMapper(value);
        break;

        case EMSMAPPEROUT:
            setEmsPage(value);
        break;
    }
}

void Super::showData(Byte val)
{
	superScreenPointer->showData(val);
}

void Super::cycle(int type)
{
	switch(cycleType_[type])
	{
		case 0:
			return;
		break;

		case KEYBRDCYCLE:
			cycleKeyboard();
		break;

		case PS2GPIOCYCLE:
			cyclePs2gpio();
		break;

		case PS2CYCLE:
			cyclePs2();
		break;

		case PIXIECYCLE:
			pixiePointer->cyclePixie();
		break;

		case TMSCYCLE:
			tmsPointer->cycleTms();
		break;

		case MC6847CYCLE:
			mc6847Pointer->cycle6847();
		break;

		case MC6845BLINK:
			mc6845Pointer->blink6845();
		break;

		case MC6845CYCLE:
			mc6845Pointer->cycle6845();
		break;

		case I8275CYCLE:
			i8275Pointer->cycle8275();
		break;

		case VT100CYCLE:
			vtPointer->cycleVt();
		break;

        case VTSERIALCYCLE:
            p_Serial->cycleVt();
        break;

		case FDCCYCLE:
			cycleFdc();
		break;

		case IDECYCLE:
			cycleIde();
		break;

        case LEDCYCLE:
            cycleLed();
        break;
    }
}

void Super::cycleLed()
{
    if (ledCycleValue_ > 0)
    {
        ledCycleValue_ --;
        if (ledCycleValue_ <= 0)
        {
            ledCycleValue_ = ledCycleSize_;
            superScreenPointer->ledTimeout();
        }
    }
}

void Super::cycleA()
{
	if (cpuMode_ != lastMode_)
	{
		superScreenPointer->setLed(0,0);
		superScreenPointer->setLed(1,0);
		superScreenPointer->setLed(2,0);
		superScreenPointer->setLed(3,0);
		switch(cpuMode_)
		{
			case LOAD: superScreenPointer->setLed(0,1); break;
			case RESET: superScreenPointer->setLed(1,1); break;
			case RUN: superScreenPointer->setLed(2,1); break;
			case PAUSE: superScreenPointer->setLed(3,1); break;
		}
		lastMode_ = cpuMode_;
	}
	if (cpuMode_ == RUN && singleStep_ != 0 && state_ == 'F')
	{
		superScreenPointer->setLed(4,1);
		superScreenPointer->setLed(5,0);
		state_ = 'E';
		setWait(0);
	}
}

void Super::cycleB()
{
	if (singleStep_ != 0)
	{
		superScreenPointer->setLed(4,0);
		superScreenPointer->setLed(5,1);
		setWait(0);
	}
}

void Super::autoBoot()
{
	setClear(1);
	setWait(1);
}

void Super::switchQ(int value)
{
	superScreenPointer->setQLed(value);

    if (elfConfiguration.vtType != VTNONE)
        vtPointer->switchQ(value);

    if (elfConfiguration.vtExternal)
        p_Serial->switchQ(value);
}

int Super::getMpButtonState()
{
	return mpButtonState_;
}

void Super::onRunButton(wxCommandEvent&WXUNUSED(event))
{
	onRunButton();
}

void Super::onRunButton()
{
	setClear(1);
	setWait(1);
	p_Main->eventUpdateTitle();
	p_Main->startTime();
}

void Super::onRun()
{
	stopTape();
	if (getClear()==0)
	{
		setClear(1);
		setWait(1);
		p_Main->eventUpdateTitle();
		p_Main->startTime();
	}
	else
	{
		lastMode_ = cpuMode_;
		setClear(0);
		setWait(1);
		p_Main->eventUpdateTitle();
		if (cpuMode_ == RESET)
			superScreenPointer->showAddress(0);
		singleStep_ = 0;
		mpButtonState_ = 0;
		monitor_ = false;
		state_ = 'F';
	}
}

void Super::onPause(wxCommandEvent&WXUNUSED(event))
{
	onPause();
}

void Super::onPause()
{
	setClear(1);
	setWait(0);
	p_Main->eventUpdateTitle();
	mpButtonState_ = 0;
	singleStep_ = 0;
}

void Super::onMpButton(wxCommandEvent&WXUNUSED(event))
{
	mpButtonState_ = 1;
}

void Super::onMpButton()
{
	mpButtonState_ = 1;
}

void Super::onMonitor(wxCommandEvent&WXUNUSED(event))
{
	onMonitor();
}

void Super::onMonitor()
{
	if (baseGiantBoard_ != 0x10000)
	{
		setClear(1);
		setWait(1);
		scratchpadRegister_[0] = baseGiantBoard_;
	}
	else monitor_ = true;
}

void Super::onLoadButton(wxCommandEvent&WXUNUSED(event))
{
    onLoadButton();
}

void Super::onLoadButton()
{
    if (cpuMode_ != LOAD)
        superScreenPointer->showAddress(0);
    lastMode_ = cpuMode_;
    setClear(0);
    setWait(0);
}

void Super::onSingleStep(wxCommandEvent&WXUNUSED(event))
{
	singleStep_ = 1;
}

void Super::onSingleStep()
{
	singleStep_ = 1;
}

void Super::onResetButton(wxCommandEvent&WXUNUSED(event))
{
    onResetButton();
}

void Super::onResetButton()
{
    lastMode_ = cpuMode_;
    setClear(0);
    setWait(1);
    if (cpuMode_ == RESET)
        superScreenPointer->showAddress(0);
    singleStep_ = 0;
    mpButtonState_ = 0;
    monitor_ = false;
    state_ = 'F';
}

void Super::onNumberKeyDown(wxCommandEvent&event)
{
	int i = event.GetId();
	onNumberKeyDown(i);
}

void Super::onNumberKeyDown(int i)
{
	ef3State_ = 0;
	switches_ = ((switches_ << 4) & 0xf0) | i;
}

void Super::onNumberKeyUp(wxCommandEvent&WXUNUSED(event))
{
	ef3State_ = 1;
}

void Super::onHexKeyDown(int keycode)
{
	for (int i=0; i<16; i++)
	{
        if (keycode == keyDefA1_[i])
        {
            ef3State_ = 0;
            switches_ = ((switches_ << 4) & 0xf0) | i;
        }
        if (keycode == keyDefA2_[i])
        {
            ef3State_ = 0;
            switches_ = ((switches_ << 4) & 0xf0) | i;
        }
	}
}

void Super::onHexKeyUp(int keycode)
{
    if (elfConfiguration.useHexKeyboard)
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

    ef3State_ = 1;
}

void Super::startComputer()
{
	startElfKeyFile("SuperElf");
	resetPressed_ = false;

	if (elfConfiguration.usePortExtender)
		configurePortExt(elfConfiguration.elfPortConf);

	ramStart_ = p_Main->getStartRam("SuperElf", SUPERELF);
	Word ramEnd = p_Main->getEndRam("SuperElf", SUPERELF);
	ramMask_ = 0x100;
	while (ramMask_ < (ramEnd + 1 - ramStart_))
		ramMask_ *= 2;
	ramMask_ -= 1;

	defineMemoryType(ramStart_, ramEnd, RAM);
    initRam(ramStart_, ramEnd);

    p_Main->assDefault("mycode", ramStart_&0xff00, ((ramEnd&0xff00)|0xff)&0xfff);

	for (int i=ramMask_ + 1 + ramStart_; i<0x10000; i+=(ramMask_+1))
		defineMemoryType(i, i+(ramEnd - ramStart_), MAPPEDRAM);

	if (elfConfiguration.usePager)
	{
		allocPagerMemory();
		definePortExtForPager();

		defineMemoryType(0, 65535, PAGER);
	}
	if (elfConfiguration.useEms)
	{
		allocEmsMemory();
		defineMemoryType(0x8000, 0xbfff, EMSMEMORY);
	}
    if (elfConfiguration.useRomMapper)
    {
        readRomMapperBinFile(p_Main->getRomDir(SUPERELF, MAINROM1)+p_Main->getRomFile(SUPERELF, MAINROM1));
        defineMemoryType(0x8000, 0xffff, ROMMAPPER);
    }

    p_Main->enableDebugGuiMemory();

    Word offset = 0;
    wxString fileName = p_Main->getRomFile(SUPERELF, MAINROM1);
    if (fileName.Right(4) == ".ch8")
        offset = 0x200;
    if (!elfConfiguration.useRomMapper)
        readProgram(p_Main->getRomDir(SUPERELF, MAINROM1), p_Main->getRomFile(SUPERELF, MAINROM1), p_Main->getLoadromMode(SUPERELF, 0), offset, NONAME);

    offset = 0;
    fileName = p_Main->getRomFile(SUPERELF, MAINROM2);
    if (fileName.Right(4) == ".ch8")
        offset = 0x200;
	readProgram(p_Main->getRomDir(SUPERELF, MAINROM2), p_Main->getRomFile(SUPERELF, MAINROM2), p_Main->getLoadromMode(SUPERELF, 1), offset, NONAME);

	configureElfExtensions();
	if (elfConfiguration.autoBoot)
	{
		scratchpadRegister_[0]=p_Main->getBootAddress("SuperElf", SUPERELF);
		autoBoot();
	}
	else
	{
		if (baseGiantBoard_ != 0x10000)
		{
			scratchpadRegister_[0] = baseGiantBoard_;
			autoBoot();
		}
	}

	if (elfConfiguration.vtType != VTNONE)
		setEf(4,0);

	p_Main->setSwName("");
	p_Main->updateTitle();
	address_ = 0;
	superScreenPointer->showAddress(address_);

	cpuCycles_ = 0;
	p_Main->startTime();

    int ms = (int) p_Main->getLedTimeMs(SUPERELF);
    superScreenPointer->setLedMs(ms);
    if (ms == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((elfClockSpeed_ * 1000000) / 8) / 1000) * ms;
    ledCycleValue_ = ledCycleSize_;

    if (p_Vt100 != NULL)
        p_Vt100->splashScreen();
    else
        p_Video->splashScreen();
    
    threadPointer->Run();
}

void Super::writeMemDataType(Word address, Byte type)
{
	switch (memoryType_[address/256])
	{
		case EMSMEMORY:
			switch (emsMemoryType_[((address & 0x3fff) |(emsPage_ << 14))/256])
			{
				case ROM:
				case RAM:
					if (emsRamDataType_[(long) ((address & 0x3fff) |(emsPage_ << 14))] != type)
					{
						p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
						emsRamDataType_[(long) ((address & 0x3fff) |(emsPage_ << 14))] = type;
					}
				break;
			}
		break;

        case ROMMAPPER:
            if (emsPage_ <= maxNumberOfPages_)
            {
                switch (romMapperMemoryType_[((address & 0x7fff) |(emsPage_ << 15))/256])
                {
                    case ROM:
                    case RAM:
                        if (expansionRomDataType_[(long) ((address & 0x7fff) |(emsPage_ << 15))] != type)
                        {
                            p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                            expansionRomDataType_[(long) ((address & 0x7fff) |(emsPage_ << 15))] = type;
                        }
                    break;
                }
            }
        break;
            
		case ROM:
			if (mainMemoryDataType_[address] != type)
			{
				p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
				mainMemoryDataType_[address] = type;
			}
		break;

		case MAPPEDRAM:
		case RAM:
			address = (address & ramMask_) + ramStart_;
			if (mainMemoryDataType_[address] != type)
			{
				p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
				mainMemoryDataType_[address] = type;
			}
		break;

		case PAGER:
			switch (pagerMemoryType_[((getPager(address>>12) << 12) |(address &0xfff))/256])
			{
				case ROM:
				case RAM:
					if (mainMemoryDataType_[(getPager(address>>12) << 12) |(address &0xfff)] != type)
					{
						p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
						mainMemoryDataType_[(getPager(address>>12) << 12) |(address &0xfff)] = type;
					}
				break;
			}
		break;

	}
}

Byte Super::readMemDataType(Word address)
{
	switch (memoryType_[address/256])
	{
		case EMSMEMORY:
			switch (emsMemoryType_[((address & 0x3fff) |(emsPage_ << 14))/256])
			{
				case ROM:
				case RAM:
					return emsRamDataType_[(long) ((address & 0x3fff) |(emsPage_ << 14))];
				break;
			}
		break;

        case ROMMAPPER:
            if (emsPage_ <= maxNumberOfPages_)
            {
                switch (romMapperMemoryType_[((address & 0x7fff) |(emsPage_ << 15))/256])
                {
                    case ROM:
                    case RAM:
                        return expansionRomDataType_[(long) ((address & 0x7fff) |(emsPage_ << 15))];
                    break;
                }
            }
            else
                return MEM_TYPE_UNDEFINED;
        break;
            
		case ROM:
			return mainMemoryDataType_[address];
		break;

		case MAPPEDRAM:
		case RAM:
			address = (address & ramMask_) + ramStart_;
			return mainMemoryDataType_[address];
		break;

		case PAGER:
			switch (pagerMemoryType_[((getPager(address>>12) << 12) |(address &0xfff))/256])
			{
				case ROM:
				case RAM:
					return mainMemoryDataType_[(getPager(address>>12) << 12) |(address &0xfff)];
				break;
			}
		break;
	}
	return MEM_TYPE_UNDEFINED;
}

Byte Super::readMem(Word addr)
{
	Byte minimon[] = { 0xf8, 0xff, 0xa1, 0xe1, 0x6c, 0x64, 0xa3, 0x21,
                   0x6c, 0x64, 0x3f, 0x07, 0x37, 0x0c, 0x3a, 0x11,
                   0xd3, 0xe3, 0xf6, 0x33, 0x17, 0x7b, 0x6c, 0x64,
                   0x23, 0x3f, 0x13, 0x37, 0x1b, 0x13, 0x30, 0x13 };

	address_ = addr;
	superScreenPointer->showAddress(address_);

	switch (memoryType_[addr/256])
	{
		case EMSMEMORY:
			switch (emsMemoryType_[((addr & 0x3fff) |(emsPage_ << 14))/256])
			{
				case UNDEFINED:
					return 255;
				break;

				case ROM:
				case RAM:
					return emsRam_[(long) ((addr & 0x3fff) |(emsPage_ << 14))];
				break;

				default:
					return 255;
				break;
			}
		break;

        case ROMMAPPER:
            if (emsPage_ <= maxNumberOfPages_)
            {
                switch (romMapperMemoryType_[((addr & 0x7fff) |(emsPage_ << 15))/256])
                {
                    case UNDEFINED:
                        return 255;
                    break;
                        
                    case ROM:
                    case RAM:
                        return expansionRom_[(long) ((addr & 0x7fff) |(emsPage_ << 15))];
                    break;
                        
                    default:
                        return 255;
                    break;
                }
            }
            else
                return 255;
        break;
            
		case UNDEFINED:
			return 255;
		break;

		case ROM:
			if (addr <32 && monitor_) return minimon[addr];
			return mainMemory_[addr];
		break;

		case MAPPEDRAM:
		case RAM:
			if (addr <32 && monitor_) return minimon[addr];
			addr = (addr & ramMask_) + ramStart_;
			return mainMemory_[addr];
		break;

		case MC6847RAM:
			return mc6847Pointer->read6847(addr);
		break;

		case MC6845RAM:
			return mc6845Pointer->read6845(addr & 0x7ff);
		break;

		case MC6845REGISTERS:
			return mc6845Pointer->readData6845(addr);
		break;

		case PAGER:
			switch (pagerMemoryType_[((getPager(addr>>12) << 12) |(addr &0xfff))/256])
			{
				case UNDEFINED:
					return 255;
				break;

				case ROM:
				case RAM:
					if (addr <32 && monitor_) return minimon[addr];
					return mainMemory_[(getPager(addr>>12) << 12) |(addr &0xfff)];
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

void Super::writeMem(Word addr, Byte value, bool writeRom)
{
	address_ = addr;
	superScreenPointer->showAddress(address_);

	if (emsMemoryDefined_)
	{
		if (addr>=0xc000 && addr <=0xffff)
		{
			emsPage_ = value & 0x1f;
		}
	}

	switch (memoryType_[addr/256])
	{
		case EMSMEMORY:
			switch (emsMemoryType_[((addr & 0x3fff) |(emsPage_ << 14))/256])
			{
				case UNDEFINED:
				case ROM:
					if (writeRom)
						emsRam_[(long) ((addr & 0x3fff) |(emsPage_ << 14))] = value;
				break;

				case RAM:
					emsRam_[(long) ((addr & 0x3fff) |(emsPage_ << 14))] = value;
					if (addr >= memoryStart_ && addr<(memoryStart_ + 256))
						p_Main->updateDebugMemory(addr);
					p_Main->updateAssTabCheck(addr);
				break;
			}
		break;

        case ROMMAPPER:
            if (emsPage_ <= maxNumberOfPages_)
            {
                switch (romMapperMemoryType_[((addr & 0x7fff) |(emsPage_ << 15))/256])
                {
                    case UNDEFINED:
                    case ROM:
                        if (writeRom)
                            expansionRom_[(long) ((addr & 0x7fff) |(emsPage_ << 15))] = value;
                    break;
                        
                    case RAM:
                        if (!getMpButtonState())
                        {
                            expansionRom_[(long) ((addr & 0x7fff) |(emsPage_ << 15))] = value;
                            if (addr >= memoryStart_ && addr<(memoryStart_ + 256))
                                p_Main->updateDebugMemory(addr);
							p_Main->updateAssTabCheck(addr);
						}
                    break;
                }
            }
        break;
            
		case MC6847RAM:
			mc6847Pointer->write(addr, value);
			mainMemory_[addr] = value;
			if (addr >= memoryStart_ && addr<(memoryStart_ + 256))
				p_Main->updateDebugMemory(addr);
		break;

		case MC6845RAM:
			mc6845Pointer->write6845(addr & 0x7ff, value);
		break;

		case MC6845REGISTERS:
			mc6845Pointer->writeRegister6845(addr, value);
		break;

		case UNDEFINED:
		case ROM:
			if (addr < 32 && monitor_) return;
			if (writeRom)
				mainMemory_[addr]=value;
		break;

		case MAPPEDRAM:
		case RAM:
			if (mainMemory_[addr]==value)
				return;
			if (addr < 32 && monitor_) return;
			addr = (addr & ramMask_) + ramStart_;
			mainMemory_[addr]=value;
			if (addr >= (memoryStart_& ramMask_) && addr<((memoryStart_& ramMask_) + 256))
				p_Main->updateDebugMemory(addr);
			p_Main->updateAssTabCheck(addr);
/*			if (loadedOs_ == ELFOS)
			{
				if (addr == 0x7cb1)
				{
					Word saveStart = (mainMemory_[0x7cae] << 8) + mainMemory_[0x7caf] ;
					Word saveEnd = saveStart + (mainMemory_[0x7cb0] << 8) + mainMemory_[0x7cb1];
					p_Main->eventSetLocation (true, saveStart, saveEnd, 0);
				}
			}*/
		break;

		case PAGER:
			switch (pagerMemoryType_[((getPager(addr>>12) << 12) |(addr &0xfff))/256])
			{
				case UNDEFINED:
				case ROM:
					if (addr < 32 && monitor_) return;
					if (writeRom)
						mainMemory_[(getPager(addr>>12) << 12) |(addr &0xfff)] = value;
				break;

				case RAM:
					if (addr < 32 && monitor_) return;
					mainMemory_[(getPager(addr>>12) << 12) |(addr &0xfff)] = value;
					if (addr >= memoryStart_ && addr<(memoryStart_ + 256))
						p_Main->updateDebugMemory(addr);
					p_Main->updateAssTabCheck(addr);
				break;
			}
		break;

	}
}

void Super::cpuInstruction()
{
	cycleA();
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
		checkElfFunction();
		if (resetPressed_)
		{
			resetCpu();
			if (elfConfiguration.use8275)
				i8275Pointer->cRegWrite(0x40);
			if (elfConfiguration.autoBoot)
			{
				scratchpadRegister_[0]=p_Main->getBootAddress("SuperElf", SUPERELF);
				autoBoot();
			}
			resetPressed_ = false;
			elfRunState_ = RESETSTATE;
			p_Main->setSwName("");
            p_Main->eventUpdateTitle();
			startElfKeyFile("SuperElf");
		}
		if (debugMode_)
			p_Main->cycleDebug();
		p_Main->cycleChip8Debug();
		state_ = 'F';
		cycleB();
	}
	else
	{
		machineCycle();
		machineCycle();
		cpuCycles_ = 0;
		p_Main->startTime();
		if (cpuMode_ == LOAD)
		{
			showData(readMem(address_));
		}
	}
}

void Super::configureElfExtensions()
{
	wxString fileName, fileName2;

	if (elfConfiguration.vtType != VTNONE)
	{
		double zoom = p_Main->getZoomVt();
        if (elfConfiguration.vtType == VT52)
            vtPointer = new Vt100("Super Elf - VT 52", p_Main->getVtPos(SUPERELF), wxSize(640*zoom, 400*zoom), zoom, SUPERELF, elfClockSpeed_, elfConfiguration);
        else
            vtPointer = new Vt100("Super Elf - VT 100", p_Main->getVtPos(SUPERELF), wxSize(640*zoom, 400*zoom), zoom, SUPERELF, elfClockSpeed_, elfConfiguration);
		p_Vt100 = vtPointer;
		vtPointer->configure(elfConfiguration.baudR, elfConfiguration.baudT, elfConfiguration.elfPortConf);
		vtPointer->Show(true);
		vtPointer->drawScreen();
	}

    if (elfConfiguration.vtExternal)
    {
        p_Serial = new Serial(SUPERELF, elfClockSpeed_, elfConfiguration);
        p_Serial->configure(elfConfiguration.baudR, elfConfiguration.baudT, elfConfiguration.elfPortConf);
    }

    if (elfConfiguration.usePixie)
	{
		double zoom = p_Main->getZoom();
		double scale = p_Main->getScale();
		pixiePointer = new Pixie( "Super Elf - Pixie", p_Main->getPixiePos(SUPERELF), wxSize(64*zoom*scale, 128*zoom), zoom, scale, SUPERELF);
		p_Video = pixiePointer;
		pixiePointer->configurePixie(elfConfiguration.elfPortConf);
		pixiePointer->initPixie();
		pixiePointer->setZoom(zoom);
		pixiePointer->Show(true);
	}

	if (elfConfiguration.use6845)
	{
		double zoom = p_Main->getZoom();
		mc6845Pointer = new MC6845( "Super Elf - MC6845", p_Main->get6845Pos(SUPERELF), wxSize(64*8*zoom, 16*8*2*zoom), zoom, SUPERELF, elfClockSpeed_, 8, elfConfiguration.elfPortConf);
		p_Video = mc6845Pointer;
		mc6845Pointer->configure6845(elfConfiguration.elfPortConf);
		mc6845Pointer->init6845();
		mc6845Pointer->Show(true);
	}

	if (elfConfiguration.useS100)
	{
		double zoom = p_Main->getZoom();
		mc6845Pointer = new MC6845( "Super Elf - Quest Super Video", p_Main->get6845Pos(SUPERELF), wxSize(64*7*zoom, 16*9*zoom), zoom, SUPERELF, elfClockSpeed_, 7, elfConfiguration.elfPortConf);
		p_Video = mc6845Pointer;
		mc6845Pointer->configureSuperVideo();
		mc6845Pointer->init6845();
		mc6845Pointer->Show(true);
	}

	if (elfConfiguration.use6847)
	{
		double zoom = p_Main->getZoom();
		mc6847Pointer = new mc6847( "Super Elf - MC6847", p_Main->get6847Pos(SUPERELF), wxSize(elfConfiguration.charLine*8*zoom, elfConfiguration.screenHeight6847*zoom), zoom, SUPERELF, elfClockSpeed_, elfConfiguration.elfPortConf);
		p_Video = mc6847Pointer;
		mc6847Pointer->configure(elfConfiguration.elfPortConf);
		mc6847Pointer->Show(true);
	}

	if (elfConfiguration.use8275)
	{
		double zoom = p_Main->getZoom();
		i8275Pointer = new i8275( "Super Elf - Intel 8275", p_Main->get8275Pos(SUPERELF), wxSize(80*8*zoom, 24*10*2*zoom), zoom, SUPERELF, elfClockSpeed_);
		p_Video = i8275Pointer;
		i8275Pointer->configure8275(elfConfiguration.elfPortConf);
		i8275Pointer->init8275();
		i8275Pointer->Show(true);
	}

	if (elfConfiguration.useTMS9918)
	{
		double zoom = p_Main->getZoom();
		tmsPointer = new Tms9918( "Super Elf - TMS 9918", p_Main->getTmsPos(SUPERELF), wxSize(320*zoom,240*zoom), zoom, SUPERELF, elfClockSpeed_);
		p_Video = tmsPointer;
		tmsPointer->configure(elfConfiguration.elfPortConf);
		tmsPointer->Show(true);
	}

	if (elfConfiguration.fdcEnabled)
	{
		configure1793(1, 40, 18, 256, SUPERELF, elfConfiguration.elfPortConf);
		resetFdc();
	}

	if (elfConfiguration.ideEnabled)
	{
		configureIde(p_Main->getIdeDir(SUPERELF) + p_Main->getIdeFile(SUPERELF), p_Main->getIdeDir(SUPERELF) + "disk2.ide", elfConfiguration.elfPortConf);
	}

	if (p_Main->getPrinterStatus(SUPERELF))
	{
		p_Printer = new Printer();
		p_Printer->configureElfPrinter(elfConfiguration.elfPortConf);
		p_Printer->initElf(p_Printer, "Quest Super Elf");
	}

	setQsound (elfConfiguration.qSound_);

	if (elfConfiguration.useKeyboard)
		configureKeyboard(SUPERELF, elfConfiguration.elfPortConf);

	if (elfConfiguration.UsePS2)
		configurePs2(elfConfiguration.ps2Interrupt, elfConfiguration.elfPortConf);
}

void Super::moveWindows()
{
	if (elfConfiguration.usePixie)
		pixiePointer->Move(p_Main->getPixiePos(SUPERELF));
	if (elfConfiguration.useTMS9918)
		tmsPointer->Move(p_Main->getTmsPos(SUPERELF));
	if (elfConfiguration.use6845||elfConfiguration.useS100)
		mc6845Pointer->Move(p_Main->get6845Pos(SUPERELF));
	if (elfConfiguration.use6847)
		mc6847Pointer->Move(p_Main->get6847Pos(SUPERELF));
	if (elfConfiguration.use8275)
		i8275Pointer->Move(p_Main->get8275Pos(SUPERELF));
	if (elfConfiguration.vtType != VTNONE)
		vtPointer->Move(p_Main->getVtPos(SUPERELF));
}

void Super::updateTitle(wxString Title)
{
	if (elfConfiguration.usePixie)
		pixiePointer->SetTitle("Super Elf - Pixie"+Title);
	if (elfConfiguration.useTMS9918)
		tmsPointer->SetTitle("Super Elf - TMS 9918"+Title);
	if (elfConfiguration.use6845)
		mc6845Pointer->SetTitle("Super Elf - MC6845"+Title);
	if (elfConfiguration.useS100)
		mc6845Pointer->SetTitle("Super Elf - Quest Super Video"+Title);
	if (elfConfiguration.use6847)
		mc6847Pointer->SetTitle("Super Elf - MC6847"+Title);
	if (elfConfiguration.use8275)
		i8275Pointer->SetTitle("Super Elf - Intel 8275"+Title);
    if (elfConfiguration.vtType == VT52)
		vtPointer->SetTitle("Super Elf - VT 52"+Title);
    if (elfConfiguration.vtType == VT100)
        vtPointer->SetTitle("Super Elf - VT 100"+Title);
}

void Super::setForceUpperCase(bool status)
{
	if (elfConfiguration.vtType != VTNONE)
		vtPointer->setForceUCVt(status);
	if (elfConfiguration.useKeyboard)
		setForceUpperCaseKeyboard(status);
}

void Super::onReset()
{
	resetPressed_ = true;
}

void Super::sleepComputer(long ms)
{
	threadPointer->Sleep(ms);
}

Byte Super::read8275CharRom(Word addr)
{
	if (elfConfiguration.use8275)
		return i8275Pointer->read8275CharRom(addr);
	else
		return 0;
}

void Super::write8275CharRom(Word addr, Byte value)
{
	if (elfConfiguration.use8275)
		i8275Pointer->write8275CharRom(addr, value);
}

Byte Super::read8275VideoRam(Word addr)
{
    if (elfConfiguration.use8275)
        return i8275Pointer->read8275VideoRam(addr);
    else
        return 0;
}

void Super::write8275VideoRam(Word addr, Byte value)
{
    if (elfConfiguration.use8275)
        i8275Pointer->write8275VideoRam(addr, value);
}

Byte Super::read6845CharRom(Word addr)
{
	if (elfConfiguration.use6845||elfConfiguration.useS100)
		return mc6845Pointer->read6845CharRom(addr);
	else
		return 0;
}

void Super::write6845CharRom(Word addr, Byte value)
{
	mc6845Pointer->write6845CharRom(addr, value);
}

Byte Super::read6847CharRom(Word addr)
{
	if (elfConfiguration.use6847)
		return mc6847Pointer->read6847CharRom(addr);
	else
		return 0;
}

void Super::write6847CharRom(Word addr, Byte value)
{
	if (elfConfiguration.use6847)
		mc6847Pointer->write6847CharRom(addr, value);
}

int Super::readDirect6847(Word addr)
{
	if (elfConfiguration.use6847)
		return mc6847Pointer->readDirect6847(addr); 
	else
		return 0;
}

void Super::writeDirect6847(Word addr, int value)
{
	if (elfConfiguration.use6847)
		mc6847Pointer->writeDirect6847(addr, value); 
}

Word Super::get6847RamMask()
{
 	if (elfConfiguration.use6847)
		return mc6847Pointer->get6847RamMask();
	else
		return 0;
}

void Super::setLedMs(long ms)
{
	superScreenPointer->setLedMs(ms);
    if (ms == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((elfClockSpeed_ * 1000000) / 8) / 1000) * ms;
    ledCycleValue_ = ledCycleSize_;
}

Byte Super::getKey(Byte vtOut)
{
	return superScreenPointer->getKey(vtOut);
}

void Super::activateMainWindow()
{
	bool maximize = IsMaximized();
	Iconize(false);
	Raise();
	Show(true);
	Maximize(maximize);
}

void Super::releaseButtonOnScreen(HexButton* buttonPointer, int WXUNUSED(buttonType))
{
	superScreenPointer->releaseButtonOnScreen(buttonPointer);
}
