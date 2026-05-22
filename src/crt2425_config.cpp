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
#include "crt2425_config.h"
#include "wx/tglbtn.h"
#include "wx/xrc/xmlres.h"

#define THIS_PANEL_NAME "PanelCrt2425"

BEGIN_EVENT_TABLE(Crt2425Config, Mc6845Config)
    EVT_TEXT_ENTER(XRCID("Crt2425Gate1_00"), Crt2425Config::Crt2425Register)
    EVT_TEXT_ENTER(XRCID("Crt2425Gate2_01"), Crt2425Config::Crt2425Register)
    EVT_TEXT_ENTER(XRCID("Crt2425Gate3_02"), Crt2425Config::Crt2425Register)
    EVT_TEXT_ENTER(XRCID("Crt2425Gate4_03"), Crt2425Config::Crt2425Register)
    EVT_TEXT_ENTER(XRCID("Crt2425Gate5_04"), Crt2425Config::Crt2425Register)
    EVT_TEXT_ENTER(XRCID("Crt2425Gate6_05"), Crt2425Config::Crt2425Register)
END_EVENT_TABLE()

wxString registerIdCrt2425[] =
{
    "Crt2425Gate1_00",
    "Crt2425Gate2_01",
    "Crt2425Gate3_02",
    "Crt2425Gate4_03",
    "Crt2425Gate5_04",
    "Crt2425Gate6_05",
    "Crt2425Sensor1_00",
    "Crt2425Sensor2_01",
    "Crt2425Sensor3_02",
    "Crt2425Sensor4_03",
};

wxString registerFunctionCrt2425[] =
{
    "Gate 1 Active",
    "Gate 2 Active",
    "Gate 3 Active",
    "Gate 4 Active",
    "Gate 5 Active",
    "Gate 6 Active",
    "Sensor 1 Clear",
    "Sensor 2 Clear",
    "Sensor 3 Clear",
    "Sensor 4 Clear",
};

Crt2425Config::Crt2425Config(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: Mc6845Config(title, pos, size, mode, dataDir, iniDir)
{
}

Crt2425Config::~Crt2425Config()
{
}

void Crt2425Config::crt2425ConfigInit()
{
    for (size_t registerNumber = 0; registerNumber<CRT2425_NUMBER_OF_REGISTERS; registerNumber++)
    {
        crt2425ConfigRegisterValueString[registerNumber] = "";
        lastCrt2425ConfigRegisterValueString[registerNumber] = "x";
    }

    if (!mode_.gui)
        return;

    XRCCTRL(*this, THIS_PANEL_NAME, wxPanel)->Hide();

    XRCCTRL(*this, "Crt2425IoGroupText", wxStaticText)->SetLabel("");
}

void Crt2425Config::parseXml_CoinMechanism(wxXmlNode &node)
{
    videoConfigUsed(THIS_PANEL_NAME);

    CoinMechanismConfiguration coinMechanismIo;
    
    wxString tagList[]=
    {
        "out",
        "in",
        "timer",
        "sensor",
        "gate",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IN,
        TAG_TIMER,
        TAG_SENSOR,
        TAG_GATE,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    wxString iogroup;
    size_t ioGroupNumber = 0;

    coinMechanismIo.ioGroupVector.clear();
    coinMechanismIo.validators = init_IoPort();
    coinMechanismIo.gates = init_IoPort();
    coinMechanismIo.control = init_IoPort();
    coinMechanismIo.sensors = init_IoPort();
    coinMechanismIo.coins = init_IoPort();

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
                if (child->GetAttribute("type") == "gates")
                    coinMechanismIo.gates = parseXml_IoPort(*child, COIN_MECHANISM_GATES);
                if (child->GetAttribute("type") == "validator")
                    coinMechanismIo.validators = parseXml_IoPort(*child, COIN_MECHANISM_VALIDATOR);
                if (child->GetAttribute("type") == "control")
                    coinMechanismIo.control = parseXml_IoPort(*child, COIN_MECHANISM_CONTROL);
            break;
                
            case TAG_IN:
                if (child->GetAttribute("type") == "sensors")
                    coinMechanismIo.sensors = parseXml_IoPort(*child, COIN_MECHANISM_SENSORS);
                if (child->GetAttribute("type") == "coins")
                    coinMechanismIo.coins = parseXml_IoPort(*child, COIN_MECHANISM_COINS);
            break;

            case TAG_TIMER:
                if (child->GetAttribute("type") == "ignore_led")
                    coinMechanismIo.ignore_led = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "before_1")
                    coinMechanismIo.before_1 = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "block_1")
                    coinMechanismIo.block_1 = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "between_1_2")
                    coinMechanismIo.between_1_2 = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "block_2")
                    coinMechanismIo.block_2 = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "between_2_x")
                    coinMechanismIo.between_2_x = (int)parseXml_Number(*child);
            break;
                
            case TAG_SENSOR:
                if (child->GetAttribute("type") == "after_gate_1")
                    coinMechanismIo.sensor_after_gate_1 = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "after_gate_2")
                    coinMechanismIo.sensor_after_gate_2 = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "upper_escrow")
                    coinMechanismIo.sensor_upper_escrow = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "lower_escrow")
                    coinMechanismIo.sensor_lower_escrow = (int)parseXml_Number(*child);
            break;

            case TAG_GATE:
                if (child->GetAttribute("type") == "first")
                    coinMechanismIo.gate_first = (int)parseXml_Number(*child)-1;
                if (child->GetAttribute("type") == "second")
                    coinMechanismIo.gate_second = (int)parseXml_Number(*child)-1;
                if (child->GetAttribute("type") == "upper_escrow_cash")
                    coinMechanismIo.gate_upper_escrow_cash = (int)parseXml_Number(*child)-1;
                if (child->GetAttribute("type") == "lower_escrow_cash")
                    coinMechanismIo.gate_lower_escrow_cash = (int)parseXml_Number(*child)-1;
                if (child->GetAttribute("type") == "upper_escrow_return")
                    coinMechanismIo.gate_upper_escrow_return = (int)parseXml_Number(*child)-1;
                if (child->GetAttribute("type") == "lower_escrow_return")
                    coinMechanismIo.gate_lower_escrow_return = (int)parseXml_Number(*child)-1;
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    coinMechanismIo.ioGroupVector.resize(ioGroupNumber+1);
                    coinMechanismIo.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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
    computerConfiguration.coinMechanismConfiguration.push_back(coinMechanismIo);
}

void Crt2425Config::updateCrt2425Panel()
{
    for (size_t registerNumber = 0; registerNumber<CRT2425_NUMBER_OF_REGISTERS; registerNumber++)
    {
        if (crt2425ConfigRegisterValueString[registerNumber] != lastCrt2425ConfigRegisterValueString[registerNumber])
        {
            p_Main->setTextCtrl(registerIdCrt2425[registerNumber], crt2425ConfigRegisterValueString[registerNumber]);
            lastCrt2425ConfigRegisterValueString[registerNumber] = crt2425ConfigRegisterValueString[registerNumber];
        }
    }
}

int Crt2425Config::setCrt2425Register(int registerNumber, Word value, int showTrace)
{
    crt2425ConfigRegisterValueString[registerNumber].Printf("%04X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdCrt2425[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showTraceText(registerFunctionCrt2425[registerNumber], crt2425ConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

int Crt2425Config::readCrt2425Register(int registerNumber, Byte WXUNUSED(value), int showTrace)
{
    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdCrt2425[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showTraceTextRead(registerFunctionCrt2425[registerNumber], crt2425ConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

int Crt2425Config::readCrt2425RegisterSetData(int registerNumber, Byte value, int showTrace)
{
    crt2425ConfigRegisterValueString[registerNumber].Printf("%02X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdCrt2425[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showTraceTextRead(registerFunctionCrt2425[registerNumber], crt2425ConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
      }
    return showTrace;
}

int Crt2425Config::setCrt2425Register(int registerNumber, Byte value, int showTrace)
{
    crt2425ConfigRegisterValueString[registerNumber].Printf("%02X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdCrt2425[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showTraceText(registerFunctionCrt2425[registerNumber], crt2425ConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
      }
    return showTrace;
}

int Crt2425Config::setCrt2425Register(int registerNumber, bool value, int showTrace)
{
    if (value)
        crt2425ConfigRegisterValueString[registerNumber] = "1";
    else
        crt2425ConfigRegisterValueString[registerNumber] = "0";

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdCrt2425[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showTraceText(registerFunctionCrt2425[registerNumber], crt2425ConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
      }
    return showTrace;
}


int Crt2425Config::setCrt2425RegisterNibble(int registerNumber, Byte value, int showTrace)
{
    crt2425ConfigRegisterValueString[registerNumber].Printf("%01X", value);

    if (!videoTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdCrt2425[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showTraceText(registerFunctionCrt2425[registerNumber], crt2425ConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
      }
    return showTrace;
}

bool Crt2425Config::isCrt2425TraceChecked(int registerNumber)
{
    if (!mode_.gui)
        return false;
    
    return XRCCTRL(*this, registerIdCrt2425[registerNumber]+"Trace", wxCheckBox)->IsChecked();
}

void Crt2425Config::Crt2425Register(wxCommandEvent&event)
{
    if (!computerRunning_)
    {
        showNotRunning();
        return;
    }

    wxString idReference = wxWindow::FindWindowById(event.GetId())->GetName();
    wxString buttonNumber = idReference.Right(2);
    
    long number;
    if (!buttonNumber.ToLong(&number, 10))
        return;

    long value = get16BitValue(registerIdCrt2425[number]);
    if (value == -1)  return;

//    scn2672Pointer->writeRegisterCrt2425(number, value, DO_NOT_SHOW_ADDRESS_TRACE);
}
