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

#include "main.h"
#include "keymaptmc.h"

#define KEY_DEF_NORMAL 0
#define KEY_DEF_SHIFT 1

#include "wx/xrc/xmlres.h"             

BEGIN_EVENT_TABLE(KeyMapTmcDialog, wxDialog)
   EVT_BUTTON(XRCID("Save"), KeyMapTmcDialog::onSaveButton)
   EVT_BUTTON(XRCID("Default"), KeyMapTmcDialog::onDefault)
   EVT_BUTTON(XRCID("Key00"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("Key01"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("Key02"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("Key03"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("Key04"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("Key05"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("Key06"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("Key07"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("Key08"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("Key09"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("Key0A"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("Key0B"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("Key0C"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("Key0D"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("Key0E"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("Key0F"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("Key10"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("Key11"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("Key12"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("Key13"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("Key14"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("KeyShift00"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("KeyShift01"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("KeyShift02"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("KeyShift03"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("KeyShift04"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("KeyShift05"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("KeyShift06"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("KeyShift07"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("KeyShift08"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("KeyShift09"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("KeyShift0A"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("KeyShift0B"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("KeyShift0C"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("KeyShift0D"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("KeyShift0E"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("KeyShift0F"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("KeyShift10"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("KeyShift11"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("KeyShift12"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("KeyShift13"), KeyMapTmcDialog::onKey)
   EVT_BUTTON(XRCID("KeyShift14"), KeyMapTmcDialog::onKey)
#ifdef __WXMAC__
   EVT_TIMER(wxID_ANY, KeyMapTmcDialog::onFocusTimer)
#endif
END_EVENT_TABLE()

KeyMapTmcDialog::KeyMapTmcDialog(wxWindow* parent)
{
    normalKey_ = -1;
    shiftKey_ = -1;
    
    wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"keymap_tmc_" + p_Main->getFontSize() + ".xrc");
    wxXmlResource::Get()->LoadDialog(this, parent, wxT("KeyMapDialog"));
    
    p_Main->getTmc600Keys(keyDefNormal_, keyDefShift_);
    
	this->connectKeyDownEvent(this);
	updateButtons();
    
#ifdef __WXMAC__
    focusTimer = new wxTimer(this, wxID_ANY);
    focusTimer->Start(1000, wxTIMER_CONTINUOUS);
#endif
}

KeyMapTmcDialog::~KeyMapTmcDialog()
{
#ifdef __WXMAC__
    focusTimer->Stop();
    delete focusTimer;
#endif
}

void KeyMapTmcDialog::compareButtons(int toBeCheckedKey, int toBeCheckedValue, int type)
{
	wxString button;
	if (type == KEY_DEF_NORMAL)
		button.Printf("Key%02X", toBeCheckedKey);
	else
        button.Printf("KeyShift%02X", toBeCheckedKey);

	for (int j=0; j<=20; j++)
	{
		if (toBeCheckedKey != j || type == KEY_DEF_SHIFT)
		{
			if (toBeCheckedValue == keyDefNormal_[j] && toBeCheckedValue != 0)
			{		
#ifdef __WXMAC__
                XRCCTRL(*this, button, wxButton)->SetBackgroundColour(wxColour(255,0,0));
#else
				XRCCTRL(*this, button, wxButton)->SetForegroundColour(wxColour(255,0,0));
#endif
			}
		}
	}
    for (int j=0; j<=20; j++)
    {
        if (toBeCheckedKey != j || type == KEY_DEF_NORMAL)
        {
            if (toBeCheckedValue == keyDefShift_[j] && toBeCheckedValue != 0)
            {
#ifdef __WXMAC__
                XRCCTRL(*this, button, wxButton)->SetBackgroundColour(wxColour(255,0,0));
#else
                XRCCTRL(*this, button, wxButton)->SetForegroundColour(wxColour(255,0,0));
#endif
            }
        }
    }
}

void KeyMapTmcDialog::updateButtons()
{
	wxString button, keyStr;

    for (int i=0; i <=16; i++)
    {
        setLabel("Key%02X", i, keyDefNormal_[i]);
        compareButtons(i, keyDefNormal_[i], KEY_DEF_NORMAL);
    }
    setLabel("Key%02X", 20, keyDefNormal_[20]);
    compareButtons(20, keyDefNormal_[20], KEY_DEF_NORMAL);
    
    for (int i=0; i <= 20; i++)
    {
        setLabel("KeyShift%02X", i, keyDefShift_[i]);
        compareButtons(i, keyDefShift_[i], KEY_DEF_SHIFT);
    }
}

void KeyMapTmcDialog::onSaveButton( wxCommandEvent& WXUNUSED(event) )
{
    p_Main->storeDefaultTmc600Keys (keyDefNormal_, keyDefShift_);
    EndModal( wxID_OK );
}

void KeyMapTmcDialog::onKey(wxCommandEvent &event)
{
	wxString buttonName, buttonNumber;

	if (normalKey_ != -1)
	{
        setLabel("Key%02X", normalKey_, keyDefNormal_[normalKey_]);
        normalKey_ = -1;
	}
	if (shiftKey_ != -1)
	{
        setLabel("KeyShift%02X", shiftKey_, keyDefShift_[shiftKey_]);
        shiftKey_ = -1;
    }
	buttonName = wxWindow::FindWindowById(event.GetId())->GetName();
	buttonNumber = buttonName.Right(2);
    
    if (buttonName.Left(4) != "KeyS")
    {
        if (buttonNumber.ToLong(&normalKey_, 16))
            XRCCTRL(*this, buttonName, wxButton)->SetLabel("Press Key");
        else
            normalKey_ = -1;
    }
    else
    {
        if (buttonNumber.ToLong(&shiftKey_, 16))
            XRCCTRL(*this, buttonName, wxButton)->SetLabel("Press Key");
        else
            shiftKey_ = -1;
    }
}

void KeyMapTmcDialog::onDefault(wxCommandEvent& WXUNUSED(event))
{
	p_Main->getDefaultTmc600Keys(keyDefNormal_, keyDefShift_);
    
    updateButtons();
    shiftKey_ = -1;
    normalKey_ = -1;
}

void KeyMapTmcDialog::onKeyDown(wxKeyEvent& event)
{
    int key = event.GetKeyCode();
    int modifier = event.GetModifiers();
    
    if (key == 8364)
        key = 36;
    
	if (normalKey_ != -1)
	{
        if (modifier != wxMOD_NONE || (key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z'))
        {
            wxString buttonName;
            buttonName.Printf("Key%02X", (int)normalKey_);
            XRCCTRL(*this, buttonName, wxButton)->SetLabel("Invalid Key");
            return;
        }
        if (key == 32)
            key = 0;
		setLabel("Key%02X", normalKey_, key);
        keyDefNormal_[normalKey_] = key;
		normalKey_ = -1;
	}
	if (shiftKey_ != -1)
	{
        if ((modifier != wxMOD_SHIFT && key != 32) || (key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z'))
        {
            wxString buttonName;
            buttonName.Printf("KeyShift%02X", (int)shiftKey_);
            XRCCTRL(*this, buttonName, wxButton)->SetLabel("Invalid Key");
            return;
        }
        if (key == 32)
            key = 0;
		setLabel("KeyShift%02X", shiftKey_, key);
        keyDefShift_[shiftKey_] = key;
		shiftKey_ = -1;
	}
	updateButtons();
}

void KeyMapTmcDialog::connectKeyDownEvent(wxWindow* pclComponent) 
{ 
  if(pclComponent) 
  { 
    pclComponent->Connect(wxID_ANY, 
                          wxEVT_CHAR,
                          wxKeyEventHandler(KeyMapTmcDialog::onKeyDown),
                          (wxObject*) NULL, 
                          this); 

    wxWindowListNode* pclNode = pclComponent->GetChildren().GetFirst(); 
    while(pclNode) 
    { 
      wxWindow* pclChild = pclNode->GetData(); 
      this->connectKeyDownEvent(pclChild); 
      
      pclNode = pclNode->GetNext(); 
    } 
  } 
} 

void KeyMapTmcDialog::setLabel(wxString printStr, long button, int key)
{
	wxString keyStr, keyStrNum, buttonStr;

    switch (key)
    {
        case WXK_CONTROL_A:
            keyStr = "Control A";
            break;
            
        case WXK_CONTROL_B:
            keyStr = "Control B";
            break;
            
        case WXK_CONTROL_C:
            keyStr = "Control C";
            break;
            
        case WXK_CONTROL_D:
            keyStr = "Control D";
            break;
            
        case WXK_CONTROL_E:
            keyStr = "Control E";
            break;
            
        case WXK_CONTROL_F:
            keyStr = "Control F";
            break;
            
        case WXK_CONTROL_G:
            keyStr = "Control G";
            break;
            
        case WXK_CONTROL_J:
            keyStr = "Control J";
            break;
            
        case WXK_CONTROL_K:
            keyStr = "Control K";
            break;
            
        case WXK_CONTROL_L:
            keyStr = "Control L";
            break;
            
        case WXK_CONTROL_N:
            keyStr = "Control N";
            break;
            
        case WXK_CONTROL_O:
            keyStr = "Control O";
            break;
            
        case WXK_CONTROL_P:
            keyStr = "Control P";
            break;
            
        case WXK_CONTROL_Q:
            keyStr = "Control Q";
            break;
            
        case WXK_CONTROL_R:
            keyStr = "Control R";
            break;
            
        case WXK_CONTROL_S:
            keyStr = "Control S";
            break;
            
        case WXK_CONTROL_T:
            keyStr = "Control T";
            break;
            
        case WXK_CONTROL_U:
            keyStr = "Control U";
            break;
            
        case WXK_CONTROL_V:
            keyStr = "Control V";
            break;
            
        case WXK_CONTROL_W:
            keyStr = "Control W";
            break;
            
        case WXK_CONTROL_X:
            keyStr = "Control X";
            break;
            
        case WXK_CONTROL_Y:
            keyStr = "Control Y";
            break;
            
        case WXK_CONTROL_Z:
            keyStr = "Control Z";
            break;
            
        case WXK_NUMPAD0:
            keyStr = "Pd 0";
            break;
            
        case WXK_NUMPAD1:
            keyStr = "Pd 1";
            break;
            
        case WXK_NUMPAD2:
            keyStr = "Pd 2";
            break;
            
        case WXK_NUMPAD3:
            keyStr = "Pd 3";
            break;
            
        case WXK_NUMPAD4:
            keyStr = "Pd 4";
            break;
            
        case WXK_NUMPAD5:
            keyStr = "Pd 5";
            break;
            
        case WXK_NUMPAD6:
            keyStr = "Pd 6";
            break;
            
        case WXK_NUMPAD7:
            keyStr = "Pd 7";
            break;
            
        case WXK_NUMPAD8:
            keyStr = "Pd 8";
            break;
            
        case WXK_NUMPAD9:
            keyStr = "Pd 9";
            break;
            
        case WXK_BACK:
#ifdef __WXMAC__
            keyStr = "Delete";
#else
            keyStr = "Back";
#endif
            break;
            
        case WXK_TAB:
            keyStr = "Tab";
            break;
            
        case WXK_RETURN:
            keyStr = "Return";
            break;
            
        case WXK_ESCAPE:
            keyStr = "ESC";
            break;
            
        case WXK_DELETE:
            keyStr = "Delete";
            break;
            
        case WXK_SPACE:
            keyStr = "Space";
            break;
            
        case WXK_SHIFT:
            keyStr = "Shift";
            break;
            
        case WXK_START:
            keyStr = "START";
            break;
        case WXK_LBUTTON:
            keyStr = "LButton";
            break;
        case WXK_RBUTTON:
            keyStr = "RButton";
            break;
        case WXK_CANCEL:
            keyStr = "Cancel";
            break;
        case WXK_MBUTTON:
            keyStr = "MButton";
            break;
        case WXK_CLEAR:
            keyStr = "Clear";
            break;
        case WXK_ALT:
#ifdef __WXMAC__
            keyStr = "Option";
#else
            keyStr = "Alt";
#endif
            break;
        case WXK_CONTROL:
#ifdef __WXMAC__
            keyStr = "Command";
#else
            keyStr = "Control";
#endif
            break;
#ifdef __WXMAC__
		case WXK_RAW_CONTROL:
            keyStr = "Control";
        break;
#endif
        case WXK_MENU:
            keyStr = "Menu";
            break;
        case WXK_PAUSE:
            keyStr = "Pause";
            break;
        case WXK_CAPITAL:
            keyStr = "Caps";
            break;
        case WXK_END:
            keyStr = "End";
            break;
        case WXK_HOME:
            keyStr = "Home";
            break;
        case WXK_LEFT:
            keyStr = "Left";
            break;
        case WXK_UP:
            keyStr = "Up";
            break;
        case WXK_RIGHT:
            keyStr = "Right";
            break;
        case WXK_DOWN:
            keyStr = "Down";
            break;
        case WXK_SELECT:
            keyStr = "Select";
            break;
        case WXK_PRINT:
            keyStr = "Print";
            break;
        case WXK_EXECUTE:
            keyStr = "Exec";
            break;
        case WXK_SNAPSHOT:
            keyStr = "Snap";
            break;
        case WXK_INSERT:
            keyStr = "Ins";
            break;
        case WXK_HELP:
            keyStr = "Help";
            break;
        case WXK_MULTIPLY:
            keyStr = "*";
            break;
        case WXK_ADD:
            keyStr = "+";
            break;
        case WXK_SEPARATOR:
            keyStr = "Sep";
            break;
        case WXK_SUBTRACT:
            keyStr = "-";
            break;
        case WXK_DECIMAL:
            keyStr = ",";
            break;
        case WXK_DIVIDE:
            keyStr = "/";
            break;
        case WXK_F1:
            keyStr = "F1";
            break;
        case WXK_F2:
            keyStr = "F2";
            break;
        case WXK_F3:
            keyStr = "F3";
            break;
        case WXK_F4:
            keyStr = "F4";
            break;
        case WXK_F5:
            keyStr = "F5";
            break;
        case WXK_F6:
            keyStr = "F6";
            break;
        case WXK_F7:
            keyStr = "F7";
            break;
        case WXK_F8:
            keyStr = "F8";
            break;
        case WXK_F9:
            keyStr = "F9";
            break;
        case WXK_F10:
            keyStr = "F10";
            break;
        case WXK_F11:
            keyStr = "F11";
            break;
        case WXK_F12:
            keyStr = "F12";
            break;
        case WXK_F13:
            keyStr = "F13";
            break;
        case WXK_F14:
            keyStr = "F14";
            break;
        case WXK_F15:
            keyStr = "F15";
            break;
        case WXK_F16:
            keyStr = "F16";
            break;
        case WXK_F17:
            keyStr = "F17";
            break;
        case WXK_F18:
            keyStr = "F18";
            break;
        case WXK_F19:
            keyStr = "F19";
            break;
        case WXK_F20:
            keyStr = "F20";
            break;
        case WXK_F21:
            keyStr = "F21";
            break;
        case WXK_F22:
            keyStr = "F22";
            break;
        case WXK_F23:
            keyStr = "F23";
            break;
        case WXK_F24:
            keyStr = "F24";
            break;
        case WXK_NUMLOCK:
            keyStr = "NumLock";
            break;
        case WXK_SCROLL:
            keyStr = "Scroll";
            break;
        case WXK_PAGEUP:
            keyStr = "PgUp";
            break;
        case WXK_PAGEDOWN:
            keyStr = "PgDn";
            break;
        case WXK_NUMPAD_SPACE:
            keyStr = "Pd space";
            break;
        case WXK_NUMPAD_TAB:
            keyStr = "Pd tab";
            break;
        case WXK_NUMPAD_ENTER:
            keyStr = "Enter";
            break;
        case WXK_NUMPAD_F1:
            keyStr = "Pd F1";
            break;
        case WXK_NUMPAD_F2:
            keyStr = "Pd F2";
            break;
        case WXK_NUMPAD_F3:
            keyStr = "Pd F3";
            break;
        case WXK_NUMPAD_F4:
            keyStr = "Pd F4";
            break;
        case WXK_NUMPAD_HOME:
            keyStr = "Pd Home";
            break;
        case WXK_NUMPAD_LEFT:
            keyStr = "Pd Left";
            break;
        case WXK_NUMPAD_UP:
            keyStr = "Pd Up";
            break;
        case WXK_NUMPAD_RIGHT:
            keyStr = "Pd Right";
            break;
        case WXK_NUMPAD_DOWN:
            keyStr = "Pd Dwn";
            break;
        case WXK_NUMPAD_PAGEUP:
            keyStr = "Pd PgUp";
            break;
        case WXK_NUMPAD_PAGEDOWN:
            keyStr = "Pd PgDn";
            break;
        case WXK_NUMPAD_END:
            keyStr = "Pd End";
            break;
        case WXK_NUMPAD_BEGIN:
            keyStr = "Pd Bgn";
            break;
        case WXK_NUMPAD_INSERT:
            keyStr = "Pd Ins";
            break;
        case WXK_NUMPAD_DELETE:
            keyStr = "Pd Del";
            break;
        case WXK_NUMPAD_EQUAL:
            keyStr = "Pd =";
            break;
        case WXK_NUMPAD_MULTIPLY:
            keyStr = "Pd *";
            break;
        case WXK_NUMPAD_ADD:
            keyStr = "Pd +";
            break;
        case WXK_NUMPAD_SEPARATOR:
            keyStr = "Pd Sep";
            break;
        case WXK_NUMPAD_SUBTRACT:
            keyStr = "Pd -";
            break;
        case WXK_NUMPAD_DECIMAL:
            keyStr = "Pd ,";
            break;
        case WXK_NUMPAD_DIVIDE:
            keyStr = "Pd /";
            break;
        case WXK_WINDOWS_LEFT:
            keyStr = "Win Left";
            break;
        case WXK_WINDOWS_RIGHT:
            keyStr = "Win Right";
            break;
        case WXK_WINDOWS_MENU:
            keyStr = "Menu";
            break;
        default:
            keyStr.Printf("%c", key);
            break;
    }
    keyStrNum.Printf("(%i)", key);
    keyStr = keyStr + keyStrNum;
    
    if (key == 0)
        keyStr = "";
    
    buttonStr.Printf(printStr, (int)button);

    XRCCTRL(*this, buttonStr, wxButton)->SetLabel(keyStr);
#ifdef __WXMAC__
    XRCCTRL(*this, buttonStr, wxButton)->SetBackgroundColour(wxNullColour);
#else
	XRCCTRL(*this, buttonStr, wxButton)->SetForegroundColour(wxNullColour);
#endif
}

void KeyMapTmcDialog::onFocusTimer(wxTimerEvent& WXUNUSED(event))
{
    XRCCTRL(*this, "Focus", wxTextCtrl)->SetFocus();
}


