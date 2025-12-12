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

BEGIN_EVENT_TABLE(Scn2672Config, Mc6845Config)
    EVT_TEXT_ENTER(XRCID("Scn2672InitReg"), Scn2672Config::Scn2672InitReg)
    EVT_TEXT_ENTER(XRCID("Scn2672Command"), Scn2672Config::Scn2672Command)
    EVT_TEXT_ENTER(XRCID("Scn2672ScreenStart"), Scn2672Config::Scn2672ScreenStart)
    EVT_TEXT_ENTER(XRCID("Scn2672Cursor"), Scn2672Config::Scn2672Cursor)
    EVT_TEXT_ENTER(XRCID("Scn2672Pointer"), Scn2672Config::Scn2672Pointer)
    EVT_TEXT_ENTER(XRCID("Scn2672Data"), Scn2672Config::Scn2672Data)
    EVT_TEXT_ENTER(XRCID("Scn2672R0"), Scn2672Config::Scn2672RegisterByte)
    EVT_TEXT_ENTER(XRCID("Scn2672R1"), Scn2672Config::Scn2672RegisterByte)
    EVT_TEXT_ENTER(XRCID("Scn2672R2"), Scn2672Config::Scn2672RegisterByte)
    EVT_TEXT_ENTER(XRCID("Scn2672R3"), Scn2672Config::Scn2672RegisterByte)
    EVT_TEXT_ENTER(XRCID("Scn2672R4"), Scn2672Config::Scn2672RegisterByte)
    EVT_TEXT_ENTER(XRCID("Scn2672R5"), Scn2672Config::Scn2672RegisterByte)
    EVT_TEXT_ENTER(XRCID("Scn2672R6"), Scn2672Config::Scn2672RegisterByte)
    EVT_TEXT_ENTER(XRCID("Scn2672R7"), Scn2672Config::Scn2672RegisterByte)
    EVT_TEXT_ENTER(XRCID("Scn2672R8"), Scn2672Config::Scn2672RegisterByte)
    EVT_TEXT_ENTER(XRCID("Scn2672R9"), Scn2672Config::Scn2672RegisterByte)
    EVT_TEXT_ENTER(XRCID("Scn2672RA"), Scn2672Config::Scn2672RegisterByte)
END_EVENT_TABLE()

wxString registerIdScn2672[] =
{
    "Scn2672R0",
    "Scn2672R1",
    "Scn2672R2",
    "Scn2672R3",
    "Scn2672R4",
    "Scn2672R5",
    "Scn2672R6",
    "Scn2672R7",
    "Scn2672R8",
    "Scn2672R9",
    "Scn2672RA",
    "Scn2672InitReg",
    "Scn2672Command",
    "Scn2672ScreenStart",
    "Scn2672Cursor",
    "Scn2672Pointer",
    "Scn2672Data",
    "Scn2672Interrupt",
    "Scn2672Status",
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
};

wxString selectorIdScn2672[] =
{
    "Scn2672Interlace",
};

wxString selectorFunctionScn2672[1][4] =
{ // false, true
    {"Interlace: disabled", "Interlace: enabled", "Interlace: forced", "Interlace: forced" },
};

Scn2672Config::Scn2672Config(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: Mc6845Config(title, pos, size, mode, dataDir, iniDir)
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

    XRCCTRL(*this, THIS_PANEL_NAME, wxPanel)->Hide();

    XRCCTRL(*this, "Scn2672IoGroupText", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "Scn2672InterlaceText", wxStaticText)->SetLabel("Interlace: disabled");
    XRCCTRL(*this, "Scn2672CharSizeText", wxStaticText)->SetLabel("Character size: 8x8");
  
    disableIoPortConfig(registerIdScn2672[SCN2672_INITIALIZATION_REGISTER]);
    disableIoPortConfig(registerIdScn2672[SCN2672_COMMAND]);
    disableIoPortConfig(registerIdScn2672[SCN2672_SCREEN_START]);
    disableIoPortConfig(registerIdScn2672[SCN2672_CURSOR]);
    disableIoPortConfig(registerIdScn2672[SCN2672_POINTER]);
    disableIoPortConfig(registerIdScn2672[SCN2672_DATA]);
    disableIoPortConfig(registerIdScn2672[SCN2672_INTERRUPT]);
    disableIoPortConfig(registerIdScn2672[SCN2672_STATUS]);
}

void Scn2672Config::crt8002ConfigInit()
{
    computerConfiguration.crt8002Configuration.attribute = init_IoPort();
    computerConfiguration.crt8002Configuration.attributeScreen1 = init_IoPort();
    
    computerConfiguration.crt8002Configuration.reverse.bitNumber = -1;
    computerConfiguration.crt8002Configuration.reverse.reversePolarity = 0;
    computerConfiguration.crt8002Configuration.blink.bitNumber = -1;
    computerConfiguration.crt8002Configuration.blink.reversePolarity = 0;
    computerConfiguration.crt8002Configuration.graphic_ms0.bitNumber = -1;
    computerConfiguration.crt8002Configuration.graphic_ms0.reversePolarity = 0;
    computerConfiguration.crt8002Configuration.graphic_ms1.bitNumber = -1;
    computerConfiguration.crt8002Configuration.graphic_ms1.reversePolarity = 0;
    computerConfiguration.crt8002Configuration.underline.bitNumber = -1;
    computerConfiguration.crt8002Configuration.underline.reversePolarity = 0;
    computerConfiguration.crt8002Configuration.strikeThru.bitNumber = -1;
    computerConfiguration.crt8002Configuration.strikeThru.reversePolarity = 0;
    computerConfiguration.crt8002Configuration.blank.bitNumber = -1;
    computerConfiguration.crt8002Configuration.blank.reversePolarity = 0;
    
    computerConfiguration.crt8002Configuration.underlineLine1 = -1;
    computerConfiguration.crt8002Configuration.underlineLine2 = -1;
    computerConfiguration.crt8002Configuration.strikeThruLine1 = -1;
    computerConfiguration.crt8002Configuration.strikeThruLine2 = -1;
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
    wxString color, position, iogroup, label, labelDetails;
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
                    setIoPortConfig(computerConfiguration.scn2672Configuration.screenStart, registerIdScn2672[SCN2672_SCREEN_START], registerFunctionScn2672[SCN2672_SCREEN_START], "B", true);
                }
                if (child->GetAttribute("type") == "cursor")
                {
                    computerConfiguration.scn2672Configuration.cursor = parseXml_IoPort(*child);
                    setIoPortConfig(computerConfiguration.scn2672Configuration.cursor, registerIdScn2672[SCN2672_CURSOR], registerFunctionScn2672[SCN2672_CURSOR], "B", true);
                }
                if (child->GetAttribute("type") == "data")
                {
                    computerConfiguration.scn2672Configuration.data = parseXml_IoPort(*child);
                    setIoPortConfig(computerConfiguration.scn2672Configuration.data, registerIdScn2672[SCN2672_DATA], registerFunctionScn2672[SCN2672_DATA], "B");
                }
            break;
                
            case TAG_OUT:
                if (child->GetAttribute("type") == "ir")
                {
                    computerConfiguration.scn2672Configuration.initializationRegister = parseXml_IoPort(*child);
                    setIoPortConfig(computerConfiguration.scn2672Configuration.initializationRegister, registerIdScn2672[SCN2672_INITIALIZATION_REGISTER], registerFunctionScn2672[SCN2672_INITIALIZATION_REGISTER], "O");
                }
                if (child->GetAttribute("type") == "command")
                {
                    computerConfiguration.scn2672Configuration.command = parseXml_IoPort(*child);
                    setIoPortConfig(computerConfiguration.scn2672Configuration.command, registerIdScn2672[SCN2672_COMMAND], registerFunctionScn2672[SCN2672_COMMAND], "O");
                }
                if (child->GetAttribute("type") == "pointer")
                {
                    computerConfiguration.scn2672Configuration.pointer = parseXml_IoPort(*child);
                    setIoPortConfig(computerConfiguration.scn2672Configuration.pointer, registerIdScn2672[SCN2672_POINTER], registerFunctionScn2672[SCN2672_POINTER], "O", true);
                }
            break;
       
            case TAG_IN:
                if (child->GetAttribute("type") == "status")
                {
                    computerConfiguration.scn2672Configuration.status = parseXml_IoPort(*child);
                    setIoPortConfig(computerConfiguration.scn2672Configuration.status, registerIdScn2672[SCN2672_STATUS], registerFunctionScn2672[SCN2672_STATUS], "I");
                }
                if (child->GetAttribute("type") == "interrupt")
                {
                    computerConfiguration.scn2672Configuration.interrupt = parseXml_IoPort(*child);
                    setIoPortConfig(computerConfiguration.scn2672Configuration.interrupt, registerIdScn2672[SCN2672_INTERRUPT], registerFunctionScn2672[SCN2672_INTERRUPT], "I");
                }
            break;

            case TAG_INTERLACE:
                computerConfiguration.interlace_ = true;
                XRCCTRL(*this, "Scn2672InterlaceText", wxStaticText)->SetLabel("Interlace: forced");
            break;
                
            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.scn2672Configuration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.scn2672Configuration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
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
                label.Printf("Character size: %dx%d", (int)width, (int)height);
                XRCCTRL(*this, "Scn2672CharSizeText", wxStaticText)->SetLabel(label);
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

void Scn2672Config::parseXml_Crt8002Video(wxXmlNode &node)
{
    wxString tagList[]=
    {
        "font",
        "dirname",
        "io",
        "in",
        "iogroup",
        "reverse",
        "blink",
        "graphic_ms0",
        "graphic_ms1",
        "underline",
        "strike_thru",
        "blank",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_FONT,
        TAG_DIRNAME,
        TAG_IO,
        TAG_IN,
        TAG_IOGROUP,
        TAG_REVERSE,
        TAG_BLINK,
        TAG_GRAPHIC_MS0,
        TAG_GRAPHIC_MS1,
        TAG_UNDERLINE,
        TAG_STRIKE_THRU,
        TAG_BLANK,
        TAG_COMMENT,
        TAG_UNDEFINED
    };

    int tagTypeInt;
    wxString iogroup;
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
                if (child->GetAttribute("type") == "attribute")
                    computerConfiguration.crt8002Configuration.attribute = parseXml_IoPort(*child);
            break;
                       
            case TAG_IN:
                if (child->GetAttribute("type") == "attribute_screen1")
                    computerConfiguration.crt8002Configuration.attributeScreen1 = parseXml_IoPort(*child);
            break;

            case TAG_REVERSE:
                computerConfiguration.crt8002Configuration.reverse.bitNumber = (int)parseXml_Number(*child);
                if (child->GetAttribute("pol") == "rev")
                    computerConfiguration.crt8002Configuration.reverse.reversePolarity = 1;
            break;

            case TAG_BLINK:
                computerConfiguration.crt8002Configuration.blink.bitNumber = (int)parseXml_Number(*child);
                if (child->GetAttribute("pol") == "rev")
                    computerConfiguration.crt8002Configuration.blink.reversePolarity = 1;
            break;

            case TAG_GRAPHIC_MS0:
                computerConfiguration.crt8002Configuration.graphic_ms0.bitNumber = (int)parseXml_Number(*child);
                if (child->GetAttribute("pol") == "rev")
                    computerConfiguration.crt8002Configuration.graphic_ms0.reversePolarity = 1;
            break;

            case TAG_GRAPHIC_MS1:
                computerConfiguration.crt8002Configuration.graphic_ms1.bitNumber = (int)parseXml_Number(*child);
                if (child->GetAttribute("pol") == "rev")
                    computerConfiguration.crt8002Configuration.graphic_ms1.reversePolarity = 1;
            break;

            case TAG_UNDERLINE:
                computerConfiguration.crt8002Configuration.underline.bitNumber = (int)parseXml_Number(*child);
                if (child->GetAttribute("pol") == "rev")
                    computerConfiguration.crt8002Configuration.underline.reversePolarity = 1;
                if (child->HasAttribute("line1"))
                    computerConfiguration.crt8002Configuration.underlineLine1 = (int)parseXml_Number(*child, "line1");
                if (child->HasAttribute("line2"))
                    computerConfiguration.crt8002Configuration.underlineLine2 = (int)parseXml_Number(*child, "line2");
            break;

            case TAG_STRIKE_THRU:
                computerConfiguration.crt8002Configuration.strikeThru.bitNumber = (int)parseXml_Number(*child);
                if (child->GetAttribute("pol") == "rev")
                    computerConfiguration.crt8002Configuration.strikeThru.reversePolarity = 1;
                if (child->HasAttribute("line1"))
                    computerConfiguration.crt8002Configuration.strikeThruLine1 = (int)parseXml_Number(*child, "line1");
                if (child->HasAttribute("line2"))
                    computerConfiguration.crt8002Configuration.strikeThruLine2 = (int)parseXml_Number(*child, "line2");
            break;

            case TAG_BLANK:
                computerConfiguration.crt8002Configuration.blank.bitNumber = (int)parseXml_Number(*child);
                if (child->GetAttribute("pol") == "rev")
                    computerConfiguration.crt8002Configuration.blank.reversePolarity = 1;
            break;

            case TAG_FONT:
                computerConfiguration.characterRomConfiguration.fileName = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                computerConfiguration.characterRomConfiguration.directory = dataDir_ + child->GetNodeContent();
                if (computerConfiguration.characterRomConfiguration.directory.Right(1) != pathSeparator_)
                    computerConfiguration.characterRomConfiguration.directory += pathSeparator_;
            break;
                
            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.crt8002Configuration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.crt8002Configuration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
                XRCCTRL(*this,"Scn2672IoGroupText", wxStaticText)->SetLabel(p_Main->getGroupMessageXml(&computerConfiguration.crt8002Configuration.ioGroupVector));
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

void Scn2672Config::updateScn2672Panel()
{
    wxString buffer;

    if (computerConfiguration.scn2672Configuration.defined)
    {
        for (size_t registerNumber = 0; registerNumber<SCN2672_NUMBER_OF_REGISTERS; registerNumber++)
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

int Scn2672Config::setScn2672Register(int registerNumber, Word value, int showTrace)
{
    scn2672ConfigRegisterValueString[registerNumber].Printf("%04X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdScn2672[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showTraceText(registerFunctionScn2672[registerNumber], scn2672ConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

int Scn2672Config::readScn2672Register(int registerNumber, Byte WXUNUSED(value), int showTrace)
{
    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdScn2672[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showTraceTextRead(registerFunctionScn2672[registerNumber], scn2672ConfigRegisterValueString[registerNumber], showTrace);
        
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
        showTraceTextRead(registerFunctionScn2672[registerNumber], scn2672ConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
      }
    return showTrace;
}

int Scn2672Config::setScn2672Register(int registerNumber, Byte value, int showTrace)
{
    scn2672ConfigRegisterValueString[registerNumber].Printf("%02X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdScn2672[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showTraceText(registerFunctionScn2672[registerNumber], scn2672ConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
      }
    return showTrace;
}

int Scn2672Config::setScn2672RegisterNibble(int registerNumber, Byte value, int showTrace)
{
    scn2672ConfigRegisterValueString[registerNumber].Printf("%01X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdScn2672[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showTraceText(registerFunctionScn2672[registerNumber], scn2672ConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
      }
    return showTrace;
}

bool Scn2672Config::isScn2672TraceChecked(int registerNumber)
{
    return XRCCTRL(*this, registerIdScn2672[registerNumber]+"Trace", wxCheckBox)->IsChecked();
}

void Scn2672Config::Scn2672InitReg(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    long value = get8BitValue("Scn2672InitReg") & 0xf;
    if (value == -1)  return;

    scn2672Pointer->writeInitializationRegisterScn2672(value, DO_NOT_SHOW_ADDRESS_TRACE);
}

void Scn2672Config::Scn2672Command(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    long value = get8BitValue("Scn2672Command");
    if (value == -1)  return;

    scn2672Pointer->writeCommandScn2672(value, DO_NOT_SHOW_ADDRESS_TRACE);
}

void Scn2672Config::Scn2672ScreenStart(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    long value = get16BitValue("Scn2672ScreenStart");
    if (value == -1)  return;

    scn2672Pointer->writeScreenStartScn2672(value, DO_NOT_SHOW_ADDRESS_TRACE);
}

void Scn2672Config::Scn2672Cursor(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    long value = get16BitValue("Scn2672Cursor");
    if (value == -1)  return;

    scn2672Pointer->writeCursorScn2672(value, DO_NOT_SHOW_ADDRESS_TRACE);
}

void Scn2672Config::Scn2672Pointer(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    long value = get16BitValue("Scn2672Pointer");
    if (value == -1)  return;

    scn2672Pointer->writePointerScn2672(value, DO_NOT_SHOW_ADDRESS_TRACE);
}

void Scn2672Config::Scn2672Data(wxCommandEvent& WXUNUSED(event))
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    long value = get8BitValue("Scn2672Data");
    if (value == -1)  return;

    scn2672Pointer->writeDataScn2672(value, DO_NOT_SHOW_ADDRESS_TRACE);
}

void Scn2672Config::Scn2672RegisterByte(wxCommandEvent&event)
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    wxString idReference = wxWindow::FindWindowById(event.GetId())->GetName();
    wxString buttonNumber = idReference.Right(1);
    
    long number;
    if (!buttonNumber.ToLong(&number, 16))
        return;

    long value = get8BitValue("Scn2672R" + buttonNumber);
    if (value == -1)  return;

    scn2672Pointer->writeInitializationRegisterScn2672(number, value, DO_NOT_SHOW_ADDRESS_TRACE);
}

int Scn2672Config::setScn2672SelectorValue(int selectorNumber, int selectorValue, int showTrace)
{
    scn2672ConfigSelector[selectorNumber] = selectorValue;
    updateScn2672ConfigSelector[selectorNumber] = true;

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,selectorIdScn2672[selectorNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showTraceText(selectorFunctionScn2672[selectorNumber][scn2672ConfigSelector[selectorNumber]], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

