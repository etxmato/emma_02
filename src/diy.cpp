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

#include "diy.h"

#define OFF    1

DiyScreen::DiyScreen(wxWindow *parent, const wxSize& size, int tilType)
: Panel(parent, size)
{
    tilType_ = tilType;
}

DiyScreen::~DiyScreen()
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
            delete dataTil313PointerItalic[i];
        }
    }
}

void DiyScreen::init()
{
	keyStart_ = 0;
	keyEnd_ = 0;
	lastKey_ = 0;
	forceUpperCase_ = p_Main->getUpperCase(DIY);

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
            dataTil313PointerItalic[i] = new Til313Italic(false);
            dataTil313PointerItalic[i]->init(dc, 370+i*28,180);
            updateDataTil313Italic_ = true;
        }
	}
	this->connectKeyEvent(this);
}

void DiyScreen::onPaint(wxPaintEvent&WXUNUSED(event))
{
	wxPaintDC dc(this);
	dc.DrawBitmap(*mainBitmapPointer, 0, 0);

#if defined(__WXMAC__)
    rePaintLeds(dc);
#endif

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
            dataTil313PointerItalic[i]->onPaint(dc);
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

void DiyScreen::onMouseRelease(wxMouseEvent&event)
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
        if (osx_buttonPointer[i]->onMouseRelease(dc, x, y))
            p_Computer->onNumberKeyUp();
#endif
}

void DiyScreen::onMousePress(wxMouseEvent&event)
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

void DiyScreen::releaseButtonOnScreen(HexButton* buttonPoint)
{
    wxClientDC dc(this);
    
    buttonPoint->releaseButtonOnScreen(dc);
}

BEGIN_EVENT_TABLE(Diy, wxFrame)
	EVT_CLOSE (Diy::onClose)
	EVT_COMMAND(0, wxEVT_ButtonDownEvent, Diy::onNumberKeyDown)
	EVT_COMMAND(1, wxEVT_ButtonDownEvent, Diy::onNumberKeyDown)
	EVT_COMMAND(2, wxEVT_ButtonDownEvent, Diy::onNumberKeyDown)
	EVT_COMMAND(3, wxEVT_ButtonDownEvent, Diy::onNumberKeyDown)
	EVT_COMMAND(4, wxEVT_ButtonDownEvent, Diy::onNumberKeyDown)
	EVT_COMMAND(5, wxEVT_ButtonDownEvent, Diy::onNumberKeyDown)
	EVT_COMMAND(6, wxEVT_ButtonDownEvent, Diy::onNumberKeyDown)
	EVT_COMMAND(7, wxEVT_ButtonDownEvent, Diy::onNumberKeyDown)
	EVT_COMMAND(8, wxEVT_ButtonDownEvent, Diy::onNumberKeyDown)
	EVT_COMMAND(9, wxEVT_ButtonDownEvent, Diy::onNumberKeyDown)
	EVT_COMMAND(10, wxEVT_ButtonDownEvent, Diy::onNumberKeyDown)
	EVT_COMMAND(11, wxEVT_ButtonDownEvent, Diy::onNumberKeyDown)
	EVT_COMMAND(12, wxEVT_ButtonDownEvent, Diy::onNumberKeyDown)
	EVT_COMMAND(13, wxEVT_ButtonDownEvent, Diy::onNumberKeyDown)
	EVT_COMMAND(14, wxEVT_ButtonDownEvent, Diy::onNumberKeyDown)
	EVT_COMMAND(15, wxEVT_ButtonDownEvent, Diy::onNumberKeyDown)
	EVT_COMMAND(0, wxEVT_ButtonUpEvent, Diy::onNumberKeyUp)
	EVT_COMMAND(1, wxEVT_ButtonUpEvent, Diy::onNumberKeyUp)
	EVT_COMMAND(2, wxEVT_ButtonUpEvent, Diy::onNumberKeyUp)
	EVT_COMMAND(3, wxEVT_ButtonUpEvent, Diy::onNumberKeyUp)
	EVT_COMMAND(4, wxEVT_ButtonUpEvent, Diy::onNumberKeyUp)
	EVT_COMMAND(5, wxEVT_ButtonUpEvent, Diy::onNumberKeyUp)
	EVT_COMMAND(6, wxEVT_ButtonUpEvent, Diy::onNumberKeyUp)
	EVT_COMMAND(7, wxEVT_ButtonUpEvent, Diy::onNumberKeyUp)
	EVT_COMMAND(8, wxEVT_ButtonUpEvent, Diy::onNumberKeyUp)
	EVT_COMMAND(9, wxEVT_ButtonUpEvent, Diy::onNumberKeyUp)
	EVT_COMMAND(10, wxEVT_ButtonUpEvent, Diy::onNumberKeyUp)
	EVT_COMMAND(11, wxEVT_ButtonUpEvent, Diy::onNumberKeyUp)
	EVT_COMMAND(12, wxEVT_ButtonUpEvent, Diy::onNumberKeyUp)
	EVT_COMMAND(13, wxEVT_ButtonUpEvent, Diy::onNumberKeyUp)
	EVT_COMMAND(14, wxEVT_ButtonUpEvent, Diy::onNumberKeyUp)
	EVT_COMMAND(15, wxEVT_ButtonUpEvent, Diy::onNumberKeyUp)
	EVT_COMMAND(20, wxEVT_ButtonDownEvent, Diy::onButtonPress)
	EVT_COMMAND(20, wxEVT_ButtonUpEvent, Diy::onButtonRelease)
    EVT_TIMER(900, Elf::OnRtcTimer)
END_EVENT_TABLE()

Diy::Diy(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf, Conf computerConf)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    computerConfiguration = computerConf;
	elfConfiguration = conf;
	elfClockSpeed_ = clock;

#ifndef __WXMAC__
	SetIcon(wxICON(app_icon));
#endif
    
	this->SetClientSize(size);

	elf2ScreenPointer = new DiyScreen(this, size, elfConfiguration.tilType);
	elf2ScreenPointer->init();

	offset_ = 0;

    rtcTimerPointer = new wxTimer(this, 900);
    cycleValue_ = -1;
    cycleSize_ = -1;

    runningGame_ = "";
}

Diy::~Diy()
{
    saveRtc();
    rtcTimerPointer->Stop();
    delete rtcTimerPointer;
	if (elfConfiguration.usePixie)
	{
		p_Main->setPixiePos(DIY, pixiePointer->GetPosition());
		pixiePointer->Destroy();
	}
	if (elfConfiguration.useTMS9918)
	{
		p_Main->setTmsPos(DIY, tmsPointer->GetPosition());
		tmsPointer->Destroy();
	}
	if (elfConfiguration.use6845||elfConfiguration.useS100)
	{
		p_Main->set6845Pos(DIY, mc6845Pointer->GetPosition());
		mc6845Pointer->Destroy();
	}
	if (elfConfiguration.use6847)
	{
		p_Main->set6847Pos(DIY, mc6847Pointer->GetPosition());
		mc6847Pointer->Destroy();
	}
	if (elfConfiguration.use8275)
	{
		p_Main->set8275Pos(DIY, i8275Pointer->GetPosition());
		i8275Pointer->Destroy();
	}
	if (elfConfiguration.vtType != VTNONE)
	{
		p_Main->setVtPos(DIY, vtPointer->GetPosition());
		vtPointer->Destroy();
	}
    if (elfConfiguration.vtExternal)
        delete p_Serial;
	if (p_Main->getPrinterStatus(DIY))
	{
		p_Printer->closeFrames();
		delete p_Printer;
	}
	p_Main->setMainPos(DIY, GetPosition());

	delete elf2ScreenPointer;
}

void Diy::onClose(wxCloseEvent&WXUNUSED(event) )
{
	p_Main->stopComputer();
}

void Diy::charEvent(int keycode)
{
	if (elfConfiguration.useKeyboard)
		charEventKeyboard(keycode);
}

bool Diy::keyDownPressed(int key)
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

bool Diy::keyUpReleased(int key)
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

void Diy::onButtonRelease(wxCommandEvent& event)
{
	onInButtonRelease();
	event.Skip();
}

void Diy::onButtonPress(wxCommandEvent& event)
{
	onInButtonPress();
	event.Skip();
}

void Diy::onInButtonPress()
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

void Diy::onInButtonRelease()
{
	ef4State_ = 1;
}

void Diy::configureComputer()
{
	efType_[1] = EF1UNDEFINED;
	efType_[2] = EF2UNDEFINED;
	efType_[3] = EF3UNDEFINED;

    setCycleType(COMPUTERCYCLE, LEDCYCLE);
	wxString printBuffer;

    p_Main->message("Configuring " + p_Main->getRunningComputerText());
	printBuffer.Printf("	Output %d: display output, input %d: data input", elfConfiguration.elfPortConf.hexOutput, elfConfiguration.elfPortConf.hexInput);
	p_Main->message(printBuffer);

	p_Computer->setInType(elfConfiguration.elfPortConf.hexInput, ELF2IN);
	p_Computer->setOutType(elfConfiguration.elfPortConf.hexOutput, ELF2OUT);

    if (elfConfiguration.useEms)
    {
        elfConfiguration.elfPortConf.emsOutput.resize(computerConfiguration.emsConfigNumber_);
        for (size_t emsNumber=0; emsNumber<computerConfiguration.emsConfigNumber_; emsNumber++)
        {
            if (elfConfiguration.elfPortConf.emsOutput[emsNumber] == -1)
            {
                printBuffer.Printf("    Address %04X-%04X: EMS-512KB page select", computerConfiguration.emsConfig_[emsNumber].outputStart, computerConfiguration.emsConfig_[emsNumber].outputEnd);
            }
            else
            {
                printBuffer.Printf("	Output %d: EMS-512KB page select", elfConfiguration.elfPortConf.emsOutput[emsNumber]);
                p_Computer->setOutType(elfConfiguration.elfPortConf.emsOutput[emsNumber], EMSMAPPEROUT);
            }
            p_Main->message(printBuffer);
        }
    }
    if (elfConfiguration.useTape && !elfConfiguration.useXmodem)
    {
        efType_[elfConfiguration.elfPortConf.tapeEf] = ELF2EF2;
        printBuffer.Printf("	EF %d: cassette in", elfConfiguration.elfPortConf.tapeEf);
        p_Main->message(printBuffer);
    }
	if (elfConfiguration.useHexKeyboardEf3)
	{
		printBuffer.Printf("	EF %d: 0 when hex button pressed", elfConfiguration.elfPortConf.hexEf);
		p_Main->message(printBuffer);
	}

    if (elfConfiguration.efButtons)
    {
        p_Main->message("	EF 3: 0 when right EF button pressed");
        p_Main->message("	EF 4: 0 when left EF button pressed");
    }
    p_Main->message("	EF 4: 0 when in button pressed");

	p_Main->message("");

    inKey1_ = p_Main->getDefaultInKey1("Diy");
    inKey2_ = p_Main->getDefaultInKey2("Diy");
	p_Main->getDefaultHexKeys(DIY, "Diy", "A", keyDefA1_, keyDefA2_, keyDefGameHexA_);

	if (p_Main->getConfigBool("/ElfII/GameAuto", true))
		p_Main->loadKeyDefinition(p_Main->getRomFile(DIY, MAINROM1), p_Main->getRomFile(DIY, MAINROM2), keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
	
	resetCpu();
}

void Diy::switchHexEf(bool state)
{
    elfConfiguration.useHexKeyboardEf3 = state;
}

void Diy::setPrinterEf()
{
    ef3State_ = 0;
}

void Diy::reLoadKeyDefinition(wxString fileName)
{
    runningGame_ = fileName;
    if (p_Main->getConfigBool("/ElfII/GameAuto", true))
        p_Main->loadKeyDefinition("", fileName, keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
}

void Diy::reDefineKeysA(int hexKeyDefA1[], int hexKeyDefA2[])
{
	for (int i=0; i<16; i++)
	{
        keyDefA1_[i] = hexKeyDefA1[i];
        keyDefA2_[i] = hexKeyDefA2[i];
	}
}

void Diy::initComputer()
{
	Show(p_Main->getUseElfControlWindows(DIY));

	cassetteEf_ = 0;
	runButtonState_ = 0;
	mpButtonState_ = 0;
	loadButtonState_ = 1;
	ef3State_ = 1;
	ef4State_ = 1;
    ef3Button_ = 1;
    ef4Button_ = 1;
	switches_ = 0;
	elfRunState_ = RESETSTATE;
    bootstrap_ = 0;
}

Byte Diy::ef(int flag)
{
    switch (flag)
    {
        case 3:
            if (ef3Button_ == 0 && elfConfiguration.efButtons)
                return ef3Button_;
        break;

        case 4:
            if (ef4Button_ == 0 && elfConfiguration.efButtons)
                return ef4Button_;
            if (ef4State_ == 0)
                return ef4State_;
        break;
    }
    if (elfConfiguration.useHexKeyboardEf3)
    {
        if (flag == elfConfiguration.elfPortConf.hexEf)
            return ef3State_;
    }
	switch(efType_[flag])
	{
		case 0:
			return 1;
		break;

        case TMSINTERRUPT:
            return tmsPointer->readEf();
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

		case ELFPRINTEREF:
            return ef3State_;
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

Byte Diy::in(Byte port, Word WXUNUSED(address))
{
	Byte ret;

	switch(inType_[port])
	{
		case 0:
			ret = 255;
		break;

        case TMSDATAPORT:
            ret = tmsPointer->readVRAM();
        break;

        case TMSREGISTERPORT:
            ret = tmsPointer->readDataPort();
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

        case ELF2KDISKREADREGISTER:
            return inDisk();
        break;

        case ELF2KDISKREADSTATUS:
            return readDiskStatus();
        break;

		default:
			ret = 255;
	}
	inValues_[port] = ret;
	return ret;
}

Byte Diy::getData()
{
	return switches_;
}

void Diy::out(Byte port, Word WXUNUSED(address), Byte value)
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

		case TMSREGISTERPORT:
			tmsPointer->modeHighOut(value);
		break;

		case TMSDATAPORT:
			tmsPointer->writeVRAM(value);
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
            
        case EMSMAPPEROUT:
            for (size_t emsNumber=0; emsNumber<computerConfiguration.emsConfigNumber_; emsNumber++)
            {
                if (computerConfiguration.emsConfig_[emsNumber].outputEnd == port)
                    setEmsPage(emsNumber, value);
            }
        break;

        case ELF2KDISKSELECTREGISTER:
            selectDiskRegister(value);
        break;

        case ELF2KDISKWRITEREGISTER:
            outDisk(value);
        break;
    }
}

void Diy::showData(Byte val)
{
    if (elfConfiguration.tilType == TIL311)
        elf2ScreenPointer->showData(val);
    else
        elf2ScreenPointer->showDataTil313Italic(val);
}

void Diy::cycle(int type)
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

void Diy::cycleLed()
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
            elf2ScreenPointer->ledTimeout();
        }
    }
}

void Diy::autoBoot()
{
	elf2ScreenPointer->runSetState(BUTTON_UP);
	runButtonState_ = 1;
	setClear(runButtonState_);
}

void Diy::switchQ(int value)
{
	elf2ScreenPointer->setQLed(value);

    if (elfConfiguration.vtType != VTNONE)
        vtPointer->switchQ(value);

    if (elfConfiguration.vtExternal)
        p_Serial->switchQ(value);
}

int Diy::getMpButtonState()
{
	return mpButtonState_;
}

void Diy::onRun()
{
    if (elfConfiguration.bootStrap)
        bootstrap_ = 0x8000;

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

void Diy::onMpButton()
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

void Diy::onLoadButton()
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

void Diy::onNumberKeyDown(wxCommandEvent&event)
{
	int id = event.GetId();
    onNumberKeyDown(id);
}

void Diy::onNumberKeyDown(int id)
{
    ef3State_ = 0;
    switches_ = ((switches_ << 4) & 0xf0) | id;
}

void Diy::onNumberKeyUp(wxCommandEvent&WXUNUSED(event))
{
	ef3State_ = 1;
}

void Diy::onNumberKeyUp()
{
    ef3State_ = 1;
}

void Diy::onHexKeyDown(int keycode)
{
    if (elfConfiguration.efButtons)
    {
        if (keycode == keyDefA2_[keyDefGameHexA_[2]])
        {
            ef3Button_ = 0;
            return;
        }
        if (keycode == keyDefA2_[keyDefGameHexA_[1]])
        {
            ef4Button_ = 0;
            return;
        }
    }
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

void Diy::onHexKeyUp(int keycode)
{
    if (elfConfiguration.efButtons)
    {
        if (keycode == keyDefA2_[keyDefGameHexA_[2]])
        {
            ef3Button_ = 1;
            return;
        }
        if (keycode == keyDefA2_[keyDefGameHexA_[1]])
        {
            ef4Button_ = 1;
            return;
        }
    }
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

void Diy::startComputer()
{
    configureMemory();
    
    resetPressed_ = false;
   
	p_Main->enableDebugGuiMemory();

	configureElfExtensions();
    startElfKeyFile("Diy");
	if (elfConfiguration.autoBoot)
	{
		scratchpadRegister_[0]=p_Main->getBootAddress("Diy", DIY);
		autoBoot();
	}

	if (elfConfiguration.vtType != VTNONE)
		setEf(4,0);

	p_Main->setSwName("");
	p_Main->updateTitle();

	cpuCycles_ = 0;
	instructionCounter_= 0;
	p_Main->startTime();

    loadRtc();
    rtcCycle_ = 4;
    rtcTimerPointer->Start(250, wxTIMER_CONTINUOUS);

    int ms = (int) p_Main->getLedTimeMs(DIY);
    elf2ScreenPointer->setLedMs(ms);
    if (ms == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((elfClockSpeed_ * 1000000) / 8) / 1000) * ms;
    ledCycleValue_ = ledCycleSize_;
    
    if (p_Vt100[UART1] != NULL)
        p_Vt100[UART1]->splashScreen();
    else
        p_Video->splashScreen();

    if (elfConfiguration.bootStrap)
        bootstrap_ = 0x8000;

    threadPointer->Run();
}

void Diy::loadRomRam(size_t configNumber)
{
    if (computerConfiguration.memConfig_[configNumber].filename != "")
    {
        if (computerConfiguration.memConfig_[configNumber].verifyFileExist)
            p_Main->checkAndReInstallFile(computerConfiguration.memConfig_[configNumber].dirname + computerConfiguration.memConfig_[configNumber].filename, DIY, computerConfiguration.memConfig_[configNumber].filename);

        readProgram(computerConfiguration.memConfig_[configNumber].dirname, computerConfiguration.memConfig_[configNumber].filename, computerConfiguration.memConfig_[configNumber].type, computerConfiguration.memConfig_[configNumber].start, NONAME);
    }
}

void Diy::writeMemDataType(Word address, Byte type)
{
    size_t emsNumber;
    address = address | bootstrap_;
	switch (memoryType_[address/256]&0xff)
	{
        case EMSMEMORY:
            emsNumber = (memoryType_[address / 256] >> 8);
            
            switch (emsMemory_[emsNumber].memoryType_[((address - computerConfiguration.emsConfig_[emsNumber].start) |(computerConfiguration.emsConfig_[emsNumber].page << computerConfiguration.emsConfig_[emsNumber].maskBits))/256])
            {
                case ROM:
                case RAM:
                    if (emsMemory_[emsNumber].dataType_[(long) ((address - computerConfiguration.emsConfig_[emsNumber].start) |(computerConfiguration.emsConfig_[emsNumber].page << computerConfiguration.emsConfig_[emsNumber].maskBits))] != type)
                    {
                        p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                        emsMemory_[emsNumber].dataType_[(long) ((address - computerConfiguration.emsConfig_[emsNumber].start) |(computerConfiguration.emsConfig_[emsNumber].page << computerConfiguration.emsConfig_[emsNumber].maskBits))] = type;
                    }
                    increaseExecutedEms(emsNumber, (long) ((address - computerConfiguration.emsConfig_[emsNumber].start) |(computerConfiguration.emsConfig_[emsNumber].page << computerConfiguration.emsConfig_[emsNumber].maskBits)), type);
                break;
            }
        break;
            
        case MAPPEDROM:
		case ROM:
            emsNumber = (memoryType_[address / 256] >> 8);
            address = (address & computerConfiguration.memConfig_[emsNumber].memMask) | computerConfiguration.memConfig_[emsNumber].start;

            if (mainMemoryDataType_[address] != type)
			{
				p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
				mainMemoryDataType_[address] = type;
			}
            increaseExecutedMainMemory(address, type);
		break;

        case MAINRAM:
		case MAPPEDRAM:
			address = (address & computerConfiguration.memConfig_[0].memMask) | computerConfiguration.memConfig_[0].start;
			if (mainMemoryDataType_[address] != type)
			{
				p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
				mainMemoryDataType_[address] = type;
			}
            increaseExecutedMainMemory(address, type);
		break;

        case RAM:
            if (mainMemoryDataType_[address] != type)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryDataType_[address] = type;
            }
            increaseExecutedMainMemory(address, type);
        break;

		case PAGER:
			switch (pagerMemoryType_[((getPager(address>>computerConfiguration.pagerMaskBits_) << computerConfiguration.pagerMaskBits_) |(address &computerConfiguration.pagerMask_))/256])
			{
				case ROM:
				case RAM:
					if (mainMemoryDataType_[(getPager(address>>computerConfiguration.pagerMaskBits_) << computerConfiguration.pagerMaskBits_) |(address &computerConfiguration.pagerMask_)] != type)
					{
						p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
						mainMemoryDataType_[(getPager(address>>computerConfiguration.pagerMaskBits_) << computerConfiguration.pagerMaskBits_) |(address &computerConfiguration.pagerMask_)] = type;
					}
                    increaseExecutedMainMemory((getPager(address>>computerConfiguration.pagerMaskBits_) << computerConfiguration.pagerMaskBits_) |(address &computerConfiguration.pagerMask_), type);
				break;
			}
		break;

	}
}

Byte Diy::readMemDataType(Word address, uint64_t* executed)
{
    size_t emsNumber;
    address = address | bootstrap_;
	switch (memoryType_[address/256]&0xff)
	{
        case EMSMEMORY:
            emsNumber = (memoryType_[address / 256] >> 8);
            
            switch (emsMemory_[emsNumber].memoryType_[((address - computerConfiguration.emsConfig_[emsNumber].start) |(computerConfiguration.emsConfig_[emsNumber].page << computerConfiguration.emsConfig_[emsNumber].maskBits))/256])
            {
                case ROM:
                case RAM:
                    if (profilerCounter_ != PROFILER_OFF)
                        *executed = emsMemory_[emsNumber].executed_[(long) ((address - computerConfiguration.emsConfig_[emsNumber].start) |(computerConfiguration.emsConfig_[emsNumber].page << computerConfiguration.emsConfig_[emsNumber].maskBits))];
                    return emsMemory_[emsNumber].dataType_[(long) ((address - computerConfiguration.emsConfig_[emsNumber].start) |(computerConfiguration.emsConfig_[emsNumber].page << computerConfiguration.emsConfig_[emsNumber].maskBits))];
                break;
            }
        break;

        case MAPPEDROM:
		case ROM:
            emsNumber = (memoryType_[address / 256] >> 8);
            address = (address & computerConfiguration.memConfig_[emsNumber].memMask) | computerConfiguration.memConfig_[emsNumber].start;

            if (profilerCounter_ != PROFILER_OFF)
                *executed = mainMemoryExecuted_[address];
			return mainMemoryDataType_[address];
		break;

        case MAINRAM:
		case MAPPEDRAM:
			address = (address & computerConfiguration.memConfig_[0].memMask) | computerConfiguration.memConfig_[0].start;
            if (profilerCounter_ != PROFILER_OFF)
                *executed = mainMemoryExecuted_[address];
			return mainMemoryDataType_[address];
		break;

        case RAM:
            if (profilerCounter_ != PROFILER_OFF)
                *executed = mainMemoryExecuted_[address];
            return mainMemoryDataType_[address];
        break;

		case PAGER:
			switch (pagerMemoryType_[((getPager(address>>computerConfiguration.pagerMaskBits_) << computerConfiguration.pagerMaskBits_) |(address &computerConfiguration.pagerMask_))/256])
			{
				case ROM:
				case RAM:
                    if (profilerCounter_ != PROFILER_OFF)
                        *executed = mainMemoryExecuted_[(getPager(address>>computerConfiguration.pagerMaskBits_) << computerConfiguration.pagerMaskBits_) |(address &computerConfiguration.pagerMask_)];
					return mainMemoryDataType_[(getPager(address>>computerConfiguration.pagerMaskBits_) << computerConfiguration.pagerMaskBits_) |(address &computerConfiguration.pagerMask_)];
				break;
			}
		break;
	}
	return MEM_TYPE_UNDEFINED;
}

Byte Diy::readMem(Word address)
{
	address_ = address;
    return readMemDebug(address_);
}

Byte Diy::readMemDebug(Word address)
{
    size_t emsNumber;

    if ((address & 0x8000) == 0x8000)
        bootstrap_ = 0;

    address = address | bootstrap_;

    switch (memoryType_[address/256]&0xff)
	{
        case EMSMEMORY:
            emsNumber = (memoryType_[address / 256] >> 8);
            
            switch (emsMemory_[emsNumber].memoryType_[((address - computerConfiguration.emsConfig_[emsNumber].start) |(computerConfiguration.emsConfig_[emsNumber].page << computerConfiguration.emsConfig_[emsNumber].maskBits))/256])
            {
                case UNDEFINED:
                    return 255;
                break;
                    
                case ROM:
                case RAM:
                    return emsMemory_[emsNumber].main[(long) ((address - computerConfiguration.emsConfig_[emsNumber].start) |(computerConfiguration.emsConfig_[emsNumber].page << computerConfiguration.emsConfig_[emsNumber].maskBits))];
                break;
                    
                default:
                    return 255;
                break;
            }
        break;
            
		case UNDEFINED:
			return 255;
		break;

        case MAPPEDROM:
		case ROM:
            emsNumber = (memoryType_[address / 256] >> 8);
            address = (address & computerConfiguration.memConfig_[emsNumber].memMask) | computerConfiguration.memConfig_[emsNumber].start;

			return mainMemory_[address];
		break;

		case MAPPEDRAM:
		case MAINRAM:
			address = (address & computerConfiguration.memConfig_[0].memMask) | computerConfiguration.memConfig_[0].start;
			return mainMemory_[address];
		break;

        case RAM:
            return mainMemory_[address];
        break;

		case MC6847RAM:
			return mc6847Pointer->read6847(address);
		break;

		case MC6845RAM:
			return mc6845Pointer->read6845(address & 0x7ff);
		break;

		case MC6845REGISTERS:
			return mc6845Pointer->readData6845(address);
		break;

		case PAGER:
			switch (pagerMemoryType_[((getPager(address>>computerConfiguration.pagerMaskBits_) << computerConfiguration.pagerMaskBits_) |(address &computerConfiguration.pagerMask_))/256])
			{
				case UNDEFINED:
					return 255;
				break;

				case ROM:
				case RAM:
					return mainMemory_[(getPager(address>>computerConfiguration.pagerMaskBits_) << computerConfiguration.pagerMaskBits_) |(address &computerConfiguration.pagerMask_)];
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

void Diy::writeMem(Word address, Byte value, bool writeRom)
{
	address_ = address;
    writeMemDebug(address_, value, writeRom);
}

void Diy::writeMemDebug(Word address, Byte value, bool writeRom)
{
    size_t emsNumber = (memoryType_[address / 256] >> 8);

    address = address | bootstrap_;

    if (loadedOs_ == ELFOS)
    {
        if (address == 0x400 && value >= 4 && value <= 128)
            loadedOs_ = ELFOS_4;
    }
        
    if (computerConfiguration.emsConfigNumber_ != 0)
    {
        for (size_t num=0; num<computerConfiguration.emsConfigNumber_; num++)
        {
            if (elfConfiguration.elfPortConf.emsOutput[num] == -1)
            {
                if (address>=computerConfiguration.emsConfig_[num].outputStart && address <=computerConfiguration.emsConfig_[num].outputEnd)
                {
                    computerConfiguration.emsConfig_[num].page = value & computerConfiguration.emsConfig_[num].outputMask;
                }
            }
        }
    }
    if (elfConfiguration.elfPortConf.mc6847OutputMode == 1 && address >= 0xff00)
        mc6847Pointer->outMc6847(value);

	switch (memoryType_[address/256]&0xff)
	{
        case EMSMEMORY:
            switch (emsMemory_[emsNumber].memoryType_[((address - computerConfiguration.emsConfig_[emsNumber].start) |(computerConfiguration.emsConfig_[emsNumber].page << computerConfiguration.emsConfig_[emsNumber].maskBits))/256])
            {
                case UNDEFINED:
                case ROM:
                    if (writeRom)
                        emsMemory_[emsNumber].main[(long) ((address - computerConfiguration.emsConfig_[emsNumber].start) |(computerConfiguration.emsConfig_[emsNumber].page << computerConfiguration.emsConfig_[emsNumber].maskBits))] = value;
                break;
                    
                case RAM:
                    if (!getMpButtonState())
                    {
                        emsMemory_[emsNumber].main[(long) ((address - computerConfiguration.emsConfig_[emsNumber].start) |(computerConfiguration.emsConfig_[emsNumber].page << computerConfiguration.emsConfig_[emsNumber].maskBits))] = value;
                        if (address >= memoryStart_ && address<(memoryStart_ + 256))
                            p_Main->updateDebugMemory(address);
                        p_Main->updateAssTabCheck(address);
                    }
                break;
            }
        break;
       
		case MC6847RAM:
			mc6847Pointer->write(address, value);
			mainMemory_[address] = value;
			if (address >= memoryStart_ && address<(memoryStart_ + 256))
				p_Main->updateDebugMemory(address);
		break;

		case MC6845RAM:
			mc6845Pointer->write6845(address & 0x7ff, value);
		break;

		case MC6845REGISTERS:
			mc6845Pointer->writeRegister6845(address, value);
		break;
            
		case UNDEFINED:
            if (writeRom)
                mainMemory_[address]=value;
        break;

        case MAPPEDROM:
        case ROM:
            emsNumber = (memoryType_[address / 256] >> 8);
            address = (address & computerConfiguration.memConfig_[emsNumber].memMask) | computerConfiguration.memConfig_[emsNumber].start;

			if (writeRom)
				mainMemory_[address]=value;
		break;

		case MAPPEDRAM:
		case MAINRAM:
			if (!getMpButtonState())
			{
				address = (address & computerConfiguration.memConfig_[0].memMask) | computerConfiguration.memConfig_[0].start;
				if (mainMemory_[address]==value)
					return;
				mainMemory_[address]=value;
				if (address >= (memoryStart_ & computerConfiguration.memConfig_[0].memMask) && address<((memoryStart_ & computerConfiguration.memConfig_[0].memMask) | 256))
					p_Main->updateDebugMemory(address);
				p_Main->updateAssTabCheck(address);
			}
		break;

        case RAM:
            if (!getMpButtonState())
            {
                if (mainMemory_[address]==value)
                    return;
                mainMemory_[address]=value;
                if (address >= memoryStart_ && address<(memoryStart_ + 256))
                    p_Main->updateDebugMemory(address);
                p_Main->updateAssTabCheck(address);
            }
        break;

        case PAGER:
			switch (pagerMemoryType_[((getPager(address>>computerConfiguration.pagerMaskBits_) << computerConfiguration.pagerMaskBits_) |(address &computerConfiguration.pagerMask_))/256])
			{
				case UNDEFINED:
				case ROM:
					if (writeRom)
						mainMemory_[(getPager(address>>computerConfiguration.pagerMaskBits_) << computerConfiguration.pagerMaskBits_) |(address &computerConfiguration.pagerMask_)] = value;
				break;

				case RAM:
					if (!getMpButtonState())
						mainMemory_[(getPager(address>>computerConfiguration.pagerMaskBits_) << computerConfiguration.pagerMaskBits_) |(address &computerConfiguration.pagerMask_)] = value;
					if (address >= memoryStart_ && address<(memoryStart_ + 256))
						p_Main->updateDebugMemory(address);
					p_Main->updateAssTabCheck(address);
				break;
			}
		break;

	}
}

void Diy::cpuInstruction()
{
	if (cpuMode_ == RUN)
	{
        cpuCycleStep();
	}
	else
	{
		machineCycle();
		machineCycle();
		cpuCycles_ = 0;
		instructionCounter_= 0;
		p_Main->startTime();
		if (cpuMode_ == LOAD)
		{
			showData(readMem(address_));
            ledCycleValue_ = 1;
		}
	}
}

void Diy::resetPressed()
{
    if (elfConfiguration.bootStrap)
        bootstrap_ = 0x8000;

    p_Main->stopTerminal();
    terminalStop();

    resetCpu();
    if (elfConfiguration.use8275)
        i8275Pointer->cRegWrite(0x40);
    if (elfConfiguration.autoBoot)
    {
        scratchpadRegister_[0]=p_Main->getBootAddress("Diy", DIY);
        autoBoot();
    }
    resetPressed_ = false;
    elfRunState_ = RESETSTATE;
    p_Main->setSwName("");
    p_Main->eventUpdateTitle();
    startElfKeyFile("Diy");
}

void Diy::configureMemory()
{
    size_t memConfNumber = 0;
    Word mask;
    
    while (memConfNumber < computerConfiguration.memConfigNumber_)
    {
        switch (computerConfiguration.memConfig_[memConfNumber].type & 0xff)
        {
            case ROM:
                defineMemoryType(computerConfiguration.memConfig_[memConfNumber].start, computerConfiguration.memConfig_[memConfNumber].end, computerConfiguration.memConfig_[memConfNumber].type);

                if (computerConfiguration.memConfig_[memConfNumber].memMask != 0xFFFF)
                {
                    defineMemoryType(computerConfiguration.memConfig_[memConfNumber].start + computerConfiguration.memConfig_[memConfNumber].memMask + 1, computerConfiguration.memConfig_[memConfNumber].end, MAPPEDROM + (computerConfiguration.memConfig_[memConfNumber].type & 0xff00));
                }

                loadRomRam(memConfNumber);
            break;
                
            case RAM:
                defineMemoryType(computerConfiguration.memConfig_[memConfNumber].start, computerConfiguration.memConfig_[memConfNumber].end, computerConfiguration.memConfig_[memConfNumber].type);
                initRam(computerConfiguration.memConfig_[memConfNumber].start, computerConfiguration.memConfig_[memConfNumber].end);
                loadRomRam(memConfNumber);
            break;

            case MAINRAM:
                defineMemoryType(computerConfiguration.memConfig_[memConfNumber].start, computerConfiguration.memConfig_[memConfNumber].end, computerConfiguration.memConfig_[memConfNumber].type);
                if (computerConfiguration.memConfig_[memConfNumber].memMask != 0xFFFF)
                {
                    defineMemoryType(computerConfiguration.memConfig_[memConfNumber].start + computerConfiguration.memConfig_[memConfNumber].memMask + 1, computerConfiguration.memConfig_[memConfNumber].end, MAPPEDRAM);
                }

                initRam(computerConfiguration.memConfig_[memConfNumber].start, computerConfiguration.memConfig_[memConfNumber].end);

                p_Main->assDefault("mycode", computerConfiguration.memConfig_[memConfNumber].start&0xff00, computerConfiguration.memConfig_[memConfNumber].start&0xff00+0xfff);
                loadRomRam(memConfNumber);
            break;
                
            case PAGER:
                configurePortExt(elfConfiguration.elfPortConf);
                
                allocPagerMemory(computerConfiguration.memConfig_[memConfNumber].start, computerConfiguration.memConfig_[memConfNumber].end);
                definePortExtForPager(computerConfiguration.startPort_, computerConfiguration.endPort_);

                defineMemoryType(computerConfiguration.memConfig_[memConfNumber].start, computerConfiguration.memConfig_[memConfNumber].end, PAGER);
            break;
        }
        memConfNumber++;
    }

    size_t emsNumber = 0;
    
    while (emsNumber < computerConfiguration.emsConfigNumber_)
    {
        computerConfiguration.emsConfig_[emsNumber].maskBits = 16;
        computerConfiguration.emsConfig_[emsNumber].mask = 0xFFFF;
        mask = computerConfiguration.emsConfig_[emsNumber].end - computerConfiguration.emsConfig_[emsNumber].start;
        while ((mask & 0x8000) == 0)
        {
            computerConfiguration.emsConfig_[emsNumber].maskBits--;
            computerConfiguration.emsConfig_[emsNumber].mask = computerConfiguration.emsConfig_[emsNumber].mask >> 1;
            mask = mask << 1;
        }
        
        emsMemory_.resize(emsNumber+1);
        
        if (computerConfiguration.emsConfig_[emsNumber].emsType == ROM)
            readRomMapperBinFile(emsNumber, computerConfiguration.emsConfig_[emsNumber].dirname+computerConfiguration.emsConfig_[emsNumber].filename);
        else
            allocEmsMemorySegment(emsNumber);

        defineMemoryType(computerConfiguration.emsConfig_[emsNumber].start, computerConfiguration.emsConfig_[emsNumber].end, computerConfiguration.emsConfig_[emsNumber].type);
        emsNumber++;
    }
}

void Diy::configureElfExtensions()
{
	wxString fileName, fileName2;

	if (elfConfiguration.vtType != VTNONE)
	{
		double zoom = p_Main->getZoomVt();
        if (elfConfiguration.vtType == VT52)
            vtPointer = new Vt100("Elf II - VT 52", p_Main->getVtPos(DIY), wxSize(800*zoom, 500*zoom), zoom, DIY, elfClockSpeed_, elfConfiguration, UART1);
        else
            vtPointer = new Vt100("Elf II - VT 100", p_Main->getVtPos(DIY), wxSize(800*zoom, 500*zoom), zoom, DIY, elfClockSpeed_, elfConfiguration, UART1);
		p_Vt100[UART1] = vtPointer;
		vtPointer->configure(elfConfiguration.baudR, elfConfiguration.baudT, elfConfiguration.elfPortConf);
        if (elfConfiguration.useUart16450)
            configureUart16450(elfConfiguration.elfPortConf);
		vtPointer->Show(true);
		vtPointer->drawScreen();
	}

    if (elfConfiguration.vtExternal)
    {
        p_Serial = new Serial(DIY, elfClockSpeed_, elfConfiguration);
        p_Serial->configure(elfConfiguration.baudR, elfConfiguration.baudT, elfConfiguration.elfPortConf);
    }

	if (elfConfiguration.usePixie)
	{
		double zoom = p_Main->getZoom();
		double scale = p_Main->getScale();
		pixiePointer = new Pixie( "Elf II - Pixie", p_Main->getPixiePos(DIY), wxSize(64*zoom*scale, 128*zoom), zoom, scale, DIY);
		p_Video = pixiePointer;
		pixiePointer->configurePixie(elfConfiguration.elfPortConf);
		pixiePointer->initPixie();
		pixiePointer->setZoom(zoom);
		pixiePointer->Show(true);
	}

	if (elfConfiguration.use6845)
	{
		double zoom = p_Main->getZoom();
		mc6845Pointer = new MC6845( "Elf II - MC6845", p_Main->get6845Pos(DIY), wxSize(64*8*zoom, 16*8*2*zoom), zoom, DIY, elfClockSpeed_, 8, elfConfiguration.elfPortConf);
		p_Video = mc6845Pointer;
		mc6845Pointer->configure6845(elfConfiguration.elfPortConf);
		mc6845Pointer->init6845();
		mc6845Pointer->Show(true);
	}

	if (elfConfiguration.useS100)
	{
		double zoom = p_Main->getZoom();
		mc6845Pointer = new MC6845( "Elf II - Quest Super Video", p_Main->get6845Pos(DIY), wxSize(64*7*zoom, 16*9*zoom), zoom, DIY, elfClockSpeed_, 7, elfConfiguration.elfPortConf);
		p_Video = mc6845Pointer;
		mc6845Pointer->configureSuperVideo();
		mc6845Pointer->init6845();
		mc6845Pointer->Show(true);
	}

	if (elfConfiguration.use8275)
	{
		double zoom = p_Main->getZoom();
		i8275Pointer = new i8275( "Elf II - Intel 8275", p_Main->get8275Pos(DIY), wxSize(80*8*zoom, 24*10*2*zoom), zoom, DIY, elfClockSpeed_);
		p_Video = i8275Pointer;
		i8275Pointer->configure8275(elfConfiguration.elfPortConf);
		i8275Pointer->init8275();
		i8275Pointer->Show(true);
	}

	if (elfConfiguration.use6847)
	{
		double zoom = p_Main->getZoom();
		mc6847Pointer = new mc6847( "Elf II - MC6847", p_Main->get6847Pos(DIY), wxSize(elfConfiguration.charLine*8*zoom, elfConfiguration.screenHeight6847*zoom), zoom, DIY, elfClockSpeed_, elfConfiguration.elfPortConf);
		p_Video = mc6847Pointer;
		mc6847Pointer->configure(elfConfiguration.elfPortConf);
		mc6847Pointer->init6847();
		mc6847Pointer->Show(true);
	}

	if (elfConfiguration.useTMS9918)
	{
		double zoom = p_Main->getZoom();
		tmsPointer = new Tms9918( "Elf II - TMS 9918", p_Main->getTmsPos(DIY), wxSize(320*zoom,240*zoom), zoom, DIY, elfClockSpeed_);
		p_Video = tmsPointer;
		tmsPointer->configure(elfConfiguration.elfPortConf);
		tmsPointer->Show(true);
	}

	if (elfConfiguration.fdcEnabled)
	{
		configure1793(1, 40, 18, 256, DIY, elfConfiguration.elfPortConf);
		resetFdc();
	}

	if (elfConfiguration.ideEnabled)
	{
		configureIde(p_Main->getIdeDir(DIY) + p_Main->getIdeFile(DIY), p_Main->getIdeDir(DIY) + "disk2.ide", elfConfiguration.elfPortConf);
	}

	if (p_Main->getPrinterStatus(DIY))
	{
		p_Printer = new Printer();
		p_Printer->configureElfPrinter(elfConfiguration.elfPortConf);
		p_Printer->initElf(p_Printer, "DIY");
	}

	setQsound (elfConfiguration.qSound_);

	if (elfConfiguration.useKeyboard)
		configureKeyboard(DIY, elfConfiguration.elfPortConf);

	if (elfConfiguration.UsePS2)
		configurePs2(elfConfiguration.ps2Interrupt, elfConfiguration.elfPortConf);
}

void Diy::moveWindows()
{
	if (elfConfiguration.usePixie)
		pixiePointer->Move(p_Main->getPixiePos(DIY));
	if (elfConfiguration.useTMS9918)
		tmsPointer->Move(p_Main->getTmsPos(DIY));
	if (elfConfiguration.use6845||elfConfiguration.useS100)
		mc6845Pointer->Move(p_Main->get6845Pos(DIY));
	if (elfConfiguration.use6847)
		mc6847Pointer->Move(p_Main->get6847Pos(DIY));
	if (elfConfiguration.use8275)
		i8275Pointer->Move(p_Main->get8275Pos(DIY));
	if (elfConfiguration.vtType != VTNONE)
		vtPointer->Move(p_Main->getVtPos(DIY));
}

void Diy::updateTitle(wxString Title)
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

void Diy::setForceUpperCase(bool status)
{
	if (elfConfiguration.vtType != VTNONE)
		vtPointer->setForceUCVt(status);
	if (elfConfiguration.useKeyboard)
		setForceUpperCaseKeyboard(status);
}

void Diy::onReset()
{
	resetPressed_ = true;
}

void Diy::sleepComputer(long ms)
{
	threadPointer->Sleep(ms);
}

Byte Diy::read6845CharRom(Word address)
{
	if (elfConfiguration.use6845||elfConfiguration.useS100)
		return mc6845Pointer->read6845CharRom(address);
	else
		return 0;
}

void Diy::write6845CharRom(Word address, Byte value)
{
	if (elfConfiguration.use6845||elfConfiguration.useS100)
		mc6845Pointer->write6845CharRom(address, value);
}

Byte Diy::read8275CharRom(Word address)
{
    if (elfConfiguration.use8275)
        return i8275Pointer->read8275CharRom(address);
    else
        return 0;
}

void Diy::write8275CharRom(Word address, Byte value)
{
    if (elfConfiguration.use8275)
        i8275Pointer->write8275CharRom(address, value);
}

Byte Diy::read8275VideoRam(Word address)
{
    if (elfConfiguration.use8275)
        return i8275Pointer->read8275VideoRam(address);
    else
        return 0;
}

void Diy::write8275VideoRam(Word address, Byte value)
{
    if (elfConfiguration.use8275)
        i8275Pointer->write8275VideoRam(address, value);
}

Byte Diy::read6847CharRom(Word address)
{
	if (elfConfiguration.use6847)
		return mc6847Pointer->read6847CharRom(address);
	else
		return 0;
}

void Diy::write6847CharRom(Word address, Byte value)
{
	if (elfConfiguration.use6847)
		mc6847Pointer->write6847CharRom(address, value);
}

int Diy::readDirect6847(Word address)
{
	if (elfConfiguration.use6847)
		return mc6847Pointer->readDirect6847(address);
	else
		return 0;
}

void Diy::writeDirect6847(Word address, int value)
{
	if (elfConfiguration.use6847)
		mc6847Pointer->writeDirect6847(address, value);
}

Word Diy::get6847RamMask()
{
 	if (elfConfiguration.use6847)
		return mc6847Pointer->get6847RamMask();
	else
		return 0;
}

void Diy::setLedMs(long ms)
{
	elf2ScreenPointer->setLedMs(ms);
    if (ms == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((elfClockSpeed_ * 1000000) / 8) / 1000) * ms;
    ledCycleValue_ = ledCycleSize_;
}

Byte Diy::getKey(Byte vtOut)
{
	return elf2ScreenPointer->getKey(vtOut);
}

void Diy::activateMainWindow()
{
	bool maximize = IsMaximized();
	Iconize(false);
	Raise();
	Show(true);
	Maximize(maximize);
}

void Diy::releaseButtonOnScreen(HexButton* buttonPointer, int WXUNUSED(buttonType))
{
    elf2ScreenPointer->releaseButtonOnScreen(buttonPointer);
}

void Diy::refreshPanel()
{
    elf2ScreenPointer->refreshPanel();
}

void Diy::OnRtcTimer(wxTimerEvent&WXUNUSED(event))
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
