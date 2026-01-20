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

#include "main.h"
#include "vtsetup.h"

#include "wx/xrc/xmlres.h"             
#include "wx/spinctrl.h"
#include "wx/statline.h"

extern "C" 
{
#include "libserialport.h"
}

BEGIN_EVENT_TABLE(VtSetupDialog, wxDialog)
    EVT_BUTTON(XRCID("VtSetupSave"), VtSetupDialog::onSaveButton)
    EVT_TEXT(XRCID("VtSetupWavFile"), VtSetupDialog::onVtWavFile)
    EVT_BUTTON(XRCID("VtSetupWavButton"), VtSetupDialog::onVtWavFileButton)
    EVT_BUTTON(XRCID("VtSetupWavEject"), VtSetupDialog::onVtWavFileEject)
    EVT_TEXT(XRCID("VtSetupCharRom"), VtSetupDialog::onVtCharRomText)
    EVT_COMBOBOX(XRCID("VtSetupCharRom"), VtSetupDialog::onVtCharRomText)
    EVT_BUTTON(XRCID("VtSetupCharRomButton"), VtSetupDialog::onVtCharRom)
    EVT_CHECKBOX(XRCID("Uart1854"), VtSetupDialog::onUart1854)
    EVT_CHECKBOX(XRCID("Uart16450"), VtSetupDialog::onUart16450)
    EVT_CHECKBOX(XRCID("VtSelectXml"), VtSetupDialog::onXmlValues)

END_EVENT_TABLE()

VtSetupDialog::VtSetupDialog(wxWindow* parent)
{
    wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"vt_" + p_Main->getFontSize() + ".xrc");
    wxXmlResource::Get()->LoadDialog(this, parent, "VtSetupDialog");

    currentComputerConfiguration = p_Main->getConfiguration();
    originalUartValue_ = currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].uart1854_defined;

    this->SetTitle("Video Terminal Setup "+p_Main->getSelectedComputerText());

    enableGuiAccordingToSelectedInterface();
    defineSetupFeature();

    switch (currentComputerConfiguration.videoTerminalConfiguration.type)
    {
        case VT52:
            XRCCTRL(*this, "VtSetupBit5", wxChoice)->Hide();
            XRCCTRL(*this, "VtSetupBit5Text", wxStaticText)->Hide();
            XRCCTRL(*this, "VtSetupBit9", wxChoice)->Hide();
            XRCCTRL(*this, "VtSetupBit9Text", wxStaticText)->Hide();
            XRCCTRL(*this, "StaticLine3", wxStaticLine)->Hide();
#ifdef __WXMSW__
            XRCCTRL(*this, "VtSerialPortChoice", wxChoice)->Hide();
#else
            XRCCTRL(*this, "VtSerialPort", wxTextCtrl)->Hide();
#endif
            XRCCTRL(*this, "VtSerialPortText", wxStaticText)->Hide();
            XRCCTRL(*this, "VtCharacters", wxChoice)->Hide();
            XRCCTRL(*this, "VtCharactersText", wxStaticText)->Hide();
        break;

        case VT100:
            XRCCTRL(*this, "StaticLine3", wxStaticLine)->Hide();
#ifdef __WXMSW__
            XRCCTRL(*this, "VtSerialPortChoice", wxChoice)->Hide();
#else
            XRCCTRL(*this, "VtSerialPort", wxTextCtrl)->Hide();
#endif
            XRCCTRL(*this, "VtSerialPortText", wxStaticText)->Hide();
        break;
    }

    if (currentComputerConfiguration.videoTerminalConfiguration.external || currentComputerConfiguration.videoTerminalConfiguration.loop_back)
    {
        XRCCTRL(*this, "VtSetupBit0", wxChoice)->Hide();
        XRCCTRL(*this, "VtSetupBit0Text", wxStaticText)->Hide();
        XRCCTRL(*this, "VtSetupBit1", wxChoice)->Hide();
        XRCCTRL(*this, "VtSetupBitsPerCharacter", wxChoice)->Show();
        XRCCTRL(*this, "VtSetupBit4", wxChoice)->Hide();
        XRCCTRL(*this, "VtSetupBit4Text", wxStaticText)->Hide();
        XRCCTRL(*this, "VtSetupBit5", wxChoice)->Hide();
        XRCCTRL(*this, "VtSetupBit5Text", wxStaticText)->Hide();
        XRCCTRL(*this, "VtSetupBit6", wxChoice)->Hide();
        XRCCTRL(*this, "VtSetupBit6Text", wxStaticText)->Hide();
        XRCCTRL(*this, "VtSetupBit7", wxChoice)->Hide();
        XRCCTRL(*this, "VtSetupBit7Text", wxStaticText)->Hide();
        XRCCTRL(*this, "VtSetupBit8", wxChoice)->Hide();
        XRCCTRL(*this, "VtSetupBit8Text", wxStaticText)->Hide();
        XRCCTRL(*this, "VtSetupBit9", wxChoice)->Hide();
        XRCCTRL(*this, "VtSetupBit9Text", wxStaticText)->Hide();
        XRCCTRL(*this, "VtSetupBit10", wxChoice)->Hide();
        XRCCTRL(*this, "VtSetupBit10Text", wxStaticText)->Hide();
        XRCCTRL(*this, "VtSetupBit11", wxChoice)->Hide();
        XRCCTRL(*this, "VtSetupBit11Text", wxStaticText)->Hide();
        XRCCTRL(*this, "VtSetupBit12", wxChoice)->Hide();
        XRCCTRL(*this, "VtSetupBit12Text", wxStaticText)->Hide();
        XRCCTRL(*this, "VtSetupBit13", wxChoice)->Hide();
        XRCCTRL(*this, "VtSetupBit13Text", wxStaticText)->Hide();
        XRCCTRL(*this, "VtSetupBit14", wxChoice)->Hide();
        XRCCTRL(*this, "VtSetupBit14Text", wxStaticText)->Hide();
        XRCCTRL(*this, "VtSetupBit15", wxChoice)->Hide();
        XRCCTRL(*this, "VtSetupBit15Text", wxStaticText)->Hide();
        XRCCTRL(*this, "VtSetupBit16", wxChoice)->Hide();
        XRCCTRL(*this, "VtSetupBit16Text", wxStaticText)->Hide();
        XRCCTRL(*this, "VtCharacters", wxChoice)->Hide();
        XRCCTRL(*this, "VtCharactersText", wxStaticText)->Hide();
        XRCCTRL(*this, "VtBellText", wxStaticText)->Hide();
        XRCCTRL(*this, "VtBell", wxTextCtrl)->Hide();
        XRCCTRL(*this, "VtHzText", wxStaticText)->Hide();
        XRCCTRL(*this, "VtSetupWavButton", wxButton)->Hide();
        XRCCTRL(*this, "VtSetupWavFile", wxTextCtrl)->Hide();
        XRCCTRL(*this, "VtSetupWavEject", wxBitmapButton)->Hide();
        XRCCTRL(*this, "StaticLine1", wxStaticLine)->Hide();
        XRCCTRL(*this, "XmodemLine", wxStaticLine)->Hide();
        XRCCTRL(*this, "XmodemLine", wxStaticLine)->Hide();
        XRCCTRL(*this, "VtXmodemPacketSizeText", wxStaticText)->Hide();
        XRCCTRL(*this, "VtXmodemPacketSizeChoice", wxChoice)->Hide();
        XRCCTRL(*this, "ESCError", wxCheckBox)->Hide();
        XRCCTRL(*this, "SerialLog", wxCheckBox)->Hide();
        XRCCTRL(*this, "VtSetupCharRomButton", wxButton)->Hide();
        XRCCTRL(*this, "VtSetupCharRom", wxComboBox)->Hide();
    }

    if (currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].uart16450_defined || currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].uart1854_defined)
    {
        XRCCTRL(*this, "VtEf", wxCheckBox)->Enable(false);
        XRCCTRL(*this, "VtQ", wxCheckBox)->Enable(false);
    }

    if (currentComputerConfiguration.videoTerminalConfiguration.xModem_defined && !(currentComputerConfiguration.videoTerminalConfiguration.external || currentComputerConfiguration.videoTerminalConfiguration.loop_back))
    {
        XRCCTRL(*this, "XmodemLine", wxStaticLine)->Show();
        XRCCTRL(*this, "VtXmodemPacketSizeText", wxStaticText)->Show();
        XRCCTRL(*this, "VtXmodemPacketSizeChoice", wxChoice)->Show();
    }
    
    loadSettingValues();
}

void VtSetupDialog::loadSettingValues()
{
    defineSetupFeature();

    XRCCTRL(*this, "VtStopBits", wxTextCtrl)->ChangeValue(currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].stopBitString);

    wxString bellFrequency;
    bellFrequency.Printf("%d", currentComputerConfiguration.videoTerminalConfiguration.bellFrequency);
    XRCCTRL(*this, "VtBell", wxTextCtrl)->ChangeValue(bellFrequency);

    switch (currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].charactersPerRow)
    {
        case 64:
            XRCCTRL(*this, "VtCharacters", wxChoice)->SetSelection(0);
        break;
        case 132:
            XRCCTRL(*this, "VtCharacters", wxChoice)->SetSelection(2);
        break;
        default:
            XRCCTRL(*this, "VtCharacters", wxChoice)->SetSelection(1);
        break;
    }

    if (currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].wavFileName != "")
        XRCCTRL(*this, "VtBell", wxTextCtrl)->Enable(false);

    XRCCTRL(*this, "VtSetupWavFile", wxTextCtrl)->ChangeValue(currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].wavFileName);

    XRCCTRL(*this, "SerialLog", wxCheckBox)->SetValue(currentComputerConfiguration.videoTerminalConfiguration.serialLog);
    XRCCTRL(*this, "ESCError", wxCheckBox)->SetValue(currentComputerConfiguration.videoTerminalConfiguration.escError);

    XRCCTRL(*this, "Uart1854", wxCheckBox)->SetValue(currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].uart1854_defined);
    XRCCTRL(*this, "Uart16450", wxCheckBox)->SetValue(currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].uart16450_defined);
    XRCCTRL(*this, "VtSelectXml", wxCheckBox)->SetValue(currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting == XML_SETTING);
    XRCCTRL(*this, "VtEf", wxCheckBox)->SetValue(currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].efReverse);
    XRCCTRL(*this, "VtQ", wxCheckBox)->SetValue(currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].reverseQ == 1);
    XRCCTRL(*this, "VtSetupBitsPerCharacter", wxChoice)->SetSelection(currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].bitsPerCharacter - 5);

    if (currentComputerConfiguration.videoTerminalConfiguration.xModem_defined && !(currentComputerConfiguration.videoTerminalConfiguration.external || currentComputerConfiguration.videoTerminalConfiguration.loop_back))
    {
        XRCCTRL(*this, "VtXmodemPacketSizeChoice", wxChoice)->SetSelection(currentComputerConfiguration.videoTerminalConfiguration.packetSize);
    }

    wxString box;
    for (int i=0; i<17; i++)
    {
        box.Printf("%d", i);
        XRCCTRL(*this, "VtSetupBit"+box, wxChoice)->SetSelection(SetUpFeature_[i]);
    }
    if (currentComputerConfiguration.videoTerminalConfiguration.external || currentComputerConfiguration.videoTerminalConfiguration.loop_back)
    {
        XRCCTRL(*this, "VtSetupBit2", wxChoice)->SetSelection(currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].parity);
        XRCCTRL(*this, "VtSetupBit3", wxChoice)->SetSelection(currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].paritySense);
    }

    XRCCTRL(*this, "VtSetupCharRom", wxComboBox)->SetValue(currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].vtCharRomFileName);
    
#ifdef __WXMSW__
    listPorts();
#else
    XRCCTRL(*this, "VtSerialPort", wxTextCtrl)->ChangeValue(currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].serialPort);
#endif
}

void VtSetupDialog::defineSetupFeature()
{
    switch (currentComputerConfiguration.videoTerminalConfiguration.type)
    {
        case VT52:
            SetUpFeature_ = currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].vt52SetUpFeature;
        break;
        case VT100:
            SetUpFeature_ = currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].vt100SetUpFeature;
        break;
    }
}

void VtSetupDialog::onSaveButton( wxCommandEvent& WXUNUSED(event) )
{
    if (XRCCTRL(*this, "VtSelectXml", wxCheckBox)->GetValue())
        currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting = XML_SETTING;
    else
        currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting = MANUAL_SETTING;

    if (currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting == MANUAL_SETTING)
    {
        if (currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[MANUAL_SETTING].wavFileName != "")
        {
            if (!p_Main->checkWavFile(currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[MANUAL_SETTING].wavDirectory + currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[MANUAL_SETTING].wavFileName))
                return;
        }
        
        wxString box;
        
        for (int i=0; i<17; i++)
        {
            box.Printf("%d", i);
            if (XRCCTRL(*this, "VtSetupBit"+box, wxChoice)->GetSelection() == 0)
                SetUpFeature_[i]  = 0;
            else
                SetUpFeature_[i]  = 1;
        }

        switch (currentComputerConfiguration.videoTerminalConfiguration.type)
        {
            case VT52:
                currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[MANUAL_SETTING].vt52SetUpFeature = SetUpFeature_;
            break;
                
            case VT100:
                currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[MANUAL_SETTING].vt100SetUpFeature = SetUpFeature_;
                switch (XRCCTRL(*this, "VtCharacters", wxChoice)->GetSelection())
                {
                    case 0:
                        currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[MANUAL_SETTING].charactersPerRow = 64;
                        currentComputerConfiguration.videoTerminalConfiguration.characterWidth = 10;
                    break;
                    case 2:
                        currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[MANUAL_SETTING].charactersPerRow = 132;
                        currentComputerConfiguration.videoTerminalConfiguration.characterWidth = 8;
                    break;
                    default:
                        currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[MANUAL_SETTING].charactersPerRow = 80;
                        currentComputerConfiguration.videoTerminalConfiguration.characterWidth = 10;
                    break;
                }
            break;
        }

        if (currentComputerConfiguration.videoTerminalConfiguration.external || currentComputerConfiguration.videoTerminalConfiguration.loop_back)
        {
            currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].parity = (XRCCTRL(*this, "VtSetupBit2", wxChoice)->GetSelection() == 1);
            currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].paritySense = (XRCCTRL(*this, "VtSetupBit3", wxChoice)->GetSelection() == 1);
        }

        currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[MANUAL_SETTING].stopBitString = p_Main->convertLocale(XRCCTRL(*this, "VtStopBits", wxTextCtrl)->GetValue());
        if (!currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[MANUAL_SETTING].stopBitString.ToDouble(&currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[MANUAL_SETTING].stopBit))
            currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[MANUAL_SETTING].stopBit = 0;
        
        currentComputerConfiguration.videoTerminalConfiguration.serialLog = XRCCTRL(*this, "SerialLog", wxCheckBox)->GetValue();
        currentComputerConfiguration.videoTerminalConfiguration.escError = XRCCTRL(*this, "ESCError", wxCheckBox)->GetValue();

        if (currentComputerConfiguration.videoTerminalConfiguration.xModem_defined)
        {
            currentComputerConfiguration.videoTerminalConfiguration.packetSize = XRCCTRL(*this, "VtXmodemPacketSizeChoice", wxChoice)->GetSelection();
        }

        currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[MANUAL_SETTING].uart1854_defined = XRCCTRL(*this, "Uart1854", wxCheckBox)->GetValue();
        currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[MANUAL_SETTING].uart16450_defined = XRCCTRL(*this, "Uart16450", wxCheckBox)->GetValue();
        currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[MANUAL_SETTING].efReverse = XRCCTRL(*this, "VtEf", wxCheckBox)->GetValue();

        if (XRCCTRL(*this, "VtQ", wxCheckBox)->IsChecked())
            currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[MANUAL_SETTING].reverseQ = 1;
        else
            currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[MANUAL_SETTING].reverseQ = 0;
        currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].bitsPerCharacter = XRCCTRL(*this, "VtSetupBitsPerCharacter", wxChoice)->GetSelection() + 5;

        long bellFrequency;
        wxString valueString = XRCCTRL(*this, "VtBell", wxTextCtrl)->GetValue();
        valueString.ToLong(&bellFrequency);
        currentComputerConfiguration.videoTerminalConfiguration.bellFrequency = (int)bellFrequency;

        currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[MANUAL_SETTING].wavFileName= XRCCTRL(*this, "VtSetupWavFile", wxTextCtrl)->GetValue();
        
#ifdef __WXMSW__
       int selection = XRCCTRL(*this, "VtSerialPortChoice", wxChoice)->GetSelection();
       if (selection != -1)
            currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[MANUAL_SETTING].serialPort= XRCCTRL(*this, "VtSerialPortChoice", wxChoice)->GetString(selection);
#else
        currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[MANUAL_SETTING].serialPort= XRCCTRL(*this, "VtSerialPort", wxTextCtrl)->GetValue();
#endif
    }

    p_Main->setConfiguration(currentComputerConfiguration);
    p_Main->writeDefaultVtConfig();

    EndModal(wxID_OK);
}

void VtSetupDialog::onVtWavFile(wxCommandEvent& event)
{
    currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].wavFileName = event.GetString();

    if (currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].wavFileName == "")
        XRCCTRL(*this, "VtBell", wxTextCtrl)->Enable(true);
    else
        XRCCTRL(*this, "VtBell", wxTextCtrl)->Enable(false);
}

void VtSetupDialog::onVtWavFileButton(wxCommandEvent& WXUNUSED(event))
{
    wxString fileName;

    fileName = wxFileSelector("Select the WAV file for the bell sound",
        currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].wavDirectory, currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].wavFileName,
        "wav",
        wxString::Format
        (
            "WAV File (*.wav)|*.wav|All files (%s)|%s",
            wxFileSelectorDefaultWildcardStr,
            wxFileSelectorDefaultWildcardStr
            ),
        wxFD_OPEN | wxFD_CHANGE_DIR | wxFD_PREVIEW,
        this
        );


    if (!fileName)
        return;

    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].wavDirectory = FullPath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].wavFileName = FullPath.GetFullName();

    XRCCTRL(*this, "VtSetupWavFile", wxTextCtrl)->SetValue(currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].wavFileName);

    if (currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].wavFileName == "")
        XRCCTRL(*this, "VtBell", wxTextCtrl)->Enable(true);
    else
        XRCCTRL(*this, "VtBell", wxTextCtrl)->Enable(false);
    
    p_Main->checkWavFile(currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].wavDirectory + currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].wavFileName);
}

void VtSetupDialog::onVtWavFileEject(wxCommandEvent& WXUNUSED(event))
{
    currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].wavFileName = "";
    XRCCTRL(*this, "VtSetupWavFile", wxTextCtrl)->SetValue(currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].wavFileName);

    XRCCTRL(*this, "VtBell", wxTextCtrl)->Enable(true);
}

void VtSetupDialog::onVtCharRom(wxCommandEvent& WXUNUSED(event) )
{
    wxString fileName;
    wxString vtCharRomDir;
    
    vtCharRomDir = currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].vtCharRomDirectory;

    fileName = wxFileSelector( "Select the VT Character Font file to load",
                               vtCharRomDir, XRCCTRL(*this, "VtSetupCharRom", wxComboBox)->GetValue(),
                               "bin",
                               wxString::Format
                              (
                                    "Binary Font File|*.bin;*.rom|All files (%s)|%s",
                                    wxFileSelectorDefaultWildcardStr,
                                    wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );
    if (!fileName)
        return;

    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].vtCharRomDirectory = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].vtCharRomFileName = FullPath.GetFullName();

    XRCCTRL(*this, "VtSetupCharRom", wxComboBox)->SetValue(currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].vtCharRomFileName);
}

void VtSetupDialog::onVtCharRomText(wxCommandEvent& WXUNUSED(event))
{
    currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].vtCharRomFileName = XRCCTRL(*this, "VtSetupCharRom", wxComboBox)->GetValue();
}


void VtSetupDialog::listPorts()
{
    int i;
    struct sp_port **ports;
    wxString port;
    
    sp_return error = sp_list_ports(&ports);
    if (error == SP_OK)
    {
        for (i = 0; ports[i]; i++)
        {
            port = sp_get_port_name(ports[i]);
            XRCCTRL(*this, "VtSerialPortChoice", wxChoice)->Append(port);
        }
        sp_free_port_list(ports);
    }
    
    int selection = XRCCTRL(*this, "VtSerialPortChoice", wxChoice)->FindString(currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].serialPort);
    if (selection != wxNOT_FOUND)
        XRCCTRL(*this, "VtSerialPortChoice", wxChoice)->SetSelection(selection);
}

void VtSetupDialog::onUart1854(wxCommandEvent&event)
{
    currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].uart1854_defined = event.IsChecked();
    if (currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].uart1854_defined)
        XRCCTRL(*this, "Uart16450", wxCheckBox)->SetValue(false);
    
    if (currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].uart16450_defined || currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].uart1854_defined)
    {
        XRCCTRL(*this, "VtEf", wxCheckBox)->Enable(false);
        XRCCTRL(*this, "VtQ", wxCheckBox)->Enable(false);
    }
    else
    {
        XRCCTRL(*this, "VtEf", wxCheckBox)->Enable(true);
        XRCCTRL(*this, "VtQ", wxCheckBox)->Enable(true);
    }
}

void VtSetupDialog::onUart16450(wxCommandEvent&event)
{
    currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].uart16450_defined = event.IsChecked();

    if (currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].uart16450_defined)
        XRCCTRL(*this, "Uart1854", wxCheckBox)->SetValue(false);
    
    if (currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].uart16450_defined || currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].uart1854_defined)
    {
        XRCCTRL(*this, "VtEf", wxCheckBox)->Enable(false);
        XRCCTRL(*this, "VtQ", wxCheckBox)->Enable(false);
    }
    else
    {
        XRCCTRL(*this, "VtEf", wxCheckBox)->Enable(true);
        XRCCTRL(*this, "VtQ", wxCheckBox)->Enable(true);
    }
}

void VtSetupDialog::onXmlValues(wxCommandEvent&event)
{
    if (event.IsChecked())
        currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting = XML_SETTING;
    else
        currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting = MANUAL_SETTING;
    enableGuiAccordingToSelectedInterface();
    loadSettingValues();
}

void VtSetupDialog::enableGuiAccordingToSelectedInterface()
{
    bool enableGui = (currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting != XML_SETTING);
    
    XRCCTRL(*this, "VtSetupBit0", wxChoice)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_BIT0]);
    XRCCTRL(*this, "VtSetupBit1", wxChoice)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_BIT1]);
    XRCCTRL(*this, "VtSetupBit2", wxChoice)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_BIT2]);
    XRCCTRL(*this, "VtSetupBit3", wxChoice)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_BIT3]);
    XRCCTRL(*this, "VtSetupBit4", wxChoice)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_BIT4]);
    XRCCTRL(*this, "VtSetupBit5", wxChoice)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_BIT5]);
    XRCCTRL(*this, "VtSetupBit6", wxChoice)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_BIT6]);
    XRCCTRL(*this, "VtSetupBit7", wxChoice)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_BIT7]);
    XRCCTRL(*this, "VtSetupBit8", wxChoice)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_BIT8]);
    XRCCTRL(*this, "VtSetupBit9", wxChoice)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_BIT9]);
    XRCCTRL(*this, "VtSetupBit10", wxChoice)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_BIT10]);
    XRCCTRL(*this, "VtSetupBit11", wxChoice)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_BIT11]);
    XRCCTRL(*this, "VtSetupBit12", wxChoice)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_BIT12]);
    XRCCTRL(*this, "VtSetupBit13", wxChoice)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_BIT13]);
    XRCCTRL(*this, "VtSetupBit14", wxChoice)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_BIT14]);
    XRCCTRL(*this, "VtSetupBit15", wxChoice)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_BIT15]);
    XRCCTRL(*this, "VtSetupBit16", wxChoice)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_BIT16]);
    
    XRCCTRL(*this, "VtSetupBitsPerCharacter", wxChoice)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_BITS_PER_CHARACTER]);
    XRCCTRL(*this, "VtCharacters", wxChoice)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_CHARACTERS]);
    XRCCTRL(*this, "VtStopBits", wxTextCtrl)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_STOPBITS]);
    XRCCTRL(*this, "VtSetupCharRomButton", wxButton)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_CHAR_ROM]);
    XRCCTRL(*this, "VtSetupCharRom", wxComboBox)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_CHAR_ROM]);
    XRCCTRL(*this, "VtSetupWavButton", wxButton)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_WAV_FILE]);
    XRCCTRL(*this, "VtSetupWavFile", wxTextCtrl)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_WAV_FILE]);
    XRCCTRL(*this, "VtSetupWavEject", wxBitmapButton)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_WAV_FILE]);
    XRCCTRL(*this, "Uart1854", wxCheckBox)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_UART_1854]);
    XRCCTRL(*this, "Uart16450", wxCheckBox)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_UART_16450]);
    XRCCTRL(*this, "VtEf", wxCheckBox)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_EF]);
    XRCCTRL(*this, "VtQ", wxCheckBox)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_Q]);
#if defined(__WXMAC__) || defined(__linux__)
    XRCCTRL(*this, "VtSerialPort", wxTextCtrl)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_SERIAL_PORT]);
#else
    XRCCTRL(*this, "VtSerialPortChoice", wxChoice)->Enable(enableGui | !currentComputerConfiguration.videoTerminalConfiguration.specifiedInXml[XML_VT_SERIAL_PORT]);
#endif
}
