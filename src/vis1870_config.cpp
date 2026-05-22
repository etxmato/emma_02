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
#include "vis1870_config.h"
#include "wx/tglbtn.h"
#include "wx/xrc/xmlres.h"

#define THIS_PANEL_NAME "PanelVideo1870"

BEGIN_EVENT_TABLE(Vis1870Config, Tms9918Config)
    EVT_TEXT_ENTER(XRCID("VisIntReset"), Vis1870Config::VisIntReset)
    EVT_TEXT_ENTER(XRCID("VisIntEnable"), Vis1870Config::VisIntEnable)
    EVT_TEXT_ENTER(XRCID("VisR2"), Vis1870Config::VisR2)
    EVT_TEXT_ENTER(XRCID("VisR3"), Vis1870Config::VisR3)
    EVT_TEXT_ENTER(XRCID("VisR4"), Vis1870Config::VisR4)
    EVT_TEXT_ENTER(XRCID("VisR5_1"), Vis1870Config::VisR5_1)
    EVT_TEXT_ENTER(XRCID("VisR5_0"), Vis1870Config::VisR5_0)
    EVT_TEXT_ENTER(XRCID("VisR6"), Vis1870Config::VisR6)
    EVT_TEXT_ENTER(XRCID("VisR7"), Vis1870Config::VisR7)
END_EVENT_TABLE()

wxString registerId[] =
{
    "VisIntEnable",
    "VisIntReset",
    "VisR2",
    "VisR3",
    "VisR4",
    "VisR5_1",
    "VisR6",
    "VisR7",
    "VisR5_0",
    "VisIntEnable",
    "VisIntReset",
};

wxString registerFunction[] =
{
    "INT Enable",
    "INT Request",
    "Color Latch",
    "R3 Format",
    "R4 Tone",
    "R5 Noise",
    "R6 Page",
    "R7 Home",
    "R5 Format",
    "Rx Select",
    "Data",
};

Vis1870Config::Vis1870Config(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: Tms9918Config(title, pos, size, mode, dataDir, iniDir)
{
}

Vis1870Config::~Vis1870Config()
{
}

void Vis1870Config::visConfigInit()
{
    for (size_t registerNumber = 0; registerNumber<VIS_NUMBER_OF_REGISTERS; registerNumber++)
    {
        vis1870ConfigRegisterValueString[registerNumber] = "";
        lastVis1870ConfigRegisterValueString[registerNumber] = "x";
        vis1870ConfigRegisterValue[registerNumber] = 0;
    }

    registerSelectActive_ = false;
    
    computerConfiguration.vis1870Configuration.ioGroupVector.clear();
    computerConfiguration.vis1870Configuration.qGroup = -1;
    computerConfiguration.vis1870Configuration.ef = init_EfFlag();
    computerConfiguration.vis1870Configuration.outputWrite = init_IoPort();
    computerConfiguration.vis1870Configuration.outputSelect = init_IoPort();
    computerConfiguration.vis1870Configuration.outputInterruptEnable = init_IoPort();
    computerConfiguration.vis1870Configuration.outputInterruptReset = init_IoPort();

    computerConfiguration.vis1870Configuration.defined = false;
    computerConfiguration.vis1870Configuration.useVideoModeEf = false;
    computerConfiguration.vis1870Configuration.useBlockWrite = true;
    computerConfiguration.vis1870Configuration.pageMemSize = 0x3ff;
    computerConfiguration.vis1870Configuration.pageMemIsRom =false;
    computerConfiguration.vis1870Configuration.graphicMemSize = 0;
    computerConfiguration.vis1870Configuration.charMemSize =0x7ff;
    computerConfiguration.vis1870Configuration.charMemIsRom = false;
    computerConfiguration.vis1870Configuration.charRomIsTmcFormat = false;
    computerConfiguration.vis1870Configuration.pcbMask = 0x7f;
    computerConfiguration.vis1870Configuration.maxCharLines = 16;
    computerConfiguration.vis1870Configuration.maxScreenLines = 24;
    computerConfiguration.vis1870Configuration.interruptMode = INT_NONE;
    computerConfiguration.vis1870Configuration.videoClock = 5.626;
    computerConfiguration.vis1870Configuration.charRomStart = 0;
    computerConfiguration.vis1870Configuration.cmaMaskFixed = false;
    computerConfiguration.vis1870Configuration.videoModeEf = 2;
    computerConfiguration.vis1870Configuration.rotateScreen = false;
    computerConfiguration.vis1870Configuration.colorRamType = CR_NONE;
    computerConfiguration.vis1870Configuration.pageMemExcludeIoGroup = false;
    computerConfiguration.vis1870Configuration.charMemExcludeIoGroup = false;
    computerConfiguration.vis1870Configuration.graphicMemExcludeIoGroup = false;
    computerConfiguration.vis1870Configuration.charLines = -1;
    computerConfiguration.vis1870Configuration.videoMode = PAL;
    computerConfiguration.vis1870Configuration.picInterrupt = 0;
    computerConfiguration.vis1870Configuration.cursorBlink = false;

    computerConfiguration.vis1870Configuration.defaultX = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    computerConfiguration.vis1870Configuration.defaultY = mainWindowY_;

    if (!mode_.gui)
        return;

    XRCCTRL(*this, THIS_PANEL_NAME, wxPanel)->Hide();

    IoPort defaultPort;
    
    defaultPort = init_IoPort();
    
    for (int port=0; port<8; port++)
    {
        defaultPort.portNumber[0] = port;
        setIoPortConfig(defaultPort, registerId[port], registerFunction[port], "O");
    }
    defaultPort.portNumber[0] = 5;
    setIoPortConfig(defaultPort, registerId[8], registerFunction[8], "O");
    
    disableIoPortConfig(registerId[VIS_COLOR_LATCH]);
    vis1870ConfigRegisterValueString[VIS_COLOR_LATCH] = "";

    disableIoPortConfig(registerId[VIS_INT_ENABLE]);
    vis1870ConfigRegisterValueString[VIS_INT_ENABLE] = "";

    disableIoPortConfig(registerId[VIS_INT_REQUEST]);
    vis1870ConfigRegisterValueString[VIS_INT_REQUEST] = "";

    XRCCTRL(*this, "VisEfDisplayText", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "VisPageMemText", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "VisCharMemText", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "VisGraphicMemText", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "VisIoGroupText", wxStaticText)->SetLabel("");
}

void Vis1870Config::parseXml_VisVideo(wxXmlNode &node)
{
    videoConfigUsed(THIS_PANEL_NAME);
    
    Byte ctone[8] = {   0,  28,  77, 105, 150, 194, 227, 255 };
    Byte red[8]   = {   0,   0,   0,   0, 255, 255, 255, 255 };
    Byte green[8] = {   0, 255,   0, 255,   0, 255,   0, 255 };
    Byte blue[8]  = {   0,   0, 255, 255,   0,   0, 255, 255 };

    if (!computerConfiguration.vis1870Configuration.defined)
    {
        computerConfiguration.vis1870Configuration.videoNumber = computerConfiguration.numberOfVideoTypes_;
        computerConfiguration.numberOfVideoTypes_++;
    }

    computerConfiguration.vis1870Configuration.defined = true;
    computerConfiguration.zoom_[computerConfiguration.vis1870Configuration.videoNumber] = "2.00";
    computerConfiguration.videoName_[computerConfiguration.vis1870Configuration.videoNumber] = "VIS 1870";
    
    wxString tagList[]=
    {
        "pagesize",
        "charsize",
        "graphicsize",
        "charrom",
        "filename",
        "dirname",
        "pcbmask",
        "charlines",
        "maxcharlines",
        "maxscreenlines",
        "mode",
        "int_mode",
        "int",
        "ef",
        "clock",
        "cmamaskfixed",
        "rotate",
        "colorram",
        "out",
        "zoom",
        "border",
        "pos",
        "color",
        "ctone",
        "cursorblink",
        "iogroup",
        "log",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_PAGE,
        TAG_CHAR,
        TAG_GRAPHIC,
        TAG_CHARROM,
        TAG_FILENAME,
        TAG_DIRNAME,
        TAG_PCB,
        TAG_CHARLINES,
        TAG_MAX_CHARLINES,
        TAG_SCREENLINES,
        TAG_MODE,
        TAG_INT_MODE,
        TAG_INTERRUPT,
        TAG_EF,
        TAG_CLOCK,
        TAG_CMAMASK,
        TAG_ROTATE,
        TAG_COLORRAM,
        TAG_OUT,
        TAG_ZOOM,
        TAG_BORDER,
        TAG_POS,
        TAG_COLOR,
        TAG_CTONE,
        TAG_CURSORBLINK,
        TAG_IOGROUP,
        TAG_LOG,
        TAG_COMMENT,
        TAG_UNDEFINED
    };

    int tagTypeInt;
    long width, height;
    int number, xpos, ypos;
    wxString color, position, iogroup, label, labelDetails;
    size_t ioGroupNumber = 0;
    IoPort defaultPort;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_PAGE:
                labelDetails = "Page RAM = %d KB";
                if (child->GetAttribute("type") == "rom")
                {
                    computerConfiguration.vis1870Configuration.pageMemIsRom = true;
                    labelDetails = "Page ROM = %d KB";
                }
                if (child->GetAttribute("iogroup") == "no")
                {
                    computerConfiguration.vis1870Configuration.pageMemExcludeIoGroup = true;
                    labelDetails += " (no grp)";
                }
                computerConfiguration.vis1870Configuration.pageMemSize = (int)parseXml_Number(*child) & 0xfff;
                label.Printf(labelDetails, (computerConfiguration.vis1870Configuration.pageMemSize+1) / 0x400);
                if (mode_.gui)
                    XRCCTRL(*this, "VisPageMemText", wxStaticText)->SetLabel(label);
                
            break;
                                
            case TAG_CHAR:
                labelDetails = "Character RAM = %d KB";
                if (child->GetAttribute("type") == "rom")
                {
                    computerConfiguration.vis1870Configuration.charMemIsRom = true;
                    labelDetails = "Character ROM = %d KB";
                }
                if (child->GetAttribute("iogroup") == "no")
                {
                    computerConfiguration.vis1870Configuration.charMemExcludeIoGroup = true;
                    labelDetails += " (no grp)";
                }
                computerConfiguration.vis1870Configuration.charMemSize =  (int)parseXml_Number(*child) & 0xfff;
                label.Printf(labelDetails, (computerConfiguration.vis1870Configuration.charMemSize+1) / 0x400);
                if (mode_.gui)
                    XRCCTRL(*this, "VisCharMemText", wxStaticText)->SetLabel(label);
            break;

            case TAG_GRAPHIC:
                labelDetails = "Graphic RAM = %d KB";
                if (child->GetAttribute("iogroup") == "no")
                {
                    computerConfiguration.vis1870Configuration.graphicMemExcludeIoGroup = true;
                    labelDetails += " (no grp)";
                }
                computerConfiguration.vis1870Configuration.graphicMemSize =  (int)parseXml_Number(*child) & 0x1fff;
                label.Printf(labelDetails, (computerConfiguration.vis1870Configuration.graphicMemSize+1) / 0x400);
                if (mode_.gui)
                    XRCCTRL(*this, "VisGraphicMemText", wxStaticText)->SetLabel(label);
            break;

            case TAG_CHARROM:
                computerConfiguration.vis1870Configuration.charRomStart = (int)parseXml_Number(*child);
            break;

            case TAG_FILENAME:
                if (child->GetAttribute("format") == "tmc")
                    computerConfiguration.vis1870Configuration.charRomIsTmcFormat = true;
                computerConfiguration.characterRomConfiguration.fileName = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                computerConfiguration.characterRomConfiguration.directory = dataDir_ + child->GetNodeContent();
                if (computerConfiguration.characterRomConfiguration.directory.Right(1) != pathSeparator_)
                    computerConfiguration.characterRomConfiguration.directory += pathSeparator_;
            break;

            case TAG_PCB:
                computerConfiguration.vis1870Configuration.pcbMask =  (int)parseXml_Number(*child);
            break;

            case TAG_CHARLINES:
                computerConfiguration.vis1870Configuration.charLines = (int)parseXml_Number(*child);
            break;

            case TAG_MAX_CHARLINES:
                computerConfiguration.vis1870Configuration.maxCharLines = (int)parseXml_Number(*child);
            break;

            case TAG_SCREENLINES:
                computerConfiguration.vis1870Configuration.maxScreenLines = (int)parseXml_Number(*child);
            break;

            case TAG_MODE:
                if (child->GetNodeContent() == "ntsc")
                    computerConfiguration.vis1870Configuration.videoMode = NTSC;
            break;

            case TAG_INT_MODE:
                computerConfiguration.vis1870Configuration.interruptMode =  (int)parseXml_Number(*child) - 1;
            break;

            case TAG_INTERRUPT:
                computerConfiguration.vis1870Configuration.picInterrupt =  (int)parseXml_Number(*child) - 1;
            break;

            case TAG_EF:
                if (child->GetAttribute("type") == "display")
                {
                    computerConfiguration.vis1870Configuration.ef = parseXml_EfFlag(*child, VIS1870_EF);
                    setEfFlagConfig(computerConfiguration.vis1870Configuration.ef, "(non)display period", "VisEfDisplayText");
                }
                if (child->GetAttribute("type") == "mode")
                {
                    computerConfiguration.vis1870Configuration.videoModeEf = (int)parseXml_Number(*child);
                    computerConfiguration.vis1870Configuration.useVideoModeEf = true;
                }
                if (child->GetAttribute("write") == "enable")
                {
                    computerConfiguration.vis1870Configuration.useBlockWrite = false;
                }
            break;
                
            case TAG_CLOCK:
                computerConfiguration.vis1870Configuration.videoClock = getDouble(child->GetNodeContent(), childName, 500, "500", false);
            break;

            case TAG_CMAMASK:
                computerConfiguration.vis1870Configuration.cmaMaskFixed = true;
            break;
                
            case TAG_ROTATE:
                computerConfiguration.vis1870Configuration.rotateScreen = true;
            break;

            case TAG_COLORRAM:
                if (child->GetAttribute("type") == "cidelsa")
                    computerConfiguration.vis1870Configuration.colorRamType = CR_CIDELSA;
                if (child->GetAttribute("type") == "tmc")
                    computerConfiguration.vis1870Configuration.colorRamType = CR_TMC600;
                if (child->GetAttribute("type") == "vp3301")
                    computerConfiguration.vis1870Configuration.colorRamType = CR_VP3301;
            break;
                
            case TAG_OUT:
                if (child->GetAttribute("type") == "write")
                {
                    registerSelectActive_ = true;

                    computerConfiguration.vis1870Configuration.outputWrite = parseXml_IoPort(*child, VIS1870_TELMAC_DATA_OUT);
                    setIoPortConfig(computerConfiguration.vis1870Configuration.outputWrite, registerId[VIS_DATA], registerFunction[VIS_DATA], "O");
                }
                if (child->GetAttribute("type") == "select")
                {
                    registerSelectActive_ = true;
                    

                    defaultPort = init_IoPort();
                    defaultPort.portNumber[0] = 2;
                    setIoPortConfig(defaultPort, registerId[2], registerFunction[2], "O");
                    if (mode_.gui)
                        for (int port=2; port<9; port++)
                            XRCCTRL(*this, registerId[port]+"Port", wxStaticText)->SetLabel("-");

                    computerConfiguration.vis1870Configuration.outputSelect = parseXml_IoPort(*child, VIS1870_TELMAC_REGISTER_OUT);
                    setIoPortConfig(computerConfiguration.vis1870Configuration.outputSelect, registerId[VIS_REGISTER], registerFunction[VIS_REGISTER], "O");
                }
                if (child->GetAttribute("type") == "int")
                {
                    computerConfiguration.vis1870Configuration.outputInterruptEnable = parseXml_IoPort(*child, VIS1870_OUT2);
                    
                    if (!registerSelectActive_)
                        setIoPortConfig(computerConfiguration.vis1870Configuration.outputInterruptEnable, registerId[VIS_INT_ENABLE], registerFunction[VIS_INT_ENABLE], "O");
                }
                if (child->GetAttribute("type") == "int_reset")
                {
                    computerConfiguration.vis1870Configuration.outputInterruptReset = parseXml_IoPort(*child, VIS1870_INT_RESET);
                    if (!registerSelectActive_)
                        setIoPortConfig(computerConfiguration.vis1870Configuration.outputInterruptReset, registerId[VIS_INT_REQUEST], registerFunction[VIS_INT_REQUEST], "O");
                }
            break;

            case TAG_ZOOM:
                computerConfiguration.zoom_[computerConfiguration.vis1870Configuration.videoNumber] = child->GetNodeContent();
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
                    screenInfo.borderX[VIDEOXML1870] = (int)width;
                    screenInfo.borderY[VIDEOXML1870] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += computerConfiguration.vis1870Configuration.defaultX;
                if (ypos != -1)
                    ypos += computerConfiguration.vis1870Configuration.defaultY;
                computerConfiguration.vis1870Configuration.defaultX = xpos;
                computerConfiguration.vis1870Configuration.defaultY = ypos;
            break;

            case TAG_COLOR:
                number = (int)parseXml_Number(*child, "num");
                color = child->GetNodeContent();
                red[number] = getNextHexDec(&color) & 0xff;
                green[number] = getNextHexDec(&color) & 0xff;
                blue[number] = getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "mask")
                {
                    if (number == 0)
                    {
                        for (int i = 0; i < 8; i++)
                        {
                            screenInfo.defaultColour[i*8 + number].Printf("#%02X%02X%02X", (red[number] ^ 0xff) & (ctone[i] ^ 0xff), (green[number] ^ 0xff) & (ctone[i] ^ 0xff), (blue[number] ^ 0xff) & (ctone[i] ^ 0xff));
                        }
                    }
                    else
                    {
                        for (int i = 0; i < 8; i++)
                        {
                            screenInfo.defaultColour[i*8 + number].Printf("#%02X%02X%02X", red[number] & ctone[i], green[number] & ctone[i], blue[number] & ctone[i]);
                        }
                    }
                }
                else
                    screenInfo.defaultColour[number+56].Printf("#%02X%02X%02X", red[number], green[number], blue[number]);
            break;

            case TAG_CTONE:
                number = (int)parseXml_Number(*child, "num");
                ctone[number] = (int)parseXml_Number(*child);
                
                screenInfo.defaultColour[number*8].Printf("#%02X%02X%02X", (red[0] ^ 0xff) & (ctone[number] ^0xff), (green[0] ^ 0xff) & (ctone[number] ^0xff), (blue[0] ^ 0xff) & (ctone[number] ^0xff));
                for (int i = 1; i < 8; i++)
                    screenInfo.defaultColour[number*8 + i].Printf("#%02X%02X%02X", red[i] & ctone[number], green[i] & ctone[number], blue[i] & ctone[number]);
            break;

            case TAG_CURSORBLINK:
                computerConfiguration.vis1870Configuration.cursorBlink = true;
            break;

            case TAG_LOG:
                computerConfiguration.debuggerConfiguration.videoLog_defined = true;
            break;

            case TAG_IOGROUP:
                if (child->GetAttribute("type") == "q")
                {
                    computerConfiguration.vis1870Configuration.qGroup = (int)parseXml_Number(*child);
                }
                else
                {
                    iogroup = child->GetNodeContent();
                    while (iogroup != "")
                    {
                        computerConfiguration.vis1870Configuration.ioGroupVector.resize(ioGroupNumber+1);
                        computerConfiguration.vis1870Configuration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                    }
                    if (mode_.gui)
                        XRCCTRL(*this,"VisIoGroupText", wxStaticText)->SetLabel(p_Main->getGroupMessageXml(&computerConfiguration.vis1870Configuration.ioGroupVector));
                }
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
    if (computerConfiguration.vis1870Configuration.charLines == -1)
    {
        if (computerConfiguration.vis1870Configuration.videoMode == PAL)
            computerConfiguration.vis1870Configuration.charLines = 9;
        else
            computerConfiguration.vis1870Configuration.charLines = 8;
    }
    if (!mode_.gui)
        return;
    
    if (computerConfiguration.vis1870Configuration.maxCharLines > computerConfiguration.vis1870Configuration.charLines)
        label.Printf("Character size: 6x%d or 6x%d", computerConfiguration.vis1870Configuration.charLines, computerConfiguration.vis1870Configuration.maxCharLines);
    else
        label.Printf("Character size: 6x%d", computerConfiguration.vis1870Configuration.charLines);
    XRCCTRL(*this, "VisCharSizeText", wxStaticText)->SetLabel(label);
    label.Printf("%d Characters", (computerConfiguration.vis1870Configuration.charMemSize+1)/computerConfiguration.vis1870Configuration.maxCharLines);
    XRCCTRL(*this, "VisCharNumberText", wxStaticText)->SetLabel(label);

    label.Printf("PCB Mask = 0x%02X", computerConfiguration.vis1870Configuration.pcbMask);
    XRCCTRL(*this, "VisPcbMaskText", wxStaticText)->SetLabel(label);

    label.Printf("Clock speed = %.3g MHz", computerConfiguration.vis1870Configuration.videoClock);
    XRCCTRL(*this, "VisClockText", wxStaticText)->SetLabel(label);
}

void Vis1870Config::updateVis1870Panel()
{
    if (computerConfiguration.vis1870Configuration.defined)
    {
        for (size_t registerNumber = 0; registerNumber<VIS_NUMBER_OF_REGISTERS; registerNumber++)
        {
            if (vis1870ConfigRegisterValueString[registerNumber] != lastVis1870ConfigRegisterValueString[registerNumber])
            {
                p_Main->setTextCtrl(registerId[registerNumber], vis1870ConfigRegisterValueString[registerNumber]);
                lastVis1870ConfigRegisterValueString[registerNumber] = vis1870ConfigRegisterValueString[registerNumber];
            }
        }
    }
}

int Vis1870Config::setVisRegister(int registerNumber, Word value, int showTrace)
{
    vis1870ConfigRegisterValue[registerNumber] = value;
    vis1870ConfigRegisterValueString[registerNumber].Printf("%04X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerId[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showVideoTraceText(registerFunction[registerNumber], vis1870ConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
      }
      return showTrace;
}

int Vis1870Config::setVisRegister(int registerNumber, Byte value, int showTrace)
{
    vis1870ConfigRegisterValue[registerNumber] = value;
    vis1870ConfigRegisterValueString[registerNumber].Printf("%02X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerId[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showVideoTraceText(registerFunction[registerNumber], vis1870ConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
      }
      return showTrace;
}

void Vis1870Config::VisIntReset(wxCommandEvent&WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    if (registerSelectActive_)
    {
        long value = get16BitValue("VisIntReset");
        if (value == -1)  return;

        vis1870Pointer->setOutData_1870((Word)value, (Byte)value, DO_NOT_SHOW_ADDRESS_TRACE);
    }
    else
    {
        long value = get8BitValue("VisIntReset");
        if (value == -1)  return;

        p_Computer->requestInterrupt(INTERRUPT_TYPE_VIS, false, computerConfiguration.vis1870Configuration.picInterrupt);
        setVisRegister(VIS_INT_REQUEST, (Byte)value, DO_NOT_SHOW_ADDRESS_TRACE);
    }
}

void Vis1870Config::VisIntEnable(wxCommandEvent&WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    long value = get8BitValue("VisIntEnable");
    if (value == -1)  return;

    if (registerSelectActive_)
        vis1870Pointer->setRegisterSelect_1870(value, DO_NOT_SHOW_ADDRESS_TRACE);
    else
    {
        vis1870Pointer->setInterruptEnable(value == 1);
        setVisRegister(VIS_INT_ENABLE, (Byte)value, DO_NOT_SHOW_ADDRESS_TRACE);
    }
}

void Vis1870Config::VisR2(wxCommandEvent&WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    long value = get8BitValue("VisR2");
    if (value == -1)  return;

    vis1870Pointer->out2_1870((Byte)value);
    setVisRegister(VIS_COLOR_LATCH, (Byte)value, DO_NOT_SHOW_ADDRESS_TRACE);
}

void Vis1870Config::VisR3(wxCommandEvent&WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    long value = get8BitValue("VisR3");
    if (value == -1)  return;

    vis1870Pointer->out3_1870(value);
    setVisRegister(VIS_R3, (Byte)value, DO_NOT_SHOW_ADDRESS_TRACE);
}

void Vis1870Config::VisR4(wxCommandEvent&WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    long value = get16BitValue("VisR4");
    if (value == -1)  return;

    value &= 0x7fff;
    vis1870Pointer->out4_1870(value);
    setVisRegister(VIS_R4, (Word)value, DO_NOT_SHOW_ADDRESS_TRACE);
}

void Vis1870Config::VisR5_1(wxCommandEvent&WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    long value = get8BitValue("VisR5_1");
    if (value == -1)  return;

    setVisRegister(VIS_R5_1, (Byte)value, DO_NOT_SHOW_ADDRESS_TRACE);

    Word wordValue = (value << 8) | vis1870ConfigRegisterValue[VIS_R5_0];
    vis1870Pointer->out5_1870(wordValue);
}

void Vis1870Config::VisR5_0(wxCommandEvent&WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    long value = get8BitValue("VisR5_0");
    if (value == -1)  return;

    value &= 0xe9;
    setVisRegister(VIS_R5_0, (Byte)value, DO_NOT_SHOW_ADDRESS_TRACE);

    value = (vis1870ConfigRegisterValue[VIS_R5_1] << 8) | value;
    vis1870Pointer->out5_1870((Word)value);
}

void Vis1870Config::VisR6(wxCommandEvent&WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    long value = get16BitValue("VisR6");
    if (value == -1)  return;

    value &= 0x3ff;
    vis1870Pointer->out6_1870(value);
    setVisRegister(VIS_R6, (Word)value, DO_NOT_SHOW_ADDRESS_TRACE);
}

void Vis1870Config::VisR7(wxCommandEvent&WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    long value = get16BitValue("VisR7");
    if (value == -1)  return;

    value &= 0x3fc;
    vis1870Pointer->out7_1870(value);
    setVisRegister(VIS_R7, (Word)value, DO_NOT_SHOW_ADDRESS_TRACE);
}
