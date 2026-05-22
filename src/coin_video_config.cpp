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
#include "coin_video_config.h"
#include "wx/tglbtn.h"
#include "wx/xrc/xmlres.h"

#define THIS_PANEL_NAME "PanelVideoCoinVideo"

BEGIN_EVENT_TABLE(CoinVideoConfig, Sn76430NConfig)
    EVT_RADIOBUTTON(XRCID("CoinVideoEnableGraphics"), CoinVideoConfig::CoinVideoEnable)
END_EVENT_TABLE()

wxString registerIdCoinVideo[] =
{
    "CoinVideoEnableGraphics",
};

wxString registerFunctionCoinVideo[] =
{
    "Enable graphics",
};

CoinVideoConfig::CoinVideoConfig(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: Sn76430NConfig(title, pos, size, mode, dataDir, iniDir)
{
}

CoinVideoConfig::~CoinVideoConfig()
{
}

void CoinVideoConfig::coinVideoConfigInit()
{
    for (size_t registerNumber = 0; registerNumber<COIN_VIDEO_NUMBER_OF_REGISTERS; registerNumber++)
    {
        coinConfigRegisterValue[registerNumber] = false;
        lastCoinConfigRegisterValue[registerNumber] = true;
    }
    
    computerConfiguration.coinConfiguration.xScale = 3;
    computerConfiguration.coinConfiguration.ioGroupVector.clear();
	computerConfiguration.coinConfiguration.output = init_IoPort();
    
    computerConfiguration.coinConfiguration.defined = false;

    computerConfiguration.coinConfiguration.defaultX = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    computerConfiguration.coinConfiguration.defaultY = mainWindowY_;

    if (!mode_.gui)
        return;

    XRCCTRL(*this, THIS_PANEL_NAME, wxPanel)->Hide();

    disableIoPortConfigRadio(registerIdCoinVideo[COIN_VIDEO_ENABLE]);
    coinConfigRegisterValue[COIN_VIDEO_ENABLE] = false;
    
    XRCCTRL(*this, "CoinVideoIoGroupText", wxStaticText)->SetLabel("");
}

void CoinVideoConfig::parseXml_CoinVideo(wxXmlNode &node)
{
    videoConfigUsed(THIS_PANEL_NAME);

    if (!computerConfiguration.coinConfiguration.defined)
    {
        computerConfiguration.coinConfiguration.videoNumber = computerConfiguration.numberOfVideoTypes_;
        computerConfiguration.numberOfVideoTypes_++;
    }

    computerConfiguration.coinConfiguration.defined = true;
    
    computerConfiguration.cdp1861Configuration.pixieGraphics.interrupt = 62;
    computerConfiguration.cdp1861Configuration.pixieGraphics.start = 72;
    computerConfiguration.cdp1861Configuration.pixieGraphics.end = 199;
    computerConfiguration.cdp1861Configuration.pixieGraphics.screenend = 322;
    computerConfiguration.cdp1861Configuration.pixieGraphics.videoHeight = 128;

    wxString tagList[]=
    {
        "out",
        "zoom",
        "border",
        "pos",
        "xscale",
        "color",
        "graphics",
        "iogroup",
        "height",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_ZOOM,
        TAG_BORDER,
        TAG_POS,
        TAG_XSCALE,
        TAG_COLOR,
        TAG_GRAPHICS,
        TAG_IOGROUP,
        TAG_HEIGHT,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    long width, height;
    int red, green, blue, xpos, ypos;
    wxString color, position, iogroup;
    size_t ioGroupNumber = 0;
	computerConfiguration.zoom_[computerConfiguration.coinConfiguration.videoNumber] = "2.00";
    computerConfiguration.videoName_[computerConfiguration.coinConfiguration.videoNumber] = "Coin Video";

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
                computerConfiguration.coinConfiguration.output = parseXml_IoPort(*child, COIN_VIDEO_ENABLE_OUT);
				setIoPortConfigRadio(computerConfiguration.coinConfiguration.output, registerIdCoinVideo[COIN_VIDEO_ENABLE], registerFunctionCoinVideo[COIN_VIDEO_ENABLE], "O");
            break;
                                
            case TAG_ZOOM:
                computerConfiguration.zoom_[computerConfiguration.coinConfiguration.videoNumber] = child->GetNodeContent();
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
                    screenInfo.borderX[VIDEOCOIN] = (int)width/computerConfiguration.coinConfiguration.xScale;
                    screenInfo.borderY[VIDEOCOIN] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += computerConfiguration.coinConfiguration.defaultX;
                if (ypos != -1)
                    ypos += computerConfiguration.coinConfiguration.defaultY;
                computerConfiguration.coinConfiguration.defaultX = xpos;
                computerConfiguration.coinConfiguration.defaultY = ypos;
            break;

            case TAG_XSCALE:
                screenInfo.borderX[VIDEOCOIN] = (int)screenInfo.borderX[VIDEOCOIN] * computerConfiguration.coinConfiguration.xScale;
                computerConfiguration.coinConfiguration.xScale = getDouble(child->GetNodeContent(), childName, -1, "", false);
                screenInfo.borderX[VIDEOCOIN] = (int)screenInfo.borderX[VIDEOCOIN] / computerConfiguration.coinConfiguration.xScale;
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
                
            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.coinConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.coinConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
                if (mode_.gui)
                    XRCCTRL(*this,"CoinVideoIoGroupText", wxStaticText)->SetLabel(p_Main->getGroupMessageXml(&computerConfiguration.coinConfiguration.ioGroupVector));
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

void CoinVideoConfig::updateCoinVideoPanel()
{
    if (computerConfiguration.coinConfiguration.defined)
    {
        for (size_t registerNumber = 0; registerNumber<COIN_VIDEO_NUMBER_OF_REGISTERS; registerNumber++)
        {
            if (coinConfigRegisterValue[registerNumber] != lastCoinConfigRegisterValue[registerNumber])
            {
                p_Main->setRadioButton(registerIdCoinVideo[registerNumber], coinConfigRegisterValue[registerNumber]);
                lastCoinConfigRegisterValue[registerNumber] = coinConfigRegisterValue[registerNumber];
            }
        }
    }
}

int CoinVideoConfig::setCoinVideoRegister(int registerNumber, bool value, int showTrace)
{
    coinConfigRegisterValue[registerNumber] = value;

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdCoinVideo[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showVideoTraceText(registerFunctionCoinVideo[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
      }
      return showTrace;
}

void CoinVideoConfig::CoinVideoEnable(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }
    coinPointer->inPixie();
}
