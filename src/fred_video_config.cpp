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
#include "fred_video_config.h"
#include "wx/tglbtn.h"
#include "wx/xrc/xmlres.h"

#define THIS_PANEL_NAME "PanelVideoFredVideo"

BEGIN_EVENT_TABLE(FredVideoConfig, Vip2KVideoConfig)
    EVT_RADIOBUTTON(XRCID("FredVideoDisableGraphics"), FredVideoConfig::FredVideoDisable)
    EVT_RADIOBUTTON(XRCID("FredVideoEnableGraphics"), FredVideoConfig::FredVideoEnable)
	EVT_TEXT_ENTER(XRCID("FredVideoType"), FredVideoConfig::FredVideoType)
END_EVENT_TABLE()

wxString registerIdFredVideoRadio[] =
{
    "FredVideoDisableGraphics",
    "FredVideoEnableGraphics",
};

wxString registerFunctionFredVideoRadio[] =
{
    "Disable graphics\n(mode = 0)",
    "Enable graphics\n(mode != 0)",
};

wxString registerIdFredVideo[] =
{
    "FredVideoType",
};

wxString registerFunctionFredVideo[] =
{
    "Video type",
};

FredVideoConfig::FredVideoConfig(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: Vip2KVideoConfig(title, pos, size, mode, dataDir, iniDir)
{
}

FredVideoConfig::~FredVideoConfig()
{
}

void FredVideoConfig::fredVideoConfigInit()
{
    for (size_t registerNumber = 0; registerNumber<FRED_VIDEO_NUMBER_OF_RADIOBUTTONS; registerNumber++)
    {
        fredConfigRegisterValue[registerNumber] = false;
        lastFredConfigRegisterValue[registerNumber] = true;
    }
    for (size_t registerNumber = 0; registerNumber<FRED_VIDEO_NUMBER_OF_REGISTERS; registerNumber++)
    {
        fredConfigRegisterValueString[registerNumber] = "";
        lastFredConfigRegisterValueString[registerNumber] = "x";
    }

    computerConfiguration.fredVideoConfiguration.xScale = 1;
    computerConfiguration.fredVideoConfiguration.ioGroupVector.clear();
	
    computerConfiguration.fredVideoConfiguration.defined = false;

    computerConfiguration.fredVideoConfiguration.defaultX = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    computerConfiguration.fredVideoConfiguration.defaultY = mainWindowY_;

    if (!mode_.gui)
        return;

    XRCCTRL(*this, THIS_PANEL_NAME, wxPanel)->Hide();

    disableIoPortConfigRadio(registerIdFredVideoRadio[FRED_VIDEO_ENABLE]);
    lastFredConfigRegisterValue[FRED_VIDEO_ENABLE] = false;
    disableIoPortConfigRadio(registerIdFredVideoRadio[FRED_VIDEO_DISABLE]);
    lastFredConfigRegisterValue[FRED_VIDEO_DISABLE] = false;

    disableIoPortConfig(registerIdFredVideo[FRED_VIDEO_TYPE]);
    fredConfigRegisterValueString[FRED_VIDEO_TYPE] = "";

    XRCCTRL(*this, "FredVideoIoGroupText", wxStaticText)->SetLabel("");
}

void FredVideoConfig::parseXml_FredVideo(wxXmlNode &node)
{
    videoConfigUsed(THIS_PANEL_NAME);

    if (!computerConfiguration.fredVideoConfiguration.defined)
    {
        computerConfiguration.fredVideoConfiguration.videoNumber = computerConfiguration.numberOfVideoTypes_;
        computerConfiguration.numberOfVideoTypes_++;
    }

    computerConfiguration.cdp1861Configuration.pixieGraphics.videoHeight = 128;
    computerConfiguration.cdp1861Configuration.pixieGraphics.videoWidth = 192;
    
    computerConfiguration.fredVideoConfiguration.defined = true;

    wxString tagList[]=
    {
        "out",
        "iogroup",
        "zoom",
        "border",
        "pos",
        "xscale",
        "color",
        "height",
        "width",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IOGROUP,
        TAG_ZOOM,
        TAG_BORDER,
        TAG_POS,
        TAG_XSCALE,
        TAG_COLOR,
        TAG_HEIGHT,
        TAG_WIDTH,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    long width, height;
    int red, green, blue, xpos, ypos;
    wxString color, position, iogroup;
    size_t ioGroupNumber = 0;
    computerConfiguration.zoom_[computerConfiguration.fredVideoConfiguration.videoNumber] = "2.00";
    computerConfiguration.videoName_[computerConfiguration.fredVideoConfiguration.videoNumber] = "FRED";

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
                computerConfiguration.fredVideoConfiguration.output = parseXml_IoPort(*child, FRED_VIDEO_TYPE_OUT);
				setIoPortConfig(computerConfiguration.fredVideoConfiguration.output, registerIdFredVideo[FRED_VIDEO_TYPE], registerFunctionFredVideo[FRED_VIDEO_TYPE], "O");
                setIoPortConfigRadio(computerConfiguration.fredVideoConfiguration.output, registerIdFredVideoRadio[FRED_VIDEO_ENABLE], registerFunctionFredVideoRadio[FRED_VIDEO_ENABLE], "O");
                setIoPortConfigRadio(computerConfiguration.fredVideoConfiguration.output, registerIdFredVideoRadio[FRED_VIDEO_DISABLE], registerFunctionFredVideoRadio[FRED_VIDEO_DISABLE], "O");
            break;
                
            case TAG_ZOOM:
                computerConfiguration.zoom_[computerConfiguration.fredVideoConfiguration.videoNumber] = child->GetNodeContent();
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
                    screenInfo.borderX[VIDEOFRED] = (int)width/computerConfiguration.fredVideoConfiguration.xScale;
                    screenInfo.borderY[VIDEOFRED] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += computerConfiguration.fredVideoConfiguration.defaultX;
                if (ypos != -1)
                    ypos += computerConfiguration.fredVideoConfiguration.defaultY;
                computerConfiguration.fredVideoConfiguration.defaultX = xpos;
                computerConfiguration.fredVideoConfiguration.defaultY = ypos;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "fore")
                    screenInfo.defaultColour[COL_PIXIE_FORE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back")
                    screenInfo.defaultColour[COL_PIXIE_BACK].Printf("#%02X%02X%02X", red, green, blue);
            break;
                
            case TAG_XSCALE:
                screenInfo.borderX[VIDEOFRED] = (int)screenInfo.borderX[VIDEOFRED] * computerConfiguration.fredVideoConfiguration.xScale;
                computerConfiguration.fredVideoConfiguration.xScale = getDouble(child->GetNodeContent(), childName, -1, "", false);
                screenInfo.borderX[VIDEOFRED] = (int)screenInfo.borderX[VIDEOFRED] / computerConfiguration.fredVideoConfiguration.xScale;
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.fredVideoConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.fredVideoConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
                if (mode_.gui)
                    XRCCTRL(*this,"FredVideoIoGroupText", wxStaticText)->SetLabel(p_Main->getGroupMessageXml(&computerConfiguration.fredVideoConfiguration.ioGroupVector));
            break;

            case TAG_HEIGHT:
                computerConfiguration.cdp1861Configuration.pixieGraphics.videoHeight = (int)parseXml_Number(*child);
            break;

            case TAG_WIDTH:
                computerConfiguration.cdp1861Configuration.pixieGraphics.videoWidth = (int)parseXml_Number(*child);
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

void FredVideoConfig::updateFredVideoPanel()
{
    if (computerConfiguration.fredVideoConfiguration.defined)
    {
        for (size_t registerNumber = 0; registerNumber<FRED_VIDEO_NUMBER_OF_RADIOBUTTONS; registerNumber++)
        {
            if (fredConfigRegisterValue[registerNumber] != lastFredConfigRegisterValue[registerNumber])
            {
                p_Main->setRadioButton(registerIdFredVideoRadio[registerNumber], fredConfigRegisterValue[registerNumber]);
                lastFredConfigRegisterValue[registerNumber] = fredConfigRegisterValue[registerNumber];
            }
        }
        for (size_t registerNumber = 0; registerNumber<FRED_VIDEO_NUMBER_OF_REGISTERS; registerNumber++)
        {
            if (fredConfigRegisterValueString[registerNumber] != lastFredConfigRegisterValueString[registerNumber])
            {
                p_Main->setTextCtrl(registerIdFredVideo[registerNumber], fredConfigRegisterValueString[registerNumber]);
                lastFredConfigRegisterValueString[registerNumber] = fredConfigRegisterValueString[registerNumber];
            }
        }
    }
}

int FredVideoConfig::setFredVideoRegister(int registerNumber, bool value, int showTrace)
{
    fredConfigRegisterValue[registerNumber] = value;

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdFredVideoRadio[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showVideoTraceText(registerFunctionFredVideo[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
      }
      return showTrace;
}

int FredVideoConfig::setFredVideoRegisterNible(int registerNumber, Byte value, int showTrace)
{
    fredConfigRegisterValueString[registerNumber].Printf("%01X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdFredVideo[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showVideoTraceText(registerFunctionFredVideo[registerNumber], fredConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
      }
      return showTrace;
}

void FredVideoConfig::FredVideoEnable(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    fredVideoPointer->enableScreen(true, DO_NOT_SHOW_ADDRESS_TRACE);
}

void FredVideoConfig::FredVideoDisable(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    fredVideoPointer->enableScreen(false, DO_NOT_SHOW_ADDRESS_TRACE);
}

void FredVideoConfig::FredVideoType(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    long value = get8BitValue("FredVideoType");
    if (value == -1)  return;

	fredVideoPointer->enableScreen(value != 0, DO_NOT_SHOW_ADDRESS_TRACE);
    fredVideoPointer->setDisplayType((int)value, DO_NOT_SHOW_ADDRESS_TRACE);
}
