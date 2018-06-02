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

#include "elf2.h"

#define OFF    1

Elf2Screen::Elf2Screen(wxWindow *parent, const wxSize& size, int tilType)
: Panel(parent, size)
{
    tilType_ = tilType;
}

Elf2Screen::~Elf2Screen()
{
#if defined (__WXMAC__)
    delete osx_push_inButtonPointer;
    for (int i=0;i<16;i++)
        delete osx_buttonPointer[i];
#else
    delete push_inButtonPointer;
    for (int i=0;i<16;i++)
        delete buttonPointer[i];
#endif
	delete mainBitmapPointer;
	delete mpSwitchButton;
	delete powerSwitchButton;
	delete loadSwitchButton;
	delete runSwitchButton;

	delete qLedPointer;

    if (tilType_ == TIL311)
    {
        for (int i=0; i<2; i++)
        {
            delete dataPointer[i];
        }
    }
    else
    {
        for (int i=0; i<2; i++)
        {
            delete dataTil313Pointer[i];
        }
    }
}

void Elf2Screen::init()
{
	keyStart_ = 0;
	keyEnd_ = 0;
	lastKey_ = 0;
	forceUpperCase_ = p_Main->getUpperCase(ELFII);

	wxClientDC dc(this);
	wxString buttonText;
	int x, y;

	mainBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/elf2.png", wxBITMAP_TYPE_PNG);
    
#if defined (__WXMAC__)
    osx_push_inButtonPointer = new HexButton(dc, ELF_HEX_BUTTON, 435, 327, "IN");
    for (int i=0; i<16; i++)
    {
        buttonText.Printf("%01X", i);
        x = 304 +(i&0x3)*32;
        y = 327 -(int)i/4*32;
        osx_buttonPointer[i] = new HexButton(dc, ELF_HEX_BUTTON, x, y, buttonText);
    }
#else
	push_inButtonPointer = new PushButton(this, 20, "IN", wxPoint(435, 327), wxSize(30, 30), 0);
    for (int i=0; i<16; i++)
    {
        buttonText.Printf("%01X", i);
        x = 304 +(i&0x3)*32;
        y = 327 -(int)i/4*32;
        buttonPointer[i] = new PushButton(this, i, buttonText, wxPoint(x, y), wxSize(30, 30), 0);
    }
#endif
	runSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(0x5a, 0x8a, 0xa5), BUTTON_DOWN, 440, 235, "");
	mpSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(0x5a, 0x8a, 0xa5), BUTTON_DOWN, 440, 295, "");
	powerSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(0x5a, 0x8a, 0xa5), BUTTON_UP, 490, 20, "");
	loadSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(0x5a, 0x8a, 0xa5), BUTTON_DOWN, 440, 265, "");

	qLedPointer = new Led(dc, 440, 190, ELFIILED);
	updateQLed_ = true;

	for (int i=0; i<2; i++)
	{
        if (tilType_ == TIL311)
        {
            dataPointer[i] = new Til311();
            dataPointer[i]->init(dc, 370+i*28,180);
            updateData_ = true;
        }
        else
        {
            dataTil313Pointer[i] = new Til313();
            dataTil313Pointer[i]->init(dc, 370+i*28,180);
            updateDataTil313_ = true;
        }
	}
	this->connectKeyEvent(this);
}

void Elf2Screen::onPaint(wxPaintEvent&WXUNUSED(event))
{
	wxPaintDC dc(this);
	dc.DrawBitmap(*mainBitmapPointer, 0, 0);

    if (tilType_ == TIL311)
    {
        for (int i=0; i<2; i++)
        {
            dataPointer[i]->onPaint(dc);
        }
    }
    else
    {
        for (int i=0; i<2; i++)
        {
            dataTil313Pointer[i]->onPaint(dc);
        }
    }
	qLedPointer->onPaint(dc);
	runSwitchButton->onPaint(dc);
	mpSwitchButton->onPaint(dc);
	powerSwitchButton->onPaint(dc);
	loadSwitchButton->onPaint(dc);
#if defined (__WXMAC__)
    osx_push_inButtonPointer->onPaint(dc);
    for (int i = 0; i<16; i++)
        osx_buttonPointer[i]->onPaint(dc);
#endif
}

void Elf2Screen::onMouseRelease(wxMouseEvent&event)
{
	int x, y;
	event.GetPosition(&x, &y);

	wxClientDC dc(this);

	if (runSwitchButton->onMouseRelease(dc, x, y))
		p_Computer->onRun();
	if (mpSwitchButton->onMouseRelease(dc, x, y))
		p_Computer->onMpButton();
	if (powerSwitchButton->onMouseRelease(dc, x, y))
		p_Main->stopComputer();
	if (loadSwitchButton->onMouseRelease(dc, x, y))
		p_Computer->onLoadButton();
#if defined (__WXMAC__)
    if (osx_push_inButtonPointer->onMouseRelease(dc, x, y))
        p_Computer->onInButtonRelease();
    for (int i = 0; i<16; i++)
        osx_buttonPointer[i]->onMouseRelease(dc, x, y);
#endif
}

void Elf2Screen::onMousePress(wxMouseEvent&event)
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

void Elf2Screen::releaseButtonOnScreen(HexButton* buttonPoint)
{
    wxClientDC dc(this);
    
    buttonPoint->releaseButtonOnScreen(dc);
}

BEGIN_EVENT_TABLE(Elf2, wxFrame)
	EVT_CLOSE (Elf2::onClose)
	EVT_COMMAND(0, wxEVT_ButtonDownEvent, Elf2::onNumberKeyDown)
	EVT_COMMAND(1, wxEVT_ButtonDownEvent, Elf2::onNumberKeyDown)
	EVT_COMMAND(2, wxEVT_ButtonDownEvent, Elf2::onNumberKeyDown)
	EVT_COMMAND(3, wxEVT_ButtonDownEvent, Elf2::onNumberKeyDown)
	EVT_COMMAND(4, wxEVT_ButtonDownEvent, Elf2::onNumberKeyDown)
	EVT_COMMAND(5, wxEVT_ButtonDownEvent, Elf2::onNumberKeyDown)
	EVT_COMMAND(6, wxEVT_ButtonDownEvent, Elf2::onNumberKeyDown)
	EVT_COMMAND(7, wxEVT_ButtonDownEvent, Elf2::onNumberKeyDown)
	EVT_COMMAND(8, wxEVT_ButtonDownEvent, Elf2::onNumberKeyDown)
	EVT_COMMAND(9, wxEVT_ButtonDownEvent, Elf2::onNumberKeyDown)
	EVT_COMMAND(10, wxEVT_ButtonDownEvent, Elf2::onNumberKeyDown)
	EVT_COMMAND(11, wxEVT_ButtonDownEvent, Elf2::onNumberKeyDown)
	EVT_COMMAND(12, wxEVT_ButtonDownEvent, Elf2::onNumberKeyDown)
	EVT_COMMAND(13, wxEVT_ButtonDownEvent, Elf2::onNumberKeyDown)
	EVT_COMMAND(14, wxEVT_ButtonDownEvent, Elf2::onNumberKeyDown)
	EVT_COMMAND(15, wxEVT_ButtonDownEvent, Elf2::onNumberKeyDown)
	EVT_COMMAND(0, wxEVT_ButtonUpEvent, Elf2::onNumberKeyUp)
	EVT_COMMAND(1, wxEVT_ButtonUpEvent, Elf2::onNumberKeyUp)
	EVT_COMMAND(2, wxEVT_ButtonUpEvent, Elf2::onNumberKeyUp)
	EVT_COMMAND(3, wxEVT_ButtonUpEvent, Elf2::onNumberKeyUp)
	EVT_COMMAND(4, wxEVT_ButtonUpEvent, Elf2::onNumberKeyUp)
	EVT_COMMAND(5, wxEVT_ButtonUpEvent, Elf2::onNumberKeyUp)
	EVT_COMMAND(6, wxEVT_ButtonUpEvent, Elf2::onNumberKeyUp)
	EVT_COMMAND(7, wxEVT_ButtonUpEvent, Elf2::onNumberKeyUp)
	EVT_COMMAND(8, wxEVT_ButtonUpEvent, Elf2::onNumberKeyUp)
	EVT_COMMAND(9, wxEVT_ButtonUpEvent, Elf2::onNumberKeyUp)
	EVT_COMMAND(10, wxEVT_ButtonUpEvent, Elf2::onNumberKeyUp)
	EVT_COMMAND(11, wxEVT_ButtonUpEvent, Elf2::onNumberKeyUp)
	EVT_COMMAND(12, wxEVT_ButtonUpEvent, Elf2::onNumberKeyUp)
	EVT_COMMAND(13, wxEVT_ButtonUpEvent, Elf2::onNumberKeyUp)
	EVT_COMMAND(14, wxEVT_ButtonUpEvent, Elf2::onNumberKeyUp)
	EVT_COMMAND(15, wxEVT_ButtonUpEvent, Elf2::onNumberKeyUp)
	EVT_COMMAND(20, wxEVT_ButtonDownEvent, Elf2::onButtonPress)
	EVT_COMMAND(20, wxEVT_ButtonUpEvent, Elf2::onButtonRelease)
END_EVENT_TABLE()

Elf2::Elf2(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
	elfConfiguration = conf;
	elfClockSpeed_ = clock;

#ifndef __WXMAC__
	SetIcon(wxICON(app_icon));
#endif
    
	this->SetClientSize(size);

	elf2ScreenPointer = new Elf2Screen(this, size, elfConfiguration.tilType);
	elf2ScreenPointer->init();

	offset_ = 0;

    runningGame_ = "";
}

Elf2::~Elf2()
{
	if (elfConfiguration.usePixie)
	{
		p_Main->setPixiePos(ELFII, pixiePointer->GetPosition());
		pixiePointer->Destroy();
	}
	if (elfConfiguration.useTMS9918)
	{
		p_Main->setTmsPos(ELFII, tmsPointer->GetPosition());
		tmsPointer->Destroy();
	}
	if (elfConfiguration.use6845||elfConfiguration.useS100)
	{
		p_Main->set6845Pos(ELFII, mc6845Pointer->GetPosition());
		mc6845Pointer->Destroy();
	}
	if (elfConfiguration.use6847)
	{
		p_Main->set6847Pos(ELFII, mc6847Pointer->GetPosition());
		mc6847Pointer->Destroy();
	}
	if (elfConfiguration.use8275)
	{
		p_Main->set8275Pos(ELFII, i8275Pointer->GetPosition());
		i8275Pointer->Destroy();
	}
	if (elfConfiguration.vtType != VTNONE)
	{
		p_Main->setVtPos(ELFII, vtPointer->GetPosition());
		vtPointer->Destroy();
	}
    if (elfConfiguration.vtExternal)
        delete p_Serial;
	if (p_Main->getPrinterStatus(ELFII))
	{
		p_Printer->closeFrames();
		delete p_Printer;
	}
	p_Main->setMainPos(ELFII, GetPosition());

	delete elf2ScreenPointer;
}

void Elf2::onClose(wxCloseEvent&WXUNUSED(event) )
{
	p_Main->stopComputer();
}

void Elf2::charEvent(int keycode)
{
	if (elfConfiguration.useKeyboard)
		charEventKeyboard(keycode);
}

bool Elf2::keyDownPressed(int key)
{
	onHexKeyDown(key);
//	if (elfConfiguration.useKeyboard)
//		keyboardUp();
	if (elfConfiguration.UsePS2)
	{
		keyDownPs2(key);
			return true;
	}
	return false;
}

bool Elf2::keyUpReleased(int key)
{

	if (key == inKey1_ || key == inKey2_)
	{
		onInButtonRelease();
		return true;
	}
	onHexKeyUp(key);
	if (elfConfiguration.UsePS2)
	{
		keyUpPs2(key);
		return true;
	}
	return false;
}

void Elf2::onButtonRelease(wxCommandEvent& event)
{
	onInButtonRelease();
	event.Skip();
}

void Elf2::onButtonPress(wxCommandEvent& event)
{
	onInButtonPress();
	event.Skip();
}

void Elf2::onInButtonPress()
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

void Elf2::onInButtonRelease()
{
	ef4State_ = 1;
}

void Elf2::configureComputer()
{
	efType_[1] = EF1UNDEFINED;
	efType_[2] = EF2UNDEFINED;
	efType_[3] = EF3UNDEFINED;

    setCycleType(COMPUTERCYCLE, LEDCYCLE);
//	int efPort;
	wxString printBuffer;

//	inType_[7] = RCADISKIN;
//	outType_[7] = RCADISKOUT;

//	int input = p_Main->getConfigItem("/ElfII/HexInput", 4l);
//	int output = p_Main->getConfigItem("/ElfII/HexOutput", 4l);

	p_Main->message("Configuring Elf II");
	printBuffer.Printf("	Output %d: display output, input %d: data input", elfConfiguration.elfPortConf.hexOutput, elfConfiguration.elfPortConf.hexInput);
	p_Main->message(printBuffer);

	p_Computer->setInType(elfConfiguration.elfPortConf.hexInput, ELF2IN);
	p_Computer->setOutType(elfConfiguration.elfPortConf.hexOutput, ELF2OUT);

    if (elfConfiguration.useRomMapper)
    {
//        output = p_Main->getConfigItem("/ElfII/EmsOutput", 7l);
        printBuffer.Printf("	Output %d: rom mapper", elfConfiguration.elfPortConf.emsOutput);
        p_Computer->setOutType(elfConfiguration.elfPortConf.emsOutput, ROMMAPPEROUT);
        p_Main->message(printBuffer);
    }
    if (elfConfiguration.useEms)
    {
//        output = p_Main->getConfigItem("/ElfII/EmsOutput", 7l);
        printBuffer.Printf("	Output %d: EMS-512KB", elfConfiguration.elfPortConf.emsOutput);
        p_Computer->setOutType(elfConfiguration.elfPortConf.emsOutput, EMSMAPPEROUT);
        p_Main->message(printBuffer);
    }
    if (elfConfiguration.useTape)
    {
//        efPort = p_Main->getConfigItem("/ElfII/TapeEf", 2l);
        efType_[elfConfiguration.elfPortConf.tapeEf] = ELF2EF2;
        printBuffer.Printf("	EF %d: cassette in", elfConfiguration.elfPortConf.tapeEf);
        p_Main->message(printBuffer);
    }
	if (elfConfiguration.useHexKeyboardEf3)
	{
//		efPort = p_Main->getConfigItem("/ElfII/HexEf", 3l);
		efType_[elfConfiguration.elfPortConf.hexEf] = ELF2EF3;
		printBuffer.Printf("	EF %d: 0 when hex button pressed", elfConfiguration.elfPortConf.hexEf);
		p_Main->message(printBuffer);
	}

	if (efType_[4] == 0)
	{
		efType_[4] = ELFINEF;
		p_Main->message("	EF 4: 0 when in button pressed");
	}
	p_Main->message("");

    inKey1_ = p_Main->getDefaultInKey1("ElfII");
    inKey2_ = p_Main->getDefaultInKey2("ElfII");
	p_Main->getDefaultHexKeys(ELFII, "ElfII", "A", keyDefA1_, keyDefA2_, keyDefGameHexA_);

	if (p_Main->getConfigBool("/ElfII/GameAuto", true))
		p_Main->loadKeyDefinition(p_Main->getRomFile(ELFII, MAINROM1), p_Main->getRomFile(ELFII, MAINROM2), keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
	
	resetCpu();
}

void Elf2::setPrinterEf()
{
    ef3State_ = 0;
}

void Elf2::reLoadKeyDefinition(wxString fileName)
{
    runningGame_ = fileName;
    if (p_Main->getConfigBool("/ElfII/GameAuto", true))
        p_Main->loadKeyDefinition("", fileName, keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
}

void Elf2::reDefineKeysA(int hexKeyDefA1[], int hexKeyDefA2[])
{
	for (int i=0; i<16; i++)
	{
        keyDefA1_[i] = hexKeyDefA1[i];
        keyDefA2_[i] = hexKeyDefA2[i];
	}
}

void Elf2::initComputer()
{
	Show(p_Main->getUseElfControlWindows(ELFII));

	cassetteEf_ = 0;
	runButtonState_ = 0;
	mpButtonState_ = 0;
	loadButtonState_ = 1;
	ef3State_ = 1;
	ef4State_ = 1;
	switches_ = 0;
	elfRunState_ = RESETSTATE;
}

Byte Elf2::ef(int flag)
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

		case ELFINEF:
			return ef4();
		break;

		case ELF2EF2:
			return cassetteEf_;
		break;

		case ELF2EF3:
		case ELFPRINTEREF:
			return ef3State_;
		break;

		case VTINEF:
			if (ef4State_ == 0)
				return 0;
			else
				return vtPointer->ef();
		break;

		case VTINEFSERIAL:
			if (ef4State_ == 0)
				return 0;
			else
				return p_Serial->ef();
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

		default:
			return 1;
	}
}

Byte Elf2::ef4()
{
	return ef4State_;
}

Byte Elf2::in(Byte port, Word WXUNUSED(address))
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

		case ELF2IN:
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

		case UARTINSERIAL:
			return p_Serial->uartIn();
		break;

		case UARTSTATUS:
			return vtPointer->uartStatus();
		break;

		case UARTSTATUSSERIAL:
			return p_Serial->uartStatus();
		break;

		default:
			ret = 255;
	}
	inValues_[port] = ret;
	return ret;
}

Byte Elf2::getData()
{
	return switches_;
}

void Elf2::out(Byte port, Word WXUNUSED(address), Byte value)
{
	outValues_[port] = value;

	switch(outType_[port])
	{
		case 0:
			return;
		break;

//		case RCADISKOUT:
//			psave(value);
//		break;

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

		case VTOUTSERIAL:
			p_Serial->out(value);
		break;

		case PRINTEROUT:
			//p_Main->eventPrintDefault(value);
			if ((value & 0xfc) != 0)
				p_Printer->printerOut(value);
		break;

		case PS2OUT:
			outPs2(value);
		break;

		case ELF2OUT:
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

		case UARTOUTSERIAL:
			 p_Serial->uartOut(value);
		break;

        case UARTCONTROL:
            vtPointer->uartControl(value);
        break;
            
        case UARTCONTROLSERIAL:
            p_Serial->uartControl(value);
        break;
            
        case ROMMAPPEROUT:
            setRomMapper(value);
        break;
            
        case EMSMAPPEROUT:
            setEmsPage(value);
        break;
    }
}

void Elf2::showData(Byte val)
{
    if (elfConfiguration.tilType == TIL311)
        elf2ScreenPointer->showData(val);
    else
        elf2ScreenPointer->showDataTil313(val);
}

void Elf2::cycle(int type)
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

void Elf2::cycleLed()
{
    if (ledCycleValue_ > 0)
    {
        ledCycleValue_ --;
        if (ledCycleValue_ <= 0)
        {
            ledCycleValue_ = ledCycleSize_;
            elf2ScreenPointer->ledTimeout();
        }
    }
}

void Elf2::autoBoot()
{
	elf2ScreenPointer->runSetState(BUTTON_UP);
	runButtonState_ = 1;
	setClear(runButtonState_);
}

void Elf2::switchQ(int value)
{
	elf2ScreenPointer->setQLed(value);

    if (elfConfiguration.vtType != VTNONE)
        vtPointer->switchQ(value);

    if (elfConfiguration.vtExternal)
        p_Serial->switchQ(value);
}

int Elf2::getMpButtonState()
{
	return mpButtonState_;
}

void Elf2::onRun()
{
	stopTape();
	if (runButtonState_)
	{
		elf2ScreenPointer->runSetState(BUTTON_DOWN);
		runButtonState_ = 0;
	}
	else
	{
		elf2ScreenPointer->runSetState(BUTTON_UP);
		runButtonState_ = 1;
		p_Main->startTime();
	}
	setClear(runButtonState_);
	p_Main->eventUpdateTitle();
}

void Elf2::onMpButton()
{
	if (mpButtonState_)
	{
		mpButtonState_ = 0;
	}
	else
	{
		mpButtonState_ = 1;
	}
}

void Elf2::onLoadButton()
{
	if (!loadButtonState_)
	{
		loadButtonState_ = 1;
	}
	else
	{
		loadButtonState_ = 0;
	}
	setWait(loadButtonState_);
}

void Elf2::onNumberKeyDown(wxCommandEvent&event)
{
	int id = event.GetId();
    onNumberKeyDown(id);
}

void Elf2::onNumberKeyDown(int id)
{
    ef3State_ = 0;
    switches_ = ((switches_ << 4) & 0xf0) | id;
}

void Elf2::onNumberKeyUp(wxCommandEvent&WXUNUSED(event))
{
	ef3State_ = 1;
}

void Elf2::onHexKeyDown(int keycode)
{
#if defined (__WXMAC__)
    if (ef3State_ == 0) // This is to avoid multiple key presses on OSX
        return;
#endif
    
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

void Elf2::onHexKeyUp(int keycode)
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

void Elf2::startComputer()
{
	startElfKeyFile("ElfII");
	resetPressed_ = false;

	if (elfConfiguration.usePortExtender)
		configurePortExt(elfConfiguration.elfPortConf);

	ramStart_ = p_Main->getStartRam("ElfII", ELFII);
	Word ramEnd = p_Main->getEndRam("ElfII", ELFII);
	ramMask_ = 0x100;
	while (ramMask_ < (ramEnd + 1 - ramStart_))
		ramMask_ *= 2;
	ramMask_ -= 1;

	defineMemoryType(ramStart_, ramEnd, RAM);
    initRam(ramStart_, ramEnd);

    p_Main->assDefault("mycode", ramStart_&0xff00, ((ramEnd&0xff00)|0xff)&0xfff);

    if (ramEnd == 0xff)
    {
        for (int i=ramMask_ + 1 + ramStart_; i<0x10000; i+=(ramMask_+1))
            defineMemoryType(i, i+(ramEnd - ramStart_), MAPPEDRAM);
    }

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
        readRomMapperBinFile(p_Main->getRomDir(ELFII, MAINROM1)+p_Main->getRomFile(ELFII, MAINROM1));
        defineMemoryType(0x8000, 0xffff, ROMMAPPER);
    }
    if (elfConfiguration.giantBoardMapping)
        defineMemoryType(0xF000, 0xFFFF, ROM);
   
	p_Main->enableDebugGuiMemory();

    Word offset = 0;
    wxString fileName = p_Main->getRomFile(ELFII, MAINROM1);
    if (fileName.Right(4) == ".ch8")
        offset = 0x200;
    if (!elfConfiguration.useRomMapper)
        readProgram(p_Main->getRomDir(ELFII, MAINROM1), p_Main->getRomFile(ELFII, MAINROM1), p_Main->getLoadromMode(ELFII, 0), offset, NONAME);

    offset = 0;
    fileName = p_Main->getRomFile(ELFII, MAINROM2);
    if (fileName.Right(4) == ".ch8")
        offset = 0x200;
    readProgram(p_Main->getRomDir(ELFII, MAINROM2), p_Main->getRomFile(ELFII, MAINROM2), p_Main->getLoadromMode(ELFII, 1), offset, NONAME);

	configureElfExtensions();
	if (elfConfiguration.autoBoot)
	{
		scratchpadRegister_[0]=p_Main->getBootAddress("ElfII", ELFII);
		autoBoot();
	}

	if (elfConfiguration.vtType != VTNONE)
		setEf(4,0);

	p_Main->setSwName("");
	p_Main->updateTitle();

	cpuCycles_ = 0;
	p_Main->startTime();

    int ms = (int) p_Main->getLedTimeMs(ELFII);
    elf2ScreenPointer->setLedMs(ms);
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

void Elf2::writeMemDataType(Word address, Byte type)
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
            if (elfConfiguration.giantBoardMapping)
                if (address >= baseGiantBoard_)
                    address = (address & 0xff) | 0xf000;
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

Byte Elf2::readMemDataType(Word address)
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
            if (elfConfiguration.giantBoardMapping)
                if (address >= baseGiantBoard_)
                    address = (address & 0xff) | 0xf000;
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

Byte Elf2::readMem(Word addr)
{
	address_ = addr;

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
            if (elfConfiguration.giantBoardMapping)
                if (addr >= baseGiantBoard_)
                    addr = (addr & 0xff) | 0xf000;
			return mainMemory_[addr];
		break;

		case MAPPEDRAM:
		case RAM:
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

void Elf2::writeMem(Word addr, Byte value, bool writeRom)
{
	address_ = addr;

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
					if (!getMpButtonState())
					{
						emsRam_[(long) ((addr & 0x3fff) |(emsPage_ << 14))] = value;
						if (addr >= memoryStart_ && addr<(memoryStart_ + 256))
							p_Main->updateDebugMemory(addr);
						p_Main->updateAssTabCheck(addr);
					}
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
            if (elfConfiguration.giantBoardMapping)
                if (addr >= baseGiantBoard_)
                    addr = (addr & 0xff) | 0xf000;
			if (writeRom)
				mainMemory_[addr]=value;
		break;

		case MAPPEDRAM:
		case RAM:
			if (!getMpButtonState())
			{
				addr = (addr & ramMask_) + ramStart_;
				if (mainMemory_[addr]==value)
					return;
				mainMemory_[addr]=value;
				if (addr >= (memoryStart_ & ramMask_) && addr<((memoryStart_ & ramMask_) + 256))
					p_Main->updateDebugMemory(addr);
				p_Main->updateAssTabCheck(addr);
			}
		break;

		case PAGER:
			switch (pagerMemoryType_[((getPager(addr>>12) << 12) |(addr &0xfff))/256])
			{
				case UNDEFINED:
				case ROM:
					if (writeRom)
						mainMemory_[(getPager(addr>>12) << 12) |(addr &0xfff)] = value;
				break;

				case RAM:
					if (!getMpButtonState())
						mainMemory_[(getPager(addr>>12) << 12) |(addr &0xfff)] = value;
					if (addr >= memoryStart_ && addr<(memoryStart_ + 256))
						p_Main->updateDebugMemory(addr);
					p_Main->updateAssTabCheck(addr);
				break;
			}
		break;

	}
}

void Elf2::cpuInstruction()
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
		checkElfFunction();
		if (resetPressed_)
		{
			resetCpu();
			if (elfConfiguration.use8275)
				i8275Pointer->cRegWrite(0x40);
			if (elfConfiguration.autoBoot)
			{
				scratchpadRegister_[0]=p_Main->getBootAddress("ElfII", ELFII);
				autoBoot();
			}
			resetPressed_ = false;
			elfRunState_ = RESETSTATE;
			p_Main->setSwName("");
            p_Main->eventUpdateTitle();
			startElfKeyFile("ElfII");
		}
		if (debugMode_)
			p_Main->cycleDebug();
		p_Main->cycleChip8Debug();
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

void Elf2::configureElfExtensions()
{
	wxString fileName, fileName2;

	if (elfConfiguration.vtType != VTNONE)
	{
		double zoom = p_Main->getZoomVt();
        if (elfConfiguration.vtType == VT52)
            vtPointer = new Vt100("Elf II - VT 52", p_Main->getVtPos(ELFII), wxSize(800*zoom, 500*zoom), zoom, ELFII, elfClockSpeed_, elfConfiguration);
        else
            vtPointer = new Vt100("Elf II - VT 100", p_Main->getVtPos(ELFII), wxSize(800*zoom, 500*zoom), zoom, ELFII, elfClockSpeed_, elfConfiguration);
		p_Vt100 = vtPointer;
		vtPointer->configure(elfConfiguration.baudR, elfConfiguration.baudT, elfConfiguration.elfPortConf);
		vtPointer->Show(true);
		vtPointer->drawScreen();
	}

    if (elfConfiguration.vtExternal)
    {
        p_Serial = new Serial(ELFII, elfClockSpeed_, elfConfiguration);
        p_Serial->configure(elfConfiguration.baudR, elfConfiguration.baudT, elfConfiguration.elfPortConf);
    }

	if (elfConfiguration.usePixie)
	{
		double zoom = p_Main->getZoom();
		double scale = p_Main->getScale();
		pixiePointer = new Pixie( "Elf II - Pixie", p_Main->getPixiePos(ELFII), wxSize(64*zoom*scale, 128*zoom), zoom, scale, ELFII);
		p_Video = pixiePointer;
		pixiePointer->configurePixie(elfConfiguration.elfPortConf);
		pixiePointer->initPixie();
		pixiePointer->setZoom(zoom);
		pixiePointer->Show(true);
	}

	if (elfConfiguration.use6845)
	{
		double zoom = p_Main->getZoom();
		mc6845Pointer = new MC6845( "Elf II - MC6845", p_Main->get6845Pos(ELFII), wxSize(64*8*zoom, 16*8*2*zoom), zoom, ELFII, elfClockSpeed_, 8, elfConfiguration.elfPortConf);
		p_Video = mc6845Pointer;
		mc6845Pointer->configure6845(elfConfiguration.elfPortConf);
		mc6845Pointer->init6845();
		mc6845Pointer->Show(true);
	}

	if (elfConfiguration.useS100)
	{
		double zoom = p_Main->getZoom();
		mc6845Pointer = new MC6845( "Elf II - Quest Super Video", p_Main->get6845Pos(ELFII), wxSize(64*7*zoom, 16*9*zoom), zoom, ELFII, elfClockSpeed_, 7, elfConfiguration.elfPortConf);
		p_Video = mc6845Pointer;
		mc6845Pointer->configureSuperVideo();
		mc6845Pointer->init6845();
		mc6845Pointer->Show(true);
	}

	if (elfConfiguration.use8275)
	{
		double zoom = p_Main->getZoom();
		i8275Pointer = new i8275( "Elf II - Intel 8275", p_Main->get8275Pos(ELFII), wxSize(80*8*zoom, 24*10*2*zoom), zoom, ELFII, elfClockSpeed_);
		p_Video = i8275Pointer;
		i8275Pointer->configure8275(elfConfiguration.elfPortConf);
		i8275Pointer->init8275();
		i8275Pointer->Show(true);
	}

	if (elfConfiguration.use6847)
	{
		double zoom = p_Main->getZoom();
		mc6847Pointer = new mc6847( "Elf II - MC6847", p_Main->get6847Pos(ELFII), wxSize(elfConfiguration.charLine*8*zoom, elfConfiguration.screenHeight6847*zoom), zoom, ELFII, elfClockSpeed_, elfConfiguration.elfPortConf);
		p_Video = mc6847Pointer;
		mc6847Pointer->configure(elfConfiguration.elfPortConf);
		mc6847Pointer->init6847();
		mc6847Pointer->Show(true);
	}

	if (elfConfiguration.useTMS9918)
	{
		double zoom = p_Main->getZoom();
		tmsPointer = new Tms9918( "Elf II - TMS 9918", p_Main->getTmsPos(ELFII), wxSize(320*zoom,240*zoom), zoom, ELFII, elfClockSpeed_);
		p_Video = tmsPointer;
		tmsPointer->configure(elfConfiguration.elfPortConf);
		tmsPointer->Show(true);
	}

	if (elfConfiguration.fdcEnabled)
	{
		configure1793(1, 40, 18, 256, ELFII, elfConfiguration.elfPortConf);
		resetFdc();
	}

	if (elfConfiguration.ideEnabled)
	{
		configureIde(p_Main->getIdeDir(ELFII) + p_Main->getIdeFile(ELFII), p_Main->getIdeDir(ELFII) + "disk2.ide", elfConfiguration.elfPortConf);
	}

	if (p_Main->getPrinterStatus(ELFII))
	{
		p_Printer = new Printer();
		p_Printer->configureElfPrinter(elfConfiguration.elfPortConf);
		p_Printer->initElf(p_Printer, "Netronics Elf II");
	}

	setQsound (elfConfiguration.qSound_);

	if (elfConfiguration.useKeyboard)
		configureKeyboard(ELFII, elfConfiguration.elfPortConf);

	if (elfConfiguration.UsePS2)
		configurePs2(elfConfiguration.ps2Interrupt, elfConfiguration.elfPortConf);
}

void Elf2::moveWindows()
{
	if (elfConfiguration.usePixie)
		pixiePointer->Move(p_Main->getPixiePos(ELFII));
	if (elfConfiguration.useTMS9918)
		tmsPointer->Move(p_Main->getTmsPos(ELFII));
	if (elfConfiguration.use6845||elfConfiguration.useS100)
		mc6845Pointer->Move(p_Main->get6845Pos(ELFII));
	if (elfConfiguration.use6847)
		mc6847Pointer->Move(p_Main->get6847Pos(ELFII));
	if (elfConfiguration.use8275)
		i8275Pointer->Move(p_Main->get8275Pos(ELFII));
	if (elfConfiguration.vtType != VTNONE)
		vtPointer->Move(p_Main->getVtPos(ELFII));
}

void Elf2::updateTitle(wxString Title)
{
	if (elfConfiguration.usePixie)
		pixiePointer->SetTitle("Elf II - Pixie"+Title);
	if (elfConfiguration.useTMS9918)
		tmsPointer->SetTitle("Elf II - TMS 9918"+Title);
	if (elfConfiguration.use6845)
		mc6845Pointer->SetTitle("Elf II - MC6845"+Title);
	if (elfConfiguration.useS100)
		mc6845Pointer->SetTitle("Elf II - Quest Super Video"+Title);
	if (elfConfiguration.use6847)
		mc6847Pointer->SetTitle("Elf II - MC6847"+Title);
	if (elfConfiguration.use8275)
		i8275Pointer->SetTitle("Elf II - Intel 8275"+Title);
    if (elfConfiguration.vtType == VT52)
        vtPointer->SetTitle("Elf II - VT 52"+Title);
    if (elfConfiguration.vtType == VT100)
        vtPointer->SetTitle("Elf II - VT 100"+Title);
}

void Elf2::setForceUpperCase(bool status)
{
	if (elfConfiguration.vtType != VTNONE)
		vtPointer->setForceUCVt(status);
	if (elfConfiguration.useKeyboard)
		setForceUpperCaseKeyboard(status);
}

void Elf2::onReset()
{
	resetPressed_ = true;
}

void Elf2::sleepComputer(long ms)
{
	threadPointer->Sleep(ms);
}

Byte Elf2::read6845CharRom(Word addr)
{
	if (elfConfiguration.use6845||elfConfiguration.useS100)
		return mc6845Pointer->read6845CharRom(addr);
	else
		return 0;
}

void Elf2::write6845CharRom(Word addr, Byte value)
{
	if (elfConfiguration.use6845||elfConfiguration.useS100)
		mc6845Pointer->write6845CharRom(addr, value);
}

Byte Elf2::read8275CharRom(Word addr)
{
    if (elfConfiguration.use8275)
        return i8275Pointer->read8275CharRom(addr);
    else
        return 0;
}

void Elf2::write8275CharRom(Word addr, Byte value)
{
    if (elfConfiguration.use8275)
        i8275Pointer->write8275CharRom(addr, value);
}

Byte Elf2::read8275VideoRam(Word addr)
{
    if (elfConfiguration.use8275)
        return i8275Pointer->read8275VideoRam(addr);
    else
        return 0;
}

void Elf2::write8275VideoRam(Word addr, Byte value)
{
    if (elfConfiguration.use8275)
        i8275Pointer->write8275VideoRam(addr, value);
}

Byte Elf2::read6847CharRom(Word addr)
{
	if (elfConfiguration.use6847)
		return mc6847Pointer->read6847CharRom(addr);
	else
		return 0;
}

void Elf2::write6847CharRom(Word addr, Byte value)
{
	if (elfConfiguration.use6847)
		mc6847Pointer->write6847CharRom(addr, value);
}

int Elf2::readDirect6847(Word addr)
{
	if (elfConfiguration.use6847)
		return mc6847Pointer->readDirect6847(addr); 
	else
		return 0;
}

void Elf2::writeDirect6847(Word addr, int value)
{
	if (elfConfiguration.use6847)
		mc6847Pointer->writeDirect6847(addr, value); 
}

Word Elf2::get6847RamMask()
{
 	if (elfConfiguration.use6847)
		return mc6847Pointer->get6847RamMask();
	else
		return 0;
}

void Elf2::setLedMs(long ms)
{
	elf2ScreenPointer->setLedMs(ms);
    if (ms == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((elfClockSpeed_ * 1000000) / 8) / 1000) * ms;
    ledCycleValue_ = ledCycleSize_;
}

Byte Elf2::getKey(Byte vtOut)
{
	return elf2ScreenPointer->getKey(vtOut);
}

void Elf2::activateMainWindow()
{
	bool maximize = IsMaximized();
	Iconize(false);
	Raise();
	Show(true);
	Maximize(maximize);
}

void Elf2::releaseButtonOnScreen(HexButton* buttonPointer, int WXUNUSED(buttonType))
{
    elf2ScreenPointer->releaseButtonOnScreen(buttonPointer);
}
