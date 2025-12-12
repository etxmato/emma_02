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
#include "mc6847_config.h"
#include "wx/tglbtn.h"
#include "wx/xrc/xmlres.h"

#define THIS_PANEL_NAME "PanelVideoMc6847"

BEGIN_EVENT_TABLE(Mc6847Config, PixieConfig)
    EVT_TEXT_ENTER(XRCID("Mc6847VideoMode"), Mc6847Config::Mc6847VideoMode)
    EVT_TEXT_ENTER(XRCID("Mc6847Inv"), Mc6845Config::Mc6847Inv)
    EVT_TEXT_ENTER(XRCID("Mc6847Ext"), Mc6845Config::Mc6847Ext)
    EVT_TEXT_ENTER(XRCID("Mc6847Css"), Mc6845Config::Mc6847Css)
    EVT_TEXT_ENTER(XRCID("Mc6847As"), Mc6845Config::Mc6847As)
    EVT_TEXT_ENTER(XRCID("Mc6847Ag"), Mc6845Config::Mc6847Ag)
    EVT_TEXT_ENTER(XRCID("Mc6847Gm"), Mc6845Config::Mc6847Gm)
END_EVENT_TABLE()

wxString registerIdMc6847[] =
{
    "Mc6847VideoMode",
    "Mc6847Inv",
    "Mc6847Ext",
    "Mc6847Css",
    "Mc6847As",
    "Mc6847Ag",
    "Mc6847Gm",
};

wxString registerFunctionMc6847[] =
{
    "Video Mode",
    "INV",
    "EXT",
    "CSS",
    "A/S",
    "A/G",
    "GM",
};

Mc6847Config::Mc6847Config(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: PixieConfig(title, pos, size, mode, dataDir, iniDir)
{
}

Mc6847Config::~Mc6847Config()
{
}

void Mc6847Config::mc6847ConfigInit()
{
    for (size_t registerNumber = 0; registerNumber<MC6847_NUMBER_OF_REGISTERS; registerNumber++)
    {
        mc6847ConfigRegisterValueString[registerNumber] = "";
        lastMc6847ConfigRegisterValueString[registerNumber] = "x";
    }

    computerConfiguration.mc6847Configuration.ioGroupVector.clear();
    computerConfiguration.mc6847Configuration.b7 = 0;
    computerConfiguration.mc6847Configuration.b6 = 0;
    computerConfiguration.mc6847Configuration.b5 = 0;
    computerConfiguration.mc6847Configuration.b4 = 0;
    computerConfiguration.mc6847Configuration.b3 = 0;
    computerConfiguration.mc6847Configuration.b2 = 0;
    computerConfiguration.mc6847Configuration.b1 = 0;
    computerConfiguration.mc6847Configuration.b0 = 0;
    computerConfiguration.mc6847Configuration.dd7 = 0;
    computerConfiguration.mc6847Configuration.dd6 = 0;

    computerConfiguration.mc6847Configuration.forceHighAg = false;
    computerConfiguration.mc6847Configuration.forceHighAs = false;
    computerConfiguration.mc6847Configuration.forceHighExt = false;
    computerConfiguration.mc6847Configuration.forceHighGm2 = false;
    computerConfiguration.mc6847Configuration.forceHighGm1 = false;
    computerConfiguration.mc6847Configuration.forceHighGm0 = false;
    computerConfiguration.mc6847Configuration.forceHighCss = false;
    computerConfiguration.mc6847Configuration.forceHighInv = false;

    computerConfiguration.mc6847Configuration.inv = -1;
    computerConfiguration.mc6847Configuration.ext = -1;
    computerConfiguration.mc6847Configuration.css = -1;
    computerConfiguration.mc6847Configuration.as = -1;
    computerConfiguration.mc6847Configuration.ag = -1;
    computerConfiguration.mc6847Configuration.gm0 = -1;
    computerConfiguration.mc6847Configuration.gm1 = -1;
    computerConfiguration.mc6847Configuration.gm2 = -1;

    computerConfiguration.mc6847Configuration.screenHeight = 192;
    computerConfiguration.mc6847Configuration.defined = false;

    computerConfiguration.mc6847Configuration.defaultX = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    computerConfiguration.mc6847Configuration.defaultY = mainWindowY_;

    XRCCTRL(*this, THIS_PANEL_NAME, wxPanel)->Hide();

    disableIoPortConfig(registerIdMc6847[MC6847_VIDEO_INV]);
    mc6847ConfigRegisterValueString[MC6847_VIDEO_INV] = "";
    disableIoPortConfig(registerIdMc6847[MC6847_VIDEO_EXT]);
    mc6847ConfigRegisterValueString[MC6847_VIDEO_EXT] = "";
    disableIoPortConfig(registerIdMc6847[MC6847_VIDEO_CSS]);
    mc6847ConfigRegisterValueString[MC6847_VIDEO_CSS] = "";
    disableIoPortConfig(registerIdMc6847[MC6847_VIDEO_AS]);
    mc6847ConfigRegisterValueString[MC6847_VIDEO_AS] = "";
    disableIoPortConfig(registerIdMc6847[MC6847_VIDEO_AG]);
    mc6847ConfigRegisterValueString[MC6847_VIDEO_AG] = "";
    disableIoPortConfig(registerIdMc6847[MC6847_VIDEO_GM]);
    mc6847ConfigRegisterValueString[MC6847_VIDEO_GM] = "";

    XRCCTRL(*this, "Mc6847IoGroupText", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "Mc6847VideoRam", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "Mc6847VideoModeRange", wxStaticText)->SetLabel("");
}

void Mc6847Config::parseXml_MC6847Video(wxXmlNode &node)
{
    videoConfigUsed(THIS_PANEL_NAME);

    long start, end;
    long width, height;

    if (!computerConfiguration.mc6847Configuration.defined)
    {
        computerConfiguration.mc6847Configuration.videoNumber = computerConfiguration.numberOfVideoTypes_;
        computerConfiguration.numberOfVideoTypes_++;
    }
    
    computerConfiguration.mc6847Configuration.defined = true;

    wxString tagList[]=
    {
        "ram",
        "out",
        "font",
        "dirname",
        "inv",
        "ext",
        "css",
        "as",
        "ag",
        "gm0",
        "gm1",
        "gm2",
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
        TAG_RAM,
        TAG_OUT,
        TAG_FONT,
        TAG_DIRNAME,
        TAG_INV,
        TAG_EXT,
        TAG_CSS,
        TAG_AS,
        TAG_AG,
        TAG_GM0,
        TAG_GM1,
        TAG_GM2,
        TAG_ZOOM,
        TAG_BORDER,
        TAG_POS,
        TAG_COLOR,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    int red, green, blue, xpos, ypos;
    wxString color, position, iogroup, label;
    size_t ioGroupNumber = 0;

    computerConfiguration.zoom_[computerConfiguration.mc6847Configuration.videoNumber] = "2.00";
    computerConfiguration.videoName_[computerConfiguration.mc6847Configuration.videoNumber] = "MC6847";

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
                    warningText_ += "No mc6847 RAM range defined";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    computerConfiguration.mc6847Configuration.startRam = (int)start;
                    computerConfiguration.mc6847Configuration.endRam = (int)end;
                    label.Printf("%04X-%04X: Video RAM", (Word)start, (Word)end);
                    XRCCTRL(*this, "Mc6847VideoRam", wxStaticText)->SetLabel(label);
                }
            break;
                
            case TAG_OUT:
                if (!parseXml_Range(*child, &start, &end))
                {
                    computerConfiguration.mc6847Configuration.outputMode = 0;
                    computerConfiguration.mc6847Configuration.output = parseXml_IoPort(*child, MC6847_OUT);
                    setIoPortConfig(computerConfiguration.mc6847Configuration.output, registerIdMc6847[MC6847_VIDEO_MODE], registerFunctionMc6847[MC6847_VIDEO_MODE], "O");
                }
                else
                {
                    computerConfiguration.mc6847Configuration.outputMode = 1;
                    computerConfiguration.mc6847Configuration.outputStart = (Word) start;
                    computerConfiguration.mc6847Configuration.outputEnd = (Word) end;
                    setIoPortConfig(registerIdMc6847[MC6847_VIDEO_MODE], registerFunctionMc6847[MC6847_VIDEO_MODE], "A");
                    if (start == end)
                        label.Printf("%04X: Video Mode", (Word)start);
                    else
                        label.Printf("%04X-%04X: Video Mode", (Word)start, (Word)end);
                    XRCCTRL(*this, "Mc6847VideoModeRange", wxStaticText)->SetLabel(label);
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

            case TAG_INV:
                if (child->GetAttribute("init") == "high")
                    computerConfiguration.mc6847Configuration.forceHighInv = true;
                computerConfiguration.mc6847Configuration.inv = parseXml_Mc6847BitSetup(*child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_EXT:
                if (child->GetAttribute("init") == "high")
                    computerConfiguration.mc6847Configuration.forceHighExt = true;
                computerConfiguration.mc6847Configuration.ext = parseXml_Mc6847BitSetup(*child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_CSS:
                if (child->GetAttribute("init") == "high")
                    computerConfiguration.mc6847Configuration.forceHighCss = true;
                computerConfiguration.mc6847Configuration.css = parseXml_Mc6847BitSetup(*child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_AS:
                if (child->GetAttribute("init") == "high")
                    computerConfiguration.mc6847Configuration.forceHighAs = true;
                computerConfiguration.mc6847Configuration.as = parseXml_Mc6847BitSetup(*child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_AG:
                if (child->GetAttribute("init") == "high")
                    computerConfiguration.mc6847Configuration.forceHighAg = true;
                computerConfiguration.mc6847Configuration.ag = parseXml_Mc6847BitSetup(*child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_GM0:
                if (child->GetAttribute("init") == "high")
                    computerConfiguration.mc6847Configuration.forceHighGm0 = true;
                computerConfiguration.mc6847Configuration.gm0 = parseXml_Mc6847BitSetup(*child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_GM1:
                if (child->GetAttribute("init") == "high")
                    computerConfiguration.mc6847Configuration.forceHighGm1 = true;
                computerConfiguration.mc6847Configuration.gm1 = parseXml_Mc6847BitSetup(*child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_GM2:
                if (child->GetAttribute("init") == "high")
                    computerConfiguration.mc6847Configuration.forceHighGm2 = true;
                computerConfiguration.mc6847Configuration.gm2 = parseXml_Mc6847BitSetup(*child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_ZOOM:
                computerConfiguration.zoom_[computerConfiguration.mc6847Configuration.videoNumber] = child->GetNodeContent();
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
                    screenInfo.borderX[VIDEOXML6847] = (int)width;
                    screenInfo.borderY[VIDEOXML6847] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += computerConfiguration.mc6847Configuration.defaultX;
                if (ypos != -1)
                    ypos += computerConfiguration.mc6847Configuration.defaultY;
                computerConfiguration.mc6847Configuration.defaultX = xpos;
                computerConfiguration.mc6847Configuration.defaultY = ypos;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "text_back")
                    screenInfo.defaultColour[COL_MC6847_TEXT_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "text_green")
                    screenInfo.defaultColour[COL_MC6847_TEXT_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "text_orange")
                    screenInfo.defaultColour[COL_MC6847_TEXT_ORANGE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "graph_green")
                    screenInfo.defaultColour[COL_MC6847_GRAPH_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "graph_yellow")
                    screenInfo.defaultColour[COL_MC6847_GRAPH_YELLOW].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "graph_blue")
                    screenInfo.defaultColour[COL_MC6847_GRAPH_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "graph_red")
                    screenInfo.defaultColour[COL_MC6847_GRAPH_RED].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "graph_buff")
                    screenInfo.defaultColour[COL_MC6847_GRAPH_BUFF].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "graph_cyan")
                    screenInfo.defaultColour[COL_MC6847_GRAPH_CYAN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "graph_magenta")
                    screenInfo.defaultColour[COL_MC6847_GRAPH_MAGENTA].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "graph_orange")
                    screenInfo.defaultColour[COL_MC6847_GRAPH_ORANGE].Printf("#%02X%02X%02X", red, green, blue);
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.mc6847Configuration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.mc6847Configuration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
                XRCCTRL(*this,"Mc6847IoGroupText", wxStaticText)->SetLabel(p_Main->getGroupMessageXml(&computerConfiguration.mc6847Configuration.ioGroupVector));
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
    setBitValue(computerConfiguration.mc6847Configuration.inv, registerIdMc6847[MC6847_VIDEO_INV], registerFunctionMc6847[MC6847_VIDEO_INV]);
    setBitValue(computerConfiguration.mc6847Configuration.ext, registerIdMc6847[MC6847_VIDEO_EXT], registerFunctionMc6847[MC6847_VIDEO_EXT]);
    setBitValue(computerConfiguration.mc6847Configuration.css, registerIdMc6847[MC6847_VIDEO_CSS], registerFunctionMc6847[MC6847_VIDEO_CSS]);
    setBitValue(computerConfiguration.mc6847Configuration.as, registerIdMc6847[MC6847_VIDEO_AS], registerFunctionMc6847[MC6847_VIDEO_AS]);
    setBitValue(computerConfiguration.mc6847Configuration.ag, registerIdMc6847[MC6847_VIDEO_AG], registerFunctionMc6847[MC6847_VIDEO_AG]);
    setGmValue(computerConfiguration.mc6847Configuration.gm0, computerConfiguration.mc6847Configuration.gm1, computerConfiguration.mc6847Configuration.gm2, registerIdMc6847[MC6847_VIDEO_GM]);
}

void Mc6847Config::setBitValue(int bitNumber, wxString registerId, wxString function)
{
    if (bitNumber == -1)
        return;
    
    wxString functionLabel[] =
    {
        "(bit 0)",
        "(bit 1)",
        "(bit 2)",
        "(bit 3)",
        "(bit 4)",
        "(bit 5)",
        "(bit 6)",
        "(bit 7)",
        "(dd6)",
        "(dd7)",
    };
    
    if (computerConfiguration.mc6847Configuration.outputMode == 0)
        setIoPortConfig(computerConfiguration.mc6847Configuration.output, registerId, function + " " + functionLabel[bitNumber], "O");
    else
        setIoPortConfig(registerId,  function + " " + functionLabel[bitNumber], "A");
    
    if (bitNumber > 7)
        XRCCTRL(*this, registerId, wxTextCtrl)->Hide();
    else
        XRCCTRL(*this, registerId, wxTextCtrl)->Show();
}

void Mc6847Config::setGmValue(int bitNumber0, int bitNumber1, int bitNumber2, wxString registerId)
{
    wxString functionLabel = "GM (bit ", functionLabelTemp;
    if (bitNumber0 != -1)
    {
        functionLabelTemp.Printf("%d, ", bitNumber0);
        functionLabel += functionLabelTemp;
    }
    if (bitNumber1 != -1)
    {
        functionLabelTemp.Printf("%d, ", bitNumber1);
        functionLabel += functionLabelTemp;
    }
    if (bitNumber2 != -1)
    {
        functionLabelTemp.Printf("%d, ", bitNumber2);
        functionLabel += functionLabelTemp;
    }
    if (functionLabel.Len() <= 8)
        return;
    
    functionLabel = functionLabel.Left(functionLabel.Len() - 2);
    functionLabel += ")";

    if (computerConfiguration.mc6847Configuration.outputMode == 0)
        setIoPortConfig(computerConfiguration.mc6847Configuration.output, registerId, functionLabel, "O");
    else
        setIoPortConfig(registerId, functionLabel, "A");
}

void Mc6847Config::updateMc6847Panel()
{
    wxString buffer;

    if (computerConfiguration.mc6847Configuration.defined)
    {
        for (size_t registerNumber = 0; registerNumber<MC6847_NUMBER_OF_REGISTERS; registerNumber++)
        {
            if (mc6847ConfigRegisterValueString[registerNumber] != lastMc6847ConfigRegisterValueString[registerNumber])
            {
                p_Main->setTextCtrl(registerIdMc6847[registerNumber], mc6847ConfigRegisterValueString[registerNumber]);
                lastMc6847ConfigRegisterValueString[registerNumber] = mc6847ConfigRegisterValueString[registerNumber];
            }
        }
    }
}

int Mc6847Config::setMc6847Register(int registerNumber, Byte value, int showTrace)
{
    mc6847ConfigRegisterValueByte[registerNumber] = value;
    mc6847ConfigRegisterValueString[registerNumber].Printf("%02X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdMc6847[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showTraceText(registerFunctionMc6847[registerNumber], mc6847ConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
      }
      return showTrace;
}

int Mc6847Config::setMc6847RegisterNibble(int registerNumber, Byte value, int showTrace)
{
    mc6847ConfigRegisterValueString[registerNumber].Printf("%01X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this, registerIdMc6847[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showTraceText(registerFunctionMc6847[registerNumber], mc6847ConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
      }
      return showTrace;
}

bool Mc6847Config::isMc6847TraceChecked(int registerNumber)
{
    return XRCCTRL(*this, registerIdMc6847[registerNumber]+"Trace", wxCheckBox)->IsChecked();
}

void Mc6847Config::Mc6847VideoMode(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    long value = get8BitValue("Mc6847VideoMode") & 0x1f;
    if (value == -1)  return;

    mc6847Pointer->outMc6847(value, DO_NOT_SHOW_ADDRESS_TRACE);
}

void Mc6847Config::setOutMc6847(Byte value, int bitshift)
{
    if (bitshift > 8 && bitshift != 16)
    {
        bitshift -= 8;
        Byte orValue = (value & 1) << bitshift;
        Byte andValue = (1 << bitshift) ^ 0xff;
        mc6847Pointer->outMc6847((mc6847ConfigRegisterValueByte[MC6847_VIDEO_MODE] & andValue) | orValue, DO_NOT_SHOW_ADDRESS_TRACE);
    }
}

void Mc6847Config::Mc6847Inv(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    long value = get8BitValue("Mc6847Inv") & 0x1f;
    if (value == -1)  return;

    int bitshift;
    mc6847Pointer->invBit((int)(value & 1), &bitshift, DO_NOT_SHOW_ADDRESS_TRACE);
    setOutMc6847(value, bitshift);
}

void Mc6847Config::Mc6847Ext(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    long value = get8BitValue("Mc6847Ext") & 0x1f;
    if (value == -1)  return;

    int bitshift;
    mc6847Pointer->extBit((int)(value & 1), &bitshift, DO_NOT_SHOW_ADDRESS_TRACE);
    setOutMc6847(value, bitshift);
}

void Mc6847Config::Mc6847Css(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    long value = get8BitValue("Mc6847Css") & 0x1f;
    if (value == -1)  return;

    int bitshift;
    mc6847Pointer->cssBit((int)(value & 1), &bitshift, DO_NOT_SHOW_ADDRESS_TRACE);
    setOutMc6847(value, bitshift);
}

void Mc6847Config::Mc6847As(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    long value = get8BitValue("Mc6847As") & 0x1f;
    if (value == -1)  return;

    int bitshift;
    mc6847Pointer->asBit((int)(value & 1), &bitshift, DO_NOT_SHOW_ADDRESS_TRACE);
    setOutMc6847(value, bitshift);
}

void Mc6847Config::Mc6847Ag(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    long value = get8BitValue("Mc6847Ag") & 0x1f;
    if (value == -1)  return;

    int bitshift;
    mc6847Pointer->agBit((int)(value & 1), &bitshift, DO_NOT_SHOW_ADDRESS_TRACE);
    setOutMc6847(value, bitshift);
}

void Mc6847Config::Mc6847Gm(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    long value = get8BitValue("Mc6847Gm") & 0x1f;
    if (value == -1)  return;
    
    mc6847Pointer->gmBits((int)(value & 7), DO_NOT_SHOW_ADDRESS_TRACE);
    for (int number=0; number <3; number++)
        setOutMc6847((value >> number) & 1, mc6847Pointer->getGraphicModeBit(number));
}
