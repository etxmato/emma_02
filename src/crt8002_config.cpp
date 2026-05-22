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
#include "crt8002_config.h"
#include "wx/tglbtn.h"
#include "wx/xrc/xmlres.h"

#define THIS_PANEL_NAME "PanelVideoCrt8002"

BEGIN_EVENT_TABLE(Crt8002Config, Mc6845Config)
    EVT_TEXT_ENTER(XRCID("Crt8002Attrbute_00"), Crt8002Config::Crt8002Register)
    EVT_TEXT_ENTER(XRCID("Crt8002AttrbuteScreen1_01"), Crt8002Config::Crt8002Register)
    EVT_TEXT_ENTER(XRCID("Crt8002Reverse_02"), Crt8002Config::Crt8002Register)
    EVT_TEXT_ENTER(XRCID("Crt8002Blink_03"), Crt8002Config::Crt8002Register)
    EVT_TEXT_ENTER(XRCID("Crt8002GraphicMs0_04"), Crt8002Config::Crt8002Register)
    EVT_TEXT_ENTER(XRCID("Crt8002GraphicMs1_05"), Crt8002Config::Crt8002Register)
    EVT_TEXT_ENTER(XRCID("Crt8002Underline_06"), Crt8002Config::Crt8002Register)
    EVT_TEXT_ENTER(XRCID("Crt8002StrikeThru_07"), Crt8002Config::Crt8002Register)
    EVT_TEXT_ENTER(XRCID("Crt8002Blank_08"), Crt8002Config::Crt8002Register)
    EVT_TEXT_ENTER(XRCID("Crt8002Underline1_09"), Crt8002Config::Crt8002Register)
    EVT_TEXT_ENTER(XRCID("Crt8002Underline2_10"), Crt8002Config::Crt8002Register)
    EVT_TEXT_ENTER(XRCID("Crt8002StrikeThruLine1_11"), Crt8002Config::Crt8002Register)
    EVT_TEXT_ENTER(XRCID("Crt8002StrikeThruLine2_12"), Crt8002Config::Crt8002Register)
END_EVENT_TABLE()

wxString registerIdCrt8002[] =
{
    "Crt8002Attrbute_00",
    "Crt8002AttrbuteScreen1_01",
    "Crt8002Reverse_02",
    "Crt8002Blink_03",
    "Crt8002GraphicMs0_04",
    "Crt8002GraphicMs1_05",
    "Crt8002Underline_06",
    "Crt8002StrikeThru_07",
    "Crt8002Blank_08",
    "Crt8002Underline1_09",
    "Crt8002Underline2_10",
    "Crt8002StrikeThruLine1_11",
    "Crt8002StrikeThruLine2_12",
};

wxString registerFunctionCrt8002[] =
{
    "Attribute",
    "Attr. SC1",
    "Reverse bit",
    "Blink bit",
    "Graphic MS0/1",
    "Graphic MS0/1",
    "Underline 1/2/bit",
    "Strike thru 1/2/bit",
    "Blank bit",
    "Underline 1/2/bit",
    "Underline 1/2/bit",
    "Strike thru 1/2/bit",
    "Strike thru 1/2/bit",
};

Crt8002Config::Crt8002Config(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: Mc6845Config(title, pos, size, mode, dataDir, iniDir)
{
}

Crt8002Config::~Crt8002Config()
{
}

void Crt8002Config::crt8002ConfigInit()
{
    for (size_t registerNumber = 0; registerNumber<CRT8002_NUMBER_OF_REGISTERS; registerNumber++)
    {
        crt8002ConfigRegisterValueString[registerNumber] = "";
        lastCrt8002ConfigRegisterValueString[registerNumber] = "x";
    }

    computerConfiguration.crt8002Configuration.defined = false;

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
    
    if (!mode_.gui)
        return;

    XRCCTRL(*this, THIS_PANEL_NAME, wxPanel)->Hide();

    XRCCTRL(*this, "Crt8002IoGroupText", wxStaticText)->SetLabel("");

    disableIoPortConfig(registerIdCrt8002[CRT8002_ATTRIBUTE]);
    disableIoPortConfig(registerIdCrt8002[CRT8002_ATTRIBUTE_SCREEN1]);
}

void Crt8002Config::parseXml_Crt8002Video(wxXmlNode &node)
{
    videoConfigUsed(THIS_PANEL_NAME);

    computerConfiguration.crt8002Configuration.defined = true;

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
    wxString iogroup, polRevText;
    size_t ioGroupNumber = 0;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();
        polRevText = "";

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_IO:
                if (child->GetAttribute("type") == "attribute")
                {
                    computerConfiguration.crt8002Configuration.attribute = parseXml_IoPort(*child);
                    setIoPortConfig(computerConfiguration.crt8002Configuration.attribute, registerIdCrt8002[CRT8002_ATTRIBUTE], registerFunctionCrt8002[CRT8002_ATTRIBUTE], "B", false, "");
                }
            break;
                       
            case TAG_IN:
                if (child->GetAttribute("type") == "attribute_screen1")
                {
                    computerConfiguration.crt8002Configuration.attributeScreen1 = parseXml_IoPort(*child);
                    setIoPortConfig(computerConfiguration.crt8002Configuration.attributeScreen1, registerIdCrt8002[CRT8002_ATTRIBUTE_SCREEN1], registerFunctionCrt8002[CRT8002_ATTRIBUTE_SCREEN1], "I", false, "");
                }
            break;

            case TAG_REVERSE:
                computerConfiguration.crt8002Configuration.reverse.bitNumber = (int)parseXml_Number(*child);
                if (child->GetAttribute("pol") == "rev")
                {
                    computerConfiguration.crt8002Configuration.reverse.reversePolarity = 1;
                    polRevText = "R";
                }
                if (mode_.gui)
                    XRCCTRL(*this, registerIdCrt8002[CRT8002_REVERSE]+"Direction", wxStaticText)->SetLabel(polRevText);
            break;

            case TAG_BLINK:
                computerConfiguration.crt8002Configuration.blink.bitNumber = (int)parseXml_Number(*child);
                if (child->GetAttribute("pol") == "rev")
                {
                    computerConfiguration.crt8002Configuration.blink.reversePolarity = 1;
                    polRevText = "R";
                }
                if (mode_.gui)
                    XRCCTRL(*this, registerIdCrt8002[CRT8002_BLINK]+"Direction", wxStaticText)->SetLabel(polRevText);
            break;

            case TAG_GRAPHIC_MS0:
                computerConfiguration.crt8002Configuration.graphic_ms0.bitNumber = (int)parseXml_Number(*child);
                if (child->GetAttribute("pol") == "rev")
                {
                    computerConfiguration.crt8002Configuration.graphic_ms0.reversePolarity = 1;
                    polRevText = "R";
                }
                if (mode_.gui)
                    XRCCTRL(*this, registerIdCrt8002[CRT8002_GRAPHIC_MS0]+"Direction", wxStaticText)->SetLabel(polRevText);
            break;

            case TAG_GRAPHIC_MS1:
                computerConfiguration.crt8002Configuration.graphic_ms1.bitNumber = (int)parseXml_Number(*child);
                if (child->GetAttribute("pol") == "rev")
                    computerConfiguration.crt8002Configuration.graphic_ms1.reversePolarity = 1;
            break;

            case TAG_UNDERLINE:
                computerConfiguration.crt8002Configuration.underline.bitNumber = (int)parseXml_Number(*child);
                if (child->GetAttribute("pol") == "rev")
                {
                    computerConfiguration.crt8002Configuration.underline.reversePolarity = 1;
                    polRevText = "R";
                }
                if (mode_.gui)
                    XRCCTRL(*this, registerIdCrt8002[CRT8002_UNDERLINE_LINE1]+"Direction", wxStaticText)->SetLabel(polRevText);
                if (child->HasAttribute("line1"))
                {
                    computerConfiguration.crt8002Configuration.underlineLine1 = (int)parseXml_Number(*child, "line1");
                }
                if (child->HasAttribute("line2"))
                {
                    computerConfiguration.crt8002Configuration.underlineLine2 = (int)parseXml_Number(*child, "line2");
                }
            break;

            case TAG_STRIKE_THRU:
                computerConfiguration.crt8002Configuration.strikeThru.bitNumber = (int)parseXml_Number(*child);
                if (child->GetAttribute("pol") == "rev")
                {
                    computerConfiguration.crt8002Configuration.strikeThru.reversePolarity = 1;
                    polRevText = "R";
                }
                if (mode_.gui)
                    XRCCTRL(*this, registerIdCrt8002[CRT8002_STRIKE_THRU_LINE1]+"Direction", wxStaticText)->SetLabel(polRevText);
                if (child->HasAttribute("line1"))
                {
                    computerConfiguration.crt8002Configuration.strikeThruLine1 = (int)parseXml_Number(*child, "line1");
                }
                if (child->HasAttribute("line2"))
                {
                    computerConfiguration.crt8002Configuration.strikeThruLine2 = (int)parseXml_Number(*child, "line2");
                }
            break;

            case TAG_BLANK:
                computerConfiguration.crt8002Configuration.blank.bitNumber = (int)parseXml_Number(*child);
                if (child->GetAttribute("pol") == "rev")
                {
                    computerConfiguration.crt8002Configuration.blank.reversePolarity = 1;
                    polRevText = "R";
                }
                if (mode_.gui)
                    XRCCTRL(*this, registerIdCrt8002[CRT8002_BLANK]+"Direction", wxStaticText)->SetLabel(polRevText);
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
                if (mode_.gui)
                    XRCCTRL(*this,"Crt8002IoGroupText", wxStaticText)->SetLabel(p_Main->getGroupMessageXml(&computerConfiguration.crt8002Configuration.ioGroupVector));
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


void Crt8002Config::updateCrt8002Panel()
{
    if (computerConfiguration.crt8002Configuration.defined)
    {
        for (size_t registerNumber = 0; registerNumber<CRT8002_NUMBER_OF_REGISTERS; registerNumber++)
        {
            if (crt8002ConfigRegisterValueString[registerNumber] != lastCrt8002ConfigRegisterValueString[registerNumber])
            {
                p_Main->setTextCtrl(registerIdCrt8002[registerNumber], crt8002ConfigRegisterValueString[registerNumber]);
                lastCrt8002ConfigRegisterValueString[registerNumber] = crt8002ConfigRegisterValueString[registerNumber];
            }
        }
    }
}

int Crt8002Config::setCrt8002Register(int registerNumber, Word value, int showTrace)
{
    crt8002ConfigRegisterValueString[registerNumber].Printf("%04X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdCrt8002[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showVideoTraceText(registerFunctionCrt8002[registerNumber], crt8002ConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

int Crt8002Config::readCrt8002Register(int registerNumber, Byte WXUNUSED(value), int showTrace)
{
    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdCrt8002[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showVideoTraceTextRead(registerFunctionCrt8002[registerNumber], crt8002ConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

int Crt8002Config::readCrt8002RegisterSetData(int registerNumber, Byte value, int showTrace)
{
    crt8002ConfigRegisterValueString[registerNumber].Printf("%02X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdCrt8002[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showVideoTraceTextRead(registerFunctionCrt8002[registerNumber], crt8002ConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
      }
    return showTrace;
}

int Crt8002Config::setCrt8002Register(int registerNumber, Byte value, int showTrace)
{
    crt8002ConfigRegisterValueString[registerNumber].Printf("%02X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdCrt8002[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showVideoTraceText(registerFunctionCrt8002[registerNumber], crt8002ConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
      }
    return showTrace;
}

int Crt8002Config::setCrt8002RegisterNibble(int registerNumber, Byte value, int showTrace)
{
    crt8002ConfigRegisterValueString[registerNumber].Printf("%01X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdCrt8002[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showVideoTraceText(registerFunctionCrt8002[registerNumber], crt8002ConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
      }
    return showTrace;
}

bool Crt8002Config::isCrt8002TraceChecked(int registerNumber)
{
    if (!mode_.gui)
        return false;
    
    return XRCCTRL(*this, registerIdCrt8002[registerNumber]+"Trace", wxCheckBox)->IsChecked();
}

void Crt8002Config::Crt8002Register(wxCommandEvent&event)
{
    long number = getButtonNumber(wxWindow::FindWindowById(event.GetId())->GetName());
    if (number == -1)  return;

    long value = getVideoRegisterValue(registerIdCrt8002[number]);
    if (value == -1)  return;

    scn2672Pointer->writeRegisterCrt8002(number, value, DO_NOT_SHOW_ADDRESS_TRACE);
}
