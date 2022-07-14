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

    elfConfiguration_ = p_Main->getElfConfiguration();
    computerTypeStr_ = p_Main->getSelectedComputerStr();
    computerType_ = p_Main->getSelectedComputerId();
    originalUartValue_ = elfConfiguration_.useUart;

    this->SetTitle("Video Terminal Setup "+p_Main->getSelectedComputerText());

    switch (elfConfiguration_.vtType)
    {
        case VT52:
            SetUpFeature_ = elfConfiguration_.vt52SetUpFeature_;
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
            SetUpFeature_ = elfConfiguration_.vt100SetUpFeature_;
            XRCCTRL(*this, "StaticLine3", wxStaticLine)->Hide();
#ifdef __WXMSW__
            XRCCTRL(*this, "VtSerialPortChoice", wxChoice)->Hide();
#else
            XRCCTRL(*this, "VtSerialPort", wxTextCtrl)->Hide();
#endif
            XRCCTRL(*this, "VtSerialPortText", wxStaticText)->Hide();
        break;
    }

    if (elfConfiguration_.vtExternal)
    {
        SetUpFeature_ = elfConfiguration_.vtExternalSetUpFeature_;
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
        XRCCTRL(*this, "Uart1854", wxCheckBox)->Hide();
        XRCCTRL(*this, "UART 16450 && RTC", wxCheckBox)->Hide();
        XRCCTRL(*this, "VtRtcClear", wxCheckBox)->Hide();
    }

    XRCCTRL(*this, "SerialLog", wxCheckBox)->SetValue(elfConfiguration_.serialLog);
    XRCCTRL(*this, "ESCError", wxCheckBox)->SetValue(elfConfiguration_.escError);
    switch (computerType_)
    {
        case ELF:
        case ELFII:
        case SUPERELF:
        case DIY:
            XRCCTRL(*this, "Uart1854", wxCheckBox)->SetValue(elfConfiguration_.useUart);
            XRCCTRL(*this, "Uart16450", wxCheckBox)->SetValue(elfConfiguration_.useUart16450);
            XRCCTRL(*this, "VtRtcClear", wxCheckBox)->SetValue(elfConfiguration_.clearRtc);
            XRCCTRL(*this, "VtEf", wxCheckBox)->SetValue(elfConfiguration_.elfPortConf.vt100ReverseEf != 1);
            XRCCTRL(*this, "VtQ", wxCheckBox)->SetValue(elfConfiguration_.elfPortConf.vt100ReverseQ != 0);
            if (elfConfiguration_.useUart16450 || elfConfiguration_.useUart)
            {
                XRCCTRL(*this, "VtEf", wxCheckBox)->Enable(false);
                XRCCTRL(*this, "VtQ", wxCheckBox)->Enable(false);
            }
        break;

        case PICO:
            XRCCTRL(*this, "Uart1854", wxCheckBox)->Hide();
            XRCCTRL(*this, "Uart16450", wxCheckBox)->SetLabel("UART 16450");
            XRCCTRL(*this, "Uart16450", wxCheckBox)->SetValue(elfConfiguration_.useUart16450);
            XRCCTRL(*this, "VtRtcClear", wxCheckBox)->Hide();
            XRCCTRL(*this, "VtEf", wxCheckBox)->SetValue(elfConfiguration_.elfPortConf.vt100ReverseEf != 1);
            XRCCTRL(*this, "VtQ", wxCheckBox)->SetValue(elfConfiguration_.elfPortConf.vt100ReverseQ != 0);
            if (elfConfiguration_.useUart16450 || elfConfiguration_.useUart)
            {
                XRCCTRL(*this, "VtEf", wxCheckBox)->Enable(false);
                XRCCTRL(*this, "VtQ", wxCheckBox)->Enable(false);
            }
        break;

        case MCDS:
        case CDP18S020:
        case MICROBOARD:
            XRCCTRL(*this, "VtEf", wxCheckBox)->Hide();
            XRCCTRL(*this, "VtQ", wxCheckBox)->Hide();
            XRCCTRL(*this, "Uart1854", wxCheckBox)->Hide();
            XRCCTRL(*this, "Uart16450", wxCheckBox)->Hide();
            XRCCTRL(*this, "VtRtcClear", wxCheckBox)->Hide();
        break;
            
        case MS2000:
            XRCCTRL(*this, "VtEf", wxCheckBox)->Hide();
            XRCCTRL(*this, "VtQ", wxCheckBox)->Hide();
            XRCCTRL(*this, "Uart1854", wxCheckBox)->Hide();
            XRCCTRL(*this, "Uart16450", wxCheckBox)->Hide();
            XRCCTRL(*this, "VtRtcClear", wxCheckBox)->Hide();
        break;

        case ELF2K:
            XRCCTRL(*this, "Uart16450", wxCheckBox)->SetValue(elfConfiguration_.useUart);
            XRCCTRL(*this, "Uart16450", wxCheckBox)->SetLabel("UART 16450");
            XRCCTRL(*this, "Uart1854", wxCheckBox)->Hide();
            XRCCTRL(*this, "VtEf", wxCheckBox)->SetValue(elfConfiguration_.elfPortConf.vt100ReverseEf != 1);
            XRCCTRL(*this, "VtQ", wxCheckBox)->SetValue(elfConfiguration_.elfPortConf.vt100ReverseQ != 0);
            XRCCTRL(*this, "VtRtcClear", wxCheckBox)->Hide();
            if (elfConfiguration_.useUart16450 || elfConfiguration_.useUart)
            {
                XRCCTRL(*this, "VtEf", wxCheckBox)->Enable(false);
                XRCCTRL(*this, "VtQ", wxCheckBox)->Enable(false);
            }
        break;

        case VIP:
        case VIP2K:
            XRCCTRL(*this, "Uart16450", wxCheckBox)->Hide();
            XRCCTRL(*this, "Uart1854", wxCheckBox)->SetValue(elfConfiguration_.useUart);
            XRCCTRL(*this, "Uart1854", wxCheckBox)->SetLabel("UART");
            XRCCTRL(*this, "VtEf", wxCheckBox)->SetValue(elfConfiguration_.vtEf);
            XRCCTRL(*this, "VtQ", wxCheckBox)->SetValue(!elfConfiguration_.vtQ);
            XRCCTRL(*this, "Uart1854", wxCheckBox)->Hide();
            XRCCTRL(*this, "VtRtcClear", wxCheckBox)->Hide();
        break;
            
        case COSMICOS:
            XRCCTRL(*this, "VtCharacters", wxChoice)->Hide();
            XRCCTRL(*this, "VtCharactersText", wxStaticText)->Hide();
            XRCCTRL(*this, "VtEf", wxCheckBox)->SetValue(elfConfiguration_.vtEf);
            XRCCTRL(*this, "VtQ", wxCheckBox)->SetValue(!elfConfiguration_.vtQ);
            XRCCTRL(*this, "Uart1854", wxCheckBox)->Hide();
            XRCCTRL(*this, "Uart16450", wxCheckBox)->Hide();
            XRCCTRL(*this, "VtRtcClear", wxCheckBox)->Hide();
        break;
        
        case MEMBER:
        case VELF:
            XRCCTRL(*this, "VtEf", wxCheckBox)->SetValue(elfConfiguration_.vtEf);
            XRCCTRL(*this, "VtQ", wxCheckBox)->SetValue(!elfConfiguration_.vtQ);
            XRCCTRL(*this, "Uart1854", wxCheckBox)->Hide();
            XRCCTRL(*this, "Uart16450", wxCheckBox)->Hide();
            XRCCTRL(*this, "VtRtcClear", wxCheckBox)->Hide();
        break;
    }

    wxString bellFrequency;
    bellFrequency.Printf("%d", elfConfiguration_.bellFrequency_);
    XRCCTRL(*this, "VtBell", wxTextCtrl)->ChangeValue(bellFrequency);

    switch (elfConfiguration_.vtCharactersPerRow)
    {
        case 132:
            XRCCTRL(*this, "VtCharacters", wxChoice)->SetSelection(1);
        break;
        default:
            XRCCTRL(*this, "VtCharacters", wxChoice)->SetSelection(0);
        break;
    }

    if (elfConfiguration_.vtWavFile_ != "")
    {
        XRCCTRL(*this, "VtSetupWavFile", wxTextCtrl)->ChangeValue(elfConfiguration_.vtWavFile_);
        XRCCTRL(*this, "VtBell", wxTextCtrl)->Enable(false);
    }

    if (elfConfiguration_.useXmodem && !elfConfiguration_.vtExternal)
    {
        XRCCTRL(*this, "XmodemLine", wxStaticLine)->Show();
        XRCCTRL(*this, "VtXmodemPacketSizeText", wxStaticText)->Show();
        XRCCTRL(*this, "VtXmodemPacketSizeChoice", wxChoice)->Show();
        XRCCTRL(*this, "VtXmodemPacketSizeChoice", wxChoice)->SetSelection(elfConfiguration_.packetSize);
    }
    
    wxString box;
    for (int i=0; i<17; i++)
    {
        box.Printf("%d", i);
        XRCCTRL(*this, "VtSetupBit"+box, wxChoice)->SetSelection(SetUpFeature_[i]);
    }
    XRCCTRL(*this, "VtSetupCharRom", wxComboBox)->SetValue(elfConfiguration_.vtCharRom_);

    
#ifdef __WXMSW__
    listPorts();
#else
    XRCCTRL(*this, "VtSerialPort", wxTextCtrl)->ChangeValue(elfConfiguration_.serialPort_);
#endif
}

void VtSetupDialog::onSaveButton( wxCommandEvent& WXUNUSED(event) )
{
    if (elfConfiguration_.vtWavFile_ != "")
    {
        if (!p_Main->checkWavFile(elfConfiguration_.vtWavFileDir_ + elfConfiguration_.vtWavFile_))
            return;
    }
    
    wxString box;
    wxCommandEvent uartElf2KEvent(ON_UART_ELF2K, 808);
    wxCommandEvent uartMS2000Event(ON_UART_MS2000, 809);
    wxCommandEvent uartVipEvent(ON_UART_VIP, 810);
    wxCommandEvent uartVip2KEvent(ON_UART_VIP2K, 811);
    
    for (int i=0; i<17; i++)
    {
        box.Printf("%d", i);
        if (XRCCTRL(*this, "VtSetupBit"+box, wxChoice)->GetSelection() == 0)
            SetUpFeature_[i]  = 0;
        else
            SetUpFeature_[i]  = 1;
    }

    switch (elfConfiguration_.vtType)
    {
        case VT52:
            elfConfiguration_.vt52SetUpFeature_ = SetUpFeature_;
        break;
            
        case VT100:
            elfConfiguration_.vt100SetUpFeature_ = SetUpFeature_;
            switch (XRCCTRL(*this, "VtCharacters", wxChoice)->GetSelection())
            {
                case 1:
                    elfConfiguration_.vtCharactersPerRow = 132;
                    elfConfiguration_.vt100CharWidth = 8;
                break;
                default:
                    elfConfiguration_.vtCharactersPerRow = 80;
                    elfConfiguration_.vt100CharWidth = 10;
                break;
            }
        break;
    }

    if (elfConfiguration_.vtExternal)
            elfConfiguration_.vtExternalSetUpFeature_ = SetUpFeature_;

    elfConfiguration_.serialLog = XRCCTRL(*this, "SerialLog", wxCheckBox)->GetValue();
    elfConfiguration_.escError = XRCCTRL(*this, "ESCError", wxCheckBox)->GetValue();

    if (elfConfiguration_.useXmodem)
    {
        elfConfiguration_.packetSize = XRCCTRL(*this, "VtXmodemPacketSizeChoice", wxChoice)->GetSelection();
    }

    elfConfiguration_.useUart16450 = false;
    switch (computerType_)
    {
        case ELF:
        case ELFII:
        case SUPERELF:
        case DIY:
            elfConfiguration_.useUart = XRCCTRL(*this, "Uart1854", wxCheckBox)->GetValue();
            elfConfiguration_.useUart16450 = XRCCTRL(*this, "Uart16450", wxCheckBox)->GetValue();
            elfConfiguration_.clearRtc = XRCCTRL(*this, "VtRtcClear", wxCheckBox)->GetValue();
            if (XRCCTRL(*this, "VtEf", wxCheckBox)->IsChecked())
                elfConfiguration_.elfPortConf.vt100ReverseEf = 0;
            else
                elfConfiguration_.elfPortConf.vt100ReverseEf = 1;

            if (XRCCTRL(*this, "VtQ", wxCheckBox)->IsChecked())
                elfConfiguration_.elfPortConf.vt100ReverseQ = 1;
            else
                elfConfiguration_.elfPortConf.vt100ReverseQ = 0;
        break;

        case PICO:
            elfConfiguration_.useUart16450 = XRCCTRL(*this, "Uart16450", wxCheckBox)->GetValue();
            if (XRCCTRL(*this, "VtEf", wxCheckBox)->IsChecked())
                elfConfiguration_.elfPortConf.vt100ReverseEf = 0;
            else
                elfConfiguration_.elfPortConf.vt100ReverseEf = 1;

            if (XRCCTRL(*this, "VtQ", wxCheckBox)->IsChecked())
                elfConfiguration_.elfPortConf.vt100ReverseQ = 1;
            else
                elfConfiguration_.elfPortConf.vt100ReverseQ = 0;
        break;

        case CDP18S020:
        case MCDS:
            elfConfiguration_.useUart = false;
        break;
            
        case MS2000:
            elfConfiguration_.useUart = true;

            uartMS2000Event.SetEventObject(this);
            wxPostEvent(p_Main, uartMS2000Event);
        break;

        case ELF2K:
            elfConfiguration_.useUart = XRCCTRL(*this, "Uart16450", wxCheckBox)->GetValue();
            
            if (originalUartValue_ != elfConfiguration_.useUart)
            {
                uartElf2KEvent.SetEventObject(this);
                wxPostEvent(p_Main, uartElf2KEvent);
            }
            if (XRCCTRL(*this, "VtEf", wxCheckBox)->IsChecked())
                elfConfiguration_.elfPortConf.vt100ReverseEf = 0;
            else
                elfConfiguration_.elfPortConf.vt100ReverseEf = 1;

            if (XRCCTRL(*this, "VtQ", wxCheckBox)->IsChecked())
                elfConfiguration_.elfPortConf.vt100ReverseQ = 1;
            else
                elfConfiguration_.elfPortConf.vt100ReverseQ = 0;
        break;

        case VIP:
            elfConfiguration_.useUart = XRCCTRL(*this, "Uart1854", wxCheckBox)->GetValue();
            
            if (originalUartValue_ != elfConfiguration_.useUart)
            {
                uartVipEvent.SetEventObject(this);
                wxPostEvent(p_Main, uartVipEvent);
            }
            elfConfiguration_.vtEf = XRCCTRL(*this, "VtEf", wxCheckBox)->GetValue();
            elfConfiguration_.vtQ = !XRCCTRL(*this, "VtQ", wxCheckBox)->GetValue();
        break;

        case VIP2K:
            elfConfiguration_.useUart = XRCCTRL(*this, "Uart1854", wxCheckBox)->GetValue();
            
            if (originalUartValue_ != elfConfiguration_.useUart)
            {
                uartVip2KEvent.SetEventObject(this);
                wxPostEvent(p_Main, uartVip2KEvent);
            }
            elfConfiguration_.vtEf = XRCCTRL(*this, "VtEf", wxCheckBox)->GetValue();
            elfConfiguration_.vtQ = !XRCCTRL(*this, "VtQ", wxCheckBox)->GetValue();
        break;
            
        case COSMICOS:
        case VELF:
        case MEMBER:
            elfConfiguration_.vtEf = XRCCTRL(*this, "VtEf", wxCheckBox)->GetValue();
            elfConfiguration_.vtQ = !XRCCTRL(*this, "VtQ", wxCheckBox)->GetValue();
        break;
            
        case MICROBOARD:
        break;
    }

    long bellFrequency;
    wxString valueString = XRCCTRL(*this, "VtBell", wxTextCtrl)->GetValue();
    valueString.ToLong(&bellFrequency);
    elfConfiguration_.bellFrequency_ = (int)bellFrequency;

    elfConfiguration_.vtWavFile_= XRCCTRL(*this, "VtSetupWavFile", wxTextCtrl)->GetValue();
    
#ifdef __WXMSW__
   int selection = XRCCTRL(*this, "VtSerialPortChoice", wxChoice)->GetSelection();
    elfConfiguration_.serialPort_= XRCCTRL(*this, "VtSerialPortChoice", wxChoice)->GetString(selection);
#else
    elfConfiguration_.serialPort_= XRCCTRL(*this, "VtSerialPort", wxTextCtrl)->GetValue();
#endif

    p_Main->setSerialPorts(elfConfiguration_.serialPort_);
    p_Main->setElfConfiguration(elfConfiguration_);

    EndModal(wxID_OK);
}

void VtSetupDialog::onVtWavFile(wxCommandEvent& event)
{
    elfConfiguration_.vtWavFile_ = event.GetString();

    if (elfConfiguration_.vtWavFile_ == "")
        XRCCTRL(*this, "VtBell", wxTextCtrl)->Enable(true);
    else
        XRCCTRL(*this, "VtBell", wxTextCtrl)->Enable(false);
}

void VtSetupDialog::onVtWavFileButton(wxCommandEvent& WXUNUSED(event))
{
    wxString fileName;

    fileName = wxFileSelector("Select the WAV file for the bell sound",
        elfConfiguration_.vtWavFileDir_, elfConfiguration_.vtWavFile_,
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
    elfConfiguration_.vtWavFileDir_ = FullPath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    elfConfiguration_.vtWavFile_ = FullPath.GetFullName();

    XRCCTRL(*this, "VtSetupWavFile", wxTextCtrl)->SetValue(elfConfiguration_.vtWavFile_);

    if (elfConfiguration_.vtWavFile_ == "")
        XRCCTRL(*this, "VtBell", wxTextCtrl)->Enable(true);
    else
        XRCCTRL(*this, "VtBell", wxTextCtrl)->Enable(false);
    
    p_Main->checkWavFile(elfConfiguration_.vtWavFileDir_ + elfConfiguration_.vtWavFile_);
}

void VtSetupDialog::onVtWavFileEject(wxCommandEvent& WXUNUSED(event))
{
    elfConfiguration_.vtWavFile_ = "";
    XRCCTRL(*this, "VtSetupWavFile", wxTextCtrl)->SetValue(elfConfiguration_.vtWavFile_);

    XRCCTRL(*this, "VtBell", wxTextCtrl)->Enable(true);
}

void VtSetupDialog::onVtCharRom(wxCommandEvent& WXUNUSED(event) )
{
    wxString fileName;

    fileName = wxFileSelector( "Select the VT Character Font file to load",
                               elfConfiguration_.vtCharRomDir_, XRCCTRL(*this, "VtSetupCharRom", wxComboBox)->GetValue(),
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
    elfConfiguration_.vtCharRomDir_ = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    elfConfiguration_.vtCharRom_ = FullPath.GetFullName();

    XRCCTRL(*this, "VtSetupCharRom", wxComboBox)->SetValue(elfConfiguration_.vtCharRom_);
}

void VtSetupDialog::onVtCharRomText(wxCommandEvent& WXUNUSED(event))
{
    elfConfiguration_.vtCharRom_ = XRCCTRL(*this, "VtSetupCharRom", wxComboBox)->GetValue();
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
    
    int selection = XRCCTRL(*this, "VtSerialPortChoice", wxChoice)->FindString(elfConfiguration_.serialPort_);
    if (selection != wxNOT_FOUND)
        XRCCTRL(*this, "VtSerialPortChoice", wxChoice)->SetSelection(selection);
}

void VtSetupDialog::onUart1854(wxCommandEvent&event)
{
    elfConfiguration_.useUart = event.IsChecked();
    if (elfConfiguration_.useUart)
        XRCCTRL(*this, "Uart16450", wxCheckBox)->SetValue(false);
    
    if (elfConfiguration_.useUart16450 || elfConfiguration_.useUart)
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
    elfConfiguration_.useUart16450 = event.IsChecked();
    if (computerType_ == ELF2K)
        elfConfiguration_.useUart = elfConfiguration_.useUart16450;
    
    if (elfConfiguration_.useUart16450)
        XRCCTRL(*this, "Uart1854", wxCheckBox)->SetValue(false);
    
    if (elfConfiguration_.useUart16450 || elfConfiguration_.useUart)
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
