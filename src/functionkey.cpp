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
#include "functionkey.h"

#include "wx/xrc/xmlres.h"             

int keyUsed[] =
{// 0  1  2  3  4  5  6  7  8  9
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, // 0 - 9
	1, 0, 0, 1, 0, 0, 0, 0, 0, 0, // 10 - 19
	0, 0, 0, 0, 0, 0, 0, 1, 0, 0, // 20 - 29
	0, 0, 1, 1, 1, 1, 1, 1, 1, 0, // 30 - 39
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 40 - 49
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 50 - 59
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 60 - 69
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 70 - 79
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 80 - 89
	1, 1, 1, 1, 1, 1, 0, 1, 1, 1, // 90 - 99
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 100 - 109
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 110 - 119
	1, 1, 0, 0, 0, 0, 0, 1, 0, 0, // 120 - 129
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 130 - 139
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 140 - 149
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 150 - 159
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 160 - 169
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 170 - 179
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 180 - 189
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 190 - 199
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 200 - 209
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 210 - 219
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 220 - 229
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 230 - 239
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 240 - 249
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 250 - 259
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 260 - 269
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 270 - 279
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 280 - 289
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 290 - 299
	0, 0, 0, 0, 0, 0, 1, 1, 1, 0, // 300 - 309
	0, 1, 1, 1, 1, 1, 1, 1, 0, 0, // 310 - 319
	0, 1, 0, 1, 1, 1, 1, 1, 1, 1, // 320 - 329
	1, 1, 1, 0, 0, 0, 0, 0, 0, 0, // 330 - 339
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 340 - 349
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 350 - 359
	0, 0, 0, 0, 0, 1, 0, 0, 0, 0, // 360 - 369
	1, 0, 0, 0, 0, 0, 1, 1, 1, 1, // 370 - 379
	0, 0, 0, 0, 0, 0, 0, 1, 1, 0, // 380 - 389
	1, 1, 1, 0, 0, 0, 0, 0, 0, 0, // 390 - 399
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 400 - 409
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 410 - 419
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 420 - 429
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 430 - 439
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 440 - 449
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 450 - 459
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 460 - 469
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 470 - 479
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 480 - 489
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 490 - 499
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 500 - 509
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 510 - 519
};

BEGIN_EVENT_TABLE(FunctionKeyMapDialog, wxDialog)
   EVT_BUTTON(XRCID("Save"), FunctionKeyMapDialog::onSaveButton)
   EVT_BUTTON(XRCID("Default"), FunctionKeyMapDialog::onDefault)
   EVT_BUTTON(XRCID("FunctionKey0"), FunctionKeyMapDialog::onFunctionKey)
   EVT_BUTTON(XRCID("FunctionKey1"), FunctionKeyMapDialog::onFunctionKey)
   EVT_BUTTON(XRCID("FunctionKey2"), FunctionKeyMapDialog::onFunctionKey)
   EVT_BUTTON(XRCID("FunctionKey3"), FunctionKeyMapDialog::onFunctionKey)
   EVT_BUTTON(XRCID("FunctionKey4"), FunctionKeyMapDialog::onFunctionKey)
   EVT_BUTTON(XRCID("FunctionKey5"), FunctionKeyMapDialog::onFunctionKey)
   EVT_BUTTON(XRCID("FunctionKey6"), FunctionKeyMapDialog::onFunctionKey)
   EVT_BUTTON(XRCID("FunctionKey7"), FunctionKeyMapDialog::onFunctionKey)
   EVT_BUTTON(XRCID("FunctionKey8"), FunctionKeyMapDialog::onFunctionKey)
   EVT_BUTTON(XRCID("FunctionKeyC"), FunctionKeyMapDialog::onFunctionKey)
   EVT_CHECKBOX(XRCID("EnableEscape"), FunctionKeyMapDialog::onEnableEscape)
#ifdef __WXMAC__
   EVT_TIMER(wxID_ANY, FunctionKeyMapDialog::onFocusTimer)
#endif
END_EVENT_TABLE()

FunctionKeyMapDialog::FunctionKeyMapDialog(wxWindow* parent)
{
	wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"functionkey.xrc");
	wxXmlResource::Get()->LoadDialog(this, parent, wxT("FunctionKeyDialog"));

	fKey_ = -1;

	XRCCTRL(*this, "Save", wxButton)->SetFocus();

	for (int i=0; i<13; i++)
		functionKey_[i] = p_Main->getFunctionKey(i);

	this->connectKeyDownEvent(this);
    
    useExitKey_ = p_Main->getUseExitKey();
	XRCCTRL(*this, "EnableEscape", wxCheckBox)->SetValue(useExitKey_);
	XRCCTRL(*this, "FunctionKey0", wxButton)->Enable(useExitKey_);

    LoadAndCompare(ELF, "Elf");
    LoadAndCompare(ELFII, "ElfII");
    LoadAndCompare(SUPERELF, "SuperElf");
    LoadAndCompare(ELF2K, "Elf2K");
    LoadAndCompare(COSMICOS, "Cosmicos");
    LoadAndCompare(ETI, "Eti");
    LoadAndCompare(TMC1800, "TMC1800");
    LoadAndCompare(TMC2000, "TMC2000");
    LoadAndCompare(NANO, "Nano");
    LoadAndCompare(VIP, "Vip");
    LoadAndCompare(VIP2K, "Vip2K");
    LoadAndCompare(VELF, "Velf");
    LoadAndCompare(FRED1, "FRED1");
    LoadAndCompare(FRED2, "FRED2");
    LoadAndCompareStudio(STUDIO, "Studio2");
    LoadAndCompareStudio(COINARCADE, "CoinArcade");
    LoadAndCompareStudio(VISICOM, "Visicom");
    LoadAndCompareStudio(VICTORY, "Victory");
    LoadAndCompareStudio(STUDIOIV, "StudioIV");
    
	updateButtons();
    
#ifdef __WXMAC__
    focusTimer = new wxTimer(this, wxID_ANY);
    focusTimer->Start(1000, wxTIMER_CONTINUOUS);
#endif
}

FunctionKeyMapDialog::~FunctionKeyMapDialog()
{
#ifdef __WXMAC__
    focusTimer->Stop();
    delete focusTimer;
#endif
}

void FunctionKeyMapDialog::LoadAndCompare(int computerType, wxString computer)
{
    int dummy[5];
    int hexKeyDef1[16];
    int hexKeyDef2[16];

    p_Main->getDefaultHexKeys(computerType, computer, "A", hexKeyDef1, hexKeyDef2, dummy);
	for (int i=0; i<16; i++)
    {
        keyUsed[hexKeyDef1[i]] = 1;
        keyUsed[hexKeyDef2[i]] = 1;
    }
    p_Main->getDefaultHexKeys(computerType, computer, "B", hexKeyDef1, hexKeyDef2, dummy);
	for (int i=0; i<16; i++)
    {
        keyUsed[hexKeyDef1[i]] = 1;
        keyUsed[hexKeyDef2[i]] = 1;
    }
}

void FunctionKeyMapDialog::LoadAndCompareStudio(int computerType, wxString computer)
{
    int dummy[5];
	int hexKeyDef1[10];
	int hexKeyDef2[10];

    p_Main->getDefaultHexKeys(computerType, computer, "A", hexKeyDef1, hexKeyDef2, dummy);
    for (int i=0; i<10; i++)
    {
        keyUsed[hexKeyDef1[i]] = 1;
        keyUsed[hexKeyDef2[i]] = 1;
    }
    p_Main->getDefaultHexKeys(computerType, computer, "B", hexKeyDef1, hexKeyDef2, dummy);
    for (int i=0; i<10; i++)
    {
        keyUsed[hexKeyDef1[i]] = 1;
        keyUsed[hexKeyDef2[i]] = 1;
    }
}

void FunctionKeyMapDialog::compareButtons(int toBeCheckedButton, int toBeCheckedValue)
{
	wxString button;
	if (keyUsed[toBeCheckedValue] != 0)
	{
		button.Printf("FunctionKey%01X", toBeCheckedButton);
#ifdef __WXMAC__
        XRCCTRL(*this, button, wxButton)->SetBackgroundColour(wxColour(255,128,0));
#else
        XRCCTRL(*this, button, wxButton)->SetForegroundColour(wxColour(255,128,0));
#endif
	}
	for (int i=0; i<13; i++)
	{
		if (i != toBeCheckedButton && functionKey_[i] == toBeCheckedValue)
		{
			button.Printf("FunctionKey%01X", toBeCheckedButton);
#ifdef __WXMAC__
            XRCCTRL(*this, button, wxButton)->SetBackgroundColour(wxColour(255,0,0));
#else
			XRCCTRL(*this, button, wxButton)->SetForegroundColour(wxColour(255,0,0));
#endif
		}
	}
}

void FunctionKeyMapDialog::updateButtons()
{
	setLabel("FunctionKey%01X", 0, functionKey_[0]);
	compareButtons(0, functionKey_[0]);
	setLabel("FunctionKey%01X", 1, functionKey_[1]);
	compareButtons(1, functionKey_[1]);
	setLabel("FunctionKey%01X", 2, functionKey_[2]);
	compareButtons(2, functionKey_[2]);
	setLabel("FunctionKey%01X", 3, functionKey_[3]);
	compareButtons(3, functionKey_[3]);
	setLabel("FunctionKey%01X", 4, functionKey_[4]);
	compareButtons(4, functionKey_[4]);
	setLabel("FunctionKey%01X", 5, functionKey_[5]);
	compareButtons(5, functionKey_[5]);
	setLabel("FunctionKey%01X", 6, functionKey_[6]);
	compareButtons(6, functionKey_[6]);
    setLabel("FunctionKey%01X", 7, functionKey_[7]);
    compareButtons(7, functionKey_[7]);
    setLabel("FunctionKey%01X", 8, functionKey_[8]);
    compareButtons(8, functionKey_[8]);
	setLabel("FunctionKey%01X", 12, functionKey_[12]);
	compareButtons(12, functionKey_[12]);
}

void FunctionKeyMapDialog::onSaveButton( wxCommandEvent& WXUNUSED(event) )
{
	for (int i=0; i<13; i++)
		p_Main->setFunctionKey(i, functionKey_[i]);

	 p_Main->setUseExitKey(useExitKey_);

	EndModal( wxID_OK );
}

void FunctionKeyMapDialog::onFunctionKey(wxCommandEvent &event)
{
    wxString buttonName, buttonNumber;
    
    if (fKey_ != -1)
        setLabel("FunctionKey%01X", fKey_, functionKey_[fKey_]);
    
    buttonName = wxWindow::FindWindowById(event.GetId())->GetName();
    buttonNumber = buttonName.Last();
    if (buttonNumber.ToLong(&fKey_, 16))
        XRCCTRL(*this, buttonName, wxButton)->SetLabel("Press Key");
    else
        fKey_ = -1;
}

void FunctionKeyMapDialog::onInputFunctionKey(wxCommandEvent &event)
{
    wxString textName, textNumber;
    long key;
 
    textName = wxWindow::FindWindowById(event.GetId())->GetName();
    textNumber = textName.Last();
  
    if (textNumber.ToLong(&fKey_, 16))
    {
        wxString keyString = XRCCTRL(*this, textName, wxTextCtrl)->GetValue();
        if (keyString.ToLong(&key))
        {
            functionKey_[fKey_] = (int) key;
            setLabel("FunctionKey%01X", fKey_, functionKey_[fKey_]);
            updateButtons();
        }
    }
    else
        fKey_ = -1;
}

void FunctionKeyMapDialog::onFocusTimer(wxTimerEvent& WXUNUSED(event))
{
    XRCCTRL(*this, "Focus", wxTextCtrl)->SetFocus();
}

void FunctionKeyMapDialog::onDefault(wxCommandEvent& WXUNUSED(event))
{
	functionKey_[0] = WXK_ESCAPE;
	functionKey_[1] = WXK_F1;
	functionKey_[2] = WXK_F2;
	functionKey_[3] = WXK_F3;
	functionKey_[4] = WXK_F4;
	functionKey_[5] = WXK_F5;
	functionKey_[6] = WXK_F6;
    functionKey_[7] = WXK_F7;
	functionKey_[8] = WXK_F8;
	functionKey_[9] = WXK_F9;
	functionKey_[10] = WXK_F10;
	functionKey_[11] = WXK_F11;
	functionKey_[12] = WXK_F12;

	updateButtons();
	fKey_ = -1;
}

void FunctionKeyMapDialog::onKeyDown(wxKeyEvent &event)
{
	int key = event.GetKeyCode();
	if (key == WXK_SHIFT)
		return;

	if (fKey_ != -1)
	{
		setLabel("FunctionKey%01X", fKey_, key);
		functionKey_[fKey_] = key;
	}

	updateButtons();
}

void FunctionKeyMapDialog::connectKeyDownEvent(wxWindow* pclComponent) 
{ 
  if(pclComponent)
  { 
    pclComponent->Connect(wxID_ANY,
                          wxEVT_KEY_DOWN, 
                          wxKeyEventHandler(FunctionKeyMapDialog::onKeyDown),
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

void FunctionKeyMapDialog::setLabel(wxString printStr, long button, int key)
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
    
	if (button < 16)
		buttonStr.Printf(printStr, (int)button);
	else
		buttonStr = printStr;
    
	XRCCTRL(*this, buttonStr, wxButton)->SetLabel(keyStr);
#ifdef __WXMAC__
	XRCCTRL(*this, buttonStr, wxButton)->SetBackgroundColour(wxNullColour);
#else
    XRCCTRL(*this, buttonStr, wxButton)->SetForegroundColour(wxNullColour);
#endif
}

void FunctionKeyMapDialog::onEnableEscape(wxCommandEvent&event)
{
	useExitKey_ = event.IsChecked();
	XRCCTRL(*this, "FunctionKey0", wxButton)->Enable(useExitKey_);
}
