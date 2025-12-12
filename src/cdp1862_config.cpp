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
#include "cdp1864_config.h"
#include "wx/tglbtn.h"
#include "wx/xrc/xmlres.h"

#define THIS_PANEL_NAME "PanelVideoCdp1862"

BEGIN_EVENT_TABLE(Cdp1862Config, Cdp1864Config)
	EVT_TEXT_ENTER(XRCID("Cdp1862BackGround"), Cdp1862Config::Cdp1862Background)
END_EVENT_TABLE()

wxString registerIdCdp1862[] =
{
    "Cdp1862ColorRam",
    "Cdp1862BackGround",
};

wxString registerFunctionCdp1862[] =
{
    "Color RAM",
    "Background",
};

Cdp1862Config::Cdp1862Config(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: Cdp1864Config(title, pos, size, mode, dataDir, iniDir)
{
}

Cdp1862Config::~Cdp1862Config()
{
}

void Cdp1862Config::cdp1862ConfigInit()
{
    for (size_t registerNumber = 0; registerNumber<CDP1862_NUMBER_OF_REGISTERS; registerNumber++)
    {
        cdp1862ConfigRegisterValueString[registerNumber] = "";
        lastCdp1862ConfigRegisterValueString[registerNumber] = "x";
    }

    computerConfiguration.cdp1862Configuration.ioGroupVector.clear();
    computerConfiguration.cdp1862Configuration.background = init_IoPort();
    computerConfiguration.cdp1862Configuration.colorMemory = init_IoPort();
    computerConfiguration.cdp1862Configuration.startRam = -1;
    computerConfiguration.cdp1862Configuration.endRam = -1;
    computerConfiguration.cdp1862Configuration.colorType = PIXIE_COLOR_DEFAULT;
    computerConfiguration.cdp1862Configuration.defined = false;

    XRCCTRL(*this, THIS_PANEL_NAME, wxPanel)->Hide();
	
    disableIoPortConfig(registerIdCdp1862[CDP1862_BACKGROUND]);
    cdp1862ConfigRegisterValueString[CDP1862_BACKGROUND] = "";
    disableIoPortConfig(registerIdCdp1862[CDP1862_COLOR_RAM]);
    cdp1862ConfigRegisterValueString[CDP1862_COLOR_RAM] = "";

    XRCCTRL(*this, "Cdp1862IoGroupText", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "Cdp1862Color", wxStaticText)->SetLabel("Cosmicos color mode");
	XRCCTRL(*this, "Cdp1862ColorRamRange", wxStaticText)->SetLabel("");
}

void Cdp1862Config::parseXml_Cdp1862Video(wxXmlNode &node)
{
    videoConfigUsed(THIS_PANEL_NAME);

    computerConfiguration.cdp1862Configuration.defined = true;

    wxString tagList[]=
    {
        "out",
        "colorram",
        "colortype",
        "color",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_COLOR_RAM,
        TAG_COLOR_TYPE,
        TAG_COLOR,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    long start, end;
    int tagTypeInt;
    int red, green, blue;
    bool whiteDefined = false, blackDefined = false;
    wxString color, scale, position, iogroup, label;
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
            case TAG_COLOR_RAM:
                if (child->HasAttribute("mask"))
                    computerConfiguration.ioMask[CDP1862_COLORRAM_OUT] = (int)parseXml_Number(*child, "mask") & 0xff;
                if (!parseXml_Range(*child, &start, &end))
                {
                    warningText_ += "No CDP1862 Colour RAM range defined";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    computerConfiguration.cdp1862Configuration.startRam = (int)start;
                    computerConfiguration.cdp1862Configuration.endRam = (int)end;
					label.Printf("Color RAM: %04X-%04X", (Word)start, (Word)end);
            		XRCCTRL(*this, "Cdp1862ColorRamRange", wxStaticText)->SetLabel(label);
                }
            break;

            case TAG_COLOR_TYPE:
				if (child->GetNodeContent() == "eti" || child->GetNodeContent() == "hug" || child->GetNodeContent() == "hec")
				{
                    computerConfiguration.cdp1862Configuration.colorType = PIXIE_COLOR_ETI_1862;
                    XRCCTRL(*this, "Cdp1862Color", wxStaticText)->SetLabel("HUG1802 color mode");
                }
                if (child->GetNodeContent() == "vic" || child->GetNodeContent() == "vip")
                {
                    if (child->GetNodeContent() == "vip")
					{
                        computerConfiguration.cdp1862Configuration.colorType = PIXIE_COLOR_VIP_1862;
                        XRCCTRL(*this, "Cdp1862Color", wxStaticText)->SetLabel("VIP color mode");
                    }
                    else
					{
                        computerConfiguration.cdp1862Configuration.colorType = PIXIE_COLOR_VICTORY_1862;
                        XRCCTRL(*this, "Cdp1862Color", wxStaticText)->SetLabel("Victory color mode");
                    }
                    if (whiteDefined && blackDefined)
                    {
                        color = screenInfo.defaultColour[COL_CDP1862_BLACK];
                        screenInfo.defaultColour[COL_CDP1862_BLACK] = screenInfo.defaultColour[COL_CDP1862_WHITE];
                        screenInfo.defaultColour[COL_CDP1862_WHITE] = color;
                    }
                    else
                    {
                        if (whiteDefined)
                            screenInfo.defaultColour[COL_CDP1862_BLACK] = screenInfo.defaultColour[COL_CDP1862_WHITE];
                        if (blackDefined)
                            screenInfo.defaultColour[COL_CDP1862_WHITE] = screenInfo.defaultColour[COL_CDP1862_BLACK];
                    }
                }

                if (child->GetNodeContent() == "tmc")
				{
                    computerConfiguration.cdp1862Configuration.colorType = PIXIE_COLOR_TMC2000_1862;
                    XRCCTRL(*this, "Cdp1862Color", wxStaticText)->SetLabel("Telmac color mode");
                }
                if (child->GetNodeContent() == "cos")
				{
                    computerConfiguration.cdp1862Configuration.colorType = PIXIE_COLOR_DEFAULT;
					XRCCTRL(*this, "Cdp1862Color", wxStaticText)->SetLabel("Cosmicos color mode");
				}
            break;
                
            case TAG_OUT:
                if (child->GetAttribute("type") == "back")
				{
                    computerConfiguration.cdp1862Configuration.background = parseXml_IoPort(*child, CDP1862_BACKGROUND_OUT);
				setIoPortConfig(computerConfiguration.cdp1862Configuration.background, registerIdCdp1862[CDP1862_BACKGROUND], registerFunctionCdp1862[CDP1862_BACKGROUND], "O");
				}
                if (child->GetAttribute("type") == "color_ram")
				{
                    computerConfiguration.cdp1862Configuration.colorMemory = parseXml_IoPort(*child, CDP1862_COLORRAM_OUT);
				setIoPortConfig(computerConfiguration.cdp1862Configuration.colorMemory, registerIdCdp1862[CDP1862_COLOR_RAM], registerFunctionCdp1862[CDP1862_COLOR_RAM], "O");
                }
            break;
                
            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "fore")
                    screenInfo.defaultColour[COL_CDP1862_WHITE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back")
                    screenInfo.defaultColour[COL_CDP1862_BACK_BLUE].Printf("#%02X%02X%02X", red, green, blue);

                if (child->GetAttribute("type") == "red")
                    screenInfo.defaultColour[COL_CDP1862_RED].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "blue")
                    screenInfo.defaultColour[COL_CDP1862_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "magenta")
                    screenInfo.defaultColour[COL_CDP1862_MAGENTA].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "green")
                    screenInfo.defaultColour[COL_CDP1862_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "yellow")
                    screenInfo.defaultColour[COL_CDP1862_YELLOW].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "cyan")
                    screenInfo.defaultColour[COL_CDP1862_CYAN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_blue")
                    screenInfo.defaultColour[COL_CDP1862_BACK_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_black")
                    screenInfo.defaultColour[COL_CDP1862_BACK_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_green")
                    screenInfo.defaultColour[COL_CDP1862_BACK_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_red")
                    screenInfo.defaultColour[COL_CDP1862_BACK_RED].Printf("#%02X%02X%02X", red, green, blue);

                if (computerConfiguration.cdp1862Configuration.colorType == PIXIE_COLOR_VICTORY_1862 || computerConfiguration.cdp1862Configuration.colorType == PIXIE_COLOR_VIP_1862)
                {
                    if (child->GetAttribute("type") == "white")
                        screenInfo.defaultColour[COL_CDP1862_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                    if (child->GetAttribute("type") == "black")
                        screenInfo.defaultColour[COL_CDP1862_WHITE].Printf("#%02X%02X%02X", red, green, blue);
                }
                else
                {
                    if (child->GetAttribute("type") == "white")
                    {
                        whiteDefined = true;
                        screenInfo.defaultColour[COL_CDP1862_WHITE].Printf("#%02X%02X%02X", red, green, blue);
                    }
                    if (child->GetAttribute("type") == "black")
                    {
                        blackDefined = true;
                        screenInfo.defaultColour[COL_CDP1862_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                    }
                }
            break;
                
            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.cdp1862Configuration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.cdp1862Configuration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
                XRCCTRL(*this,"Cdp1862IoGroupText", wxStaticText)->SetLabel(p_Main->getGroupMessageXml(&computerConfiguration.cdp1862Configuration.ioGroupVector));
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

void Cdp1862Config::updateCdp1862Panel()
{
    wxString buffer;

    if (computerConfiguration.cdp1862Configuration.defined)
    {
        for (size_t registerNumber = 0; registerNumber<CDP1862_NUMBER_OF_REGISTERS; registerNumber++)
        {
            if (cdp1862ConfigRegisterValueString[registerNumber] != lastCdp1862ConfigRegisterValueString[registerNumber])
            {
                p_Main->setTextCtrl(registerIdCdp1862[registerNumber], cdp1862ConfigRegisterValueString[registerNumber]);
                lastCdp1862ConfigRegisterValueString[registerNumber] = cdp1862ConfigRegisterValueString[registerNumber];
            }
        }
    }
}

int Cdp1862Config::setCdp1862Register(int registerNumber, Byte value, int showTrace)
{
    cdp1862ConfigRegisterValueString[registerNumber].Printf("%02X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdCdp1862[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showTraceText(registerFunctionCdp1862[registerNumber], cdp1862ConfigRegisterValueString[registerNumber], showTrace);
        
        if (showTrace == SHOW_ADDRESS_TRACE)
            return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

int Cdp1862Config::setCdp1862RegisterNible(int registerNumber, Byte value, int showTrace)
{
    cdp1862ConfigRegisterValueString[registerNumber].Printf("%01X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdCdp1862[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showTraceText(registerFunctionCdp1862[registerNumber], cdp1862ConfigRegisterValueString[registerNumber], showTrace);
        
        if (showTrace == SHOW_ADDRESS_TRACE)
            return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

int Cdp1862Config::setCdp1862RegisterNible(int registerNumber, Word address, Byte value, int showTrace)
{
    cdp1862ConfigRegisterValueString[registerNumber].Printf("%01X", value);
    wxString addressString;
    addressString.Printf("%02X", address);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdCdp1862[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showTraceText(registerFunctionCdp1862[registerNumber], addressString, cdp1862ConfigRegisterValueString[registerNumber], showTrace);
        
        if (showTrace == SHOW_ADDRESS_TRACE)
            return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

void Cdp1862Config::Cdp1862Background(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    long value = get8BitValue("Cdp1862BackGround");
    if (value == -1)  return;

    pixiePointer->outCdp1862BackGround(value);
}

