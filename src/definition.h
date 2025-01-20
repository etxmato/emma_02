
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
#define MATRIX_TEXT_PAUSE_KEY 9
#define MATRIX_TEXT_MENU_KEY 10
#define MATRIX_TEXT_TAB_KEY 11
#define MATRIX_TEXT_NUMPAD_ENTER_KEY 12
#define LAST_MATRIX_TEXT_KEY 13

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

#define MAX_NIBBLE_LEDS 2
#define MAX_BIT_LEDS 24
#define MAX_DATA_TIL 2
#define MAX_ADDRESS_TIL 4
#define MAX_MULTI_TIL 8
#define MAX_THUMB_VALUE 2

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

#define KEY_UP 0
#define KEY_LEFT 1
#define KEY_RIGHT 2
#define KEY_DOWN 3
#define KEY_FIRE 4
#define KEY_COIN 5

#define BOOTSTRAPREAD 0
#define BOOTSTRAPOUT 1
#define BOOTSTRAPIN 2
#define BOOTSTRAPIO 3
#define BOOTSTRAPRUN 4
#define BOOTSTRAPNONE 5
#define BOOTSTRAPTIMER 6
#define BOOTSTRAPTIMER_ONIN 7
#define BOOTSTRAPQ 8

#define LOAD 0
#define RESET 1
#define PAUSE 2
#define RUN 3
#define UNDEFINDEDMODE 5

#define PANEL_MAIN 0
#define POS_TYPE_GRID 0
#define POS_TYPE_RELATIVE 1
#define POS_TYPE_REAL 2

#define IN_BUTTON_PUSH false
#define IN_BUTTON_SWITCH true

enum
{
    UNDEFINED,
    RAM,
    RAM_MAP1,
    RAM_MAP2,
    RAM_MAP3,
    RAM_MAP4,
    RAM_MAP5,
    RAM_MAP6,
    RAM_MAP7,
    ROM,
    ROM_MAP1,
    ROM_MAP2,
    ROM_MAP3,
    ROM_MAP4,
    ROM_MAP5,
    ROM_MAP6,
    ROM_MAP7,
    NVRAM,
    NVRAM_MAP1,
    NVRAM_MAP2,
    NVRAM_MAP3,
    NVRAM_MAP4,
    NVRAM_MAP5,
    NVRAM_MAP6,
    NVRAM_MAP7,
    RAMROM,
    PAGER,
    PARTRAM,
    CRAM1870,
    PRAM1870,
    MC6845RAM,
    MC6845REGISTERS,
    EMSMEMORY,
    MC6847RAM,
    CARTRIDGEROM,
    MAPPEDRAM,
    COLOURRAM,
    VP570RAM,
    DIAGROM,
    MAPPEDROM,
    TESTCARTRIDGEROM,
    MCRMEM,
    CPURAM,
    UART1_82C51,
    UART2_82C51,
    NOCHANGE,
    SLOTMEM,
    SN76430NRAM,
    COLOURRAM1864,
    COLOURRAMST4,
};

enum
{
    LOADLED,
    RESETLED,
    PAUSELED,
    RUNLED,
    SC0LED,
    SC1LED,
    WAITLED,
    CLEARLED,
    DMALED,
    INTERRUPTLED,
    DISKLED,
    MAX_CPU_STATE_LEDS
};

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
    BUTTON_FUNC_SUPER_MONITOR,
    BUTTON_FUNC_STOP,
    BUTTON_FUNC_READ,
    BUTTON_FUNC_NANO_MONITOR,
    BUTTON_FUNC_CARDSWITCH,
    BUTTON_FUNC_POWER,
    BUTTON_FUNC_SUPER_MP,
    BUTTON_FUNC_WAIT,
    BUTTON_FUNC_CLEAR,
    BUTTON_FUNC_CLEAR_RUN,
    BUTTON_FUNC_CLEAR_RESET,
    BUTTON_FUNC_NVRAM_DISABLE,
    BUTTON_FUNC_IN_INTERRUPT,
    BUTTON_FUNC_IN_SWITCH,

    BUTTON_FUNC_CARD,
    BUTTON_FUNC_DIPMP,
    BUTTON_FUNC_EMS,
    BUTTON_FUNC_TMC_AD,
    BUTTON_FUNC_BIT,
    BUTTON_FUNC_EF_SWITCH,
    BUTTON_FUNC_VELF,
    BUTTON_FUNC_HEX,
    BUTTON_FUNC_EF,
    BUTTON_FUNC_THUMB_MINUS,
    BUTTON_FUNC_THUMB_PLUS,
    LED_FUNC_POWER,
    LED_FUNC_STOP,
    LED_FUNC_READY,
    LED_FUNC_ERROR,
    LED_FUNC_Q,
    LED_FUNC_RESET,
    LED_FUNC_PAUSE,
    LED_FUNC_RUN,
    LED_FUNC_LOAD,
    LED_FUNC_BIT,
    LED_FUNC_SWITCH,
    LED_FUNC_BUTTON,
    LED_FUNC_ADDRESS,
    LED_FUNC_NIBBLE,
    LED_FUNC_FETCH,
    LED_FUNC_EXECUTE,
    LED_FUNC_WAIT,
    LED_FUNC_CLEAR,
    LED_FUNC_DMA,
    LED_FUNC_INTERRUPT,
    LED_FUNC_DISK,
    LED_FUNC_CPUSTATE,
    TIL_DATA,
    TIL_ADDRESS,
    TIL_MULTI,
};

enum
{
    SWITCH_BUTTON_VERTICAL,
    SWITCH_BUTTON_HORIZONTAL,
    ELF2K_POWER_BUTTON,
    DIP_SWITCH_BUTTON,
    SWITCH_BUTTON_VERTICAL_RED,
    SWITCH_BUTTON_VERTICAL_GREEN,
    SWITCH_BUTTON_ORANGE,
    SWITCH_BUTTON_SQUARE_GREEN,
    SWITCH_BUTTON_YELLOW,
    SWITCH_BUTTON_BLUE,
    SWITCH_BUTTON_VERTICAL_PIO, // Last switch button - all switch buttons need to be defined before this
    PUSH_BUTTON,                // First push button - only push button can be defined after this
    PUSH_BUTTON_SMALL,
    PUSH_BUTTON_RECTANGLE,
    PUSH_BUTTON_RED,
    PUSH_BUTTON_ORANGE,
    PUSH_BUTTON_GREEN,
    PUSH_BUTTON_YELLOW,
    PUSH_BUTTON_BLUE,
    PUSH_BUTTON_PIO,
    PUSH_BUTTON_RECTANGLE_SMALL,
    PUSH_BUTTON_ROUND_BLACK,
    PUSH_BUTTON_ROUND_RED,
    PUSH_BUTTON_ROUND_RED_LARGE,
    PUSH_BUTTON_ROUND_GREEN_LARGE,
    ELF2K_LOAD_BUTTON,
    ELF2K_MP_BUTTON,
    ELF2K_RUN_BUTTON,
    ELF2K_IN_BUTTON,
    THUMB_MINUS_BUTTON,
    THUMB_PLUS_BUTTON,
    ROT_SWITCH_BUTTON,          // Rotating button with multiple positions
    ADI_SPINCTRL,
    ADI_VOLT_SPINCTRL,
    ADS_SPINCTRL,
    ADS_VOLT_SPINCTRL,
    THUMB_TEXT,
    PANEL_TEXT,
    PANEL_PNG,
    LED_SMALL_RED,
    LED_REAL_RED,
    LED_REAL_GREEN,
    LED_REAL_ORANGE,
    LED_LARGE_RED,
    LED_LARGE_GREEN,
    LED_LARGE_ORANGE,
    LED_LARGE_COLOR,
    LED_SMALL_RED_DISABLE,
    TIL_311,
    TIL_313,
    TIL_313_ITALIC,
};

enum
{
    FULL_TIL_313,
    FULL_TIL_313_ITALIC,
};

enum
{
    RUN_TYPE_DEFAULT,
    RUN_TYPE_BOOT,
    RUN_TYPE_RESET,
    RUN_TYPE_MONITOR,
    RUN_TYPE_SWITCH,
    RUN_TYPE_VELF,
    RUN_TYPE_UC1800,
    RUN_TYPE_MICROTUTOR,
};

enum
{
    STEP_TYPE_DEFAULT,
    STEP_TYPE_COSMICOS,
};

enum
{
    RESET_TYPE_DEFAULT,
    RESET_TYPE_FULL,
};

enum
{
    POWER_TYPE_DEFAULT,
    POWER_TYPE_KEEP,
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
    WRITE_ADDRESS_DEBUG,
};

enum
{
    FLIPFLOP_NONE,
    FLIPFLOP_IOGROUP,
    FLIPFLOP_VIS_MODE,
    FLIPFLOP_RS232_CTS,
    FLIPFLOP_VIS_PCB,
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

enum
{
    CART_NONE,
    CART_BIN,
    CART_ST2,
    CART_CH8,
};

enum
{
    // Terminal I/O
    VIDEO_TERMINAL_EF = 1,
    VIDEO_TERMINAL_EF_INTERRUPT,
    VIDEO_TERMINAL_OUT,
    VIDEO_TERMINAL_CYCLE,
    
    UART1854_LOAD_CONTROL_OUT,
    UART1854_LOAD_TRANSMITTER_OUT,
    UART1854_READ_RECEIVER_IN,
    UART1854_READ_STATUS_IN,

    UART16450_CONTROL_OUT,
    UART16450_WRITE_REGISTER_OUT,
    UART16450_READ_REGISTER_IN,
    UART16450_READ_STATUS_IN,
    
    EXTERNAL_VIDEO_TERMINAL_EF,
    EXTERNAL_VIDEO_TERMINAL_EF_INTERRUPT,
    EXTERNAL_VIDEO_TERMINAL_OUT,
    EXTERNAL_VIDEO_TERMINAL_CYCLE,
    
    EXTERNAL_UART1854_LOAD_CONTROL_OUT,
    EXTERNAL_UART1854_LOAD_TRANSMITTER_OUT,
    EXTERNAL_UART1854_READ_RECEIVER_IN,
    EXTERNAL_UART1854_READ_STATUS_IN,
    
    EXTERNAL_UART16450_CONTROL_OUT,
    EXTERNAL_UART16450_WRITE_REGISTER_OUT,
    EXTERNAL_UART16450_READ_REGISTER_IN,
    EXTERNAL_UART16450_READ_STATUS_IN,
    
    // Display I/O
    
    CDP1861_IN_FRAME_EF,
    CDP1861_DISABLE_OUT,
    CDP1861_ENABLE_IN,
    CDP1861_CYCLE,
    
    CDP1862_BACKGROUND_OUT,
    CDP1862_COLORRAM_OUT,
    CDP1862_CYCLE,
    
    CDP1864_TONE_LATCH_OUT,
    CDP1864_BACKGROUND_OUT,
    CDP1864_COLORRAM_OUT,
    CDP1864_DISABLE_IN,
    CDP1864_ENABLE_IN,
    CDP1864_CYCLE,
    
    VIS1870_EF,
    VIS1870_INT_RESET,
    VIS1870_OUT2,
    VIS1870_OUT3,
    VIS1870_OUT4,
    VIS1870_OUT5,
    VIS1870_OUT6,
    VIS1870_OUT7,
    VIS1870_CYCLE,
    
    VIS1870_TELMAC_DATA_OUT,
    VIS1870_TELMAC_REGISTER_OUT,
    VIS1870_BLINK_CYCLE,
    
    TMS_INTERRUPT_EF,
    TMS_REGISTER_PORT_OUT,
    TMS_DATA_PORT_OUT,
    TMS_CYCLE,
    
    MC6845_CYCLE,
    MC6845_BLINK_CYCLE,
    
    MC6845_EF,
    MC6847_OUT,
    MC6847_CYCLE,

    I8275_VERTICAL_EF,
    I8275_HORIZONTAL_EF,
    I8275_WRITE_COMMAND_OUT,
    I8275_WRITE_PARAMETER_OUT,
    I8275_READ_STATUS_IN,
    I8275_READ_PARAMETER_IN,
    I8275_CYCLE,
    
    STUDIOIV_VIDEO_OUT,
    STUDIOIV_VIDEO_DMA_ENABLE_OUT,
    STUDIOIV_VIDEO_CYCLE,
    
    VIP2K_VIDEO_DISABLE_OUT,
    VIP2K_VIDEO_ENABLE_IN,
    VIP2K_VIDEO_CYCLE,
    
    FRED_VIDEO_TYPE_OUT,
    FRED_VIDEO_CYCLE,
    
    COIN_VIDEO_ENABLE_OUT,
    COIN_VIDEO_CYCLE,
    
    SN76430N_CYCLE,
    
    // HEX & Led Display I/O
    HEX_DISPLAY_OUT,
    HEX_KEY_IN,
    HEX_KEY_EF,

    SWITCHES_IN_DIRECT,

    HEX_DISPLAY_COSMICOS_OUT,

    MULTI_TIL_DISPLAY_EF,
    MULTI_TIL_DISPLAY_OUT,
    MULTI_TIL_DISPLAY_IN,
    MULTI_TIL_DISPLAY_CYCLE,
    
    LED_CIDELSA_OUT,

    LED_CYCLE,
    
    // Keyboard I/O
    CDP1871_REPEAT_EF,
    CDP1871_EF,
    CDP1871_IN,
    CDP1871_CYCLE,

    ASCII_KEYBOARD_EF,
    ASCII_KEYBOARD_IN,
    
    PS2_KEYBOARD_EF,
    PS2_KEYBOARD_OUT,
    PS2_KEYBOARD_IN,
    PS2_KEYBOARD_CYCLE,
    
    PS2_GPIO_EF,
    PS2_GPIO_WRITE_CONTROL_OUT,
    PS2_GPIO_READ_DATA_IN,
    PS2_GPIO_CYCLE,
    
    ASCII_KEYBOARD_CYCLE,
    
    LATCH_KEYBOARD_EF,
    LATCH_KEYPAD_EF1, // This needs to follow LATCH_KEYBOARD_EF directly!
    LATCH_KEYPAD_EF2,

    LATCH_KEYBOARD_OUT,
    LATCH_KEYPAD_OUT1, // This needs to follow LATCH_KEYBOARD_OUT directly!
    LATCH_KEYPAD_OUT2,
    
    LATCH_KEYPAD_DOUBLE_OUT,
    LATCH_KEYPAD_CYCLE,

    MATRIX_KEYBOARD_OUT,
    MATRIX_KEYBOARD_IN,
    
    KEY_TAPE_FRED_EF,
    KEY_FRED_MODE_OUT,
    KEY_TAPE_FRED_READ_DATA_IN,
    KEY_FRED_CYCLE,

    KEY_FRED_COIN_ARCADE_EF,
    KEY_FRED_COIN_ARCADE_IN,
    
    CV_KEYPAD_DATA_READY_EF,
    CV_KEYPAD_READ_DATA_IN,
    
    ETI_KEYPAD_EF,
    ETI_KEYPAD_OUT,
    ETI_KEYPAD_IN,
    
    BIT_KEYPAD_IN_0,
    BIT_KEYPAD_IN_1,
    BIT_KEYPAD_IN_2,
    BIT_KEYPAD_IN_3,
    BIT_KEYPAD_IN_4,
    BIT_KEYPAD_IN_5,
    BIT_KEYPAD_IN_6,
    BIT_KEYPAD_CYCLE,
    
    // Disk I/O
    FDC1770_EF,
    FDC1770_SELECT_OUT,
    FDC1770_WRITE_OUT,
    FDC1770_READ_IN,
    FDC1770_INTRQ_IN,
    FDC_CYCLE,
    
    FDC1793_EF,
    FDC1793_SELECT_OUT,
    FDC1793_WRITE_OUT,
    FDC1793_READ_IN,
    
    IDE_SELECT_PORT_OUT,
    IDE_WRITE_PORT_OUT,
    IDE_READ_PORT_IN,
    IDE_READ_STATUS_IN,
    IDE_CYCLE,

    UPD765_EF,
    UPD765_DMA_CONTROL_OUT,
    UPD765_DMA_COUNT_OUT,
    UPD765_WRITE_COMMAND_OUT,
    UPD765_READ_COMMAND_IN,
    UPD765_READ_STATUS_IN,
    UPD765_CYCLE,
    
    // Sound I/O
    SOUND_SUPER_VP550_CYCLE,
    
    BIT_SOUND_OUT,
    
    CDP1863_TONE_LATCH_OUT,
    CDP1863_TONE_SWITCH_OUT1,
    CDP1863_TONE_SWITCH_OUT2,
    
    // Printer I/O
    SERIAL_PRINTER_OUT,
    SERIAL_PRINTER_IN,
    
    PARALLEL_PRINTER_OUT,
    PARALLEL_PRINTER_IN,
    
    THERMAL_PRINTER_EF,
    THERMAL_PRINTER_OUT,
    THERMAL_PRINTER_IN,
    THERMAL_PRINTER_CYCLE,
    
    CENTRONICS_PRINTER_OUT,
    CENTRONICS_PRINTER_EF,

    BASIC_PRINTER_EF,
    BASIC_PRINTER_OUT,
    
    BASIC_Q_PRINTER_EF,

    // Memory I/O
    EMS_PAGE_SELECT_OUT,

    MEMORY_SLOT_OUT,
    
    NVRAM_MEMORY_PROTECT_IN,
    
    RTC_CYCLE,
    RTC_EF,
    RTC_SELECT_PORT_OUT,
    RTC_WRITE_PORT_OUT,
    RTC_READ_PORT_IN,

    // Tape I/O
    TAPE_EF,
    
    TAPE_RUNNING_EF,
    TAPE_ON_OFF_OUT,
    TAPE_MICRO_OUT,
    TAPE_Q_OUT,
    
    // KEY_TAPE_FRED_EF,
    TAPE_ERROR_FRED_EF,
    TAPE_CONTROL_FRED_EF,
    TAPE_MODE_FRED_OUT,
    TAPE_SOUND_FRED_OUT,
    // KEY_TAPE_FRED_READ_DATA_IN,
    
    TAPE_CV_DATA_READY_EF,
    TAPE_CV_BUFFER_EMPTY_EF,
    TAPE_CV_WRITE_DATA_OUT,
    TAPE_CV_READ_DATA_IN,

    // MDU I/O
    MDU_X,
    MDU_Y,
    MDU_Z,
    MDU_EF,
    MDU_STATUS,
    MDU_CONTROL,
    MDU_CYCLE,

    // Other I/O
    UNDEFINED_EF1,
    UNDEFINED_EF2,
    UNDEFINED_EF3,
    UNDEFINED_EF4,
    
    COMX_DIAGNOSTIC_BOARD_IN1,
    COMX_DIAGNOSTIC_BOARD_IN2,
    COMX_DIAGNOSTIC_BOARD_OUT,
    
    SUPERBOARD_EF,
    SUPERBOARD_UART_OUT,
    SUPERBOARD_UART_IN,
    SUPERBOARD_RESET_EF_IN,
    
    PORT_EXTENDER_SELECT_OUT,
    PORT_EXTENDER_WRITE_OUT,
    PORT_EXTENDER_IN,

    NETWORK_CYCLE,
    
    DIP_IN,
    
    DRAM_DMA_CYCLE,
    
    INTERRUPT_CLOCK_CYCLE,
    
    AD_CONVERTOR_OUT,
    AD_CONVERTOR_IN,
    
    NANO_MONITOR_EF,
    
    CDP1851_A_EF,
    CDP1851_B_EF,
    CDP1851_WRITE_A_OUT,
    CDP1851_WRITE_B_OUT,
    CDP1851_READ_A_OUT,
    CDP1851_READ_B_OUT,
    CDP1851_WRITE_CONTROL_OUT,
    CDP1851_READ_STATUS_IN,

    CDP1852_EF,
    CDP1852_WRITE_OUT,
    CDP1852_READ_IN,

    CD4536B_EF,
    CD4536B_WRITE_OUT,
    CD4536B_CYCLE,
        
    FLIPFLOP_OUT,
    FLIPFLOP_EF,

    LAST_IO_DEFINITION
};

enum
{
    CYCLE_TYPE_COMPUTER,
    CYCLE_TYPE_VIDEO_VIS1870,
    CYCLE_TYPE_VIDEO_CDP1861,
    CYCLE_TYPE_VIDEO_I8275,
    CYCLE_TYPE_VIDEO_MC6845,
    CYCLE_TYPE_VIDEO_MC6847,
    CYCLE_TYPE_VIDEO_TMS9918,
    CYCLE_TYPE_VIDEO_CDP1864,
    CYCLE_TYPE_VIDEO_SN76430N,
    CYCLE_TYPE_VIDEO_VIP2K,
    CYCLE_TYPE_VIDEO_FRED,
    CYCLE_TYPE_VIDEO_ST4,
    CYCLE_TYPE_VIDEO_COIN,
    CYCLE_TYPE_BLINK_MC6845,
    CYCLE_TYPE_BLINK_VIS1870,
    CYCLE_TYPE_SEVEN_SEGMENT,
    CYCLE_TYPE_PRINT,
    CYCLE_TYPE_DISK_FDC,
    CYCLE_TYPE_DISK_IDE,
    CYCLE_TYPE_VIDEO_TERMINAL,
    CYCLE_TYPE_EXTERNAL_VIDEO_TERMINAL,
    CYCLE_TYPE_KEYBOARD,
    CYCLE_TYPE_DRAM_DMA,
    CYCLE_TYPE_INTERRUPT_CLOCK,
    CYCLE_TYPE_SOUND,
    CYCLE_TYPE_CD3536B,
    CYCLE_TYPE_RTC,
    CYCLE_TYPE_MDU,
    CYCLE_TYPE_MAX,
};

enum
{
    INTERRUPT_TYPE_VIS,
    INTERRUPT_TYPE_KEYBOARD,
    INTERRUPT_TYPE_I8275_1,
    INTERRUPT_TYPE_I8275_4,
    INTERRUPT_TYPE_UART,
    INTERRUPT_TYPE_RTC,
    INTERRUPT_TYPE_INPUT,
    INTERRUPT_TYPE_MAX
};

#define READ_FUNCTION_NONE 0
#define READ_FUNCTION_FREEZE_PIC_VECTOR 1
#define READ_FUNCTION_NOT_DEBUG 2

static wxString interruptTypeList_[]=
{
    "VIS",
    "Keyboard",
    "i8275 frame",
    "i8275 row",
    "UART",
    "RTC",
    "INPUT",
};

static wxString commandComputerList_[]=
{
    "evaluationkit",
    "CDP18S020",
    "coin",
    "CoinArcade",
    "comx35",
    "Comx",
    "comix35",
    "Comix",
    "cosmac",
    "CosmacElf",
    "gamesystem",
    "CosmacGameSystem",
    "cosmacgame",
    "CosmacGameSystem",
    "cv",
    "Cybervision",
    "cyber",
    "Cybervision",
    "elf2000",
    "Elf2K",
    "eti660",
    "Eti",
    "eti-660",
    "Eti",
    "fred",
    "FRED1",
    "fred15",
    "FRED1_5",
    "fred1.5",
    "FRED1_5",
    "hec",
    "HEC1802",
    "hug",
    "HUG1802",
    "infinite", 
    "UC1800",
    "member",
    "Membership",
    "microtutori",
    "Microtutor",
    "microtutor1",
    "Microtutor",
    "microtutor2",
    "MicrotutorII", 
    "oscom", 
    "Nano",
    "oscomnano", 
    "Nano",
    "netronics", 
    "NetronicsElfII",
    "elfii", 
    "NetronicsElfII",
    "elf2", 
    "NetronicsElfII",
    "pecom",
    "Pecom64",
    "pico", 
    "PicoElfV2",
    "quest", 
    "QuestSuperElf",
    "studio", 
    "StudioII",
    "studio2", 
    "StudioII", 
    "studio3",
    "StudioIII", 
    "studio4", 
    "StudioIV", 
    "telmac600",
    "TMC600",
    "telmac1800",
    "TMC1800",
    "telmac2000", 
    "TMC2000",
    "vip2",
    "VipII",
    "victory",
    "Conic",
    "",
    ""
};

static wxString defaultComputerList_[]=
{
    "CDP18S020",
    "CDP18S020 Evaluation Kit",
    "tiny basic ram.xml",
    "Cidelsa",
    "Cidelsa Arcade Game Console",
    "altair.xml",
    "CoinArcade",
    "RCA Video Coin Arcade Game Console",
    "bare.xml",
    "Comix",
    "COMIX-35",
    "pal_high_speed_direct_connect.xml",
    "Comx",
    "COMX-35",
    "superboard.xml",
    "Conic",
    "Conic (Apollo, Mustang, MPT-02 and M1200)",
    "soundic_victory_mpt-02.xml",
    "CosmacElf",
    "COSMAC Elf",
    "bare.xml",
    "CosmacGameSystem",
    "RCA COSMAC Computer Game System",
    "bare.xml",
    "Cosmicos",
    "Cosmicos - COSmac MIcro COmputer System",
    "ut4-monitor.xml",
    "Cybervision",
    "Cybervision 2001",
    "escape.xml",
    "Elf",
    "Elf",
    "elfos-serial.xml",
    "Elf2K",
    "COSMAC Elf 2000",
    "i8275.xml",
    "Eti",
    "ETI-660",
    "wipeout.xml",
    "FRED1",
    "FRED 1",
    "animate demo.xml",
    "FRED1_5",
    "FRED 1.5",
    "bare.xml",
    "HEC1802",
    "HEC1802",
    "basic.xml",
    "HUG1802",
    "HUG1802",
    "basic.xml",
    "JVIP",
    "JVIP",
    "bare.xml",
    "Macbug",
    "Macbug",
    "bare.xml",
    "MCDS",
    "RCA Microboard Computer Development System",
    "cdp18s695.xml",
    "Membership",
    "Membership Card",
    "monitor-j-basic3-rom-8000.xml",
    "Microboard",
    "RCA COSMAC Microboard Computer",
    "cdp18s600.xml",
    "Microtutor",
    "COSMAC Microtutor I",
    "bare.xml",
    "MicrotutorII",
    "COSMAC Microtutor II",
    "tops.xml",
    "MS2000",
    "RCA MicroDisk Development System MS2000",
    "hd.xml",
    "Nano",
    "Oscom Nano",
    "chip8.xml",
    "NetronicsElfII",
    "Netronics Elf II",
    "tinybasic-serial.xml",
    "Pecom32",
    "PECOM 32",
    "bare.xml",
    "Pecom64",
    "PECOM 64",
    "bare.xml",
    "PicoElfV2",
    "Pico/Elf V2",
    "elfos-uart.xml",
    "QuestSuperElf",
    "Quest Super Elf",
    "bare.xml",
    "Studio2020",
    "Studio 2020",
    "pal.xml",
    "StudioII",
    "RCA Studio II",
    "standard.xml",
    "StudioIII",
    "RCA Studio III",
    "standard-ntsc.xml",
    "StudioIV",
    "RCA Studio IV",
    "ntsc-basic-32k-2020.xml",
    "TMC600",
    "Telmac TMC-600",
    "exp-151182.xml",
    "TMC1800",
    "Telmac 1800",
    "chip8.xml",
    "TMC2000",
    "Telmac 2000",
    "chip8.xml",
    "UC1800",
    "Infinite UC1800",
    "keybug2020.xml",
    "Velf",
    "VELF",
    "bare.xml",
    "Vip",
    "COSMAC VIP",
    "fpb-vp590.xml",
    "Vip2K",
    "VIP2K Membership Card",
    "monitor-1.5,ntsc.xml",
    "VIS1802",
    "VIS1802",
    "vis1802.xml",
    "VT1802",
    "VT1802",
    "vt1802-14Mhz-8pixels.xml",
    "VipII",
    "COSMAC VIP II",
    "ed.xml",
    "Visicom",
    "Visicom COM-100",
    "standard.xml",
    "",
    "",
    "",
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
    wxSize buttonSize_;
    wxCoord buttonStartX_;
    wxCoord buttonStartY_;
    int type_;
};

class RotButton
{
public:
    RotButton(wxDC& dc, int state, wxCoord x, wxCoord y, int type);
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
    wxSize buttonSize_;
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

#endif // DEFINITION_H
