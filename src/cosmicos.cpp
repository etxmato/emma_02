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

#if defined(__linux__)
#include "app_icon.xpm"
#endif

#include "main.h"
#include "pushbutton.h"

#include "cosmicos.h"

CosmicosScreen::CosmicosScreen(wxWindow *parent, const wxSize& size)
: Panel(parent, size)
{
}

CosmicosScreen::~CosmicosScreen()
{
	delete mainBitmapPointer;

	delete mpSwitchButton;
 	delete ramSwitchButton;

#if defined (__WXMAC__)
    delete osx_stepButtonPointer;
    delete osx_clearButtonPointer;
    delete osx_enterButtonPointer;
    delete osx_text_loadButtonPointer;
    delete osx_text_resetButtonPointer;
    delete osx_pauseButtonPointer;
    delete osx_text_runButtonPointer;
    for (int i=0; i<8; i++)
    {
        delete ledPointer[i];
        delete osx_text_dataSwitchPointer[i];
    }
#else
    delete stepButtonPointer;
    delete clearButtonPointer;
    delete enterButtonPointer;
    delete text_loadButtonPointer;
    delete text_resetButtonPointer;
    delete pauseButtonPointer;
    delete text_runButtonPointer;
    for (int i=0; i<8; i++)
    {
        delete ledPointer[i];
        delete text_dataSwitchPointer[i];
    }
#endif
 	delete loadLedPointer;
 	delete resetLedPointer;
 	delete pauseLedPointer;
 	delete runLedPointer;

	for (int i=0; i<2; i++)
	{
		delete dataTil313Pointer[i];
	}
 	delete qLedPointer;
}

void CosmicosScreen::init()
{
	keyStart_ = 0;
	keyEnd_ = 0;
	lastKey_ = 0;
	forceUpperCase_ = p_Main->getUpperCase(COSMICOS);

	wxClientDC dc(this);
	wxString switchNumber;

    int offsetX = 10;
    int offsetY = 8;
    
#if defined(__linux__)
	wxFont defaultFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	SetFont(defaultFont);
#endif
#if defined(__WXMAC__)
	wxFont defaultFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	SetFont(defaultFont);
#endif

    mpSwitchButton = new SwitchButton(dc, HORIZONTAL_BUTTON, wxColour(255, 255, 255), BUTTON_LEFT, offsetX, offsetY+10, "");
    ramSwitchButton = new SwitchButton(dc, HORIZONTAL_BUTTON, wxColour(255, 255, 255), BUTTON_LEFT, offsetX+140, offsetY+10, "");
    
#if defined (__WXMAC__)
    osx_stepButtonPointer = new HexButton(dc, COSMICOS_HEX_BUTTON, offsetX, offsetY+40, "S");
    osx_clearButtonPointer = new HexButton(dc, COSMICOS_HEX_BUTTON, offsetX, offsetY+70, "C");
    osx_enterButtonPointer = new HexButton(dc, COSMICOS_HEX_BUTTON, offsetX, offsetY+100, "E");
    osx_text_resetButtonPointer = new HexButton(dc, COSMICOS_HEX_BUTTON, offsetX+70+30*(7-0), offsetY+40, "R");
    osx_pauseButtonPointer = new HexButton(dc, COSMICOS_HEX_BUTTON, offsetX+70+30*(7-1), offsetY+40, "P");
    osx_text_loadButtonPointer = new HexButton(dc, COSMICOS_HEX_BUTTON, offsetX+70+30*(7-2), offsetY+40, "L");
    osx_text_runButtonPointer = new HexButton(dc, COSMICOS_HEX_BUTTON, offsetX+70+30*(7-3), offsetY+40, "G");
    for (int i=0; i<8; i++)
    {
        switchNumber.Printf("%i", i);
        ledPointer[i] = new Led(dc, offsetX+77+30*(7-i), offsetY+80, COSMICOSLED);
        osx_text_dataSwitchPointer[i] = new HexButton(dc, COSMICOS_HEX_BUTTON, offsetX+70+30*(7-i), offsetY+100, switchNumber);
        if (i == 3)
            offsetX -= 10;
    }
#else
    stepButtonPointer = new wxButton(this, 9, "S", wxPoint(offsetX, offsetY+40), wxSize(25, 25), 0, wxDefaultValidator, "SSButton");
    stepButtonPointer->SetToolTip("Single Step");
    clearButtonPointer = new wxButton(this, 7, "C", wxPoint(offsetX, offsetY+70), wxSize(25, 25), 0, wxDefaultValidator, "ClearButton");
    clearButtonPointer->SetToolTip("Clear");
    enterButtonPointer = new PushButton(this, 8, "E", wxPoint(offsetX, offsetY+100), wxSize(25, 25), 0);
    enterButtonPointer->SetToolTip("Enter");

    text_resetButtonPointer = new wxButton(this, 5, "R", wxPoint(offsetX+70+30*(7-0), offsetY+40), wxSize(25, 25), 0, wxDefaultValidator, "ResetButton");
    text_resetButtonPointer->SetToolTip("Reset");
    pauseButtonPointer = new wxButton(this, 6, "P", wxPoint(offsetX+70+30*(7-1), offsetY+40), wxSize(25, 25), 0, wxDefaultValidator, "PauseButton");
    pauseButtonPointer->SetToolTip("Pause - Stop");
    text_loadButtonPointer = new wxButton(this, 4, "L", wxPoint(offsetX+70+30*(7-2), offsetY+40), wxSize(25, 25), 0, wxDefaultValidator, "LoadButton");
    text_loadButtonPointer->SetToolTip("Load");
    text_runButtonPointer = new wxButton(this, 1, "G", wxPoint(offsetX+70+30*(7-3), offsetY+40), wxSize(25, 25), 0, wxDefaultValidator, "RunButton");
    text_runButtonPointer->SetToolTip("Go - RUN");
    for (int i=0; i<8; i++)
    {
        switchNumber.Printf("%i", i);
        ledPointer[i] = new Led(dc, offsetX+77+30*(7-i), offsetY+80, COSMICOSLED);
        text_dataSwitchPointer[i] = new wxButton(this, i+10, switchNumber, wxPoint(offsetX+70+30*(7-i), offsetY+100), wxSize(25, 25));
        if (i == 3)
            offsetX -= 10;
    }
#endif
    offsetX = 10;
	resetLedPointer = new Led(dc, offsetX+77+30*(7-0), offsetY+20, COSMICOSLED);
	pauseLedPointer = new Led(dc, offsetX+77+30*(7-1), offsetY+20, COSMICOSLED);
	loadLedPointer = new Led(dc, offsetX+77+30*(7-2), offsetY+20, COSMICOSLED);
	runLedPointer = new Led(dc, offsetX+77+30*(7-3), offsetY+20, COSMICOSLED);

	offsetX += 10;
	for (int i=0; i<2; i++)
	{
		dataTil313Pointer[i] = new Til313();
		dataTil313Pointer[i]->init(dc, offsetX+45+i*28, offsetY+20);
	}
	qLedPointer = new Led(dc, offsetX+100, offsetY+20, COSMICOSLED);

	mainBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/cosmicos.png", wxBITMAP_TYPE_PNG);

	this->connectKeyEvent(this);
}

void CosmicosScreen::onPaint(wxPaintEvent&WXUNUSED(event))
{
	wxPaintDC dc(this);
	dc.DrawBitmap(*mainBitmapPointer, 0, 0);

#if defined(__WXMAC__)
    rePaintLeds(dc);
#endif

	for (int i=0; i<8; i++)
	{
		ledPointer[i]->onPaint(dc);
	}
	for (int i=0; i<2; i++)
	{
		dataTil313Pointer[i]->onPaint(dc);
	}
	runLedPointer->onPaint(dc);
	loadLedPointer->onPaint(dc);
	pauseLedPointer->onPaint(dc);
	resetLedPointer->onPaint(dc);
	qLedPointer->onPaint(dc);
	mpSwitchButton->onPaint(dc);
	ramSwitchButton->onPaint(dc);
#if defined (__WXMAC__)
    osx_text_resetButtonPointer->onPaint(dc);
    osx_text_loadButtonPointer->onPaint(dc);
    osx_text_runButtonPointer->onPaint(dc);
    osx_pauseButtonPointer->onPaint(dc);
    osx_stepButtonPointer->onPaint(dc);
    osx_clearButtonPointer->onPaint(dc);
    osx_enterButtonPointer->onPaint(dc);
    for (int i=0; i<8; i++)
        osx_text_dataSwitchPointer[i]->onPaint(dc);
#endif
}

void CosmicosScreen::onMouseRelease(wxMouseEvent&event)
{
	int x, y;
	event.GetPosition(&x, &y);

	wxClientDC dc(this);

	if (mpSwitchButton->onMouseRelease(dc, x, y))
		p_Computer->onMpButton();
	if (ramSwitchButton->onMouseRelease(dc, x, y))
		p_Computer->onRamButton();
#if defined (__WXMAC__)
    osx_text_resetButtonPointer->onMouseRelease(dc, x, y);
    osx_text_loadButtonPointer->onMouseRelease(dc, x, y);
    osx_text_runButtonPointer->onMouseRelease(dc, x, y);
    osx_pauseButtonPointer->onMouseRelease(dc, x, y);
    osx_stepButtonPointer->onMouseRelease(dc, x, y);
    osx_clearButtonPointer->onMouseRelease(dc, x, y);
    if (osx_enterButtonPointer->onMouseRelease(dc, x, y))
        p_Computer->onInButtonRelease();

    for (int i=0; i<8; i++)
        osx_text_dataSwitchPointer[i]->onMouseRelease(dc, x, y);
#endif
}

void CosmicosScreen::onMousePress(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);
    
    wxClientDC dc(this);
    
#if defined (__WXMAC__)
    if (osx_text_resetButtonPointer->onMousePress(dc, x, y))
        p_Computer->onResetButton();
    
    if (osx_text_loadButtonPointer->onMousePress(dc, x, y))
        p_Computer->onLoadButton();
    
    if (osx_text_runButtonPointer->onMousePress(dc, x, y))
        p_Computer->onRunButton();
    
    if (osx_pauseButtonPointer->onMousePress(dc, x, y))
        p_Computer->onPause();
    
    if (osx_stepButtonPointer->onMousePress(dc, x, y))
        p_Computer->onSingleStep();
    
    if (osx_clearButtonPointer->onMousePress(dc, x, y))
        p_Computer->onClearButton();
    
    if (osx_enterButtonPointer->onMousePress(dc, x, y))
        p_Computer->onInButtonPress();
    
    for (int i=0; i<8; i++)
    {
        if (osx_text_dataSwitchPointer[i]->onMousePress(dc, x, y))
            p_Computer->dataButton(i);
    }
#endif
}

void CosmicosScreen::releaseButtonOnScreen(HexButton* buttonPoint)
{
    wxClientDC dc(this);
    
    buttonPoint->releaseButtonOnScreen(dc);
}

BEGIN_EVENT_TABLE(Cosmicos, wxFrame)
	EVT_CLOSE (Cosmicos::onClose)
	EVT_BUTTON(1, Cosmicos::onRunButton)
	EVT_BUTTON(4, Cosmicos::onLoadButton)
	EVT_BUTTON(5, Cosmicos::onResetButton)
	EVT_BUTTON(6, Cosmicos::onPause)
	EVT_BUTTON(7, Cosmicos::onClearButton)
	EVT_COMMAND(8, wxEVT_ButtonDownEvent, Cosmicos::onButtonPress)
	EVT_COMMAND(8, wxEVT_ButtonUpEvent, Cosmicos::onButtonRelease)
	EVT_BUTTON(9, Cosmicos::onSingleStep)

	EVT_BUTTON(10, Cosmicos::dataButton)
	EVT_BUTTON(11, Cosmicos::dataButton)
	EVT_BUTTON(12, Cosmicos::dataButton)
	EVT_BUTTON(13, Cosmicos::dataButton)
	EVT_BUTTON(14, Cosmicos::dataButton)
	EVT_BUTTON(15, Cosmicos::dataButton)
	EVT_BUTTON(16, Cosmicos::dataButton)
	EVT_BUTTON(17, Cosmicos::dataButton)

	EVT_LEFT_UP(Cosmicos::onMouseRelease)
END_EVENT_TABLE()

Cosmicos::Cosmicos(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf, Conf computerConf)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    computerConfiguration = computerConf;
	cosmicosConfiguration = conf;

	cosmicosClockSpeed_ = clock;
	data_ = 0;
	lastAddress_ = 0;

#ifndef __WXMAC__
	SetIcon(wxICON(app_icon));
#endif

	this->SetClientSize(size);

	cosmicosScreenPointer = new CosmicosScreen(this, size);
	cosmicosScreenPointer->init();

	cycleSize_ = (int)(1000 / ((1/1.75) * 8)); // ~1000 Hz on 1.75 CPU
	cycleValue_ = cycleSize_;

    bootstrap_ = 0;
}

Cosmicos::~Cosmicos()
{
    switch (loadedProgram_)
    {
        case HEXMON:
        case NOPROGRAM:
            p_Main->saveScrtValues("");
        break;
            
        case ASCIIMON:
            p_Main->saveScrtValues("ASCIIMON");
        break;
    }
	saveRam();
	if (cosmicosConfiguration.usePixie)
	{
		p_Main->setPixiePos(COSMICOS, pixiePointer->GetPosition());
		pixiePointer->Destroy();
	}
	if (cosmicosConfiguration.vtType != VTNONE)
	{
		p_Main->setVtPos(COSMICOS, vtPointer->GetPosition());
		vtPointer->Destroy();
	}
    if (cosmicosConfiguration.vtExternal)
        delete p_Serial;
	p_Main->setMainPos(COSMICOS, GetPosition());

	if (cosmicosConfiguration.useHex)
	{
		p_Main->setKeypadPos(COSMICOS, p_Cosmicoshex->GetPosition());
	}
	p_Cosmicoshex->Destroy();
	delete cosmicosScreenPointer;
}

void Cosmicos::onClose(wxCloseEvent&WXUNUSED(event) )
{
	p_Main->stopComputer();
}

bool Cosmicos::keyDownPressed(int key)
{
	if (cosmicosConfiguration.useHexKeyboardEf3)
		onHexKeyDown(key);
	return false;
}

bool Cosmicos::keyUpReleased(int key)
{
	if (key == inKey1_ || key == inKey2_)
	{
		onInButtonRelease();
		return true;
	}
	if (cosmicosConfiguration.useHexKeyboardEf3)
		onHexKeyUp(key);
	return false;
}

void Cosmicos::onButtonRelease(wxCommandEvent& event)
{
	onInButtonRelease();
	event.Skip();
}

void Cosmicos::onButtonPress(wxCommandEvent& event)
{
	onInButtonPress(getData());
	event.Skip();
}

void Cosmicos::onInButtonPress()
{
	onInButtonPress(getData());
}

void Cosmicos::onHexKeyDown(int keycode)
{
	for (int i=0; i<16; i++)
	{
        if (keycode == keyDefA1_[i] || keycode == keyDefA2_[i])
            p_Cosmicoshex->onNumberKeyPress(i);
	}
}

void Cosmicos::onHexKeyUp(int keycode)
{
	for (int i=0; i<16; i++)
	{
        if (keycode == keyDefA1_[i] || keycode == keyDefA2_[i])
			p_Cosmicoshex->onNumberKeyRelease(i);
	}
}

void Cosmicos::onPause(wxCommandEvent&WXUNUSED(event))
{
    onPause();
}

void Cosmicos::onPause()
{
    singleStateStep_ = true;
	setClear(1);
	setWait(0);
	pixieOn_ = false;
	p_Main->eventUpdateTitle();
}

void Cosmicos::onSingleStep(wxCommandEvent&WXUNUSED(event))
{
    onSingleStep();
}

void Cosmicos::onSingleStep()
{
    setWait(1);
}

void Cosmicos::onResetButton(wxCommandEvent&WXUNUSED(event))
{
    onResetButton();
}

void Cosmicos::onResetButton()
{
    singleStateStep_ = false;
	lastMode_ = UNDEFINDEDMODE;
	setClear(0);
	setWait(1);
	segNumber_ = 0;
	pixieOn_ = false;
}

void Cosmicos::onRunButton(wxCommandEvent&WXUNUSED(event))
{
    onRunButton();
}

void Cosmicos::onRunButton()
{
    singleStateStep_ = false;
    resetEffectiveClock();
	setClear(1);
	setWait(1);
	p_Main->eventUpdateTitle();
}

void Cosmicos::onMouseRelease(wxMouseEvent&event)
{
	cosmicosScreenPointer->onMouseRelease(event);
}

void Cosmicos::onRun() 
{
	stopTape();
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
		pixieOn_ = false;
		p_Main->eventUpdateTitle();
	}
}

void Cosmicos::autoBoot()
{
	runButtonState_ = 1;

	setClear(runButtonState_);
	segNumber_ = 0;
}

Byte Cosmicos::inPressed()
{
	return !inPressed_;
}

void Cosmicos::onInButtonPress(Byte value)
{
	if (inPressed_ == 1)  return;
	inPressed_ = 1;
	if (getCpuMode() == LOAD)
	{
		dmaIn(value);
		showData(value);
		for (int i=0; i<8; i++)
		{
			dataSwitchState_[i] = 0;
			cosmicosScreenPointer->setLed(i,0);
		}
	}
}

void Cosmicos::onInButtonRelease()
{
	if (inPressed_ == 0)  return;
	inPressed_ = 0;
}

void Cosmicos::onLoadButton(wxCommandEvent&WXUNUSED(event))
{
    onLoadButton();
}

void Cosmicos::onLoadButton()
{
	setClear(0);
	setWait(0);
	segNumber_ = 0;
	if (cosmicosConfiguration.usePixie)
	{
		pixiePointer->outPixie();
		pixieOn_ = false;
	}
}

void Cosmicos::onMpButton()
{
	if (mpButtonState_)
		mpButtonState_ = 0;
	else
		mpButtonState_ = 1;
}

void Cosmicos::onRamButton()
{
	if (ramButtonState_)
		ramButtonState_ = 0;
	else
		ramButtonState_ = 1;
}

void Cosmicos::dataButton(wxCommandEvent&event)
{
    int i;
    
    i = event.GetId() - 10;
    
    dataButton(i);
}

void Cosmicos::dataButton(int i)
{
	if (dataSwitchState_[i])
		dataSwitchState_[i] = 0;
	else
		dataSwitchState_[i] = 1;

	cosmicosScreenPointer->setLed(i,dataSwitchState_[i]);
}

void Cosmicos::onNumberKeyPress(int key)
{
    p_Cosmicoshex->onNumberKeyPress(key);
}

void Cosmicos::onNumberKeyRelease(int key)
{
    p_Cosmicoshex->onNumberKeyRelease(key);
}

void Cosmicos::onClearButton(wxCommandEvent&WXUNUSED(event))
{
    onClearButton();
}

void Cosmicos::onClearButton()
{
    for (int i=0; i<8; i++)
    {
        dataSwitchState_[i] = 0;
        cosmicosScreenPointer->setLed(i,0);
    }
}
        
Byte Cosmicos::getData()
{
	return(dataSwitchState_[7]?128:0) +(dataSwitchState_[6]?64:0) +(dataSwitchState_[5]?32:0) +(dataSwitchState_[4]?16:0) +(dataSwitchState_[3]?8:0) +(dataSwitchState_[2]?4:0) +(dataSwitchState_[1]?2:0) +(dataSwitchState_[0]?1:0);
}

void Cosmicos::configureComputer()
{
	inType_[7] = COSMICOSIN;
	outType_[7] = COSMICOSOUT;

    setCycleType(COMPUTERCYCLE, LEDCYCLE);
	p_Main->message("Configuring Cosmicos");
	p_Main->message("	Output 7: display output, input 7: data input");
	if (cosmicosConfiguration.useHexKeyboardEf3 || cosmicosConfiguration.useHex)
	{
		efType_[1] = COSMICOSRET;
		efType_[2] = COSMICOSDEC;
		efType_[3] = COSMICOSREQ;
		inType_[5] = COSMICOSHEX;
		inType_[6] = COSMICOS7SEG;
		outType_[5] = COSMICOSHEX;
		outType_[6] = COSMICOS7SEG;
		cycleType_[5] = COSMICOS7SEG;
		p_Main->message("	Output 5: select hex row, input 5: hex column");
		p_Main->message("	Output 6: 7 segment display, input 6: reset 7 segment");
		p_Main->message("	EF 1: hex RET, EF2: hex DEC/SEQ, EF3: hex REQ/SEQ");
	}
	if (cosmicosConfiguration.vtType == VTNONE)
	{
		efType_[4] = COSMICOSEF;
		p_Main->message("	EF 4: 0 when in button pressed");
	}
	p_Main->message("");

    inKey1_ = p_Main->getDefaultInKey1("Cosmicos");
    inKey2_ = p_Main->getDefaultInKey2("Cosmicos");
    p_Main->getDefaultHexKeys(COSMICOS, "Cosmicos", "A", keyDefA1_, keyDefA2_, keyDefGameHexA_);

	resetCpu();
	runButtonState_ = 0;
	ramButtonState_ = 0;
	for (int i=0; i<8; i++)  dataSwitchState_[i]=0;

	resetCpu();
}

void Cosmicos::reDefineKeysA(int hexKeyDefA1[], int hexKeyDefA2[])
{
	for (int i=0; i<16; i++)
	{
        keyDefA1_[i] = hexKeyDefA1[i];
        keyDefA2_[i] = hexKeyDefA2[i];
	}
}

void Cosmicos::initComputer()
{
	Show(p_Main->getUseElfControlWindows(COSMICOS));
	lastMode_ = UNDEFINDEDMODE;

	qState_ = flipFlopQ_ + 1;
	switches_ = 0;
	inPressed_ = 0;
	segNumber_ = 0;
	cassetteEf_ = 0;
	pixieOn_ = false;
}

Byte Cosmicos::ef(int flag)
{
	switch(efType_[flag])
	{
		case 0:
			return 1;
		break;

		case VT100EF:
			return vtPointer->ef();
		break;

        case VTSERIALEF:
            return p_Serial->ef();
        break;
 
		case COSMICOSEF:
			return inPressed();
		break;

		case COSMICOSREQ:
			if (pixieOn_ && cosmicosConfiguration.usePixie)
			{
				return pixiePointer->efPixie();
			}
			else
			{
				if (cosmicosConfiguration.useHexKeyboardEf3 || cosmicosConfiguration.useHex)
					return (p_Cosmicoshex->reqState() & p_Cosmicoshex->seqState());
			}
			return 1;
		break;

		case COSMICOSDEC:
			if (isLoading() || realCassetteLoad_)
				return cassetteEf_;
			else
				return (p_Cosmicoshex->decState() & p_Cosmicoshex->seqState());
		break;

		case COSMICOSRET:
			return p_Cosmicoshex->retState();
		break;

		default:
			return 1;
	}
}

Byte Cosmicos::in(Byte port, Word WXUNUSED(address))
{
	Byte ret;
	ret = 0;

	switch(inType_[port])
	{
		case 0:
			ret = 255;
		break;

		case PIXIEIN:
			if (qState_ == 0)
			{
				ret = pixiePointer->inPixie();
				pixieOn_ = true;
			}
		break;

		case PIXIEOUT:
			if (qState_ == 0)
			{
				pixiePointer->outPixie();
				pixieOn_ = false;
			}
		break;

		case COSMICOSIN:
			ret = getData();
		break;

		case COSMICOSHEX:
			ret = p_Cosmicoshex->getKeyinput();
		break;

		case COSMICOS7SEG:
			segNumber_ = 0;
			ret = 255;
		break;

		default:
			ret = 255;
	}
	inValues_[port] = ret;
	return ret;
}

void Cosmicos::out(Byte port, Word WXUNUSED(address), Byte value)
{
	outValues_[port] = value;

	switch(outType_[port])
	{
		case 0:
			return;
		break;

		case VT100OUT:
			vtPointer->out(value);
		break;

		case VTOUTSERIAL:
			p_Serial->out(value);
		break;

		case COSMICOSOUT:
			showData(value);
		break;

		case COSMICOSHEX:
			p_Cosmicoshex->keyLatch(value);
		break;

		case COSMICOS7SEG:
			p_Cosmicoshex->segUpdate(segNumber_, value);
			segNumber_++;
			segNumber_&=0x7;
		break;

		case COSMICOSTONE:
			if (qState_ == 1)
				tone1864Latch(value);
		break;
	}
}

void Cosmicos::switchQ(int value)
{
	qState_ = value;
	cosmicosScreenPointer->setQLed(value);

	if (bootstrap_ != 0 && flipFlopQ_ == 1)
		bootstrap_ = 0;
    
    if (cosmicosConfiguration.vtType != VTNONE)
        vtPointer->switchQ(value);

    if (cosmicosConfiguration.vtExternal)
        p_Serial->switchQ(value);
}

void Cosmicos::showData(Byte val)
{
	cosmicosScreenPointer->showDataTil313(val);
}

void Cosmicos::cycle(int type)
{
	switch(cycleType_[type])
	{
		case 0:
			return;
		break;

		case PIXIECYCLE:
			pixiePointer->cyclePixieTelmac();
		break;

		case COSMICOS7SEG:
            cycleValue_ --;
            if (cycleValue_ <= 0)
            {
                if (!pixieOn_)
                    interrupt();
                cycleValue_ = cycleSize_;
            }
		break;

		case VT100CYCLE:
			vtPointer->cycleVt();
		break;
            
        case VTSERIALCYCLE:
            p_Serial->cycleVt();
        break;

        case LEDCYCLE:
            cycleLed();
        break;
    }
}

void Cosmicos::cycleLed()
{
    if (ledCycleValue_ > 0)
    {
        ledCycleValue_ --;
        if (ledCycleValue_ <= 0)
        {
            ledCycleValue_ = ledCycleSize_;
            cosmicosScreenPointer->ledTimeout();
            p_Cosmicoshex->ledTimeout();
        }
    }
}

void Cosmicos::startComputer()
{
	resetPressed_ = false;

	p_Cosmicoshex = new Cosmicoshex("Hex Panel", p_Main->getKeypadPos(COSMICOS), wxSize(185, 160));
	p_Cosmicoshex->Show(cosmicosConfiguration.useHex);

	if (p_Main->getRamType(COSMICOS)==0)
	{
		mainRamAddress_ = 0;
		defineMemoryType(0, 0xff, RAM);
        initRam(0, 0xff);
		cosmicosScreenPointer->mpSetState(BUTTON_LEFT);
		mpButtonState_ = 0;
        p_Main->assDefault("mycode", 0, 0xA0);
	}
	else
	{
		mainRamAddress_ = 0xFF00;
		defineMemoryType(0, p_Main->getRamType(COSMICOS)*0x400-1, RAM);
        initRam(0, p_Main->getRamType(COSMICOS)*0x400-1);
		defineMemoryType(0xff00, 0xffff, MAPPEDRAM);
		cosmicosScreenPointer->mpSetState(BUTTON_RIGHT);
		mpButtonState_ = 1;
        
        if (p_Main->getRamType(COSMICOS) == 4)
            p_Main->assDefault("mycode", 0, 0xEFF);
        else
            p_Main->assDefault("mycode", 0, 0xFFF);
	}
	
    p_Main->checkAndReInstallMainRom(COSMICOS);
	readProgram(p_Main->getRomDir(COSMICOS, MAINROM1), p_Main->getRomFile(COSMICOS, MAINROM1), ROM, 0xc000, NONAME);
	loadRam();

	configureElfExtensions();
	if (cosmicosConfiguration.autoBoot)
	{
		bootstrap_ = 0xC0C0;
		autoBoot();
	}
	else
		bootstrap_ = 0;

	if (cosmicosConfiguration.vtType != VTNONE)
		setEf(4,0);

	p_Main->setSwName("");
	p_Main->updateTitle();

	cpuCycles_ = 0;
	instructionCounter_= 0;
	p_Main->startTime();

    int ms = (int) p_Main->getLedTimeMs(COSMICOS);
    cosmicosScreenPointer->setLedMs(ms);
    p_Cosmicoshex->setLedMs(ms);
    if (ms == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((cosmicosClockSpeed_ * 1000000) / 8) / 1000) * ms;
    ledCycleValue_ = ledCycleSize_;
    
	threadPointer->Run();
}

void Cosmicos::writeMemDataType(Word address, Byte type)
{
	address = address | bootstrap_;
	switch (memoryType_[address/256]&0xff)
	{
		case ROM:
			if (mainMemoryDataType_[address] != type)
			{
				p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
				mainMemoryDataType_[address] = type;
			}
            increaseExecutedMainMemory(address, type);
		break;

		case RAM:
		case MAPPEDRAM:
			if (mainMemoryDataType_[address | bootstrap_] != type)
			{
				p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
				mainMemoryDataType_[address | bootstrap_] = type;
			}
            increaseExecutedMainMemory(address | bootstrap_, type);
		break;
	}
}

Byte Cosmicos::readMemDataType(Word address, uint64_t* executed)
{
	address = address | bootstrap_;
	switch (memoryType_[address/256]&0xff)
	{
		case RAM:
		case ROM:
		case MAPPEDRAM:
            if (profilerCounter_ != PROFILER_OFF)
                *executed = mainMemoryExecuted_[address | bootstrap_];
			return mainMemoryDataType_[address | bootstrap_];
		break;
	}
	return MEM_TYPE_UNDEFINED;
}

Byte Cosmicos::readMem(Word address)
{
	address_ = address | bootstrap_;
    return readMemDebug(address_);
}

Byte Cosmicos::readMemDebug(Word address)
{
    address = address | bootstrap_;
    switch (memoryType_[address / 256]&0xff)
    {
        case UNDEFINED:
            return 255;
        break;
            
        case ROM:
        case RAM:
        case MAPPEDRAM:
            return mainMemory_[address];
        break;
            
        default:
            return 255;
        break;
    }
}

void Cosmicos::writeMem(Word address, Byte value, bool writeRom)
{
	address_ = address | bootstrap_;
    writeMemDebug(address_, value, writeRom);
}

void Cosmicos::writeMemDebug(Word address, Byte value, bool writeRom)
{
    address = address | bootstrap_;
    switch (memoryType_[address/256]&0xff)
    {
        case UNDEFINED:
        case ROM:
            if (writeRom)
                mainMemory_[address]=value;
        break;
            
        case MAPPEDRAM:
            if (mpButtonState_ == 0)
            {
                if (mainMemory_[address | bootstrap_]==value)
                    return;
                mainMemory_[address | bootstrap_]=value;
                if (address >= (memoryStart_ | bootstrap_) && address<((memoryStart_ | bootstrap_ ) +256))
                    p_Main->updateDebugMemory(address);
                p_Main->updateAssTabCheck(address);
            }
        break;
            
        case RAM:
            if (mainMemory_[address]==value)
                return;
            mainMemory_[address]=value;
            if (address >= (memoryStart_ | bootstrap_) && address<((memoryStart_ | bootstrap_ ) +256))
                p_Main->updateDebugMemory(address);
            p_Main->updateAssTabCheck(address);
        break;
    }
}

void Cosmicos::cpuInstruction()
{
	if (cpuMode_ != lastMode_)
	{
		cosmicosScreenPointer->setRunLed(0);
		cosmicosScreenPointer->setLoadLed(0);
		cosmicosScreenPointer->setResetLed(0);
		cosmicosScreenPointer->setPauseLed(0);
		switch(cpuMode_)
		{
			case LOAD: cosmicosScreenPointer->setLoadLed(1); break;
			case RESET: cosmicosScreenPointer->setResetLed(1); break;
			case RUN: cosmicosScreenPointer->setRunLed(1); break;
			case PAUSE: cosmicosScreenPointer->setPauseLed(1); break;
		}
		lastMode_ = cpuMode_;
	}

	if (cpuMode_ == RUN)
	{
        cpuCycleStep();
	}
	else
	{
        cycleLed();
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

void Cosmicos::resetPressed()
{
    pixieOn_ = false;
    resetCpu();
    initComputer();
    if (cosmicosConfiguration.autoBoot)
    {
        bootstrap_ = 0xC0C0;
        autoBoot();
    }
    else
        bootstrap_ = 0;
    resetPressed_ = false;
    p_Main->setSwName("");
    p_Main->eventUpdateTitle();
}

void Cosmicos::configureElfExtensions()
{
	wxString path, fileName1, fileName2;

	p_Computer->setSoundFollowQ(!cosmicosConfiguration.usePixie);
	if (cosmicosConfiguration.vtType != VTNONE)
	{
		double zoom = p_Main->getZoomVt();
        if (cosmicosConfiguration.vtType == VT52)
            vtPointer = new Vt100("Cosmicos - VT 52", p_Main->getVtPos(COSMICOS), wxSize(640*zoom, 400*zoom), zoom, COSMICOS, cosmicosClockSpeed_, cosmicosConfiguration, UART1);
        else
            vtPointer = new Vt100("Cosmicos - VT 100", p_Main->getVtPos(COSMICOS), wxSize(640*zoom, 400*zoom), zoom, COSMICOS, cosmicosClockSpeed_, cosmicosConfiguration, UART1);
		p_Vt100[UART1] = vtPointer;
		vtPointer->configureStandard(cosmicosConfiguration.baudR, cosmicosConfiguration.baudT, 4);
		vtPointer->Show(true);
	}

    if (cosmicosConfiguration.vtExternal)
    {
        p_Serial = new Serial(COSMICOS, cosmicosClockSpeed_, cosmicosConfiguration);
        p_Serial->configureStandard(cosmicosConfiguration.baudR, cosmicosConfiguration.baudT, 4);
    }

    if (cosmicosConfiguration.usePixie)
	{
		double zoom = p_Main->getZoom();
		double scale = p_Main->getScale();
		pixiePointer = new Pixie( "Cosmicos - CDP1864", p_Main->getPixiePos(COSMICOS), wxSize(64*zoom*scale, 192*zoom), zoom, scale, COSMICOS);
		p_Video = pixiePointer;
		pixiePointer->configurePixieCosmicos();
		pixiePointer->initPixie();
		pixiePointer->setZoom(zoom);
		pixiePointer->Show(true);
	}

}

void Cosmicos::moveWindows()
{
	if (cosmicosConfiguration.usePixie)
		pixiePointer->Move(p_Main->getPixiePos(COSMICOS));
	if (cosmicosConfiguration.vtType != VTNONE)
		vtPointer->Move(p_Main->getVtPos(COSMICOS));
	if (cosmicosConfiguration.useHex)
		p_Cosmicoshex->Move(p_Main->getKeypadPos(COSMICOS));
}

void Cosmicos::updateTitle(wxString Title)
{
	if (cosmicosConfiguration.usePixie)
		pixiePointer->SetTitle("Cosmicos - CDP1864"+Title);
    if (cosmicosConfiguration.vtType == VT52)
		vtPointer->SetTitle("Cosmicos - VT 52"+Title);
    if (cosmicosConfiguration.vtType == VT100)
        vtPointer->SetTitle("Cosmicos - VT 100"+Title);
	if (cosmicosConfiguration.useHex)
		p_Cosmicoshex->SetTitle("Hex Panel"+Title);
}

void Cosmicos::setForceUpperCase(bool status)
{
	if (cosmicosConfiguration.vtType != VTNONE)
		vtPointer->setForceUCVt(status);
}

void Cosmicos::onReset()
{
	resetPressed_ = true;
}

void Cosmicos::removeCosmicosHex() 
{
	cosmicosConfiguration.useHex = false;
	p_Main->eventSetCheckBox("HexCosmicos", false);
	p_Cosmicoshex->Show(false);
}

void Cosmicos::showModules(bool useHex)
{
	p_Cosmicoshex->Show(useHex);
}

void Cosmicos::saveRam()
{
	if (ramButtonState_ == 0)
		return;

	Byte value;
	wxFile outputFile;
	wxString fileName;

	fileName = p_Main->getMainDir()+"ramdump.bin";

	if (wxFile::Exists(fileName))
		outputFile.Open(fileName, wxFile::write);
	else
		outputFile.Create(fileName);
	for (long address = 0; address <= 0xff; address++)
	{
		value = mainMemory_[address+mainRamAddress_];
		outputFile.Write(&value, 1);
	}
	outputFile.Close();
}

void Cosmicos::loadRam()
{
	wxFFile inFile;
	size_t length;
	char buffer[0x100];

	if (wxFile::Exists(p_Main->getMainDir()+"ramdump.bin"))
	{
		if (inFile.Open(p_Main->getMainDir()+"ramdump.bin", _("rb")))
		{
			length = inFile.Read(buffer, 0x100);
			for (size_t i=0; i<length; i++)
				mainMemory_[i+mainRamAddress_] = (Byte)buffer[i];
			inFile.Close();
		}
	}
}

void Cosmicos::checkComputerFunction()
{
	switch (loadedProgram_)
	{
		case NOPROGRAM:
		break;

		case HEXMON:
			switch (scratchpadRegister_[programCounter_])
			{
				case 0xc366:	// SAVE
				case 0xc3b7:	// LOAD
					p_Main->stopCassette();
				break;

				case 0xc2f0:	// SAVE
					p_Main->startCassetteSave(0);
				break;

				case 0xc37d:	// LOAD
					p_Main->setSwName ("");
                    p_Main->eventUpdateTitle();
					p_Main->startCassetteLoad(0);
				break;
			}
		break;

		case ASCIIMON:
			switch (scratchpadRegister_[programCounter_])
			{
				case 0xc5d3:	// SAVE ASCII MON
				case 0xc6c3:	// LOAD ASCII MON
					p_Main->stopCassette();
				break;

				case 0xc571:	// SAVE ASCII MON
					p_Main->startCassetteSave(0);
				break;

				case 0xc643:	// LOAD ASCII MON
					p_Main->setSwName ("");
                    p_Main->eventUpdateTitle();
					p_Main->startCassetteLoad(0);
				break;
			}
		break;
	}
}

void Cosmicos::setLedMs(long ms)
{
    cosmicosScreenPointer->setLedMs(ms);
    p_Cosmicoshex->setLedMs(ms);
    if (ms == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((cosmicosClockSpeed_ * 1000000) / 8) / 1000) * ms;
    ledCycleValue_ = ledCycleSize_;
}

Byte Cosmicos::getKey(Byte vtOut)
{
	Byte tempVtOut = cosmicosScreenPointer->getKey(vtOut);

	if (tempVtOut == vtOut)
		return p_Cosmicoshex->getKey(vtOut);
	else
		return tempVtOut;
}

void Cosmicos::activateMainWindow()
{
	bool maximize = IsMaximized();
	Iconize(false);
	Raise();
	Show(true);
	Maximize(maximize);
}

void Cosmicos::releaseButtonOnScreen(HexButton* buttonPointer, int buttonType)
{
    if (buttonType == PANEL_HEX_BUTTON || buttonType == PANEL_WIDE_HEX_BUTTON)
        p_Cosmicoshex->releaseButtonOnScreen(buttonPointer);
    else
        cosmicosScreenPointer->releaseButtonOnScreen(buttonPointer);
}

void Cosmicos::refreshPanel()
{
    cosmicosScreenPointer->refreshPanel();
}
