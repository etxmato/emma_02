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
#include "keymap.h"

#define KEYPAD_HEX_A1 0
#define KEYPAD_GAME_A 1
#define KEYPAD_HEX_B1 2
#define KEYPAD_GAME_B 3
#define IN_BUTTON_1 4
#define KEYPAD_HEX_A2 5
#define KEYPAD_HEX_B2 6
#define IN_BUTTON_2 7

#include "wx/xrc/xmlres.h"             
#include <wx/statline.h>

BEGIN_EVENT_TABLE(KeyMapDialog, wxDialog)
   EVT_BUTTON(XRCID("HexSave"), KeyMapDialog::onSaveButton)
   EVT_BUTTON(XRCID("CoinSave"), KeyMapDialog::onSaveArcadeCoin)
   EVT_BUTTON(XRCID("CoinDefault"), KeyMapDialog::onDefaultArcadeCoin)
   EVT_BUTTON(XRCID("HexSwitchPad"), KeyMapDialog::onSwitchPad)
   EVT_BUTTON(XRCID("HexSwitchPlayer"), KeyMapDialog::onSwitchPlayer)
   EVT_BUTTON(XRCID("HexLocation"), KeyMapDialog::onHexLocation)
   EVT_BUTTON(XRCID("HexChar"), KeyMapDialog::onHexChar)
   EVT_BUTTON(XRCID("TinyBASIC"), KeyMapDialog::onTinyBASIC)
   EVT_BUTTON(XRCID("HexSwitchSet1"), KeyMapDialog::onHexSwitchSet1)
   EVT_BUTTON(XRCID("HexSwitchSet2"), KeyMapDialog::onHexSwitchSet2)
   EVT_BUTTON(XRCID("StudioSwitchSet"), KeyMapDialog::onSwitchSet)
   EVT_BUTTON(XRCID("StudioSwitchPad"), KeyMapDialog::onSwitchStudio)
   EVT_BUTTON(XRCID("StudioLocation"), KeyMapDialog::onStudioLocation)
   EVT_BUTTON(XRCID("StudioChar"), KeyMapDialog::onStudioChar)
   EVT_BUTTON(XRCID("InButton"), KeyMapDialog::onInButton)
   EVT_BUTTON(XRCID("HexKey0"), KeyMapDialog::onHexKey)
   EVT_BUTTON(XRCID("HexKey1"), KeyMapDialog::onHexKey)
   EVT_BUTTON(XRCID("HexKey2"), KeyMapDialog::onHexKey)
   EVT_BUTTON(XRCID("HexKey3"), KeyMapDialog::onHexKey)
   EVT_BUTTON(XRCID("HexKey4"), KeyMapDialog::onHexKey)
   EVT_BUTTON(XRCID("HexKey5"), KeyMapDialog::onHexKey)
   EVT_BUTTON(XRCID("HexKey6"), KeyMapDialog::onHexKey)
   EVT_BUTTON(XRCID("HexKey7"), KeyMapDialog::onHexKey)
   EVT_BUTTON(XRCID("HexKey8"), KeyMapDialog::onHexKey)
   EVT_BUTTON(XRCID("HexKey9"), KeyMapDialog::onHexKey)
   EVT_BUTTON(XRCID("HexKeyA"), KeyMapDialog::onHexKey)
   EVT_BUTTON(XRCID("HexKeyB"), KeyMapDialog::onHexKey)
   EVT_BUTTON(XRCID("HexKeyC"), KeyMapDialog::onHexKey)
   EVT_BUTTON(XRCID("HexKeyD"), KeyMapDialog::onHexKey)
   EVT_BUTTON(XRCID("HexKeyE"), KeyMapDialog::onHexKey)
   EVT_BUTTON(XRCID("HexKeyF"), KeyMapDialog::onHexKey)
   EVT_BUTTON(XRCID("KeyTextA0"), KeyMapDialog::onArcadeADirKey)
   EVT_BUTTON(XRCID("KeyTextA1"), KeyMapDialog::onArcadeADirKey)
   EVT_BUTTON(XRCID("KeyTextA2"), KeyMapDialog::onArcadeADirKey)
   EVT_BUTTON(XRCID("KeyTextA3"), KeyMapDialog::onArcadeADirKey)
   EVT_BUTTON(XRCID("KeyTextA4"), KeyMapDialog::onArcadeADirKey)
   EVT_BUTTON(XRCID("KeyTextB0"), KeyMapDialog::onArcadeBDirKey)
   EVT_BUTTON(XRCID("KeyTextB1"), KeyMapDialog::onArcadeBDirKey)
   EVT_BUTTON(XRCID("KeyTextB2"), KeyMapDialog::onArcadeBDirKey)
   EVT_BUTTON(XRCID("KeyTextB3"), KeyMapDialog::onArcadeBDirKey)
   EVT_BUTTON(XRCID("KeyTextB4"), KeyMapDialog::onArcadeBDirKey)
   EVT_BUTTON(XRCID("CoinButton"), KeyMapDialog::onArcadeCoinKey)
   EVT_CHECKBOX(XRCID("GameAuto"), KeyMapDialog::onAuto)
   EVT_CHECKBOX(XRCID("StudioEnableDiagonal"), KeyMapDialog::onStudioEnableDiagonal)
#ifdef __WXMAC__
   EVT_TIMER(wxID_ANY, KeyMapDialog::onFocusTimer)
#endif
END_EVENT_TABLE()

KeyMapDialog::KeyMapDialog(wxWindow* parent)
{
    int dummy[5];
    
	computerTypeStr_ = p_Main->getSelectedComputerStr();
	int computerType = p_Main->getSelectedComputerId();

    arcadeADirKey_ = -1;
    arcadeBDirKey_ = -1;
    arcadeCoinKey_ = -1;
    hexKey_ = -1;
	inKey_ = -1;
	numberOfKeys_ = 16;
	hexPadBdefined_ = false;
	player2defined_ = false;

	for (int i=0; i<16; i++)
	{
        hexKeyDefA1_[i] = 0;
        hexKeyDefB1_[i] = 0;
        hexKeyDefA2_[i] = 0;
        hexKeyDefB2_[i] = 0;
	}
	hexPadA_ = true;
    hexPadSet1_ = true;
    gamePlayer1_ = true;
    inButton1_ = 255;
    inButton2_ = 255;

	wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"keymap.xrc");
    
    keyDefGameHexA_[0] = 2;
    keyDefGameHexA_[1] = 4;
    keyDefGameHexA_[2] = 6;
    keyDefGameHexA_[3] = 8;
    keyDefGameHexA_[4] = 5;
    keyDefGameHexB_[0] = 0xc;
    keyDefGameHexB_[1] = 7;
    keyDefGameHexB_[2] = 9;
    keyDefGameHexB_[3] = 0xd;
    keyDefGameHexB_[4] = 0;

    switch (computerType)
	{
		case ELF:
		case ELFII:
		case SUPERELF:
		case ETI: // Add Member GUI for IN Button
            keyDefGameHexA_[0] = 0xd;
            keyDefGameHexA_[1] = 8;
            keyDefGameHexA_[2] = 0xa;
            keyDefGameHexA_[3] = 5;
            keyDefGameHexA_[4] = 9;
            keyDefGameHexB_[0] = 0xf;
            keyDefGameHexB_[1] = 4;
            keyDefGameHexB_[2] = 6;
            keyDefGameHexB_[3] = 0xb;
            keyDefGameHexB_[4] = 1;

            wxXmlResource::Get()->LoadDialog(this, parent, wxT("KeyMapDialog2"));
			player2defined_ = true;
			p_Main->getDefaultHexKeys(computerType, computerTypeStr_, "A", hexKeyDefA1_, hexKeyDefA2_, dummy);
			autoGame_ = p_Main->getConfigBool(computerTypeStr_+"/GameAuto", true);
			XRCCTRL(*this, "GameAuto", wxCheckBox)->SetValue(autoGame_);
            inButton1_ = p_Main->getDefaultInKey1(computerTypeStr_);
            inButton2_ = p_Main->getDefaultInKey2(computerTypeStr_);
		break;

		case ELF2K:
		case COSMICOS:
            keyDefGameHexA_[0] = 8;
            keyDefGameHexA_[1] = 4;
            keyDefGameHexA_[2] = 6;
            keyDefGameHexA_[3] = 2;
            keyDefGameHexA_[4] = 5;
            keyDefGameHexB_[0] = 9;
            keyDefGameHexB_[1] = 5;
            keyDefGameHexB_[2] = 0xe;
            keyDefGameHexB_[3] = 3;
            keyDefGameHexB_[4] = 6;
            
			wxXmlResource::Get()->LoadDialog(this, parent, wxT("KeyMapDialog2"));
            XRCCTRL(*this, "GameAuto", wxCheckBox)->Hide();
            XRCCTRL(*this, "GameAutoLine", wxStaticLine)->Hide();
            autoGame_ = false;
			player2defined_ = true;
			p_Main->getDefaultHexKeys(computerType, computerTypeStr_, "A", hexKeyDefA1_, hexKeyDefA2_, dummy);
            inButton1_ = p_Main->getDefaultInKey1(computerTypeStr_);
            inButton2_ = p_Main->getDefaultInKey2(computerTypeStr_);
		break;

        case FRED1:
        case FRED1_5:
            keyDefGameHexA_[0] = 0xd;
            keyDefGameHexA_[1] = 8;
            keyDefGameHexA_[2] = 0xa;
            keyDefGameHexA_[3] = 5;
            keyDefGameHexA_[4] = 9;
            keyDefGameHexB_[0] = 0xf;
            keyDefGameHexB_[1] = 4;
            keyDefGameHexB_[2] = 6;
            keyDefGameHexB_[3] = 0xb;
            keyDefGameHexB_[4] = 1;
            
            wxXmlResource::Get()->LoadDialog(this, parent, wxT("KeyMapDialog4"));
            XRCCTRL(*this, "GameAutoLine", wxStaticLine)->Hide();
            autoGame_ = p_Main->getConfigBool(computerTypeStr_+"/GameAuto", true);
            XRCCTRL(*this, "GameAuto", wxCheckBox)->SetValue(autoGame_);
            player2defined_ = false;
            p_Main->getDefaultHexKeys(computerType, computerTypeStr_, "A", hexKeyDefA1_, hexKeyDefA2_, dummy);
            
            XRCCTRL(*this, "InButton", wxButton)->Hide();
            XRCCTRL(*this, "InButtonText", wxStaticText)->Hide();
            inButton1_ = 0;
            inButton2_ = 0;
        break;
           
		case UC1800:
            keyDefGameHexA_[0] = 0xd;
            keyDefGameHexA_[1] = 8;
            keyDefGameHexA_[2] = 0xa;
            keyDefGameHexA_[3] = 5;
            keyDefGameHexA_[4] = 9;
            keyDefGameHexB_[0] = 0xf;
            keyDefGameHexB_[1] = 4;
            keyDefGameHexB_[2] = 6;
            keyDefGameHexB_[3] = 0xb;
            keyDefGameHexB_[4] = 1;
            
            wxXmlResource::Get()->LoadDialog(this, parent, wxT("KeyMapDialogUC1800"));
            XRCCTRL(*this, "GameAutoLine", wxStaticLine)->Hide();
            autoGame_ = p_Main->getConfigBool(computerTypeStr_+"/GameAuto", false);
            XRCCTRL(*this, "GameAuto", wxCheckBox)->Hide();
            player2defined_ = false;
            p_Main->getDefaultHexKeys(computerType, computerTypeStr_, "A", hexKeyDefA1_, hexKeyDefA2_, dummy);
            
            inButton1_ = p_Main->getDefaultInKey1(computerTypeStr_);
            inButton2_ = p_Main->getDefaultInKey2(computerTypeStr_, 313);
        break;

        case COINARCADE:
            wxXmlResource::Get()->LoadDialog(this, parent, wxT("KeyMapDialog5"));
            keyDefCoin_ = p_Main->getDefaultCoinArcadeKeys(keyDefGameHexA_, keyDefGameHexB_);
        break;
            
        case STUDIO:
		case VISICOM:
        case VICTORY:
            keyDefGameHexA_[4] = 0;
            keyDefGameHexB_[0] = 2;
            keyDefGameHexB_[1] = 4;
            keyDefGameHexB_[2] = 6;
            keyDefGameHexB_[3] = 8;
            keyDefGameHexB_[4] = 0;

            wxXmlResource::Get()->LoadDialog(this, parent, wxT("KeyMapDialog3"));
			hexPadBdefined_ = true;
            p_Main->getDefaultHexKeys(computerType, computerTypeStr_, "A", hexKeyDefA1_, hexKeyDefA2_, dummy);
            p_Main->getDefaultHexKeys(computerType, computerTypeStr_, "B", hexKeyDefB1_, hexKeyDefB2_, dummy);
			numberOfKeys_ = 10;
			autoGame_ = p_Main->getConfigBool(computerTypeStr_+"/GameAuto", true);
			XRCCTRL(*this, "GameAuto", wxCheckBox)->SetValue(autoGame_);
        break;

		case TMC1800:
		case TMC2000:
		case NANO:
			wxXmlResource::Get()->LoadDialog(this, parent, wxT("KeyMapDialog1"));
            XRCCTRL(*this, "InButton", wxButton)->Hide();
            XRCCTRL(*this, "InButtonText", wxStaticText)->Hide();
			player2defined_ = true;
			XRCCTRL(*this, "PadText", wxStaticText)->SetLabel("Hex Keypad Definition, Set 1");
			XRCCTRL(*this, "GamePlayer", wxStaticText)->SetLabel("Arcade Key Definition: Player 1");
			XRCCTRL(*this, "HexSwitchPad", wxButton)->Hide();
			p_Main->getDefaultHexKeys(computerType, computerTypeStr_, "A", hexKeyDefA1_, hexKeyDefA2_, dummy);
			autoGame_ = p_Main->getConfigBool(computerTypeStr_+"/GameAuto", true);
			XRCCTRL(*this, "GameAuto", wxCheckBox)->SetValue(autoGame_);
		break;

        case VIP:
        case VIP2K:
			wxXmlResource::Get()->LoadDialog(this, parent, wxT("KeyMapDialog1"));
            XRCCTRL(*this, "InButton", wxButton)->Hide();
            XRCCTRL(*this, "InButtonText", wxStaticText)->Hide();
            p_Main->getDefaultHexKeys(computerType, computerTypeStr_, "A", hexKeyDefA1_, hexKeyDefA2_, dummy);
			if (!p_Main->getVipVp590() && !p_Main->getVipVp580())
			{
                XRCCTRL(*this, "PadText", wxStaticText)->SetLabel("Hex Keypad Definition, Set 1");
				XRCCTRL(*this, "GamePlayer", wxStaticText)->SetLabel("Arcade Key Definition: Player 1"); 
				XRCCTRL(*this, "HexSwitchPad", wxButton)->Hide();
				player2defined_ = true;
			}
			else
			{
                p_Main->getDefaultHexKeys(computerType, computerTypeStr_, "B", hexKeyDefB1_, hexKeyDefB2_, dummy);
                
                keyDefGameHexA_[4] = 0;
                keyDefGameHexB_[0] = 2;
                keyDefGameHexB_[1] = 4;
                keyDefGameHexB_[2] = 6;
                keyDefGameHexB_[3] = 8;
                keyDefGameHexB_[4] = 0;

                hexPadBdefined_ = true;
				XRCCTRL(*this, "HexSwitchPlayer", wxButton)->Hide();
			}
			autoGame_ = p_Main->getConfigBool(computerTypeStr_+"/GameAuto", true);
			XRCCTRL(*this, "GameAuto", wxCheckBox)->SetValue(autoGame_);
		break;

        case VIPII:
            wxXmlResource::Get()->LoadDialog(this, parent, wxT("KeyMapDialog1"));
            XRCCTRL(*this, "InButton", wxButton)->Hide();
            XRCCTRL(*this, "InButtonText", wxStaticText)->Hide();
            
            XRCCTRL(*this, "PadText", wxStaticText)->SetLabel("Hex Keypad Definition, Set 1");
            XRCCTRL(*this, "GamePlayer", wxStaticText)->SetLabel("Arcade Key Definition: Player 1");
            XRCCTRL(*this, "HexSwitchPad", wxButton)->Hide();
            player2defined_ = true;
            
            p_Main->getDefaultHexKeys(computerType, computerTypeStr_, "A", hexKeyDefA1_, hexKeyDefA2_, dummy);
            autoGame_ = p_Main->getConfigBool(computerTypeStr_+"/GameAuto", true);
            XRCCTRL(*this, "GameAuto", wxCheckBox)->SetValue(autoGame_);
        break;
            
        case STUDIOIV:
            wxXmlResource::Get()->LoadDialog(this, parent, wxT("KeyMapDialog1"));
            XRCCTRL(*this, "InButton", wxButton)->Hide();
            XRCCTRL(*this, "InButtonText", wxStaticText)->Hide();
            XRCCTRL(*this, "TinyBASIC", wxButton)->Show();
            p_Main->getDefaultHexKeys(computerType, computerTypeStr_, "A", hexKeyDefA1_, hexKeyDefA2_, dummy);
            p_Main->getDefaultHexKeys(computerType, computerTypeStr_, "B", hexKeyDefB1_, hexKeyDefB2_, dummy);
            
            hexPadBdefined_ = true;
            XRCCTRL(*this, "HexSwitchPlayer", wxButton)->Hide();

            autoGame_ = p_Main->getConfigBool(computerTypeStr_+"/GameAuto", false);
            XRCCTRL(*this, "GameAuto", wxCheckBox)->SetValue(autoGame_);
        break;
            
        case VELF:
            wxXmlResource::Get()->LoadDialog(this, parent, wxT("KeyMapDialog1"));
            
            XRCCTRL(*this, "PadText", wxStaticText)->SetLabel("Hex Keypad Definition, Set 1");
            XRCCTRL(*this, "GamePlayer", wxStaticText)->SetLabel("Arcade Key Definition: Player 1");
            XRCCTRL(*this, "HexSwitchPad", wxButton)->Hide();
            player2defined_ = true;
            
            p_Main->getDefaultHexKeys(computerType, computerTypeStr_, "A", hexKeyDefA1_, hexKeyDefA2_, dummy);
            autoGame_ = p_Main->getConfigBool(computerTypeStr_+"/GameAuto", true);
            XRCCTRL(*this, "GameAuto", wxCheckBox)->SetValue(autoGame_);
            inButton1_ = p_Main->getDefaultInKey1(computerTypeStr_);
            inButton2_ = p_Main->getDefaultInKey2(computerTypeStr_);
        break;
	}

    if (computerType != COINARCADE)
    {
        if (computerType == STUDIO || computerType == VISICOM || computerType == VICTORY )
        {
            simDefA2_ = p_Main->getConfigBool(computerTypeStr_+"/DiagonalA2", true);
            simDefB2_ = p_Main->getConfigBool(computerTypeStr_+"/DiagonalB2", true);
        }
        else
        {
            simDefA2_ = p_Main->getConfigBool(computerTypeStr_+"/DiagonalA2", false);
            simDefB2_ = p_Main->getConfigBool(computerTypeStr_+"/DiagonalB2", false);
        }
        
        XRCCTRL(*this, "HexSave", wxButton)->SetFocus();
 
        wxString gameName;
        if (autoGame_)
        {
            switch (computerType)
            {
                case ELF:
                case ELFII:
                case SUPERELF:
                    if (p_Main->isComputerRunning())
                    {
                        gameName = p_Computer->getRunningGame();
                        if (gameName == "")
                            gameName = p_Main->getRomFile(computerType, MAINROM2);
                    }
                    p_Main->loadKeyDefinition(p_Main->getRomFile(computerType, MAINROM1), gameName, hexKeyDefA1_, hexKeyDefB1_, hexKeyDefA2_, &simDefA2_, hexKeyDefB2_, &simDefB2_, &inButton1_, &inButton2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
                break;
                    
                case ETI:
                    p_Main->loadKeyDefinition("", p_Main->getChip8SW(computerType), hexKeyDefA1_, hexKeyDefB1_, hexKeyDefA2_, &simDefA2_, hexKeyDefB2_, &simDefB2_, &inButton1_, &inButton2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
                break;
                    
                case VELF:
                case VIP:
                case VIPII:
                case VIP2K:
                   p_Main->loadKeyDefinition(p_Main->getRamFile(computerType), p_Main->getChip8SW(computerType), hexKeyDefA1_, hexKeyDefB1_, hexKeyDefA2_, &simDefA2_, hexKeyDefB2_, &simDefB2_, &inButton1_, &inButton2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
                break;
                    
                case STUDIOIV:
                    p_Main->loadKeyDefinition(p_Main->getRomFile(computerType, MAINROM1), p_Main->getRomFile(computerType, MAINROM1), hexKeyDefA1_, hexKeyDefB1_, hexKeyDefA2_, &simDefA2_, hexKeyDefB2_, &simDefB2_, &inButton1_, &inButton2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
                break;
                    
                case FRED1:
                case FRED1_5:
                case UC1800:
                    p_Main->loadKeyDefinition(p_Main->getRamFile(computerType), p_Main->getRamFile(computerType), hexKeyDefA1_, hexKeyDefB1_, hexKeyDefA2_, &simDefA2_, hexKeyDefB2_, &simDefB2_, &inButton1_, &inButton2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
                break;
                    
                case NANO:
                case TMC2000:
                case TMC1800:
                    p_Main->loadKeyDefinition(p_Main->getRamFile(computerType), p_Main->getChip8SW(computerType), hexKeyDefA1_, hexKeyDefB1_, hexKeyDefA2_, &simDefA2_, hexKeyDefB2_, &simDefB2_, &inButton1_, &inButton2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
                break;
                    
                case VICTORY:
                    gameName = p_Main->getRomFile(computerType, CARTROM);
                    if (p_Main->isComputerRunning())
                    {
                        if (p_Computer->getBuildInGame() != 0)
                        {
                            gameName.Printf("victorygrandpack%01d", p_Computer->getBuildInGame());
                        }
                    }
                    p_Main->loadKeyDefinition("", gameName, hexKeyDefA1_, hexKeyDefB1_, hexKeyDefA2_, &simDefA2_, hexKeyDefB2_, &simDefB2_, &inButton1_, &inButton2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition_studio.txt");
                break;
                case VISICOM:
                    gameName = p_Main->getRomFile(computerType, CARTROM);
                    if (p_Main->isComputerRunning())
                    {
                        if (p_Computer->getBuildInGame() != 0)
                        {
                            gameName.Printf("visicombuildin%01d", p_Computer->getBuildInGame());
                        }
                    }
                    p_Main->loadKeyDefinition("", gameName, hexKeyDefA1_, hexKeyDefB1_, hexKeyDefA2_, &simDefA2_, hexKeyDefB2_, &simDefB2_, &inButton1_, &inButton2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
                break;
                case STUDIO:
                    gameName = p_Main->getRomFile(computerType, CARTROM);
                    if (p_Main->isComputerRunning())
                    {
                        if (p_Computer->getBuildInGame() != 0)
                        {
                            gameName.Printf("studio2buildin%01d", p_Computer->getBuildInGame());
                        }
                    }
                    p_Main->loadKeyDefinition("", gameName, hexKeyDefA1_, hexKeyDefB1_, hexKeyDefA2_, &simDefA2_, hexKeyDefB2_, &simDefB2_, &inButton1_, &inButton2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition_studio.txt");
                break;
            }
        }
        enableAuto(autoGame_);
        
        this->connectKeyDownEvent(this); 
        updateButtons();
    }
    else
    {
        XRCCTRL(*this, "CoinSave", wxButton)->SetFocus();
        this->connectKeyDownEvent(this);
        updateButtonsCoinArcade();
    }
    
#ifdef __WXMAC__
    focusTimer = new wxTimer(this, wxID_ANY);
    focusTimer->Start(1000, wxTIMER_CONTINUOUS);
#endif
}

KeyMapDialog::~KeyMapDialog()
{
#ifdef __WXMAC__
    focusTimer->Stop();
    delete focusTimer;
#endif
}

void KeyMapDialog::compareButtons(int toBeCheckedHex, int toBeCheckedValue, int type)
{
	wxString button;
	if (type == KEYPAD_HEX_A1 || type == KEYPAD_HEX_B1 || type == KEYPAD_HEX_A2 || type == KEYPAD_HEX_B2)
		button.Printf("HexKey%01X", toBeCheckedHex);
	else
	{
		if (type == IN_BUTTON_1 || type == IN_BUTTON_2)
			button = "InButton";
		else
			button.Printf("GameKey%01X", toBeCheckedHex);
	}

    if (type != IN_BUTTON_1)
    {
        if (toBeCheckedValue == inButton1_)
        {
#ifdef __WXMAC__
            XRCCTRL(*this, button, wxButton)->SetBackgroundColour(wxColour(255,0,0));
#else
            XRCCTRL(*this, button, wxButton)->SetForegroundColour(wxColour(255,0,0));
#endif
        }
    }

    if (type != IN_BUTTON_2)
    {
        if (toBeCheckedValue == inButton2_)
        {
#ifdef __WXMAC__
            XRCCTRL(*this, button, wxButton)->SetBackgroundColour(wxColour(255,0,0));
#else
            XRCCTRL(*this, button, wxButton)->SetForegroundColour(wxColour(255,0,0));
#endif
        }
    }

    for (int j=0; j<numberOfKeys_; j++)
    {
        if ((toBeCheckedHex != j && type == KEYPAD_HEX_A1) || type != KEYPAD_HEX_A1)
        {
            if (toBeCheckedValue == hexKeyDefA1_[j])
            {
#ifdef __WXMAC__
                XRCCTRL(*this, button, wxButton)->SetBackgroundColour(wxColour(255,0,0));
#else
                XRCCTRL(*this, button, wxButton)->SetForegroundColour(wxColour(255,0,0));
#endif
            }
        }
    }
    for (int j=0; j<numberOfKeys_; j++)
    {
        if ((toBeCheckedHex != j && type == KEYPAD_HEX_A2) || type != KEYPAD_HEX_A2)
        {
            if (toBeCheckedValue == hexKeyDefA2_[j])
            {
#ifdef __WXMAC__
                XRCCTRL(*this, button, wxButton)->SetBackgroundColour(wxColour(255,0,0));
#else
                XRCCTRL(*this, button, wxButton)->SetForegroundColour(wxColour(255,0,0));
#endif
            }
        }
    }

	if (!hexPadBdefined_ && !player2defined_)
		return;
	for (int j=0; j<numberOfKeys_; j++)
	{
		if ((toBeCheckedHex != j && type == KEYPAD_HEX_B1) || type != KEYPAD_HEX_B1)
		{
			if (toBeCheckedValue == hexKeyDefB1_[j])			
			{		
#ifdef __WXMAC__
                XRCCTRL(*this, button, wxButton)->SetBackgroundColour(wxColour(255,0,0));
#else
				XRCCTRL(*this, button, wxButton)->SetForegroundColour(wxColour(255,0,0));
#endif
			}
		}				
	}
    for (int j=0; j<numberOfKeys_; j++)
    {
        if ((toBeCheckedHex != j && type == KEYPAD_HEX_B2) || type != KEYPAD_HEX_B2)
        {
            if (toBeCheckedValue == hexKeyDefB2_[j])
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

void KeyMapDialog::updateButtons()
{
	wxString button, line1, line2, keyStr1, keyStr2, keyStrNum;
    if (hexPadSet1_)
    {
        if (inButton1_ != 0 && inButton1_ != 255)
        {
            setLabel("InButton", 20, inButton1_);
            compareButtons(-1, inButton1_, IN_BUTTON_1);
        }
        else
            if (inButton1_ != 255)
            {
                XRCCTRL(*this, "InButton", wxButton)->SetLabel("");
#ifdef __WXMAC__
                XRCCTRL(*this, "InButton", wxButton)->SetBackgroundColour(wxNullColour);
#else
                XRCCTRL(*this, "InButton", wxButton)->SetForegroundColour(wxNullColour);
#endif
            }
	}
    else
    {
        if (inButton2_ != 0 && inButton2_ != 255)
        {
            setLabel("InButton", 20, inButton2_);
            compareButtons(-1, inButton2_, IN_BUTTON_2);
        }
        else
        {
            if (inButton2_ != 255)
            {
                XRCCTRL(*this, "InButton", wxButton)->SetLabel("");
#ifdef __WXMAC__
                XRCCTRL(*this, "InButton", wxButton)->SetBackgroundColour(wxNullColour);
#else
                XRCCTRL(*this, "InButton", wxButton)->SetForegroundColour(wxNullColour);
#endif
            }
        }
    }

	if (hexPadA_)
	{
		for (int i= 0; i < numberOfKeys_; i++)
		{
            if (hexPadSet1_)
            {
                if (hexKeyDefA1_[i] != 0)
                {
                    setLabel("HexKey%01X", i, hexKeyDefA1_[i]);
                    compareButtons(i, hexKeyDefA1_[i], KEYPAD_HEX_A1);
                }
                else
                {
                    button.Printf("HexKey%01X", i);
                    XRCCTRL(*this, button, wxButton)->SetLabel("");
#ifdef __WXMAC__
                    XRCCTRL(*this, button, wxButton)->SetBackgroundColour(wxNullColour);
#else
                    XRCCTRL(*this, button, wxButton)->SetForegroundColour(wxNullColour);
#endif
                }
            }
            else
            {
                if (hexKeyDefA2_[i] != 0)
                {
                    setLabel("HexKey%01X", i, hexKeyDefA2_[i]);
                    compareButtons(i, hexKeyDefA2_[i], KEYPAD_HEX_A2);
                }
                else
                {
                    button.Printf("HexKey%01X", i);
                    XRCCTRL(*this, button, wxButton)->SetLabel("");
#ifdef __WXMAC__
                    XRCCTRL(*this, button, wxButton)->SetBackgroundColour(wxNullColour);
#else
                    XRCCTRL(*this, button, wxButton)->SetForegroundColour(wxNullColour);
#endif
                }
				if (simDefA2_)
				{
					XRCCTRL(*this, "HexKey1", wxButton)->SetLabel(getKeyStr(hexKeyDefA2_[4]) + "+" + getKeyStr(hexKeyDefA2_[2]));
					XRCCTRL(*this, "HexKey3", wxButton)->SetLabel(getKeyStr(hexKeyDefA2_[2]) + "+" + getKeyStr(hexKeyDefA2_[6]));
					XRCCTRL(*this, "HexKey7", wxButton)->SetLabel(getKeyStr(hexKeyDefA2_[4]) + "+" + getKeyStr(hexKeyDefA2_[8]));
					XRCCTRL(*this, "HexKey9", wxButton)->SetLabel(getKeyStr(hexKeyDefA2_[8]) + "+" + getKeyStr(hexKeyDefA2_[6]));
				}
			}
		}
	}
	else
	{
/*		if (player2defined_)
		{
			for (int i= 0; i < numberOfKeys_; i++)
			{
                if (hexPadSet1_)
                {
                    setLabel("HexKey%01X", i, hexKeyDefA1_[i]);
                    compareButtons(i, hexKeyDefA1_[i], KEYPAD_HEX_A);
                }
                else
                {
                    setLabel("HexKey%01X", i, hexKeyDefA2_[i]);
                    compareButtons(i, hexKeyDefA2_[i], KEYPAD_HEX_A);
                }
			}
		}
		else
		{*/
			for (int i= 0; i < numberOfKeys_; i++)
			{
                if (hexPadSet1_)
                {
                    if (hexKeyDefB1_[i] != 0)
                    {
                        setLabel("HexKey%01X", i, hexKeyDefB1_[i]);
                        compareButtons(i, hexKeyDefB1_[i], KEYPAD_HEX_B1);
                    }
                    else
                    {
                        button.Printf("HexKey%01X", i);
                        XRCCTRL(*this, button, wxButton)->SetLabel("");
#ifdef __WXMAC__
                        XRCCTRL(*this, button, wxButton)->SetBackgroundColour(wxNullColour);
#else
                        XRCCTRL(*this, button, wxButton)->SetForegroundColour(wxNullColour);
#endif
                   }
				}
                else
                {
                    if (hexKeyDefB2_[i] != 0)
                    {
                        setLabel("HexKey%01X", i, hexKeyDefB2_[i]);
                        compareButtons(i, hexKeyDefB2_[i], KEYPAD_HEX_B2);
                    }
                    else
                    {
                        button.Printf("HexKey%01X", i);
                        XRCCTRL(*this, button, wxButton)->SetLabel("");
#ifdef __WXMAC__
                        XRCCTRL(*this, button, wxButton)->SetBackgroundColour(wxNullColour);
#else
                        XRCCTRL(*this, button, wxButton)->SetForegroundColour(wxNullColour);
#endif
                    }
					if (simDefB2_)
					{
						XRCCTRL(*this, "HexKey1", wxButton)->SetLabel(getKeyStr(hexKeyDefB2_[4]) + "+" + getKeyStr(hexKeyDefB2_[2]));
						XRCCTRL(*this, "HexKey3", wxButton)->SetLabel(getKeyStr(hexKeyDefB2_[2]) + "+" + getKeyStr(hexKeyDefB2_[6]));
						XRCCTRL(*this, "HexKey7", wxButton)->SetLabel(getKeyStr(hexKeyDefB2_[4]) + "+" + getKeyStr(hexKeyDefB2_[8]));
						XRCCTRL(*this, "HexKey9", wxButton)->SetLabel(getKeyStr(hexKeyDefB2_[8]) + "+" + getKeyStr(hexKeyDefB2_[6]));
					}
				}
			}
//		}
	}
    if (gamePlayer1_)
    {
        for (int i=0; i<5; i++)
        {
            keyStrNum.Printf("(%i)", hexKeyDefA2_[keyDefGameHexA_[i]]);
            line1.Printf("GameHexText%01X", i);
            line2.Printf("GameText%01X", i);

            if (keyDefGameHexA_[i] < 0x10)
            {
                keyStr1.Printf("Key %01X:", keyDefGameHexA_[i]);
                keyStr2 = getKeyStr(hexKeyDefA2_[keyDefGameHexA_[i]])+keyStrNum;
            }
            if (keyDefGameHexA_[i] == 0x10)
            {
                keyStr1 = "";
                keyStr2 = "Not used";
            }
            if (keyDefGameHexA_[i] == 0x11)
            {
                keyStr1 = "Key IN:";
                keyStr2 = getKeyStr(inButton2_)+keyStrNum;
            }
            XRCCTRL(*this, line1, wxStaticText)->SetLabel(keyStr1);
            XRCCTRL(*this, line2, wxStaticText)->SetLabel(keyStr2);
       }
	}
    else
    {
        for (int i=0; i<5; i++)
        {
            if (player2defined_)
                keyStrNum.Printf("(%i)", hexKeyDefA2_[keyDefGameHexB_[i]]);
            else
                keyStrNum.Printf("(%i)", hexKeyDefB2_[keyDefGameHexB_[i]]);
            line1.Printf("GameHexText%01X", i);
            line2.Printf("GameText%01X", i);
            
            if (keyDefGameHexB_[i] < 0x10)
            {
                keyStr1.Printf("Key %01X:", keyDefGameHexB_[i]);
                if (player2defined_)
                    keyStr2 = getKeyStr(hexKeyDefA2_[keyDefGameHexB_[i]])+keyStrNum;
                else
                    keyStr2 = getKeyStr(hexKeyDefB2_[keyDefGameHexB_[i]])+keyStrNum;
            }
            if (keyDefGameHexB_[i] == 0x10)
            {
                keyStr1 = "";
                keyStr2 = "Not used";
            }
            if (keyDefGameHexB_[i] == 0x11)
            {
                keyStr1 = "Key IN:";
                keyStr2 = getKeyStr(inButton2_)+keyStrNum;
            }
            XRCCTRL(*this, line1, wxStaticText)->SetLabel(keyStr1);
            XRCCTRL(*this, line2, wxStaticText)->SetLabel(keyStr2);
        }
	}
}

void KeyMapDialog::onSaveButton( wxCommandEvent& WXUNUSED(event) )
{
	wxString conf;
    if (inButton1_ != 0 && inButton1_ != 255)
    {
        p_Main->setConfigItem(computerTypeStr_+"/InButton1", inButton1_);
    }
    if (inButton2_ != 0 && inButton2_ != 255)
    {
        p_Main->setConfigItem(computerTypeStr_+"/InButton2", inButton2_);
    }

	for (int i= 0; i < numberOfKeys_; i++)
	{	
        conf.Printf("/HexKeySet1A%01X", i);
        p_Main->setConfigItem(computerTypeStr_+conf, hexKeyDefA1_[i]);
        conf.Printf("/HexKeySet2A%01X", i);
        p_Main->setConfigItem(computerTypeStr_+conf, hexKeyDefA2_[i]);
	}

	if ((computerTypeStr_ == "Vip") || (computerTypeStr_ == "Studio2") || (computerTypeStr_ == "Visicom") || (computerTypeStr_ == "Victory") || (computerTypeStr_ == "StudioIV"))
	{
		for (int i= 0; i < numberOfKeys_; i++)
		{	
            conf.Printf("/HexKeySet1B%01X", i);
            p_Main->setConfigItem(computerTypeStr_+conf, hexKeyDefB1_[i]);
            conf.Printf("/HexKeySet2B%01X", i);
            p_Main->setConfigItem(computerTypeStr_+conf, hexKeyDefB2_[i]);
		}
        
        if (computerTypeStr_ == "Vip")
        {
            p_Main->setConfigItem(computerTypeStr_+"/HexKeySet2Up", hexKeyDefB2_[16]);
            p_Main->setConfigItem(computerTypeStr_+"/HexKeySet2Up", hexKeyDefB2_[17]);
        }
    }

    p_Main->setConfigBool(computerTypeStr_+"/GameAuto", autoGame_);
	if (p_Computer != NULL)
	{
		p_Computer->reDefineKeysA(hexKeyDefA1_, hexKeyDefA2_);
		if (hexPadBdefined_ || player2defined_ )
        {
            p_Computer->reDefineKeysB(hexKeyDefB1_, hexKeyDefB2_);
        }
        p_Computer->reDefineInKey(inButton1_, inButton2_);
	}
    if (computerTypeStr_ == "Studio2" || computerTypeStr_ == "Visicom" || computerTypeStr_ == "Victory")
    {
        p_Main->setConfigBool(computerTypeStr_+"/DiagonalA2", simDefA2_);
        p_Main->setConfigBool(computerTypeStr_+"/DiagonalB2", simDefB2_);
    }

	EndModal( wxID_OK );
}

void KeyMapDialog::onInButton(wxCommandEvent& WXUNUSED(event) )
{
	wxString buttonName, buttonNumber;

	if (hexKey_ != -1)
	{
		if (hexPadA_ || player2defined_)
			setLabel("HexKey%01X", hexKey_, hexKeyDefA1_[hexKey_]);
		else
			setLabel("HexKey%01X", hexKey_, hexKeyDefB1_[hexKey_]);
		hexKey_ = -1;
	}

	XRCCTRL(*this, "InButton", wxButton)->SetLabel("Press Key");
	inKey_ = 0;
}

void KeyMapDialog::onHexKey(wxCommandEvent &event)
{
	wxString buttonName, buttonNumber;

	if (hexKey_ != -1)
	{
		if (hexPadA_)
			if (hexPadSet1_)
				setLabel("HexKey%01X", hexKey_, hexKeyDefA1_[hexKey_]);
			else
				setLabel("HexKey%01X", hexKey_, hexKeyDefA2_[hexKey_]);
		else
			if (hexPadSet1_)
				setLabel("HexKey%01X", hexKey_, hexKeyDefB1_[hexKey_]);
			else
				setLabel("HexKey%01X", hexKey_, hexKeyDefB2_[hexKey_]);
	}
	if (inKey_ != -1)
	{
        if (hexPadSet1_)
            setLabel("InButton", 20, inButton1_);
        else
            setLabel("InButton", 20, inButton2_);
		inKey_ = -1;
	}

    buttonName = wxWindow::FindWindowById(event.GetId())->GetName();
	buttonNumber = buttonName.Last();
	if (buttonNumber.ToLong(&hexKey_, 16))
		XRCCTRL(*this, buttonName, wxButton)->SetLabel("Press Key");
	else
		hexKey_ = -1;
}

void KeyMapDialog::onGameHex(wxCommandEvent &event)
{
	wxString buttonName, buttonNumber;
	long gameHex;
	
	buttonName = wxWindow::FindWindowById(event.GetId())->GetName();
	buttonNumber = buttonName.Last();
	if (buttonNumber.ToLong(&gameHex, 16))
	{
		if (hexPadA_)
			keyDefGameHexA_[gameHex] = event.GetSelection();
		else
			keyDefGameHexB_[gameHex] = event.GetSelection();
	}
}

void KeyMapDialog::onHexLocation(wxCommandEvent& WXUNUSED(event))
{
    bool keysFound;

    if (computerTypeStr_ == "Vip" && (p_Main->getVipVp590() || p_Main->getVipVp580()))
        keysFound = p_Main->loadKeyDefinition("", "viponlocation", hexKeyDefA1_, hexKeyDefB1_, hexKeyDefA2_, &simDefA2_, hexKeyDefB2_, &simDefB2_, &inButton1_, &inButton2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
    else
    {
        if (computerTypeStr_ == "Elf" || computerTypeStr_ == "ElfII" || computerTypeStr_ == "SuperElf" || computerTypeStr_ == "Elf2K"|| computerTypeStr_ == "Cosmicos")
            keysFound = p_Main->loadKeyDefinition("", "elfonlocation", hexKeyDefA1_, hexKeyDefB1_, hexKeyDefA2_, &simDefA2_, hexKeyDefB2_, &simDefB2_, &inButton1_, &inButton2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
        else
        {
            if (computerTypeStr_ == "FRED1" || computerTypeStr_ == "FRED1_5")
                keysFound = p_Main->loadKeyDefinition("", "fredonlocation", hexKeyDefA1_, hexKeyDefB1_, hexKeyDefA2_, &simDefA2_, hexKeyDefB2_, &simDefB2_, &inButton1_, &inButton2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
            else
			{
				if (computerTypeStr_ == "UC1800")
					keysFound = p_Main->loadKeyDefinition("", "uc1800onlocation", hexKeyDefA1_, hexKeyDefB1_, hexKeyDefA2_, &simDefA2_, hexKeyDefB2_, &simDefB2_, &inButton1_, &inButton2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
				else
                {
                    if (computerTypeStr_ == "StudioIV")
                        keysFound = p_Main->loadKeyDefinition("", "studioivonlocation", hexKeyDefA1_, hexKeyDefB1_, hexKeyDefA2_, &simDefA2_, hexKeyDefB2_, &simDefB2_, &inButton1_, &inButton2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
                    else
                        keysFound = p_Main->loadKeyDefinition("", "vipiionlocation", hexKeyDefA1_, hexKeyDefB1_, hexKeyDefA2_, &simDefA2_, hexKeyDefB2_, &simDefB2_, &inButton1_, &inButton2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
                }
			}
        }
    }

    if (!keysFound)
    {
        if (computerTypeStr_ == "Elf" || computerTypeStr_ == "ElfII" || computerTypeStr_ == "SuperElf" || computerTypeStr_ == "Elf2K"|| computerTypeStr_ == "Cosmicos")
        {
            hexKeyDefA1_[0] = 90;
            hexKeyDefA1_[1] = 88;
            hexKeyDefA1_[2] = 67;
            hexKeyDefA1_[3] = 86;
            hexKeyDefA1_[4] = 65;
            hexKeyDefA1_[5] = 83;
            hexKeyDefA1_[6] = 68;
            hexKeyDefA1_[7] = 70;
            hexKeyDefA1_[8] = 81;
            hexKeyDefA1_[9] = 87;
            hexKeyDefA1_[10] = 69;
            hexKeyDefA1_[11] = 82;
            hexKeyDefA1_[12] = 49;
            hexKeyDefA1_[13] = 50;
            hexKeyDefA1_[14] = 51;
            hexKeyDefA1_[15] = 52;
            
            if (p_Main->useNumPad())
            {
                hexKeyDefA2_[0] = 0;
                hexKeyDefA2_[1] = WXK_NUMPAD_ENTER;
                hexKeyDefA2_[2] = 0;
                hexKeyDefA2_[3] = 0;
                hexKeyDefA2_[4] = WXK_NUMPAD4;
                hexKeyDefA2_[5] = 317;
                hexKeyDefA2_[6] = WXK_NUMPAD6;
                hexKeyDefA2_[7] = 0;
                hexKeyDefA2_[8] = 314;
                hexKeyDefA2_[9] = 32;
                hexKeyDefA2_[10] = 316;
                hexKeyDefA2_[11] = WXK_NUMPAD2;
                hexKeyDefA2_[12] = 0;
                hexKeyDefA2_[13] = 315;
                hexKeyDefA2_[14] = 0;
                hexKeyDefA2_[15] = WXK_NUMPAD8;
            }
            else
            {
                hexKeyDefA2_[0] = 0;
                hexKeyDefA2_[1] = WXK_TAB;
                hexKeyDefA2_[2] = 0;
                hexKeyDefA2_[3] = 0;
                hexKeyDefA2_[4] = 71;
                hexKeyDefA2_[5] = 317;
                hexKeyDefA2_[6] = 72;
                hexKeyDefA2_[7] = 0;
                hexKeyDefA2_[8] = 314;
                hexKeyDefA2_[9] = 32;
                hexKeyDefA2_[10] = 316;
                hexKeyDefA2_[11] = 78;
                hexKeyDefA2_[12] = 0;
                hexKeyDefA2_[13] = 315;
                hexKeyDefA2_[14] = 0;
                hexKeyDefA2_[15] = 89;
            }
        }
        else
        {
            hexKeyDefA1_[0] = 88;
            hexKeyDefA1_[1] = 49;
            hexKeyDefA1_[2] = 50;
            hexKeyDefA1_[3] = 51;
            hexKeyDefA1_[4] = 81;
            hexKeyDefA1_[5] = 87;
            hexKeyDefA1_[6] = 69;
            hexKeyDefA1_[7] = 65;
            hexKeyDefA1_[8] = 83;
            hexKeyDefA1_[9] = 68;
            hexKeyDefA1_[10] = 90;
            hexKeyDefA1_[11] = 67;
            hexKeyDefA1_[12] = 52;
            hexKeyDefA1_[13] = 82;
            hexKeyDefA1_[14] = 70;
            hexKeyDefA1_[15] = 86;
            
            if (p_Main->useNumPad())
            {
                hexKeyDefA2_[0] = WXK_NUMPAD_ENTER;
                hexKeyDefA2_[1] = 0;
                hexKeyDefA2_[2] = 315;
                hexKeyDefA2_[3] = 0;
                hexKeyDefA2_[4] = 314;
                hexKeyDefA2_[5] = 32;
                hexKeyDefA2_[6] = 316;
                hexKeyDefA2_[7] = WXK_NUMPAD4;
                hexKeyDefA2_[8] = 317;
                hexKeyDefA2_[9] = WXK_NUMPAD6;
                hexKeyDefA2_[10] = 0;
                hexKeyDefA2_[11] = 0;
                hexKeyDefA2_[12] = WXK_NUMPAD8;
                hexKeyDefA2_[13] = WXK_NUMPAD2;
                hexKeyDefA2_[14] = 0;
                hexKeyDefA2_[15] = 0;
            }
            else
            {
                hexKeyDefA2_[0] = WXK_TAB;
                hexKeyDefA2_[1] = 0;
                hexKeyDefA2_[2] = 315;
                hexKeyDefA2_[3] = 0;
                hexKeyDefA2_[4] = 314;
                hexKeyDefA2_[5] = 32;
                hexKeyDefA2_[6] = 316;
                hexKeyDefA2_[7] = 71;
                hexKeyDefA2_[8] = 317;
                hexKeyDefA2_[9] = 72;
                hexKeyDefA2_[10] = 0;
                hexKeyDefA2_[11] = 0;
                hexKeyDefA2_[12] = 89;
                hexKeyDefA2_[13] = 78;
                hexKeyDefA2_[14] = 0;
                hexKeyDefA2_[15] = 0;
            }
        }
        
        if ((computerTypeStr_ == "Vip" && (p_Main->getVipVp590() || p_Main->getVipVp580())) || computerTypeStr_ == "Studio IV")
        {
            hexKeyDefA2_[0] = 32;
            hexKeyDefA2_[5] = 0;
            hexKeyDefA2_[7] = 0;
            hexKeyDefA2_[9] = 0;
            hexKeyDefA2_[12] = 0;
            hexKeyDefA2_[13] = 0;
            
            hexKeyDefB1_[0] = 44;
            hexKeyDefB1_[1] = 55;
            hexKeyDefB1_[2] = 56;
            hexKeyDefB1_[3] = 57;
            hexKeyDefB1_[4] = 85;
            hexKeyDefB1_[5] = 73;
            hexKeyDefB1_[6] = 79;
            hexKeyDefB1_[7] = 74;
            hexKeyDefB1_[8] = 75;
            hexKeyDefB1_[9] = 76;
            hexKeyDefB1_[10] = 77;
            hexKeyDefB1_[11] = 46;
            hexKeyDefB1_[12] = 48;
            hexKeyDefB1_[13] = 80;
            hexKeyDefB1_[14] = 59;
            hexKeyDefB1_[15] = 47;
            
            if (p_Main->useNumPad())
            {
                hexKeyDefB2_[0] = WXK_NUMPAD_ENTER;
                hexKeyDefB2_[1] = 0;
                hexKeyDefB2_[2] = WXK_NUMPAD8;
                hexKeyDefB2_[3] = 0;
                hexKeyDefB2_[4] = WXK_NUMPAD4;
                hexKeyDefB2_[5] = 0;
                hexKeyDefB2_[6] = WXK_NUMPAD6;
                hexKeyDefB2_[7] = 0;
                hexKeyDefB2_[8] = WXK_NUMPAD2;
                hexKeyDefB2_[9] = 0;
                hexKeyDefB2_[10] = 0;
                hexKeyDefB2_[11] = 0;
                hexKeyDefB2_[12] = 0;
                hexKeyDefB2_[13] = 0;
                hexKeyDefB2_[14] = 0;
                hexKeyDefB2_[15] = 0;
            }
            else
            {
                hexKeyDefB2_[0] = WXK_TAB;
                hexKeyDefB2_[1] = 0;
                hexKeyDefB2_[2] = 89;
                hexKeyDefB2_[3] = 0;
                hexKeyDefB2_[4] = 71;
                hexKeyDefB2_[5] = 0;
                hexKeyDefB2_[6] = 72;
                hexKeyDefB2_[7] = 0;
                hexKeyDefB2_[8] = 78;
                hexKeyDefB2_[9] = 0;
                hexKeyDefB2_[10] = 0;
                hexKeyDefB2_[11] = 0;
                hexKeyDefB2_[12] = 0;
                hexKeyDefB2_[13] = 0;
                hexKeyDefB2_[14] = 0;
                hexKeyDefB2_[15] = 0;
            }
        }
    }

	updateButtons();
	inKey_ = -1;
	hexKey_ = -1;
}

void KeyMapDialog::onTinyBASIC(wxCommandEvent& WXUNUSED(event))
{
    p_Main->loadKeyDefinition("", "studioivdefault", hexKeyDefA1_, hexKeyDefB1_, hexKeyDefA2_, &simDefA2_, hexKeyDefB2_, &simDefB2_, &inButton1_, &inButton2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
}

void KeyMapDialog::onHexChar(wxCommandEvent& WXUNUSED(event))
{
    bool keysFound;

    if (computerTypeStr_ == "Vip" && (p_Main->getVipVp590() || p_Main->getVipVp580()))
        keysFound = p_Main->loadKeyDefinition("", "viponcharacter", hexKeyDefA1_, hexKeyDefB1_, hexKeyDefA2_, &simDefA2_, hexKeyDefB2_, &simDefB2_, &inButton1_, &inButton2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
    else
    {
        if (computerTypeStr_ == "Elf" || computerTypeStr_ == "ElfII" || computerTypeStr_ == "SuperElf" || computerTypeStr_ == "Elf2K"|| computerTypeStr_ == "Cosmicos")
            keysFound = p_Main->loadKeyDefinition("", "elfoncharacter", hexKeyDefA1_, hexKeyDefB1_, hexKeyDefA2_, &simDefA2_, hexKeyDefB2_, &simDefB2_, &inButton1_, &inButton2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
        else
            if (computerTypeStr_ == "FRED1" || computerTypeStr_ == "FRED1_5")
                keysFound = p_Main->loadKeyDefinition("", "fredoncharacter", hexKeyDefA1_, hexKeyDefB1_, hexKeyDefA2_, &simDefA2_, hexKeyDefB2_, &simDefB2_, &inButton1_, &inButton2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
            else
			{
				if (computerTypeStr_ == "UC1800")
					keysFound = p_Main->loadKeyDefinition("", "uc1800oncharacter", hexKeyDefA1_, hexKeyDefB1_, hexKeyDefA2_, &simDefA2_, hexKeyDefB2_, &simDefB2_, &inButton1_, &inButton2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
				else
                {
                    if (computerTypeStr_ == "StudioIV")
                        keysFound = p_Main->loadKeyDefinition("", "studioivoncharacter", hexKeyDefA1_, hexKeyDefB1_, hexKeyDefA2_, &simDefA2_, hexKeyDefB2_, &simDefB2_, &inButton1_, &inButton2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
                    else
                        keysFound = p_Main->loadKeyDefinition("", "vipiioncharacter", hexKeyDefA1_, hexKeyDefB1_, hexKeyDefA2_, &simDefA2_, hexKeyDefB2_, &simDefB2_, &inButton1_, &inButton2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
                }
			}
    }

    if (!keysFound)
    {
        hexKeyDefA1_[0] = 48;
        hexKeyDefA1_[1] = 49;
        hexKeyDefA1_[2] = 50;
        hexKeyDefA1_[3] = 51;
        hexKeyDefA1_[4] = 52;
        hexKeyDefA1_[5] = 53;
        hexKeyDefA1_[6] = 54;
        hexKeyDefA1_[7] = 55;
        hexKeyDefA1_[8] = 56;
        hexKeyDefA1_[9] = 57;
        hexKeyDefA1_[10] = 65;
        hexKeyDefA1_[11] = 66;
        hexKeyDefA1_[12] = 67;
        hexKeyDefA1_[13] = 68;
        hexKeyDefA1_[14] = 69;
        hexKeyDefA1_[15] = 70;
        
        if (computerTypeStr_ == "Elf" || computerTypeStr_ == "ElfII" || computerTypeStr_ == "SuperElf" || computerTypeStr_ == "Elf2K"|| computerTypeStr_ == "Cosmicos")
        {
            if (p_Main->useNumPad())
            {
                hexKeyDefA2_[0] = 0;
                hexKeyDefA2_[1] = WXK_NUMPAD_ENTER;
                hexKeyDefA2_[2] = 0;
                hexKeyDefA2_[3] = 0;
                hexKeyDefA2_[4] = WXK_NUMPAD4;
                hexKeyDefA2_[5] = 317;
                hexKeyDefA2_[6] = WXK_NUMPAD6;
                hexKeyDefA2_[7] = 0;
                hexKeyDefA2_[8] = 314;
                hexKeyDefA2_[9] = 32;
                hexKeyDefA2_[10] = 316;
                hexKeyDefA2_[11] = WXK_NUMPAD2;
                hexKeyDefA2_[12] = 0;
                hexKeyDefA2_[13] = 315;
                hexKeyDefA2_[14] = 0;
                hexKeyDefA2_[15] = WXK_NUMPAD8;
            }
            else
            {
                hexKeyDefA2_[0] = 0;
                hexKeyDefA2_[1] = WXK_TAB;
                hexKeyDefA2_[2] = 0;
                hexKeyDefA2_[3] = 0;
                hexKeyDefA2_[4] = 71;
                hexKeyDefA2_[5] = 317;
                hexKeyDefA2_[6] = 72;
                hexKeyDefA2_[7] = 0;
                hexKeyDefA2_[8] = 314;
                hexKeyDefA2_[9] = 32;
                hexKeyDefA2_[10] = 316;
                hexKeyDefA2_[11] = 78;
                hexKeyDefA2_[12] = 0;
                hexKeyDefA2_[13] = 315;
                hexKeyDefA2_[14] = 0;
                hexKeyDefA2_[15] = 89;
            }
        }
        else
        {
            if (p_Main->useNumPad())
            {
                hexKeyDefA2_[0] = WXK_NUMPAD_ENTER;
                hexKeyDefA2_[1] = 0;
                hexKeyDefA2_[2] = 315;
                hexKeyDefA2_[3] = 0;
                hexKeyDefA2_[4] = 314;
                hexKeyDefA2_[5] = 32;
                hexKeyDefA2_[6] = 316;
                hexKeyDefA2_[7] = WXK_NUMPAD4;
                hexKeyDefA2_[8] = 317;
                hexKeyDefA2_[9] = WXK_NUMPAD6;
                hexKeyDefA2_[10] = 0;
                hexKeyDefA2_[11] = 0;
                hexKeyDefA2_[12] = WXK_NUMPAD8;
                hexKeyDefA2_[13] = WXK_NUMPAD2;
                hexKeyDefA2_[14] = 0;
                hexKeyDefA2_[15] = 0;
            }
            else
            {
                hexKeyDefA2_[0] = WXK_TAB;
                hexKeyDefA2_[1] = 0;
                hexKeyDefA2_[2] = 315;
                hexKeyDefA2_[3] = 0;
                hexKeyDefA2_[4] = 314;
                hexKeyDefA2_[5] = 32;
                hexKeyDefA2_[6] = 316;
                hexKeyDefA2_[7] = 71;
                hexKeyDefA2_[8] = 317;
                hexKeyDefA2_[9] = 72;
                hexKeyDefA2_[10] = 0;
                hexKeyDefA2_[11] = 0;
                hexKeyDefA2_[12] = 89;
                hexKeyDefA2_[13] = 78;
                hexKeyDefA2_[14] = 0;
                hexKeyDefA2_[15] = 0;
            }
        }
        
        if ((computerTypeStr_ == "Vip" && (p_Main->getVipVp590() || p_Main->getVipVp580())) || computerTypeStr_ == "Studio IV")
        {
            hexKeyDefA2_[0] = 32;
            hexKeyDefA2_[5] = 0;
            hexKeyDefA2_[7] = 0;
            hexKeyDefA2_[9] = 0;
            hexKeyDefA2_[12] = 0;
            hexKeyDefA2_[13] = 0;
            
            hexKeyDefB1_[0] = WXK_NUMPAD0;
            hexKeyDefB1_[1] = WXK_NUMPAD1;
            hexKeyDefB1_[2] = WXK_NUMPAD2;
            hexKeyDefB1_[3] = WXK_NUMPAD3;
            hexKeyDefB1_[4] = WXK_NUMPAD4;
            hexKeyDefB1_[5] = WXK_NUMPAD5;
            hexKeyDefB1_[6] = WXK_NUMPAD6;
            hexKeyDefB1_[7] = WXK_NUMPAD7;
            hexKeyDefB1_[8] = WXK_NUMPAD8;
            hexKeyDefB1_[9] = WXK_NUMPAD9;
            hexKeyDefB1_[10] = WXK_NUMPAD_DIVIDE;
            hexKeyDefB1_[11] = WXK_NUMPAD_MULTIPLY;
            hexKeyDefB1_[12] = WXK_NUMPAD_SUBTRACT;
            hexKeyDefB1_[13] = WXK_NUMPAD_ADD;
            hexKeyDefB1_[14] = WXK_NUMPAD_ENTER;
            hexKeyDefB1_[15] = WXK_NUMPAD_DECIMAL;
            
            hexKeyDefB2_[0] = WXK_TAB;
            hexKeyDefB2_[1] = 0;
            hexKeyDefB2_[2] = 89;
            hexKeyDefB2_[3] = 0;
            hexKeyDefB2_[4] = 71;
            hexKeyDefB2_[5] = 0;
            hexKeyDefB2_[6] = 72;
            hexKeyDefB2_[7] = 0;
            hexKeyDefB2_[8] = 78;
            hexKeyDefB2_[9] = 0;
            hexKeyDefB2_[10] = 0;
            hexKeyDefB2_[11] = 0;
            hexKeyDefB2_[12] = 0;
            hexKeyDefB2_[13] = 0;
            hexKeyDefB2_[14] = 0;
            hexKeyDefB2_[15] = 0;
        }
    }

	updateButtons();
	inKey_ = -1;
	hexKey_ = -1;
}

void KeyMapDialog::onStudioLocation(wxCommandEvent& WXUNUSED(event))
{
    bool keysFound;

    keysFound = p_Main->loadKeyDefinition("", "studioonlocation", hexKeyDefA1_, hexKeyDefB1_, hexKeyDefA2_, &simDefA2_, hexKeyDefB2_, &simDefB2_, &inButton1_, &inButton2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition_studio.txt");
 
    if (!keysFound)
    {
        simDefA2_ = true;
        simDefB2_ = true;

        hexKeyDefA1_[0] = 88;
        hexKeyDefA1_[1] = 49;
        hexKeyDefA1_[2] = 50;
        hexKeyDefA1_[3] = 51;
        hexKeyDefA1_[4] = 81;
        hexKeyDefA1_[5] = 87;
        hexKeyDefA1_[6] = 69;
        hexKeyDefA1_[7] = 65;
        hexKeyDefA1_[8] = 83;
        hexKeyDefA1_[9] = 68;
        
        hexKeyDefA2_[0] = 32;
        hexKeyDefA2_[1] = 0;
        hexKeyDefA2_[2] = 315;
        hexKeyDefA2_[3] = 0;
        hexKeyDefA2_[4] = 314;
        hexKeyDefA2_[5] = 0;
        hexKeyDefA2_[6] = 316;
        hexKeyDefA2_[7] = 0;
        hexKeyDefA2_[8] = 317;
        hexKeyDefA2_[9] = 0;
        
        if (p_Main->useNumPad())
        {
            hexKeyDefB1_[0] = WXK_NUMPAD0;
            hexKeyDefB1_[1] = WXK_NUMPAD7;
            hexKeyDefB1_[2] = WXK_NUMPAD8;
            hexKeyDefB1_[3] = WXK_NUMPAD9;
            hexKeyDefB1_[4] = WXK_NUMPAD4;
            hexKeyDefB1_[5] = WXK_NUMPAD5;
            hexKeyDefB1_[6] = WXK_NUMPAD6;
            hexKeyDefB1_[7] = WXK_NUMPAD1;
            hexKeyDefB1_[8] = WXK_NUMPAD2;
            hexKeyDefB1_[9] = WXK_NUMPAD3;
        }
        else
        {
            hexKeyDefB1_[0] = 44;
            hexKeyDefB1_[1] = 55;
            hexKeyDefB1_[2] = 56;
            hexKeyDefB1_[3] = 57;
            hexKeyDefB1_[4] = 85;
            hexKeyDefB1_[5] = 73;
            hexKeyDefB1_[6] = 79;
            hexKeyDefB1_[7] = 74;
            hexKeyDefB1_[8] = 75;
            hexKeyDefB1_[9] = 76;
        }
        
        hexKeyDefB2_[0] = WXK_TAB;
        hexKeyDefB2_[1] = 0;
        hexKeyDefB2_[2] = 89;
        hexKeyDefB2_[3] = 0;
        hexKeyDefB2_[4] = 71;
        hexKeyDefB2_[5] = 0;
        hexKeyDefB2_[6] = 72;
        hexKeyDefB2_[7] = 0;
        hexKeyDefB2_[8] = 78;
        hexKeyDefB2_[9] = 0;
    }

    updateButtons();
	inKey_ = -1;
	hexKey_ = -1;
}

void KeyMapDialog::onStudioChar(wxCommandEvent& WXUNUSED(event))
{
    bool keysFound;

    keysFound = p_Main->loadKeyDefinition("", "studiooncharacter", hexKeyDefA1_, hexKeyDefB1_, hexKeyDefA2_, &simDefA2_, hexKeyDefB2_, &simDefB2_, &inButton1_, &inButton2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition_studio.txt");
    
    if (!keysFound)
    {
        simDefA2_ = true;
        simDefB2_ = true;

        hexKeyDefA1_[0] = 48;
        hexKeyDefA1_[1] = 49;
        hexKeyDefA1_[2] = 50;
        hexKeyDefA1_[3] = 51;
        hexKeyDefA1_[4] = 52;
        hexKeyDefA1_[5] = 53;
        hexKeyDefA1_[6] = 54;
        hexKeyDefA1_[7] = 55;
        hexKeyDefA1_[8] = 56;
        hexKeyDefA1_[9] = 57;
        
        hexKeyDefA2_[0] = 32;
        hexKeyDefA2_[1] = 0;
        hexKeyDefA2_[2] = 315;
        hexKeyDefA2_[3] = 0;
        hexKeyDefA2_[4] = 314;
        hexKeyDefA2_[5] = 0;
        hexKeyDefA2_[6] = 316;
        hexKeyDefA2_[7] = 0;
        hexKeyDefA2_[8] = 317;
        hexKeyDefA2_[9] = 0;
        
        if (p_Main->useNumPad())
        {
            hexKeyDefB1_[0] = WXK_NUMPAD0;
            hexKeyDefB1_[1] = WXK_NUMPAD1;
            hexKeyDefB1_[2] = WXK_NUMPAD2;
            hexKeyDefB1_[3] = WXK_NUMPAD3;
            hexKeyDefB1_[4] = WXK_NUMPAD4;
            hexKeyDefB1_[5] = WXK_NUMPAD5;
            hexKeyDefB1_[6] = WXK_NUMPAD6;
            hexKeyDefB1_[7] = WXK_NUMPAD7;
            hexKeyDefB1_[8] = WXK_NUMPAD8;
            hexKeyDefB1_[9] = WXK_NUMPAD9;
            
        }
        else
        {
            hexKeyDefB1_[0] = 80;
            hexKeyDefB1_[1] = 81;
            hexKeyDefB1_[2] = 0;
            hexKeyDefB1_[3] = 69;
            hexKeyDefB1_[4] = 82;
            hexKeyDefB1_[5] = 84;
            hexKeyDefB1_[6] = 89;
            hexKeyDefB1_[7] = 85;
            hexKeyDefB1_[8] = 73;
            hexKeyDefB1_[9] = 79;
        }
        
        hexKeyDefB2_[0] = WXK_TAB;
        hexKeyDefB2_[1] = 0;
        hexKeyDefB2_[2] = 87;
        hexKeyDefB2_[3] = 0;
        hexKeyDefB2_[4] = 65;
        hexKeyDefB2_[5] = 0;
        hexKeyDefB2_[6] = 83;
        hexKeyDefB2_[7] = 0;
        hexKeyDefB2_[8] = 90;
        hexKeyDefB2_[9] = 0;
    }
    
    updateButtons();
    inKey_ = -1;
    hexKey_ = -1;
}

void KeyMapDialog::onSwitchPad(wxCommandEvent & WXUNUSED(event))
{
	hexPadA_ = !hexPadA_;
	if (hexPadBdefined_)
        gamePlayer1_ = !gamePlayer1_;

    if (hexPadA_)
        XRCCTRL(*this, "PadText", wxStaticText)->SetLabel("Hex Keypad Definition: A, Set 1");
    else
        XRCCTRL(*this, "PadText", wxStaticText)->SetLabel("Hex Keypad Definition: B, Set 1");
    
	updateButtons();
	inKey_ = -1;
	hexKey_ = -1;
}

void KeyMapDialog::onSwitchPlayer(wxCommandEvent & WXUNUSED(event))
{
	gamePlayer1_ = !gamePlayer1_;
	if (gamePlayer1_)
		XRCCTRL(*this, "GamePlayer", wxStaticText)->SetLabel("Arcade Key Definition: Player 1");
	else
		XRCCTRL(*this, "GamePlayer", wxStaticText)->SetLabel("Arcade Key Definition: Player 2");
	updateButtons();
	inKey_ = -1;
	hexKey_ = -1;
}

void KeyMapDialog::onSwitchStudio(wxCommandEvent & WXUNUSED(event))
{
    hexPadA_ = !hexPadA_;
    gamePlayer1_ = !gamePlayer1_;

	updatePadAndSet();
	updateButtons();
    inKey_ = -1;
    hexKey_ = -1;
}

void KeyMapDialog::onSwitchSet(wxCommandEvent & WXUNUSED(event))
{
    hexPadSet1_ = !hexPadSet1_;

	updatePadAndSet();
    updateButtons();
    inKey_ = -1;
    hexKey_ = -1;
}

void KeyMapDialog::updatePadAndSet()
{
	if (hexPadA_)
	{
		if (hexPadSet1_)
			XRCCTRL(*this, "PadText", wxStaticText)->SetLabel("Keypad Definition: A - Left, Set 1");
		else
		{
			XRCCTRL(*this, "PadText", wxStaticText)->SetLabel("Keypad Definition: A - Left, Set 2");
			XRCCTRL(*this, "StudioEnableDiagonal", wxCheckBox)->SetLabel("Diagonal Directions Pad A");
			XRCCTRL(*this, "StudioEnableDiagonal", wxCheckBox)->SetValue(simDefA2_);
		}
	}
	else
	{
		if (hexPadSet1_)
			XRCCTRL(*this, "PadText", wxStaticText)->SetLabel("Keypad Definition: B - Right, Set 1");
		else
		{
			XRCCTRL(*this, "PadText", wxStaticText)->SetLabel("Keypad Definition: B - Right, Set 2");
			XRCCTRL(*this, "StudioEnableDiagonal", wxCheckBox)->SetLabel("Diagonal Directions Pad B");
			XRCCTRL(*this, "StudioEnableDiagonal", wxCheckBox)->SetValue(simDefB2_);
		}
	}

	XRCCTRL(*this, "StudioEnableDiagonal", wxCheckBox)->Show(!hexPadSet1_);
}
void KeyMapDialog::onHexSwitchSet1(wxCommandEvent & WXUNUSED(event))
{
    hexPadSet1_ = !hexPadSet1_;
    if (hexPadSet1_)
        XRCCTRL(*this, "PadText", wxStaticText)->SetLabel("Hex Keypad Definition, Set 1");
    else
        XRCCTRL(*this, "PadText", wxStaticText)->SetLabel("Hex Keypad Definition, Set 2");
    
	if (computerTypeStr_ == "UC1800")
	{
		if (hexPadSet1_)
			XRCCTRL(*this, "InButtonText", wxStaticText)->SetLabel("Enter Button");
		else
			XRCCTRL(*this, "InButtonText", wxStaticText)->SetLabel("Start/EF1 Button");
	}

    updateButtons();
    inKey_ = -1;
    hexKey_ = -1;
}

void KeyMapDialog::onHexSwitchSet2(wxCommandEvent & WXUNUSED(event))
{
    hexPadSet1_ = !hexPadSet1_;
    if (hexPadA_)
    {
        if (hexPadSet1_)
            XRCCTRL(*this, "PadText", wxStaticText)->SetLabel("Hex Keypad Definition: A, Set 1");
        else
            XRCCTRL(*this, "PadText", wxStaticText)->SetLabel("Hex Keypad Definition: A, Set 2");
    }
    else
    {
        if (hexPadSet1_)
            XRCCTRL(*this, "PadText", wxStaticText)->SetLabel("Hex Keypad Definition: B, Set 1");
        else
            XRCCTRL(*this, "PadText", wxStaticText)->SetLabel("Hex Keypad Definition: B, Set 2");
    }
    
    updateButtons();
    inKey_ = -1;
    hexKey_ = -1;
}

void KeyMapDialog::onKeyDown(wxKeyEvent& event)
{
	if (hexKey_ != -1)
	{
		int key = event.GetKeyCode();
		setLabel("HexKey%01X", hexKey_, key);
		if (hexPadA_)
			if (hexPadSet1_)
				hexKeyDefA1_[hexKey_] = key;
			else
				hexKeyDefA2_[hexKey_] = key;
		else
			if (hexPadSet1_)
				hexKeyDefB1_[hexKey_] = key;
			else
				hexKeyDefB2_[hexKey_] = key;
		hexKey_ = -1;
        updateButtons();
	}
	if (inKey_ != -1)
	{
		int key = event.GetKeyCode();
		setLabel("InButton", 20, key);
        if (hexPadSet1_)
            inButton1_ = key;
        else
            inButton2_ = key;
		inKey_ = -1;
        updateButtons();
	}
    if (arcadeADirKey_ != -1)
    {
        int key = event.GetKeyCode();
        keyDefGameHexA_[arcadeADirKey_] = key;
        
        setLabel("KeyTextA%01X", arcadeADirKey_, keyDefGameHexA_[arcadeADirKey_]);
        arcadeADirKey_ = -1;
        updateButtonsCoinArcade();
    }
    if (arcadeBDirKey_ != -1)
    {
        int key = event.GetKeyCode();
        keyDefGameHexB_[arcadeBDirKey_] = key;
        setLabel("KeyTextB%01X", arcadeBDirKey_, keyDefGameHexB_[arcadeBDirKey_]);
        arcadeBDirKey_ = -1;
        updateButtonsCoinArcade();
    }
    if (arcadeCoinKey_ != -1)
    {
        int key = event.GetKeyCode();
        keyDefCoin_ = key;
        setLabel("CoinButton", 20, keyDefCoin_);
        arcadeCoinKey_ = -1;
        updateButtonsCoinArcade();
    }
}

void KeyMapDialog::connectKeyDownEvent(wxWindow* pclComponent) 
{ 
  if(pclComponent) 
  {
      pclComponent->Connect(wxID_ANY,
                          wxEVT_KEY_DOWN, 
                          wxKeyEventHandler(KeyMapDialog::onKeyDown), 
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

wxString KeyMapDialog::getKeyStr(int key)
{
	wxString keyStr;
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
	return keyStr;
}

void KeyMapDialog::setLabel(wxString printStr, long button, int key)
{
	wxString keyStr, keyStrNum, buttonStr;


    keyStrNum.Printf("(%i)", key);
    keyStr = getKeyStr(key) + keyStrNum;
    
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

void KeyMapDialog::onAuto(wxCommandEvent&event)
{
    autoGame_ = event.IsChecked();
    enableAuto(autoGame_);
}

void KeyMapDialog::onStudioEnableDiagonal(wxCommandEvent&event)
{
	if (hexPadA_)
		simDefA2_ = event.IsChecked();
	else
		simDefB2_ = event.IsChecked();

    updateButtons();
}

void KeyMapDialog::enableAuto(bool status)
{
    wxString button;
    for (int i= 0; i < numberOfKeys_; i++)
    {
        button.Printf("HexKey%01X",i);
        XRCCTRL(*this, button, wxButton)->Enable(!status);
    }
    
    if ((computerTypeStr_ == "Studio2") || (computerTypeStr_ == "Visicom") || (computerTypeStr_ == "Victory"))
    {
        XRCCTRL(*this, "StudioChar", wxButton)->Enable(!status);
        XRCCTRL(*this, "StudioLocation", wxButton)->Enable(!status);
    }
    else
    {
        if (computerTypeStr_ == "StudioIV")
        {
            XRCCTRL(*this, "TinyBASIC", wxButton)->Enable(!status);
            XRCCTRL(*this, "HexChar", wxButton)->Enable(!status);
            XRCCTRL(*this, "HexLocation", wxButton)->Enable(!status);
        }
        else
        {
            XRCCTRL(*this, "HexChar", wxButton)->Enable(!status);
            XRCCTRL(*this, "HexLocation", wxButton)->Enable(!status);
        }
    }
}

void KeyMapDialog::onFocusTimer(wxTimerEvent& WXUNUSED(event))
{
    XRCCTRL(*this, "Focus", wxTextCtrl)->SetFocus();
}

void KeyMapDialog::updateButtonsCoinArcade()
{
    wxString keyStr, keyStrNum;
    
    keyStrNum.Printf("(%i)", keyDefGameHexA_[KEY_UP]);
    keyStr = getKeyStr(keyDefGameHexA_[KEY_UP])+keyStrNum;
    XRCCTRL(*this, "KeyTextA0", wxButton)->SetLabel(keyStr);
    keyStrNum.Printf("(%i)", keyDefGameHexA_[KEY_LEFT]);
    keyStr = getKeyStr(keyDefGameHexA_[KEY_LEFT])+keyStrNum;
    XRCCTRL(*this, "KeyTextA1", wxButton)->SetLabel(keyStr);
    keyStrNum.Printf("(%i)", keyDefGameHexA_[KEY_RIGHT]);
    keyStr = getKeyStr(keyDefGameHexA_[KEY_RIGHT])+keyStrNum;
    XRCCTRL(*this, "KeyTextA2", wxButton)->SetLabel(keyStr);
    keyStrNum.Printf("(%i)", keyDefGameHexA_[KEY_DOWN]);
    keyStr = getKeyStr(keyDefGameHexA_[KEY_DOWN])+keyStrNum;
    XRCCTRL(*this, "KeyTextA3", wxButton)->SetLabel(keyStr);
    keyStrNum.Printf("(%i)", keyDefGameHexA_[KEY_FIRE]);
    keyStr = getKeyStr(keyDefGameHexA_[KEY_FIRE])+keyStrNum;
    XRCCTRL(*this, "KeyTextA4", wxButton)->SetLabel(keyStr);

    keyStrNum.Printf("(%i)", keyDefGameHexB_[KEY_UP]);
    keyStr = getKeyStr(keyDefGameHexB_[KEY_UP])+keyStrNum;
    XRCCTRL(*this, "KeyTextB0", wxButton)->SetLabel(keyStr);
    keyStrNum.Printf("(%i)", keyDefGameHexB_[KEY_LEFT]);
    keyStr = getKeyStr(keyDefGameHexB_[KEY_LEFT])+keyStrNum;
    XRCCTRL(*this, "KeyTextB1", wxButton)->SetLabel(keyStr);
    keyStrNum.Printf("(%i)", keyDefGameHexB_[KEY_RIGHT]);
    keyStr = getKeyStr(keyDefGameHexB_[KEY_RIGHT])+keyStrNum;
    XRCCTRL(*this, "KeyTextB2", wxButton)->SetLabel(keyStr);
    keyStrNum.Printf("(%i)", keyDefGameHexB_[KEY_DOWN]);
    keyStr = getKeyStr(keyDefGameHexB_[KEY_DOWN])+keyStrNum;
    XRCCTRL(*this, "KeyTextB3", wxButton)->SetLabel(keyStr);
    keyStrNum.Printf("(%i)", keyDefGameHexB_[KEY_FIRE]);
    keyStr = getKeyStr(keyDefGameHexB_[KEY_FIRE])+keyStrNum;
    XRCCTRL(*this, "KeyTextB4", wxButton)->SetLabel(keyStr);

    keyStrNum.Printf("(%i)", keyDefCoin_);
    keyStr = getKeyStr(keyDefCoin_)+keyStrNum;
    XRCCTRL(*this, "CoinButton", wxButton)->SetLabel(keyStr);
}

void KeyMapDialog::onArcadeADirKey(wxCommandEvent &event)
{
    wxString buttonName, buttonNumber;
    
    if (arcadeADirKey_ != -1)
    {
        setLabel("KeyTextA%01X", arcadeADirKey_, keyDefGameHexA_[arcadeADirKey_]);
        arcadeADirKey_ = -1;
    }
    if (arcadeBDirKey_ != -1)
    {
        setLabel("KeyTextB%01X", arcadeBDirKey_, keyDefGameHexB_[arcadeBDirKey_]);
        arcadeBDirKey_ = -1;
    }
    if (arcadeCoinKey_ != -1)
    {
        setLabel("CoinButton", 20, keyDefCoin_);
        arcadeCoinKey_ = -1;
    }
    
    buttonName = wxWindow::FindWindowById(event.GetId())->GetName();
    buttonNumber = buttonName.Last();
    if (buttonNumber.ToLong(&arcadeADirKey_, 16))
        XRCCTRL(*this, buttonName, wxButton)->SetLabel("Press Key");
    else
        arcadeADirKey_ = -1;
}

void KeyMapDialog::onArcadeBDirKey(wxCommandEvent &event)
{
    wxString buttonName, buttonNumber;
    
    if (arcadeADirKey_ != -1)
    {
        setLabel("KeyTextA%01X", arcadeADirKey_, keyDefGameHexA_[arcadeADirKey_]);
        arcadeADirKey_ = -1;
    }
    if (arcadeBDirKey_ != -1)
    {
        setLabel("KeyTextB%01X", arcadeBDirKey_, keyDefGameHexB_[arcadeBDirKey_]);
        arcadeBDirKey_ = -1;
    }
    if (arcadeCoinKey_ != -1)
    {
        setLabel("CoinButton", 20, keyDefCoin_);
        arcadeCoinKey_ = -1;
    }
    
    buttonName = wxWindow::FindWindowById(event.GetId())->GetName();
    buttonNumber = buttonName.Last();
    if (buttonNumber.ToLong(&arcadeBDirKey_, 16))
        XRCCTRL(*this, buttonName, wxButton)->SetLabel("Press Key");
    else
        arcadeBDirKey_ = -1;
}

void KeyMapDialog::onArcadeCoinKey(wxCommandEvent& WXUNUSED(event))
{
    wxString buttonName, buttonNumber;
    
    if (arcadeADirKey_ != -1)
    {
        setLabel("KeyTextA%01X", arcadeADirKey_, keyDefGameHexA_[arcadeADirKey_]);
        arcadeADirKey_ = -1;
    }
    if (arcadeBDirKey_ != -1)
    {
        setLabel("KeyTextB%01X", arcadeBDirKey_, keyDefGameHexB_[arcadeBDirKey_]);
        arcadeBDirKey_ = -1;
    }
    if (arcadeCoinKey_ != -1)
    {
        setLabel("CoinButton", 20, keyDefCoin_);
        arcadeCoinKey_ = -1;
    }
    else
    {
        arcadeCoinKey_ = 20;
        XRCCTRL(*this, "CoinButton", wxButton)->SetLabel("Press Key");
    }
}

void KeyMapDialog::onSaveArcadeCoin( wxCommandEvent& WXUNUSED(event) )
{
    p_Main->storeDefaultCoinArcadeKeys(keyDefGameHexA_, keyDefGameHexB_, keyDefCoin_);
    if (p_Computer != NULL)
        p_Computer->reDefineKeys(keyDefGameHexA_, keyDefGameHexB_, keyDefCoin_);
    
    EndModal( wxID_OK );
}

void KeyMapDialog::onDefaultArcadeCoin(wxCommandEvent& WXUNUSED(event))
{
    keyDefGameHexA_[0] = 315;
    keyDefGameHexA_[1] = 314;
    keyDefGameHexA_[2] = 316;
    keyDefGameHexA_[3] = 317;
    keyDefGameHexA_[4] = 32;
    
    keyDefGameHexB_[0] = 87;
    keyDefGameHexB_[1] = 65;
    keyDefGameHexB_[2] = 83;
    keyDefGameHexB_[3] = 90;
    keyDefGameHexB_[4] = 9;
    
    keyDefCoin_ = 67;
    updateButtonsCoinArcade();
}
