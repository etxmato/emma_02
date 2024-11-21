#ifndef PANEL_H
#define PANEL_H

#include "til.h"
#include "til313.full.h"
#include "led.h"

#define BUTTON_UP true
#define BUTTON_DOWN false
#define BUTTON_RIGHT true
#define BUTTON_LEFT false
#define BASICADDR_MAX 4

wxDEFINE_EVENT(COMPUTER_MSG, guiEvent);

#define guiEventHandler(func) (&func)

#define EVT_COMPUTER_MSG(id, func) \
wx__DECLARE_EVT1(COMPUTER_MSG, id, (&func))

class HexButtonCdp1851 : public HexButton
{
public:
    HexButtonCdp1851(wxDC& dc, int type, wxCoord x, wxCoord y, wxString label, int pioNumber);
    void OnTimer(wxTimerEvent& event);
    
private:
    int pioNumber_;
    
    DECLARE_EVENT_TABLE()
};

class HexButtonCdp1852 : public HexButton
{
public:
    HexButtonCdp1852(wxDC& dc, int type, wxCoord x, wxCoord y, wxString label, int pioNumber);
    void OnTimer(wxTimerEvent& event);
    
private:
    int pioNumber_;
    
    DECLARE_EVENT_TABLE()
};

class RunComputer : public wxThread
{
public:
    RunComputer() {};
    virtual void *Entry();
};

class Panel : public wxWindow
{
public:
    Panel(wxWindow *parent, const wxSize& size);
    virtual ~Panel();

    void init(vector<GuiItemConfiguration> buttonConfig, wxSize panelSize);
    virtual void init();
    virtual void init(int computerType);
    void connectKeyEvent(wxWindow* pclComponent);
    virtual void onPaint(wxPaintEvent&event);
    void refreshPanel();
    void onChar(wxKeyEvent&event);
    void vtOut(int value);
    void onKeyDown(wxKeyEvent&event);
    void onKeyUp(wxKeyEvent&event);
    Byte getKey(Byte vtOut);
    virtual void onMousePress(wxMouseEvent& event);
    virtual void onMouseRelease(wxMouseEvent& event);
    virtual void onMouseRightRelease(wxMouseEvent& event);
    void executeMousePressFunction(std::vector<GuiItemConfiguration>::iterator button);
    void executeMouseReleaseFunction(std::vector<GuiItemConfiguration>::iterator button);
    void executeMouseLeftFunction(std::vector<GuiItemConfiguration>::iterator button);
    void executeMouseRightReleaseFunction(std::vector<GuiItemConfiguration>::iterator button);
    void ledTimeout();
    void rePaintLeds(wxDC& dc);
    void setLedMs(long ms);

    void setReadyLed(int status);
    void updateReadyLed(wxDC& dc);
    void setStopLed(int status);
    void updateStopLed(wxDC& dc);
    void setPowerLed(int status);
    void updatePowerLed(wxDC& dc);
    void setErrorLed(int status);
    void updateErrorLed(wxDC& dc);
    void setQLed(int status);
    void updateQLed(wxDC& dc);
    void setLed(int i, int status);
    void updateLed(wxDC& dc, int i);
    void refreshLed(wxDC& dc, int i);
    void setNibbleLed(int i, int status);
    void updateNibbleLed(wxDC& dc, int i);
    void setStateLed(int i, int status);
    void updateStateLed(wxDC& dc, int i);
    void showData(Byte value);
    void updateData(wxDC& dc);
    void showDp313Italic(bool status, int i);
    void turnOff313Italic(bool status);
    void updateDp313Italic(wxDC& dc);
    void showSeg(int number, Byte value);
    void updateSeg(wxDC& dc, int number);
    void showMulti(int number, Byte value);
    void updateMulti(wxDC& dc, int number);
    void showAddress(Word address);
    void updateAddress(wxDC& dc);
    virtual void inUp();
    virtual void inDown();
    void inSetState(bool state);
    void clearSetState(bool state);
    void clearResetSetState(bool state);
//    virtual void resetSetState(bool state);
    void waitSetState(bool state);
    void runSetState(bool state);
    void powerSetState(bool state);
    void releaseButtonOnScreen(HexButton* WXUNUSED(buttonPoint));

    void onAdsChannel(wxSpinEvent&event);
    void onAdsVolt(wxSpinEvent&event);
    void onAdsChannelText(wxCommandEvent& event);
    void onAdsVoltText(wxCommandEvent& event);
    void onAdiChannel(wxSpinEvent&event);
    void onAdiVolt(wxSpinEvent&event);
    void onAdiChannelText(wxCommandEvent& event);
    void onAdiVoltText(wxCommandEvent& event);

    void setAds(int i, int val);
    bool isAdsDefined();
    int getAds(int i);
    void setAdi(int i, int val);
    bool isAdiDefined();
    int getAdi(int i);
    
protected:
    wxBitmap *mainBitmapPointer;
    std::vector<GuiItemConfiguration>::iterator runButton;
    bool runButtonDefined;
    std::vector<GuiItemConfiguration>::iterator powerButton;
    bool powerButtonDefined;
    std::vector<GuiItemConfiguration>::iterator inButton;
    bool inButtonDefined;
    std::vector<GuiItemConfiguration>::iterator waitButton;
    bool waitButtonDefined;
    std::vector<GuiItemConfiguration>::iterator clearButton;
    bool clearButtonDefined;
    std::vector<GuiItemConfiguration>::iterator clearResetButton;
    bool clearResetButtonDefined;
    std::vector<GuiItemConfiguration>::iterator clearRunButton;
    bool clearRunButtonDefined;

#if wxCHECK_VERSION(2, 9, 0)
    wxButton *inButtonPointer;
    wxButton *resetButtonPointer;
    wxButton *runButtonPointer;
    wxButton *mpButtonPointer;
    wxButton *powerButtonPointer;
    wxButton *loadButtonPointer;
    wxButton *ramButtonPointer;
    wxButton *dataSwitchPointer[8];
    wxButton *efSwitchPointer[4];
#else
    wxBitmapButton *inButtonPointer;
    wxBitmapButton *resetButtonPointer;
    wxBitmapButton *runButtonPointer;
    wxBitmapButton *mpButtonPointer;
    wxBitmapButton *powerButtonPointer;
    wxBitmapButton *loadButtonPointer;
    wxBitmapButton *ramButtonPointer;
    wxBitmapButton *dataSwitchPointer[8];
    wxBitmapButton *efSwitchPointer[4];
#endif
    wxButton *buttonPointer[16];
    wxButton *pauseButtonPointer;
    wxButton *stepButtonPointer;
    wxButton *monitorButtonPointer;
    wxButton *clearButtonPointer;
    wxButton *enterButtonPointer;
    wxButton *reqButtonPointer;
    wxButton *seqButtonPointer;
    wxButton *decButtonPointer;
    wxButton *retButtonPointer;
    wxButton *push_inButtonPointer;
    wxButton *text_readButtonPointer;
    wxButton *text_resetButtonPointer;
    wxButton *text_loadButtonPointer;
    wxButton *text_runButtonPointer;
    wxButton *text_runPButtonPointer;
    wxButton *text_mpButtonPointer;
    wxButton *text_dataSwitchPointer[8];
    wxButton *text_stbButtonPointer;
    wxButton *text_ardyButtonPointer;
    wxButton *text_brdyButtonPointer;

    HexButton *osx_pauseButtonPointer;
    HexButton *osx_monitorButtonPointer;
    HexButton *osx_stepButtonPointer;
    HexButton *osx_push_inButtonPointer;
    HexButton *osx_text_readButtonPointer;
    HexButton *osx_text_resetButtonPointer;
    HexButton *osx_text_loadButtonPointer;
    HexButton *osx_text_runButtonPointer;
    HexButton *osx_text_runPButtonPointer;
    HexButton *osx_text_mpButtonPointer;
    HexButton *osx_text_dataSwitchPointer[8];
    HexButton *osx_buttonPointer[16];
    HexButton *osx_clearButtonPointer;
    HexButton *osx_enterButtonPointer;
    HexButton *osx_reqButtonPointer;
    HexButton *osx_seqButtonPointer;
    HexButton *osx_decButtonPointer;
    HexButton *osx_retButtonPointer;
    HexButton *osx_stbButtonPointer;
    HexButton *osx_ardyButtonPointer;
    HexButton *osx_brdyButtonPointer;

    SwitchButton *powerSwitchButton;
    SwitchButton *resetSwitchButton;
    SwitchButton *runSwitchButton;
    SwitchButton *loadSwitchButton;
    SwitchButton *mpSwitchButton;
    SwitchButton *ramSwitchButton;
    SwitchButton *inSwitchButton;
    SwitchButton *readSwitchButton;
    SwitchButton *cardSwitchButton;
    SwitchButton *clearSwitchButton;
    SwitchButton *waitSwitchButton;
    SwitchButton *stepSwitchButton;
    SwitchButton *velfSwitchButton;
    SwitchButton *dataSwitchButton[20];
    SwitchButton *efSwitchButton[4];
    SwitchButton *dipSwitchButton[4];

    wxBitmap *hexButtonPointer;
    wxBitmap *upBitmapPointer;
    wxBitmap *downBitmapPointer;
    wxBitmap *pushUpBitmapPointer;
    wxBitmap *pushDownBitmapPointer;

    wxMask *maskUp;
    wxMask *maskDown;

    bool tilDataPointerDefined[MAX_DATA_TIL];
    Til *dataPointer[MAX_DATA_TIL];
    bool tilAddressPointerDefined[MAX_ADDRESS_TIL];
    Til *addressPointer[MAX_ADDRESS_TIL];
    bool tilMultiPointerDefined[MAX_MULTI_TIL];
    Tilfull *multiPointer[MAX_MULTI_TIL];
    Tilfull *segPointer[MAX_MULTI_TIL];
    
    Word addressStatus;
    Byte dataStatus;
    Byte multiStatus[MAX_MULTI_TIL];
    Byte segStatus[MAX_MULTI_TIL];
    bool dpStatus[MAX_DATA_TIL];

    Led *readyLedPointer;
    bool readyLedPointerDefined;
    Led *stopLedPointer;
    bool stopLedPointerDefined;
    Led *powerLedPointer;
    bool powerLedPointerDefined;
    Led *errorLedPointer;
    bool errorLedPointerDefined;
    Led *qLedPointer;
    bool qLedPointerDefined;
    Led *nibbleLedPointer[2];
    bool nibbleLedPointerDefined[2];
    Led *ledPointer[MAX_BIT_LEDS];
    bool ledPointerDefined[MAX_BIT_LEDS];
    Led *stateLedPointer[MAX_CPU_STATE_LEDS];
    bool stateLedPointerDefined[MAX_CPU_STATE_LEDS];

    int readyLedStatus;
    int stopLedStatus;
    int powerLedStatus;
    int errorLedStatus;
    int qLedStatus;
    int nibbleLedStatus[2];
    int ledStatus[MAX_BIT_LEDS];
    int stateLedStatus[MAX_CPU_STATE_LEDS];

    bool updateReadyLed_;
    bool updateStopLed_;
    bool updatePowerLed_;
    bool updateErrorLed_;
    bool updateQLed_;
    bool updateNibbleLed_[2];
    bool updateLed_[MAX_BIT_LEDS];
    bool updateStateLed_[MAX_CPU_STATE_LEDS];
    bool updateData_;
    bool updateAddress_;
    bool updateMulti_[MAX_MULTI_TIL];
    bool updateDp313_[MAX_DATA_TIL];
    bool updateSeg_[MAX_MULTI_TIL];

    long ms_;

    char keyBuffer_[26];
    int  keyStart_;
    int  keyEnd_;
    int lastKey_;
    bool repeat_;
    bool forceUpperCase_;
    bool xmlButtonDefined_;

    wxSpinCtrl *spinCtrlAdi;
    wxSpinCtrl *spinCtrlAdiVolt;
    wxSpinCtrl *spinCtrlAds;
    wxSpinCtrl *spinCtrlAdsVolt;

    bool spinCtrlAdiDefined;
    bool spinCtrlAdiVoltDefined;
    bool spinCtrlAdsDefined;
    bool spinCtrlAdsVoltDefined;

    int adiArray_[16];
    int adsArray_[16];

private:
    bool functionKeyReleaseTwo_;\
    
    vector<GuiItemConfiguration> guiItemConfiguration;
    wxSize panelSize_;
    
    DECLARE_EVENT_TABLE()
};

class PanelFrame : public wxFrame
{
public:
    PanelFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~PanelFrame();
  
    void onClose(wxCloseEvent& event);

    void init(vector<GuiItemConfiguration> buttonConfig, wxSize panelSize) {panelPointer->init(buttonConfig, panelSize);};
    void ledTimeout() {panelPointer->ledTimeout();};
    void setLedMs(long ms) {panelPointer->setLedMs(ms);};
    Byte getKey(Byte vtOut) {return panelPointer->getKey(vtOut);};
    void releaseButtonOnScreen(HexButton* buttonPoint) {panelPointer->releaseButtonOnScreen(buttonPoint);};
    void refreshPanel() {panelPointer->refreshPanel();};

    void setLed(int i, int status) {panelPointer->setLed(i, status);};
    void setNibbleLed(int i, int status) {panelPointer->setNibbleLed(i, status);};
    void setPowerLed(int status) {panelPointer->setPowerLed(status);};
    void setStateLed(int i, int status) {panelPointer->setStateLed(i, status);};
    void setReadyLed(int status) {panelPointer->setReadyLed(status);};
    void setStopLed(int status) {panelPointer->setStopLed(status);};
    void setQLed(int status) {panelPointer->setQLed(status);};
    void setErrorLed(int status) {panelPointer->setQLed(status);};

    void showData(Byte value) {panelPointer->showData(value);};
    void showAddress(Word address) {panelPointer->showAddress(address);};
    void showMulti(int number, Byte value) {panelPointer->showMulti(number, value);};
    void showDp313Italic(bool status, int i) {panelPointer->showDp313Italic(status, i);};
    void turnOff313Italic(bool status) {panelPointer->turnOff313Italic(status);};

    void inSetState(bool state) {panelPointer->inSetState(state);};
    void clearSetState(bool state) {panelPointer->clearSetState(state);};
    void waitSetState(bool state) {panelPointer->waitSetState(state);};
    void runSetState(bool state) {panelPointer->runSetState(state);};
    void clearResetSetState(bool state) {panelPointer->clearResetSetState(state);};
    void powerSetState(bool state) {panelPointer->powerSetState(state);};

    bool isAdiDefined() {return panelPointer->isAdiDefined();};
    int getAdi(int i) {return panelPointer->getAdi(i);};
    bool isAdsDefined() {return panelPointer->isAdsDefined();};
    int getAds(int i) {return panelPointer->getAds(i);};

private:
    Panel *panelPointer;

    DECLARE_EVENT_TABLE()
};
#endif    // PANEL_H
