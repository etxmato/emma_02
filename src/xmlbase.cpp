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

#include "wx/xrc/xmlres.h"
#include "wx/spinctrl.h"
#include "wx/statbox.h"

#include "main.h"
#include "xmlbase.h"

BEGIN_EVENT_TABLE(XmlBase, GuiMain)

END_EVENT_TABLE()

XmlBase::XmlBase(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiMain(title, pos, size, mode, dataDir, iniDir)
{
}

void XmlBase::xmlBaseConfigInit()
{
    computerConfiguration.cdp1861Configuration.pixieGraphics.interrupt = 62;
    computerConfiguration.cdp1861Configuration.pixieGraphics.start = 64;
    computerConfiguration.cdp1861Configuration.pixieGraphics.end = 191;
    computerConfiguration.cdp1861Configuration.pixieGraphics.screenend = 262;
    computerConfiguration.cdp1861Configuration.pixieGraphics.videoWidth = 64;
    computerConfiguration.cdp1861Configuration.pixieGraphics.videoHeight = 128;
}

void XmlBase::parseXml_pixieGraphics(wxXmlNode &node)
{
    wxString tagList[]=
    {
        "interrupt",
        "start",
        "end",
        "screenend",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_INTERRUPT,
        TAG_START,
        TAG_END,
        TAG_SCREENEND,
        TAG_COMMENT,
        TAG_UNDEFINED
    };

    int tagTypeInt;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_INTERRUPT:
                computerConfiguration.cdp1861Configuration.pixieGraphics.interrupt = (int)parseXml_Number(*child);
            break;

            case TAG_START:
                computerConfiguration.cdp1861Configuration.pixieGraphics.start = (int)parseXml_Number(*child);
            break;
                
            case TAG_END:
                computerConfiguration.cdp1861Configuration.pixieGraphics.end = (int)parseXml_Number(*child);
            break;
 
            case TAG_SCREENEND:
                computerConfiguration.cdp1861Configuration.pixieGraphics.screenend = (int)parseXml_Number(*child);
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

void XmlBase::disableIoPortConfig(wxString registerIdentifier)
{
    XRCCTRL(*this, registerIdentifier+"Trace", wxCheckBox)->Enable(false);
    XRCCTRL(*this, registerIdentifier+"Direction", wxStaticText)->Enable(false);
    XRCCTRL(*this, registerIdentifier+"Port", wxStaticText)->Enable(false);
    XRCCTRL(*this, registerIdentifier+"Q", wxStaticText)->Enable(false);
    XRCCTRL(*this, registerIdentifier, wxTextCtrl)->Enable(false);
}

void XmlBase::disableIoPortConfigRadio(wxString registerIdentifier)
{
    XRCCTRL(*this, registerIdentifier+"Trace", wxCheckBox)->Enable(false);
    XRCCTRL(*this, registerIdentifier+"Direction", wxStaticText)->Enable(false);
    XRCCTRL(*this, registerIdentifier+"Port", wxStaticText)->Enable(false);
    XRCCTRL(*this, registerIdentifier+"Q", wxStaticText)->Enable(false);
    XRCCTRL(*this, registerIdentifier, wxRadioButton)->Enable(false);
}

void XmlBase::setIoPortConfig(IoPort ioport, wxString registerIdentifier, wxString checkBoxLabel, wxString directionLabel, bool sixteenBit, wxString qLabel)
{
    XRCCTRL(*this, registerIdentifier+"Trace", wxCheckBox)->Enable(true);
    XRCCTRL(*this, registerIdentifier+"Direction", wxStaticText)->Enable(true);
    XRCCTRL(*this, registerIdentifier+"Port", wxStaticText)->Enable(true);
    XRCCTRL(*this, registerIdentifier+"Q", wxStaticText)->Enable(true);
    XRCCTRL(*this, registerIdentifier, wxTextCtrl)->Enable(true);

    XRCCTRL(*this, registerIdentifier+"Trace", wxCheckBox)->SetLabel(checkBoxLabel);
    XRCCTRL(*this, registerIdentifier+"Direction", wxStaticText)->SetLabel(directionLabel);

    wxString textLabel;
    if (ioport.addressMode)
    {
        if (sixteenBit)
            textLabel.Printf("%04X/%01X", ioport.portNumber[0], (ioport.portNumber[0] & 0xf)+1);
        else
            textLabel.Printf("%04X", ioport.portNumber[0]);
    }
    else
        textLabel.Printf("%d", ioport.portNumber[0]);
    XRCCTRL(*this, registerIdentifier+"Port", wxStaticText)->SetLabel(textLabel);

    if (ioport.qValue != -1)
        qLabel.Printf("%d", ioport.qValue);
    XRCCTRL(*this, registerIdentifier+"Q", wxStaticText)->SetLabel(qLabel);
}

void XmlBase::setIoPortConfig(wxString registerIdentifier, wxString checkBoxLabel, wxString directionLabel, wxString portLabel, wxString qLabel)
{
    XRCCTRL(*this, registerIdentifier+"Trace", wxCheckBox)->Enable(true);
    XRCCTRL(*this, registerIdentifier+"Direction", wxStaticText)->Enable(true);
    XRCCTRL(*this, registerIdentifier+"Port", wxStaticText)->Enable(false);
    XRCCTRL(*this, registerIdentifier+"Q", wxStaticText)->Enable(false);
    XRCCTRL(*this, registerIdentifier, wxTextCtrl)->Enable(true);

    XRCCTRL(*this, registerIdentifier+"Trace", wxCheckBox)->SetLabel(checkBoxLabel);
    XRCCTRL(*this, registerIdentifier+"Direction", wxStaticText)->SetLabel(directionLabel);

    XRCCTRL(*this, registerIdentifier+"Port", wxStaticText)->SetLabel(portLabel);
    XRCCTRL(*this, registerIdentifier+"Q", wxStaticText)->SetLabel(qLabel);
}

void XmlBase::setIoPortConfigRadio(IoPort ioport, wxString registerIdentifier, wxString checkBoxLabel, wxString textLabel)
{
    XRCCTRL(*this, registerIdentifier+"Trace", wxCheckBox)->Enable(true);
    XRCCTRL(*this, registerIdentifier+"Direction", wxStaticText)->Enable(true);
    XRCCTRL(*this, registerIdentifier+"Port", wxStaticText)->Enable(true);
    XRCCTRL(*this, registerIdentifier+"Q", wxStaticText)->Enable(true);
    XRCCTRL(*this, registerIdentifier, wxRadioButton)->Enable(true);

    XRCCTRL(*this, registerIdentifier+"Trace", wxCheckBox)->SetLabel(checkBoxLabel);
    XRCCTRL(*this, registerIdentifier+"Direction", wxStaticText)->SetLabel(textLabel);

    textLabel.Printf("%d", ioport.portNumber[0]);
    XRCCTRL(*this, registerIdentifier+"Port", wxStaticText)->SetLabel(textLabel);

    if (ioport.qValue == -1)
        textLabel = "-";
    else
        textLabel.Printf("%d", ioport.qValue);
    XRCCTRL(*this, registerIdentifier+"Q", wxStaticText)->SetLabel(textLabel);
}

void XmlBase::setEfFlagConfig(EfFlag efflag, wxString text, wxString flagId)
{
    wxString label;
    label.Printf("EF%d: " + text, efflag.flagNumber);
    
    if (efflag.excludeIoGroup)
        label += "(no grp)";
    XRCCTRL(*this, flagId, wxStaticText)->SetLabel(label);
}

IoPort XmlBase::parseXml_IoPort(wxXmlNode &node, int ioDefinition, Byte defaultMask)
{
    IoPort ioPort;
    
    wxString ports = node.GetNodeContent();
    long portNumber;
    ioPort.portNumber.clear();
    ioPort.addressMode = false;

    while (ports != "")
    {
        portNumber = getNextHexDec(&ports);
        ioPort.portNumber.push_back((int)portNumber);
    }
    if (ioPort.portNumber.size() == 0)
    {
        ioPort.portNumber.resize(1);
        ioPort.portNumber[0] = -1;
    }

    ioPort.qValue = -1;
    if (node.GetAttribute("q") == "1")
        ioPort.qValue = 1;
    if (node.GetAttribute("q") == "0")
        ioPort.qValue = 0;

    ioPort.mask = defaultMask;
    if (node.HasAttribute("mask"))
        ioPort.mask = (Byte)parseXml_Number(node, "mask");

    if (ioPort.portNumber[0] > 7 || node.GetAttribute("type") == "address")
        ioPort.addressMode = true;
    else
    {
        ioPort.ioDefinition = 0;
        if (ioDefinition != 0)
        {
            computerConfiguration.ioMask[ioDefinition] = ioPort.mask;
            ioPort.ioDefinition = ioDefinition;
        }
    }

    ioPort.addressMask = 0xffff;
    if (node.HasAttribute("addressmask"))
        ioPort.addressMask = (Word)parseXml_Number(node, "addressmask");

    return ioPort;
}

IoPort XmlBase::init_IoPort()
{
    IoPort ioPort;

    ioPort.portNumber.clear();
    ioPort.portNumber.resize(1);
    ioPort.portNumber[0] = -1;
    ioPort.qValue = -1;
    ioPort.mask = 0xff;
    ioPort.addressMask = 0xffff;
    ioPort.addressMode = false;
    ioPort.ioDefinition = 0;

    return ioPort;
}

EfFlag XmlBase::parseXml_EfFlag(wxXmlNode &node, int ioDefinition)
{
    EfFlag efFlag;
    
    efFlag.ioDefinition = 0;
    efFlag.excludeIoGroup = false;

    efFlag.flagNumber = (int)parseXml_Number(node);
    efFlag.qValue = -1;
    if (node.GetAttribute("q") == "1")
        efFlag.qValue = 1;
    if (node.GetAttribute("q") == "0")
        efFlag.qValue = 0;
    
    if (ioDefinition != 0)
        efFlag.ioDefinition = ioDefinition;

    if (node.GetAttribute("pol") == "rev")
        efFlag.reverse = 1;
    else
        efFlag.reverse = 0;

    if (node.GetAttribute("iogroup") == "no")
        efFlag.excludeIoGroup = true;

    return efFlag;
}

EfFlag XmlBase::init_EfFlag()
{
    EfFlag efFlag;

    efFlag.flagNumber = -1;
    efFlag.qValue = -1;
    efFlag.reverse = 0;
    efFlag.ioDefinition = 0;
    efFlag.excludeIoGroup = false;

    return efFlag;
}

long XmlBase::parseXml_Number(wxXmlNode &node)
{
    return getHexDec(node.GetNodeContent());
}

int XmlBase::parseXml_Mc6847BitSetup(wxXmlNode &node,  int bitNumber, wxString childName)
{
    wxString bitList[]=
    {
        "",
        "b0",
        "b1",
        "b2",
        "b3",
        "b4",
        "b5",
        "b6",
        "b7",
        "0",
        "1",
        "2",
        "3",
        "4",
        "5",
        "6",
        "7",
        "dd6",
        "dd7",
        "dd8",
        "dd9",
        "dd10",
        "dd11",
        "comment",
        "undefined"
    };
    
    enum
    {
        BIT_NONE,
        BIT_B0,
        BIT_B1,
        BIT_B2,
        BIT_B3,
        BIT_B4,
        BIT_B5,
        BIT_B6,
        BIT_B7,
        BIT_0,
        BIT_1,
        BIT_2,
        BIT_3,
        BIT_4,
        BIT_5,
        BIT_6,
        BIT_7,
        BIT_DD6,
        BIT_DD7,
        BIT_DD8,
        BIT_DD9,
        BIT_DD10,
        BIT_DD11,
        TAG_COMMENT,
        BIT_UNDEFINED
    };

    int bitTypeInt = 0;

    wxString bitType = node.GetNodeContent();
    
    while (bitTypeInt != BIT_UNDEFINED && bitList[bitTypeInt] != bitType)
        bitTypeInt++;
    
    switch (bitTypeInt)
    {
        case BIT_NONE:
        break;

        case BIT_0:
        case BIT_B0:
        case BIT_DD8:
            computerConfiguration.mc6847Configuration.b0 =  bitNumber;
            return 0;
        break;

        case BIT_1:
        case BIT_B1:
        case BIT_DD9:
            computerConfiguration.mc6847Configuration.b1 =  bitNumber;
            return 1;
        break;

        case BIT_2:
        case BIT_B2:
        case BIT_DD10:
            computerConfiguration.mc6847Configuration.b2 =  bitNumber;
            return 2;
        break;

        case BIT_3:
        case BIT_B3:
        case BIT_DD11:
            computerConfiguration.mc6847Configuration.b3 =  bitNumber;
            return 3;
        break;

        case BIT_4:
        case BIT_B4:
            computerConfiguration.mc6847Configuration.b4 =  bitNumber;
            return 4;
        break;

        case BIT_5:
        case BIT_B5:
            computerConfiguration.mc6847Configuration.b5 =  bitNumber;
            return 5;
        break;

        case BIT_6:
        case BIT_B6:
            computerConfiguration.mc6847Configuration.b6 =  bitNumber;
            return 6;
        break;

        case BIT_7:
        case BIT_B7:
            computerConfiguration.mc6847Configuration.b7 =  bitNumber;
            return 7;
        break;

        case BIT_DD6:
            computerConfiguration.mc6847Configuration.dd6 =  bitNumber;
            return 8;
        break;

        case BIT_DD7:
            computerConfiguration.mc6847Configuration.dd7 =  bitNumber;
            return 9;
        break;
            
        case TAG_COMMENT:
        break;

        default:
            warningText_ += "Unkown mc6847 bit number";
            warningText_ += childName;
            warningText_ += "\n";
        break;
    }
    return -1;
}

long XmlBase::parseXml_Number(wxXmlNode &node, wxString attribute)
{
    return getHexDec(node.GetAttribute(attribute));
}

bool XmlBase::parseXml_Range(wxXmlNode &node, long *start, long *end)
{
    wxString numberString1, numberString2;

    numberString1 = node.GetNodeContent();
    int dash = numberString1.Find("-");
    int x = numberString1.Find("x");

    if (dash == wxNOT_FOUND && x == wxNOT_FOUND)
    {
        *start = parseXml_Number(node);
        return false;
    }
    
    if (dash == wxNOT_FOUND)
    {
        *start = parseXml_Number(node);
        *end = *start;
        return true;
    }
    numberString2 = numberString1.Right(numberString1.Len()-(dash+1));
    numberString1 = numberString1.Left(dash);
    
    *start = getHexDec(numberString1);
    *end = getHexDec(numberString2);

    return true;
}

long XmlBase::getHexDec(wxString numberString)
{
    int base;
    long number;

    if (numberString.Left(2) == "0x")
    {
        base = 16;
        numberString = numberString.Right(numberString.Len()-2);
    }
    else
        base = 10;
    
    if (!numberString.ToLong(&number, base))
        number = 0;

    return number;
}

long XmlBase::getNextHexDec(wxString *numberString)
{
    int base;
    long number;
    wxString nextNumberString;

    nextNumberString = *numberString;
    nextNumberString.Trim(false);
    numberString->Trim(false);
    
    if (nextNumberString.Left(2) == "0x")
    {
        base = 16;
        nextNumberString = nextNumberString.Right(nextNumberString.Len()-2);
        *numberString = nextNumberString;
    }
    else
        base = 10;
    
    int comma = nextNumberString.Find(",");
    if (comma == wxNOT_FOUND)
		comma = nextNumberString.Find("-");
    if (comma == wxNOT_FOUND)
    {
        int cr = nextNumberString.Find("\n");
        if (cr == wxNOT_FOUND)
            *numberString = "";
        else
        {
            nextNumberString = nextNumberString.Left(cr);
            nextNumberString.Trim();
            *numberString = numberString->Right(numberString->Len()-(cr+1));
        }
    }
    else
    {
        nextNumberString = nextNumberString.Left(comma);
        nextNumberString.Trim();
        *numberString = numberString->Right(numberString->Len()-(comma+1));
    }
    
    if (!nextNumberString.ToLong(&number, base))
        number = 0;

    return number;
}

bool XmlBase::parseXml_Size(wxXmlNode &node, long *width, long *height)
{
    wxString numberString1, numberString2;
    numberString1 = node.GetNodeContent();

    int x = numberString1.Find("x");
    if (x == wxNOT_FOUND)
        return false;

    numberString2 = numberString1.Right(numberString1.Len()-(x+1));
    numberString1 = numberString1.Left(x);

    *width = getHexDec(numberString1);
    *height = getHexDec(numberString2);

    return true;
}

wxString XmlBase::getDoubleString(wxString doubleString, wxString tag, double max, wxString maxStr)
{
    double floatValue;

    if (!toDouble(doubleString, (double*)&floatValue))
    {
        warningText_ += tag;
        warningText_ += " incorrect number format\n";
        return "1.0";
    }
    if (floatValue == 0)
    {
        warningText_ += tag;
        warningText_ += " should not be 0\n";
        return "1.0";
    }
    if (floatValue >= max && max != -1)
    {
        warningText_ += tag;
        warningText_ += " should be < ";
        warningText_ += maxStr;
        warningText_ += "\n";
        return "1.0";
    }
    return doubleString;
}

double XmlBase::getDouble(wxString doubleString, wxString tag, double max, wxString maxStr, bool allowZero)
{
    double floatValue;

    if (!toDouble(doubleString, (double*)&floatValue))
    {
        warningText_ += tag;
        warningText_ += " incorrect number format\n";
        return 1;
    }
    if (floatValue == 0 && !allowZero)
    {
        warningText_ += tag;
        warningText_ += " should not be 0\n";
        return 1;
    }
    if (floatValue >= max && max != -1)
    {
        warningText_ += tag;
        warningText_ += " should be < ";
        warningText_ += maxStr;
        warningText_ += "\n";
        return 1;
    }
    return floatValue;
}

int XmlBase::textToColorCode(wxString color, bool returnMinusOnNotFound)
{
	if (color == "black")
        return GUI_COL_BLACK;
	if (color == "blue")
        return GUI_COL_BLUE;
	if (color == "pink")
        return GUI_COL_PINK;
	if (color == "red")
        return GUI_COL_RED;
	if (color == "orange")
        return GUI_COL_ORANGE;
	if (color == "purple")
        return GUI_COL_PURPLE;
	if (color == "steel")
        return GUI_COL_STEEL;
	if (color == "grey")
        return GUI_COL_GREY;
	if (color == "green")
        return GUI_COL_GREEN;
	if (color == "white")
        return GUI_COL_WHITE;
    
    if (returnMinusOnNotFound)
        return -1;
    
    return GUI_COL_BLACK;
}

wxColour XmlBase::textToWxColour(wxString color)
{
    int red, green, blue;

    red = (int)getNextHexDec(&color) & 0xff;
    green = (int)getNextHexDec(&color) & 0xff;
    blue = (int)getNextHexDec(&color) & 0xff;

    return wxColour (red, green, blue);
}
