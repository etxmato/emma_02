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
#include "guiupdate.h"
#include "guipopup.h"

BEGIN_EVENT_TABLE(GuiUpdate, GuiMain)

END_EVENT_TABLE()

GuiUpdate::GuiUpdate(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiMain(title, pos, size, mode, dataDir, iniDir)
{
}

void GuiUpdate::guiSetButtonLabel(wxString info, wxString value)
{
    if (!mode_.gui)
        return;
    
    CallAfter([this, info, value]() {
        XRCCTRL(*this, info, wxButton)->SetLabel(value);
    });
}

void GuiUpdate::guiSetCheckBox(wxString info, bool state)
{
    if (!mode_.gui)
        return;
    
    CallAfter([this, info, state]() {
        setCheckBox(info, state);
    });
}

void GuiUpdate::guiShowMessage(Word message)
{
    wxString buffer;
    buffer.Printf("%04X", message);
    
    CallAfter([this, buffer]() {
        p_Main->message(buffer);
    });
}

void GuiUpdate::guiSetTextValue(wxString info, wxString value)
{
    if (!mode_.gui)
        return;
    
    CallAfter([this, info, value]() {
        XRCCTRL(*this, info, wxTextCtrl)->SetValue(value);
    });
}

void GuiUpdate::guiSetStaticTextValue(wxString info, wxString value)
{
    if (!mode_.gui)
        return;
    
    CallAfter([this, info, value]() {
        XRCCTRL(*this, info, wxStaticText)->SetLabel(value);
    });
}

void GuiUpdate::guiEnableClock(bool state)
{
    if (!mode_.gui)
        return;
    
    CallAfter([this, state]() {
        XRCCTRL(*this, "clockTextCtrl", wxTextCtrl)->Enable(state);
    });
}

void GuiUpdate::guiSetConvertState(bool state)
{
    if (!mode_.gui)
        return;

    CallAfter([this, state]() {
        XRCCTRL(*this, "BatchConvertButtonXml", wxButton)->Enable(state);
        XRCCTRL(*this, "BatchButtonXml", wxButton)->Enable(state);
    });
}

void GuiUpdate::guiSetLocation(bool state, Word saveStart, Word saveEnd, Word saveExec)
{
    wxString saveStartString, saveEndString, saveExecString;
    saveStartString.Printf("%04X", saveStart);
    saveEndString.Printf("%04X", saveEnd);
    saveExecString.Printf("%04X", saveExec);

    computerConfiguration.memAccessConfiguration.useLocation = state;
    computerConfiguration.memAccessConfiguration.saveStart = saveStart;
    computerConfiguration.memAccessConfiguration.saveEnd = saveEnd;
    computerConfiguration.memAccessConfiguration.saveExec = saveExec;
    computerConfiguration.memAccessConfiguration.saveStartString = saveStartString;
    computerConfiguration.memAccessConfiguration.saveEndString = saveEndString;
    computerConfiguration.memAccessConfiguration.saveExecString = saveExecString;

    if (!mode_.gui)
        return;

    CallAfter([this, state, saveStartString, saveEndString, saveExecString]() {
        if (popupDialog_ != NULL)
            popupDialog_->setLocation(state, saveStartString, saveEndString, saveExecString);

        if (state)
        {
            XRCCTRL(*this,"SaveStartXml", wxTextCtrl)->ChangeValue(saveStartString);
            XRCCTRL(*this,"SaveEndXml", wxTextCtrl)->ChangeValue(saveEndString);
            XRCCTRL(*this,"SaveExecXml", wxTextCtrl)->ChangeValue(saveExecString);
        }
        else
        {
            XRCCTRL(*this,"SaveStartXml", wxTextCtrl)->ChangeValue("");
            XRCCTRL(*this,"SaveEndXml", wxTextCtrl)->ChangeValue("");
            XRCCTRL(*this,"SaveExecXml", wxTextCtrl)->ChangeValue("");
        }
        XRCCTRL(*this, "UseLocationXml", wxCheckBox)->SetValue(state);
        enableLocationGui();
    });
}

void GuiUpdate::guiSetLocationState(bool state)
{
    computerConfiguration.memAccessConfiguration.useLocation = state;

    if (!mode_.gui)
        return;

    CallAfter([this, state]() {
        if (popupDialog_ != NULL)
            popupDialog_->setLocation(state);

        XRCCTRL(*this, "UseLocationXml", wxCheckBox)->SetValue(state);
        enableLocationGui();
    });
}

void GuiUpdate::guiSaveStart(Word saveStart)
{
    wxString saveStartString;
    saveStartString.Printf("%04X", saveStart);

    computerConfiguration.memAccessConfiguration.saveStart = saveStart;
    computerConfiguration.memAccessConfiguration.saveStartString = saveStartString;

    if (!mode_.gui)
        return;

    CallAfter([this, saveStartString]() {
        if (popupDialog_ != NULL)
            popupDialog_->setStartLocation(saveStartString);

        XRCCTRL(*this,"SaveStartXml", wxTextCtrl)->SetValue(saveStartString);
    });
}

void GuiUpdate::guiSaveEnd(Word saveEnd)
{
    wxString saveEndString;
    saveEndString.Printf("%04X", saveEnd);

    computerConfiguration.memAccessConfiguration.saveEnd = saveEnd;
    computerConfiguration.memAccessConfiguration.saveEndString = saveEndString;

    if (!mode_.gui)
        return;

    CallAfter([this, saveEndString]() {
        if (popupDialog_ != NULL)
            popupDialog_->setEndLocation(saveEndString);

        XRCCTRL(*this,"SaveEndXml", wxTextCtrl)->SetValue(saveEndString);
    });
}

void GuiUpdate::guiSetTapeState(int tapeState, wxString tapeNumber)
{
    CallAfter([this, tapeState, tapeNumber]() {
        setTapeState(tapeState, tapeNumber);
    });
}

void GuiUpdate::guiSetSwName(wxString swName)
{
    CallAfter([this, swName]() {
        p_Main->setSwName(swName);
        p_Main->updateTitle();
    });
}

void GuiUpdate::guiUpdateTitle()
{
    CallAfter([this]() {
        p_Main->updateTitle();
    });
}

void GuiUpdate::guiHwTapeStateChange(int status)
{
    hwTapeState_ = status;

    CallAfter([this]() {
        if (!mode_.gui)
            return;

        switch (hwTapeState_)
        {
            case HW_TAPE_STATE_PLAY:
                XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->SetBitmapLabel(playDarkGreenBitmap);
                XRCCTRL(*this, "CasForwardXml", wxBitmapButton)->SetBitmapLabel(forwardBlackBitmap);
                XRCCTRL(*this, "CasRewindXml", wxBitmapButton)->SetBitmapLabel(rewindBlackBitmap);
                XRCCTRL(*this, "CasSaveXml", wxBitmapButton)->SetBitmapLabel(recOffBitmap);
                break;

            case HW_TAPE_STATE_FF:
                XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->SetBitmapLabel(playBlackBitmap);
                XRCCTRL(*this, "CasForwardXml", wxBitmapButton)->SetBitmapLabel(forwardDarkGreenBitmap);
                XRCCTRL(*this, "CasRewindXml", wxBitmapButton)->SetBitmapLabel(rewindBlackBitmap);
                XRCCTRL(*this, "CasSaveXml", wxBitmapButton)->SetBitmapLabel(recOffBitmap);
                break;

            case HW_TAPE_STATE_RW:
                XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->SetBitmapLabel(playBlackBitmap);
                XRCCTRL(*this, "CasForwardXml", wxBitmapButton)->SetBitmapLabel(forwardBlackBitmap);
                XRCCTRL(*this, "CasRewindXml", wxBitmapButton)->SetBitmapLabel(rewindDarkGreenBitmap);
                XRCCTRL(*this, "CasSaveXml", wxBitmapButton)->SetBitmapLabel(recOffBitmap);
                break;

            case HW_TAPE_STATE_REC:
                XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->SetBitmapLabel(playBlackBitmap);
                XRCCTRL(*this, "CasForwardXml", wxBitmapButton)->SetBitmapLabel(forwardBlackBitmap);
                XRCCTRL(*this, "CasRewindXml", wxBitmapButton)->SetBitmapLabel(rewindBlackBitmap);
                XRCCTRL(*this, "CasSaveXml", wxBitmapButton)->SetBitmapLabel(recOnBitmap);
                break;

            default:
                hwTapeState_ = HW_TAPE_STATE_OFF;
                XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->SetBitmapLabel(playBlackBitmap);
                XRCCTRL(*this, "CasForwardXml", wxBitmapButton)->SetBitmapLabel(forwardBlackBitmap);
                XRCCTRL(*this, "CasRewindXml", wxBitmapButton)->SetBitmapLabel(rewindBlackBitmap);
                XRCCTRL(*this, "CasSaveXml", wxBitmapButton)->SetBitmapLabel(recOffBitmap);
                break;
        }
    });
}

void GuiUpdate::guiShowTextMessage(wxString messageText)
{
    CallAfter([this, messageText]() {
        p_Main->message(messageText);
    });
}

void GuiUpdate::guiPrintParallel(Byte value)
{
    if (statusLedUpdate_)
        p_Main->statusLedOnEvent();
    CallAfter([this, value]() {
        p_PrinterParallel->outParallel(value);
        if (statusLedUpdate_)
            p_Main->statusLedOffDirect();
    });
}

void GuiUpdate::guiPrintParallelFinished()
{
    CallAfter([this]() {
        p_PrinterParallel->printFinished();
    });
}

void GuiUpdate::guiPrintThermal(Byte value, Byte Qflag)
{
    if (statusLedUpdate_)
        p_Main->statusLedOnEvent();
    thermalEf_ = true;
    CallAfter([this, value, Qflag]() {
        p_PrinterThermal->outThermal(value, Qflag);
        thermalEf_ = false;
        if (statusLedUpdate_)
            p_Main->statusLedOffDirect();
    });
}

void GuiUpdate::guiPrintThermalFinished()
{
    CallAfter([this]() {
        p_PrinterThermal->printFinished();
        thermalEf_ = false;
    });
}

void GuiUpdate::guiPrintSerial(Byte value)
{
    if (statusLedUpdate_)
        p_Main->statusLedOnEvent();
    CallAfter([this, value]() {
        p_PrinterSerial->outSerial(value);
        if (statusLedUpdate_)
            p_Main->statusLedOffDirect();
    });
}

void GuiUpdate::guiPrintSerialFinished()
{
    CallAfter([this]() {
        p_PrinterSerial->printFinished();
    });
}

void GuiUpdate::guiPrintPecom(Byte value)
{
    CallAfter([this, value]() {
        p_Printer->printerOut(value);
    });
}

void GuiUpdate::guiZoomChange(double zoom, int videoNumber)
{
    CallAfter([this, zoom, videoNumber]() {
        if (p_Video[videoNumber] == NULL)
            return;

        p_Video[videoNumber]->setZoom(zoom);
        if (computerRunning_)
        {
#if defined(__linux__)
            guiRedrawBarTimeOutPointer->Start(200, wxTIMER_ONE_SHOT);
#else
            p_Video[videoNumber]->reDrawBar();
#endif
        }
        p_Main->zoomEventFinished();
    });
}

void GuiUpdate::guiZoomVtChange(double zoom, int uartNumber)
{
    CallAfter([this, zoom, uartNumber]() {
        p_Vt100[uartNumber]->setZoom(zoom);
//        if (runningComputer_ != ELF2K && runningComputer_ != MEMBER)
//            p_Vt100[UART1]->copyScreen();
        p_Main->zoomVtEventFinished();
    });
}

void GuiUpdate::guiZoom(double zoom, int videoNumber, bool isVt)
{
    CallAfter([this, zoom, videoNumber, isVt]() {
        if (isVt)
            p_Main->zoomEventVt(zoom);
        else
            p_Main->zoomEvent(zoom, videoNumber);
    });
}

void GuiUpdate::guiVideoSetFullScreen(bool state, int videoNumber)
{
    CallAfter([this, state, videoNumber]() {
        fullScreenEventOngoing_ = true;
        if (p_Video[videoNumber] != NULL)
            p_Video[videoNumber]->setFullScreen(state);
    });
}

void GuiUpdate::guiVtSetFullScreen(bool state, int uartNumber)
{
    CallAfter([this, state, uartNumber]() {
        if (p_Vt100[uartNumber] != NULL)
            p_Vt100[uartNumber]->setFullScreen(state);
    });
}

void GuiUpdate::guiChangeNoteBook()
{
    CallAfter([this]() {
        XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(XMLTAB);
        XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(DEBUGGERTAB);
        long selected_tab = configPointer->Read("/Main/Selected_Tab", 0l);
        if (selected_tab > DEBUGGERTAB)
            selected_tab = 0;
        XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(selected_tab);
    });
}

void GuiUpdate::guiSetFandMBasicGui()
{
    if (!mode_.gui)
        return;
    CallAfter([this]() {
        p_Main->setFandMBasicGui();
    });
}

void GuiUpdate::guiPauseState()
{
    CallAfter([this]() {
        p_Main->setPauseState();
    });
}

void GuiUpdate::guiRefreshVideo(bool isVt, int uart_video_Number)
{
    if (videoRefreshOngoing_[uart_video_Number])
        return;

    videoRefreshOngoing_[uart_video_Number] = true;
    CallAfter([this, isVt, uart_video_Number]() {
        if (isVt)
            p_Vt100[uart_video_Number]->refreshVideo();
        else
            p_Video[uart_video_Number]->refreshVideo();
        videoRefreshOngoing_[uart_video_Number] = false;
    });
}

void GuiUpdate::guiRefreshPanel()
{
    if (panelRefreshOngoing_)
        return;

    panelRefreshOngoing_ = true;
    CallAfter([this]() {
        p_Computer->refreshPanel();
        panelRefreshOngoing_ = false;
    });
}

void GuiUpdate::guiUpdateLedStatus(bool status, int card, int i)
{
    CallAfter([this, status, card, i]() {
        if (computerRunning_)
            p_Video[computerConfiguration.vis1870Configuration.videoNumber]->updateStatusLed(status, card, i);
    });
}

void GuiUpdate::guiUpdateVipIILedStatus(int led, bool status)
{
    CallAfter([this, led, status]() {
        p_Computer->updateStatusBarLedStatus(status, led);
    });
}

void GuiUpdate::guiDebounceTimer()
{
    CallAfter([this]() {
        keyDebounceTimeoutPointer->Start(80, wxTIMER_ONE_SHOT);
    });
}

void GuiUpdate::guiBeepTimer(int frequency, int ms)
{
    p_Computer->toneBeep(frequency, true);
    CallAfter([this, ms]() {
        beepTimerPointer->Start(ms, wxTIMER_ONE_SHOT);
    });
}

void GuiUpdate::guiErrorMessage(wxString messageText)
{
    CallAfter([this, messageText]() {
        (void)wxMessageBox(messageText, "Emma 02", wxICON_ERROR | wxOK);
    });
}

void GuiUpdate::guiKillComputer()
{
    CallAfter([this]() {
        p_Main->killComputer();
    });
}


