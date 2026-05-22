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
#include "pixie_config.h"
#include "wx/tglbtn.h"
#include "wx/xrc/xmlres.h"

#define THIS_PANEL_NAME "PanelVideoPixie"

BEGIN_EVENT_TABLE(PixieConfig, CoinVideoConfig)
    EVT_RADIOBUTTON(XRCID("PixieDisableGraphics"), PixieConfig::PixieDisable)
    EVT_RADIOBUTTON(XRCID("PixieEnableGraphics"), PixieConfig::PixieEnable)
END_EVENT_TABLE()

wxString registerIdPixie[] =
{
    "PixieDisableGraphics",
    "PixieEnableGraphics",
};

wxString registerFunctionPixie[] =
{
    "Disable graphics",
    "Enable graphics",
};

PixieConfig::PixieConfig(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: CoinVideoConfig(title, pos, size, mode, dataDir, iniDir)
{
}

PixieConfig::~PixieConfig()
{
}

void PixieConfig::pixieConfigInit()
{
    for (size_t registerNumber = 0; registerNumber<PIXIE_NUMBER_OF_REGISTERS; registerNumber++)
    {
        pixieConfigRegisterValue[registerNumber] = false;
        lastPixieConfigRegisterValue[registerNumber] = true;
    }

    computerConfiguration.cdp1861Configuration.ioGroupVector.clear();
    computerConfiguration.cdp1861Configuration.input = init_IoPort();
    computerConfiguration.cdp1861Configuration.output = init_IoPort();
    computerConfiguration.cdp1861Configuration.ef = init_EfFlag();

    computerConfiguration.cdp1861Configuration.statusBarType = STATUSBAR_NONE;
    computerConfiguration.cdp1861Configuration.defined = false;
    computerConfiguration.cdp1861Configuration.efScreenOn = false;
    computerConfiguration.cdp1861Configuration.highRes = false;
    computerConfiguration.cdp1861Configuration.colorType = PIXIE_COLOR_DEFAULT;
    computerConfiguration.cdp1861Configuration.doubleScreenIo = PIXIE_IN_OUT;
    computerConfiguration.cdp1861Configuration.xScale = 3;

    computerConfiguration.cdp1861Configuration.defaultX = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    computerConfiguration.cdp1861Configuration.defaultY = mainWindowY_;

    if (!mode_.gui)
        return;

    XRCCTRL(*this, THIS_PANEL_NAME, wxPanel)->Hide();

    disableIoPortConfigRadio(registerIdPixie[PIXIE_ENABLE]);
    pixieConfigRegisterValue[PIXIE_ENABLE] = false;
    disableIoPortConfigRadio(registerIdPixie[PIXIE_DISABLE]);

    XRCCTRL(*this, "PixieIoGroupText", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "PixieEfDisplayText", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "PixieEfDisplayScreenOn", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "PixieHighRes", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "PixieVisicomColor", wxStaticText)->SetLabel("");
}

void PixieConfig::parseXml_PixieVideo(wxXmlNode &node)
{
    videoConfigUsed(THIS_PANEL_NAME);

    if (!computerConfiguration.cdp1861Configuration.defined)
    {
        computerConfiguration.cdp1861Configuration.videoNumber = computerConfiguration.numberOfVideoTypes_;
        computerConfiguration.numberOfVideoTypes_++;
    }

    computerConfiguration.cdp1861Configuration.defined = true;
    computerConfiguration.zoom_[computerConfiguration.cdp1861Configuration.videoNumber] = "2.00";
    computerConfiguration.videoName_[computerConfiguration.cdp1861Configuration.videoNumber] = "1861 Pixie";

    wxString tagList[]=
    {
        "io",
        "out",
        "in",
        "ef",
        "iogroup",
        "zoom",
        "border",
        "pos",
        "xscale",
        "color",
        "colortype",
        "graphics",
        "highres",
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
        TAG_IOGROUP,
        TAG_ZOOM,
        TAG_BORDER,
        TAG_POS,
        TAG_XSCALE,
        TAG_COLOR,
        TAG_COLOR_TYPE,
        TAG_GRAPHICS,
        TAG_HIGH_RES,
        TAG_HEIGHT,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    long width, height;
    int red, green, blue, xpos, ypos;
    wxString color, position, iogroup;
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
            case TAG_IO:
                if (child->HasAttribute("type"))
                {
                    if (child->GetAttribute("type") == "on")
                    {
                        computerConfiguration.cdp1861Configuration.input = parseXml_IoPort(*child, CDP1861_ENABLE_IN);
                        setIoPortConfigRadio(computerConfiguration.cdp1861Configuration.input, registerIdPixie[PIXIE_ENABLE], registerFunctionPixie[PIXIE_ENABLE], "B");
                    }
                    if (child->GetAttribute("type") == "off")
                    {
                        computerConfiguration.cdp1861Configuration.output = parseXml_IoPort(*child, CDP1861_DISABLE_OUT);
                        setIoPortConfigRadio(computerConfiguration.cdp1861Configuration.output, registerIdPixie[PIXIE_DISABLE], registerFunctionPixie[PIXIE_DISABLE], "B");
                    }
                }
                else
                {
                    computerConfiguration.cdp1861Configuration.input = parseXml_IoPort(*child, CDP1861_ENABLE_IN);
                    setIoPortConfigRadio(computerConfiguration.cdp1861Configuration.input, registerIdPixie[PIXIE_ENABLE], registerFunctionPixie[PIXIE_ENABLE], "B");
                }
                computerConfiguration.cdp1861Configuration.doubleScreenIo = PIXIE_DOUBLE;
            break;
                
            case TAG_IN:
                if (child->HasAttribute("type"))
                {
                    computerConfiguration.cdp1861Configuration.doubleScreenIo = PIXIE_IN_IN;
                    if (child->GetAttribute("type") == "on")
                    {
                        computerConfiguration.cdp1861Configuration.input = parseXml_IoPort(*child, CDP1861_ENABLE_IN);
                        setIoPortConfigRadio(computerConfiguration.cdp1861Configuration.input, registerIdPixie[PIXIE_ENABLE], registerFunctionPixie[PIXIE_ENABLE], "I");
                    }
                    if (child->GetAttribute("type") == "off")
                    {
                        computerConfiguration.cdp1861Configuration.output = parseXml_IoPort(*child, CDP1861_DISABLE_OUT);
                        setIoPortConfigRadio(computerConfiguration.cdp1861Configuration.output, registerIdPixie[PIXIE_DISABLE], registerFunctionPixie[PIXIE_DISABLE], "I");
                    }
                }
                else
                {
                    computerConfiguration.cdp1861Configuration.input = parseXml_IoPort(*child, CDP1861_ENABLE_IN);
                    setIoPortConfigRadio(computerConfiguration.cdp1861Configuration.input, registerIdPixie[PIXIE_ENABLE], registerFunctionPixie[PIXIE_ENABLE], "I");
                }
            break;
                
            case TAG_OUT:
                if (child->HasAttribute("type"))
                {
                    computerConfiguration.cdp1861Configuration.doubleScreenIo = PIXIE_OUT_OUT;
                    if (child->GetAttribute("type") == "on")
                    {
                        computerConfiguration.cdp1861Configuration.input = parseXml_IoPort(*child, CDP1861_ENABLE_IN);
                        setIoPortConfigRadio(computerConfiguration.cdp1861Configuration.input, registerIdPixie[PIXIE_ENABLE], registerFunctionPixie[PIXIE_ENABLE], "O");
                    }
                    if (child->GetAttribute("type") == "off")
                    {
                        computerConfiguration.cdp1861Configuration.output = parseXml_IoPort(*child, CDP1861_DISABLE_OUT);
                        setIoPortConfigRadio(computerConfiguration.cdp1861Configuration.output, registerIdPixie[PIXIE_DISABLE], registerFunctionPixie[PIXIE_DISABLE], "O");
                    }
                }
                else
                {
                    computerConfiguration.cdp1861Configuration.output = parseXml_IoPort(*child, CDP1861_DISABLE_OUT);
                    setIoPortConfigRadio(computerConfiguration.cdp1861Configuration.output, registerIdPixie[PIXIE_DISABLE], registerFunctionPixie[PIXIE_DISABLE], "O");
                }
            break;
                
            case TAG_EF:
                if (child->GetAttribute("type") == "on")
                {
                    computerConfiguration.cdp1861Configuration.efScreenOn = true;
                    if (mode_.gui)
                        XRCCTRL(*this, "PixieEfDisplayScreenOn", wxStaticText)->SetLabel("Screen off: no frame indicator");
                }
                computerConfiguration.cdp1861Configuration.ef = parseXml_EfFlag(*child, CDP1861_IN_FRAME_EF);
                setEfFlagConfig(computerConfiguration.cdp1861Configuration.ef, "in frame indicator", "PixieEfDisplayText");
            break;
                
            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.cdp1861Configuration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.cdp1861Configuration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
                if (mode_.gui)
                    XRCCTRL(*this,"PixieIoGroupText", wxStaticText)->SetLabel(p_Main->getGroupMessageXml(&computerConfiguration.cdp1861Configuration.ioGroupVector));
            break;

            case TAG_ZOOM:
                computerConfiguration.zoom_[computerConfiguration.cdp1861Configuration.videoNumber] = child->GetNodeContent();
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
                    screenInfo.borderX[VIDEOXMLPIXIE] = (int)width/computerConfiguration.cdp1861Configuration.xScale;
                    screenInfo.borderY[VIDEOXMLPIXIE] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += computerConfiguration.cdp1861Configuration.defaultX;
                if (ypos != -1)
                    ypos += computerConfiguration.cdp1861Configuration.defaultY;
                computerConfiguration.cdp1861Configuration.defaultX = xpos;
                computerConfiguration.cdp1861Configuration.defaultY = ypos;
            break;

            case TAG_XSCALE:
                screenInfo.borderX[VIDEOXMLPIXIE] = (int)screenInfo.borderX[VIDEOXMLPIXIE] * computerConfiguration.cdp1861Configuration.xScale;
                computerConfiguration.cdp1861Configuration.xScale = getDouble(child->GetNodeContent(), childName, -1, "", false);
                screenInfo.borderX[VIDEOXMLPIXIE] = (int)screenInfo.borderX[VIDEOXMLPIXIE] / computerConfiguration.cdp1861Configuration.xScale;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "fore")
                    screenInfo.defaultColour[COL_PIXIE_FORE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "fore1")
                    screenInfo.defaultColour[COL_PIXIE_VISICOM_COL1].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "fore2")
                    screenInfo.defaultColour[COL_PIXIE_VISICOM_COL2].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "fore3")
                    screenInfo.defaultColour[COL_PIXIE_VISICOM_COL3].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back")
                    screenInfo.defaultColour[COL_PIXIE_BACK].Printf("#%02X%02X%02X", red, green, blue);
            break;
                
            case TAG_COLOR_TYPE:
                if (child->GetNodeContent() == "vis")
                {
                    computerConfiguration.cdp1861Configuration.colorType = PIXIE_COLOR_VISICOM;
                    if (mode_.gui)
                        XRCCTRL(*this, "PixieVisicomColor", wxStaticText)->SetLabel("Visicom COM-100 color");
                }
            break;

            case TAG_GRAPHICS:
                parseXml_pixieGraphics (*child);
            break;

            case TAG_HIGH_RES:
                computerConfiguration.cdp1861Configuration.highRes = true;
                if (mode_.gui)
                    XRCCTRL(*this, "PixieHighRes", wxStaticText)->SetLabel("High resolution mode");
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

void PixieConfig::updatePixiePanel()
{
    if (computerConfiguration.cdp1861Configuration.defined)
    {
        for (size_t registerNumber = 0; registerNumber<PIXIE_NUMBER_OF_REGISTERS; registerNumber++)
        {
            if (pixieConfigRegisterValue[registerNumber] != lastPixieConfigRegisterValue[registerNumber])
            {
                p_Main->setRadioButton(registerIdPixie[registerNumber], pixieConfigRegisterValue[registerNumber]);
                lastPixieConfigRegisterValue[registerNumber] = pixieConfigRegisterValue[registerNumber];
            }
        }
    }
}

int PixieConfig::setPixieRegister(int registerNumber, bool value, int showTrace)
{
    pixieConfigRegisterValue[registerNumber] = value;

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdPixie[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showVideoTraceText(registerFunctionPixie[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
      }
      return showTrace;
}

void PixieConfig::PixieEnable(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    pixiePointer->inPixie();
}

void PixieConfig::PixieDisable(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    pixiePointer->outPixie();
}
