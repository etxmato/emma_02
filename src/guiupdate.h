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
    void guiSetButtonLabelHelper();
    void guiSetCheckBoxHelper();
    void guiShowMessageHelper();
    void guiSetTextValueHelper();
    void guiSetStaticTextValueHelper();
    void guiEnableClockHelper();
    void guiSetConvertStateHelper();
    void guiSetLocationHelper();
    void guiSetLocationStateHelper();
    void guiSaveStartHelper();
    void guiSaveEndHelper();
    void guiSetTapeStateHelper();
    void guiSetSwNameHelper();
    void guiUpdateTitleHelper();
    void guiHwTapeStateChangeHelper();
    void guiShowTextMessageHelper();
    void guiPrintParallelHelper();
    void guiPrintParallelFinishedHelper();
    void guiPrintThermalHelper();
    void guiPrintThermalFinishedHelper();
    void guiPrintSerialHelper();
    void guiPrintSerialFinishedHelper();
    void guiPrintPecomHelper();
    void guiZoomChangeHelper();
    void guiZoomVtChangeHelper();
    void guiZoomHelper();
    void guiVideoSetFullScreenHelper();
    void guiVtSetFullScreenHelper();
    void guiChangeNoteBookHelper();
    void guiSetFandMBasicGuiHelper();
    void guiPauseStateHelper();
    void guiRefreshVideoHelper();
    void guiRefreshPanelHelper();
    void guiUpdateLedStatusHelper();
    void guiUpdateVipIILedStatusHelper();
    void guiDebounceTimerHelper();
    void guiBeepTimerHelper();
    void guiErrorMessageHelper();
    void guiKillComputerHelper();

    wxString callAfterInfo_;
    wxString callAfterValue_;
    wxString callAfterText_;
    wxString callAfterSwName_;
    wxString callAfterMessageText_;
    wxString callAfterTapeNumber_;
    wxString callAfterSaveStartString_;
    wxString callAfterSaveEndString_;
    wxString callAfterSaveExecString_;
    bool callAfterState_;
    bool callAfterStatus_;
    bool callAfterIsVt_;
    bool callAfterLedStatus_;
    int callAfterCard_;
    int callAfterI_;
    int callAfterLed_;
    int callAfterTapeState_;
    int callAfterUartVideoNumber_;
    int callAfterUartNumber_;
    int callAfterVideoNumber_;
    int callAfterFrequency_;
    int callAfterMs_;
    double callAfterZoom_;
    Byte callAfterValueByte_;
    Byte callAfterQflag_;
    Word callAfterMessage_;

    DECLARE_EVENT_TABLE()
};

#endif // GUIUPDATE_H
