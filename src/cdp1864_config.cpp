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

#define THIS_PANEL_NAME "PanelVideoCdp1864"

BEGIN_EVENT_TABLE(Cdp1864Config, FredVideoConfig)
    EVT_RADIOBUTTON(XRCID("Cdp1864DisableGraphics"), Cdp1864Config::Cdp1864Disable)
    EVT_RADIOBUTTON(XRCID("Cdp1864EnableGraphics"), Cdp1864Config::Cdp1864Enable)
    EVT_TEXT_ENTER(XRCID("Cdp1864BackGround"), Cdp1864Config::Cdp1864Background)
    EVT_TEXT_ENTER(XRCID("Cdp1864ToneLatch"), Cdp1864Config::Cdp1864ColorToneLatch)
END_EVENT_TABLE()

wxString registerIdCdp1864Radio[] =
{
    "Cdp1864DisableGraphics",
    "Cdp1864EnableGraphics",
};

wxString registerFunctionCdp1864Radio[] =
{
    "Disable graphics",
    "Enable graphics",
};

wxString registerIdCdp1864[] =
{
    "Cdp1864ColorRam",
    "Cdp1864ToneLatch",
    "Cdp1864ToneLatch",
    "Cdp1864BackGround",
};

wxString registerFunctionCdp1864[] =
{
    "Color RAM",
    "Tone Latch",
    "Color/tone Latch",
    "Background",
};

Cdp1864Config::Cdp1864Config(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: FredVideoConfig(title, pos, size, mode, dataDir, iniDir)
{
}

Cdp1864Config::~Cdp1864Config()
{
}

void Cdp1864Config::cdp1864ConfigInit()
{
    for (size_t registerNumber = 0; registerNumber<CDP1864_NUMBER_OF_RADIOBUTTONS; registerNumber++)
    {
        cdp1864ConfigRegisterValue[registerNumber] = false;
        lastCdp1864ConfigRegisterValue[registerNumber] = true;
    }
    for (size_t registerNumber = 0; registerNumber<CDP1864_NUMBER_OF_REGISTERS; registerNumber++)
    {
        cdp1864ConfigRegisterValueString[registerNumber] = "";
        lastCdp1864ConfigRegisterValueString[registerNumber] = "x";
    }

    computerConfiguration.cdp1864Configuration.ioGroupVector.clear();
    computerConfiguration.cdp1864Configuration.disable = init_IoPort();
    computerConfiguration.cdp1864Configuration.enable = init_IoPort();
    computerConfiguration.cdp1864Configuration.toneLatch = init_IoPort();
    computerConfiguration.cdp1864Configuration.colorMemory = init_IoPort();
    computerConfiguration.cdp1864Configuration.background = init_IoPort();
    computerConfiguration.cdp1864Configuration.ef = init_EfFlag();

    computerConfiguration.cdp1864Configuration.defined = false;
    computerConfiguration.cdp1864Configuration.screenOn = false;
    computerConfiguration.cdp1864Configuration.colorType = PIXIE_COLOR_DEFAULT;
    computerConfiguration.cdp1864Configuration.xScale = 4;
    computerConfiguration.cdp1864Configuration.startRam = -1;
    computerConfiguration.cdp1864Configuration.ramMask = 0xff;
    computerConfiguration.cdp1864Configuration.colorLatch = false;
    computerConfiguration.cdp1864Configuration.endRam = -1;

    computerConfiguration.cdp1864Configuration.defaultX = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    computerConfiguration.cdp1864Configuration.defaultY = mainWindowY_;

    XRCCTRL(*this, THIS_PANEL_NAME, wxPanel)->Hide();

    disableIoPortConfigRadio(registerIdCdp1864Radio[CDP1864_ENABLE]);
    lastCdp1864ConfigRegisterValue[CDP1864_ENABLE] = false;
    disableIoPortConfigRadio(registerIdCdp1864Radio[CDP1864_DISABLE]);
    lastCdp1864ConfigRegisterValue[CDP1864_DISABLE] = false;

    disableIoPortConfig(registerIdCdp1864[CDP1864_BACKGROUND]);
    cdp1864ConfigRegisterValueString[CDP1864_BACKGROUND] = "";
    disableIoPortConfig(registerIdCdp1864[CDP1864_COLOR_RAM]);
    cdp1864ConfigRegisterValueString[CDP1864_COLOR_RAM] = "";
    disableIoPortConfig(registerIdCdp1864[CDP1864_TONE_LATCH]);
    cdp1864ConfigRegisterValueString[CDP1864_TONE_LATCH] = "";

    XRCCTRL(*this, "Cdp1864IoGroupText", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "Cdp1864EfDisplayText", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "Cdp1864EfDisplayScreenOn", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "Cdp1864Color", wxStaticText)->SetLabel("Cosmicos color mode");
	XRCCTRL(*this, "Cdp1864ColorRamRange", wxStaticText)->SetLabel("");
}

void Cdp1864Config::parseXml_Cdp1864Video(wxXmlNode &node)
{
    videoConfigUsed(THIS_PANEL_NAME);
	
    if (!computerConfiguration.cdp1864Configuration.defined)
    {
        computerConfiguration.cdp1864Configuration.videoNumber = computerConfiguration.numberOfVideoTypes_;
        computerConfiguration.numberOfVideoTypes_++;
    }

    computerConfiguration.cdp1864Configuration.defined = true;

    wxString tagList[]=
    {
        "io",
        "out",
        "in",
        "ef",
        "zoom",
        "border",
        "pos",
        "xscale",
        "colortype",
        "color",
        "colorram",
        "iogroup",
        "graphics",
        "height",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_IO,
        TAG_OUT,
        TAG_IN,
        TAG_EF,
        TAG_ZOOM,
        TAG_BORDER,
        TAG_POS,
        TAG_XSCALE,
        TAG_COLOR_TYPE,
        TAG_COLOR,
        TAG_COLOR_RAM,
        TAG_IOGROUP,
        TAG_GRAPHICS,
        TAG_HEIGHT,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    long width, height, start, end;
    int red, green, blue, xpos, ypos;
    bool whiteDefined = false, blackDefined = false;
    wxString color, scale, position, iogroup, label;
    size_t ioGroupNumber = 0;

    computerConfiguration.zoom_[computerConfiguration.cdp1864Configuration.videoNumber] = "2.00";
    computerConfiguration.videoName_[computerConfiguration.cdp1864Configuration.videoNumber] = "CDP1864";

    computerConfiguration.cdp1861Configuration.pixieGraphics.interrupt = 62;
    computerConfiguration.cdp1861Configuration.pixieGraphics.start = 64;
    computerConfiguration.cdp1861Configuration.pixieGraphics.end = 191;
    computerConfiguration.cdp1861Configuration.pixieGraphics.screenend = 262;
    computerConfiguration.cdp1861Configuration.pixieGraphics.videoHeight = 192;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_IN:
                if (child->HasAttribute("type"))
                {
                    if (child->GetAttribute("type") == "off")
                    {
                        computerConfiguration.cdp1864Configuration.disable = parseXml_IoPort(*child, CDP1864_DISABLE_IN);
                        setIoPortConfigRadio(computerConfiguration.cdp1864Configuration.disable, registerIdCdp1864Radio[CDP1864_DISABLE], registerFunctionCdp1864Radio[CDP1864_DISABLE], "I");
                    }
                    if (child->GetAttribute("type") == "on")
                    {
                        computerConfiguration.cdp1864Configuration.enable = parseXml_IoPort(*child, CDP1864_ENABLE_IN);
                        setIoPortConfigRadio(computerConfiguration.cdp1864Configuration.enable, registerIdCdp1864Radio[CDP1864_ENABLE], registerFunctionCdp1864Radio[CDP1864_ENABLE], "I");
                    }
                }
                else
                    computerConfiguration.cdp1864Configuration.enable = parseXml_IoPort(*child, CDP1864_ENABLE_IN);
            break;
                
            case TAG_COLOR_TYPE:
                if (child->GetNodeContent() == "eti" || child->GetNodeContent() == "hug" || child->GetNodeContent() == "hec")
                {
                    computerConfiguration.cdp1864Configuration.colorType = PIXIE_COLOR_ETI_1864;
                    XRCCTRL(*this, "Cdp1864Color", wxStaticText)->SetLabel("HUG1802 color mode");
                }
                if (child->GetNodeContent() == "vic" || child->GetNodeContent() == "vip")
                {
                    if (child->GetNodeContent() == "vip")
                    {
                        computerConfiguration.cdp1864Configuration.colorType = PIXIE_COLOR_VIP_1864;
                        XRCCTRL(*this, "Cdp1864Color", wxStaticText)->SetLabel("VIP color mode");
                    }
                    else
                    {
                        computerConfiguration.cdp1864Configuration.colorType = PIXIE_COLOR_VICTORY_1864;
                        XRCCTRL(*this, "Cdp1864Color", wxStaticText)->SetLabel("Victory color mode");
                    }
                    if (whiteDefined && blackDefined)
                    {
                        color = screenInfo.defaultColour[COL_CDP1864_BLACK];
                        screenInfo.defaultColour[COL_CDP1864_BLACK] = screenInfo.defaultColour[COL_CDP1864_WHITE];
                        screenInfo.defaultColour[COL_CDP1864_WHITE] = color;
                    }
                    else
                    {
                        if (whiteDefined)
                            screenInfo.defaultColour[COL_CDP1864_BLACK] = screenInfo.defaultColour[COL_CDP1864_WHITE];
                        if (blackDefined)
                            screenInfo.defaultColour[COL_CDP1864_WHITE] = screenInfo.defaultColour[COL_CDP1864_BLACK];
                    }
                }
                if (child->GetNodeContent() == "tmc")
                {
                    computerConfiguration.cdp1864Configuration.colorType = PIXIE_COLOR_TMC2000_1864;
                    XRCCTRL(*this, "Cdp1864Color", wxStaticText)->SetLabel("Telmac color mode");
                }
                if (child->GetNodeContent() == "cos")
                {
                    computerConfiguration.cdp1864Configuration.colorType = PIXIE_COLOR_DEFAULT;
                    XRCCTRL(*this, "Cdp1864Color", wxStaticText)->SetLabel("Cosmicos color mode");
                }
            break;

            case TAG_OUT:
                if (child->GetAttribute("type") == "back")
                {
                    computerConfiguration.cdp1864Configuration.background = parseXml_IoPort(*child, CDP1864_BACKGROUND_OUT);
                    setIoPortConfig(computerConfiguration.cdp1864Configuration.background, registerIdCdp1864[CDP1864_BACKGROUND], registerFunctionCdp1864[CDP1864_BACKGROUND], "O");
                }
                if (child->GetAttribute("type") == "tone")
                {
                    computerConfiguration.soundConfiguration.type = SOUND_1863_1864;
                    computerConfiguration.cdp1864Configuration.toneLatch = parseXml_IoPort(*child, CDP1864_TONE_LATCH_OUT);
                    setIoPortConfig(computerConfiguration.cdp1864Configuration.toneLatch, registerIdCdp1864[CDP1864_TONE_LATCH], registerFunctionCdp1864[CDP1864_TONE_LATCH], "O");
                }
                if (child->GetAttribute("type") == "color_ram")
                {
                    computerConfiguration.cdp1864Configuration.colorMemory = parseXml_IoPort(*child, CDP1864_COLORRAM_OUT);
                    setIoPortConfig(computerConfiguration.cdp1864Configuration.colorMemory, registerIdCdp1864[CDP1864_COLOR_RAM], registerFunctionCdp1864[CDP1864_COLOR_RAM], "O");
                }
                if (child->GetAttribute("type") == "color_tone")
                {
                    computerConfiguration.cdp1864Configuration.colorLatch = true;
                    computerConfiguration.soundConfiguration.type = SOUND_1863_1864;
                    computerConfiguration.cdp1864Configuration.toneLatch = parseXml_IoPort(*child, CDP1864_TONE_LATCH_OUT);
                    setIoPortConfig(computerConfiguration.cdp1864Configuration.toneLatch, registerIdCdp1864[CDP1864_COLOR_TONE_LATCH], registerFunctionCdp1864[CDP1864_COLOR_TONE_LATCH], "O");
                }
            break;
                
            case TAG_EF:
                if (child->GetAttribute("type") == "on")
                {
                    computerConfiguration.cdp1864Configuration.screenOn = true;
                    XRCCTRL(*this, "Cdp1864EfDisplayScreenOn", wxStaticText)->SetLabel("Screen off: no frame indicator");
                }
                computerConfiguration.cdp1864Configuration.ef = parseXml_EfFlag(*child, CDP1861_IN_FRAME_EF);
                setEfFlagConfig(computerConfiguration.cdp1864Configuration.ef, "in frame indicator", "Cdp1864EfDisplayText");
            break;
                
            case TAG_COLOR_RAM:
                if (child->HasAttribute("mask"))
                    computerConfiguration.cdp1864Configuration.colorMemory.mask = (int)parseXml_Number(*child, "mask") & 0xff;
                if (!parseXml_Range(*child, &start, &end))
                {
                    warningText_ += "No CDP1864 Colour RAM range defined";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    computerConfiguration.cdp1864Configuration.startRam = (int)start;
                    computerConfiguration.cdp1864Configuration.endRam = (int)end;
                    computerConfiguration.cdp1864Configuration.ramMask = (end - start) & 0xff;
					label.Printf("Color RAM: %04X-%04X", (Word)start, (Word)end);
            		XRCCTRL(*this, "Cdp1864ColorRamRange", wxStaticText)->SetLabel(label);
                }
            break;

            case TAG_ZOOM:
                computerConfiguration.zoom_[computerConfiguration.cdp1864Configuration.videoNumber] = child->GetNodeContent();
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
                    screenInfo.borderX[VIDEOXML1864] = (int)width/computerConfiguration.cdp1864Configuration.xScale;
                    screenInfo.borderY[VIDEOXML1864] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += computerConfiguration.cdp1864Configuration.defaultX;
                if (ypos != -1)
                    ypos += computerConfiguration.cdp1864Configuration.defaultY;
                computerConfiguration.cdp1864Configuration.defaultX = xpos;
                computerConfiguration.cdp1864Configuration.defaultY = ypos;
            break;

            case TAG_XSCALE:
                screenInfo.borderX[VIDEOXML1864] = (int)screenInfo.borderX[VIDEOXML1864] * computerConfiguration.cdp1864Configuration.xScale;
                computerConfiguration.cdp1864Configuration.xScale = getDouble(child->GetNodeContent(), childName, -1, "", false);
                screenInfo.borderX[VIDEOXML1864] = (int)screenInfo.borderX[VIDEOXML1864] / computerConfiguration.cdp1864Configuration.xScale;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "fore")
                    screenInfo.defaultColour[COL_CDP1864_WHITE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back")
                    screenInfo.defaultColour[COL_CDP1864_BACK_BLUE].Printf("#%02X%02X%02X", red, green, blue);

                if (child->GetAttribute("type") == "red")
                    screenInfo.defaultColour[COL_CDP1864_RED].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "blue")
                    screenInfo.defaultColour[COL_CDP1864_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "magenta")
                    screenInfo.defaultColour[COL_CDP1864_MAGENTA].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "green")
                    screenInfo.defaultColour[COL_CDP1864_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "yellow")
                    screenInfo.defaultColour[COL_CDP1864_YELLOW].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "cyan")
                    screenInfo.defaultColour[COL_CDP1864_CYAN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_blue")
                    screenInfo.defaultColour[COL_CDP1864_BACK_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_black")
                    screenInfo.defaultColour[COL_CDP1864_BACK_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_green")
                    screenInfo.defaultColour[COL_CDP1864_BACK_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_red")
                    screenInfo.defaultColour[COL_CDP1864_BACK_RED].Printf("#%02X%02X%02X", red, green, blue);

                if (computerConfiguration.cdp1864Configuration.colorType == PIXIE_COLOR_VICTORY_1864 || computerConfiguration.cdp1864Configuration.colorType == PIXIE_COLOR_VIP_1864)
                {
                    if (child->GetAttribute("type") == "white")
                        screenInfo.defaultColour[COL_CDP1864_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                    if (child->GetAttribute("type") == "black")
                        screenInfo.defaultColour[COL_CDP1864_WHITE].Printf("#%02X%02X%02X", red, green, blue);
                }
                else
                {
                    if (child->GetAttribute("type") == "white")
                    {
                        whiteDefined = true;
                        screenInfo.defaultColour[COL_CDP1864_WHITE].Printf("#%02X%02X%02X", red, green, blue);
                    }
                    if (child->GetAttribute("type") == "black")
                    {
                        blackDefined = true;
                        screenInfo.defaultColour[COL_CDP1864_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                    }
                }
            break;
                
            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.cdp1864Configuration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.cdp1864Configuration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
                XRCCTRL(*this,"Cdp1864IoGroupText", wxStaticText)->SetLabel(p_Main->getGroupMessageXml(&computerConfiguration.cdp1864Configuration.ioGroupVector));
            break;

            case TAG_GRAPHICS:
                parseXml_pixieGraphics (*child);
            break;

            case TAG_HEIGHT:
                computerConfiguration.cdp1861Configuration.pixieGraphics.videoHeight = (int)parseXml_Number(*child);
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

void Cdp1864Config::updateCdp1864Panel()
{
    wxString buffer;

    if (computerConfiguration.cdp1864Configuration.defined)
    {
        for (size_t registerNumber = 0; registerNumber<CDP1864_NUMBER_OF_RADIOBUTTONS; registerNumber++)
        {
            if (cdp1864ConfigRegisterValue[registerNumber] != lastCdp1864ConfigRegisterValue[registerNumber])
            {
                p_Main->setRadioButton(registerIdCdp1864Radio[registerNumber], cdp1864ConfigRegisterValue[registerNumber]);
                lastCdp1864ConfigRegisterValue[registerNumber] = cdp1864ConfigRegisterValue[registerNumber];
            }
        }
        for (size_t registerNumber = 0; registerNumber<CDP1864_NUMBER_OF_REGISTERS; registerNumber++)
        {
            if (cdp1864ConfigRegisterValueString[registerNumber] != lastCdp1864ConfigRegisterValueString[registerNumber])
            {
                p_Main->setTextCtrl(registerIdCdp1864[registerNumber], cdp1864ConfigRegisterValueString[registerNumber]);
                lastCdp1864ConfigRegisterValueString[registerNumber] = cdp1864ConfigRegisterValueString[registerNumber];
            }
        }
    }
}

int Cdp1864Config::setCdp1864Register(int registerNumber, bool value, int showTrace)
{
    cdp1864ConfigRegisterValue[registerNumber] = value;

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdCdp1864Radio[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showTraceText(registerFunctionCdp1864Radio[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
      }
      return showTrace;
}

int Cdp1864Config::setCdp1864Register(int registerNumber, Byte value, int showTrace)
{
    cdp1864ConfigRegisterValueString[registerNumber].Printf("%02X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdCdp1864[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showTraceText(registerFunctionCdp1864[registerNumber], cdp1864ConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
      }
      return showTrace;
}

int Cdp1864Config::setCdp1864RegisterNible(int registerNumber, Byte value, int showTrace)
{
    cdp1864ConfigRegisterValueString[registerNumber].Printf("%01X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdCdp1864[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showTraceText(registerFunctionCdp1864[registerNumber], cdp1864ConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
      }
      return showTrace;
}

int Cdp1864Config::setCdp1864RegisterNible(int registerNumber, Word address, Byte value, int showTrace)
{
    cdp1864ConfigRegisterValueString[registerNumber].Printf("%01X", value);
    wxString addressString;
    addressString.Printf("%02X", address);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdCdp1864[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showTraceText(registerFunctionCdp1864[registerNumber], addressString, cdp1864ConfigRegisterValueString[registerNumber], showTrace);

        if (showTrace == SHOW_ADDRESS_TRACE)
            return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}


void Cdp1864Config::Cdp1864Enable(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    cdp1864Pointer->inPixie();
}

void Cdp1864Config::Cdp1864Disable(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    cdp1864Pointer->outPixie();
}

void Cdp1864Config::Cdp1864Background(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    long value = get8BitValue("Cdp1864BackGround");
    if (value == -1)  return;

    cdp1864Pointer->outCdp1864BackGround(value);
}

void Cdp1864Config::Cdp1864ColorToneLatch(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    long value = get8BitValue("Cdp1864ToneLatch");
    if (value == -1)  return;

    p_Computer->setCdp1864ColorToneLatch(value, DO_NOT_SHOW_ADDRESS_TRACE);
}


