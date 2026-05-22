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
#include "mc6845_config.h"
#include "wx/tglbtn.h"
#include "wx/xrc/xmlres.h"

#define THIS_PANEL_NAME "PanelVideoMc6845"

BEGIN_EVENT_TABLE(Mc6845Config, Mc6847Config)
    EVT_TEXT_ENTER(XRCID("Mc6845Address"), Mc6845Config::Mc6845Address)
    EVT_TEXT_ENTER(XRCID("Mc6845Data"), Mc6845Config::Mc6845Data)
    EVT_TEXT_ENTER(XRCID("Mc6845R0"), Mc6845Config::Mc6845RegisterByte)
    EVT_TEXT_ENTER(XRCID("Mc6845R1"), Mc6845Config::Mc6845RegisterByte)
    EVT_TEXT_ENTER(XRCID("Mc6845R2"), Mc6845Config::Mc6845RegisterByte)
    EVT_TEXT_ENTER(XRCID("Mc6845R3"), Mc6845Config::Mc6845RegisterByte)
    EVT_TEXT_ENTER(XRCID("Mc6845R4"), Mc6845Config::Mc6845RegisterByte)
    EVT_TEXT_ENTER(XRCID("Mc6845R5"), Mc6845Config::Mc6845RegisterByte)
    EVT_TEXT_ENTER(XRCID("Mc6845R6"), Mc6845Config::Mc6845RegisterByte)
    EVT_TEXT_ENTER(XRCID("Mc6845R7"), Mc6845Config::Mc6845RegisterByte)
    EVT_TEXT_ENTER(XRCID("Mc6845R8"), Mc6845Config::Mc6845RegisterByte)
    EVT_TEXT_ENTER(XRCID("Mc6845R9"), Mc6845Config::Mc6845RegisterByte)
    EVT_TEXT_ENTER(XRCID("Mc6845RA"), Mc6845Config::Mc6845RegisterByte)
    EVT_TEXT_ENTER(XRCID("Mc6845RB"), Mc6845Config::Mc6845RegisterByte)
    EVT_TEXT_ENTER(XRCID("Mc6845RC"), Mc6845Config::Mc6845RegisterWord)
    EVT_TEXT_ENTER(XRCID("Mc6845RE"), Mc6845Config::Mc6845RegisterWord)
END_EVENT_TABLE()

wxString registerIdMc6845[] =
{
    "Mc6845R0",
    "Mc6845R1",
    "Mc6845R2",
    "Mc6845R3",
    "Mc6845R4",
    "Mc6845R5",
    "Mc6845R6",
    "Mc6845R7",
    "Mc6845R8",
    "Mc6845R9",
    "Mc6845RA",
    "Mc6845RB",
    "Mc6845RC", // Register C+D
    "Mc6845RE", // Register E+F
    "Mc6845Address",
    "Mc6845Data",
};

wxString registerFunctionMc6845[] =
{
    "HS Total",
    "HS Display",
    "HS Position",
    "HS Width",
    "VS Total",
    "VS Adjust",
    "VS Display",
    "VS Position",
    "Interlace",
    "Max Scan Line",
    "Cursor Start",
    "Cursor End",
    "Start Address",
    "Cursor",
    "Address",
    "Data",
};

Mc6845Config::Mc6845Config(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: Mc6847Config(title, pos, size, mode, dataDir, iniDir)
{
}

Mc6845Config::~Mc6845Config()
{
}

void Mc6845Config::mc6845ConfigInit()
{
    for (size_t registerNumber = 0; registerNumber<MC6845_NUMBER_OF_REGISTERS; registerNumber++)
    {
        mc6845ConfigRegisterValueString[registerNumber] = "";
        lastMc6845ConfigRegisterValueString[registerNumber] = "x";
    }

    computerConfiguration.mc6845Configuration.defined = false;

    computerConfiguration.mc6845Configuration.defaultX = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    computerConfiguration.mc6845Configuration.defaultY = mainWindowY_;
    computerConfiguration.mc6845Configuration.address = -1;
    computerConfiguration.mc6845Configuration.addressMask = 0xFFFF;
    computerConfiguration.mc6845Configuration.data = -1;
    computerConfiguration.mc6845Configuration.dataMask = 0xFFFF;
    computerConfiguration.mc6845Configuration.ef = init_EfFlag();
    computerConfiguration.mc6845Configuration.charSize.x = 8;
    computerConfiguration.mc6845Configuration.charSize.y = 8;
    computerConfiguration.mc6845Configuration.screenSize.x = 64;
    computerConfiguration.mc6845Configuration.screenSize.y = 16;
    computerConfiguration.mc6845Configuration.ioGroupVector.clear();

    if (!mode_.gui)
        return;

    XRCCTRL(*this, THIS_PANEL_NAME, wxPanel)->Hide();

    XRCCTRL(*this, "Mc6845VideoRam", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "Mc6845Ef", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "Mc6845IoGroupText", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "Mc6845Interlace", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "Mc6845CharSizeText", wxStaticText)->SetLabel("Character size: 8x8");
}

void Mc6845Config::parseXml_MC6845Video(wxXmlNode &node)
{
    videoConfigUsed(THIS_PANEL_NAME);

    long start, end;
    long width, height;

    if (!computerConfiguration.mc6845Configuration.defined)
    {
        computerConfiguration.mc6845Configuration.videoNumber = computerConfiguration.numberOfVideoTypes_;
        computerConfiguration.numberOfVideoTypes_++;
    }

    computerConfiguration.mc6845Configuration.defined = true;
    computerConfiguration.zoom_[computerConfiguration.mc6845Configuration.videoNumber] = "2.00";
    computerConfiguration.videoName_[computerConfiguration.mc6845Configuration.videoNumber] = "MC6845";

    wxString tagList[]=
    {
        "ram",
        "font",
        "dirname",
        "out",
        "ef",
        "interlace",
        "iogroup",
        "zoom",
        "screen",
        "char",
        "border",
        "pos",
        "color",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_RAM,
        TAG_FONT,
        TAG_DIRNAME,
        TAG_OUT,
        TAG_EF,
        TAG_INTERLACE,
        TAG_IOGROUP,
        TAG_ZOOM,
        TAG_SCREEN,
        TAG_CHAR,
        TAG_BORDER,
        TAG_POS,
        TAG_COLOR,
        TAG_COMMENT,
        TAG_UNDEFINED
    };

    int tagTypeInt;
    Word mask;
    int red, green, blue, xpos, ypos;
    wxString color, position, iogroup, label;
    size_t ioGroupNumber = 0;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_RAM:
                if (!parseXml_Range(*child, &start, &end))
                {
                    warningText_ += "No MC6845 RAM range defined";
                    warningText_ += childName;
                    warningText_ += "\n";
                    if (mode_.gui)
                        XRCCTRL(*this, "Mc6845VideoRam", wxStaticText)->SetLabel("No MC6845 RAM range defined");
                }
                else
                {
                    computerConfiguration.mc6845Configuration.startRam = (Word)start;
                    computerConfiguration.mc6845Configuration.endRam = (Word)end;
                    mask = (Word)end - start;
                    
                    computerConfiguration.mc6845Configuration.ramMask = 0x3FFF;
                    while ((mask & 0x2000) == 0)
                    {
                        computerConfiguration.mc6845Configuration.ramMask = computerConfiguration.mc6845Configuration.ramMask >> 1;
                        mask = mask << 1;
                    }
                    label.Printf("Video RAM: %04X-%04X", (Word)start, (Word)end);
                    if (mode_.gui)
                        XRCCTRL(*this, "Mc6845VideoRam", wxStaticText)->SetLabel(label);
                }
             break;
                
            case TAG_OUT:
                if (child->GetAttribute("type") == "register")
                {
                    computerConfiguration.mc6845Configuration.address = (int)parseXml_Number(*child);
                    computerConfiguration.mc6845Configuration.addressMask = (int)parseXml_Number(*child, "mask");
                    if (computerConfiguration.mc6845Configuration.addressMask == 0)
                        computerConfiguration.mc6845Configuration.addressMask = 0xFFFF;
                }
                if (child->GetAttribute("type") == "data")
                {
                    computerConfiguration.mc6845Configuration.data = (int)parseXml_Number(*child);
                    computerConfiguration.mc6845Configuration.dataMask = (int)parseXml_Number(*child, "mask");
                    if (computerConfiguration.mc6845Configuration.dataMask == 0)
                        computerConfiguration.mc6845Configuration.dataMask = 0xFFFF;
                }
           break;
                
            case TAG_FONT:
                computerConfiguration.characterRomConfiguration.fileName = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                computerConfiguration.characterRomConfiguration.directory = dataDir_ + child->GetNodeContent();
                if (computerConfiguration.characterRomConfiguration.directory.Right(1) != pathSeparator_)
                    computerConfiguration.characterRomConfiguration.directory += pathSeparator_;
            break;

            case TAG_EF:
                computerConfiguration.mc6845Configuration.ef = parseXml_EfFlag(*child, MC6845_EF);
                setEfFlagConfig(computerConfiguration.mc6845Configuration.ef, "display enable", "Mc6845Ef");
            break;
                
            case TAG_INTERLACE:
                computerConfiguration.interlace_ = true;
                if (mode_.gui)
                    XRCCTRL(*this, "Mc6845Interlace", wxStaticText)->SetLabel("Force interlace");
            break;
                
            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.mc6845Configuration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.mc6845Configuration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
                if (mode_.gui)
                    XRCCTRL(*this,"Mc6845IoGroupText", wxStaticText)->SetLabel(p_Main->getGroupMessageXml(&computerConfiguration.mc6845Configuration.ioGroupVector));
            break;

            case TAG_ZOOM:
                computerConfiguration.zoom_[computerConfiguration.mc6845Configuration.videoNumber] = child->GetNodeContent();
            break;

            case TAG_CHAR:
                if (!parseXml_Size(*child, &width, &height))
                {
                    warningText_ += "Incorrect char size";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    computerConfiguration.mc6845Configuration.charSize.x = (int)width;
                    computerConfiguration.mc6845Configuration.charSize.y = (int)height;
                }
                label.Printf("Character size: %dx%d", (int)width, (int)height);
                if (mode_.gui)
                    XRCCTRL(*this, "Mc6845CharSizeText", wxStaticText)->SetLabel(label);
            break;

            case TAG_SCREEN:
                if (!parseXml_Size(*child, &width, &height))
                {
                    warningText_ += "Incorrect screen size";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    computerConfiguration.mc6845Configuration.screenSize.x = (int)width;
                    computerConfiguration.mc6845Configuration.screenSize.y = (int)height;
                }
            break;

            case TAG_BORDER:
                if (!parseXml_Size(*child, &width, &height))
                {
                    warningText_ += "Incorrect border size";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    screenInfo.borderX[VIDEOXML6845] = (int)width;
                    screenInfo.borderY[VIDEOXML6845] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += computerConfiguration.mc6845Configuration.defaultX;
                if (ypos != -1)
                    ypos += computerConfiguration.mc6845Configuration.defaultY;
                computerConfiguration.mc6845Configuration.defaultX = xpos;
                computerConfiguration.mc6845Configuration.defaultY = ypos;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "fore")
                    screenInfo.defaultColour[COL_MC6845_FORE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back")
                    screenInfo.defaultColour[COL_MC6845_BACK].Printf("#%02X%02X%02X", red, green, blue);
            break;
                
            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
    currentInterLace_ = computerConfiguration.interlace_;
}

void Mc6845Config::updateMc6845Panel()
{
    if (computerConfiguration.mc6845Configuration.defined)
    {
        for (size_t registerNumber = 0; registerNumber<MC6845_NUMBER_OF_REGISTERS; registerNumber++)
        {
            if (mc6845ConfigRegisterValueString[registerNumber] != lastMc6845ConfigRegisterValueString[registerNumber])
            {
                p_Main->setTextCtrl(registerIdMc6845[registerNumber], mc6845ConfigRegisterValueString[registerNumber]);
                lastMc6845ConfigRegisterValueString[registerNumber] = mc6845ConfigRegisterValueString[registerNumber];
            }
        }
    }
    if (currentInterLace_ != computerConfiguration.interlace_)
    {
        currentInterLace_ = computerConfiguration.interlace_;
        if (computerConfiguration.interlace_)
            XRCCTRL(*this, "Mc6845Interlace", wxStaticText)->SetLabel("Force interlace");
        else
            XRCCTRL(*this, "Mc6845Interlace", wxStaticText)->SetLabel("");
    }
}

int Mc6845Config::setMc6845Register(int registerNumber, Word value, int showTrace)
{
    mc6845ConfigRegisterValueString[registerNumber].Printf("%04X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdMc6845[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showVideoTraceText(registerFunctionMc6845[registerNumber], mc6845ConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
      }
    return showTrace;
}

int Mc6845Config::setMc6845Register(int registerNumber, Byte value, int showTrace)
{
    mc6845ConfigRegisterValueString[registerNumber].Printf("%02X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdMc6845[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showVideoTraceText(registerFunctionMc6845[registerNumber], mc6845ConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
      }
    return showTrace;
}

int Mc6845Config::setMc6845RegisterNibble(int registerNumber, Byte value, int showTrace)
{
    mc6845ConfigRegisterValueString[registerNumber].Printf("%01X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdMc6845[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showVideoTraceText(registerFunctionMc6845[registerNumber], mc6845ConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
      }
    return showTrace;
}

bool Mc6845Config::isMc6845TraceChecked(int registerNumber)
{
    if (!mode_.gui)
        return false;
    
    return XRCCTRL(*this, registerIdMc6845[registerNumber]+"Trace", wxCheckBox)->IsChecked();
}

void Mc6845Config::Mc6845Address(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    long value = get8BitValue("Mc6845Address") & 0x1f;
    if (value == -1)  return;

    mc6845Pointer->writeAddressRegister6845(value, DO_NOT_SHOW_ADDRESS_TRACE);
}

void Mc6845Config::Mc6845Data(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    long value = get8BitValue("Mc6845Data");
    if (value == -1)  return;

    mc6845Pointer->writeData6845(value, DO_NOT_SHOW_ADDRESS_TRACE);
}

void Mc6845Config::Mc6845RegisterByte(wxCommandEvent&event)
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    wxString idReference = wxWindow::FindWindowById(event.GetId())->GetName();
    wxString buttonNumber = idReference.Right(1);
    
    long number;
    if (!buttonNumber.ToLong(&number, 16))
        return;

    long value = get8BitValue("Mc6845R" + buttonNumber);
    if (value == -1)  return;

    mc6845Pointer->writeRegister6845(number, value, DO_NOT_SHOW_ADDRESS_TRACE);
}

void Mc6845Config::Mc6845RegisterWord(wxCommandEvent&event)
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    wxString idReference = wxWindow::FindWindowById(event.GetId())->GetName();
    wxString buttonNumber = idReference.Right(1);

    long number;
    if (!buttonNumber.ToLong(&number, 16))
        return;

    long value = get16BitValue("Mc6845R" + buttonNumber);
    if (value == -1)  return;

    mc6845Pointer->writeRegister6845(number, value >> 8, DO_NOT_SHOW_ADDRESS_TRACE);
    mc6845Pointer->writeRegister6845(number+1, value & 0xff, DO_NOT_SHOW_ADDRESS_TRACE);
}
