#ifndef ELFCONFIG_H
#define ELFCONFIG_H

class ElfPortConfiguration
{
public:
	int ef1default;
	int ef2default;
	int ef3default;
    
    int pixieInput;
    int pixieOutput;
    int pixieEf;

    int portExtenderSelectOutput;
    int portExtenderWriteOutput;
    int portExtenderInput;

    int ideSelectOutput;
    int ideWriteOutput;
    int ideStatus;
    int ideInput;
    int ideTracks;
    int ideHeads;
    int ideSectors;

    int fdcSelectOutput;
    int fdcWriteOutput;
    int fdcInput;
    int fdcEf;
    
    int keyboardInput;
    int keyboardEf;
      
    int ps2KeyboardInput;
    int ps2KeyboardOutput;
    int ps2KeyboardEf;
    
    int printerOutput;
    int printerEf;

    int emsOutput;

    int vt100Output;
    int vt100Ef;
    int vt100ReverseEf;
    int vt100ReverseQ;

    int uartOut;
    int uartIn;
    int uartControl;
    int uartStatus;

    int tmsModeHighOutput;
    int tmsModeLowOutput;

    int i8275WriteCommand;
    int i8275ReadStatus;
    int i8275WriteParameter;
    int i8275ReadParameter;
    int i8275VerticalRetrace;
      
    int led_Module_Output;

    int mc6847Output;
    int mc6847b7;
    int mc6847b6;
    int mc6847b5;
    int mc6847b4;
    int mc6847b3;
    int mc6847b2;
    int mc6847b1;
    int mc6847b0;
    int mc6847dd7;
    int mc6847dd6;

    int mc6847StartRam;
    int mc6847EndRam;

    int mc6845StartRam;
    int mc6845EndRam;
    int mc6845Address;
    int mc6845Data;
    int mc6845Ef;

    int hexOutput;
    int hexInput;
    int hexEf;
    
    int tapeEf;
};

class ElfConfiguration
{
public:
	bool usePixie;
	bool useS100;
	bool use6845;
	bool use6847;
	int charLine;
	int screenHeight6847;
	bool useTMS9918;
	bool use8275;
	int vtType;
    bool vtExternal;
    bool serialLog;
	int bellFrequency_;
	wxString vtWavFileDir_;
    wxString vtWavFile_;
    wxString serialPort_;
	bool useUart;
	int baudR;
	int baudT;
    bool autoBoot;
    bool stopTone;
    bool tapeStart;
	bool rtc;
	bool nvr;
	bool useSwitch;
	bool useHex;
	bool useTape;

	int keyboardType;
	bool useHexKeyboard;
	bool useHexKeyboardEf3;
	bool useKeyboard;
	bool UsePS2;
	bool ps2Interrupt;	
	bool usePs2gpio;

	int diskType;
    bool ideEnabled;
    bool fdcEnabled;

	int memoryType;
	bool usePortExtender;
	bool usePager;
    bool useEms;
    bool useRomMapper;
	bool useLedModule;
	bool useElfControlWindows;
	bool showAddress;

    bool bootStrap;
    bool bootRam;
	bool clearRam;
	bool clearRtc;

    int ioType;
    int frontType;

    bool giantBoardMapping;
    
    bool forceUpperCase;
	int qSound_;
    int tilType;

	bool vtEf;
	bool vtQ;
	bitset<32> vt52SetUpFeature_;
    bitset<32> vt100SetUpFeature_;
    bitset<32> vtExternalSetUpFeature_;

    int tapeFormat_;
	bool coinArcadeControl_;

	ElfPortConfiguration elfPortConf;
};

#endif	// ELFCONFIG_H
