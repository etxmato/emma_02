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
#include "ports.h"

#include "wx/xrc/xmlres.h"             
#include "wx/spinctrl.h"
#include "elfconfiguration.h"

BEGIN_EVENT_TABLE(DevicePortsDialog, wxDialog)
    EVT_BUTTON(XRCID("DevicePorts_Save"), DevicePortsDialog::onSaveButton)
    EVT_CHOICE(XRCID("MC6847OutputMode"), DevicePortsDialog::onMC6847OutputMode)
    EVT_CHOICE(XRCID("MC6847-B7"), DevicePortsDialog::onMC6847B7)
    EVT_CHOICE(XRCID("MC6847-B6"), DevicePortsDialog::onMC6847B6)
    EVT_CHOICE(XRCID("MC6847-B5"), DevicePortsDialog::onMC6847B5)
    EVT_CHOICE(XRCID("MC6847-B4"), DevicePortsDialog::onMC6847B4)
    EVT_CHOICE(XRCID("MC6847-B3"), DevicePortsDialog::onMC6847B3)
    EVT_CHOICE(XRCID("MC6847-B2"), DevicePortsDialog::onMC6847B2)
    EVT_CHOICE(XRCID("MC6847-B1"), DevicePortsDialog::onMC6847B1)
    EVT_CHOICE(XRCID("MC6847-B0"), DevicePortsDialog::onMC6847B0)
    EVT_CHOICE(XRCID("MC6847-DD7"), DevicePortsDialog::onMC6847DD7)
    EVT_CHOICE(XRCID("MC6847-DD6"), DevicePortsDialog::onMC6847DD6)
END_EVENT_TABLE()

DevicePortsDialog::DevicePortsDialog(wxWindow* parent)
{
    wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"ports_" + p_Main->getFontSize() + ".xrc");
    elfConfiguration = p_Main->getElfConfiguration();
    elfTypeStr_ = p_Main->getSelectedComputerStr();
    
    if (elfTypeStr_ == "Pico")
        wxXmlResource::Get()->LoadDialog(this, parent, "DevicePortsPico");
    else
    {
        if (elfTypeStr_ == "Elf2K")
            wxXmlResource::Get()->LoadDialog(this, parent, "DevicePortsElf2K");
        else
            wxXmlResource::Get()->LoadDialog(this, parent, "DevicePorts");
    }

    this->SetTitle("Device Ports Definition "+p_Main->getSelectedComputerText());

    XRCCTRL(*this, "UnusedEf1", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.ef1default);
    XRCCTRL(*this, "UnusedEf2", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.ef2default);
    XRCCTRL(*this, "UnusedEf3", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.ef3default);

    XRCCTRL(*this, "PixieInput", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.pixieInput);
    XRCCTRL(*this, "PixieOutput", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.pixieOutput);
    XRCCTRL(*this, "PixieEF", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.pixieEf);
    if (!elfConfiguration.usePixie)
    {
        XRCCTRL(*this, "PixieInput", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "PixieOutput", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "PixieEF", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "PixieInputText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "PixieOutputText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "PixieEfText", wxStaticText)->Enable(false);
        
        if (elfTypeStr_ == "SuperElf")
        {
            XRCCTRL(*this, "PixieInputText", wxStaticText)->SetLabel("In/Out, Enable Graphics");
            XRCCTRL(*this, "PixieOutputText", wxStaticText)->SetLabel("In/Out, Disable Graphics");
        }

    }    

    XRCCTRL(*this, "PortExtenderSelectOutput", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.portExtenderSelectOutput);
    XRCCTRL(*this, "PortExtenderWriteOutput", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.portExtenderWriteOutput);
    XRCCTRL(*this, "PortExtenderInput", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.portExtenderInput);
    if (!elfConfiguration.usePortExtender)
    {
        XRCCTRL(*this, "PortExtenderSelectOutput", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "PortExtenderWriteOutput", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "PortExtenderInput", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "PortExtenderSelectOutputText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "PortExtenderWriteOutputText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "PortExtenderInputText", wxStaticText)->Enable(false);
    }    
   
    XRCCTRL(*this, "IdeSelectOutput", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.ideSelectOutput);
    XRCCTRL(*this, "IdeWriteOutput", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.ideWriteOutput);
    XRCCTRL(*this, "IdeStatus", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.ideStatus);
    XRCCTRL(*this, "IdeInput", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.ideInput);
    XRCCTRL(*this, "IdeTracks", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.ideTracks);
    XRCCTRL(*this, "IdeHeads", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.ideHeads);
    XRCCTRL(*this, "IdeSectors", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.ideSectors);
    
    if (!elfConfiguration.ideEnabled)
    {
        XRCCTRL(*this, "IdeSelectOutput", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "IdeWriteOutput", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "IdeInput", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "IdeStatus", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "IdeTracks", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "IdeHeads", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "IdeSectors", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "IdeSelectOutputText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "IdeWriteOutputText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "IdeInputText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "IdeStatusText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "text_tr", wxStaticText)->Enable(false);
        XRCCTRL(*this, "text_hd", wxStaticText)->Enable(false);
        XRCCTRL(*this, "text_sc", wxStaticText)->Enable(false);
    }    
    if (elfTypeStr_ != "Elf2K")
    {
        XRCCTRL(*this, "IdeStatus", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "IdeStatusText", wxStaticText)->Enable(false);
    }

    XRCCTRL(*this, "FdcSelectOutput", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.fdcSelectOutput.portNumber);
    XRCCTRL(*this, "FdcWriteOutput", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.fdcWriteOutput.portNumber);
    XRCCTRL(*this, "FdcInput", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.fdcReadInput.portNumber);
    XRCCTRL(*this, "FdcEf", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.fdcEf);
    if (!elfConfiguration.fdc1793Enabled)
    {
        XRCCTRL(*this, "FdcSelectOutput", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "FdcWriteOutput", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "FdcInput", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "FdcEf", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "FdcSelectOutputText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "FdcWriteOutputText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "FdcInputText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "FdcEfText", wxStaticText)->Enable(false);
    }    

    XRCCTRL(*this, "KeyboardInput", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.keyboardInput);
    XRCCTRL(*this, "KeyboardEf", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.keyboardEf);
    if (!elfConfiguration.useKeyboard && !elfConfiguration.usePs2gpio)
    {
        XRCCTRL(*this, "KeyboardInput", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "KeyboardEf", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "KeyboardInputText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "KeyboardEfText", wxStaticText)->Enable(false);
    }    

    XRCCTRL(*this, "Ps2KeyboardInput", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.ps2KeyboardInput);
    XRCCTRL(*this, "Ps2KeyboardOutput", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.ps2KeyboardOutput);
    XRCCTRL(*this, "Ps2KeyboardEf", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.ps2KeyboardEf);
    if (!elfConfiguration.usePS2)
    {
        XRCCTRL(*this, "Ps2KeyboardInput", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "Ps2KeyboardOutput", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "Ps2KeyboardEf", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "Ps2KeyboardInputText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "Ps2KeyboardOutputText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "Ps2KeyboardEfText", wxStaticText)->Enable(false);
    }    

    XRCCTRL(*this, "PrinterOutput", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.printerOutput);
    XRCCTRL(*this, "PrinterEf", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.printerEf);
    if (!p_Main->getPrinterStatus(p_Main->getSelectedComputerId()))
    {
        XRCCTRL(*this, "PrinterOutput", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "PrinterOutputText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "PrinterEf", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "PrinterEfText", wxStaticText)->Enable(false);
    }
    if (elfConfiguration.useEms)
        XRCCTRL(*this, "EmsOutput", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.emsOutput[0]);
    else
    {
        XRCCTRL(*this, "EmsOutput", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "EmsOutputText", wxStaticText)->Enable(false);
    }    

    XRCCTRL(*this, "Vt100Output", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.vt100Output);
    if (elfTypeStr_ != "Elf2K")
    {
        XRCCTRL(*this, "Vt100Ef", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.vt100Ef);
    }
    else
    {
        XRCCTRL(*this, "Vt100Ef", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.vt100Ef);
        XRCCTRL(*this, "Vt100Output", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "Vt100OutputText", wxStaticText)->Enable(false);
    }
    XRCCTRL(*this, "Vt100ReverseEf", wxCheckBox)->SetValue(elfConfiguration.ioConfiguration.vt100ReverseEf != 1);
    XRCCTRL(*this, "Vt100ReverseQ", wxCheckBox)->SetValue(elfConfiguration.ioConfiguration.vt100ReverseQ != 0);
    if ((elfConfiguration.vtType == VTNONE) || elfConfiguration.useUart || elfConfiguration.useUart16450)
    {
        XRCCTRL(*this, "Vt100Q", wxStaticText)->Enable(false);
        XRCCTRL(*this, "Vt100Output", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "Vt100Ef", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "Vt100ReverseQText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "Vt100OutputText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "Vt100EfText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "Vt100ReverseEf", wxCheckBox)->Enable(false);
        XRCCTRL(*this, "Vt100ReverseQ", wxCheckBox)->Enable(false);
    }    

    XRCCTRL(*this, "UartOut", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.uartOut);
    XRCCTRL(*this, "UartIn", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.uartIn);
    XRCCTRL(*this, "UartControl", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.uartControl);
    XRCCTRL(*this, "UartStatus", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.uartStatus);
    if ((!elfConfiguration.useUart && !elfConfiguration.useUart16450) || (elfTypeStr_ == "Elf2K"))
    {
        XRCCTRL(*this, "UartOutText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "UartOutText16450", wxStaticText)->Enable(false);
        XRCCTRL(*this, "UartOut", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "UartInText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "UartInText16450", wxStaticText)->Enable(false);
        XRCCTRL(*this, "UartIn", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "UartControlText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "UartControlText16450", wxStaticText)->Enable(false);
        XRCCTRL(*this, "UartControl", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "UartStatusText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "UartStatusText16450", wxStaticText)->Enable(false);
        XRCCTRL(*this, "UartStatus", wxSpinCtrl)->Enable(false);
    }
    if (elfConfiguration.useUart16450)
    {
        XRCCTRL(*this, "UartOutText", wxStaticText)->Hide();
        XRCCTRL(*this, "UartInText", wxStaticText)->Hide();
        XRCCTRL(*this, "UartControlText", wxStaticText)->Hide();
        XRCCTRL(*this, "UartStatusText", wxStaticText)->Hide();
        XRCCTRL(*this, "UartOutText16450", wxStaticText)->Show();
        XRCCTRL(*this, "UartInText16450", wxStaticText)->Show();
        XRCCTRL(*this, "UartControlText16450", wxStaticText)->Show();
        XRCCTRL(*this, "UartStatusText16450", wxStaticText)->Show();
    }
    else
    {
        XRCCTRL(*this, "UartOutText16450", wxStaticText)->Hide();
        XRCCTRL(*this, "UartInText16450", wxStaticText)->Hide();
        XRCCTRL(*this, "UartControlText16450", wxStaticText)->Hide();
        XRCCTRL(*this, "UartStatusText16450", wxStaticText)->Hide();
        XRCCTRL(*this, "UartOutText", wxStaticText)->Show();
        XRCCTRL(*this, "UartInText", wxStaticText)->Show();
        XRCCTRL(*this, "UartControlText", wxStaticText)->Show();
        XRCCTRL(*this, "UartStatusText", wxStaticText)->Show();
        
    }

    XRCCTRL(*this, "TmsModeHighOutput", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.tmsModeHighOutput);
    XRCCTRL(*this, "TmsModeLowOutput", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.tmsModeLowOutput);

    XRCCTRL(*this, "TmsInterrupt", wxChoice)->SetSelection(elfConfiguration.ioConfiguration.tmsInterrupt);
    if (!elfConfiguration.useTMS9918)
    {
        XRCCTRL(*this, "TmsModeHighOutput", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "TmsModeLowOutput", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "TmsInterrupt", wxChoice)->Enable(false);
        XRCCTRL(*this, "TmsModeHighOutputText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "TmsModeLowOutputText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "TmsInterruptText", wxStaticText)->Enable(false);
    }


    XRCCTRL(*this, "I8275WriteCommand", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.i8275WriteCommand);
    XRCCTRL(*this, "I8275ReadStatus", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.i8275ReadStatus);
    XRCCTRL(*this, "I8275WriteParameter", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.i8275WriteParameter);
    XRCCTRL(*this, "I8275ReadParameter", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.i8275ReadParameter);
    XRCCTRL(*this, "I8275VerticalRetrace", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.i8275VerticalRetrace);
    if (!elfConfiguration.use8275)
    {
        XRCCTRL(*this, "I8275WriteCommand", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "I8275ReadStatus", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "I8275WriteParameter", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "I8275ReadParameter", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "I8275VerticalRetrace", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "I8275WriteCommandText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "I8275ReadStatusText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "I8275WriteParameterText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "I8275ReadParameterText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "I8275VerticalRetraceText", wxStaticText)->Enable(false);
    }    

    XRCCTRL(*this, "Led_Module_Output", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.led_Module_Output);
    if (!elfConfiguration.useLedModule)
    {
        XRCCTRL(*this, "Led_Module_Output", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "Led_Module_Output_Text", wxStaticText)->Enable(false);
    }    

    XRCCTRL(*this, "MC6847OutputMode", wxChoice)->SetSelection(elfConfiguration.ioConfiguration.mc6847OutputMode);
    if (elfConfiguration.ioConfiguration.mc6847OutputMode == 1)
        XRCCTRL(*this, "MC6847Output", wxSpinCtrl)->Enable(false);
    XRCCTRL(*this, "MC6847Output", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.mc6847Output);
    XRCCTRL(*this, "MC6847-B7", wxChoice)->SetSelection(elfConfiguration.ioConfiguration.mc6847b7);
    XRCCTRL(*this, "MC6847-B6", wxChoice)->SetSelection(elfConfiguration.ioConfiguration.mc6847b6);
    XRCCTRL(*this, "MC6847-B5", wxChoice)->SetSelection(elfConfiguration.ioConfiguration.mc6847b5);
    XRCCTRL(*this, "MC6847-B4", wxChoice)->SetSelection(elfConfiguration.ioConfiguration.mc6847b4);
    XRCCTRL(*this, "MC6847-B3", wxChoice)->SetSelection(elfConfiguration.ioConfiguration.mc6847b3);
    XRCCTRL(*this, "MC6847-B2", wxChoice)->SetSelection(elfConfiguration.ioConfiguration.mc6847b2);
    XRCCTRL(*this, "MC6847-B1", wxChoice)->SetSelection(elfConfiguration.ioConfiguration.mc6847b1);
    XRCCTRL(*this, "MC6847-B0", wxChoice)->SetSelection(elfConfiguration.ioConfiguration.mc6847b0);
    XRCCTRL(*this, "MC6847-DD7", wxChoice)->SetSelection(elfConfiguration.ioConfiguration.mc6847dd7);
    XRCCTRL(*this, "MC6847-DD6", wxChoice)->SetSelection(elfConfiguration.ioConfiguration.mc6847dd6);

    XRCCTRL(*this, "MC6847-AG", wxCheckBox)->SetValue(elfConfiguration.ioConfiguration.forceHighAg);
    XRCCTRL(*this, "MC6847-AS", wxCheckBox)->SetValue(elfConfiguration.ioConfiguration.forceHighAs);
    XRCCTRL(*this, "MC6847-EXT", wxCheckBox)->SetValue(elfConfiguration.ioConfiguration.forceHighExt);
    XRCCTRL(*this, "MC6847-GM2", wxCheckBox)->SetValue(elfConfiguration.ioConfiguration.forceHighGm2);
    XRCCTRL(*this, "MC6847-GM1", wxCheckBox)->SetValue(elfConfiguration.ioConfiguration.forceHighGm1);
    XRCCTRL(*this, "MC6847-GM0", wxCheckBox)->SetValue(elfConfiguration.ioConfiguration.forceHighGm0);
    XRCCTRL(*this, "MC6847-CSS", wxCheckBox)->SetValue(elfConfiguration.ioConfiguration.forceHighCss);
    XRCCTRL(*this, "MC6847-INV", wxCheckBox)->SetValue(elfConfiguration.ioConfiguration.forceHighInv);

    int start = elfConfiguration.ioConfiguration.mc6847StartRam;
    int sel = (start - 0x8000)/0x1000;
    XRCCTRL(*this, "MC6847StartRam", wxChoice)->SetSelection(sel);

    int end = elfConfiguration.ioConfiguration.mc6847EndRam;
    int size = end - start;
    sel = 0;
    if (size <= 1024)
        sel = 0;
    else if (size <= 1536)
        sel = 1;
    else if (size <= 2048)
        sel = 2;
    else if (size <= 3072)
        sel = 3;
    else if (size <= 6144)
        sel = 4;
    XRCCTRL(*this, "MC6847SizeRam", wxChoice)->SetSelection(sel);

    if (!elfConfiguration.use6847)
    {
        XRCCTRL(*this, "MC6847OutputMode", wxChoice)->Enable(false);
        XRCCTRL(*this, "MC6847Output", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "MC6847OutputText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "MC6847-ForceHighText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "MC6847-B7Text", wxStaticText)->Enable(false);
        XRCCTRL(*this, "MC6847-B6Text", wxStaticText)->Enable(false);
        XRCCTRL(*this, "MC6847-B5Text", wxStaticText)->Enable(false);
        XRCCTRL(*this, "MC6847-B4Text", wxStaticText)->Enable(false);
        XRCCTRL(*this, "MC6847-B3Text", wxStaticText)->Enable(false);
        XRCCTRL(*this, "MC6847-B2Text", wxStaticText)->Enable(false);
        XRCCTRL(*this, "MC6847-B1Text", wxStaticText)->Enable(false);
        XRCCTRL(*this, "MC6847-B0Text", wxStaticText)->Enable(false);
        XRCCTRL(*this, "MC6847-DD7Text", wxStaticText)->Enable(false);
        XRCCTRL(*this, "MC6847-DD6Text", wxStaticText)->Enable(false);
        XRCCTRL(*this, "MC6847-B7", wxChoice)->Enable(false);
        XRCCTRL(*this, "MC6847-B6", wxChoice)->Enable(false);
        XRCCTRL(*this, "MC6847-B5", wxChoice)->Enable(false);
        XRCCTRL(*this, "MC6847-B4", wxChoice)->Enable(false);
        XRCCTRL(*this, "MC6847-B3", wxChoice)->Enable(false);
        XRCCTRL(*this, "MC6847-B2", wxChoice)->Enable(false);
        XRCCTRL(*this, "MC6847-B1", wxChoice)->Enable(false);
        XRCCTRL(*this, "MC6847-B0", wxChoice)->Enable(false);
        XRCCTRL(*this, "MC6847-DD7", wxChoice)->Enable(false);
        XRCCTRL(*this, "MC6847-DD6", wxChoice)->Enable(false);
        XRCCTRL(*this, "MC6847-AG", wxCheckBox)->Enable(false);
        XRCCTRL(*this, "MC6847-AS", wxCheckBox)->Enable(false);
        XRCCTRL(*this, "MC6847-EXT", wxCheckBox)->Enable(false);
        XRCCTRL(*this, "MC6847-GM2", wxCheckBox)->Enable(false);
        XRCCTRL(*this, "MC6847-GM1", wxCheckBox)->Enable(false);
        XRCCTRL(*this, "MC6847-GM0", wxCheckBox)->Enable(false);
        XRCCTRL(*this, "MC6847-CSS", wxCheckBox)->Enable(false);
        XRCCTRL(*this, "MC6847-INV", wxCheckBox)->Enable(false);
        XRCCTRL(*this, "MC6847StartRam", wxChoice)->Enable(false);
        XRCCTRL(*this, "MC6847StartRamText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "MC6847SizeRam", wxChoice)->Enable(false);
        XRCCTRL(*this, "MC6847SizeRamText", wxStaticText)->Enable(false);
    }    

    start = elfConfiguration.ioConfiguration.mc6845StartRam;
    sel = start/0x1000;
    XRCCTRL(*this, "MC6845StartRam", wxChoice)->SetSelection(sel);

    end = elfConfiguration.ioConfiguration.mc6845EndRam;
    size = end - start;
    sel = 0;
    if (size <= 1024)
        sel = 0;
    else if (size <= 2048)
        sel = 1;
    else if (size <= 4096)
        sel = 2;
    else if (size <= 8192)
        sel = 3;
    else if (size <= 16384)
        sel = 4;
    XRCCTRL(*this, "MC6845SizeRam", wxChoice)->SetSelection(sel);

    int addr = elfConfiguration.ioConfiguration.mc6845Address;
    wxString addrStr;
    addrStr.Printf("%04X", addr);
    XRCCTRL(*this, "MC6845Address", wxTextCtrl)->SetValue(addrStr);
    int data = elfConfiguration.ioConfiguration.mc6845Data;
    wxString dataStr;
    dataStr.Printf("%04X", data);
    XRCCTRL(*this, "MC6845Data", wxTextCtrl)->SetValue(dataStr);
    XRCCTRL(*this, "MC6845EF", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.mc6845Ef);

    if (!elfConfiguration.use6845)
    {
        XRCCTRL(*this, "MC6845StartRam", wxChoice)->Enable(false);
        XRCCTRL(*this, "MC6845StartRamText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "MC6845SizeRam", wxChoice)->Enable(false);
        XRCCTRL(*this, "MC6845SizeRamText", wxStaticText)->Enable(false);

        XRCCTRL(*this, "MC6845AddressText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "MC6845Address", wxTextCtrl)->Enable(false);
        XRCCTRL(*this, "MC6845DataText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "MC6845Data", wxTextCtrl)->Enable(false);

        XRCCTRL(*this, "MC6845EfText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "MC6845EF", wxSpinCtrl)->Enable(false);
    }    

    if (elfConfiguration.useS100)
    {
        XRCCTRL(*this, "MC6845StartRam", wxChoice)->Enable(true);
        XRCCTRL(*this, "MC6845StartRamText", wxStaticText)->Enable(true);
        XRCCTRL(*this, "MC6845SizeRam", wxChoice)->Enable(true);
        XRCCTRL(*this, "MC6845SizeRamText", wxStaticText)->Enable(true);
    }    

    XRCCTRL(*this, "HexOutput", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.hexOutput);
    XRCCTRL(*this, "HexInput", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.hexInput);
    XRCCTRL(*this, "HexEf", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.hexEf);
    if (!(elfConfiguration.useHexKeyboardEf3 || elfConfiguration.useHexKeyboard) && elfTypeStr_ == "Elf")
    {
        XRCCTRL(*this, "HexInputText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "HexInput", wxSpinCtrl)->Enable(false);
    }
    if (!elfConfiguration.useHexKeyboardEf3)
    {
        XRCCTRL(*this, "HexEfText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "HexEf", wxSpinCtrl)->Enable(false);
    }
    
    XRCCTRL(*this, "TapeEf", wxSpinCtrl)->SetValue(elfConfiguration.ioConfiguration.tapeEf);
    if (!elfConfiguration.useTape || elfConfiguration.useXmodem)
    {
        XRCCTRL(*this, "TapeEf", wxSpinCtrl)->Enable(false);
        XRCCTRL(*this, "TapeEfText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "TapeQ", wxStaticText)->Enable(false);
        XRCCTRL(*this, "TapeQText", wxStaticText)->Enable(false);
    }
}

void DevicePortsDialog::onSaveButton( wxCommandEvent& WXUNUSED(event) )
{
    long value;

    wxString strValue = XRCCTRL(*this, "MC6845Address", wxTextCtrl)->GetValue();
    if (strValue != "")
    {
        if (strValue.ToLong(&value, 16))
            elfConfiguration.ioConfiguration.mc6845Address = (int)value;
    }

    strValue = XRCCTRL(*this, "MC6845Data", wxTextCtrl)->GetValue();
    if (strValue != "")
    {
        if (strValue.ToLong(&value, 16))
            elfConfiguration.ioConfiguration.mc6845Data = (int)value;
    }

    elfConfiguration.ioConfiguration.ef1default = XRCCTRL(*this, "UnusedEf1", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.ef2default = XRCCTRL(*this, "UnusedEf2", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.ef3default = XRCCTRL(*this, "UnusedEf3", wxSpinCtrl)->GetValue();

    elfConfiguration.ioConfiguration.pixieInput = XRCCTRL(*this, "PixieInput", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.pixieOutput = XRCCTRL(*this, "PixieOutput", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.pixieEf = XRCCTRL(*this, "PixieEF", wxSpinCtrl)->GetValue();

    elfConfiguration.ioConfiguration.portExtenderSelectOutput = XRCCTRL(*this, "PortExtenderSelectOutput", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.portExtenderWriteOutput = XRCCTRL(*this, "PortExtenderWriteOutput", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.portExtenderInput = XRCCTRL(*this, "PortExtenderInput", wxSpinCtrl)->GetValue();

    elfConfiguration.ioConfiguration.ideSelectOutput = XRCCTRL(*this, "IdeSelectOutput", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.ideWriteOutput = XRCCTRL(*this, "IdeWriteOutput", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.ideStatus = XRCCTRL(*this, "IdeStatus", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.ideInput = XRCCTRL(*this, "IdeInput", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.ideTracks = XRCCTRL(*this, "IdeTracks", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.ideHeads = XRCCTRL(*this, "IdeHeads", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.ideSectors = XRCCTRL(*this, "IdeSectors", wxSpinCtrl)->GetValue();

    elfConfiguration.ioConfiguration.fdcSelectOutput.portNumber = XRCCTRL(*this, "FdcSelectOutput", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.fdcWriteOutput.portNumber = XRCCTRL(*this, "FdcWriteOutput", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.fdcReadInput.portNumber = XRCCTRL(*this, "FdcInput", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.fdcEf = XRCCTRL(*this, "FdcEf", wxSpinCtrl)->GetValue();

    elfConfiguration.ioConfiguration.keyboardInput = XRCCTRL(*this, "KeyboardInput", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.keyboardEf = XRCCTRL(*this, "KeyboardEf", wxSpinCtrl)->GetValue();

    elfConfiguration.ioConfiguration.ps2KeyboardInput = XRCCTRL(*this, "Ps2KeyboardInput", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.ps2KeyboardOutput = XRCCTRL(*this, "Ps2KeyboardOutput", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.ps2KeyboardEf = XRCCTRL(*this, "Ps2KeyboardEf", wxSpinCtrl)->GetValue();

    elfConfiguration.ioConfiguration.i8275WriteCommand = XRCCTRL(*this, "I8275WriteCommand", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.i8275ReadStatus = XRCCTRL(*this, "I8275ReadStatus", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.i8275WriteParameter = XRCCTRL(*this, "I8275WriteParameter", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.i8275ReadParameter = XRCCTRL(*this, "I8275ReadParameter", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.i8275VerticalRetrace = XRCCTRL(*this, "I8275VerticalRetrace", wxSpinCtrl)->GetValue();

    elfConfiguration.ioConfiguration.printerOutput = XRCCTRL(*this, "PrinterOutput", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.printerEf = XRCCTRL(*this, "PrinterEf", wxSpinCtrl)->GetValue();

    if (elfConfiguration.useEms)
        elfConfiguration.ioConfiguration.emsOutput[0] = XRCCTRL(*this, "EmsOutput", wxSpinCtrl)->GetValue();

    elfConfiguration.ioConfiguration.vt100Output = XRCCTRL(*this, "Vt100Output", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.vt100Ef = XRCCTRL(*this, "Vt100Ef", wxSpinCtrl)->GetValue();

    if (XRCCTRL(*this, "Vt100ReverseEf", wxCheckBox)->IsChecked())
        elfConfiguration.ioConfiguration.vt100ReverseEf = 0;
    else
        elfConfiguration.ioConfiguration.vt100ReverseEf = 1;

    if (XRCCTRL(*this, "Vt100ReverseQ", wxCheckBox)->IsChecked())
        elfConfiguration.ioConfiguration.vt100ReverseQ = 1;
    else
        elfConfiguration.ioConfiguration.vt100ReverseQ = 0;

    elfConfiguration.ioConfiguration.uartOut = XRCCTRL(*this, "UartOut", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.uartIn = XRCCTRL(*this, "UartIn", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.uartControl = XRCCTRL(*this, "UartControl", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.uartStatus = XRCCTRL(*this, "UartStatus", wxSpinCtrl)->GetValue();

    elfConfiguration.ioConfiguration.tmsModeHighOutput = XRCCTRL(*this, "TmsModeHighOutput", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.tmsModeLowOutput = XRCCTRL(*this, "TmsModeLowOutput", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.tmsInterrupt = XRCCTRL(*this, "TmsInterrupt", wxChoice)->GetCurrentSelection();

    elfConfiguration.ioConfiguration.led_Module_Output = XRCCTRL(*this, "Led_Module_Output", wxSpinCtrl)->GetValue();

    elfConfiguration.ioConfiguration.mc6847OutputMode = XRCCTRL(*this, "MC6847OutputMode", wxChoice)->GetCurrentSelection();
    elfConfiguration.ioConfiguration.mc6847Output = XRCCTRL(*this, "MC6847Output", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.mc6847b7 = XRCCTRL(*this, "MC6847-B7", wxChoice)->GetCurrentSelection();
    elfConfiguration.ioConfiguration.mc6847b6 = XRCCTRL(*this, "MC6847-B6", wxChoice)->GetCurrentSelection();
    elfConfiguration.ioConfiguration.mc6847b5 = XRCCTRL(*this, "MC6847-B5", wxChoice)->GetCurrentSelection();
    elfConfiguration.ioConfiguration.mc6847b4 = XRCCTRL(*this, "MC6847-B4", wxChoice)->GetCurrentSelection();
    elfConfiguration.ioConfiguration.mc6847b3 = XRCCTRL(*this, "MC6847-B3", wxChoice)->GetCurrentSelection();
    elfConfiguration.ioConfiguration.mc6847b2 = XRCCTRL(*this, "MC6847-B2", wxChoice)->GetCurrentSelection();
    elfConfiguration.ioConfiguration.mc6847b1 = XRCCTRL(*this, "MC6847-B1", wxChoice)->GetCurrentSelection();
    elfConfiguration.ioConfiguration.mc6847b0 = XRCCTRL(*this, "MC6847-B0", wxChoice)->GetCurrentSelection();
    elfConfiguration.ioConfiguration.mc6847dd7 = XRCCTRL(*this, "MC6847-DD7", wxChoice)->GetCurrentSelection();
    elfConfiguration.ioConfiguration.mc6847dd6 = XRCCTRL(*this, "MC6847-DD6", wxChoice)->GetCurrentSelection();

    elfConfiguration.ioConfiguration.forceHighAg =  XRCCTRL(*this, "MC6847-AG", wxCheckBox)->IsChecked();
    elfConfiguration.ioConfiguration.forceHighAs =  XRCCTRL(*this, "MC6847-AS", wxCheckBox)->IsChecked();
    elfConfiguration.ioConfiguration.forceHighExt =  XRCCTRL(*this, "MC6847-EXT", wxCheckBox)->IsChecked();
    elfConfiguration.ioConfiguration.forceHighGm2 =  XRCCTRL(*this, "MC6847-GM2", wxCheckBox)->IsChecked();
    elfConfiguration.ioConfiguration.forceHighGm1 =  XRCCTRL(*this, "MC6847-GM1", wxCheckBox)->IsChecked();
    elfConfiguration.ioConfiguration.forceHighGm0 =  XRCCTRL(*this, "MC6847-GM0", wxCheckBox)->IsChecked();
    elfConfiguration.ioConfiguration.forceHighCss =  XRCCTRL(*this, "MC6847-CSS", wxCheckBox)->IsChecked();
    elfConfiguration.ioConfiguration.forceHighInv =  XRCCTRL(*this, "MC6847-INV", wxCheckBox)->IsChecked();

    int sel = XRCCTRL(*this, "MC6847StartRam", wxChoice)->GetCurrentSelection();
    int start = (sel * 0x1000) + 0x8000;
    elfConfiguration.ioConfiguration.mc6847StartRam = start;

    int size = 0x3ff;
    sel = XRCCTRL(*this, "MC6847SizeRam", wxChoice)->GetCurrentSelection();
    switch (sel)
    {
        case 0:
            size = 0x3ff;
        break;
        case 1:
            size = 0x5ff;
        break;
        case 2:
            size = 0x7ff;
        break;
        case 3:
            size = 0xbff;
        break;
        case 4:
            size = 0x17ff;
        break;
    }
    int end = start + size;
    if (end > 0xffff)  end = 0xffff;
    elfConfiguration.ioConfiguration.mc6847EndRam = end;

    sel = XRCCTRL(*this, "MC6845StartRam", wxChoice)->GetCurrentSelection();
    start = sel * 0x1000;
    elfConfiguration.ioConfiguration.mc6845StartRam = start;

    size = 0x3ff;
    sel = XRCCTRL(*this, "MC6845SizeRam", wxChoice)->GetCurrentSelection();
    switch (sel)
    {
        case 0:
            size = 0x3ff;
        break;
        case 1:
            size = 0x7ff;
        break;
        case 2:
            size = 0xfff;
        break;
        case 3:
            size = 0x1fff;
        break;
        case 4:
            size = 0x3fff;
        break;
    }
    end = start + size;
    if (end > 0xffff)  end = 0xffff;
    elfConfiguration.ioConfiguration.mc6845EndRam = end;
    elfConfiguration.ioConfiguration.mc6845Ef = XRCCTRL(*this, "MC6845EF", wxSpinCtrl)->GetValue();

    elfConfiguration.ioConfiguration.hexOutput = XRCCTRL(*this, "HexOutput", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.hexInput = XRCCTRL(*this, "HexInput", wxSpinCtrl)->GetValue();
    elfConfiguration.ioConfiguration.hexEf = XRCCTRL(*this, "HexEf", wxSpinCtrl)->GetValue();

    elfConfiguration.ioConfiguration.tapeEf = XRCCTRL(*this, "TapeEf", wxSpinCtrl)->GetValue();

    p_Main->setElfConfiguration(elfConfiguration);

    EndModal(wxID_OK);
}

void DevicePortsDialog::reset6847Item(int dontCheck, int num)
{
    if ((XRCCTRL(*this, "MC6847-B7", wxChoice)->GetCurrentSelection() == num) && (dontCheck != 10))
        XRCCTRL(*this, "MC6847-B7", wxChoice)->SetSelection(0);
    if ((XRCCTRL(*this, "MC6847-B6", wxChoice)->GetCurrentSelection() == num) && (dontCheck != 9))
        XRCCTRL(*this, "MC6847-B6", wxChoice)->SetSelection(0);
    if ((XRCCTRL(*this, "MC6847-B5", wxChoice)->GetCurrentSelection() == num) && (dontCheck != 8))
        XRCCTRL(*this, "MC6847-B5", wxChoice)->SetSelection(0);
    if ((XRCCTRL(*this, "MC6847-B4", wxChoice)->GetCurrentSelection() == num) && (dontCheck != 7))
        XRCCTRL(*this, "MC6847-B4", wxChoice)->SetSelection(0);
    if ((XRCCTRL(*this, "MC6847-B3", wxChoice)->GetCurrentSelection() == num) && (dontCheck != 6))
        XRCCTRL(*this, "MC6847-B3", wxChoice)->SetSelection(0);
    if ((XRCCTRL(*this, "MC6847-B2", wxChoice)->GetCurrentSelection() == num) && (dontCheck != 5))
        XRCCTRL(*this, "MC6847-B2", wxChoice)->SetSelection(0);
    if ((XRCCTRL(*this, "MC6847-B1", wxChoice)->GetCurrentSelection() == num) && (dontCheck != 4))
        XRCCTRL(*this, "MC6847-B1", wxChoice)->SetSelection(0);
    if ((XRCCTRL(*this, "MC6847-B0", wxChoice)->GetCurrentSelection() == num) && (dontCheck != 3))
        XRCCTRL(*this, "MC6847-B0", wxChoice)->SetSelection(0);
    if ((XRCCTRL(*this, "MC6847-DD7", wxChoice)->GetCurrentSelection() == num) && (dontCheck != 2))
        XRCCTRL(*this, "MC6847-DD7", wxChoice)->SetSelection(0);
    if ((XRCCTRL(*this, "MC6847-DD6", wxChoice)->GetCurrentSelection() == num) && (dontCheck != 1))
        XRCCTRL(*this, "MC6847-DD6", wxChoice)->SetSelection(0);
}

void DevicePortsDialog::onMC6847OutputMode(wxCommandEvent&event)
{
    XRCCTRL(*this, "MC6847Output", wxSpinCtrl)->Enable(event.GetSelection() != 1);
}

void DevicePortsDialog::onMC6847DD6(wxCommandEvent&event)
{
    reset6847Item(1, event.GetSelection()); 
}

void DevicePortsDialog::onMC6847DD7(wxCommandEvent&event)
{
    reset6847Item(2, event.GetSelection()); 
}

void DevicePortsDialog::onMC6847B0(wxCommandEvent&event)
{
    reset6847Item(3, event.GetSelection()); 
}

void DevicePortsDialog::onMC6847B1(wxCommandEvent&event)
{
    reset6847Item(4, event.GetSelection()); 
}

void DevicePortsDialog::onMC6847B2(wxCommandEvent&event)
{
    reset6847Item(5, event.GetSelection()); 
}

void DevicePortsDialog::onMC6847B3(wxCommandEvent&event)
{
    reset6847Item(6, event.GetSelection()); 
}

void DevicePortsDialog::onMC6847B4(wxCommandEvent&event)
{
    reset6847Item(7, event.GetSelection()); 
}

void DevicePortsDialog::onMC6847B5(wxCommandEvent&event)
{
    reset6847Item(8, event.GetSelection()); 
}

void DevicePortsDialog::onMC6847B6(wxCommandEvent&event)
{
    reset6847Item(9, event.GetSelection()); 
}

void DevicePortsDialog::onMC6847B7(wxCommandEvent&event)
{
    reset6847Item(10, event.GetSelection()); 
}
