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
#include "tms9918_config.h"
#include "wx/tglbtn.h"
#include "wx/xrc/xmlres.h"

#define THIS_PANEL_NAME "PanelVideoTms9918"

BEGIN_EVENT_TABLE(Tms9918Config, I8275Config)
    EVT_TEXT_ENTER(XRCID("Tms9918Register"), Tms9918Config::Tms9918Register)
    EVT_TEXT_ENTER(XRCID("Tms9918Data"), Tms9918Config::Tms9918Data)
    EVT_TEXT_ENTER(XRCID("Tms9918R0"), Tms9918Config::Tms9918RegisterNibble)
    EVT_TEXT_ENTER(XRCID("Tms9918R1"), Tms9918Config::Tms9918RegisterByte)
    EVT_TEXT_ENTER(XRCID("Tms9918R2"), Tms9918Config::Tms9918RegisterWord)
    EVT_TEXT_ENTER(XRCID("Tms9918R3"), Tms9918Config::Tms9918RegisterWord)
    EVT_TEXT_ENTER(XRCID("Tms9918R4"), Tms9918Config::Tms9918RegisterWord)
    EVT_TEXT_ENTER(XRCID("Tms9918R5"), Tms9918Config::Tms9918RegisterWord)
    EVT_TEXT_ENTER(XRCID("Tms9918R6"), Tms9918Config::Tms9918RegisterWord)
    EVT_TEXT_ENTER(XRCID("Tms9918R7"), Tms9918Config::Tms9918RegisterByte)
    EVT_TEXT_ENTER(XRCID("Tms9918Status"), Tms9918Config::Tms9918Status)
    EVT_TEXT_ENTER(XRCID("Tms9918FifthSprite"), Tms9918Config::Tms9918FifthSprite)
    EVT_TEXT_ENTER(XRCID("Tms9918DisplayMode"), Tms9918Config::Tms9918DisplayMode)
    EVT_TEXT_ENTER(XRCID("Tms9918CurrentReadAddress"), Tms9918Config::Tms9918CurrentReadAddress)
    EVT_TEXT_ENTER(XRCID("Tms9918CurrentWriteAddress"), Tms9918Config::Tms9918CurrentWriteAddress)
END_EVENT_TABLE()

wxString registerIdTms9918[] =
{
    "Tms9918R0",
    "Tms9918R1",
    "Tms9918R2",
    "Tms9918R3",
    "Tms9918R4",
    "Tms9918R5",
    "Tms9918R6",
    "Tms9918R7",
    "Tms9918Status",
    "Tms9918FifthSprite",
    "Tms9918DisplayMode",
    "Tms9918Register",
    "Tms9918Data",
    "Tms9918CurrentReadAddress",
    "Tms9918CurrentWriteAddress",
};

wxString registerFunctionTms9918[] =
{
    "Control R0",
    "Control R1",
    "Name table",
    "Color table",
    "Pattern table",
    "Sprite attribute",
    "Sprite pattern",
    "Color",
    "Status",
    "Fifth sprite",
    "Display mode",
    "Register port",
    "VRAM port",
    "Read address",
    "Write address",
};

wxString selectorIdTms9918[] =
{
    "Tms9918DisplayModeSelector",
    "Tms9918MemorySize",
    "Tms9918DisplayEnable",
    "Tms9918InterruprEnable",
    "Tms9918SpriteSize",
};

wxString selectorFunctionTms9918[5][5] =
{ // false, true
    {"Display Mode: Graphics I", "Display Mode: Graphics II", "Display Mode: Multicolor", "", "Display Mode: Text" },
    {"Memory size: 4K", "Memory size: 16K" },
    {"Display disabled", "Display enabled" },
    {"Interrupt disabled", "Interrupt enabled" },
    {"Sprite size: 8x8", "Sprite size: 16x16", "Sprite size: 16x16", "Sprite size: 32x32"}
};

Tms9918Config::Tms9918Config(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: I8275Config(title, pos, size, mode, dataDir, iniDir)
{
}

Tms9918Config::~Tms9918Config()
{
}

void Tms9918Config::tms9918ConfigInit()
{
    for (size_t registerNumber = 0; registerNumber<TMS9918_NUMBER_OF_REGISTERS; registerNumber++)
    {
        tms9918ConfigRegisterValueString[registerNumber] = "";
        lastTms9918ConfigRegisterValueString[registerNumber] = "x";
    }
    for (size_t registerNumber = 0; registerNumber<TMS9918_NUMBER_OF_SELECTORS; registerNumber++)
    {
        tms9918ConfigSelector[registerNumber] = 0;
        updateTms9918ConfigSelector[registerNumber] = true;
    }

    computerConfiguration.tmsConfiguration.ioGroupVector.clear();
    computerConfiguration.tmsConfiguration.efInterrupt = init_EfFlag();
    computerConfiguration.tmsConfiguration.registerOutput = init_IoPort();
    computerConfiguration.tmsConfiguration.dataOutput = init_IoPort();

    computerConfiguration.tmsConfiguration.defined = false;

    computerConfiguration.tmsConfiguration.defaultX = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    computerConfiguration.tmsConfiguration.defaultY = mainWindowY_;

    XRCCTRL(*this, THIS_PANEL_NAME, wxPanel)->Hide();

    XRCCTRL(*this, "Tms9918IoGroupText", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "Tms9918Ef", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "Tms9918MemorySizeText", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "Tms9918DisplayEnableText", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "Tms9918InterruprEnableText", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "Tms9918DisplayModeSelectorText", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "Tms9918SpriteSizeText", wxStaticText)->SetLabel("");

    disableIoPortConfig(registerIdTms9918[TMS9918_REGISTER]);
}

void Tms9918Config::parseXml_TMS9918Video(wxXmlNode &node)
{
    videoConfigUsed(THIS_PANEL_NAME);

    if (!computerConfiguration.tmsConfiguration.defined)
    {
        computerConfiguration.tmsConfiguration.videoNumber = computerConfiguration.numberOfVideoTypes_;
        computerConfiguration.numberOfVideoTypes_++;
    }

    computerConfiguration.tmsConfiguration.defined = true;

    wxString tagList[]=
    {
        "out",
        "ef",
        "zoom",
        "border",
        "pos",
        "color",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_EF,
        TAG_ZOOM,
        TAG_BORDER,
        TAG_POS,
        TAG_COLOR,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    long width, height;
    int red, green, blue, xpos, ypos;
    wxString color, position, iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.zoom_[computerConfiguration.tmsConfiguration.videoNumber] = "2.00";
    computerConfiguration.videoName_[computerConfiguration.tmsConfiguration.videoNumber] = "TMS 9918";

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
                if (child->GetAttribute("type") == "register")
                {
                    computerConfiguration.tmsConfiguration.registerOutput = parseXml_IoPort(*child, TMS_REGISTER_PORT_OUT);
                    setIoPortConfig(computerConfiguration.tmsConfiguration.registerOutput, registerIdTms9918[TMS9918_REGISTER], registerFunctionTms9918[TMS9918_REGISTER], "B");
                }
                if (child->GetAttribute("type") == "data")
                {
                    computerConfiguration.tmsConfiguration.dataOutput = parseXml_IoPort(*child, TMS_DATA_PORT_OUT);
                    setIoPortConfig(computerConfiguration.tmsConfiguration.dataOutput, registerIdTms9918[TMS9918_DATA], registerFunctionTms9918[TMS9918_DATA], "B");
                }
           break;
                
            case TAG_EF:
                computerConfiguration.tmsConfiguration.efInterrupt = parseXml_EfFlag(*child, TMS_INTERRUPT_EF);
                setEfFlagConfig(computerConfiguration.tmsConfiguration.efInterrupt, "interrupt", "Tms9918Ef");
            break;
                
            case TAG_ZOOM:
                computerConfiguration.zoom_[computerConfiguration.tmsConfiguration.videoNumber] = child->GetNodeContent();
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
                    screenInfo.borderX[VIDEOXMLTMS] = (int)width;
                    screenInfo.borderY[VIDEOXMLTMS] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += computerConfiguration.tmsConfiguration.defaultX;
                if (ypos != -1)
                    ypos += computerConfiguration.tmsConfiguration.defaultY;
                computerConfiguration.tmsConfiguration.defaultX = xpos;
                computerConfiguration.tmsConfiguration.defaultY = ypos;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "transparant")
                    screenInfo.defaultColour[COL_TMS_TRANSPARANT].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "black")
                    screenInfo.defaultColour[COL_TMS_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "medium_green")
                    screenInfo.defaultColour[COL_TMS_MEDIUM_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "light_green")
                    screenInfo.defaultColour[COL_TMS_LIGHT_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "dark_blue")
                    screenInfo.defaultColour[COL_TMS_DARK_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "light_blue")
                    screenInfo.defaultColour[COL_TMS_LIGHT_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "dark_red")
                    screenInfo.defaultColour[COL_TMS_DARK_RED].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "aqua_blue")
                    screenInfo.defaultColour[COL_TMS_AQUA_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "medium_red")
                    screenInfo.defaultColour[COL_TMS_MEDIUM_RED].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "light_red")
                    screenInfo.defaultColour[COL_TMS_LIGHT_RED].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "dark_yellow")
                    screenInfo.defaultColour[COL_TMS_DARK_YELLOW].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "light_yellow")
                    screenInfo.defaultColour[COL_TMS_LIGHT_YELLOW].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "dark_green")
                    screenInfo.defaultColour[COL_TMS_DARK_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "purple")
                    screenInfo.defaultColour[COL_TMS_PURPLE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "gray")
                    screenInfo.defaultColour[COL_TMS_GRAY].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "white")
                    screenInfo.defaultColour[COL_TMS_WHITE].Printf("#%02X%02X%02X", red, green, blue);
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.tmsConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.tmsConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
                XRCCTRL(*this,"Tms9918IoGroupText", wxStaticText)->SetLabel(p_Main->getGroupMessageXml(&computerConfiguration.tmsConfiguration.ioGroupVector));
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
}

void Tms9918Config::updateTms9918Panel()
{
    wxString buffer;

    if (computerConfiguration.tmsConfiguration.defined)
    {
        for (size_t registerNumber = 0; registerNumber<TMS9918_NUMBER_OF_REGISTERS; registerNumber++)
        {
            if (tms9918ConfigRegisterValueString[registerNumber] != lastTms9918ConfigRegisterValueString[registerNumber])
            {
                p_Main->setTextCtrl(registerIdTms9918[registerNumber], tms9918ConfigRegisterValueString[registerNumber]);
                lastTms9918ConfigRegisterValueString[registerNumber] = tms9918ConfigRegisterValueString[registerNumber];
            }
        }
        for (size_t selectorNumber = 0; selectorNumber<TMS9918_NUMBER_OF_SELECTORS; selectorNumber++)
        {
            if (updateTms9918ConfigSelector[selectorNumber])
            {
                XRCCTRL(*this, selectorIdTms9918[selectorNumber] + "Text", wxStaticText)->SetLabel(selectorFunctionTms9918[selectorNumber][tms9918ConfigSelector[selectorNumber]]);
                updateTms9918ConfigSelector[selectorNumber] = false;
            }
        }
    }
}

int Tms9918Config::setTms9918Register(int registerNumber, Word value, int showTrace)
{
    tms9918ConfigRegisterValueString[registerNumber].Printf("%04X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdTms9918[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showTraceText(registerFunctionTms9918[registerNumber], tms9918ConfigRegisterValueString[registerNumber], showTrace);
      
        if (showTrace == SHOW_ADDRESS_TRACE)
            return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

int Tms9918Config::setTms9918Register(int registerNumber, Byte value, int showTrace)
{
    tms9918ConfigRegisterValueString[registerNumber].Printf("%02X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdTms9918[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showTraceText(registerFunctionTms9918[registerNumber], tms9918ConfigRegisterValueString[registerNumber], showTrace);
        
        if (showTrace == SHOW_ADDRESS_TRACE)
            return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

int Tms9918Config::setTms9918RegisterNibble(int registerNumber, Byte value, int showTrace)
{
    tms9918ConfigRegisterValueString[registerNumber].Printf("%01X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdTms9918[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showTraceText(registerFunctionTms9918[registerNumber], tms9918ConfigRegisterValueString[registerNumber], showTrace);
        
        if (showTrace == SHOW_ADDRESS_TRACE)
            return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

int Tms9918Config::setTms9918RegisterValue(int registerNumber, Byte value, int showTrace)
{
    tms9918ConfigRegisterValueString[registerNumber].Printf("%02X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdTms9918[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showTraceText("Register value", tms9918ConfigRegisterValueString[registerNumber], showTrace);

        if (showTrace == SHOW_ADDRESS_TRACE)
            return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

int Tms9918Config::setTms9918DisplayMode(Byte mode, int showTrace)
{
    setTms9918SelectorValue(TMS9918_DISPLAY_MODE_SELECTOR, mode);
    return p_Main->setTms9918RegisterNibble(TMS9918_DISPLAY_MODE, mode, showTrace);
}

int Tms9918Config::setTms9918SelectorValue(int selectorNumber, int selectorValue, int showTrace)
{
    tms9918ConfigSelector[selectorNumber] = selectorValue;
    updateTms9918ConfigSelector[selectorNumber] = true;

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,selectorIdTms9918[selectorNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showTraceText(selectorFunctionTms9918[selectorNumber][tms9918ConfigSelector[selectorNumber]], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

int Tms9918Config::setTms9918SelectorValue(int selectorNumber, bool selectorValue, int showTrace)
{
    if (selectorValue)
        tms9918ConfigSelector[selectorNumber] = 1;
    else
        tms9918ConfigSelector[selectorNumber] = 0;
    updateTms9918ConfigSelector[selectorNumber] = true;

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,selectorIdTms9918[selectorNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showTraceText(selectorFunctionTms9918[selectorNumber][tms9918ConfigSelector[selectorNumber]], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

void Tms9918Config::Tms9918Register(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    long value = get8BitValue("Tms9918Register");
    if (value == -1)  return;

    tmsPointer->modeHighOut(value, DO_NOT_SHOW_ADDRESS_TRACE);
}

void Tms9918Config::Tms9918Data(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    long value = get8BitValue("Tms9918Data");
    if (value == -1)  return;

    tmsPointer->writeVRAM(value, DO_NOT_SHOW_ADDRESS_TRACE);
}

void Tms9918Config::Tms9918RegisterNibble(wxCommandEvent&event)
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    wxString idReference = wxWindow::FindWindowById(event.GetId())->GetName();
    wxString buttonNumber = idReference.Right(1);
    
    long number;
    if (!buttonNumber.ToLong(&number, 16))
        return;

    long value = get4BitValue("Tms9918R" + buttonNumber);
    if (value == -1)  return;

    tmsPointer->writeRegisterDirect(number, value, DO_NOT_SHOW_ADDRESS_TRACE);
}

void Tms9918Config::Tms9918RegisterByte(wxCommandEvent&event)
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    wxString idReference = wxWindow::FindWindowById(event.GetId())->GetName();
    wxString buttonNumber = idReference.Right(1);
    
    long number;
    if (!buttonNumber.ToLong(&number, 16))
        return;

    long value = get8BitValue("Tms9918R" + buttonNumber);
    if (value == -1)  return;

    tmsPointer->writeRegisterDirect(number, value, DO_NOT_SHOW_ADDRESS_TRACE);
}

void Tms9918Config::Tms9918RegisterWord(wxCommandEvent&event)
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    wxString idReference = wxWindow::FindWindowById(event.GetId())->GetName();
    wxString buttonNumber = idReference.Right(1);

    long number;
    if (!buttonNumber.ToLong(&number, 16))
        return;

    long value = get16BitValue("Tms9918R" + buttonNumber);
    if (value == -1)  return;

    tmsPointer->writeRegisterDirect(number, value, DO_NOT_SHOW_ADDRESS_TRACE);
}

void Tms9918Config::Tms9918Status(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    long value = get8BitValue("Tms9918Status");
    if (value == -1)  return;

    tmsPointer->setStatus(value);

}

void Tms9918Config::Tms9918FifthSprite(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    long value = get8BitValue("Tms9918FifthSprite");
    if (value == -1)  return;

    tmsPointer->setFifthSprite(value);
}

void Tms9918Config::Tms9918DisplayMode(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    long value = get8BitValue("Tms9918DisplayMode");
    if (value == -1)  return;

    tmsPointer->setDisplayModeDirect(value);
}

void Tms9918Config::Tms9918CurrentReadAddress(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    long value = get16BitValue("Tms9918CurrentReadAddress");
    if (value == -1)  return;

    tmsPointer->setCurrentReadAddress(value);
}

void Tms9918Config::Tms9918CurrentWriteAddress(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    long value = get16BitValue("Tms9918CurrentWriteAddress");
    if (value == -1)  return;

    tmsPointer->setCurrentWriteAddress(value);
}
