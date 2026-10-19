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

    callAfterInfo_ = info;
    callAfterValue_ = value;
    CallAfter(&GuiUpdate::guiSetButtonLabelHelper);
}

void GuiUpdate::guiSetButtonLabelHelper()
{
    XRCCTRL(*this, callAfterInfo_, wxButton)->SetLabel(callAfterValue_);
}

void GuiUpdate::guiSetCheckBox(wxString info, bool state)
{
    if (!mode_.gui)
        return;

    callAfterInfo_ = info;
    callAfterState_ = state;
    CallAfter(&GuiUpdate::guiSetCheckBoxHelper);
}

void GuiUpdate::guiSetCheckBoxHelper()
{
    setCheckBox(callAfterInfo_, callAfterState_);
}

void GuiUpdate::guiShowMessage(Word message)
{
    wxString buffer;
    buffer.Printf("%04X", message);

    callAfterMessageText_ = buffer;
    CallAfter(&GuiUpdate::guiShowMessageHelper);
}

void GuiUpdate::guiShowMessageHelper()
{
    p_Main->message(callAfterMessageText_);
}

void GuiUpdate::guiSetTextValue(wxString info, wxString value)
{
    if (!mode_.gui)
        return;

    callAfterInfo_ = info;
    callAfterValue_ = value;
    CallAfter(&GuiUpdate::guiSetTextValueHelper);
}

void GuiUpdate::guiSetTextValueHelper()
{
    XRCCTRL(*this, callAfterInfo_, wxTextCtrl)->SetValue(callAfterValue_);
}

void GuiUpdate::guiSetStaticTextValue(wxString info, wxString value)
{
    if (!mode_.gui)
        return;

    callAfterInfo_ = info;
    callAfterValue_ = value;
    CallAfter(&GuiUpdate::guiSetStaticTextValueHelper);
}

void GuiUpdate::guiSetStaticTextValueHelper()
{
    XRCCTRL(*this, callAfterInfo_, wxStaticText)->SetLabel(callAfterValue_);
}

void GuiUpdate::guiEnableClock(bool state)
{
    if (!mode_.gui)
        return;

    callAfterState_ = state;
    CallAfter(&GuiUpdate::guiEnableClockHelper);
}

void GuiUpdate::guiEnableClockHelper()
{
    XRCCTRL(*this, "clockTextCtrl", wxTextCtrl)->Enable(callAfterState_);
}

void GuiUpdate::guiSetConvertState(bool state)
{
    if (!mode_.gui)
        return;

    callAfterState_ = state;
    CallAfter(&GuiUpdate::guiSetConvertStateHelper);
}

void GuiUpdate::guiSetConvertStateHelper()
{
    XRCCTRL(*this, "BatchConvertButtonXml", wxButton)->Enable(callAfterState_);
    XRCCTRL(*this, "BatchButtonXml", wxButton)->Enable(callAfterState_);
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

    callAfterState_ = state;
    callAfterSaveStartString_ = saveStartString;
    callAfterSaveEndString_ = saveEndString;
    callAfterSaveExecString_ = saveExecString;
    CallAfter(&GuiUpdate::guiSetLocationHelper);
}

void GuiUpdate::guiSetLocationHelper()
{
    if (popupDialog_ != NULL)
        popupDialog_->setLocation(callAfterState_, callAfterSaveStartString_, callAfterSaveEndString_, callAfterSaveExecString_);

    if (callAfterState_)
    {
        XRCCTRL(*this,"SaveStartXml", wxTextCtrl)->ChangeValue(callAfterSaveStartString_);
        XRCCTRL(*this,"SaveEndXml", wxTextCtrl)->ChangeValue(callAfterSaveEndString_);
        XRCCTRL(*this,"SaveExecXml", wxTextCtrl)->ChangeValue(callAfterSaveExecString_);
    }
    else
    {
        XRCCTRL(*this,"SaveStartXml", wxTextCtrl)->ChangeValue("");
        XRCCTRL(*this,"SaveEndXml", wxTextCtrl)->ChangeValue("");
        XRCCTRL(*this,"SaveExecXml", wxTextCtrl)->ChangeValue("");
    }
    XRCCTRL(*this, "UseLocationXml", wxCheckBox)->SetValue(callAfterState_);
    enableLocationGui();
}

void GuiUpdate::guiSetLocationState(bool state)
{
    computerConfiguration.memAccessConfiguration.useLocation = state;

    if (!mode_.gui)
        return;

    callAfterState_ = state;
    CallAfter(&GuiUpdate::guiSetLocationStateHelper);
}

void GuiUpdate::guiSetLocationStateHelper()
{
    if (popupDialog_ != NULL)
        popupDialog_->setLocation(callAfterState_);

    XRCCTRL(*this, "UseLocationXml", wxCheckBox)->SetValue(callAfterState_);
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

    callAfterSaveStartString_ = saveStartString;
    CallAfter(&GuiUpdate::guiSaveStartHelper);
}

void GuiUpdate::guiSaveStartHelper()
{
    if (popupDialog_ != NULL)
        popupDialog_->setStartLocation(callAfterSaveStartString_);

    XRCCTRL(*this,"SaveStartXml", wxTextCtrl)->SetValue(callAfterSaveStartString_);
}

void GuiUpdate::guiSaveEnd(Word saveEnd)
{
    wxString saveEndString;
    saveEndString.Printf("%04X", saveEnd);

    computerConfiguration.memAccessConfiguration.saveEnd = saveEnd;
    computerConfiguration.memAccessConfiguration.saveEndString = saveEndString;

    if (!mode_.gui)
        return;

    callAfterSaveEndString_ = saveEndString;
    CallAfter(&GuiUpdate::guiSaveEndHelper);
}

void GuiUpdate::guiSaveEndHelper()
{
    if (popupDialog_ != NULL)
        popupDialog_->setEndLocation(callAfterSaveEndString_);

    XRCCTRL(*this,"SaveEndXml", wxTextCtrl)->SetValue(callAfterSaveEndString_);
}

void GuiUpdate::guiSetTapeState(int tapeState, wxString tapeNumber)
{
    callAfterTapeState_ = tapeState;
    callAfterTapeNumber_ = tapeNumber;
    CallAfter(&GuiUpdate::guiSetTapeStateHelper);
}

void GuiUpdate::guiSetTapeStateHelper()
{
    setTapeState(callAfterTapeState_, callAfterTapeNumber_);
}

void GuiUpdate::guiSetSwName(wxString swName)
{
    callAfterSwName_ = swName;
    CallAfter(&GuiUpdate::guiSetSwNameHelper);
}

void GuiUpdate::guiSetSwNameHelper()
{
    p_Main->setSwName(callAfterSwName_);
    p_Main->updateTitle();
}

void GuiUpdate::guiUpdateTitle()
{
    CallAfter(&GuiUpdate::guiUpdateTitleHelper);
}

void GuiUpdate::guiUpdateTitleHelper()
{
    p_Main->updateTitle();
}

void GuiUpdate::guiHwTapeStateChange(int status)
{
    hwTapeState_ = status;

    CallAfter(&GuiUpdate::guiHwTapeStateChangeHelper);
}

void GuiUpdate::guiHwTapeStateChangeHelper()
{
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
}

void GuiUpdate::guiShowTextMessage(wxString messageText)
{
    callAfterMessageText_ = messageText;
    CallAfter(&GuiUpdate::guiShowTextMessageHelper);
}

void GuiUpdate::guiShowTextMessageHelper()
{
    p_Main->message(callAfterMessageText_);
}

void GuiUpdate::guiPrintParallel(Byte value)
{
    if (statusLedUpdate_)
        p_Main->statusLedOnEvent();
    callAfterValueByte_ = value;
    CallAfter(&GuiUpdate::guiPrintParallelHelper);
}

void GuiUpdate::guiPrintParallelHelper()
{
    p_PrinterParallel->outParallel(callAfterValueByte_);
    if (statusLedUpdate_)
        p_Main->statusLedOffDirect();
}

void GuiUpdate::guiPrintParallelFinished()
{
    CallAfter(&GuiUpdate::guiPrintParallelFinishedHelper);
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
    callAfterValueByte_ = value;
    callAfterQflag_ = Qflag;
    CallAfter(&GuiUpdate::guiPrintThermalHelper);
}

void GuiUpdate::guiPrintThermalHelper()
{
    p_PrinterThermal->outThermal(callAfterValueByte_, callAfterQflag_);
    thermalEf_ = false;
    if (statusLedUpdate_)
        p_Main->statusLedOffDirect();
}

void GuiUpdate::guiPrintThermalFinished()
{
    CallAfter(&GuiUpdate::guiPrintThermalFinishedHelper);
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
    callAfterValueByte_ = value;
    CallAfter(&GuiUpdate::guiPrintSerialHelper);
}

void GuiUpdate::guiPrintSerialHelper()
{
    p_PrinterSerial->outSerial(callAfterValueByte_);
    if (statusLedUpdate_)
        p_Main->statusLedOffDirect();
}

void GuiUpdate::guiPrintSerialFinished()
{
    CallAfter(&GuiUpdate::guiPrintSerialFinishedHelper);
}

void GuiUpdate::guiPrintSerialFinishedHelper()
{
    p_PrinterSerial->printFinished();
}

void GuiUpdate::guiPrintPecom(Byte value)
{
    callAfterValueByte_ = value;
    CallAfter(&GuiUpdate::guiPrintPecomHelper);
}

void GuiUpdate::guiPrintPecomHelper()
{
    p_Printer->printerOut(callAfterValueByte_);
}

void GuiUpdate::guiZoomChange(double zoom, int videoNumber)
{
    callAfterZoom_ = zoom;
    callAfterVideoNumber_ = videoNumber;
    CallAfter(&GuiUpdate::guiZoomChangeHelper);
}

void GuiUpdate::guiZoomChangeHelper()
{
    if (p_Video[callAfterVideoNumber_] == NULL)
        return;

    p_Video[callAfterVideoNumber_]->setZoom(callAfterZoom_);
    if (computerRunning_)
    {
#if defined(__linux__)
        guiRedrawBarTimeOutPointer->Start(200, wxTIMER_ONE_SHOT);
#else
        p_Video[callAfterVideoNumber_]->reDrawBar();
#endif
    }
    p_Main->zoomEventFinished();
}

void GuiUpdate::guiZoomVtChange(double zoom, int uartNumber)
{
    callAfterZoom_ = zoom;
    callAfterUartNumber_ = uartNumber;
    CallAfter(&GuiUpdate::guiZoomVtChangeHelper);
}

void GuiUpdate::guiZoomVtChangeHelper()
{
    p_Vt100[callAfterUartNumber_]->setZoom(callAfterZoom_);
//        if (runningComputer_ != ELF2K && runningComputer_ != MEMBER)
//            p_Vt100[UART1]->copyScreen();
    p_Main->zoomVtEventFinished();
}

void GuiUpdate::guiZoom(double zoom, int videoNumber, bool isVt)
{
    callAfterZoom_ = zoom;
    callAfterVideoNumber_ = videoNumber;
    callAfterIsVt_ = isVt;
    CallAfter(&GuiUpdate::guiZoomHelper);
}

void GuiUpdate::guiZoomHelper()
{
    if (callAfterIsVt_)
        p_Main->zoomEventVt(callAfterZoom_);
    else
        p_Main->zoomEvent(callAfterZoom_, callAfterVideoNumber_);
}

void GuiUpdate::guiVideoSetFullScreen(bool state, int videoNumber)
{
    callAfterState_ = state;
    callAfterVideoNumber_ = videoNumber;
    CallAfter(&GuiUpdate::guiVideoSetFullScreenHelper);
}

void GuiUpdate::guiVideoSetFullScreenHelper()
{
    fullScreenEventOngoing_ = true;
    if (p_Video[callAfterVideoNumber_] != NULL)
        p_Video[callAfterVideoNumber_]->setFullScreen(callAfterState_);
}

void GuiUpdate::guiVtSetFullScreen(bool state, int uartNumber)
{
    callAfterState_ = state;
    callAfterUartNumber_ = uartNumber;
    CallAfter(&GuiUpdate::guiVtSetFullScreenHelper);
}

void GuiUpdate::guiVtSetFullScreenHelper()
{
    if (p_Vt100[callAfterUartNumber_] != NULL)
        p_Vt100[callAfterUartNumber_]->setFullScreen(callAfterState_);
}

void GuiUpdate::guiChangeNoteBook()
{
    CallAfter(&GuiUpdate::guiChangeNoteBookHelper);
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
    CallAfter(&GuiUpdate::guiSetFandMBasicGuiHelper);
}

void GuiUpdate::guiSetFandMBasicGuiHelper()
{
    p_Main->setFandMBasicGui();
}

void GuiUpdate::guiPauseState()
{
    CallAfter(&GuiUpdate::guiPauseStateHelper);
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
    callAfterIsVt_ = isVt;
    callAfterUartVideoNumber_ = uart_video_Number;
    CallAfter(&GuiUpdate::guiRefreshVideoHelper);
}

void GuiUpdate::guiRefreshVideoHelper()
{
    if (callAfterIsVt_)
        p_Vt100[callAfterUartVideoNumber_]->refreshVideo();
    else
        p_Video[callAfterUartVideoNumber_]->refreshVideo();
    videoRefreshOngoing_[callAfterUartVideoNumber_] = false;
}

void GuiUpdate::guiRefreshPanel()
{
    if (panelRefreshOngoing_)
        return;

    panelRefreshOngoing_ = true;
    CallAfter(&GuiUpdate::guiRefreshPanelHelper);
}

void GuiUpdate::guiRefreshPanelHelper()
{
    p_Computer->refreshPanel();
    panelRefreshOngoing_ = false;
}

void GuiUpdate::guiUpdateLedStatus(bool status, int card, int i)
{
    callAfterStatus_ = status;
    callAfterCard_ = card;
    callAfterI_ = i;
    CallAfter(&GuiUpdate::guiUpdateLedStatusHelper);
}

void GuiUpdate::guiUpdateLedStatusHelper()
{
    if (computerRunning_)
        p_Video[computerConfiguration.vis1870Configuration.videoNumber]->updateStatusLed(callAfterStatus_, callAfterCard_, callAfterI_);
}

void GuiUpdate::guiUpdateVipIILedStatus(int led, bool status)
{
    callAfterLed_ = led;
    callAfterLedStatus_ = status;
    CallAfter(&GuiUpdate::guiUpdateVipIILedStatusHelper);
}

void GuiUpdate::guiUpdateVipIILedStatusHelper()
{
    p_Computer->updateStatusBarLedStatus(callAfterLedStatus_, callAfterLed_);
}

void GuiUpdate::guiDebounceTimer()
{
    CallAfter(&GuiUpdate::guiDebounceTimerHelper);
}

void GuiUpdate::guiDebounceTimerHelper()
{
    keyDebounceTimeoutPointer->Start(80, wxTIMER_ONE_SHOT);
}

void GuiUpdate::guiBeepTimer(int frequency, int ms)
{
    p_Computer->toneBeep(frequency, true);
    callAfterMs_ = ms;
    CallAfter(&GuiUpdate::guiBeepTimerHelper);
}

void GuiUpdate::guiBeepTimerHelper()
{
    beepTimerPointer->Start(callAfterMs_, wxTIMER_ONE_SHOT);
}

void GuiUpdate::guiErrorMessage(wxString messageText)
{
    callAfterMessageText_ = messageText;
    CallAfter(&GuiUpdate::guiErrorMessageHelper);
}

void GuiUpdate::guiErrorMessageHelper()
{
    (void)wxMessageBox(callAfterMessageText_, "Emma 02", wxICON_ERROR | wxOK);
}

void GuiUpdate::guiKillComputer()
{
    CallAfter(&GuiUpdate::guiKillComputerHelper);
}

void GuiUpdate::guiKillComputerHelper()
{
    p_Main->killComputer();
}

