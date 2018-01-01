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

#include <libserialport.h>

BEGIN_EVENT_TABLE(VtSetupDialog, wxDialog)
	EVT_BUTTON(XRCID("VtSetupSave"), VtSetupDialog::onSaveButton)
	EVT_TEXT(XRCID("VtSetupWavFile"), VtSetupDialog::onVtWavFile)
	EVT_BUTTON(XRCID("VtSetupWavButton"), VtSetupDialog::onVtWavFileButton)
	EVT_BUTTON(XRCID("VtSetupWavEject"), VtSetupDialog::onVtWavFileEject)
	END_EVENT_TABLE()

VtSetupDialog::VtSetupDialog(wxWindow* parent)
{
	wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"vt.xrc");
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
		break;

		case VT100:
            SetUpFeature_ = elfConfiguration_.vt100SetUpFeature_;
		break;
	}

    XRCCTRL(*this, "SerialLog", wxCheckBox)->SetValue(elfConfiguration_.serialLog);
	switch (computerType_)
	{
		case ELF:
		case ELFII:
		case SUPERELF:
            XRCCTRL(*this, "Uart", wxCheckBox)->SetValue(elfConfiguration_.useUart);
			XRCCTRL(*this, "Uart", wxCheckBox)->SetLabel("Uart CDP1854");
			XRCCTRL(*this, "VtEf", wxCheckBox)->Hide();
			XRCCTRL(*this, "VtQ", wxCheckBox)->Hide();
		break;

        case MCDS:
            XRCCTRL(*this, "VtEf", wxCheckBox)->Hide();
            XRCCTRL(*this, "VtQ", wxCheckBox)->Hide();
            XRCCTRL(*this, "Uart", wxCheckBox)->Hide();
        break;
            
		case MS2000:
			XRCCTRL(*this, "VtEf", wxCheckBox)->Hide();
			XRCCTRL(*this, "VtQ", wxCheckBox)->Hide();
			XRCCTRL(*this, "Uart", wxCheckBox)->Hide();
		break;

		case ELF2K:
            XRCCTRL(*this, "Uart", wxCheckBox)->SetValue(elfConfiguration_.useUart);
			XRCCTRL(*this, "Uart", wxCheckBox)->SetLabel("Uart 16450");
			XRCCTRL(*this, "VtEf", wxCheckBox)->Hide();
			XRCCTRL(*this, "VtQ", wxCheckBox)->Hide();
		break;

		case COSMICOS:
		case MEMBER:
        case VIP:
        case VELF:
			XRCCTRL(*this, "VtEf", wxCheckBox)->SetValue(elfConfiguration_.vtEf);
			XRCCTRL(*this, "VtQ", wxCheckBox)->SetValue(!elfConfiguration_.vtQ);
			XRCCTRL(*this, "Uart", wxCheckBox)->Hide();
		break;
	}

    wxString bellFrequency;
    bellFrequency.Printf("%d", elfConfiguration_.bellFrequency_);
    XRCCTRL(*this, "VtBell", wxTextCtrl)->ChangeValue(bellFrequency);

	if (elfConfiguration_.vtWavFile_ != "")
	{
		XRCCTRL(*this, "VtSetupWavFile", wxTextCtrl)->ChangeValue(elfConfiguration_.vtWavFile_);
		XRCCTRL(*this, "VtBell", wxTextCtrl)->Enable(false);
	}

	wxString box;
	for (int i=0; i<17; i++)
	{
		box.Printf("%d", i);
		XRCCTRL(*this, "VtSetupBit"+box, wxChoice)->SetSelection(SetUpFeature_[i]);
	}
    
    listPorts();
}

void VtSetupDialog::onSaveButton( wxCommandEvent& WXUNUSED(event) )
{
    if (elfConfiguration_.vtWavFile_ != "")
    {
        if (!p_Main->checkWavFile(elfConfiguration_.vtWavFileDir_ + elfConfiguration_.vtWavFile_))
            return;
    }
    
    wxString box;
	wxCommandEvent uartEvent(ON_UART, 807);
    wxCommandEvent uartElf2KEvent(ON_UART_ELF2K, 808);
    wxCommandEvent uartMS2000Event(ON_UART_MS2000, 809);
    
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
        break;
    }

    elfConfiguration_.serialLog = XRCCTRL(*this, "SerialLog", wxCheckBox)->GetValue();
    
	switch (computerType_)
	{
		case ELF:
		case ELFII:
		case SUPERELF:
            elfConfiguration_.useUart = XRCCTRL(*this, "Uart", wxCheckBox)->GetValue();
            
            if (originalUartValue_ != elfConfiguration_.useUart)
            {
                uartEvent.SetEventObject(this);
                wxPostEvent(p_Main, uartEvent);
            }
        break;

        case MCDS:
            elfConfiguration_.useUart = false;
        break;
            
		case MS2000:
			elfConfiguration_.useUart = true;

			uartMS2000Event.SetEventObject(this);
			wxPostEvent(p_Main, uartMS2000Event);
		break;

		case ELF2K:
            elfConfiguration_.useUart = XRCCTRL(*this, "Uart", wxCheckBox)->GetValue();
            
            if (originalUartValue_ != elfConfiguration_.useUart)
            {
                uartElf2KEvent.SetEventObject(this);
                wxPostEvent(p_Main, uartElf2KEvent);
            }
		break;

		case COSMICOS:
        case VIP:
        case VELF:
		case MEMBER:
			elfConfiguration_.vtEf = XRCCTRL(*this, "VtEf", wxCheckBox)->GetValue();
			elfConfiguration_.vtQ = !XRCCTRL(*this, "VtQ", wxCheckBox)->GetValue();
        break;
	}

    long bellFrequency;
    wxString valueString = XRCCTRL(*this, "VtBell", wxTextCtrl)->GetValue();
    valueString.ToLong(&bellFrequency);
    elfConfiguration_.bellFrequency_ = (int)bellFrequency;

	elfConfiguration_.vtWavFile_= XRCCTRL(*this, "VtSetupWavFile", wxTextCtrl)->GetValue();
    
    int selection = XRCCTRL(*this, "VtSerialPortChoice", wxChoice)->GetSelection();
    elfConfiguration_.serialPort_= XRCCTRL(*this, "VtSerialPortChoice", wxChoice)->GetString(selection);

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

void VtSetupDialog::listPorts()
{
/*    int i;
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
        XRCCTRL(*this, "VtSerialPortChoice", wxChoice)->SetSelection(selection);*/
}

