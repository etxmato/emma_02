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

END_EVENT_TABLE()

VtSetupDialog::VtSetupDialog(wxWindow* parent)
{
    wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"vt_" + p_Main->getFontSize() + ".xrc");
    wxXmlResource::Get()->LoadDialog(this, parent, "VtSetupDialog");

    currentComputerConfiguration = p_Main->getConfiguration();
    originalUartValue_ = currentComputerConfiguration.videoTerminalConfiguration.uart1854_defined;

    this->SetTitle("Video Terminal Setup "+p_Main->getSelectedComputerText());

    switch (currentComputerConfiguration.videoTerminalConfiguration.type)
    {
        case VT52:
            SetUpFeature_ = currentComputerConfiguration.videoTerminalConfiguration.vt52SetUpFeature;
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
            SetUpFeature_ = currentComputerConfiguration.videoTerminalConfiguration.vt100SetUpFeature;
            XRCCTRL(*this, "StaticLine3", wxStaticLine)->Hide();
#ifdef __WXMSW__
            XRCCTRL(*this, "VtSerialPortChoice", wxChoice)->Hide();
#else
            XRCCTRL(*this, "VtSerialPort", wxTextCtrl)->Hide();
#endif
            XRCCTRL(*this, "VtSerialPortText", wxStaticText)->Hide();
        break;
    }

    if (currentComputerConfiguration.videoTerminalConfiguration.external)
    {
        SetUpFeature_ = currentComputerConfiguration.videoTerminalConfiguration.vtExternalSetUpFeature;
        XRCCTRL(*this, "VtSetupBit0", wxChoice)->Hide();
        XRCCTRL(*this, "VtSetupBit0Text", wxStaticText)->Hide();
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
//        XRCCTRL(*this, "Uart1854", wxCheckBox)->Hide();
//        XRCCTRL(*this, "Uart16450", wxCheckBox)->Hide();
        XRCCTRL(*this, "VtRtcClear", wxCheckBox)->Hide();
        XRCCTRL(*this, "VtSetupCharRomButton", wxButton)->Hide();
        XRCCTRL(*this, "VtSetupCharRom", wxComboBox)->Hide();
    }

    XRCCTRL(*this, "SerialLog", wxCheckBox)->SetValue(currentComputerConfiguration.videoTerminalConfiguration.serialLog);
    XRCCTRL(*this, "ESCError", wxCheckBox)->SetValue(currentComputerConfiguration.videoTerminalConfiguration.escError);

    XRCCTRL(*this, "Uart1854", wxCheckBox)->SetValue(currentComputerConfiguration.videoTerminalConfiguration.uart1854_defined);
    XRCCTRL(*this, "Uart16450", wxCheckBox)->SetValue(currentComputerConfiguration.videoTerminalConfiguration.uart16450_defined);
    XRCCTRL(*this, "VtRtcClear", wxCheckBox)->Hide();
    XRCCTRL(*this, "VtEf", wxCheckBox)->SetValue(currentComputerConfiguration.videoTerminalConfiguration.ef.reverse != 1);
    XRCCTRL(*this, "VtQ", wxCheckBox)->SetValue(currentComputerConfiguration.videoTerminalConfiguration.reverseQ != 1);
    if (currentComputerConfiguration.videoTerminalConfiguration.uart16450_defined || currentComputerConfiguration.videoTerminalConfiguration.uart1854_defined)
    {
        XRCCTRL(*this, "VtEf", wxCheckBox)->Enable(false);
        XRCCTRL(*this, "VtQ", wxCheckBox)->Enable(false);
    }

    wxString bellFrequency;
    bellFrequency.Printf("%d", currentComputerConfiguration.videoTerminalConfiguration.bellFrequency);
    XRCCTRL(*this, "VtBell", wxTextCtrl)->ChangeValue(bellFrequency);

    switch (currentComputerConfiguration.videoTerminalConfiguration.charactersPerRow)
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

    if (currentComputerConfiguration.videoTerminalConfiguration.wavFileName != "")
    {
        XRCCTRL(*this, "VtSetupWavFile", wxTextCtrl)->ChangeValue(currentComputerConfiguration.videoTerminalConfiguration.wavFileName);
        XRCCTRL(*this, "VtBell", wxTextCtrl)->Enable(false);
    }

    if (currentComputerConfiguration.videoTerminalConfiguration.xModem_defined && !currentComputerConfiguration.videoTerminalConfiguration.external)
    {
        XRCCTRL(*this, "XmodemLine", wxStaticLine)->Show();
        XRCCTRL(*this, "VtXmodemPacketSizeText", wxStaticText)->Show();
        XRCCTRL(*this, "VtXmodemPacketSizeChoice", wxChoice)->Show();
        XRCCTRL(*this, "VtXmodemPacketSizeChoice", wxChoice)->SetSelection(currentComputerConfiguration.videoTerminalConfiguration.packetSize);
    }
    
    wxString box;
    for (int i=0; i<17; i++)
    {
        box.Printf("%d", i);
        XRCCTRL(*this, "VtSetupBit"+box, wxChoice)->SetSelection(SetUpFeature_[i]);
    }
    XRCCTRL(*this, "VtSetupCharRom", wxComboBox)->SetValue(currentComputerConfiguration.videoTerminalConfiguration.vtCharRomFileName);
    
#ifdef __WXMSW__
    listPorts();
#else
    XRCCTRL(*this, "VtSerialPort", wxTextCtrl)->ChangeValue(currentComputerConfiguration.videoTerminalConfiguration.serialPort);
#endif
}

void VtSetupDialog::onSaveButton( wxCommandEvent& WXUNUSED(event) )
{
    if (currentComputerConfiguration.videoTerminalConfiguration.wavFileName != "")
    {
        if (!p_Main->checkWavFile(currentComputerConfiguration.videoTerminalConfiguration.wavDirectory + currentComputerConfiguration.videoTerminalConfiguration.wavFileName))
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
            currentComputerConfiguration.videoTerminalConfiguration.vt52SetUpFeature = SetUpFeature_;
        break;
            
        case VT100:
            currentComputerConfiguration.videoTerminalConfiguration.vt100SetUpFeature = SetUpFeature_;
            switch (XRCCTRL(*this, "VtCharacters", wxChoice)->GetSelection())
            {
                case 0:
                    currentComputerConfiguration.videoTerminalConfiguration.charactersPerRow = 64;
                    currentComputerConfiguration.videoTerminalConfiguration.characterWidth = 10;
                break;
                case 2:
                    currentComputerConfiguration.videoTerminalConfiguration.charactersPerRow = 132;
                    currentComputerConfiguration.videoTerminalConfiguration.characterWidth = 8;
                break;
                default:
                    currentComputerConfiguration.videoTerminalConfiguration.charactersPerRow = 80;
                    currentComputerConfiguration.videoTerminalConfiguration.characterWidth = 10;
                break;
            }
        break;
    }

    if (currentComputerConfiguration.videoTerminalConfiguration.external)
            currentComputerConfiguration.videoTerminalConfiguration.vtExternalSetUpFeature = SetUpFeature_;

    currentComputerConfiguration.videoTerminalConfiguration.serialLog = XRCCTRL(*this, "SerialLog", wxCheckBox)->GetValue();
    currentComputerConfiguration.videoTerminalConfiguration.escError = XRCCTRL(*this, "ESCError", wxCheckBox)->GetValue();

    if (currentComputerConfiguration.videoTerminalConfiguration.xModem_defined)
    {
        currentComputerConfiguration.videoTerminalConfiguration.packetSize = XRCCTRL(*this, "VtXmodemPacketSizeChoice", wxChoice)->GetSelection();
    }

    currentComputerConfiguration.videoTerminalConfiguration.uart16450_defined = false;
    currentComputerConfiguration.videoTerminalConfiguration.uart1854_defined = XRCCTRL(*this, "Uart1854", wxCheckBox)->GetValue();
    currentComputerConfiguration.videoTerminalConfiguration.uart16450_defined = XRCCTRL(*this, "Uart16450", wxCheckBox)->GetValue();
    currentComputerConfiguration.clearRtc = XRCCTRL(*this, "VtRtcClear", wxCheckBox)->GetValue();
    if (XRCCTRL(*this, "VtEf", wxCheckBox)->IsChecked())
        currentComputerConfiguration.videoTerminalConfiguration.ef.reverse = 0;
    else
        currentComputerConfiguration.videoTerminalConfiguration.ef.reverse = 1;

    if (XRCCTRL(*this, "VtQ", wxCheckBox)->IsChecked())
        currentComputerConfiguration.videoTerminalConfiguration.reverseQ = 0;
    else
        currentComputerConfiguration.videoTerminalConfiguration.reverseQ = 1;

    long bellFrequency;
    wxString valueString = XRCCTRL(*this, "VtBell", wxTextCtrl)->GetValue();
    valueString.ToLong(&bellFrequency);
    currentComputerConfiguration.videoTerminalConfiguration.bellFrequency = (int)bellFrequency;

    currentComputerConfiguration.videoTerminalConfiguration.wavFileName= XRCCTRL(*this, "VtSetupWavFile", wxTextCtrl)->GetValue();
    
#ifdef __WXMSW__
   int selection = XRCCTRL(*this, "VtSerialPortChoice", wxChoice)->GetSelection();
   if (selection != -1)
		currentComputerConfiguration.videoTerminalConfiguration.serialPort= XRCCTRL(*this, "VtSerialPortChoice", wxChoice)->GetString(selection);
#else
    currentComputerConfiguration.videoTerminalConfiguration.serialPort= XRCCTRL(*this, "VtSerialPort", wxTextCtrl)->GetValue();
#endif

    p_Main->setConfiguration(currentComputerConfiguration);
    p_Main->writeDefaultVtConfig();

    EndModal(wxID_OK);
}

void VtSetupDialog::onVtWavFile(wxCommandEvent& event)
{
    currentComputerConfiguration.videoTerminalConfiguration.wavFileName = event.GetString();

    if (currentComputerConfiguration.videoTerminalConfiguration.wavFileName == "")
        XRCCTRL(*this, "VtBell", wxTextCtrl)->Enable(true);
    else
        XRCCTRL(*this, "VtBell", wxTextCtrl)->Enable(false);
}

void VtSetupDialog::onVtWavFileButton(wxCommandEvent& WXUNUSED(event))
{
    wxString fileName;

    fileName = wxFileSelector("Select the WAV file for the bell sound",
        currentComputerConfiguration.videoTerminalConfiguration.wavDirectory, currentComputerConfiguration.videoTerminalConfiguration.wavFileName,
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
    currentComputerConfiguration.videoTerminalConfiguration.wavDirectory = FullPath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    currentComputerConfiguration.videoTerminalConfiguration.wavFileName = FullPath.GetFullName();

    XRCCTRL(*this, "VtSetupWavFile", wxTextCtrl)->SetValue(currentComputerConfiguration.videoTerminalConfiguration.wavFileName);

    if (currentComputerConfiguration.videoTerminalConfiguration.wavFileName == "")
        XRCCTRL(*this, "VtBell", wxTextCtrl)->Enable(true);
    else
        XRCCTRL(*this, "VtBell", wxTextCtrl)->Enable(false);
    
    p_Main->checkWavFile(currentComputerConfiguration.videoTerminalConfiguration.wavDirectory + currentComputerConfiguration.videoTerminalConfiguration.wavFileName);
}

void VtSetupDialog::onVtWavFileEject(wxCommandEvent& WXUNUSED(event))
{
    currentComputerConfiguration.videoTerminalConfiguration.wavFileName = "";
    XRCCTRL(*this, "VtSetupWavFile", wxTextCtrl)->SetValue(currentComputerConfiguration.videoTerminalConfiguration.wavFileName);

    XRCCTRL(*this, "VtBell", wxTextCtrl)->Enable(true);
}

void VtSetupDialog::onVtCharRom(wxCommandEvent& WXUNUSED(event) )
{
    wxString fileName;
    wxString vtCharRomDir;
    
    vtCharRomDir = currentComputerConfiguration.videoTerminalConfiguration.vtCharRomDirectory;

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
    currentComputerConfiguration.videoTerminalConfiguration.vtCharRomDirectory = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    currentComputerConfiguration.videoTerminalConfiguration.vtCharRomFileName = FullPath.GetFullName();

    XRCCTRL(*this, "VtSetupCharRom", wxComboBox)->SetValue(currentComputerConfiguration.videoTerminalConfiguration.vtCharRomFileName);
}

void VtSetupDialog::onVtCharRomText(wxCommandEvent& WXUNUSED(event))
{
    currentComputerConfiguration.videoTerminalConfiguration.vtCharRomFileName = XRCCTRL(*this, "VtSetupCharRom", wxComboBox)->GetValue();
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
    
    int selection = XRCCTRL(*this, "VtSerialPortChoice", wxChoice)->FindString(currentComputerConfiguration.videoTerminalConfiguration.serialPort);
    if (selection != wxNOT_FOUND)
        XRCCTRL(*this, "VtSerialPortChoice", wxChoice)->SetSelection(selection);
}

void VtSetupDialog::onUart1854(wxCommandEvent&event)
{
    currentComputerConfiguration.videoTerminalConfiguration.uart1854_defined = event.IsChecked();
    if (currentComputerConfiguration.videoTerminalConfiguration.uart1854_defined)
        XRCCTRL(*this, "Uart16450", wxCheckBox)->SetValue(false);
    
    if (currentComputerConfiguration.videoTerminalConfiguration.uart16450_defined || currentComputerConfiguration.videoTerminalConfiguration.uart1854_defined)
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
    currentComputerConfiguration.videoTerminalConfiguration.uart16450_defined = event.IsChecked();

    if (currentComputerConfiguration.videoTerminalConfiguration.uart16450_defined)
        XRCCTRL(*this, "Uart1854", wxCheckBox)->SetValue(false);
    
    if (currentComputerConfiguration.videoTerminalConfiguration.uart16450_defined || currentComputerConfiguration.videoTerminalConfiguration.uart1854_defined)
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
