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
#include "i8275_config.h"
#include "wx/tglbtn.h"
#include "wx/xrc/xmlres.h"

#define THIS_PANEL_NAME "PanelVideo8275"

BEGIN_EVENT_TABLE(I8275Config, Scn2672Config)
    EVT_TEXT_ENTER(XRCID("8275Command"), I8275Config::I8275Command)
    EVT_TEXT_ENTER(XRCID("8275WriteParameter"), I8275Config::I8275WriteParameter)
    EVT_TEXT_ENTER(XRCID("8275ReadStatus"), I8275Config::I8275Status)

    EVT_TEXT_ENTER(XRCID("8275CharRow"), I8275Config::I8275CharRow)
    EVT_TEXT_ENTER(XRCID("8275VerticalRetrace"), I8275Config::I8275VerticalRetrace)
    EVT_TEXT_ENTER(XRCID("8275RowsFrame"), I8275Config::I8275RowsFrame)
    EVT_TEXT_ENTER(XRCID("8275Underline"), I8275Config::I8275Underline)
    EVT_TEXT_ENTER(XRCID("8275LinesRow"), I8275Config::I8275LinesRow)
    EVT_TEXT_ENTER(XRCID("8275CursorFormat"), I8275Config::I8275CursorFormat)
    EVT_TEXT_ENTER(XRCID("8275HorizontalRetrace"), I8275Config::I8275HorizontalRetrace)
    EVT_TEXT_ENTER(XRCID("8275BurstSpaceCode"), I8275Config::I8275BurstSpaceCode)
    EVT_TEXT_ENTER(XRCID("8275DmaCyclesBurst"), I8275Config::I8275DmaCyclesBurst)
    EVT_TEXT_ENTER(XRCID("8275CursorChar"), I8275Config::I8275CursorChar)
    EVT_TEXT_ENTER(XRCID("8275CursorRow"), I8275Config::I8275CursorRow)

END_EVENT_TABLE()

wxString registerIdI8275[] =
{
    "8275Command",
    "8275WriteParameter",
    "8275ReadParameter",
    "8275ReadStatus",
    "8275CharRow",
    "8275VerticalRetrace",
    "8275RowsFrame",
    "8275Underline",
    "8275LinesRow",
    "8275CursorFormat",
    "8275HorizontalRetrace",
    "8275BurstSpaceCode",
    "8275DmaCyclesBurst",
    "8275CursorChar",
    "8275CursorRow",
};

wxString registerFunctionI8275[] =
{
    "Command",
    "Write parameter",
    "Read parameter",
    "Status",
    "Char/row",
    "Retrace rows",
    "Rows/frame",
    "Underline no.",
    "Lines/row",
    "Cursor format",
    "Retrace char",
    "Burst space code",
    "Dma cycles/burst",
    "Cursor - char",
    "Cursor - row",
};

wxString selectorIdI8275[] =
{
    "8275ParameterS",
    "8275ParameterM",
    "8275ParameterF",
    "8275ParameterCC",
    "8275Display",
    "8275Interrupt",
};

wxString selectorFunctionI8275[6][4] =
{ // false, true
    {"Normal rows", "Spaced rows", "", "" },
    {"LCM: non-offset", "LCM: offset +1", "", "" },
    {"FAM: transparent", "FAM: non-transparent", "", "" },
    {"Cursor: blinking block", "Cursor: blinking underline", "Cursor: nonblinking block", "Cursor: nonblinking underline" },
    {"Display: off", "Display: on", "", "" },
    {"Interrupt disable", "Interrupt enabled", "", "" }
};

I8275Config::I8275Config(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: Scn2672Config(title, pos, size, mode, dataDir, iniDir)
{
}

I8275Config::~I8275Config()
{
}

void I8275Config::i8275ConfigInit()
{
    for (size_t registerNumber = 0; registerNumber<I8275_NUMBER_OF_REGISTERS; registerNumber++)
    {
        i8275ConfigRegisterValueString[registerNumber] = "";
        lastI8275ConfigRegisterValueString[registerNumber] = "x";
    }
    for (size_t registerNumber = 0; registerNumber<I8275_NUMBER_OF_SELECTORS; registerNumber++)
    {
        i8275ConfigSelector[registerNumber] = 0;
        updateI8275ConfigSelector[registerNumber] = true;
    }

    computerConfiguration.i8275Configuration.writeCommandOutput = init_IoPort();
    computerConfiguration.i8275Configuration.readStatus = init_IoPort();
    computerConfiguration.i8275Configuration.writeParameter = init_IoPort();
    computerConfiguration.i8275Configuration.readParameter = init_IoPort();
    computerConfiguration.i8275Configuration.efVerticalRetrace = init_EfFlag();
    computerConfiguration.i8275Configuration.efHorizontalRetrace = init_EfFlag();

    computerConfiguration.i8275Configuration.defined = false;
    computerConfiguration.i8275Configuration.ioGroupVector.clear();
    computerConfiguration.i8275Configuration.charSize.x = 8;
    computerConfiguration.i8275Configuration.charSize.y = 10;
    computerConfiguration.i8275Configuration.screenSize.x = 80;
    computerConfiguration.i8275Configuration.screenSize.y = 24;
    computerConfiguration.i8275Configuration.videoClock = 12;
    computerConfiguration.i8275Configuration.gpaSwitched = false;
    
    computerConfiguration.i8275Configuration.picInterrupt = 0;
    computerConfiguration.i8275Configuration.picInterruptHorizontal = 0;

    computerConfiguration.i8275Configuration.defaultX = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    computerConfiguration.i8275Configuration.defaultY = mainWindowY_;

    if (!mode_.gui)
        return;

    XRCCTRL(*this, THIS_PANEL_NAME, wxPanel)->Hide();

    XRCCTRL(*this, "8275IoGroupText", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "8275EfHorizontal", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "8275EfVertical", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "8275ParameterSText", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "8275ParameterMText", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "8275ParameterFText", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "8275ParameterCCText", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "8275DisplayText", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "8275InterruptText", wxStaticText)->SetLabel("");

    disableIoPortConfig(registerIdI8275[I8275_COMMAND]);
    disableIoPortConfig(registerIdI8275[I8275_WRITE_PARAMETER]);
    disableIoPortConfig(registerIdI8275[I8275_READ_PARAMETER]);
    disableIoPortConfig(registerIdI8275[I8275_STATUS]);
}

void I8275Config::parseXml_Intel8275Video(wxXmlNode &node)
{
    videoConfigUsed(THIS_PANEL_NAME);

    if (!computerConfiguration.i8275Configuration.defined)
    {
        computerConfiguration.i8275Configuration.videoNumber = computerConfiguration.numberOfVideoTypes_;
        computerConfiguration.numberOfVideoTypes_++;
    }

    computerConfiguration.i8275Configuration.defined = true;

    wxString tagList[]=
    {
        "font",
        "dirname",
        "in",
        "out",
        "ef",
        "int",
        "interlace",
        "zoom",
        "border",
        "pos",
        "color",
        "clock",
        "char",
        "screen",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_FONT,
        TAG_DIRNAME,
        TAG_IN,
        TAG_OUT,
        TAG_EF,
        TAG_INTERRUPT,
        TAG_INTERLACE,
        TAG_ZOOM,
        TAG_BORDER,
        TAG_POS,
        TAG_COLOR,
        TAG_CLOCK,
        TAG_CHAR,
        TAG_SCREEN,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };

    computerConfiguration.zoom_[computerConfiguration.i8275Configuration.videoNumber] = "2.00";
    computerConfiguration.videoName_[computerConfiguration.i8275Configuration.videoNumber] = "Intel 8275";
    
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
            case TAG_IN:
                if (child->GetAttribute("type") == "status")
                {
                    computerConfiguration.i8275Configuration.readStatus = parseXml_IoPort(*child, I8275_READ_STATUS_IN);
                    setIoPortConfig(computerConfiguration.i8275Configuration.readStatus, registerIdI8275[I8275_STATUS], registerFunctionI8275[I8275_STATUS], "I");
                }
                if (child->GetAttribute("type") == "parameter")
                {
                    computerConfiguration.i8275Configuration.readParameter = parseXml_IoPort(*child, I8275_READ_PARAMETER_IN);
                    setIoPortConfig(computerConfiguration.i8275Configuration.readParameter, registerIdI8275[I8275_READ_PARAMETER], registerFunctionI8275[I8275_READ_PARAMETER], "I");
                    if (mode_.gui)
                        XRCCTRL(*this, registerIdI8275[I8275_READ_PARAMETER], wxTextCtrl)->Enable(false);
                }
            break;

            case TAG_OUT:
                if (child->GetAttribute("type") == "command")
                {
                    computerConfiguration.i8275Configuration.writeCommandOutput = parseXml_IoPort(*child, I8275_WRITE_COMMAND_OUT);
                    setIoPortConfig(computerConfiguration.i8275Configuration.writeCommandOutput, registerIdI8275[I8275_COMMAND], registerFunctionI8275[I8275_COMMAND], "O");
                }
                if (child->GetAttribute("type") == "parameter")
                {
                    computerConfiguration.i8275Configuration.writeParameter  = parseXml_IoPort(*child, I8275_WRITE_PARAMETER_OUT);
                    setIoPortConfig(computerConfiguration.i8275Configuration.writeParameter, registerIdI8275[I8275_WRITE_PARAMETER], registerFunctionI8275[I8275_WRITE_PARAMETER], "O");
                }
            break;
                
            case TAG_FONT:
                computerConfiguration.characterRomConfiguration.fileName = child->GetNodeContent();
                if (child->GetAttribute("gpa") == "switched")
                    computerConfiguration.i8275Configuration.gpaSwitched = true;
            break;

            case TAG_DIRNAME:
                computerConfiguration.characterRomConfiguration.directory = dataDir_ + child->GetNodeContent();
                if (computerConfiguration.characterRomConfiguration.directory.Right(1) != pathSeparator_)
                    computerConfiguration.characterRomConfiguration.directory += pathSeparator_;
            break;

            case TAG_EF:
                if (child->GetAttribute("type") == "horizontal")
                {
                    computerConfiguration.i8275Configuration.efHorizontalRetrace = parseXml_EfFlag(*child, I8275_HORIZONTAL_EF);
                    setEfFlagConfig(computerConfiguration.i8275Configuration.efHorizontalRetrace, "horizontal retrace", "8275EfHorizontal");
                }
                else
                {
                    computerConfiguration.i8275Configuration.efVerticalRetrace = parseXml_EfFlag(*child, I8275_VERTICAL_EF);
                    setEfFlagConfig(computerConfiguration.i8275Configuration.efVerticalRetrace, "vertical retrace", "8275EfVertical");
                }
            break;
                                
            case TAG_INTERRUPT:
                if (child->GetAttribute("type") == "horizontal")
                    computerConfiguration.i8275Configuration.picInterrupt = (int)parseXml_Number(*child);
                else
                    computerConfiguration.i8275Configuration.picInterruptHorizontal = (int)parseXml_Number(*child);
            break;

            case TAG_INTERLACE:
                computerConfiguration.interlace_ = true;
            break;
                
            case TAG_ZOOM:
                computerConfiguration.zoom_[computerConfiguration.i8275Configuration.videoNumber] = child->GetNodeContent();
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
                    screenInfo.borderX[VIDEOXMLI8275] = (int)width;
                    screenInfo.borderY[VIDEOXMLI8275] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += computerConfiguration.i8275Configuration.defaultX;
                if (ypos != -1)
                    ypos += computerConfiguration.i8275Configuration.defaultY;
                computerConfiguration.i8275Configuration.defaultX = xpos;
                computerConfiguration.i8275Configuration.defaultY = ypos;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "fore")
                    screenInfo.defaultColour[COL_I8275_FORE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back")
                    screenInfo.defaultColour[COL_I8275_BACK].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "high")
                    screenInfo.defaultColour[COL_I8275_HIGH].Printf("#%02X%02X%02X", red, green, blue);
            break;
                
            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.i8275Configuration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.i8275Configuration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
            break;

            case TAG_CLOCK:
                computerConfiguration.i8275Configuration.videoClock = getDouble(child->GetNodeContent(), childName, 500, "500", false);
            break;

            case TAG_CHAR:
                if (!parseXml_Size(*child, &width, &height))
                {
                    warningText_ += "Incorrect char size";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    computerConfiguration.i8275Configuration.charSize.x = (int)width;
                    computerConfiguration.i8275Configuration.charSize.y = (int)height;
                }
            break;

            case TAG_SCREEN:
                if (!parseXml_Size(*child, &width, &height))
                {
                    warningText_ += "Incorrect screen size";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    computerConfiguration.i8275Configuration.screenSize.x = (int)width;
                    computerConfiguration.i8275Configuration.screenSize.y = (int)height;
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
}

void I8275Config::updateI8275Panel()
{
    if (computerConfiguration.i8275Configuration.defined)
    {
        for (size_t registerNumber = 0; registerNumber<I8275_NUMBER_OF_REGISTERS; registerNumber++)
        {
            if (i8275ConfigRegisterValueString[registerNumber] != lastI8275ConfigRegisterValueString[registerNumber])
            {
                p_Main->setTextCtrl(registerIdI8275[registerNumber], i8275ConfigRegisterValueString[registerNumber]);
                lastI8275ConfigRegisterValueString[registerNumber] = i8275ConfigRegisterValueString[registerNumber];
            }
        }
        for (size_t selectorNumber = 0; selectorNumber<I8275_NUMBER_OF_SELECTORS; selectorNumber++)
        {
            if (updateI8275ConfigSelector[selectorNumber])
            {
                XRCCTRL(*this, selectorIdI8275[selectorNumber] + "Text", wxStaticText)->SetLabel(selectorFunctionI8275[selectorNumber][i8275ConfigSelector[selectorNumber]]);
                updateI8275ConfigSelector[selectorNumber] = false;
            }
        }
    }
}

int I8275Config::setI8275Register(int registerNumber, Word value, int showTrace)
{
    i8275ConfigRegisterValueString[registerNumber].Printf("%04X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdI8275[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showVideoTraceText(registerFunctionI8275[registerNumber], i8275ConfigRegisterValueString[registerNumber], showTrace);
      
        if (showTrace == SHOW_ADDRESS_TRACE)
            return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

int I8275Config::setI8275Register(int registerNumber, Byte value, int showTrace)
{
    i8275ConfigRegisterValueString[registerNumber].Printf("%02X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdI8275[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showVideoTraceText(registerFunctionI8275[registerNumber], i8275ConfigRegisterValueString[registerNumber], showTrace);
        
        if (showTrace == SHOW_ADDRESS_TRACE)
            return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

int I8275Config::setI8275RegisterNibble(int registerNumber, Byte value, int showTrace)
{
    i8275ConfigRegisterValueString[registerNumber].Printf("%01X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdI8275[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showVideoTraceText(registerFunctionI8275[registerNumber], i8275ConfigRegisterValueString[registerNumber], showTrace);
        
        if (showTrace == SHOW_ADDRESS_TRACE)
            return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

int I8275Config::setI8275RegisterValue(int registerNumber, Byte value, int showTrace)
{
    i8275ConfigRegisterValueString[registerNumber].Printf("%02X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdI8275[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showVideoTraceText("Register value", i8275ConfigRegisterValueString[registerNumber], showTrace);

        if (showTrace == SHOW_ADDRESS_TRACE)
            return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}
/*
int I8275Config::setI8275DisplayMode(Byte mode, int showTrace)
{
    setI8275SelectorValue(I8275_DISPLAY_MODE_SELECTOR, mode);
    return p_Main->setI8275RegisterNibble(I8275_DISPLAY_MODE, mode, showTrace);
}*/

int I8275Config::setI8275SelectorValue(int selectorNumber, int selectorValue, int showTrace)
{
    i8275ConfigSelector[selectorNumber] = selectorValue;
    updateI8275ConfigSelector[selectorNumber] = true;

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,selectorIdI8275[selectorNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showVideoTraceText(selectorFunctionI8275[selectorNumber][i8275ConfigSelector[selectorNumber]], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

int I8275Config::setI8275SelectorValue(int selectorNumber, bool selectorValue, int showTrace)
{
    if (selectorValue)
        i8275ConfigSelector[selectorNumber] = 1;
    else
        i8275ConfigSelector[selectorNumber] = 0;
    updateI8275ConfigSelector[selectorNumber] = true;

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,selectorIdI8275[selectorNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showVideoTraceText(selectorFunctionI8275[selectorNumber][i8275ConfigSelector[selectorNumber]], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

void I8275Config::I8275Command(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    long value = get8BitValue("8275Command");
    if (value == -1)  return;

    i8275Pointer->cRegWrite((Byte)value, DO_NOT_SHOW_ADDRESS_TRACE);
}

void I8275Config::I8275WriteParameter(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    long value = get8BitValue("8275WriteParameter");
    if (value == -1)  return;

    i8275Pointer->pRegWrite((Byte)value, DO_NOT_SHOW_ADDRESS_TRACE);
}

void I8275Config::I8275Status(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    long value = get8BitValue("8275ReadStatus");
    if (value == -1)  return;

    i8275Pointer->setStatus((Byte)value);
}

void I8275Config::I8275CharRow(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    long value = get8BitValue("8275CharRow");
    if (value == -1)  return;

    i8275Pointer->setCharRow((Byte)value-1, DO_NOT_SHOW_ADDRESS_TRACE);
}

void I8275Config::I8275VerticalRetrace(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    long value = get8BitValue("8275VerticalRetrace");
    if (value == -1)  return;

    i8275Pointer->setVerticalRetrace((Byte)value-1, DO_NOT_SHOW_ADDRESS_TRACE);
}

void I8275Config::I8275RowsFrame(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    long value = get8BitValue("8275RowsFrame");
    if (value == -1)  return;

    i8275Pointer->setRowsFrame((Byte)value-1, DO_NOT_SHOW_ADDRESS_TRACE);
}

void I8275Config::I8275Underline(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    long value = get8BitValue("8275Underline");
    if (value == -1)  return;

    i8275Pointer->setUnderline((Byte)value-1, DO_NOT_SHOW_ADDRESS_TRACE);
}

void I8275Config::I8275LinesRow(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    long value = get8BitValue("8275LinesRow");
    if (value == -1)  return;

    i8275Pointer->setLinesRow((Byte)value-1, DO_NOT_SHOW_ADDRESS_TRACE);
}

void I8275Config::I8275CursorFormat(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    long value = get8BitValue("8275CursorFormat");
    if (value == -1)  return;

    i8275Pointer->setCursorFormat((Byte)value, DO_NOT_SHOW_ADDRESS_TRACE);
}

void I8275Config::I8275HorizontalRetrace(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    long value = get8BitValue("8275HorizontalRetrace");
    if (value == -1)  return;

    lastI8275ConfigRegisterValueString[I8275_HORIZONTAL_RETRACE] = "x";
    i8275Pointer->setHorizontalRetrace((Byte)value/2-1, DO_NOT_SHOW_ADDRESS_TRACE);
}

void I8275Config::I8275BurstSpaceCode(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    long value = get8BitValue("8275BurstSpaceCode");
    if (value == -1)  return;

    lastI8275ConfigRegisterValueString[I8275_BURST_SPACE_CODE] = "x";
    i8275Pointer->setBurstSpaceCode((value+1)/8, DO_NOT_SHOW_ADDRESS_TRACE);
}

void I8275Config::I8275DmaCyclesBurst(wxCommandEvent& WXUNUSED(event))
{
    Byte const BB [8] = {0, 1, 1, 2, 2, 2, 2, 3};
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    long value = get8BitValue("8275DmaCyclesBurst");
    if (value == -1)  return;

    lastI8275ConfigRegisterValueString[I8275_DMA_CYCLES_BURST] = "x";
    value -= 1;
    value &= 0x7;
    
    lastI8275ConfigRegisterValueString[I8275_DMA_CYCLES_BURST] = "x";
    i8275Pointer->setDmaCyclesBurst(BB[value], DO_NOT_SHOW_ADDRESS_TRACE);
}

void I8275Config::I8275CursorChar(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    long value = get8BitValue("8275CursorChar");
    if (value == -1)  return;

    i8275Pointer->setCursorChar(value, DO_NOT_SHOW_ADDRESS_TRACE);
}

void I8275Config::I8275CursorRow(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showVideoNotRunning();
        return;
    }

    long value = get8BitValue("8275CursorRow");
    if (value == -1)  return;

    i8275Pointer->setCursorRow(value, DO_NOT_SHOW_ADDRESS_TRACE);
}

