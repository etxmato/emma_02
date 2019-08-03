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

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/frame.h"
#include "wx/zipstrm.h"
#include "wx/wfstream.h"
#include "wx/datstrm.h"
#include "wx/dir.h"

#include "main.h"
#include "computer.h"
#include <memory>


BEGIN_EVENT_TABLE(HexButton, wxEvtHandler )
    EVT_TIMER(wxID_ANY, HexButton::OnTimer)
END_EVENT_TABLE()

HexButton::HexButton(wxDC& dc, int type, wxCoord x, wxCoord y, wxString label)
{
    wxBitmap *upBitmap;
    wxBitmap *downBitmap;
    wxSize buttonSize;
    
    buttonType_ = type;
    switch (buttonType_)
    {
        case ELF2K_RESET_BUTTON:
            upBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/resetButtonUp.png", wxBITMAP_TYPE_PNG);
            downBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/resetButtonDown.png", wxBITMAP_TYPE_PNG);
            buttonSize.x = 30;
            buttonSize.y = 30;
        break;
            
        case COSMICOS_HEX_BUTTON:
            upBitmap = new wxBitmap (p_Main->getApplicationDir() + IMAGES_FOLDER + "/hex_button_small.png", wxBITMAP_TYPE_PNG);
            downBitmap = new wxBitmap (p_Main->getApplicationDir() + IMAGES_FOLDER + "/hex_button_small_pushed.png", wxBITMAP_TYPE_PNG);
            buttonSize.x = 26;
            buttonSize.y = 26;
        break;
            
        case PANEL_WIDE_HEX_BUTTON:
            upBitmap = new wxBitmap (p_Main->getApplicationDir() + IMAGES_FOLDER + "/hex_button_wide.png", wxBITMAP_TYPE_PNG);
            downBitmap = new wxBitmap (p_Main->getApplicationDir() + IMAGES_FOLDER + "/hex_button_wide_pushed.png", wxBITMAP_TYPE_PNG);
            buttonSize.x = 34;
            buttonSize.y = 30;
        break;
            
        case ELF_HEX_BUTTON:
        case PANEL_HEX_BUTTON:
        default:
            upBitmap = new wxBitmap (p_Main->getApplicationDir() + IMAGES_FOLDER + "/hex_button.png", wxBITMAP_TYPE_PNG);
            downBitmap = new wxBitmap (p_Main->getApplicationDir() + IMAGES_FOLDER + "/hex_button_pushed.png", wxBITMAP_TYPE_PNG);
            buttonSize.x = 30;
            buttonSize.y = 30;
        break;
   }
    upBitmapPointer = new wxBitmap(buttonSize);
    downBitmapPointer = new wxBitmap(buttonSize);
    
    wxMemoryDC memDC(*upBitmapPointer);
    wxSize labelSize = memDC.GetTextExtent(label);
    wxCoord xPosition = (buttonSize.x - labelSize.x)/2;
    wxCoord yPosition = (buttonSize.y - labelSize.y)/2;
    memDC.Clear();
    memDC.DrawBitmap(*upBitmap, 0, 0);
    if (buttonType_ == ELF2K_RESET_BUTTON)
        memDC.SetTextForeground(*wxWHITE);
    memDC.DrawText(label, xPosition, yPosition);
    memDC.SelectObject(wxNullBitmap);
    
    memDC.SelectObject(*downBitmapPointer);
    memDC.Clear();
    memDC.DrawBitmap(*downBitmap, 0, 0);
    if (buttonType_ == ELF2K_RESET_BUTTON)
        memDC.SetTextForeground(*wxWHITE);
    memDC.DrawText(label, xPosition, yPosition);
    memDC.SelectObject(wxNullBitmap);
    
    delete upBitmap;
    delete downBitmap;
    
    x_ = x;
    y_ = y;
    
    state_ = BUTTON_UP;
    focusTimer = new wxTimer(this, wxID_ANY);

    dc.DrawBitmap(*upBitmapPointer, x, y);
}

HexButton::~HexButton()
{
    delete upBitmapPointer;
    delete downBitmapPointer;
    delete focusTimer;
}

void HexButton::onPaint(wxDC& dc)
{
    if (state_ == BUTTON_UP)
    {
        dc.DrawBitmap(*upBitmapPointer, x_, y_);
    }
    else
    {
        dc.DrawBitmap(*downBitmapPointer, x_, y_);
    }
}

bool HexButton::onMousePress(wxDC& dc, wxCoord x, wxCoord y)
{
    focusTimer->Stop();
    if ((x >= x_) &&(x <= (x_+30)) &&(y >= y_) &&(y <= (y_+30)))
    {
        state_ = BUTTON_DOWN;
        dc.DrawBitmap(*downBitmapPointer, x_, y_);
        return true;
    }
    return false;
}

bool HexButton::onMouseRelease(wxDC& WXUNUSED(dc), wxCoord x, wxCoord y)
{
    if ((x >= x_) &&(x <= (x_+30)) &&(y >= y_) &&(y <= (y_+30)))
    {
        focusTimer->Start(50, wxTIMER_ONE_SHOT);
        return true;
    }
    return false;
}

void HexButton::OnTimer(wxTimerEvent& WXUNUSED(event))
{
    state_ = BUTTON_UP;
	p_Computer->releaseButtonOnScreen(this, buttonType_);
}

void HexButton::releaseButtonOnScreen(wxDC& dc)
{
	dc.DrawBitmap(*upBitmapPointer, x_, y_);
}

SwitchButton::SwitchButton(wxDC& dc, int type, wxColour bkgrClr, bool state, wxCoord x, wxCoord y, wxString label)
{
	wxBitmap *upBitmap;
	wxBitmap *downBitmap;

    wxString linuxExtension = "";
#if defined (__linux__)
    linuxExtension = "_linux";
#endif
    
    buttonSizeX_ = 30;
    buttonSizeY_ = 30;

	switch (type)
	{
        case VERTICAL_BUTTON:
            upBitmap = new wxBitmap (p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/swup.png", wxBITMAP_TYPE_PNG);
            downBitmap = new wxBitmap (p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/swdown.png", wxBITMAP_TYPE_PNG);
        break;

		case VERTICAL_BUTTON_RED:
			upBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/swupred.png", wxBITMAP_TYPE_PNG);
			downBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/swdownred.png", wxBITMAP_TYPE_PNG);
		break;

		case HORIZONTAL_BUTTON:
			upBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/swright.png", wxBITMAP_TYPE_PNG);
			downBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/swleft.png", wxBITMAP_TYPE_PNG);
		break;

		case PUSH_BUTTON:
			upBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/pushup.png", wxBITMAP_TYPE_PNG);
			downBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/pushdown.png", wxBITMAP_TYPE_PNG);
		break;

		case PUSH_BUTTON_BLACK:
			upBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/pushupblack.png", wxBITMAP_TYPE_PNG);
			downBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/pushdownblack.png", wxBITMAP_TYPE_PNG);
		break;

		case ELF2K_POWER_BUTTON:
			upBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/Elf2Kon.png", wxBITMAP_TYPE_PNG);
			downBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/Elf2Koff.png", wxBITMAP_TYPE_PNG);
		break;

		case ELF2K_LOAD_BUTTON:
			upBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/loadButtonUp.png", wxBITMAP_TYPE_PNG);
			downBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/loadButtonDown.png", wxBITMAP_TYPE_PNG);
		break;

		case ELF2K_MP_BUTTON:
			upBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/mpButtonUp.png", wxBITMAP_TYPE_PNG);
			downBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/mpButtonDown.png", wxBITMAP_TYPE_PNG);
		break;

		case ELF2K_RUN_BUTTON:
			upBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/runButtonUp.png", wxBITMAP_TYPE_PNG);
			downBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/runButtonDown.png", wxBITMAP_TYPE_PNG);
		break;

		case ELF2K_IN_BUTTON:
			upBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/inButtonUp.png", wxBITMAP_TYPE_PNG);
			downBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/inButtonDown.png", wxBITMAP_TYPE_PNG);
		break;

        case DIP_SWITCH_BUTTON:
            upBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/dip_switch_on.png", wxBITMAP_TYPE_PNG);
            downBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/dip_switch_off.png", wxBITMAP_TYPE_PNG);
            buttonSizeX_ = 8;
            buttonSizeY_ = 20;
        break;

        default:
			upBitmap = new wxBitmap (p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/swup.png", wxBITMAP_TYPE_PNG);
			downBitmap = new wxBitmap (p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/swdown.png", wxBITMAP_TYPE_PNG);
		break;
	}
    
    if (type == PUSH_BUTTON_BLACK)
        type = PUSH_BUTTON;

	wxColour maskColour(255, 0, 255);

	maskUp = new wxMask (*upBitmap, maskColour);
	upBitmap->SetMask(maskUp);
	maskDown = new wxMask (*downBitmap, maskColour);
	downBitmap->SetMask(maskDown);

	upBitmapPointer = new wxBitmap(upBitmap->GetWidth(), upBitmap->GetHeight());  
	downBitmapPointer = new wxBitmap(downBitmap->GetWidth(), downBitmap->GetHeight());  

	wxMemoryDC memDC(*upBitmapPointer); 
    wxSize labelSize = memDC.GetTextExtent(label);
    wxCoord xPosition = (upBitmap->GetWidth() - labelSize.x)/2;
    wxCoord yPosition = (downBitmap->GetHeight() - labelSize.y)/2;

    memDC.SetBackground(*wxTheBrushList->FindOrCreateBrush(bkgrClr));
	memDC.Clear();
	memDC.DrawBitmap(*upBitmap, 0, 0, true);
    if (label != "")
    {
        memDC.SetTextForeground(*wxWHITE);
        memDC.DrawText(label, xPosition, yPosition);
    }
	memDC.SelectObject(wxNullBitmap);

	memDC.SelectObject(*downBitmapPointer); 
	memDC.SetBackground(*wxTheBrushList->FindOrCreateBrush(bkgrClr)); 
	memDC.Clear(); 
	memDC.DrawBitmap(*downBitmap, 0, 0, true);
    if (label != "")
    {
        memDC.SetTextForeground(*wxWHITE);
        memDC.DrawText(label, xPosition, yPosition);
    }
	memDC.SelectObject(wxNullBitmap);

	delete upBitmap;
	delete downBitmap;

	x_ = x;
	y_ = y;
	state_ = state;
	type_ = type;

	if (state == BUTTON_UP)
	{
		dc.DrawBitmap(*upBitmapPointer, x, y);
	}
	else
	{
		dc.DrawBitmap(*downBitmapPointer, x, y);
	}
}

SwitchButton::~SwitchButton()
{
	delete upBitmapPointer;
	delete downBitmapPointer;
}

void SwitchButton::onPaint(wxDC& dc)
{
	if (state_ == BUTTON_UP)
	{
		dc.DrawBitmap(*upBitmapPointer, x_, y_);
	}
	else
	{
		dc.DrawBitmap(*downBitmapPointer, x_, y_);
	}
}

bool SwitchButton::onMousePress(wxDC& dc, wxCoord x, wxCoord y)
{
	if (type_ < PUSH_BUTTON)
		return false;

	if ((x >= x_) &&(x <= (x_+buttonSizeX_)) &&(y >= y_) &&(y <= (y_+buttonSizeY_)))
	{
		state_ = !state_;
		if (state_ == BUTTON_UP)
		{
			dc.DrawBitmap(*upBitmapPointer, x_, y_);
		}
		else
		{
			dc.DrawBitmap(*downBitmapPointer, x_, y_);
		}
		return true;
	}
	return false;
}

bool SwitchButton::onMouseRelease(wxDC& dc, wxCoord x, wxCoord y)
{
    if (type_ > PUSH_BUTTON)
        return false;

    if ((x >= x_) &&(x <= (x_+buttonSizeX_)) &&(y >= y_) &&(y <= (y_+buttonSizeY_)))
	{
		state_ = !state_;
		if (state_ == BUTTON_UP)
		{
			dc.DrawBitmap(*upBitmapPointer, x_, y_);
		}
		else
		{
			dc.DrawBitmap(*downBitmapPointer, x_, y_);
		}
		return true;
	}
	return false;
}

void SwitchButton::setState(wxDC& dc, bool state)
{
	state_ = state;
	if (state_ == BUTTON_UP)
	{
		dc.DrawBitmap(*upBitmapPointer, x_, y_);
	}
	else
	{
		dc.DrawBitmap(*downBitmapPointer, x_, y_);
	}
}

void *RunComputer::Entry()
{
	while(!p_Main->emuClosing())
	{
		p_Computer->cpuInstruction();
	}
	wxCommandEvent event(KILL_COMPUTER, 809);
	event.SetEventObject(p_Main);
	wxPostEvent(p_Main, event);
	return NULL;
}

BEGIN_EVENT_TABLE(Panel, wxWindow)
	EVT_PAINT(Panel::onPaint)
	EVT_CHAR(Panel::onChar)
	EVT_KEY_DOWN(Panel::onKeyDown)
	EVT_KEY_UP(Panel::onKeyUp)
	EVT_LEFT_DOWN(Panel::onMousePress)
	EVT_LEFT_UP(Panel::onMouseRelease)
END_EVENT_TABLE()

Panel::Panel(wxWindow *parent, const wxSize& size)
: wxWindow(parent, wxID_ANY, wxDefaultPosition, size)
{
    updateQLed_ = false;
    updateReadyLed_ = false;
    updateStopLed_ = false;
    updateErrorLed_ = false;
	updateResetLed_ = false;
	updatePauseLed_ = false;
	updateRunLed_ = false;
	updateLoadLed_ = false;
	for (int i=0; i<24; i++)
	{
		ledStatus[i] = 0;
		updateLed_[i] = false;
    }
    for (int i=0; i<4; i++)
    {
        stateLedStatus[i] = 0;
        updateStateLed_[i] = false;
    }
    for (int i=0; i<8; i++)
    {
		segStatus[i] = 0;
		updateSeg_[i] = false;
	}
    updateAddress_ = false;
    updateAddressTil313_ = false;
    updateAddressTil313Italic_ = false;
	updateData_ = false;
	updateDataTil313_ = false;
    updateDataTil313Italic_ = false;
    updateDp313_ = false;

	qLedStatus = 0;
    readyLedStatus = 0;
    stopLedStatus = 0;
    errorLedStatus = 0;
    resetLedStatus = 0;
	pauseLedStatus = 0;
	runLedStatus = 0;
	loadLedStatus = 0;
    addressStatus = 0;
    addressTil313Status = 0;
    addressTil313StatusItalic = 0;
	dataStatus = 0;
    dataTil313Status = 0;
    dataTil313StatusItalic = 0;
    dpStatus = false;
	ms_ = 100;

	functionKeyReleaseTwo_ = false;
}

Panel::~Panel()
{
}

void Panel::init()
{
}

void Panel::init(int WXUNUSED(computerType))
{
}

void Panel::connectKeyEvent(wxWindow* pclComponent)
{
  if(pclComponent)
  {
    pclComponent->Connect(wxID_ANY,
                          wxEVT_KEY_DOWN,
                          wxKeyEventHandler(Panel::onKeyDown),
                          (wxObject*) NULL,
                          this);
    pclComponent->Connect(wxID_ANY,
                          wxEVT_KEY_UP,
                          wxKeyEventHandler(Panel::onKeyUp),
                          (wxObject*) NULL,
                          this);
    pclComponent->Connect(wxID_ANY,
                          wxEVT_CHAR,
                          wxKeyEventHandler(Panel::onChar),
                          (wxObject*) NULL,
                          this);

	wxWindowListNode* pclNode = pclComponent->GetChildren().GetFirst();
    while(pclNode)
    {
      wxWindow* pclChild = pclNode->GetData();
      this->connectKeyEvent(pclChild);
     
      pclNode = pclNode->GetNext();
    }
  }
}
 
void Panel::onPaint(wxPaintEvent&WXUNUSED(event))
{
}

void Panel::onChar(wxKeyEvent& event)
{
	if (p_Vt100 != NULL)
	{
		int key = event.GetKeyCode();
		if (!p_Vt100->charPressed(event))
		{	
			if (forceUpperCase_ && key >= 'a' && key <= 'z')
				key -= 32;
			if (key > 255) key = 0;
			if (key !=0 && key < 128)
			{
				vtOut(key);
			}
		}
	}
	p_Computer->charEvent(event.GetKeyCode());
}

void Panel::vtOut(int value)
{
	if (keyEnd_ != keyStart_-1 ||(keyEnd_ ==25 && keyStart_ != 0))
	{
		keyBuffer_[keyEnd_++] = value;
		if (keyEnd_ == 26) keyEnd_ = 0;
		p_Vt100->dataAvailable();
		if (value == 27) p_Vt100->framingError(1);
	}
}

void Panel::onKeyDown(wxKeyEvent& event)
{
	int keycode;
	keycode = event.GetKeyCode();

	if (p_Vt100 != NULL)
	{
		if (keycode == lastKey_)
		{
			if (repeat_)
			{
				if (event.GetModifiers() != wxMOD_CONTROL || keycode != WXK_HOME || keycode != WXK_ESCAPE || keycode != WXK_SCROLL || keycode != WXK_TAB || keycode != WXK_RETURN)
				{
					lastKey_ = keycode;
					p_Vt100->keyDownPressed(event);
				}
			}
		}
		else
		{
			lastKey_ = keycode;
			p_Vt100->keyDownPressed(event);
		}
	}
	if (p_Main->checkFunctionKey(event))
		return;
	if (!p_Computer->keyDownPressed(keycode))
		event.Skip();
}

void Panel::onKeyUp(wxKeyEvent& event)
{
	if (!functionKeyReleaseTwo_)
		p_Main->onKeyUp(event);
	functionKeyReleaseTwo_ = !functionKeyReleaseTwo_;

	if (p_Vt100 != NULL)
	{
		lastKey_ = 0;
		p_Vt100->keyUpPressed();
	}
	if (!p_Computer->keyUpReleased(event.GetKeyCode()))
		event.Skip();
}

Byte Panel::getKey(Byte vtOut)
{
	if (keyStart_ == keyEnd_)
		return vtOut;
	vtOut = keyBuffer_[keyStart_++];
	if (keyStart_ == 26) keyStart_ = 0;
	if (keyStart_ != keyEnd_)
		p_Vt100->dataAvailable();
	return vtOut;
}

void Panel::onMousePress(wxMouseEvent& WXUNUSED(event))
{
}

void Panel::onMouseRelease(wxMouseEvent& WXUNUSED(event))
{
}

void Panel::ledTimeout()
{
    wxClientDC dc(this);
    updateReadyLed(dc);
    updateStopLed(dc);
    updateErrorLed(dc);
    updateQLed(dc);
	updateResetLed(dc);
	updatePauseLed(dc);
	updateRunLed(dc);
	updateLoadLed(dc);
    for (int i=0; i<24; i++)
        updateLed(dc, i);
    for (int i=0; i<4; i++)
        updateStateLed(dc, i);
    for (int i=0; i<8; i++)
		updateSeg(dc, i);
	updateData(dc);
    updateDataTil313(dc);
    updateDataTil313Italic(dc);
    updateDp313Italic(dc);
    updateAddress(dc);
    updateAddressTil313(dc);
    updateAddressTil313Italic(dc);
}

void Panel::setLedMs(long ms)
{
	ms_ = ms;
}

void Panel::updateReadyLed(wxDC& dc)
{
    if (updateReadyLed_)
    {
        readyLedPointer->setStatus(dc, readyLedStatus);
        updateReadyLed_ = false;
    }
}

void Panel::setReadyLed(int status)
{
    if (readyLedStatus != status)
    {
        readyLedStatus = status;
        updateReadyLed_ = true;
        if (ms_ == 0)
        {
            wxClientDC dc(this);
            updateReadyLed(dc);
        }
    }
}

void Panel::setStopLed(int status)
{
    if (stopLedStatus != status)
    {
        stopLedStatus = status;
        updateStopLed_ = true;
        if (ms_ == 0)
        {
            wxClientDC dc(this);
            updateStopLed(dc);
        }
    }
}

void Panel::updateStopLed(wxDC& dc)
{
    if (updateStopLed_)
    {
        stopLedPointer->setStatus(dc, stopLedStatus);
        updateStopLed_ = false;
    }
}

void Panel::setErrorLed(int status)
{
	if (errorLedStatus != status)
	{
		errorLedStatus = status;
		updateErrorLed_ = true;
		if (ms_ == 0)
		{
			wxClientDC dc(this);
			updateErrorLed(dc);
		}
	}
}

void Panel::updateErrorLed(wxDC& dc)
{
	if (updateErrorLed_)
	{
		errorLedPointer->setStatus(dc, errorLedStatus);
		updateErrorLed_ = false;
	}
}

void Panel::setQLed(int status)
{
	if (qLedStatus != status)
	{
		qLedStatus = status;
		updateQLed_ = true;
		if (ms_ == 0)
		{
			wxClientDC dc(this);
			updateQLed(dc);
		}
	}
}

void Panel::updateQLed(wxDC& dc)
{
	if (updateQLed_)
	{
		qLedPointer->setStatus(dc, qLedStatus);
		updateQLed_ = false;
	}
}

void Panel::setResetLed(int status)
{
	if (resetLedStatus != status)
	{
		resetLedStatus = status;
		updateResetLed_ = true;
		if (ms_ == 0)
		{
			wxClientDC dc(this);
			updateResetLed(dc);
		}
	}
}

void Panel::updateResetLed(wxDC& dc)
{
	if (updateResetLed_)
	{
		resetLedPointer->setStatus(dc, resetLedStatus);
		updateResetLed_ = false;
	}
}

void Panel::setPauseLed(int status)
{
	if (pauseLedStatus != status)
	{
		pauseLedStatus = status;
		updatePauseLed_ = true;
		if (ms_ == 0)
		{
			wxClientDC dc(this);
			updatePauseLed(dc);
		}
	}
}

void Panel::updatePauseLed(wxDC& dc)
{
	if (updatePauseLed_)
	{
		pauseLedPointer->setStatus(dc, pauseLedStatus);
		updatePauseLed_ = false;
	}
}

void Panel::setRunLed(int status)
{
	if (runLedStatus != status)
	{
		runLedStatus = status;
		updateRunLed_ = true;
		if (ms_ == 0)
		{
			wxClientDC dc(this);
			updateRunLed(dc);
		}
	}
}

void Panel::updateRunLed(wxDC& dc)
{
	if (updateRunLed_)
	{
		runLedPointer->setStatus(dc, runLedStatus);
		updateRunLed_ = false;
	}
}

void Panel::setLoadLed(int status)
{
	if (loadLedStatus != status)
	{
		loadLedStatus = status;
		updateLoadLed_ = true;
		if (ms_ == 0)
		{
			wxClientDC dc(this);
			updateLoadLed(dc);
		}
	}
}

void Panel::updateLoadLed(wxDC& dc)
{
	if (updateLoadLed_)
	{
		loadLedPointer->setStatus(dc, loadLedStatus);
		updateLoadLed_ = false;
	}
}

void Panel::setLed(int i, int status)
{
	if (ledStatus[i] != status)
	{
		ledStatus[i] = status;
		updateLed_[i] = true;
		if (ms_ == 0)
		{
			wxClientDC dc(this);
			updateLed(dc, i);
		}
	}
}

void Panel::updateLed(wxDC& dc, int i)
{
	if (updateLed_[i])
	{
		ledPointer[i]->setStatus(dc, ledStatus[i]);
		updateLed_[i] = false;
	}
}

void Panel::setStateLed(int i, int status)
{
    if (stateLedStatus[i] != status)
    {
        stateLedStatus[i] = status;
        updateStateLed_[i] = true;
        if (ms_ == 0)
        {
            wxClientDC dc(this);
            updateStateLed(dc, i);
        }
    }
}

void Panel::updateStateLed(wxDC& dc, int i)
{
    if (updateStateLed_[i])
    {
        stateLedPointer[i]->setStatus(dc, stateLedStatus[i]);
        updateStateLed_[i] = false;
    }
}

void Panel::showData(Byte value)
{
	if (dataStatus != value)
	{
		dataStatus = value;
		updateData_ = true;
		if (ms_ == 0)
		{
			wxClientDC dc(this);
			updateData(dc);
		}
	}
}

void Panel::updateData(wxDC& dc)
{
	if (updateData_)
	{
		dataPointer[0]->update(dc,(dataStatus>>4)&15);
		dataPointer[1]->update(dc, dataStatus&15);
		updateData_ = false;
	}
}

void Panel::showDataTil313(Byte value)
{
	if (dataTil313Status != value)
	{
		dataTil313Status = value;
		updateDataTil313_ = true;
		if (ms_ == 0)
		{
			wxClientDC dc(this);
			updateDataTil313(dc);
		}
	}
}

void Panel::updateDataTil313(wxDC& dc)
{
	if (updateDataTil313_)
	{
		dataTil313Pointer[0]->update(dc,(dataTil313Status>>4)&15);
		dataTil313Pointer[1]->update(dc, dataTil313Status&15);
		updateDataTil313_ = false;
	}
}

void Panel::showDataTil313Italic(Byte value)
{
    if (dataTil313StatusItalic != value)
    {
        dataTil313StatusItalic = value;
        updateDataTil313Italic_ = true;
        if (ms_ == 0)
        {
            wxClientDC dc(this);
            updateDataTil313Italic(dc);
        }
    }
}

void Panel::updateDataTil313Italic(wxDC& dc)
{
    if (updateDataTil313Italic_)
    {
        dataTil313PointerItalic[0]->update(dc,(dataTil313StatusItalic>>4)&15);
        dataTil313PointerItalic[1]->update(dc, dataTil313StatusItalic&15);
        updateDataTil313Italic_ = false;
    }
}

void Panel::showDp313Italic(bool status)
{
    if (dpStatus != status)
    {
        dpStatus = status;
        updateDp313_ = true;
        if (ms_ == 0)
        {
            wxClientDC dc(this);
            updateDp313Italic(dc);
        }
    }
}

void Panel::updateDp313Italic(wxDC& dc)
{
    if (updateDp313_)
    {
        dataTil313PointerItalic[1]->dp(dc, dpStatus);
        updateDp313_ = false;
    }
}

void Panel::showSeg(int number, Byte value)
{
	if (segStatus[number] != value)
	{
		segStatus[number] = value;
		updateSeg_[number] = true;
		if (ms_ == 0)
		{
			wxClientDC dc(this);
			updateSeg(dc, number);
		}
	}
}

void Panel::updateSeg(wxDC& dc, int number)
{
	if (updateSeg_[number])
	{
		segPointer[number]->update(dc, segStatus[number]);
		updateSeg_[number] = false;
	}
}

void Panel::showAddress(Word address)
{
    if (addressStatus != address)
    {
        addressStatus = address;
        updateAddress_ = true;
        if (ms_ == 0)
        {
            wxClientDC dc(this);
            updateAddress(dc);
        }
    }
}

void Panel::showAddressTil313(Word address)
{
    if (addressTil313Status != address)
    {
        addressTil313Status = address;
        updateAddressTil313_ = true;
        if (ms_ == 0)
        {
            wxClientDC dc(this);
            updateAddressTil313(dc);
        }
    }
}

void Panel::showAddressTil313Italic(Word address)
{
    if (addressTil313StatusItalic != address)
    {
        addressTil313StatusItalic = address;
        updateAddressTil313Italic_ = true;
        if (ms_ == 0)
        {
            wxClientDC dc(this);
            updateAddressTil313Italic(dc);
        }
    }
}

void Panel::updateAddress(wxDC& dc)
{
	if (updateAddress_)
	{
		addressPointer[0]->update(dc, addressStatus>>12);
		addressPointer[1]->update(dc,(addressStatus>>8)&15);
		addressPointer[2]->update(dc,(addressStatus>>4)&15);
		addressPointer[3]->update(dc, addressStatus&15);
		updateAddress_ = false;
	}
}

void Panel::updateAddressTil313(wxDC& dc)
{
    if (updateAddressTil313_)
    {
        addressTil313Pointer[0]->update(dc, addressTil313Status>>12);
        addressTil313Pointer[1]->update(dc,(addressTil313Status>>8)&15);
        addressTil313Pointer[2]->update(dc,(addressTil313Status>>4)&15);
        addressTil313Pointer[3]->update(dc, addressTil313Status&15);
        updateAddressTil313_ = false;
    }
}

void Panel::updateAddressTil313Italic(wxDC& dc)
{
    if (updateAddressTil313Italic_)
    {
        addressTil313PointerItalic[0]->update(dc, addressTil313StatusItalic>>12);
        addressTil313PointerItalic[1]->update(dc,(addressTil313StatusItalic>>8)&15);
        addressTil313PointerItalic[2]->update(dc,(addressTil313StatusItalic>>4)&15);
        addressTil313PointerItalic[3]->update(dc, addressTil313StatusItalic&15);
        updateAddressTil313Italic_ = false;
    }
}

void Panel::inUp()
{
}

void Panel::inDown()
{
}

void Panel::inSetState(bool state)
{
    wxClientDC dc(this);
    inSwitchButton->setState(dc, state);
}

void Panel::clearSetState(bool state)
{
    wxClientDC dc(this);
    clearSwitchButton->setState(dc, state);
}

void Panel::waitSetState(bool state)
{
    wxClientDC dc(this);
    waitSwitchButton->setState(dc, state);
}

void Panel::runUp()
{
#if wxCHECK_VERSION(2, 9, 0)
    runButtonPointer->SetBitmap(*upBitmapPointer);
#else
    runButtonPointer->SetBitmapLabel(*upBitmapPointer);
#endif
}

void Panel::runDown()
{
#if wxCHECK_VERSION(2, 9, 0)
    runButtonPointer->SetBitmap(*downBitmapPointer);
#else
    runButtonPointer->SetBitmapLabel(*downBitmapPointer);
#endif
}

void Panel::runSetState(bool state)
{
	wxClientDC dc(this);
	runSwitchButton->setState(dc, state);
}

void Panel::mpUp()
{
#if wxCHECK_VERSION(2, 9, 0)
	mpButtonPointer->SetBitmap(*upBitmapPointer);
#else
	mpButtonPointer->SetBitmapLabel(*upBitmapPointer);
#endif
}

void Panel::mpDown()
{
#if wxCHECK_VERSION(2, 9, 0)
	mpButtonPointer->SetBitmap(*downBitmapPointer);
#else
	mpButtonPointer->SetBitmapLabel(*downBitmapPointer);
#endif
}

void Panel::mpSetState(bool state)
{
	wxClientDC dc(this);
	mpSwitchButton->setState(dc, state);
}

void Panel::powerUp()
{
#if wxCHECK_VERSION(2, 9, 0)
	powerButtonPointer->SetBitmap(*upBitmapPointer);
#else
	powerButtonPointer->SetBitmapLabel(*upBitmapPointer);
#endif
}

void Panel::powerDown()
{
#if wxCHECK_VERSION(2, 9, 0)
	powerButtonPointer->SetBitmap(*downBitmapPointer);
#else
	powerButtonPointer->SetBitmapLabel(*downBitmapPointer);
#endif
}

void Panel::loadUp()
{
#if wxCHECK_VERSION(2, 9, 0)
	loadButtonPointer->SetBitmap(*upBitmapPointer);
#else
	loadButtonPointer->SetBitmapLabel(*upBitmapPointer);
#endif
}

void Panel::loadDown()
{
#if wxCHECK_VERSION(2, 9, 0)
	loadButtonPointer->SetBitmap(*downBitmapPointer);
#else
	loadButtonPointer->SetBitmapLabel(*downBitmapPointer);
#endif
}

void Panel::loadSetState(bool state)
{
	wxClientDC dc(this);
	loadSwitchButton->setState(dc, state);
}

void Panel::dataUp(int number)
{
#if wxCHECK_VERSION(2, 9, 0)
	dataSwitchPointer[number]->SetBitmap(*upBitmapPointer);
#else
	dataSwitchPointer[number]->SetBitmapLabel(*upBitmapPointer);
#endif
}

void Panel::dataDown(int number)
{
#if wxCHECK_VERSION(2, 9, 0)
	dataSwitchPointer[number]->SetBitmap(*downBitmapPointer);
#else
	dataSwitchPointer[number]->SetBitmapLabel(*downBitmapPointer);
#endif
}

void Panel::dataSetState(int number, bool state)
{
	wxClientDC dc(this);
	dataSwitchButton[number]->setState(dc, state);
}

void Panel::efUp(int number)
{
#if wxCHECK_VERSION(2, 9, 0)
	efSwitchPointer[number]->SetBitmap(*upBitmapPointer);
#else
	efSwitchPointer[number]->SetBitmapLabel(*upBitmapPointer);
#endif
}

void Panel::efDown(int number)
{
#if wxCHECK_VERSION(2, 9, 0)
	efSwitchPointer[number]->SetBitmap(*downBitmapPointer);
#else
	efSwitchPointer[number]->SetBitmapLabel(*downBitmapPointer);
#endif
}

Computer::Computer()
{
	memoryStart_ = 0;
	loadedProgram_ = NOPROGRAM;
	loadedOs_ = NOOS;
	lastTapeInput_ = 0;
	maxTapeInput_ = 0;
	gaugeValue_ = 0;
	sign_ = true;
	counter_ = 10;
	period_ = 24;
	basicExecAddress_[BASICADDR_KEY] = -1;
	basicExecAddress_[BASICADDR_READY] = -1;
	basicExecAddress_[BASICADDR_KEY_VT_RESTART] = -1;
	basicExecAddress_[BASICADDR_KEY_VT_INPUT] = -1;
	chip8baseVar_ = 0xef0;
	chip8mainLoop_ = 0x1b;
    pseudoLoaded_ = false;
    inKey1_ = -1;
    inKey2_ = -1;
	for (int i = 0; i<16; i++)
		chip8Register[i] = -1;

	threadPointer = new RunComputer();
	if ( threadPointer->Create() != wxTHREAD_NO_ERROR )
	{
		p_Main->message("Can't create thread");
	}
	threadPointer->SetPriority(WXTHREAD_MAX_PRIORITY);
}

bool Computer::isComputerRunning()
{
	return false;
}

void Computer::startComputerRun(bool WXUNUSED(load))
{
	p_Main->message("Illegal request to start Computer");
}

void Computer::onReset()
{
	p_Main->message("Illegal request to reuqest reset Computer");
}

void Computer::onRun()
{
	resetPressed_ = true;
}

void Computer::keyDown(int WXUNUSED(keycode))
{
}

bool Computer::keyDownExtended(int WXUNUSED(keycode), wxKeyEvent& WXUNUSED(event))
{
	return false;
}

bool Computer::keyDownPressed(int WXUNUSED(keycode))
{
	return false;
}

void Computer::keyUp(int WXUNUSED(keycode))
{
}

void Computer::keyUpExtended(int WXUNUSED(keycode), wxKeyEvent& WXUNUSED(event))
{
}

bool Computer::keyUpReleased(int WXUNUSED(keycode))
{
	return false;
}

void Computer::charEvent(int WXUNUSED(keycode))
{
}

void Computer::onButtonRelease(wxCommandEvent& WXUNUSED(event))
{
}

void Computer::onButtonPress(wxCommandEvent& WXUNUSED(event))
{
}

void Computer::cid1Bit8(bool WXUNUSED(set))
{
	p_Main->message("Illegal call to Cidelsa video bit set/reset");
}

int Computer::getComxExpansionType(int WXUNUSED(card))
{
	p_Main->message("Illegal call to fetch Comx expansion type");
	return 0;
}

void Computer::sleepComputer(long WXUNUSED(ms))
{
    p_Main->message("Illegal call to sleep computer");
}

void Computer::pauseComputer()
{
    p_Main->message("Illegal call to pause computer");
}

void Computer::resumeComputer()
{
    p_Main->message("Illegal call to resume computer");
}

void Computer::checkCaps()
{
}

void Computer::finishStopTape()
{
	p_Main->message("Illegal call to stop PECOM/ETI tape");
}

void Computer::printOutPecom(int WXUNUSED(q))
{
	p_Main->message("Illegal call to send Q bit to PECOM printer");
}

void Computer::setElf2KDivider(Byte WXUNUSED(value))
{
	p_Main->message("Illegal call to set Elf 2000 divider value");
}

void Computer::removeElf2KSwitch()
{
	p_Main->message("Illegal call to stop Elf 2000 swicth panel");
}

void Computer::removeElfHex()
{
	p_Main->message("Illegal call to stop Elf hex panel");
}

void Computer::removeCosmicosHex()
{
	p_Main->message("Illegal call to stop Comsicos hex panel");
}

void Computer::removeElfLedModule()
{
	p_Main->message("Illegal call to stop Elf led panel");
}

void Computer::showData(Byte WXUNUSED(val))
{
}

void Computer::showCycleData(Byte WXUNUSED(val))
{
}

void Computer::showDmaLed()
{
}

void Computer::showIntLed()
{
}

void Computer::resetVideo()
{
	p_Main->message("Illegal call to reset video");
}

void Computer::resetComputer()
{
	p_Main->message("Illegal call to reset computer");
}

void Computer::clearBootstrap()
{
	p_Main->message("Illegal call to clear bootstrap");
}

void Computer::onInButtonRelease()
{
	p_Main->message("Illegal call to release Elf IN button");
}

void Computer::onInButtonPress()
{
	p_Main->message("Illegal call to press Elf IN button");
}

void Computer::onInButtonPress(Byte WXUNUSED(value))
{
	p_Main->message("Illegal call to press Elf IN button");
}

void Computer::cassette(short val)
{
	if (conversionTypeWav_ == 0)
	{
		if (val <= 0)
		{
			cassetteEf_ = tapePolarity_; // 0
			maxTapeInput_ = 0;
		}
		else
		{
			cassetteEf_ = !tapePolarity_; //1
			if (val > lastTapeInput_)
				maxTapeInput_ = val;
			else
				gaugeValue_ = maxTapeInput_ / 5;
		}
	}
	else
	{
		if (cassetteEf_ != tapePolarity_)
		{
			if (val > lastTapeInput_)
				maxTapeInput_ = val;
			else
			{
				gaugeValue_ = maxTapeInput_ / 5;
				cassetteEf_ = tapePolarity_; // 0
			}
		}
		else
		{
			if (val < lastTapeInput_)
				maxTapeInput_ = -val;
			else
			{
				gaugeValue_ = maxTapeInput_ / 5;
				cassetteEf_ = !tapePolarity_; //1
			}
		}
	}
	lastTapeInput_ = val;
}

void Computer::cassette(char val)
{
	if (conversionTypeWav_ == 0)
	{
		if (val < 0)
		{
/*			if (cassetteEf_ != tapePolarity_)
			{
//				if (period_ > 29)
//					p_Main->messageInt(period_);
				if (period_ < 23 || (period_ > 36 && period_ < 63))
				{
					period_++;
					lastTapeInput_ = val;
					return;
				}
				period_ = 0; // short 23-29 long 60-64
			}
			period_++;*/
			cassetteEf_ = tapePolarity_; // 0
			maxTapeInput_ = 0;
		}
		else
		{
/*			if (cassetteEf_ == tapePolarity_)
			{
//				if (period_ > 29)
//					p_Main->messageInt(period_);
				if (period_ < 23 || (period_ > 36 && period_ < 63))
				{
					period_++;
					lastTapeInput_ = val;
					return;
				}
				period_ = 0;
			}
			period_++;*/
			cassetteEf_ = !tapePolarity_; //1
			if (val > lastTapeInput_)
				maxTapeInput_ = val;
			else
				gaugeValue_ = maxTapeInput_ * 45;
		}
	}
	else
	{
		if (cassetteEf_ != tapePolarity_)
		{
			if (val > lastTapeInput_)
				maxTapeInput_ = val;
			else
			{
				gaugeValue_ = maxTapeInput_ * 45;
				cassetteEf_ = tapePolarity_; // 0
			}
		}
		else
		{
			if (val < lastTapeInput_)
				maxTapeInput_ = -val;
			else
			{
				gaugeValue_ = maxTapeInput_ * 45;
				cassetteEf_ = !tapePolarity_; //1
			}
		}
	}
	lastTapeInput_ = val;
}

void Computer::cassetteFred(short WXUNUSED(val))
{
}

void Computer::cassetteFred(char WXUNUSED(val))
{
}

void Computer::realCassette(short val)
{
	if (conversionType_ == 0)
	{
		if (val <= 0)
		{
			cassetteEf_ = tapePolarity_; // 0
			maxTapeInput_ = 0;
		}
		else
		{
			cassetteEf_ = !tapePolarity_; //1
			if (val > lastTapeInput_)
				maxTapeInput_ = val;
			else
				gaugeValue_ = maxTapeInput_;
		}
	}
	else
	{
		if (cassetteEf_ != tapePolarity_)
		{
			if (val > lastTapeInput_)
				maxTapeInput_ = val;
			else
			{
				gaugeValue_ = maxTapeInput_;
				cassetteEf_ = tapePolarity_; // 0
			}
		}
		else
		{
			if (val < lastTapeInput_)
				maxTapeInput_ = -val;
			else
			{
				gaugeValue_ = maxTapeInput_;
				cassetteEf_ = !tapePolarity_; //1
			}
		}
	}
	lastTapeInput_ = val;
}

void Computer::keyClear()
{
}

void Computer::startComputer()
{
}

void Computer::initComputer()
{
}

void Computer::configureComputer()
{
}

void Computer::onRunButton()
{
}

void Computer::onRunPButton()
{
}

void Computer::onReadButton()
{
}

void Computer::onCardButton()
{
}

void Computer::onRunButton(wxCommandEvent&WXUNUSED(event))
{
}

void Computer::onRunPButton(wxCommandEvent&WXUNUSED(event))
{
}

void Computer::dataButton(int WXUNUSED(i))
{
}

void Computer::onNumberKeyRelease(int WXUNUSED(i))
{
}

void Computer::onNumberKeyPress(int WXUNUSED(i))
{
}

void Computer::onNumberKeyDown(int WXUNUSED(i))
{
}

void Computer::onPause()
{
}

void Computer::onPause(wxCommandEvent&WXUNUSED(event))
{
}

void Computer::onMonitor()
{
}

void Computer::onMonitor(wxCommandEvent&WXUNUSED(event))
{
}

void Computer::onSingleStep()
{
}

void Computer::onSingleStep(wxCommandEvent&WXUNUSED(event))
{
}

void Computer::onMpButton()
{
}

void Computer::onMpButton(int WXUNUSED(buttonNumber))
{
}

void Computer::onMpButton(wxCommandEvent&WXUNUSED(event))
{
}

void Computer::onWaitButton()
{
}

void Computer::onClearButton()
{
}

void Computer::onClearButton(wxCommandEvent&WXUNUSED(event))
{
}

void Computer::onResetButton()
{
}

void Computer::onResetButton(wxCommandEvent&WXUNUSED(event))
{
}

void Computer::onLoadButton()
{
}

void Computer::onLoadButton(wxCommandEvent&WXUNUSED(event))
{
}

void Computer::onRamButton()
{
}

void Computer::dataSwitch(int WXUNUSED(number))
{
}

void Computer::efSwitch(int WXUNUSED(number))
{
}

Byte Computer::getData()
{
	return 0;
}

void Computer::onHexDown(int WXUNUSED(hex))
{
}

void Computer::onHexKeyUp(int WXUNUSED(keycode))
{
}

void Computer::reDefineInKey(int inKey1, int inKey2)
{
    inKey1_ = inKey1;
    inKey2_ = inKey2;
}

void Computer::reDefineKeys()
{
}

void Computer::reDefineKeys(int* WXUNUSED(hexKeyDefA[]), int* WXUNUSED(keyDefGameValueA[]), int WXUNUSED(coin))
{
}

int Computer::getRunState()
{
	return 0;
}

void Computer::checkLoadedSoftware()
{
}

void Computer::dataAvailable(bool WXUNUSED(data))
{
}

void Computer::thrStatus(bool WXUNUSED(data))
{
}

void Computer::setTempo(int WXUNUSED(tempo))
{
}

void Computer::switchQ(int WXUNUSED(value))
{
}

void Computer::reDefineKeysA(int* WXUNUSED(hexKeyDefA[]), int* WXUNUSED(keyDefGameValueA[]), int* WXUNUSED(keyDefGameHexA[]))
{
}

void Computer::reDefineKeysB(int* WXUNUSED(hexKeyDefB[]), int* WXUNUSED(keyDefGameValueB[]), int* WXUNUSED(keyDefGameHexB[]))
{
}

void Computer::reDefineKeysA(int* WXUNUSED(hexKeyDefA1[]), int* WXUNUSED(hexKeyDefA2[]))
{
}

void Computer::reDefineKeysB(int* WXUNUSED(hexKeyDefB1[]), int* WXUNUSED(hexKeyDefA2[]))
{
}

void Computer::onNumberKeyDown(wxCommandEvent& WXUNUSED(event))
{
}

void Computer::onNumberKeyUp(wxCommandEvent& WXUNUSED(event))
{
}

void Computer::onNumberKeyUp()
{
}

void Computer::ledTimeout()
{
}

void Computer::setLedMs(long WXUNUSED(ms))
{
}

void Computer::showDataLeds(Byte WXUNUSED(value))
{
}

Byte Computer::getKey(Byte WXUNUSED(vtOut))
{
	return 0;
}

void Computer::activateMainWindow()
{
}

void Computer::showChip8Registers()
{
	int reg = chip8baseVar_;
	int newValue;

	for (int i=0; i<16; i++)
	{
        if (chip8register12bit_)
        {
            newValue = (p_Computer->readMem(reg++) << 8);
            newValue += p_Computer->readMem(reg++);
        }
        else
            newValue = p_Computer->readMem(reg++);
		if (newValue != chip8Register[i])
		{
			p_Main->showChip8Register(i, newValue, chip8register12bit_);
			chip8Register[i] = newValue;
		}
	}
}

void Computer::writeMemDataType(Word WXUNUSED(address), Byte  WXUNUSED(type))
{
}

Byte Computer::readMemDataType(Word WXUNUSED(address))
{
	return 0;
}

void Computer::writeDebugFileExit(wxString dir, wxString name, Word start, Word end)
{
	if (p_Main->getSaveDebugFile())
	{
		writeDebugFile(dir, name, start, end);
	}
}

void Computer::writeDebugFile(wxString dir, wxString name, Word start, Word end)
{
	wxFileName FullPath = wxFileName(dir+name, wxPATH_NATIVE);
    dir = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    name = FullPath.GetName() + ".debug";

	wxFFileOutputStream out(dir+name);
	wxZipOutputStream zip(out);
	wxDataOutputStream data(zip);

	zip.PutNextEntry(FullPath.GetName()+".deb");

	for (size_t i=start; i<=end; i++)
		data << p_Computer->readMemDataType(i);

    zip.PutNextEntry(FullPath.GetName()+".lab");
    
    for (size_t i=start; i<=end; i++)
        data << p_Computer->readMemLabelType(i);
 
    zip.Close();
}

void Computer::readDebugFile(wxString dir, wxString name, wxString number, Word start)
{
	using namespace std;

	wxFileName FullPath = wxFileName(dir+name, wxPATH_NATIVE);
	dir = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
	name = FullPath.GetName() + ".debug" + number;
    Word startAddress;
    
	if (wxFile::Exists(dir+name))
	{
		auto_ptr<wxZipEntry> entry;

		wxFFileInputStream in(dir+name);
		wxZipInputStream zip(in);

		while (entry.reset(zip.GetNextEntry()), entry.get() != NULL)
		{
			// access meta-data
			name = entry->GetName();
			Byte nextByte = 0;
            
            startAddress = start;
            if (name.Right(3) == "deb")
            {
                while (!zip.Eof())
                {
                    nextByte = zip.GetC();
     //               if (nextByte >= 70 && nextByte <=86)
       //                 nextByte += 100;
                    if (nextByte != 0xff)
                        p_Computer->writeMemDataType(startAddress, nextByte);
                    startAddress++;
                }
                zip.CloseEntry();
            }
            if (name.Right(3) == "lab")
            {
                while (!zip.Eof())
                {
                    nextByte = zip.GetC();
                    if (nextByte != 0xff)
                        p_Computer->writeMemLabelType(startAddress, nextByte);
                    startAddress++;
                }
                zip.CloseEntry();
            }
        }
    }
}

void Computer::deleteAllBackup(wxString WXUNUSED(directory), bool WXUNUSED(sub))
{
}

void Computer::onBackupYes(wxString WXUNUSED(dir), bool WXUNUSED(sub))
{
}

void Computer::releaseButtonOnScreen(HexButton* WXUNUSED(buttonPointer), int WXUNUSED(buttonType))
{
}

void Computer::terminalSave(wxString WXUNUSED(fileName))
{
}

void Computer::terminalLoad(wxString WXUNUSED(fileName), wxString WXUNUSED(fileName), bool WXUNUSED(binaryFile))
{
}

void Computer::terminalStop()
{
}

void Computer::setGreenLed(int WXUNUSED(status))
{
}

void Computer::setMultiCartLsb(Byte WXUNUSED(lsb))
{
}

void Computer::setMultiCartMsb(Byte WXUNUSED(msb))
{
}

void Computer::setDisableSystemRom(bool WXUNUSED(disableSystemRom))
{
}

void Computer::setAutoKeyDef(bool WXUNUSED(autoKeyDef))
{
}

void Computer::showAddress(Word WXUNUSED(address))
{
}

void Computer::showState(int WXUNUSED(state))
{
}

void Computer::switchHexEf(bool WXUNUSED(state))
{
}


