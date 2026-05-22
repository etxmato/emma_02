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
#include "ct2425_config.h"
#include "wx/tglbtn.h"
#include "wx/xrc/xmlres.h"

#define THIS_PANEL_NAME "PanelOtherCt2425"

BEGIN_EVENT_TABLE(Ct2425Config, OtherConfig)
    EVT_TEXT_ENTER(XRCID("Ct2425GateAndLeds_11"), Ct2425Config::ct2425Register)
    EVT_TEXT_ENTER(XRCID("Ct2425Gates_12"), Ct2425Config::ct2425Register)
    EVT_TEXT_ENTER(XRCID("Ct2425Leds_13"), Ct2425Config::ct2425Register)
    EVT_TEXT_ENTER(XRCID("Ct2425Validator_14"), Ct2425Config::ct2425Register)
    EVT_TEXT_ENTER(XRCID("Ct2425Control_15"), Ct2425Config::ct2425Register)
    EVT_TEXT_ENTER(XRCID("Ct2425Sensors_16"), Ct2425Config::ct2425Register)
    EVT_TEXT_ENTER(XRCID("Ct2425Coins_17"), Ct2425Config::ct2425Register)
END_EVENT_TABLE()

wxString registerIdCt2425[] =
{
    "Ct2425GateAndLeds_11",
    "Ct2425Gates_12",
    "Ct2425Leds_13",
    "Ct2425Validator_14",
    "Ct2425Control_15",
    "Ct2425Sensors_16",
    "Ct2425Coins_17",
    "Ct2425UpperEscrow_18",
    "Ct2425LowerEscrow_19",
};

wxString registerFunctionCt2425[] =
{
    "Gates and leds",
    "Gates",
    "Leds",
    "Validator",
    "Control",
    "Sensors",
    "Coins",
    "Upper Escrow",
    "Lower Escrow",
};

wxString selectorIdCt2425[] =
{
    "Ct2425Gate1_00",
    "Ct2425Gate2_01",
    "Ct2425Gate3_02",
    "Ct2425Gate4_03",
    "Ct2425Gate5_04",
    "Ct2425Gate6_05",
    "Ct2425Sensor1_06",
    "Ct2425Sensor2_07",
    "Ct2425Sensor3_08",
    "Ct2425Sensor4_09",
    "Ct2425Sensor5_10",
    "Ct2425SpmEf_11",
    "Ct2425LastCoin_12"
};

wxString selectorFunctionCt2425[13][7] =
{ // false, true
    {"Gate 1 inactive", "Gate 1 active", "", "", "", "", "" },
    {"Gate 2 inactive", "Gate 2 active", "", "", "", "", "" },
    {"Gate 3 inactive", "Gate 3 active", "", "", "", "", "" },
    {"Gate 4 inactive", "Gate 4 active", "", "", "", "", "" },
    {"Gate 5 inactive", "Gate 5 active", "", "", "", "", "" },
    {"Gate 6 inactive", "Gate 6 active", "", "", "", "", "" },
    {"Sensor 1 blocked", "Sensor 1 clear", "", "", "", "", "" },
    {"Sensor 2 blocked", "Sensor 2 clear", "", "", "", "", "" },
    {"Sensor 3 blocked", "Sensor 3 clear", "", "", "", "", "" },
    {"Sensor 4 blocked", "Sensor 4 clear", "", "", "", "", "" },
    {"Sensor 5 blocked", "Sensor 5 clear", "", "", "", "", "" },
    {"EF2: SPM", "EF2: SPM", "", "", "", "", "" },
    {"Coin: X", "Coin: 10p", "Coin: 20p", "Coin: 50p", "Coin: 50p", "Coin: 1 Pound", "Coin: X" },
};

wxString coinButtonString[16] =
{
    "Inv", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"
};

Ct2425Config::Ct2425Config(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: OtherConfig(title, pos, size, mode, dataDir, iniDir)
{
}

Ct2425Config::~Ct2425Config()
{
}

void Ct2425Config::ct2425ConfigInit()
{
    for (size_t registerNumber = 0; registerNumber<CT2425_NUMBER_OF_REGISTERS; registerNumber++)
    {
        ct2425ConfigRegisterValueString[registerNumber] = "";
        lastCt2425ConfigRegisterValueString[registerNumber] = "x";
    }
    for (size_t registerNumber = 0; registerNumber<CT2425_NUMBER_OF_SELECTORS; registerNumber++)
    {
        ct2425ConfigSelector[registerNumber] = 0;
        updateCt2425ConfigSelector[registerNumber] = true;
    }

    if (!mode_.gui)
        return;

    XRCCTRL(*this, THIS_PANEL_NAME, wxPanel)->Hide();

    disableIoPortConfig(registerIdCt2425[CT2425_GATES_AND_LEDS]);
    ct2425ConfigRegisterValueString[CT2425_GATES_AND_LEDS] = "";
    disableIoPortConfig(registerIdCt2425[CT2425_GATES]);
    ct2425ConfigRegisterValueString[CT2425_GATES] = "";
    disableIoPortConfig(registerIdCt2425[CT2425_LEDS]);
    ct2425ConfigRegisterValueString[CT2425_LEDS] = "";
    disableIoPortConfig(registerIdCt2425[CT2425_VALIDATOR]);
    ct2425ConfigRegisterValueString[CT2425_VALIDATOR] = "";
    disableIoPortConfig(registerIdCt2425[CT2425_CONTROL]);
    ct2425ConfigRegisterValueString[CT2425_CONTROL] = "";
    disableIoPortConfig(registerIdCt2425[CT2425_SENSORS]);
    ct2425ConfigRegisterValueString[CT2425_SENSORS] = "";
    disableIoPortConfig(registerIdCt2425[CT2425_COINS]);
    ct2425ConfigRegisterValueString[CT2425_COINS] = "";

    XRCCTRL(*this, "Ct2425IoGroupText", wxStaticText)->SetLabel("");
}

void Ct2425Config::parseXml_Ct2425Coins(wxXmlNode &node)
{
    Ct2425CoinConfiguration ct2425Coins;
    
    wxString tagList[]=
    {
        "filename",
        "coin",
        "comment"
        "undefined"
    };

    enum
    {
        TAG_FILENAME,
        TAG_COIN,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt, bitNumber;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_FILENAME:
                ct2425Coins.filename = child->GetNodeContent();
            break;
                
            case TAG_COIN:
                bitNumber = (int)parseXml_Number(*child, "bit");
                if (bitNumber > 5)
                    bitNumber = 0;
                ct2425Coins.coinString[bitNumber] = child->GetNodeContent();
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
    
    computerConfiguration.ct2425CoinConfiguration.push_back(ct2425Coins);
}

void Ct2425Config::setCoinLogText(wxString filename)
{
    for (std::vector<Ct2425CoinConfiguration>::iterator coinConfigIterator = computerConfiguration.ct2425CoinConfiguration.begin (); coinConfigIterator != computerConfiguration.ct2425CoinConfiguration.end (); ++coinConfigIterator)
    {
        if (coinConfigIterator->filename == filename)
        {
            for (int bitNumber = 0; bitNumber<6; bitNumber++)
            {
                selectorFunctionCt2425[12][bitNumber+1] = "Coin: " + coinConfigIterator->coinString[bitNumber];
                coinButtonString[bitNumber + 0xa] = coinConfigIterator->coinString[bitNumber];
            }
        }
    }
}

wxString Ct2425Config::getCoinLogText(int value)
{
    return coinButtonString[value & 0xf];
}

void Ct2425Config::parseXml_Ct2425(wxXmlNode &node)
{
    otherConfigUsed(THIS_PANEL_NAME);

    Ct2425Configuration ct2425Io;
    
    wxString tagList[]=
    {
        "out",
        "in",
        "ef",
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
        TAG_EF,
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

    ct2425Io.ioGroupVector.clear();
    ct2425Io.validators = init_IoPort();
    ct2425Io.gates = init_IoPort();
    ct2425Io.control = init_IoPort();
    ct2425Io.sensors = init_IoPort();
    ct2425Io.coins = init_IoPort();

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
                {
                    ct2425Io.gates = parseXml_IoPort(*child, CT2425_IO_GATES);
                    setIoPortConfig(ct2425Io.gates, registerIdCt2425[CT2425_GATES_AND_LEDS], registerFunctionCt2425[CT2425_GATES_AND_LEDS], "O", false, "");
                    setIoPortConfig(ct2425Io.gates, registerIdCt2425[CT2425_GATES], registerFunctionCt2425[CT2425_GATES], "O", false, "");
                    setIoPortConfig(ct2425Io.gates, registerIdCt2425[CT2425_LEDS], registerFunctionCt2425[CT2425_LEDS], "O", false, "");
                }
                if (child->GetAttribute("type") == "validator")
                {
                    ct2425Io.validators = parseXml_IoPort(*child, CT2425_IO_VALIDATOR);
                    setIoPortConfig(ct2425Io.validators, registerIdCt2425[CT2425_VALIDATOR], registerFunctionCt2425[CT2425_VALIDATOR], "O", false, "");
                }
                if (child->GetAttribute("type") == "control")
                {
                    ct2425Io.control = parseXml_IoPort(*child, CT2425_IO_CONTROL);
                    setIoPortConfig(ct2425Io.control, registerIdCt2425[CT2425_CONTROL], registerFunctionCt2425[CT2425_CONTROL], "O", false, "");
                }
            break;
                
            case TAG_IN:
                if (child->GetAttribute("type") == "sensors")
                {
                    ct2425Io.sensors = parseXml_IoPort(*child, CT2425_IO_SENSORS);
                    setIoPortConfig(ct2425Io.sensors, registerIdCt2425[CT2425_SENSORS], registerFunctionCt2425[CT2425_SENSORS], "I", false, "");
                }
                if (child->GetAttribute("type") == "coins")
                {
                    ct2425Io.coins = parseXml_IoPort(*child, CT2425_IO_COINS);
                    setIoPortConfig(ct2425Io.coins, registerIdCt2425[CT2425_COINS], registerFunctionCt2425[CT2425_COINS], "I", false, "");
                }
            break;

            case TAG_EF:
                if (child->GetAttribute("type") == "spm")
                {
                    ct2425Io.efSpm = parseXml_EfFlag(*child, CT2425_IO_SPM_EF);
                    setEfFlagConfig(ct2425Io.efSpm , "SPM", "Ct2425SpmEf_11Text");
                    
                    wxString traceString;
                    traceString.Printf("EF%d: SPM", ct2425Io.efSpm.flagNumber);
                    selectorFunctionCt2425[11][0] = traceString;
                    selectorFunctionCt2425[11][1] = traceString;
                }
            break;
                
           case TAG_TIMER:
                if (child->GetAttribute("type") == "ignore_led")
                    ct2425Io.ignore_led = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "before_1")
                    ct2425Io.before_1 = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "block_1")
                    ct2425Io.block_1 = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "between_1_2")
                    ct2425Io.between_1_2 = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "block_2")
                    ct2425Io.block_2 = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "between_2_x")
                    ct2425Io.between_2_x = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "after_gate_5_6")
                    ct2425Io.after_gate_5_6 = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "block_5")
                    ct2425Io.block_5 = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "spm_interval")
                    ct2425Io.spm_interval = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "spm_pulse")
                    ct2425Io.spm_pulse = (int)parseXml_Number(*child);
            break;
                
            case TAG_SENSOR:
                if (child->GetAttribute("type") == "after_gate_1")
                    ct2425Io.sensor_after_gate_1 = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "after_gate_2")
                    ct2425Io.sensor_after_gate_2 = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "upper_escrow")
                    ct2425Io.sensor_upper_escrow = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "lower_escrow")
                    ct2425Io.sensor_lower_escrow = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "return")
                    ct2425Io.sensor_return = (int)parseXml_Number(*child);
            break;

            case TAG_GATE:
                if (child->GetAttribute("type") == "first")
                    ct2425Io.gate_first = (int)parseXml_Number(*child)-1;
                if (child->GetAttribute("type") == "second")
                    ct2425Io.gate_second = (int)parseXml_Number(*child)-1;
                if (child->GetAttribute("type") == "upper_escrow_cash")
                    ct2425Io.gate_upper_escrow_cash = (int)parseXml_Number(*child)-1;
                if (child->GetAttribute("type") == "lower_escrow_cash")
                    ct2425Io.gate_lower_escrow_cash = (int)parseXml_Number(*child)-1;
                if (child->GetAttribute("type") == "upper_escrow_return")
                    ct2425Io.gate_upper_escrow_return = (int)parseXml_Number(*child)-1;
                if (child->GetAttribute("type") == "lower_escrow_return")
                    ct2425Io.gate_lower_escrow_return = (int)parseXml_Number(*child)-1;
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    ct2425Io.ioGroupVector.resize(ioGroupNumber+1);
                    ct2425Io.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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
    computerConfiguration.ct2425Configuration.push_back(ct2425Io);
}

void Ct2425Config::updateCt2425Panel()
{
    for (size_t registerNumber = 0; registerNumber<CT2425_NUMBER_OF_REGISTERS; registerNumber++)
    {
        if (ct2425ConfigRegisterValueString[registerNumber] != lastCt2425ConfigRegisterValueString[registerNumber])
        {
            p_Main->setTextCtrl(registerIdCt2425[registerNumber], ct2425ConfigRegisterValueString[registerNumber]);
            lastCt2425ConfigRegisterValueString[registerNumber] = ct2425ConfigRegisterValueString[registerNumber];
        }
    }
    for (size_t selectorNumber = 0; selectorNumber<CT2425_NUMBER_OF_SELECTORS; selectorNumber++)
    {
        if (updateCt2425ConfigSelector[selectorNumber])
        {
            XRCCTRL(*this, selectorIdCt2425[selectorNumber] + "Text", wxStaticText)->SetLabel(selectorFunctionCt2425[selectorNumber][ct2425ConfigSelector[selectorNumber]]);
            updateCt2425ConfigSelector[selectorNumber] = false;
        }
    }
}

int Ct2425Config::setCt2425Register(int registerNumber, Byte value, int showTrace)
{
    ct2425ConfigRegisterValueString[registerNumber].Printf("%02X", value);

    if (!otherTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,registerIdCt2425[registerNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showOtherTraceText(registerFunctionCt2425[registerNumber], ct2425ConfigRegisterValueString[registerNumber], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
      }
    return showTrace;
}

int Ct2425Config::setCt2425SelectorValue(int selectorNumber, int selectorValue, int showTrace)
{
    ct2425ConfigSelector[selectorNumber] = selectorValue;
    updateCt2425ConfigSelector[selectorNumber] = true;

    if (!otherTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,selectorIdCt2425[selectorNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showOtherTraceText(selectorFunctionCt2425[selectorNumber][ct2425ConfigSelector[selectorNumber]], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

int Ct2425Config::setCt2425SelectorValue(int selectorNumber, bool selectorValue, int showTrace)
{
    if (selectorValue)
        ct2425ConfigSelector[selectorNumber] = 1;
    else
        ct2425ConfigSelector[selectorNumber] = 0;
    updateCt2425ConfigSelector[selectorNumber] = true;

    if (!otherTrace_ || !mode_.gui)  return showTrace;

    if (XRCCTRL(*this,selectorIdCt2425[selectorNumber]+"Trace", wxCheckBox)->IsChecked())
    {
        showOtherTraceText(selectorFunctionCt2425[selectorNumber][ct2425ConfigSelector[selectorNumber]], showTrace);
        
          if (showTrace == SHOW_ADDRESS_TRACE)
              return DO_NOT_SHOW_ADDRESS_TRACE;
    }
    return showTrace;
}

void Ct2425Config::ct2425Register(wxCommandEvent&event)
{
    long number = getButtonNumber(wxWindow::FindWindowById(event.GetId())->GetName());
    if (number == -1)  return;
    
    long value = getOtherRegisterValue(registerIdCt2425[number]);
    if (value == -1)  return;

    ct2425Pointer[0]->writeRegisterCt2425(number, value, DO_NOT_SHOW_ADDRESS_TRACE);
}
