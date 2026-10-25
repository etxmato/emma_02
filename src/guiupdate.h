#ifndef GUIUPDATE_H
#define GUIUPDATE_H

#include "guimain.h"
#include "computerconfig.h"

class GuiUpdate: public GuiMain
{
public:

    GuiUpdate(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~GuiUpdate() {};

    void guiSetButtonLabel(wxString info, wxString value);
    void guiSetCheckBox(wxString info, bool state);
    void guiShowMessage(Word value);
    void guiSetTextValue(wxString info, wxString value);
    void guiSetStaticTextValue(wxString info, wxString value);
    void guiEnableClock(bool state);
    void guiSetConvertState(bool state);
    void guiSetLocation(bool state, Word saveStart, Word saveEnd, Word saveExec);
    void guiSetLocationState(bool state);
    void guiSaveStart(Word saveStart);
    void guiSaveEnd(Word saveEnd);
    void guiSetTapeState(int tapeState, wxString tapeNumber);
    void guiSetSwName(wxString swName);
    void guiUpdateTitle();
    void guiHwTapeStateChange(int status);
    void guiShowTextMessage(wxString messageText);
    void guiPrintParallel(Byte value);
    void guiPrintParallelFinished();
    void guiPrintThermal(Byte value, Byte Qflag);
    void guiPrintThermalFinished();
    void guiPrintSerial(Byte value);
    void guiPrintSerialFinished();
    void guiPrintPecom(Byte value);
    void guiZoomChange(double zoom, int videoNumber);
    void guiZoomVtChange(double zoom, int uartNumber);
    void guiZoom(double zoom, int videoNumber, bool isVt);
    void guiVideoSetFullScreen(bool state, int videoNumber);
    void guiVtSetFullScreen(bool state, int uartNumber);
    void guiChangeNoteBook();
    void guiSetFandMBasicGui();
    void guiPauseState();
    void guiRefreshVideo(bool isVt, int uart_video_Number);
    void guiRefreshPanel();
    void guiUpdateLedStatus(bool status, int card, int i = 0);
    void guiUpdateVipIILedStatus(int led, bool status);
    void guiDebounceTimer();
    void guiBeepTimer(int frequency, int ms);
    void guiErrorMessage(wxString messageText);
    void guiKillComputer();

    bool getThermalEf() { return thermalEf_; };

protected:
    bool thermalEf_;
    bool panelRefreshOngoing_;
    bool videoRefreshOngoing_[VIDEOXMLMAX];

private:
    // Every helper takes the values it needs as parameters: the call is queued
    // and may run after a later update has been posted (see EmmaCallAfter()).
    void guiSetButtonLabelHelper(wxString info, wxString value);
    void guiSetCheckBoxHelper(wxString info, bool state);
    void guiShowMessageHelper(wxString messageText);
    void guiSetTextValueHelper(wxString info, wxString value);
    void guiSetStaticTextValueHelper(wxString info, wxString value);
    void guiEnableClockHelper(bool state);
    void guiSetConvertStateHelper(bool state);
    void guiSetLocationHelper(bool state, wxString saveStartString, wxString saveEndString, wxString saveExecString);
    void guiSetLocationStateHelper(bool state);
    void guiSaveStartHelper(wxString saveStartString);
    void guiSaveEndHelper(wxString saveEndString);
    void guiSetTapeStateHelper(int tapeState, wxString tapeNumber);
    void guiSetSwNameHelper(wxString swName);
    void guiUpdateTitleHelper();
    void guiHwTapeStateChangeHelper(int status);
    void guiShowTextMessageHelper(wxString messageText);
    void guiPrintParallelHelper(Byte value);
    void guiPrintParallelFinishedHelper();
    void guiPrintThermalHelper(Byte value, Byte Qflag);
    void guiPrintThermalFinishedHelper();
    void guiPrintSerialHelper(Byte value);
    void guiPrintSerialFinishedHelper();
    void guiPrintPecomHelper(Byte value);
    void guiZoomChangeHelper(double zoom, int videoNumber);
    void guiZoomVtChangeHelper(double zoom, int uartNumber);
    void guiZoomHelper(double zoom, int videoNumber, bool isVt);
    void guiVideoSetFullScreenHelper(bool state, int videoNumber);
    void guiVtSetFullScreenHelper(bool state, int uartNumber);
    void guiChangeNoteBookHelper();
    void guiSetFandMBasicGuiHelper();
    void guiPauseStateHelper();
    void guiRefreshVideoHelper(bool isVt, int uart_video_Number);
    void guiRefreshPanelHelper();
    void guiUpdateLedStatusHelper(bool status, int card, int i);
    void guiUpdateVipIILedStatusHelper(int led, bool status);
    void guiDebounceTimerHelper();
    void guiBeepTimerHelper(int ms);
    void guiErrorMessageHelper(wxString messageText);
    void guiKillComputerHelper();

    DECLARE_EVENT_TABLE()
};

#endif // GUIUPDATE_H
