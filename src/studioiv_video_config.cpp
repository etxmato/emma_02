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
#include "studioiv_video_config.h"
#include "wx/tglbtn.h"
#include "wx/xrc/xmlres.h"

#define THIS_PANEL_NAME "PanelVideoSt4Video"

BEGIN_EVENT_TABLE(St4VideoConfig, VideoConfig)
    EVT_RADIOBUTTON(XRCID("St4VideoDisableGraphics"), St4VideoConfig::St4VideoDisable)
    EVT_RADIOBUTTON(XRCID("St4VideoEnableGraphics"), St4VideoConfig::St4VideoEnable)
    EVT_TEXT_ENTER(XRCID("St4VideoMode"), St4VideoConfig::St4VideoMode)
    EVT_TEXT_ENTER(XRCID("St4VideoBackGround"), St4VideoConfig::St4VideoBackground)
END_EVENT_TABLE()

wxString registerIdSt4VideoRadio[] =
{
    "St4VideoDisableGraphics",
    "St4VideoEnableGraphics",
    "St4VideoDmaEnable",
};

wxString registerFunctionSt4VideoRadio[] =
{
    "Disable graphics\n(mode bit 4&&5 = 0)",
    "Enable graphics\n(mode bit 4&&5 = 1)",
    "DMA Enable",
};

wxString registerFunctionSt4VideoRadioOn[] =
{
    "Disable graphics",
    "Enable graphics",
    "White foreground",
    "PAL",
    "DMA Enable",
};

wxString registerFunctionSt4VideoRadioOff[] =
{
    "Disable graphics",
    "Enable graphics",
    "Color foreground",
    "NTSC",
    "DMA Enable",
};

wxString registerIdSt4Video[] =
{
    "St4VideoBackGround",
    "St4VideoMode",
};

wxString registerFunctionSt4Video[] =
{
    "Background",
    "Video mode",
};

wxString bacgroundColorSt4Video[] =
{
    "Black background",
    "Red background",
    "Blue background",
    "Magenta background",
    "Green background",
    "Yellow background",
    "Cyan background",
    "Whyte background",
};

wxString selectorIdSt4[] =
{
    "St4WhiteForeground",
    "St4Standard",
};

wxString selectorFunctionSt4[2][2] =
{ // false, true
    {"Color foreground (mode b3)", "White foreground (mode b3)" },
    {"Standard: NTSC (mode b6)", "Standard: PAL (mode b6)" },
};

St4VideoConfig::St4VideoConfig(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: VideoConfig(title, pos, size, mode, dataDir, iniDir)
{
}

St4VideoConfig::~St4VideoConfig()
{
}

void St4VideoConfig::st4VideoConfigInit()
{
    for (size_t registerNumber = 0; registerNumber<ST4_VIDEO_NUMBER_OF_RADIOBUTTONS; registerNumber++)
    {
        st4ConfigRegisterValueBool[registerNumber] = false;
        lastSt4ConfigRegisterValueBool[registerNumber] = true;
    }
    for (size_t registerNumber = 0; registerNumber<ST4_VIDEO_NUMBER_OF_REGISTERS; registerNumber++)
    {
        st4ConfigRegisterValueString[registerNumber] = "";
        lastSt4ConfigRegisterValueString[registerNumber] = "x";
        st4ConfigRegisterValueByte[registerNumber] = 0;
    }
    for (size_t registerNumber = 0; registerNumber<TMS9918_NUMBER_OF_SELECTORS; registerNumber++)
    {
        st4ConfigSelector[registerNumber] = 0;
        updateSt4ConfigSelector[registerNumber] = true;
    }

    computerConfiguration.studio4VideoConfiguration.ioGroupVector.clear();
    computerConfiguration.studio4VideoConfiguration.dmaEnable = init_IoPort();
    computerConfiguration.studio4VideoConfiguration.output = init_IoPort();
    computerConfiguration.studio4VideoConfiguration.ef = init_EfFlag();
    
    computerConfiguration.studio4VideoConfiguration.defined = false;
    computerConfiguration.studio4VideoConfiguration.startRam = -1;
    computerConfiguration.studio4VideoConfiguration.endRam = -1;
    computerConfiguration.studio4VideoConfiguration.xScale = 4;

    computerConfiguration.studio4VideoConfiguration.defaultX = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    computerConfiguration.studio4VideoConfiguration.defaultY = mainWindowY_;

    if (!mode_.gui)
        return;

    XRCCTRL(*this, THIS_PANEL_NAME, wxPanel)->Hide();

    disableIoPortConfigRadio(registerIdSt4VideoRadio[ST4_VIDEO_ENABLE]);
    disableIoPortConfigRadio(registerIdSt4VideoRadio[ST4_VIDEO_DISABLE]);
    disableIoPortConfigRadio(registerIdSt4VideoRadio[ST4_VIDEO_DMA_ENABLE]);

    disableIoPortConfig(registerIdSt4Video[ST4_VIDEO_BACKGROUND]);
    disableIoPortConfig(registerIdSt4Video[ST4_VIDEO_MODE]);

    XRCCTRL(*this, "St4VideoIoGroupText", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "St4VideoEfDisplayText", wxStaticText)->SetLabel("");
	XRCCTRL(*this, "St4VideoColorRamRange", wxStaticText)->SetLabel("");
}

void St4VideoConfig::parseXml_St4Video(wxXmlNode &node)
{
    videoConfigUsed(THIS_PANEL_NAME);

    if (!computerConfiguration.studio4VideoConfiguration.defined)
    {
        computerConfiguration.studio4VideoConfiguration.videoNumber = computerConfiguration.numberOfVideoTypes_;
        computerConfiguration.numberOfVideoTypes_++;
    }

    computerConfiguration.studio4VideoConfiguration.defined = true;

    wxString tagList[]=
    {
        "out",
        "ef",
        "zoom",
        "border",
        "pos",
        "xscale",
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
        TAG_OUT,
        TAG_EF,
        TAG_ZOOM,
        TAG_BORDER,
        TAG_POS,
        TAG_XSCALE,
        TAG_COLOR,
        TAG_COLOR_RAM,
        TAG_IOGROUP,
        TAG_GRAPHICS,
        TAG_HEIGHT,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    long width, height;
    long start, end;
    int red, green, blue, xpos, ypos;
    wxString color, position, iogroup, label;
    size_t ioGroupNumber = 0;

    computerConfiguration.zoom_[computerConfiguration.studio4VideoConfiguration.videoNumber] = "2.00";
    computerConfiguration.videoName_[computerConfiguration.studio4VideoConfiguration.videoNumber] = "Studio IV";

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
            case TAG_OUT:
                if (child->GetAttribute("type") == "color")
                {
                    computerConfiguration.studio4VideoConfiguration.output = parseXml_IoPort(*child, STUDIOIV_VIDEO_OUT);
                    setIoPortConfig(computerConfiguration.studio4VideoConfiguration.output, registerIdSt4Video[ST4_VIDEO_MODE], registerFunctionSt4Video[ST4_VIDEO_MODE], "O");
                    setIoPortConfig(computerConfiguration.studio4VideoConfiguration.output, registerIdSt4Video[ST4_VIDEO_BACKGROUND], registerFunctionSt4Video[ST4_VIDEO_BACKGROUND], "O");
                }
                if (child->GetAttribute("type") == "dma")
                {
                    computerConfiguration.studio4VideoConfiguration.dmaEnable = parseXml_IoPort(*child, STUDIOIV_VIDEO_DMA_ENABLE_OUT);
                    setIoPortConfigRadio(computerConfiguration.studio4VideoConfiguration.dmaEnable, registerIdSt4VideoRadio[ST4_VIDEO_DMA_ENABLE], registerFunctionSt4VideoRadio[ST4_VIDEO_DMA_ENABLE], "O");
                }
                setIoPortConfigRadio(computerConfiguration.studio4VideoConfiguration.output, registerIdSt4VideoRadio[ST4_VIDEO_ENABLE], registerFunctionSt4VideoRadio[ST4_VIDEO_ENABLE], "O");
                setIoPortConfigRadio(computerConfiguration.studio4VideoConfiguration.output, registerIdSt4VideoRadio[ST4_VIDEO_DISABLE], registerFunctionSt4VideoRadio[ST4_VIDEO_DISABLE], "O");
            break;
                
            case TAG_EF:
                computerConfiguration.studio4VideoConfiguration.ef = parseXml_EfFlag(*child);
                setEfFlagConfig(computerConfiguration.studio4VideoConfiguration.ef, "in frame indicator", "St4VideoEfDisplayText");
            break;
                
            case TAG_COLOR_RAM:
                if (!parseXml_Range(*child, &start, &end))
                {
                    warningText_ += "No Studio IV Colour RAM range defined";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    computerConfiguration.studio4VideoConfiguration.startRam = (int)start;
                    computerConfiguration.studio4VideoConfiguration.endRam = (int)end;
                    label.Printf("Color RAM: %04X-%04X", (Word)start, (Word)end);
                    if (mode_.gui)
                        XRCCTRL(*this, "St4VideoColorRamRange", wxStaticText)->SetLabel(label);
                }
            break;

            case TAG_ZOOM:
                computerConfiguration.zoom_[computerConfiguration.studio4VideoConfiguration.videoNumber] = child->GetNodeContent();
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
                    screenInfo.borderX[VIDEOSTUDIOIV] = (int)width/computerConfiguration.studio4VideoConfiguration.xScale;
                    screenInfo.borderY[VIDEOSTUDIOIV] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += computerConfiguration.studio4VideoConfiguration.defaultX;
                if (ypos != -1)
                    ypos += computerConfiguration.studio4VideoConfiguration.defaultY;
                computerConfiguration.studio4VideoConfiguration.defaultX = xpos;
                computerConfiguration.studio4VideoConfiguration.defaultY = ypos;
            break;

            case TAG_XSCALE:
                screenInfo.borderX[VIDEOSTUDIOIV] = (int)screenInfo.borderX[VIDEOSTUDIOIV] * computerConfiguration.studio4VideoConfiguration.xScale;
                computerConfiguration.studio4VideoConfiguration.xScale = getDouble(child->GetNodeContent(), childName, -1, "", false);
                screenInfo.borderX[VIDEOSTUDIOIV] = (int)screenInfo.borderX[VIDEOSTUDIOIV] / computerConfiguration.studio4VideoConfiguration.xScale;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "black")
                    screenInfo.defaultColour[COL_ST4_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "red")
                    screenInfo.defaultColour[COL_ST4_RED].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "blue")
                    screenInfo.defaultColour[COL_ST4_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "magenta")
                    screenInfo.defaultColour[COL_ST4_MAGENTA].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "green")
                    screenInfo.defaultColour[COL_ST4_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "yellow")
                    screenInfo.defaultColour[COL_ST4_YELLOW].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "cyan")
                    screenInfo.defaultColour[COL_ST4_CYAN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "white")
                    screenInfo.defaultColour[COL_ST4_WHITE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_black")
                    screenInfo.defaultColour[COL_ST4_BACK_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_red")
                    screenInfo.defaultColour[COL_ST4_BACK_RED].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_blue")
                    screenInfo.defaultColour[COL_ST4_BACK_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_magenta")
                    screenInfo.defaultColour[COL_ST4_BACK_MAGENTA].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_green")
                    screenInfo.defaultColour[COL_ST4_BACK_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_yellow")
                    screenInfo.defaultColour[COL_ST4_BACK_YELLOW].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_cyan")
                    screenInfo.defaultColour[COL_ST4_BACK_CYAN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_white")
                    screenInfo.defaultColour[COL_ST4_BACK_WHITE].Printf("#%02X%02X%02X", red, green, blue);
            break;
                
            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.studio4VideoConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.studio4VideoConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
                if (mode_.gui)
                    XRCCTRL(*this,"St4VideoIoGroupText", wxStaticText)->SetLabel(p_Main->getGroupMessageXml(&computerConfiguration.studio4VideoConfiguration.ioGroupVector));
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

void St4VideoConfig::updateSt4VideoPanel()
{
    if (computerConfiguration.studio4VideoConfiguration.defined)
    {
        for (size_t registerNumber = 0; registerNumber<ST4_VIDEO_NUMBER_OF_RADIOBUTTONS; registerNumber++)
        {
            if (st4ConfigRegisterValueBool[registerNumber] != lastSt4ConfigRegisterValueBool[registerNumber])
            {
                p_Main->setRadioButton(registerIdSt4VideoRadio[registerNumber], st4ConfigRegisterValueBool[registerNumber]);
                lastSt4ConfigRegisterValueBool[registerNumber] = st4ConfigRegisterValueBool[registerNumber];
            }
        }
        for (size_t registerNumber = 0; registerNumber<ST4_VIDEO_NUMBER_OF_REGISTERS; registerNumber++)
        {
            if (st4ConfigRegisterValueString[registerNumber] != lastSt4ConfigRegisterValueString[registerNumber])
            {
                p_Main->setTextCtrl(registerIdSt4Video[registerNumber], st4ConfigRegisterValueString[registerNumber]);
                lastSt4ConfigRegisterValueString[registerNumber] = st4ConfigRegisterValueString[registerNumber];
            }
        }
        for (size_t selectorNumber = 0; selectorNumber<ST4_VIDEO_NUMBER_OF_SELECTORS; selectorNumber++)
        {
            if (updateSt4ConfigSelector[selectorNumber])
            {
                XRCCTRL(*this, selectorIdSt4[selectorNumber] + "Text", wxStaticText)->SetLabel(selectorFunctionSt4[selectorNumber][st4ConfigSelector[selectorNumber]]);
                updateSt4ConfigSelector[selectorNumber] = false;
            }
        }
    }
}

int St4VideoConfig::setSt4VideoRegister(int registerNumber, bool value, int showTrace)
{
    st4ConfigRegisterValueBool[registerNumber] = value;

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdSt4VideoRadio[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        if (value)
            showVideoTraceText(registerFunctionSt4VideoRadioOn[registerNumber], showTrace);
        else
            showVideoTraceText(registerFunctionSt4VideoRadioOff[registerNumber], showTrace);
        
        if (showTrace == SHOW_ADDRESS_TRACE)
            return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

int St4VideoConfig::setSt4VideoRegister(int registerNumber, Byte value, int showTrace)
{
    st4ConfigRegisterValueByte[registerNumber] = value;
    st4ConfigRegisterValueString[registerNumber].Printf("%02X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdSt4Video[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showVideoTraceText(registerFunctionSt4Video[registerNumber], st4ConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
      }
      return showTrace;
}

int St4VideoConfig::setSt4VideoRegisterNibleBackground(int registerNumber, Byte value, int showTrace)
{
    st4ConfigRegisterValueByte[registerNumber] = value;
    st4ConfigRegisterValueString[registerNumber].Printf("%01X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdSt4Video[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showVideoTraceText(bacgroundColorSt4Video[value&0x7], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

int St4VideoConfig::setSt4SelectorValue(int selectorNumber, bool selectorValue, int showTrace)
{
    if (selectorValue)
        st4ConfigSelector[selectorNumber] = 1;
    else
        st4ConfigSelector[selectorNumber] = 0;
    updateSt4ConfigSelector[selectorNumber] = true;

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,selectorIdSt4[selectorNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showVideoTraceText(selectorFunctionSt4[selectorNumber][st4ConfigSelector[selectorNumber]], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

void St4VideoConfig::St4VideoEnable(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    st4VideoPointer->enableScreen(true, DO_NOT_SHOW_ADDRESS_TRACE);
    st4VideoPointer->outPixieStudioIV(st4ConfigRegisterValueByte[ST4_VIDEO_MODE] | 0x30, DO_NOT_SHOW_ANY_TRACE);
}

void St4VideoConfig::St4VideoDisable(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    st4VideoPointer->enableScreen(false, DO_NOT_SHOW_ADDRESS_TRACE);
    st4VideoPointer->outPixieStudioIV(st4ConfigRegisterValueByte[ST4_VIDEO_MODE] & 0xcf, DO_NOT_SHOW_ANY_TRACE);
}

void St4VideoConfig::St4VideoBackground(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    long value = get8BitValue("St4VideoBackGround");
    if (value == -1)  return;

    st4VideoPointer->st4BackGround((Byte)value, DO_NOT_SHOW_ADDRESS_TRACE);
    st4VideoPointer->outPixieStudioIV((st4ConfigRegisterValueByte[ST4_VIDEO_MODE] & 0xf8) | (value & 0x7), DO_NOT_SHOW_ANY_TRACE);
}

void St4VideoConfig::St4VideoMode(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    long value = get8BitValue("St4VideoMode");
    if (value == -1)  return;

    st4VideoPointer->outPixieStudioIV((Byte)value, DO_NOT_SHOW_ADDRESS_TRACE);
}


