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
#include "scn2672_config.h"
#include "wx/tglbtn.h"
#include "wx/xrc/xmlres.h"

#define THIS_PANEL_NAME "PanelVideoScn2672"

BEGIN_EVENT_TABLE(Scn2672Config, Crt8002Config)
    EVT_TEXT_ENTER(XRCID("Scn2672R02"), Scn2672Config::Scn2672Register)
    EVT_TEXT_ENTER(XRCID("Scn2672R03"), Scn2672Config::Scn2672Register)
    EVT_TEXT_ENTER(XRCID("Scn2672R04"), Scn2672Config::Scn2672Register)
    EVT_TEXT_ENTER(XRCID("Scn2672R05"), Scn2672Config::Scn2672Register)
    EVT_TEXT_ENTER(XRCID("Scn2672R06"), Scn2672Config::Scn2672Register)
    EVT_TEXT_ENTER(XRCID("Scn2672R07"), Scn2672Config::Scn2672Register)
    EVT_TEXT_ENTER(XRCID("Scn2672R08"), Scn2672Config::Scn2672Register)
    EVT_TEXT_ENTER(XRCID("Scn2672R09"), Scn2672Config::Scn2672Register)
    EVT_TEXT_ENTER(XRCID("Scn2672R10"), Scn2672Config::Scn2672Register)
    EVT_TEXT_ENTER(XRCID("Scn2672InitReg11"), Scn2672Config::Scn2672Register)
    EVT_TEXT_ENTER(XRCID("Scn2672Command12"), Scn2672Config::Scn2672Register)
    EVT_TEXT_ENTER(XRCID("Scn2672ScreenStart13"), Scn2672Config::Scn2672Register)
    EVT_TEXT_ENTER(XRCID("Scn2672Cursor14"), Scn2672Config::Scn2672Register)
    EVT_TEXT_ENTER(XRCID("Scn2672Pointer15"), Scn2672Config::Scn2672Register)
    EVT_TEXT_ENTER(XRCID("Scn2672Data16"), Scn2672Config::Scn2672Register)
    EVT_TEXT_ENTER(XRCID("Scn2672Scanlines19"), Scn2672Config::Scn2672Register)
    EVT_TEXT_ENTER(XRCID("Scn2672CharWidth20"), Scn2672Config::Scn2672Register)
    EVT_TEXT_ENTER(XRCID("Scn2672EqualizingConstant21"), Scn2672Config::Scn2672Register)
    EVT_TEXT_ENTER(XRCID("Scn2672HorizontalSync22"), Scn2672Config::Scn2672Register)
    EVT_TEXT_ENTER(XRCID("Scn2672HorizontalBackPorch23"), Scn2672Config::Scn2672Register)
    EVT_TEXT_ENTER(XRCID("Scn2672VerticalFrontPorch24"), Scn2672Config::Scn2672Register)
    EVT_TEXT_ENTER(XRCID("Scn2672VerticalBackPorch25"), Scn2672Config::Scn2672Register)
    EVT_TEXT_ENTER(XRCID("Scn2672ScreenRows26"), Scn2672Config::Scn2672Register)
    EVT_TEXT_ENTER(XRCID("Scn2672CharPerRow27"), Scn2672Config::Scn2672Register)
    EVT_TEXT_ENTER(XRCID("Scn2672FirstCursorLine28"), Scn2672Config::Scn2672Register)
    EVT_TEXT_ENTER(XRCID("Scn2672LastCursorLine29"), Scn2672Config::Scn2672Register)
    EVT_TEXT_ENTER(XRCID("Scn2672UnderLine30"), Scn2672Config::Scn2672Register)
    EVT_TEXT_ENTER(XRCID("Scn2672DisplayBufferStart31"), Scn2672Config::Scn2672Register)
    EVT_TEXT_ENTER(XRCID("Scn2672DisplayBufferEnd32"), Scn2672Config::Scn2672Register)
    EVT_TEXT_ENTER(XRCID("Scn2672SplitScreenIntRow33"), Scn2672Config::Scn2672Register)
END_EVENT_TABLE()

wxString registerIdScn2672[] =
{
    "Scn2672R00",
    "Scn2672R01",
    "Scn2672R02",
    "Scn2672R03",
    "Scn2672R04",
    "Scn2672R05",
    "Scn2672R06",
    "Scn2672R07",
    "Scn2672R08",
    "Scn2672R09",
    "Scn2672R10",
    "Scn2672InitReg11",
    "Scn2672Command12",
    "Scn2672ScreenStart13",
    "Scn2672Cursor14",
    "Scn2672Pointer15",
    "Scn2672Data16",
    "Scn2672Interrupt",
    "Scn2672Status",
    "Scn2672Scanlines19",
    "Scn2672CharWidth20",
    "Scn2672EqualizingConstant21",
    "Scn2672HorizontalSync22",
    "Scn2672HorizontalBackPorch23",
    "Scn2672VerticalFrontPorch24",
    "Scn2672VerticalBackPorch25",
    "Scn2672ScreenRows26",
    "Scn2672CharPerRow27",
    "Scn2672FirstCursorLine28",
    "Scn2672LastCursorLine29",
    "Scn2672UnderLine30",
    "Scn2672DisplayBufferStart31",
    "Scn2672DisplayBufferEnd32",
    "Scn2672SplitScreenIntRow33",
};

wxString registerFunctionScn2672[] =
{
    "IR0",
    "IR1",
    "IR2",
    "IR3",
    "IR4",
    "IR5",
    "IR6",
    "IR7",
    "IR8",
    "IR9",
    "IR10",
    "Initialization",
    "Command",
    "Screen start",
    "Cursor",
    "Pointer",
    "Data",
    "Interrupt",
    "Status",
    "Lines/row",
    "Char width",
    "Eq. const.",
    "Horiz. sync",
    "Horiz. b. porch",
    "Vert. f. porch",
    "Vert. b. porch",
    "Rows/screen",
    "Char/row ",
    "1st cursor line",
    "Last cursor line",
    "Underline pos",
    "Buffer start",
    "Buffer end",
    "Split screen",
};

wxString selectorIdScn2672[] =
{
    "Scn2672Interlace",
    "Scn2672CursorBlinkRate",
    "Scn2672CursorBlink",
    "Scn2672CharBlinkRate",
};

wxString selectorFunctionScn2672[5][4] =
{ // false, true
    {"Interlace: disabled", "Interlace: enabled", "Interlace: forced", "Interlace: forced" },
    {"Cursor blink: 1/16", "Cursor blink: 1/32", "", "" },
    {"Cursor blink: off", "Cursor blink: on", "", "" },
    {"Char blink: 1/16", "Char blink: 1/32", "", "" },
    {"", "", "", "" },
};

Scn2672Config::Scn2672Config(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: Crt8002Config(title, pos, size, mode, dataDir, iniDir)
{
}

Scn2672Config::~Scn2672Config()
{
}

void Scn2672Config::scn2672ConfigInit()
{
    for (size_t registerNumber = 0; registerNumber<SCN2672_NUMBER_OF_REGISTERS; registerNumber++)
    {
        scn2672ConfigRegisterValueString[registerNumber] = "";
        lastScn2672ConfigRegisterValueString[registerNumber] = "x";
    }

    computerConfiguration.scn2672Configuration.defined = false;

    computerConfiguration.scn2672Configuration.defaultX = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    computerConfiguration.scn2672Configuration.defaultY = mainWindowY_;
    computerConfiguration.scn2672Configuration.initializationRegister = init_IoPort();
    computerConfiguration.scn2672Configuration.command = init_IoPort();
    computerConfiguration.scn2672Configuration.screenStart = init_IoPort();
    computerConfiguration.scn2672Configuration.cursor = init_IoPort();
    computerConfiguration.scn2672Configuration.pointer = init_IoPort();
    computerConfiguration.scn2672Configuration.data = init_IoPort();
    computerConfiguration.scn2672Configuration.status = init_IoPort();
    computerConfiguration.scn2672Configuration.interrupt = init_IoPort();
    computerConfiguration.scn2672Configuration.charSize.x = 7;
    computerConfiguration.scn2672Configuration.charSize.y = 8;
    computerConfiguration.scn2672Configuration.screenSize.x = 40;
    computerConfiguration.scn2672Configuration.screenSize.y = 24;
    computerConfiguration.scn2672Configuration.videoMode = false;
    computerConfiguration.scn2672Configuration.ioGroupVector.clear();
	computerConfiguration.scn2672Configuration.videoClock = 2.7;
    computerConfiguration.scn2672Configuration.picInterrupt = 0;
    computerConfiguration.scn2672Configuration.xScale = 1.7;

    if (!mode_.gui)
        return;

    XRCCTRL(*this, THIS_PANEL_NAME, wxPanel)->Hide();

    XRCCTRL(*this, "Scn2672IoGroupText", wxStaticText)->SetLabel("");
    XRCCTRL(*this, selectorIdScn2672[SCN2672_SEL_INTERLACE] + "Text", wxStaticText)->SetLabel(selectorFunctionScn2672[SCN2672_SEL_INTERLACE][0]);
    XRCCTRL(*this, selectorIdScn2672[SCN2672_SEL_CURSOR_BLINK_RATE] + "Text", wxStaticText)->SetLabel(selectorFunctionScn2672[SCN2672_SEL_CURSOR_BLINK_RATE][0]);
    XRCCTRL(*this, selectorIdScn2672[SCN2672_SEL_CURSOR_BLINK] + "Text", wxStaticText)->SetLabel(selectorFunctionScn2672[SCN2672_SEL_CURSOR_BLINK][0]);
    XRCCTRL(*this, selectorIdScn2672[SCN2672_SEL_CHAR_BLINK_RATE] + "Text", wxStaticText)->SetLabel(selectorFunctionScn2672[SCN2672_SEL_CHAR_BLINK_RATE][0]);
   
    disableIoPortConfig(registerIdScn2672[SCN2672_INITIALIZATION_REGISTER]);
    disableIoPortConfig(registerIdScn2672[SCN2672_COMMAND]);
    disableIoPortConfig(registerIdScn2672[SCN2672_SCREEN_START]);
    disableIoPortConfig(registerIdScn2672[SCN2672_CURSOR]);
    disableIoPortConfig(registerIdScn2672[SCN2672_POINTER]);
    disableIoPortConfig(registerIdScn2672[SCN2672_DATA]);
    disableIoPortConfig(registerIdScn2672[SCN2672_INTERRUPT]);
    disableIoPortConfig(registerIdScn2672[SCN2672_STATUS]);
    
    setIoPortConfig(registerIdScn2672[SCN2672_SCANLINES], registerFunctionScn2672[SCN2672_SCANLINES], "O", "", "");
    setIoPortConfig(registerIdScn2672[SCN2672_CHAR_WIDTH], registerFunctionScn2672[SCN2672_CHAR_WIDTH], "O", "", "");
    setIoPortConfig(registerIdScn2672[SCN2672_EQUALIZING_CONSTANT], registerFunctionScn2672[SCN2672_EQUALIZING_CONSTANT], "O", "", "");
    setIoPortConfig(registerIdScn2672[SCN2672_HORIZONTAL_SYNC_WIDTH], registerFunctionScn2672[SCN2672_HORIZONTAL_SYNC_WIDTH], "O", "", "");
    setIoPortConfig(registerIdScn2672[SCN2672_HORIZONTAL_BACK_PORCH], registerFunctionScn2672[SCN2672_HORIZONTAL_BACK_PORCH], "O", "", "");
    setIoPortConfig(registerIdScn2672[SCN2672_VERTICAL_FRONT_PORCH], registerFunctionScn2672[SCN2672_VERTICAL_FRONT_PORCH], "O", "", "");
    setIoPortConfig(registerIdScn2672[SCN2672_VERTICAL_BACK_PORCH], registerFunctionScn2672[SCN2672_VERTICAL_BACK_PORCH], "O", "", "");
}

void Scn2672Config::parseXml_Scn2672Video(wxXmlNode &node)
{
    videoConfigUsed(THIS_PANEL_NAME);

    long width, height;

    if (!computerConfiguration.scn2672Configuration.defined)
    {
        computerConfiguration.scn2672Configuration.videoNumber = computerConfiguration.numberOfVideoTypes_;
        computerConfiguration.numberOfVideoTypes_++;
    }

    computerConfiguration.scn2672Configuration.defined = true;
    computerConfiguration.zoom_[computerConfiguration.scn2672Configuration.videoNumber] = "2.00";
    computerConfiguration.videoName_[computerConfiguration.scn2672Configuration.videoNumber] = "SCN2672";

    wxString tagList[]=
    {
        "io",
        "out",
        "in",
        "interlace",
        "iogroup",
        "zoom",
        "screen",
        "char",
        "border",
        "pos",
        "xscale",
        "color",
        "video_mode",
        "int",
        "clock",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_IO,
        TAG_OUT,
        TAG_IN,
        TAG_INTERLACE,
        TAG_IOGROUP,
        TAG_ZOOM,
        TAG_SCREEN,
        TAG_CHAR,
        TAG_BORDER,
        TAG_POS,
        TAG_XSCALE,
        TAG_COLOR,
        TAG_VIDEO_MODE,
        TAG_INTERRUPT,
        TAG_CLOCK,
        TAG_COMMENT,
        TAG_UNDEFINED
    };

    int tagTypeInt;
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
                if (child->GetAttribute("type") == "screen_start")
                {
                    computerConfiguration.scn2672Configuration.screenStart = parseXml_IoPort(*child);
                    setIoPortConfig(computerConfiguration.scn2672Configuration.screenStart, registerIdScn2672[SCN2672_SCREEN_START], registerFunctionScn2672[SCN2672_SCREEN_START], "B", true, "");
                }
                if (child->GetAttribute("type") == "cursor")
                {
                    computerConfiguration.scn2672Configuration.cursor = parseXml_IoPort(*child);
                    setIoPortConfig(computerConfiguration.scn2672Configuration.cursor, registerIdScn2672[SCN2672_CURSOR], registerFunctionScn2672[SCN2672_CURSOR], "B", true, "");
                }
                if (child->GetAttribute("type") == "data")
                {
                    computerConfiguration.scn2672Configuration.data = parseXml_IoPort(*child);
                    setIoPortConfig(computerConfiguration.scn2672Configuration.data, registerIdScn2672[SCN2672_DATA], registerFunctionScn2672[SCN2672_DATA], "B", false, "");
                }
            break;
                
            case TAG_OUT:
                if (child->GetAttribute("type") == "ir")
                {
                    computerConfiguration.scn2672Configuration.initializationRegister = parseXml_IoPort(*child);
                    setIoPortConfig(computerConfiguration.scn2672Configuration.initializationRegister, registerIdScn2672[SCN2672_INITIALIZATION_REGISTER], registerFunctionScn2672[SCN2672_INITIALIZATION_REGISTER], "O", false, "");
                }
                if (child->GetAttribute("type") == "command")
                {
                    computerConfiguration.scn2672Configuration.command = parseXml_IoPort(*child);
                    setIoPortConfig(computerConfiguration.scn2672Configuration.command, registerIdScn2672[SCN2672_COMMAND], registerFunctionScn2672[SCN2672_COMMAND], "O", false, "");
                }
                if (child->GetAttribute("type") == "pointer")
                {
                    computerConfiguration.scn2672Configuration.pointer = parseXml_IoPort(*child);
                    setIoPortConfig(computerConfiguration.scn2672Configuration.pointer, registerIdScn2672[SCN2672_POINTER], registerFunctionScn2672[SCN2672_POINTER], "O", true, "");
                }
            break;
       
            case TAG_IN:
                if (child->GetAttribute("type") == "status")
                {
                    computerConfiguration.scn2672Configuration.status = parseXml_IoPort(*child);
                    setIoPortConfig(computerConfiguration.scn2672Configuration.status, registerIdScn2672[SCN2672_STATUS], registerFunctionScn2672[SCN2672_STATUS], "I", false, "");
                }
                if (child->GetAttribute("type") == "interrupt")
                {
                    computerConfiguration.scn2672Configuration.interrupt = parseXml_IoPort(*child);
                    setIoPortConfig(computerConfiguration.scn2672Configuration.interrupt, registerIdScn2672[SCN2672_INTERRUPT], registerFunctionScn2672[SCN2672_INTERRUPT], "I", false, "");
                }
            break;

            case TAG_INTERLACE:
                computerConfiguration.interlace_ = true;
                if (mode_.gui)
                    XRCCTRL(*this, "Scn2672InterlaceText", wxStaticText)->SetLabel("Interlace: forced");
            break;
                
            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.scn2672Configuration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.scn2672Configuration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
                if (mode_.gui)
                    XRCCTRL(*this,"Scn2672IoGroupText", wxStaticText)->SetLabel(p_Main->getGroupMessageXml(&computerConfiguration.scn2672Configuration.ioGroupVector));
            break;

            case TAG_ZOOM:
                computerConfiguration.zoom_[computerConfiguration.scn2672Configuration.videoNumber] = child->GetNodeContent();
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
                    computerConfiguration.scn2672Configuration.charSize.x = (int)width;
                    computerConfiguration.scn2672Configuration.charSize.y = (int)height;
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
                    computerConfiguration.scn2672Configuration.screenSize.x = (int)width;
                    computerConfiguration.scn2672Configuration.screenSize.y = (int)height;
                }
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
                    screenInfo.borderX[VIDEOSCN2672] = (int)width;
                    screenInfo.borderY[VIDEOSCN2672] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += computerConfiguration.scn2672Configuration.defaultX;
                if (ypos != -1)
                    ypos += computerConfiguration.scn2672Configuration.defaultY;
                computerConfiguration.scn2672Configuration.defaultX = xpos;
                computerConfiguration.scn2672Configuration.defaultY = ypos;
            break;

            case TAG_XSCALE:
                screenInfo.borderX[VIDEOSCN2672] = (int)screenInfo.borderX[VIDEOSCN2672] * computerConfiguration.scn2672Configuration.xScale;
                computerConfiguration.scn2672Configuration.xScale = getDouble(child->GetNodeContent(), childName, -1, "", false);
                screenInfo.borderX[VIDEOSCN2672] = (int)screenInfo.borderX[VIDEOSCN2672] / computerConfiguration.scn2672Configuration.xScale;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "fore")
                    screenInfo.defaultColour[COL_SCN2672_FORE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back")
                    screenInfo.defaultColour[COL_SCN2672_BACK].Printf("#%02X%02X%02X", red, green, blue);
            break;
                
            case TAG_VIDEO_MODE:
            	computerConfiguration.scn2672Configuration.videoMode = true;
            break;
            
            case TAG_INTERRUPT:
                computerConfiguration.scn2672Configuration.picInterrupt =  (int)parseXml_Number(*child) - 1;
            break;

            case TAG_CLOCK:
                computerConfiguration.scn2672Configuration.videoClock = getDouble(child->GetNodeContent(), childName, 500, "500", false);
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
    currentInterLace_ = computerConfiguration.interlace_;
}

void Scn2672Config::updateScn2672Panel()
{
    if (computerConfiguration.scn2672Configuration.defined)
    {
        for (size_t registerNumber = SCN2672_INITIALIZATION_REGISTER; registerNumber<SCN2672_NUMBER_OF_REGISTERS; registerNumber++)
        {
            if (scn2672ConfigRegisterValueString[registerNumber] != lastScn2672ConfigRegisterValueString[registerNumber])
            {
                p_Main->setTextCtrl(registerIdScn2672[registerNumber], scn2672ConfigRegisterValueString[registerNumber]);
                lastScn2672ConfigRegisterValueString[registerNumber] = scn2672ConfigRegisterValueString[registerNumber];
            }
        }
    }
    if (currentInterLace_ != computerConfiguration.interlace_)
    {
        currentInterLace_ = computerConfiguration.interlace_;
        updateScn2672ConfigSelector[SCN2672_SEL_INTERLACE] = true;
        if (computerConfiguration.interlace_)
            scn2672ConfigSelector[SCN2672_SEL_INTERLACE] |= 0x2;
        else
            scn2672ConfigSelector[SCN2672_SEL_INTERLACE] &= 0x1;
    }
    for (size_t selectorNumber = 0; selectorNumber<SCN2672_NUMBER_OF_SELECTORS; selectorNumber++)
    {
        if (updateScn2672ConfigSelector[selectorNumber])
        {
            XRCCTRL(*this, selectorIdScn2672[selectorNumber] + "Text", wxStaticText)->SetLabel(selectorFunctionScn2672[selectorNumber][scn2672ConfigSelector[selectorNumber]]);
            updateScn2672ConfigSelector[selectorNumber] = false;
        }
    }
}

int Scn2672Config::readScn2672Register(int registerNumber, Byte WXUNUSED(value), int showTrace)
{
    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdScn2672[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showVideoTraceTextRead(registerFunctionScn2672[registerNumber], scn2672ConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

int Scn2672Config::readScn2672RegisterSetData(int registerNumber, Byte value, int showTrace)
{
    scn2672ConfigRegisterValueString[registerNumber].Printf("%02X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdScn2672[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showVideoTraceTextRead(registerFunctionScn2672[registerNumber], scn2672ConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
      }
    return showTrace;
}

int Scn2672Config::setScn2672Register(int registerNumber, int showTrace)
{
    lastScn2672ConfigRegisterValueString[registerNumber] = "";
    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdScn2672[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showVideoTraceText(registerFunctionScn2672[registerNumber], scn2672ConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

int Scn2672Config::setScn2672RegisterWord(int registerNumber, Word value, int showTrace)
{
    scn2672ConfigRegisterValueString[registerNumber].Printf("%04X", value);
    return setScn2672Register(registerNumber, showTrace);
}

int Scn2672Config::setScn2672Register12Bit(int registerNumber, Word value, int showTrace)
{
    scn2672ConfigRegisterValueString[registerNumber].Printf("%03X", value);
    return setScn2672Register(registerNumber, showTrace);
}

int Scn2672Config::setScn2672RegisterByte(int registerNumber, Byte value, int showTrace)
{
    scn2672ConfigRegisterValueString[registerNumber].Printf("%02X", value);
    return setScn2672Register(registerNumber, showTrace);
}

int Scn2672Config::setScn2672RegisterNibble(int registerNumber, Byte value, int showTrace)
{
    scn2672ConfigRegisterValueString[registerNumber].Printf("%01X", value);
    return setScn2672Register(registerNumber, showTrace);
}

bool Scn2672Config::isScn2672TraceChecked(int registerNumber)
{
    if (!mode_.gui)
        return false;
    
    return XRCCTRL(*this, registerIdScn2672[registerNumber]+"Trace", wxCheckBox)->IsChecked();
}

void Scn2672Config::Scn2672Register(wxCommandEvent&event)
{
    long number = getButtonNumber(wxWindow::FindWindowById(event.GetId())->GetName());
    if (number == -1)  return;

    long value = getVideoRegisterValue(registerIdScn2672[number]);
    if (value == -1)  return;

    scn2672Pointer->writeRegisterScn2672(number, value, DO_NOT_SHOW_ADDRESS_TRACE);
}

int Scn2672Config::setScn2672SelectorValue(int selectorNumber, int selectorValue, int showTrace)
{
    scn2672ConfigSelector[selectorNumber] = selectorValue;
    updateScn2672ConfigSelector[selectorNumber] = true;

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,selectorIdScn2672[selectorNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showVideoTraceText(selectorFunctionScn2672[selectorNumber][scn2672ConfigSelector[selectorNumber]], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

