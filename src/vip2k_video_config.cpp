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
#include "vip2k_video_config.h"
#include "wx/tglbtn.h"
#include "wx/xrc/xmlres.h"

#define THIS_PANEL_NAME "PanelVideoVip2KVideo"

BEGIN_EVENT_TABLE(Vip2KVideoConfig, St4VideoConfig)
    EVT_RADIOBUTTON(XRCID("Vip2KVideoDisableGraphics"), Vip2KVideoConfig::Vip2KVideoDisable)
    EVT_RADIOBUTTON(XRCID("Vip2KVideoEnableGraphics"), Vip2KVideoConfig::Vip2KVideoEnable)
END_EVENT_TABLE()

wxString registerIdVip2KVideo[] =
{
    "Vip2KVideoDisableGraphics",
    "Vip2KVideoEnableGraphics",
};

wxString registerFunctionVip2KVideo[] =
{
    "Disable graphics",
    "Enable graphics",
};

Vip2KVideoConfig::Vip2KVideoConfig(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: St4VideoConfig(title, pos, size, mode, dataDir, iniDir)
{
}

Vip2KVideoConfig::~Vip2KVideoConfig()
{
}

void Vip2KVideoConfig::vip2KVideoConfigInit()
{
    for (size_t registerNumber = 0; registerNumber<PIXIE_NUMBER_OF_REGISTERS; registerNumber++)
    {
        vip2KConfigRegisterValue[registerNumber] = false;
        lastVip2KConfigRegisterValue[registerNumber] = true;
    }

    computerConfiguration.vip2KVideoConfiguration.xScale = 1.5;
    computerConfiguration.vip2KVideoConfiguration.ioGroupVector.clear();
    computerConfiguration.vip2KVideoConfiguration.output = init_IoPort();
    computerConfiguration.vip2KVideoConfiguration.input = init_IoPort();
    computerConfiguration.vip2KVideoConfiguration.ef = init_EfFlag();

    computerConfiguration.vip2KVideoConfiguration.doubleScreenIo = false;
    computerConfiguration.vip2KVideoConfiguration.defined = false;
    computerConfiguration.vip2KVideoConfiguration.sequencerFile = "2716-ntsc.hex";
    computerConfiguration.vip2KVideoConfiguration.sequencerDirectory = computerConfiguration.mainDir_;

    computerConfiguration.vip2KVideoConfiguration.defaultX = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    computerConfiguration.vip2KVideoConfiguration.defaultY = mainWindowY_;

    if (!mode_.gui)
        return;

    XRCCTRL(*this, THIS_PANEL_NAME, wxPanel)->Hide();

    disableIoPortConfigRadio(registerIdVip2KVideo[VIP2K_VIDEO_ENABLE]);
    vip2KConfigRegisterValue[VIP2K_VIDEO_ENABLE] = false;
    disableIoPortConfigRadio(registerIdVip2KVideo[VIP2K_VIDEO_DISABLE]);

    XRCCTRL(*this, "Vip2KVideoIoGroupText", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "Vip2KVideoEfDisplayText", wxStaticText)->SetLabel("");
}

void Vip2KVideoConfig::parseXml_Vip2KVideo(wxXmlNode &node)
{
    videoConfigUsed(THIS_PANEL_NAME);

    if (!computerConfiguration.vip2KVideoConfiguration.defined)
    {
        computerConfiguration.vip2KVideoConfiguration.videoNumber = computerConfiguration.numberOfVideoTypes_;
        computerConfiguration.numberOfVideoTypes_++;
    }

    computerConfiguration.vip2KVideoConfiguration.defined = true;

    computerConfiguration.cdp1861Configuration.pixieGraphics.videoHeight = 198;

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
        "color",
        "height",
        "width",
        "filename",
        "dirname",
        "iogroup",
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
        TAG_COLOR,
        TAG_HEIGHT,
        TAG_WIDTH,
        TAG_FILENAME,
        TAG_DIRNAME,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    long width, height;
    int red, green, blue, xpos, ypos;
    wxString color, position, iogroup;
    size_t ioGroupNumber = 0;
    computerConfiguration.zoom_[computerConfiguration.vip2KVideoConfiguration.videoNumber] = "2.00";
    computerConfiguration.videoName_[computerConfiguration.vip2KVideoConfiguration.videoNumber] = "VIP2K";

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_IO:
                if (child->HasAttribute("type"))
                {
                    if (child->GetAttribute("type") == "on")
					{
                        computerConfiguration.vip2KVideoConfiguration.input = parseXml_IoPort(*child, VIP2K_VIDEO_ENABLE_IN);
                        setIoPortConfigRadio(computerConfiguration.vip2KVideoConfiguration.input, registerIdVip2KVideo[VIP2K_VIDEO_ENABLE], registerFunctionVip2KVideo[VIP2K_VIDEO_ENABLE], "B");
                    }
                    if (child->GetAttribute("type") == "off")
					{
                        computerConfiguration.vip2KVideoConfiguration.output = parseXml_IoPort(*child, VIP2K_VIDEO_DISABLE_OUT);
                        setIoPortConfigRadio(computerConfiguration.vip2KVideoConfiguration.output, registerIdVip2KVideo[VIP2K_VIDEO_DISABLE], registerFunctionVip2KVideo[VIP2K_VIDEO_DISABLE], "B");
                    }
                }
                else
				{
                    computerConfiguration.vip2KVideoConfiguration.input = parseXml_IoPort(*child, VIP2K_VIDEO_ENABLE_IN);
                    setIoPortConfigRadio(computerConfiguration.vip2KVideoConfiguration.input, registerIdVip2KVideo[VIP2K_VIDEO_ENABLE], registerFunctionVip2KVideo[VIP2K_VIDEO_ENABLE], "B");
                }                computerConfiguration.vip2KVideoConfiguration.doubleScreenIo = true;
            break;
                
            case TAG_IN:
                if (child->HasAttribute("type"))
                {
                    if (child->GetAttribute("type") == "off")
					{
                        computerConfiguration.vip2KVideoConfiguration.output = parseXml_IoPort(*child, VIP2K_VIDEO_DISABLE_OUT);
                        setIoPortConfigRadio(computerConfiguration.vip2KVideoConfiguration.output, registerIdVip2KVideo[VIP2K_VIDEO_DISABLE], registerFunctionVip2KVideo[VIP2K_VIDEO_DISABLE], "I");
                    }
                    if (child->GetAttribute("type") == "on")
					{
                        computerConfiguration.vip2KVideoConfiguration.input = parseXml_IoPort(*child, VIP2K_VIDEO_ENABLE_IN);
                        setIoPortConfigRadio(computerConfiguration.vip2KVideoConfiguration.input, registerIdVip2KVideo[VIP2K_VIDEO_ENABLE], registerFunctionVip2KVideo[VIP2K_VIDEO_ENABLE], "I");
                    }
                }
                else
				{
                    computerConfiguration.vip2KVideoConfiguration.input = parseXml_IoPort(*child, VIP2K_VIDEO_ENABLE_IN);
                    setIoPortConfigRadio(computerConfiguration.vip2KVideoConfiguration.input, registerIdVip2KVideo[VIP2K_VIDEO_ENABLE], registerFunctionVip2KVideo[VIP2K_VIDEO_ENABLE], "I");
                }
            break;
                
            case TAG_OUT:
                if (child->HasAttribute("type"))
                {
                    if (child->GetAttribute("type") == "off")
					{
                        computerConfiguration.vip2KVideoConfiguration.output = parseXml_IoPort(*child, VIP2K_VIDEO_DISABLE_OUT);
                        setIoPortConfigRadio(computerConfiguration.vip2KVideoConfiguration.output, registerIdVip2KVideo[VIP2K_VIDEO_DISABLE], registerFunctionVip2KVideo[VIP2K_VIDEO_DISABLE], "O");
                    }
                    if (child->GetAttribute("type") == "on")
					{
                        computerConfiguration.vip2KVideoConfiguration.input = parseXml_IoPort(*child, VIP2K_VIDEO_ENABLE_IN);
                        setIoPortConfigRadio(computerConfiguration.vip2KVideoConfiguration.input, registerIdVip2KVideo[VIP2K_VIDEO_ENABLE], registerFunctionVip2KVideo[VIP2K_VIDEO_ENABLE], "O");
                    }
                }
                else
				{
                    computerConfiguration.vip2KVideoConfiguration.output = parseXml_IoPort(*child, VIP2K_VIDEO_DISABLE_OUT);
                    setIoPortConfigRadio(computerConfiguration.vip2KVideoConfiguration.output, registerIdVip2KVideo[VIP2K_VIDEO_DISABLE], registerFunctionVip2KVideo[VIP2K_VIDEO_DISABLE], "O");
                }
            break;
                
            case TAG_EF:
                computerConfiguration.vip2KVideoConfiguration.ef = parseXml_EfFlag(*child);
                setEfFlagConfig(computerConfiguration.vip2KVideoConfiguration.ef, "in frame indicator", "Vip2KVideoEfDisplayText");
            break;
                
            case TAG_ZOOM:
                computerConfiguration.zoom_[computerConfiguration.vip2KVideoConfiguration.videoNumber] = child->GetNodeContent();
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
                    screenInfo.borderX[VIDEOVIP2K] = (int)width/computerConfiguration.vip2KVideoConfiguration.xScale;
                    screenInfo.borderY[VIDEOVIP2K] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += computerConfiguration.vip2KVideoConfiguration.defaultX;
                if (ypos != -1)
                    ypos += computerConfiguration.vip2KVideoConfiguration.defaultY;
                computerConfiguration.vip2KVideoConfiguration.defaultX = xpos;
                computerConfiguration.vip2KVideoConfiguration.defaultY = ypos;
            break;

            case TAG_XSCALE: 
                screenInfo.borderX[VIDEOVIP2K] = (int)screenInfo.borderX[VIDEOVIP2K] * computerConfiguration.vip2KVideoConfiguration.xScale;
                computerConfiguration.vip2KVideoConfiguration.xScale = getDouble(child->GetNodeContent(), childName, -1, "", false);
                screenInfo.borderX[VIDEOVIP2K] = (int)screenInfo.borderX[VIDEOVIP2K] / computerConfiguration.vip2KVideoConfiguration.xScale;
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
                
            case TAG_HEIGHT:
                computerConfiguration.cdp1861Configuration.pixieGraphics.videoHeight = (int)parseXml_Number(*child);
            break;

            case TAG_WIDTH:
                computerConfiguration.cdp1861Configuration.pixieGraphics.videoWidth = (int)parseXml_Number(*child);
            break;

            case TAG_FILENAME:
                computerConfiguration.vip2KVideoConfiguration.sequencerFile = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                computerConfiguration.vip2KVideoConfiguration.sequencerDirectory = dataDir_ + child->GetNodeContent();
                if (computerConfiguration.vip2KVideoConfiguration.sequencerDirectory.Right(1) != pathSeparator_)
                    computerConfiguration.vip2KVideoConfiguration.sequencerDirectory += pathSeparator_;
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.vip2KVideoConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.vip2KVideoConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
                if (mode_.gui)
                    XRCCTRL(*this,"Vip2KVideoIoGroupText", wxStaticText)->SetLabel(p_Main->getGroupMessageXml(&computerConfiguration.vip2KVideoConfiguration.ioGroupVector));
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

void Vip2KVideoConfig::updateVip2KVideoPanel()
{
    if (computerConfiguration.vip2KVideoConfiguration.defined)
    {
        for (size_t registerNumber = 0; registerNumber<PIXIE_NUMBER_OF_REGISTERS; registerNumber++)
        {
            if (vip2KConfigRegisterValue[registerNumber] != lastVip2KConfigRegisterValue[registerNumber])
            {
                p_Main->setRadioButton(registerIdVip2KVideo[registerNumber], vip2KConfigRegisterValue[registerNumber]);
                lastVip2KConfigRegisterValue[registerNumber] = vip2KConfigRegisterValue[registerNumber];
            }
        }
    }
}

int Vip2KVideoConfig::setVip2KVideoRegister(int registerNumber, bool value, int showTrace)
{
    vip2KConfigRegisterValue[registerNumber] = value;

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdVip2KVideo[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showVideoTraceText(registerFunctionVip2KVideo[registerNumber], showTrace);
        
        if (showTrace == SHOW_ADDRESS_TRACE)
            return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

void Vip2KVideoConfig::Vip2KVideoEnable(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    vip2KVideoPointer->inPixie();
}

void Vip2KVideoConfig::Vip2KVideoDisable(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    vip2KVideoPointer->outPixie();
}
