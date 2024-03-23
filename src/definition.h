#ifndef DEFINITION_H
#define DEFINITION_H

#define MAXROM 6
#define VIDEOXMLMAX 13

#define STATUSBAR_NONE 0
#define STATUSBAR_COMX 1
#define STATUSBAR_DIAG 2
#define STATUSBAR_CIDELSA 3
#define STATUSBAR_VIP2 4

#define MATRIX_CTRL_KEY 0
#define MATRIX_SHIFT_KEY 1
#define MATRIX_CAPS_KEY 2
#define MATRIX_ESC_KEY 3
#define LAST_MATRIX_EF_KEY 4

#define MATRIX_TEXT_RETURN_KEY 0
#define MATRIX_TEXT_END_KEY 1
#define MATRIX_TEXT_HOME_KEY 2
#define MATRIX_TEXT_DOWN_KEY 3
#define MATRIX_TEXT_LEFT_KEY 4
#define MATRIX_TEXT_RIGHT_KEY 5
#define MATRIX_TEXT_UP_KEY 6
#define MATRIX_TEXT_ESC_KEY 7
#define MATRIX_TEXT_BACK_KEY 8
#define LAST_MATRIX_TEXT_KEY 9

#define TAPE_FORMAT_AUTO 0
#define TAPE_FORMAT_PM 1
#define TAPE_FORMAT_56 2
#define TAPE_FORMAT_CV 3

#define MAX_BITKEYPADS 7
#define MAX_LATCHKEYPADS 3

#define TAPE_MODE_PROGRAM 0x10
#define TAPE_MODE_DIRECT 0x20
#define TAPE_MODE_WRITE 0x40

#define INP_MODE_NONE 0
#define INP_MODE_KEYPAD 1
#define INP_MODE_TAPE_PROGRAM 2
#define INP_MODE_TAPE_DIRECT 3
#define INP_MODE_TAPE_RUN 4
#define INP_MODE_KEY_DIRECT 5

#define FRED_TAPE_FORMAT_AUTO 0
#define FRED_TAPE_FORMAT_PM 1
#define FRED_TAPE_FORMAT_56 2

#define MAX_BIT_LEDS 24
#define MAX_CPU_STATE_LEDS 4
#define MAX_DATA_TIL 2
#define MAX_ADDRESS_TIL 4

#define SC0LED 0
#define SC1LED 1
#define WAITLED 2
#define CLEARLED 3

#define AD_CONVERTOR_PRINT 1
#define AD_CONVERTOR_TEMP 2
#define AD_CONVERTOR_SELECT 3

#define RESETSTATE 0
#define BASICSTATE 1
#define RUNSTATE 2
#define BASICSTATECW 3
#define UTSTATE 4
#define LAST_COMMAND_STATE 5

#define COMMAND_C 3

enum
{
    PIXIE_IN_OUT,
    PIXIE_IN_IN,
    PIXIE_OUT_OUT,
    PIXIE_DOUBLE
};

enum
{
    BUTTON_FUNC_IN,
    BUTTON_FUNC_RUN,
    BUTTON_FUNC_MP,
    BUTTON_FUNC_RUN0,
    BUTTON_FUNC_LOAD,
    BUTTON_FUNC_PAUSE,
    BUTTON_FUNC_STEP,
    BUTTON_FUNC_RESET,
    BUTTON_FUNC_MONITOR,
    BUTTON_FUNC_POWER,
    BUTTON_FUNC_READ,
    BUTTON_FUNC_NANO_MONITOR,
    BUTTON_FUNC_CARDSWITCH,
    BUTTON_FUNC_CARD,
    BUTTON_FUNC_DIPMP,
    BUTTON_FUNC_EMS,
    BUTTON_FUNC_TMC_AD,
    LED_FUNC_STOP,
    LED_FUNC_READY,
    LED_FUNC_ERROR,
    LED_FUNC_Q,
    LED_FUNC_RESET,
    LED_FUNC_PAUSE,
    LED_FUNC_RUN,
    LED_FUNC_LOAD,
    LED_FUNC_BIT,
    LED_FUNC_ADDRESS,
    LED_FUNC_CPUSTATE,
    TIL_DATA,
    TIL_ADDRESS,
};

enum
{
    SWITCH_BUTTON_VERTICAL,
    SWITCH_BUTTON_HORIZONTAL,
    ELF2K_POWER_BUTTON,
    PUSH_BUTTON_ROUND_BLACK,
    SWITCH_BUTTON_VERTICAL_RED,
    SWITCH_BUTTON_VERTICAL_PIO,
    PUSH_BUTTON_ROUND_RED,
    ELF2K_LOAD_BUTTON,
    ELF2K_MP_BUTTON,
    ELF2K_RUN_BUTTON,
    ELF2K_IN_BUTTON,
    DIP_SWITCH_BUTTON,
    ROT_SWITCH_BUTTON,
    ADI_SPINCTRL,
    ADI_VOLT_SPINCTRL,
    ADS_SPINCTRL,
    ADS_VOLT_SPINCTRL,
    PUSH_BUTTON,
    PUSH_BUTTON_SMALL,
    PUSH_BUTTON_RECTANGLE,
    PUSH_BUTTON_RED,
    PUSH_BUTTON_PIO,
    PUSH_BUTTON_RECTANGLE_SMALL,
    PANEL_TEXT,
    PANEL_PNG,
    LED_SMALL_RED,
    LED_REAL_RED,
    LED_REAL_GREEN,
    LED_REAL_ORANGE,
    LED_LARGE_RED,
    LED_LARGE_GREEN,
    LED_SMALL_RED_DISABLE,
    TIL_311,
    TIL_313,
    TIL_313_ITALIC,
};

enum
{
    RUN_TYPE_DEFAULT,
    RUN_TYPE_BOOT,
    RUN_TYPE_RESET,
    RUN_TYPE_MONITOR,
};

enum
{
    RESET_TYPE_DEFAULT,
    RESET_TYPE_FULL,
};

enum
{
    HW_TAPE_STATE_OFF,
    HW_TAPE_STATE_PLAY,
    HW_TAPE_STATE_FF,
    HW_TAPE_STATE_REC,
    HW_TAPE_STATE_RW,
};

enum
{
    COL_V1870_MAIN =56,
    COL_MC6845_FORE =64,
    COL_MC6845_BACK,
    COL_VT_FORE,
    COL_VT_BACK,
    COL_VT_HIGH,
    COL_CDP1862_WHITE,
    COL_CDP1862_RED,
    COL_CDP1862_BLUE,
    COL_CDP1862_MAGENTA,
    COL_CDP1862_GREEN,
    COL_CDP1862_YELLOW,
    COL_CDP1862_CYAN,
    COL_CDP1862_BLACK,
    COL_CDP1862_BACK_BLUE,
    COL_CDP1862_BACK_BLACK,
    COL_CDP1862_BACK_GREEN,
    COL_CDP1862_BACK_RED,
    COL_PIXIE_FORE,
    COL_PIXIE_BACK,
    COL_PIXIE_VISICOM_COL1,
    COL_PIXIE_VISICOM_COL2,
    COL_PIXIE_VISICOM_COL3,
    COL_CDP1864_WHITE,
    COL_CDP1864_RED,
    COL_CDP1864_BLUE,
    COL_CDP1864_MAGENTA,
    COL_CDP1864_GREEN,
    COL_CDP1864_YELLOW,
    COL_CDP1864_CYAN,
    COL_CDP1864_BLACK,
    COL_CDP1864_BACK_BLUE,
    COL_CDP1864_BACK_BLACK,
    COL_CDP1864_BACK_GREEN,
    COL_CDP1864_BACK_RED,
    COL_I8275_FORE,
    COL_I8275_BACK,
    COL_I8275_HIGH,
    COL_MC6847_TEXT_BLACK,
    COL_MC6847_TEXT_GREEN,
    COL_MC6847_TEXT_ORANGE,
    COL_MC6847_GRAPH_GREEN,
    COL_MC6847_GRAPH_YELLOW,
    COL_MC6847_GRAPH_BLUE,
    COL_MC6847_GRAPH_RED,
    COL_MC6847_GRAPH_BUFF,
    COL_MC6847_GRAPH_CYAN,
    COL_MC6847_GRAPH_MAGENTA,
    COL_MC6847_GRAPH_ORANGE,
    COL_TMS_TRANSPARANT,
    COL_TMS_BLACK,
    COL_TMS_MEDIUM_GREEN,
    COL_TMS_LIGHT_GREEN,
    COL_TMS_DARK_BLUE,
    COL_TMS_LIGHT_BLUE,
    COL_TMS_DARK_RED,
    COL_TMS_AQUA_BLUE,
    COL_TMS_MEDIUM_RED,
    COL_TMS_LIGHT_RED,
    COL_TMS_DARK_YELLOW,
    COL_TMS_LIGHT_YELLOW,
    COL_TMS_DARK_GREEN,
    COL_TMS_PURPLE,
    COL_TMS_GRAY,
    COL_TMS_WHITE,
    COL_SN76430N_WHITE,
    COL_SN76430N_YELLOW,
    COL_SN76430N_GREEN,
    COL_SN76430N_RED,
    COL_SN76430N_BLACK,
    COL_ST4_BLACK,
    COL_ST4_RED,
    COL_ST4_BLUE,
    COL_ST4_MAGENTA,
    COL_ST4_GREEN,
    COL_ST4_YELLOW,
    COL_ST4_CYAN,
    COL_ST4_WHITE,
    COL_ST4_BACK_BLACK,
    COL_ST4_BACK_RED,
    COL_ST4_BACK_BLUE,
    COL_ST4_BACK_MAGENTA,
    COL_ST4_BACK_GREEN,
    COL_ST4_BACK_YELLOW,
    COL_ST4_BACK_CYAN,
    COL_ST4_BACK_WHITE,
    COL_MAX
};

enum
{
    PIXIE_COLOR_DEFAULT,
    PIXIE_COLOR_ETI_1862,
    PIXIE_COLOR_ETI_1864,
    PIXIE_COLOR_VIP_1862,
    PIXIE_COLOR_VIP_1864,
    PIXIE_COLOR_VICTORY_1862,
    PIXIE_COLOR_VICTORY_1864,
    PIXIE_COLOR_STUDIOIV,
    PIXIE_COLOR_TMC2000_1862,
    PIXIE_COLOR_TMC2000_1864,
    PIXIE_COLOR_VISICOM,
};

enum
{
    BAR_LED_RUN,
    BAR_LED_Q,
    BAR_LED_TAPE,
};

class SwitchButton
{
public:
    SwitchButton(wxDC& dc, int type, wxColour bkgrClr, bool state, wxCoord x, wxCoord y, wxString label);
    ~SwitchButton();
    void onPaint(wxDC& dc);
    bool onMousePress(wxDC& dc, wxCoord x, wxCoord y);
    bool onMouseRelease(wxDC& dc, wxCoord x, wxCoord y);
    void setState(wxDC& dc, bool state);
    void enable(wxDC& dc, bool enabled);

private:
    wxBitmap *upBitmapPointer;
    wxBitmap *downBitmapPointer;
    wxBitmap *disabledUpBitmapPointer;
    wxBitmap *disabledDownBitmapPointer;

    wxMask *maskUp;
    wxMask *maskDown;
    
    bool enabled_;
    
    wxCoord x_;
    wxCoord y_;
    bool state_;
    wxCoord buttonSizeX_;
    wxCoord buttonSizeY_;
    wxCoord buttonStartX_;
    wxCoord buttonStartY_;
    int type_;
};

class RotButton
{
public:
    RotButton(wxDC& dc, int state, wxCoord x, wxCoord y);
    ~RotButton();
    void onPaint(wxDC& dc);
    bool onMouseLeftRelease(wxDC& dc, wxCoord x, wxCoord y);
    bool onMouseRightRelease(wxDC& dc, wxCoord x, wxCoord y);
    void setState(wxDC& dc, int state);

private:
    wxBitmap *bitmapPointer[16];
        
    wxCoord x_;
    wxCoord y_;
    int state_;
    wxCoord buttonSizeX_;
    wxCoord buttonSizeY_;
    wxCoord buttonStartX_;
    wxCoord buttonStartY_;
};

class HexButton : public wxEvtHandler
{
public:
    HexButton(wxDC& dc, int type, wxCoord x, wxCoord y, wxString label);
    ~HexButton();
    void onPaint(wxDC& dc);
    bool onMousePress(wxDC& dc, wxCoord x, wxCoord y);
    bool onMouseRelease(wxDC& dc, wxCoord x, wxCoord y);
    virtual void OnTimer(wxTimerEvent& event);
    void releaseButtonOnScreen(wxDC& dc);
    void enable(wxDC& dc, bool enabled);

protected:
    bool state_;
    int buttonType_;

private:
    wxBitmap *upBitmapPointer;
    wxBitmap *downBitmapPointer;
    wxBitmap *disabledBitmapPointer;

    wxCoord x_;
    wxCoord y_;
 
    bool enabled_;

    wxTimer *focusTimer;

    DECLARE_EVENT_TABLE()
};

#endif // COMPUTERCONFIG_H
