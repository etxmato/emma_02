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
#include "hd44780_config.h"
#include "wx/xrc/xmlres.h"

#define THIS_PANEL_NAME "PanelVideoHd44780"

BEGIN_EVENT_TABLE(Hd44780Config, Mc6845Config)
    EVT_TEXT_ENTER(XRCID("Hd44780Command"), Hd44780Config::Hd44780Command)
    EVT_TEXT_ENTER(XRCID("Hd44780Data"), Hd44780Config::Hd44780Data)
END_EVENT_TABLE()

wxString registerIdHd44780[] =
{
    "Hd44780Command",
    "Hd44780Data",
    "Hd44780Address",
    "Hd44780BusyFlag",
};

wxString registerFunctionHd44780[] =
{
    "Command",
    "Data",
    "Address Counter",
    "Busy Flag",
};

Hd44780Config::Hd44780Config(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: Mc6845Config(title, pos, size, mode, dataDir, iniDir)
{
}

Hd44780Config::~Hd44780Config()
{
}

void Hd44780Config::hd44780ConfigInit()
{
    for (size_t registerNumber = 0; registerNumber < HD44780_NUMBER_OF_REGISTERS; registerNumber++)
    {
        hd44780ConfigRegisterValueString[registerNumber] = "";
        lastHd44780ConfigRegisterValueString[registerNumber] = "x";
    }

    computerConfiguration.hd44780Configuration.defined = false;

    computerConfiguration.hd44780Configuration.defaultX = mainWindowX_ + windowInfo.mainwX + windowInfo.xBorder;
    computerConfiguration.hd44780Configuration.defaultY = mainWindowY_;
    computerConfiguration.hd44780Configuration.commandPort = init_IoPort();
    computerConfiguration.hd44780Configuration.dataPort = init_IoPort();
    computerConfiguration.hd44780Configuration.statusPort = init_IoPort();
    computerConfiguration.hd44780Configuration.dataReadPort = init_IoPort();
    computerConfiguration.hd44780Configuration.ef = init_EfFlag();
    computerConfiguration.hd44780Configuration.charSize.x = 5;
    computerConfiguration.hd44780Configuration.charSize.y = 8;
    computerConfiguration.hd44780Configuration.screenSize.x = 16;
    computerConfiguration.hd44780Configuration.screenSize.y = 2;
    computerConfiguration.hd44780Configuration.ioGroupVector.clear();

    if (!mode_.gui)
        return;

    XRCCTRL(*this, THIS_PANEL_NAME, wxPanel)->Hide();

    XRCCTRL(*this, "Hd44780IoGroupText", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "Hd44780ScreenSizeText", wxStaticText)->SetLabel("Screen: 16x2");
}

void Hd44780Config::parseXml_HD44780Video(wxXmlNode &node)
{
    videoConfigUsed(THIS_PANEL_NAME);

    long width, height;

    if (!computerConfiguration.hd44780Configuration.defined)
    {
        computerConfiguration.hd44780Configuration.videoNumber = computerConfiguration.numberOfVideoTypes_;
        computerConfiguration.numberOfVideoTypes_++;
    }

    computerConfiguration.hd44780Configuration.defined = true;
    computerConfiguration.zoom_[computerConfiguration.hd44780Configuration.videoNumber] = "3.00";
    computerConfiguration.videoName_[computerConfiguration.hd44780Configuration.videoNumber] = "HD44780";

    wxString tagList[]=
    {
        "font",
        "dirname",
        "out",
        "in",
        "ef",
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
        TAG_FONT,
        TAG_DIRNAME,
        TAG_OUT,
        TAG_IN,
        TAG_EF,
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
            case TAG_OUT:
                if (child->GetAttribute("type") == "command")
                {
                    computerConfiguration.hd44780Configuration.commandPort = parseXml_IoPort(*child, HD44780_COMMAND_OUT);
                }
                if (child->GetAttribute("type") == "data")
                {
                    computerConfiguration.hd44780Configuration.dataPort = parseXml_IoPort(*child, HD44780_DATA_OUT);
                }
            break;

            case TAG_IN:
                if (child->GetAttribute("type") == "status")
                {
                    computerConfiguration.hd44780Configuration.statusPort = parseXml_IoPort(*child, HD44780_STATUS_IN);
                }
                if (child->GetAttribute("type") == "data")
                {
                    computerConfiguration.hd44780Configuration.dataReadPort = parseXml_IoPort(*child, HD44780_DATA_IN);
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
                computerConfiguration.hd44780Configuration.ef = parseXml_EfFlag(*child, HD44780_EF);
                setEfFlagConfig(computerConfiguration.hd44780Configuration.ef, "busy", "Hd44780EfDisplayText");
            break;
                
            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.hd44780Configuration.ioGroupVector.resize(ioGroupNumber + 1);
                    computerConfiguration.hd44780Configuration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
                if (mode_.gui)
                    XRCCTRL(*this, "Hd44780IoGroupText", wxStaticText)->SetLabel(p_Main->getGroupMessageXml(&computerConfiguration.hd44780Configuration.ioGroupVector));
            break;

            case TAG_ZOOM:
                computerConfiguration.zoom_[computerConfiguration.hd44780Configuration.videoNumber] = child->GetNodeContent();
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
                    computerConfiguration.hd44780Configuration.charSize.x = (int)width;
                    computerConfiguration.hd44780Configuration.charSize.y = (int)height;
                }
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
                    computerConfiguration.hd44780Configuration.screenSize.x = (int)width;
                    computerConfiguration.hd44780Configuration.screenSize.y = (int)height;
                }
                label.Printf("Screen: %dx%d", (int)width, (int)height);
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
                    screenInfo.borderX[VIDEOXMLHD44780] = (int)width;
                    screenInfo.borderY[VIDEOXMLHD44780] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += computerConfiguration.hd44780Configuration.defaultX;
                if (ypos != -1)
                    ypos += computerConfiguration.hd44780Configuration.defaultY;
                computerConfiguration.hd44780Configuration.defaultX = xpos;
                computerConfiguration.hd44780Configuration.defaultY = ypos;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "fore")
                    screenInfo.defaultColour[COL_HD44780_FORE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back")
                    screenInfo.defaultColour[COL_HD44780_BACK].Printf("#%02X%02X%02X", red, green, blue);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unknown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void Hd44780Config::updateHd44780Panel()
{
    if (computerConfiguration.hd44780Configuration.defined)
    {
        for (size_t registerNumber = 0; registerNumber < HD44780_NUMBER_OF_REGISTERS; registerNumber++)
        {
            if (hd44780ConfigRegisterValueString[registerNumber] != lastHd44780ConfigRegisterValueString[registerNumber])
            {
                p_Main->setTextCtrl(registerIdHd44780[registerNumber], hd44780ConfigRegisterValueString[registerNumber]);
                lastHd44780ConfigRegisterValueString[registerNumber] = hd44780ConfigRegisterValueString[registerNumber];
            }
        }
    }
}

int Hd44780Config::setHd44780Register(int registerNumber, Byte value, int showTrace)
{
    hd44780ConfigRegisterValueString[registerNumber].Printf("%02X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this, registerIdHd44780[registerNumber] + "Trace", wxCheckBox)->IsChecked())
    {
        showVideoTraceText(registerFunctionHd44780[registerNumber], hd44780ConfigRegisterValueString[registerNumber], showTrace);
        
        if (showTrace == SHOW_ADDRESS_TRACE)
            return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

bool Hd44780Config::isHd44780TraceChecked(int registerNumber)
{
    if (!mode_.gui)
        return false;
    
    return XRCCTRL(*this, registerIdHd44780[registerNumber] + "Trace", wxCheckBox)->IsChecked();
}

void Hd44780Config::Hd44780Command(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    long value = get8BitValue("Hd44780Command");
    if (value == -1)  return;

    hd44780Pointer->writeCommand(value);
}

void Hd44780Config::Hd44780Data(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    long value = get8BitValue("Hd44780Data");
    if (value == -1)  return;

    hd44780Pointer->writeData(value);
}
