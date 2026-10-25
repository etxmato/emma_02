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
#include "xrc_compat.h"
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
    thermalEf_ = false;
    panelRefreshOngoing_ = false;
    for (int i = 0; i < VIDEOXMLMAX; i++)
        videoRefreshOngoing_[i] = false;
}

void GuiUpdate::guiSetButtonLabel(wxString info, wxString value)
{
    if (!mode_.gui)
        return;

    EmmaCallAfter(this, &GuiUpdate::guiSetButtonLabelHelper, info, value);
}

void GuiUpdate::guiSetButtonLabelHelper(wxString info, wxString value)
{
    XRCCTRL(*this, info, wxButton)->SetLabel(value);
}

void GuiUpdate::guiSetCheckBox(wxString info, bool state)
{
    if (!mode_.gui)
        return;

    EmmaCallAfter(this, &GuiUpdate::guiSetCheckBoxHelper, info, state);
}

void GuiUpdate::guiSetCheckBoxHelper(wxString info, bool state)
{
    setCheckBox(info, state);
}

void GuiUpdate::guiShowMessage(Word message)
{
    wxString buffer;
    buffer.Printf("%04X", message);

    EmmaCallAfter(this, &GuiUpdate::guiShowMessageHelper, buffer);
}

void GuiUpdate::guiShowMessageHelper(wxString messageText)
{
    p_Main->message(messageText);
}

void GuiUpdate::guiSetTextValue(wxString info, wxString value)
{
    if (!mode_.gui)
        return;

    EmmaCallAfter(this, &GuiUpdate::guiSetTextValueHelper, info, value);
}

void GuiUpdate::guiSetTextValueHelper(wxString info, wxString value)
{
    XRCCTRL(*this, info, wxTextCtrl)->SetValue(value);
}

void GuiUpdate::guiSetStaticTextValue(wxString info, wxString value)
{
    if (!mode_.gui)
        return;

    EmmaCallAfter(this, &GuiUpdate::guiSetStaticTextValueHelper, info, value);
}

void GuiUpdate::guiSetStaticTextValueHelper(wxString info, wxString value)
{
    XRCCTRL(*this, info, wxStaticText)->SetLabel(value);
}

void GuiUpdate::guiEnableClock(bool state)
{
    if (!mode_.gui)
        return;

    EmmaCallAfter(this, &GuiUpdate::guiEnableClockHelper, state);
}

void GuiUpdate::guiEnableClockHelper(bool state)
{
    XRCCTRL(*this, "clockTextCtrl", wxTextCtrl)->Enable(state);
}

void GuiUpdate::guiSetConvertState(bool state)
{
    if (!mode_.gui)
        return;

    EmmaCallAfter(this, &GuiUpdate::guiSetConvertStateHelper, state);
}

void GuiUpdate::guiSetConvertStateHelper(bool state)
{
    XRCCTRL(*this, "BatchConvertButtonXml", wxButton)->Enable(state);
    XRCCTRL(*this, "BatchButtonXml", wxButton)->Enable(state);
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

    EmmaCallAfter(this, &GuiUpdate::guiSetLocationHelper, state, saveStartString, saveEndString, saveExecString);
}

void GuiUpdate::guiSetLocationHelper(bool state, wxString saveStartString, wxString saveEndString, wxString saveExecString)
{
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
}

void GuiUpdate::guiSetLocationState(bool state)
{
    computerConfiguration.memAccessConfiguration.useLocation = state;

    if (!mode_.gui)
        return;

    EmmaCallAfter(this, &GuiUpdate::guiSetLocationStateHelper, state);
}

void GuiUpdate::guiSetLocationStateHelper(bool state)
{
    if (popupDialog_ != NULL)
        popupDialog_->setLocation(state);

    XRCCTRL(*this, "UseLocationXml", wxCheckBox)->SetValue(state);
    enableLocationGui();
}

void GuiUpdate::guiSaveStart(Word saveStart)
{
    wxString saveStartString;
    saveStartString.Printf("%04X", saveStart);

    computerConfiguration.memAccessConfiguration.saveStart = saveStart;
    computerConfiguration.memAccessConfiguration.saveStartString = saveStartString;

    if (!mode_.gui)
        return;

    EmmaCallAfter(this, &GuiUpdate::guiSaveStartHelper, saveStartString);
}

void GuiUpdate::guiSaveStartHelper(wxString saveStartString)
{
    if (popupDialog_ != NULL)
        popupDialog_->setStartLocation(saveStartString);

    XRCCTRL(*this,"SaveStartXml", wxTextCtrl)->SetValue(saveStartString);
}

void GuiUpdate::guiSaveEnd(Word saveEnd)
{
    wxString saveEndString;
    saveEndString.Printf("%04X", saveEnd);

    computerConfiguration.memAccessConfiguration.saveEnd = saveEnd;
    computerConfiguration.memAccessConfiguration.saveEndString = saveEndString;

    if (!mode_.gui)
        return;

    EmmaCallAfter(this, &GuiUpdate::guiSaveEndHelper, saveEndString);
}

void GuiUpdate::guiSaveEndHelper(wxString saveEndString)
{
    if (popupDialog_ != NULL)
        popupDialog_->setEndLocation(saveEndString);

    XRCCTRL(*this,"SaveEndXml", wxTextCtrl)->SetValue(saveEndString);
}

void GuiUpdate::guiSetTapeState(int tapeState, wxString tapeNumber)
{
    EmmaCallAfter(this, &GuiUpdate::guiSetTapeStateHelper, tapeState, tapeNumber);
}

void GuiUpdate::guiSetTapeStateHelper(int tapeState, wxString tapeNumber)
{
    setTapeState(tapeState, tapeNumber);
}

void GuiUpdate::guiSetSwName(wxString swName)
{
    EmmaCallAfter(this, &GuiUpdate::guiSetSwNameHelper, swName);
}

void GuiUpdate::guiSetSwNameHelper(wxString swName)
{
    p_Main->setSwName(swName);
    p_Main->updateTitle();
}

void GuiUpdate::guiUpdateTitle()
{
    EmmaCallAfter(this, &GuiUpdate::guiUpdateTitleHelper);
}

void GuiUpdate::guiUpdateTitleHelper()
{
    p_Main->updateTitle();
}

void GuiUpdate::guiHwTapeStateChange(int status)
{
    hwTapeState_ = status;

    EmmaCallAfter(this, &GuiUpdate::guiHwTapeStateChangeHelper, status);
}

void GuiUpdate::guiHwTapeStateChangeHelper(int status)
{
    if (!mode_.gui)
        return;

    switch (status)
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
}

void GuiUpdate::guiShowTextMessage(wxString messageText)
{
    EmmaCallAfter(this, &GuiUpdate::guiShowTextMessageHelper, messageText);
}

void GuiUpdate::guiShowTextMessageHelper(wxString messageText)
{
    p_Main->message(messageText);
}

void GuiUpdate::guiPrintParallel(Byte value)
{
    if (statusLedUpdate_)
        p_Main->statusLedOnEvent();
    EmmaCallAfter(this, &GuiUpdate::guiPrintParallelHelper, value);
}

void GuiUpdate::guiPrintParallelHelper(Byte value)
{
    p_PrinterParallel->outParallel(value);
    if (statusLedUpdate_)
        p_Main->statusLedOffDirect();
}

void GuiUpdate::guiPrintParallelFinished()
{
    EmmaCallAfter(this, &GuiUpdate::guiPrintParallelFinishedHelper);
}

void GuiUpdate::guiPrintParallelFinishedHelper()
{
    p_PrinterParallel->printFinished();
}

void GuiUpdate::guiPrintThermal(Byte value, Byte Qflag)
{
    if (statusLedUpdate_)
        p_Main->statusLedOnEvent();
    thermalEf_ = true;
    EmmaCallAfter(this, &GuiUpdate::guiPrintThermalHelper, value, Qflag);
}

void GuiUpdate::guiPrintThermalHelper(Byte value, Byte Qflag)
{
    p_PrinterThermal->outThermal(value, Qflag);
    thermalEf_ = false;
    if (statusLedUpdate_)
        p_Main->statusLedOffDirect();
}

void GuiUpdate::guiPrintThermalFinished()
{
    EmmaCallAfter(this, &GuiUpdate::guiPrintThermalFinishedHelper);
}

void GuiUpdate::guiPrintThermalFinishedHelper()
{
    p_PrinterThermal->printFinished();
    thermalEf_ = false;
}

void GuiUpdate::guiPrintSerial(Byte value)
{
    if (statusLedUpdate_)
        p_Main->statusLedOnEvent();
    EmmaCallAfter(this, &GuiUpdate::guiPrintSerialHelper, value);
}

void GuiUpdate::guiPrintSerialHelper(Byte value)
{
    p_PrinterSerial->outSerial(value);
    if (statusLedUpdate_)
        p_Main->statusLedOffDirect();
}

void GuiUpdate::guiPrintSerialFinished()
{
    EmmaCallAfter(this, &GuiUpdate::guiPrintSerialFinishedHelper);
}

void GuiUpdate::guiPrintSerialFinishedHelper()
{
    p_PrinterSerial->printFinished();
}

void GuiUpdate::guiPrintPecom(Byte value)
{
    EmmaCallAfter(this, &GuiUpdate::guiPrintPecomHelper, value);
}

void GuiUpdate::guiPrintPecomHelper(Byte value)
{
    p_Printer->printerOut(value);
}

void GuiUpdate::guiZoomChange(double zoom, int videoNumber)
{
    EmmaCallAfter(this, &GuiUpdate::guiZoomChangeHelper, zoom, videoNumber);
}

void GuiUpdate::guiZoomChangeHelper(double zoom, int videoNumber)
{
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
}

void GuiUpdate::guiZoomVtChange(double zoom, int uartNumber)
{
    EmmaCallAfter(this, &GuiUpdate::guiZoomVtChangeHelper, zoom, uartNumber);
}

void GuiUpdate::guiZoomVtChangeHelper(double zoom, int uartNumber)
{
    p_Vt100[uartNumber]->setZoom(zoom);
//        if (runningComputer_ != ELF2K && runningComputer_ != MEMBER)
//            p_Vt100[UART1]->copyScreen();
    p_Main->zoomVtEventFinished();
}

void GuiUpdate::guiZoom(double zoom, int videoNumber, bool isVt)
{
    EmmaCallAfter(this, &GuiUpdate::guiZoomHelper, zoom, videoNumber, isVt);
}

void GuiUpdate::guiZoomHelper(double zoom, int videoNumber, bool isVt)
{
    if (isVt)
        p_Main->zoomEventVt(zoom);
    else
        p_Main->zoomEvent(zoom, videoNumber);
}

void GuiUpdate::guiVideoSetFullScreen(bool state, int videoNumber)
{
    EmmaCallAfter(this, &GuiUpdate::guiVideoSetFullScreenHelper, state, videoNumber);
}

void GuiUpdate::guiVideoSetFullScreenHelper(bool state, int videoNumber)
{
    fullScreenEventOngoing_ = true;
    if (p_Video[videoNumber] != NULL)
        p_Video[videoNumber]->setFullScreen(state);
}

void GuiUpdate::guiVtSetFullScreen(bool state, int uartNumber)
{
    EmmaCallAfter(this, &GuiUpdate::guiVtSetFullScreenHelper, state, uartNumber);
}

void GuiUpdate::guiVtSetFullScreenHelper(bool state, int uartNumber)
{
    if (p_Vt100[uartNumber] != NULL)
        p_Vt100[uartNumber]->setFullScreen(state);
}

void GuiUpdate::guiChangeNoteBook()
{
    EmmaCallAfter(this, &GuiUpdate::guiChangeNoteBookHelper);
}

void GuiUpdate::guiChangeNoteBookHelper()
{
    XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(XMLTAB);
    XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(DEBUGGERTAB);
    long selected_tab = configPointer->Read("/Main/Selected_Tab", 0l);
    if (selected_tab > DEBUGGERTAB)
        selected_tab = 0;
    XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(selected_tab);
}

void GuiUpdate::guiSetFandMBasicGui()
{
    if (!mode_.gui)
        return;
    EmmaCallAfter(this, &GuiUpdate::guiSetFandMBasicGuiHelper);
}

void GuiUpdate::guiSetFandMBasicGuiHelper()
{
    p_Main->setFandMBasicGui();
}

void GuiUpdate::guiPauseState()
{
    EmmaCallAfter(this, &GuiUpdate::guiPauseStateHelper);
}

void GuiUpdate::guiPauseStateHelper()
{
    p_Main->setPauseState();
}

void GuiUpdate::guiRefreshVideo(bool isVt, int uart_video_Number)
{
    if (videoRefreshOngoing_[uart_video_Number])
        return;

    videoRefreshOngoing_[uart_video_Number] = true;
    EmmaCallAfter(this, &GuiUpdate::guiRefreshVideoHelper, isVt, uart_video_Number);
}

void GuiUpdate::guiRefreshVideoHelper(bool isVt, int uart_video_Number)
{
    if (isVt)
        p_Vt100[uart_video_Number]->refreshVideo();
    else
        p_Video[uart_video_Number]->refreshVideo();
    videoRefreshOngoing_[uart_video_Number] = false;
}

void GuiUpdate::guiRefreshPanel()
{
    if (panelRefreshOngoing_)
        return;

    panelRefreshOngoing_ = true;
    EmmaCallAfter(this, &GuiUpdate::guiRefreshPanelHelper);
}

void GuiUpdate::guiRefreshPanelHelper()
{
    p_Computer->refreshPanel();
    panelRefreshOngoing_ = false;
}

void GuiUpdate::guiUpdateLedStatus(bool status, int card, int i)
{
    EmmaCallAfter(this, &GuiUpdate::guiUpdateLedStatusHelper, status, card, i);
}

void GuiUpdate::guiUpdateLedStatusHelper(bool status, int card, int i)
{
    if (computerRunning_)
        p_Video[computerConfiguration.vis1870Configuration.videoNumber]->updateStatusLed(status, card, i);
}

void GuiUpdate::guiUpdateVipIILedStatus(int led, bool status)
{
    EmmaCallAfter(this, &GuiUpdate::guiUpdateVipIILedStatusHelper, led, status);
}

void GuiUpdate::guiUpdateVipIILedStatusHelper(int led, bool status)
{
    p_Computer->updateStatusBarLedStatus(status, led);
}

void GuiUpdate::guiDebounceTimer()
{
    EmmaCallAfter(this, &GuiUpdate::guiDebounceTimerHelper);
}

void GuiUpdate::guiDebounceTimerHelper()
{
    keyDebounceTimeoutPointer->Start(80, wxTIMER_ONE_SHOT);
}

void GuiUpdate::guiBeepTimer(int frequency, int ms)
{
    p_Computer->toneBeep(frequency, true);
    EmmaCallAfter(this, &GuiUpdate::guiBeepTimerHelper, ms);
}

void GuiUpdate::guiBeepTimerHelper(int ms)
{
    beepTimerPointer->Start(ms, wxTIMER_ONE_SHOT);
}

void GuiUpdate::guiErrorMessage(wxString messageText)
{
    EmmaCallAfter(this, &GuiUpdate::guiErrorMessageHelper, messageText);
}

void GuiUpdate::guiErrorMessageHelper(wxString messageText)
{
    (void)wxMessageBox(messageText, "Emma 02", wxICON_ERROR | wxOK);
}

void GuiUpdate::guiKillComputer()
{
    EmmaCallAfter(this, &GuiUpdate::guiKillComputerHelper);
}

void GuiUpdate::guiKillComputerHelper()
{
    p_Main->killComputer();
}

