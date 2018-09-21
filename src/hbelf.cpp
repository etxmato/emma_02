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

#include "hbelf.h"

ElfScreen::ElfScreen(wxWindow *parent, const wxSize& size, int tilType)
: Panel(parent, size)
{
    tilType_ = tilType;
}

ElfScreen::~ElfScreen()
{
	delete mainBitmapPointer;

	delete runSwitchButton;
	delete mpSwitchButton;
	delete powerSwitchButton;
	delete loadSwitchButton;
	delete inSwitchButton;

	for (int i=0; i<8; i++)
	{
		delete dataSwitchButton[i];
	}
	for (int i=0; i<4; i++)
	{
		delete efSwitchButton[i];
	}
    if (tilType_ == TIL311)
    {
        for (int i=0; i<4; i++)
            delete addressPointer[i];
        for (int i=0; i<2; i++)
            delete dataPointer[i];
    }
    else
    {
        for (int i=0; i<4; i++)
            delete addressTil313Pointer[i];
        for (int i=0; i<2; i++)
            delete dataTil313Pointer[i];
    }
    
 	delete qLedPointer;
}

void ElfScreen::init()
{
	keyStart_ = 0;
	keyEnd_ = 0;
	lastKey_ = 0;
	forceUpperCase_ = p_Main->getUpperCase(ELF);

	wxClientDC dc(this);

	mainBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/elf.png", wxBITMAP_TYPE_PNG);

	runSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(0x31, 0x31, 0x30), BUTTON_DOWN, 190, 312, "");
	mpSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(0x31, 0x31, 0x30), BUTTON_DOWN, 150, 312, "");
	powerSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(0x31, 0x31, 0x30), BUTTON_DOWN, 313, 312, "");
	loadSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(0x31, 0x31, 0x30), BUTTON_DOWN, 75, 312, "");
	inSwitchButton = new SwitchButton(dc, PUSH_BUTTON, wxColour(0x31, 0x31, 0x30), BUTTON_UP, 45, 312, "");

	qLedPointer = new Led(dc, 324, 250, ELFLED);
	updateQLed_ = true;

	for (int i=0; i<8; i++)
	{
		dataSwitchButton[i] = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(0x31, 0x31, 0x30), BUTTON_DOWN, 18+30*(7-i), 362, "");
	}
	for (int i=0; i<4; i++)
	{
		efSwitchButton[i] = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(0x31, 0x31, 0x30), BUTTON_DOWN, 138+30*(3-i), 422, "");
	}
    for (int i=0; i<4; i++)
    {
        if (tilType_ == TIL311)
        {
            addressPointer[i] = new Til311();
            addressPointer[i]->init(dc, 18+i*40, 226);
            updateAddress_ = true;
        }
        else
        {
            addressTil313Pointer[i] = new Til313();
            addressTil313Pointer[i]->init(dc, 18+i*40, 226);
            updateAddressTil313_ = true;
        }
    }
	for (int i=0; i<2; i++)
	{
        if (tilType_ == TIL311)
        {
            dataPointer[i] = new Til311();
            dataPointer[i]->init(dc, 218+i*40, 226);
            updateData_ = true;
        }
        else
        {
            dataTil313Pointer[i] = new Til313();
            dataTil313Pointer[i]->init(dc, 218+i*40, 226);
            updateDataTil313_ = true;
        }
	}
	this->connectKeyEvent(this);
}

void ElfScreen::onPaint(wxPaintEvent&WXUNUSED(event))
{
	wxPaintDC dc(this);
	dc.DrawBitmap(*mainBitmapPointer, 0, 0);

    for (int i=0; i<4; i++)
    {
        efSwitchButton[i]->onPaint(dc);
    }
    if (tilType_ == TIL311)
    {
        for (int i=0; i<4; i++)
            addressPointer[i]->onPaint(dc);
        for (int i=0; i<2; i++)
            dataPointer[i]->onPaint(dc);
	}
    else
    {
        for (int i=0; i<4; i++)
            addressTil313Pointer[i]->onPaint(dc);
        for (int i=0; i<2; i++)
            dataTil313Pointer[i]->onPaint(dc);
    }
	qLedPointer->onPaint(dc);

	inSwitchButton->onPaint(dc);
	runSwitchButton->onPaint(dc);
	mpSwitchButton->onPaint(dc);
	powerSwitchButton->onPaint(dc);
	loadSwitchButton->onPaint(dc);
	for (int i=0; i<8; i++)
	{
		dataSwitchButton[i]->onPaint(dc);
	}
}

void ElfScreen::onMousePress(wxMouseEvent&event)
{
	int x, y;
	event.GetPosition(&x, &y);

	wxClientDC dc(this);

	if (inSwitchButton->onMousePress(dc, x, y))
		p_Computer->onInButtonPress(p_Computer->getData());
}

void ElfScreen::onMouseRelease(wxMouseEvent&event)
{
	int x, y;
	event.GetPosition(&x, &y);

	wxClientDC dc(this);

	if (inSwitchButton->onMouseRelease(dc, x, y))
		p_Computer->onInButtonRelease();
	if (runSwitchButton->onMouseRelease(dc, x, y))
		p_Computer->onRun();
	if (mpSwitchButton->onMouseRelease(dc, x, y))
		p_Computer->onMpButton();
	if (powerSwitchButton->onMouseRelease(dc, x, y))
		p_Main->stopComputer();
	if (loadSwitchButton->onMouseRelease(dc, x, y))
		p_Computer->onLoadButton();
	for (int i=0; i<8; i++)
	{
		if (dataSwitchButton[i]->onMouseRelease(dc, x, y))
			p_Computer->dataSwitch(i);
	}
	for (int i=0; i<4; i++)
	{
		if (efSwitchButton[i]->onMouseRelease(dc, x, y))
			p_Computer->efSwitch(i);
	}
}

BEGIN_EVENT_TABLE(Elf, wxFrame)
	EVT_CLOSE (Elf::onClose)
END_EVENT_TABLE()

Elf::Elf(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
	elfClockSpeed_ = clock;
	elfConfiguration = conf;
	data_ = 0;
	lastAddress_ = 0;

#ifndef __WXMAC__
	SetIcon(wxICON(app_icon));
#endif

	this->SetClientSize(size);

	elfScreenPointer = new ElfScreen(this, size, elfConfiguration.tilType);
	elfScreenPointer->init();
    
    runningGame_ = "";
}

Elf::~Elf()
{
	if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
	{
		p_Main->setKeypadPos(ELF, keypadPointer->GetPosition());
		keypadPointer->Destroy();
	}
	if (elfConfiguration.useLedModule && !ledModuleClosed_)
	{
		p_Main->setLedModulePos(ledModulePointer->GetPosition());
		ledModulePointer->Destroy();
	}
	if (elfConfiguration.usePixie)
	{
		p_Main->setPixiePos(ELF, pixiePointer->GetPosition());
		pixiePointer->Destroy();
	}
	if (elfConfiguration.useTMS9918)
	{
		p_Main->setTmsPos(ELF, tmsPointer->GetPosition());
		tmsPointer->Destroy();
	}
	if (elfConfiguration.use6845||elfConfiguration.useS100)
	{
		p_Main->set6845Pos(ELF, mc6845Pointer->GetPosition());
		mc6845Pointer->Destroy();
	}
	if (elfConfiguration.use6847)
	{
		p_Main->set6847Pos(ELF, mc6847Pointer->GetPosition());
		mc6847Pointer->Destroy();
	}
	if (elfConfiguration.use8275)
	{
		p_Main->set8275Pos(ELF, i8275Pointer->GetPosition());
		i8275Pointer->Destroy();
	}
	if (elfConfiguration.vtType != VTNONE)
	{
		p_Main->setVtPos(ELF, vtPointer->GetPosition());
		vtPointer->Destroy();
	}
    if (elfConfiguration.vtExternal)
        delete p_Serial;
	if (p_Main->getPrinterStatus(ELF))
	{
		p_Printer->closeFrames();
		delete p_Printer;
	}
	p_Main->setMainPos(ELF, GetPosition());

	delete elfScreenPointer;
}

void Elf::showModules(bool status)
{
	if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
		keypadPointer->Show(status);
	if (elfConfiguration.useLedModule && !ledModuleClosed_)
		ledModulePointer->Show(status);
}

void Elf::onClose(wxCloseEvent&WXUNUSED(event) )
{
	p_Main->stopComputer();
}

bool Elf::keyDownPressed(int key)
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

bool Elf::keyUpReleased(int key)
{
	if (key == inKey1_ || key == inKey2_)
	{
		onInButtonRelease();
		return true;
	}
	onHexKeyUp(key);
//    keyboardUp();
	if (elfConfiguration.UsePS2)
	{
		keyUpPs2(key);
		return true;
	}
	return false;
}

void Elf::charEvent(int keycode)
{
	if (elfConfiguration.useKeyboard)
		charEventKeyboard(keycode);
}

void Elf::onInButtonPress()
{
	if (elfConfiguration.useHexKeyboard)
		onInButtonPress(switches_);
	else
		onInButtonPress(getData());
}

void Elf::onInButtonPress(Byte value)
{
	inPressed_ = true;
	if (cpuMode_ == LOAD)
	{
		dmaIn(value);
		showData(value);
	}
	else
	{
		efSwitchState_[3] = 1;
	}
	elfScreenPointer->inSetState(BUTTON_DOWN);
}

void Elf::onInButtonRelease()
{
	if (efSwitchState_[3] == 1)
	{
		inPressed_ = false;
		efSwitchState_[3] = 0;
	}
	elfScreenPointer->inSetState(BUTTON_UP);
}

void Elf::onHexKeyDown(int keycode)
{
#if defined (__WXMAC__)
    if (ef3State_ == 0) // This is to avoid multiple key presses on OSX
        return;
#endif
    
//	if (!elfConfiguration.useHexKeyboard)
//		return;
	for (int i=0; i<16; i++)
	{
		if (keycode == keyDefA1_[i])
		{
            ef3State_ = 0;
            switches_ = ((switches_ << 4) & 0xf0) | i;
            if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
                keypadPointer->onNumberDown(i);
		}
		if (keycode == keyDefA2_[i])
		{
            ef3State_ = 0;
            switches_ = ((switches_ << 4) & 0xf0) | i;
            if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
                keypadPointer->onNumberDown(i);
		}
	}
}

void Elf::onHexDown(int hex)
{
	ef3State_ = 0;
	switches_ = ((switches_ << 4) & 0xf0) | hex;
}

void Elf::onHexKeyUp(int keycode)
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

void Elf::configureComputer()
{
	efType_[1] = EF1UNDEFINED;
	efType_[2] = EF2UNDEFINED;
	efType_[3] = EF3UNDEFINED;

//	int efPort;
	wxString printBuffer;

//	int input = p_Main->getConfigItem("/Elf/HexInput", 4l);
//	int output = p_Main->getConfigItem("/Elf/HexOutput", 4l);

    setCycleType(COMPUTERCYCLE, LEDCYCLE);
	p_Main->message("Configuring Elf");
	printBuffer.Printf("	Output %d: display output, input %d: data input", elfConfiguration.elfPortConf.hexOutput, elfConfiguration.elfPortConf.hexInput);
	p_Main->message(printBuffer);

	p_Computer->setInType(elfConfiguration.elfPortConf.hexInput, ELFIN);
	p_Computer->setOutType(elfConfiguration.elfPortConf.hexOutput, ELFOUT);

    if (elfConfiguration.useRomMapper)
    {
//      output = p_Main->getConfigItem("/Elf/EmsOutput", 7l);
        printBuffer.Printf("	Output %d: rom mapper", elfConfiguration.elfPortConf.emsOutput);
        p_Computer->setOutType(elfConfiguration.elfPortConf.emsOutput, ROMMAPPEROUT);
        p_Main->message(printBuffer);
    }
    if (elfConfiguration.useEms)
    {
//      output = p_Main->getConfigItem("/Elf/EmsOutput", 7l);
        printBuffer.Printf("	Output %d: EMS-512KB", elfConfiguration.elfPortConf.emsOutput);
        p_Computer->setOutType(elfConfiguration.elfPortConf.emsOutput, EMSMAPPEROUT);
        p_Main->message(printBuffer);
    }
	if (elfConfiguration.useTape)
	{
//		efPort = p_Main->getConfigItem("/Elf/TapeEf", 2l);
		efType_[elfConfiguration.elfPortConf.tapeEf] = ELF2EF2;
		printBuffer.Printf("	EF %d: cassette in", elfConfiguration.elfPortConf.tapeEf);
		p_Main->message(printBuffer);
	}
    if (elfConfiguration.useHexKeyboardEf3)
    {
//        efPort = p_Main->getConfigItem("/Elf/HexEf", 3l);
        efType_[elfConfiguration.elfPortConf.hexEf] = ELFEF3;
        printBuffer.Printf("	EF %d: 0 when hex button pressed", elfConfiguration.elfPortConf.hexEf);
        p_Main->message(printBuffer);
    }
	if (efType_[4] == 0)
	{
		efType_[4] = ELFINEF;
		p_Main->message("	EF 4: 0 when in button pressed");
	}
	p_Main->message("");

    inKey1_ = p_Main->getDefaultInKey1("Elf");
    inKey2_ = p_Main->getDefaultInKey2("Elf");
	p_Main->getDefaultHexKeys(ELF, "Elf", "A", keyDefA1_, keyDefA2_, keyDefGameHexA_);

	if (p_Main->getConfigBool("/Elf/GameAuto", true))
		p_Main->loadKeyDefinition(p_Main->getRomFile(ELF, MAINROM1), p_Main->getRomFile(ELF, MAINROM2), keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
	
	resetCpu();
}

void Elf::setPrinterEf()
{
    ef3State_ = 0;
}

void Elf::reLoadKeyDefinition(wxString fileName)
{
    runningGame_ = fileName;
    if (p_Main->getConfigBool("/Elf/GameAuto", true))
        p_Main->loadKeyDefinition("", fileName, keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
}

void Elf::reDefineKeysA(int hexKeyDefA1[], int hexKeyDefA2[])
{
	for (int i=0; i<16; i++)
	{
		keyDefA1_[i] = hexKeyDefA1[i];
		keyDefA2_[i] = hexKeyDefA2[i];
	}
}

void Elf::reDefineKeysB(int hexKeyDefB1[], int hexKeyDefB2[])
{
    for (int i=0; i<16; i++)
    {
        keyDefB1_[i] = hexKeyDefB1[i];
        keyDefB2_[i] = hexKeyDefB2[i];
    }
}

void Elf::initComputer()
{
	Show(p_Main->getUseElfControlWindows(ELF));
	runButtonState_ = 0;
	for (int i=0; i<8; i++)  dataSwitchState_[i]=0;
	for (int i=0; i<4; i++)  efSwitchState_[i]=0;
	inPressed_ = false;
	mpButtonState_ = 0;
	loadButtonState_ = 1;
	cassetteEf_ = 0;

	switches_ = 0;
	ef3State_ = 1;
	hexKeypadClosed_ = false;
	ledModuleClosed_ = false;
	elfRunState_ = RESETSTATE;
}

Byte Elf::ef(int flag)
{
	switch(efType_[flag])
	{
		case 0:
			return 1-efSwitchState_[flag-1];
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

		case VTINEF:
			if (inPressed_ == true)
				return 0;
			else
				return vtPointer->ef();
		break;

		case VTINEFSERIAL:
			if (inPressed_ == true)
				return 0;
			else
				return p_Serial->ef();
		break;

		case ELF2EF2:
			return cassetteEf_;
		break;

		case ELFEF3:
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

Byte Elf::ef4()
{
	if (inPressed_ == true)
		return 0;
	else
		return 1;
}

Byte Elf::in(Byte port, Word WXUNUSED(address))
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

		case ELFIN:
			if (elfConfiguration.useHexKeyboard)
				ret = switches_;
			else
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

Byte Elf::getData()
{
	return(dataSwitchState_[7]?128:0) +(dataSwitchState_[6]?64:0) +(dataSwitchState_[5]?32:0) +(dataSwitchState_[4]?16:0) +(dataSwitchState_[3]?8:0) +(dataSwitchState_[2]?4:0) +(dataSwitchState_[1]?2:0) +(dataSwitchState_[0]?1:0);
}

void Elf::out(Byte port, Word WXUNUSED(address), Byte value)
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

		case VTOUTSERIAL:
			p_Serial->out(value);
		break;

		case PRINTEROUT:
//			p_Main->eventPrintDefault(value); *** Maybe this was needed on Mac or Linux? however it doesn't work on windows, i.e. no printer window will be opened.
			if ((value & 0xfc) != 0)
				p_Printer->printerOut(value);
		break;

		case LEDMODOUT:
			ledModulePointer->write(value);
		break;

		case PS2OUT:
			outPs2(value);
		break;

		case ELFOUT:
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

void Elf::showData(Byte val)
{
    if (elfConfiguration.tilType == TIL311)
        elfScreenPointer->showData(val);
    else
        elfScreenPointer->showDataTil313(val);
}

void Elf::cycle(int type)
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

void Elf::cycleLed()
{
    if (ledCycleValue_ > 0)
    {
        ledCycleValue_ --;
        if (ledCycleValue_ <= 0)
        {
            ledCycleValue_ = ledCycleSize_;
            elfScreenPointer->ledTimeout();
            if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
                keypadPointer->ledTimeout();
            if (elfConfiguration.useLedModule && !ledModuleClosed_)
                ledModulePointer->ledTimeout();
        }
    }
}

void Elf::autoBoot()
{
	elfScreenPointer->runSetState(BUTTON_UP);
	runButtonState_ = 1;
	setClear(runButtonState_);
	if (cpuMode_ == RESET)  
    {
        if (elfConfiguration.tilType == TIL311)
            elfScreenPointer->showAddress(0);
        else
            elfScreenPointer->showAddressTil313(0);
    }
}

void Elf::switchQ(int value)
{
	elfScreenPointer->setQLed(value);

    if (elfConfiguration.vtType != VTNONE)
        vtPointer->switchQ(value);

    if (elfConfiguration.vtExternal)
        p_Serial->switchQ(value);
}

int Elf::getMpButtonState()
{
	return mpButtonState_;
}

void Elf::onRun()
{
	if (runButtonState_)
	{
		elfScreenPointer->runSetState(BUTTON_DOWN);
		runButtonState_ = 0;
	}
	else
	{
		elfScreenPointer->runSetState(BUTTON_UP);
		runButtonState_ = 1;
		p_Main->startTime();
	}
	setClear(runButtonState_);
	p_Main->eventUpdateTitle();
	if (cpuMode_ == RESET)  	
    {
        if (elfConfiguration.tilType == TIL311)
            elfScreenPointer->showAddress(0);
        else
            elfScreenPointer->showAddressTil313(0);
    }
}

void Elf::onMpButton()
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

void Elf::onLoadButton()
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
	if (cpuMode_ == RESET)  
    {
        if (elfConfiguration.tilType == TIL311)
            elfScreenPointer->showAddress(0);
        else
            elfScreenPointer->showAddressTil313(0);
    }
}

void Elf::dataSwitch(int i)
{
	if (dataSwitchState_[i])
	{
		dataSwitchState_[i] = 0;
	}
	else
	{
		dataSwitchState_[i] = 1;
	}
}

void Elf::efSwitch(int i)
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

void Elf::startComputer()
{
	startElfKeyFile("Elf");

	resetPressed_ = false;

	if (elfConfiguration.usePortExtender)
		configurePortExt(elfConfiguration.elfPortConf);

	ramStart_ = p_Main->getStartRam("Elf", ELF);
	Word ramEnd = p_Main->getEndRam("Elf", ELF);
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
        readRomMapperBinFile(p_Main->getRomDir(ELF, MAINROM1)+p_Main->getRomFile(ELF, MAINROM1));
        defineMemoryType(0x8000, 0xffff, ROMMAPPER);
    }
    
	p_Main->enableDebugGuiMemory();
    
    Word offset = 0;
    wxString fileName = p_Main->getRomFile(ELF, MAINROM1);
    if (fileName.Right(4) == ".ch8")
        offset = 0x200;
    if (!elfConfiguration.useRomMapper)
        readProgram(p_Main->getRomDir(ELF, MAINROM1), p_Main->getRomFile(ELF, MAINROM1), p_Main->getLoadromMode(ELF, 0), offset, NONAME);
  
    fileName = p_Main->getRomFile(ELF, MAINROM2);
    if (fileName.Right(4) == ".ch8")
        offset = 0x200;
    else
        offset = 0;
	readProgram(p_Main->getRomDir(ELF, MAINROM2), p_Main->getRomFile(ELF, MAINROM2), p_Main->getLoadromMode(ELF, 1), offset, NONAME);

	configureElfExtensions();
	if (elfConfiguration.autoBoot)
	{
		scratchpadRegister_[0]=p_Main->getBootAddress("Elf", ELF);
		autoBoot();
	}

	if (elfConfiguration.vtType != VTNONE)
		setEf(4,0);

	p_Main->setSwName("");
	p_Main->updateTitle();
	address_ = 0;

    if (elfConfiguration.tilType == TIL311)
        elfScreenPointer->showAddress(0);
    else
        elfScreenPointer->showAddressTil313(0);

	cpuCycles_ = 0;
	p_Main->startTime();

    int ms = (int) p_Main->getLedTimeMs(ELF);
    elfScreenPointer->setLedMs(ms);
    if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
        keypadPointer->setLedMs(ms);
    if (elfConfiguration.useLedModule && !ledModuleClosed_)
        ledModulePointer->setLedMs(ms);
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

void Elf::writeMemDataType(Word address, Byte type)
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
				mainMemoryDataType_[address]=type;
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

Byte Elf::readMemDataType(Word address)
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

Byte Elf::readMem(Word addr)
{
	address_ = addr;
    if (elfConfiguration.tilType == TIL311)
        elfScreenPointer->showAddress(0);
    else
        elfScreenPointer->showAddressTil313(0);

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

void Elf::writeMem(Word addr, Byte value, bool writeRom)
{
	address_ = addr;
    if (elfConfiguration.tilType == TIL311)
        elfScreenPointer->showAddress(0);
    else
        elfScreenPointer->showAddressTil313(0);

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
				if (addr >= (memoryStart_& ramMask_) && addr<((memoryStart_& ramMask_) + 256))
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

void Elf::cpuInstruction()
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
				scratchpadRegister_[0]=p_Main->getBootAddress("Elf", ELF);
				autoBoot();
			}
			resetPressed_ = false;
			p_Main->setSwName("");
            p_Main->eventUpdateTitle();
			startElfKeyFile("Elf");
		}
		if (debugMode_)
			p_Main->cycleDebug();
		if (pseudoLoaded_ && cycle0_ == 0)
			p_Main->cyclePseudoDebug();
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
            ledCycleValue_ = 1;
			threadPointer->Sleep(1);
		}
	}
}

void Elf::configureElfExtensions()
{
	wxString fileName, fileName2;

	if (elfConfiguration.vtType != VTNONE)
	{
		double zoom = p_Main->getZoomVt();
        if (elfConfiguration.vtType == VT52)
            vtPointer = new Vt100("Elf - VT 52", p_Main->getVtPos(ELF), wxSize(640*zoom, 400*zoom), zoom, ELF, elfClockSpeed_, elfConfiguration);
        else
            vtPointer = new Vt100("Elf - VT 100", p_Main->getVtPos(ELF), wxSize(640*zoom, 400*zoom), zoom, ELF, elfClockSpeed_, elfConfiguration);
		p_Vt100 = vtPointer;
		vtPointer->configure(elfConfiguration.baudR, elfConfiguration.baudT, elfConfiguration.elfPortConf);
		vtPointer->Show(true);
		vtPointer->drawScreen();
	}

    if (elfConfiguration.vtExternal)
    {
        p_Serial = new Serial(ELF, elfClockSpeed_, elfConfiguration);
        p_Serial->configure(elfConfiguration.baudR, elfConfiguration.baudT, elfConfiguration.elfPortConf);
    }

    if (elfConfiguration.usePixie)
	{
		double zoom = p_Main->getZoom();
		double scale = p_Main->getScale();
		pixiePointer = new Pixie( "Elf - Pixie", p_Main->getPixiePos(ELF), wxSize(64*zoom*scale, 128*zoom), zoom, scale, ELF);
		p_Video = pixiePointer;
		pixiePointer->configurePixie(elfConfiguration.elfPortConf);
		pixiePointer->initPixie();
		pixiePointer->setZoom(zoom);
		pixiePointer->Show(true);
	}

	if (elfConfiguration.use6845)
	{
		double zoom = p_Main->getZoom();
		mc6845Pointer = new MC6845( "Elf - MC6845", p_Main->get6845Pos(ELF), wxSize(64*8*zoom, 16*8*zoom), zoom, ELF, elfClockSpeed_, 8, elfConfiguration.elfPortConf);
		p_Video = mc6845Pointer;
		mc6845Pointer->configure6845(elfConfiguration.elfPortConf);
		mc6845Pointer->init6845();
		mc6845Pointer->Show(true);
	}

	if (elfConfiguration.useS100)
	{
		double zoom = p_Main->getZoom();
		mc6845Pointer = new MC6845( "Elf - Quest Super Video", p_Main->get6845Pos(ELF), wxSize(64*7*zoom, 16*9*zoom), zoom, ELF, elfClockSpeed_, 7, elfConfiguration.elfPortConf);
		p_Video = mc6845Pointer;
		mc6845Pointer->configureSuperVideo();
		mc6845Pointer->init6845();
		mc6845Pointer->Show(true);
	}

	if (elfConfiguration.use6847)
	{
		double zoom = p_Main->getZoom();
		mc6847Pointer = new mc6847( "Elf - MC6847", p_Main->get6847Pos(ELF), wxSize(elfConfiguration.charLine*8*zoom, elfConfiguration.screenHeight6847*zoom), zoom, ELF, elfClockSpeed_, elfConfiguration.elfPortConf);
		p_Video = mc6847Pointer;
		mc6847Pointer->configure(elfConfiguration.elfPortConf);
		mc6847Pointer->Show(true);
	}

	if (elfConfiguration.use8275)
	{
		double zoom = p_Main->getZoom();
		i8275Pointer = new i8275( "Elf - Intel 8275", p_Main->get8275Pos(ELF), wxSize(80*8*zoom, 24*10*2*zoom), zoom, ELF, elfClockSpeed_);
		p_Video = i8275Pointer;
		i8275Pointer->configure8275(elfConfiguration.elfPortConf);
		i8275Pointer->init8275();
		i8275Pointer->Show(true);
	}

	if (elfConfiguration.useTMS9918)
	{
		double zoom = p_Main->getZoom();
		tmsPointer = new Tms9918( "Elf - TMS 9918", p_Main->getTmsPos(ELF), wxSize(320*zoom,240*zoom), zoom, ELF, elfClockSpeed_);
		p_Video = tmsPointer;
		tmsPointer->configure(elfConfiguration.elfPortConf);
		tmsPointer->Show(true);
	}

	if (elfConfiguration.fdcEnabled)
	{
		configure1793(1, 40, 18, 256, ELF, elfConfiguration.elfPortConf);
		resetFdc();
	}

	if (elfConfiguration.ideEnabled)
	{
		configureIde(p_Main->getIdeDir(ELF) + p_Main->getIdeFile(ELF), p_Main->getIdeDir(ELF) + "disk2.ide", elfConfiguration.elfPortConf);
	}

	if (p_Main->getPrinterStatus(ELF))
	{
		p_Printer = new Printer();
		p_Printer->configureElfPrinter(elfConfiguration.elfPortConf);
		p_Printer->initElf(p_Printer, "Cosmac Elf");
	}

	if (elfConfiguration.useLedModule)
	{
		ledModulePointer = new LedModule("Led Module", p_Main->getLedModulePos(), wxSize(172, 116), ELF);
		ledModulePointer->configure(elfConfiguration.elfPortConf);
		ledModulePointer->Show(p_Main->getUseElfControlWindows(ELF));
	}

	setQsound (elfConfiguration.qSound_);

	if (elfConfiguration.useHexKeyboard)
	{
		keypadPointer = new Keypad("Keypad", p_Main->getKeypadPos(ELF), wxSize(172, 229), ELF);
		keypadPointer->Show(p_Main->getUseElfControlWindows(ELF));
		p_Main->message("Configuring Elf Keypad\n");
	}

	if (elfConfiguration.useKeyboard)
		configureKeyboard(ELF, elfConfiguration.elfPortConf);

	if (elfConfiguration.UsePS2)
		configurePs2(elfConfiguration.ps2Interrupt, elfConfiguration.elfPortConf);
}

void Elf::moveWindows()
{
	if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
		keypadPointer->Move(p_Main->getKeypadPos(ELF));
	if (elfConfiguration.useLedModule)
		ledModulePointer->Move(p_Main->getLedModulePos());
	if (elfConfiguration.usePixie)
		pixiePointer->Move(p_Main->getPixiePos(ELF));
	if (elfConfiguration.useTMS9918)
		tmsPointer->Move(p_Main->getTmsPos(ELF));
	if (elfConfiguration.use6845||elfConfiguration.useS100)
		mc6845Pointer->Move(p_Main->get6845Pos(ELF));
	if (elfConfiguration.use6847)
		mc6847Pointer->Move(p_Main->get6847Pos(ELF));
	if (elfConfiguration.use8275)
		i8275Pointer->Move(p_Main->get8275Pos(ELF));
	if (elfConfiguration.vtType != VTNONE)
		vtPointer->Move(p_Main->getVtPos(ELF));
}

void Elf::updateTitle(wxString Title)
{
	if (elfConfiguration.usePixie)
		pixiePointer->SetTitle("Elf - Pixie"+Title);
	if (elfConfiguration.useTMS9918)
		tmsPointer->SetTitle("Elf - TMS 9918"+Title);
	if (elfConfiguration.use6845)
		mc6845Pointer->SetTitle("Elf - MC6845"+Title);
	if (elfConfiguration.useS100)
		mc6845Pointer->SetTitle("Elf - Quest Super Video"+Title);
	if (elfConfiguration.use6847)
		mc6847Pointer->SetTitle("Elf - MC6847"+Title);
	if (elfConfiguration.use8275)
		i8275Pointer->SetTitle("Elf - Intel 8275"+Title);
    if (elfConfiguration.vtType == VT52)
		vtPointer->SetTitle("Elf - VT 52"+Title);
    if (elfConfiguration.vtType == VT100)
        vtPointer->SetTitle("Elf - VT 100"+Title);
	if (elfConfiguration.useLedModule)
		ledModulePointer->SetTitle("Led Module"+Title);
	if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
		keypadPointer->SetTitle("Keypad"+Title);
}

void Elf::setForceUpperCase(bool status)
{
	if (elfConfiguration.vtType != VTNONE)
		vtPointer->setForceUCVt(status);
	if (elfConfiguration.useKeyboard)
		setForceUpperCaseKeyboard(status);
}

void Elf::onReset()
{
	resetPressed_ = true;
}

void Elf::sleepComputer(long ms)
{
    threadPointer->Sleep(ms);
}

void Elf::pauseComputer()
{
    threadPointer->Pause();
}

void Elf::resumeComputer()
{
    threadPointer->Resume();
}

Byte Elf::read8275CharRom(Word addr)
{
	if (elfConfiguration.use8275)
		return i8275Pointer->read8275CharRom(addr);
	else
		return 0;
}

Byte Elf::read8275VideoRam(Word addr)
{
    if (elfConfiguration.use8275)
        return i8275Pointer->read8275VideoRam(addr);
    else
        return 0;
}

void Elf::write8275VideoRam(Word addr, Byte value)
{
    if (elfConfiguration.use8275)
        i8275Pointer->write8275VideoRam(addr, value);
}

void Elf::write8275CharRom(Word addr, Byte value)
{
	if (elfConfiguration.use8275)
		i8275Pointer->write8275CharRom(addr, value);
}

Byte Elf::read6845CharRom(Word addr)
{
	if (elfConfiguration.use6845||elfConfiguration.useS100)
		return mc6845Pointer->read6845CharRom(addr);
	else
		return 0;
}

void Elf::write6845CharRom(Word addr, Byte value)
{
	mc6845Pointer->write6845CharRom(addr, value);
}

Byte Elf::read6847CharRom(Word addr)
{
	if (elfConfiguration.use6847)
		return mc6847Pointer->read6847CharRom(addr);
	else
		return 0;
}

void Elf::write6847CharRom(Word addr, Byte value)
{
	if (elfConfiguration.use6847)
		mc6847Pointer->write6847CharRom(addr, value);
}

int Elf::readDirect6847(Word addr)
{
	if (elfConfiguration.use6847)
		return mc6847Pointer->readDirect6847(addr); 
	else
		return 0;
}

void Elf::writeDirect6847(Word addr, int value)
{
	if (elfConfiguration.use6847)
		mc6847Pointer->writeDirect6847(addr, value); 
}

Word Elf::get6847RamMask()
{
 	if (elfConfiguration.use6847)
		return mc6847Pointer->get6847RamMask();
	else
		return 0;
}

void Elf::setLedMs(long ms)
{
    elfScreenPointer->setLedMs(ms);
    if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
        keypadPointer->setLedMs(ms);
    if (elfConfiguration.useLedModule && !ledModuleClosed_)
        ledModulePointer->setLedMs(ms);
    if (ms == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((elfClockSpeed_ * 1000000) / 8) / 1000) * ms;
    ledCycleValue_ = ledCycleSize_;
}

Byte Elf::getKey(Byte vtOut)
{
	Byte tempVtOut = elfScreenPointer->getKey(vtOut);

	if (tempVtOut == vtOut)
	{
	if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
			tempVtOut = keypadPointer->getKey(vtOut);
		if (tempVtOut == vtOut)
		{
			if (elfConfiguration.useLedModule && !ledModuleClosed_)
				tempVtOut = ledModulePointer->getKey(vtOut);
		}
	}
	return tempVtOut;
}

void Elf::activateMainWindow()
{
	bool maximize = IsMaximized();
	Iconize(false);
	Raise();
	Show(true);
	Maximize(maximize);
}

void Elf::releaseButtonOnScreen(HexButton* buttonPointer, int WXUNUSED(buttonType))
{
    if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
        keypadPointer->releaseButtonOnScreen(buttonPointer);
}

void Elf::onNumberKeyDown(int i)
{
    if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
        keypadPointer->onNumberKeyDown(i);
}

