#define COMXEF2 1
#define PIXIEEF 2
#define CIDELSAEF2 3
#define V1870EF 4
#define KEYBRDEF 5 
#define PS2EF 6
#define STUDIOEF3 7
#define FDCEF 8
#define VT100EF 9
#define COMXIN 10
#define COMXOUT 11
#define V1870OUT3 12
#define V1870OUT4 13
#define V1870OUT5 14
#define V1870OUT6 15
#define V1870OUT7 16
#define PIXIEOUT 17
#define PIXIEIN 18
#define STUDIOOUT 19
#define CIDELSAIN1 20
#define CIDELSAIN2 21
#define CIDELSAIN4 22
#define ELF2OUT 23
#define ELF2IN 24
#define FDCIN 25
#define FDCSELECTOUT 26
#define FDCWRITEOUT 27
#define ELFOUT 28
#define ELFIN 29
#define IDEIN 30
#define IDESELECTOUT 31
#define IDEWRITEOUT 32
#define KEYBRDIN 33
#define LEDMODOUT 34
#define PORTEXTIN 35
#define PORTEXTSELECTOUT 36
#define PORTEXTWRITEOUT 37
#define PRINTEROUT 38
#define PS2OUT 39
#define PS2IN 40
#define SUPEROUT 41
#define SUPERIN 42
#define TMSHIGHOUT 43
#define TMSLOWOUT 44
#define VT100OUT 45
#define ELFINEF 46
#define VTINEF 47

#define COMXCYCLE 49
#define V1870CYCLE 50
#define FDCCYCLE 51
#define IDECYCLE 52
#define KEYBRDCYCLE 53
#define PIXIECYCLE 54
#define PS2CYCLE 55
#define VT100CYCLE 56
#define COMXINTEST 57
#define COMXEXPOUT 58
#define COMXEXPIN2 59
#define COMXEXPIN4 60
#define MC6845CYCLE 61
#define THERMALCYCLE 62
#define COMXEF3 63
#define COMXEF4 64
#define CIDELSAEF4 65
#define STUDIOEF4 66
#define CIDELSAEF3 67
#define CIDELSAOUT1 68
#define TELMACIN 69
#define TELMACOUTDATA 70
#define TELMACOUTREGISTER 71
#define TELMACEF3 72
#define TELMACOUTKEY 73
#define V1870BLINK 74
#define TELMACEF2 75
#define TELMAPRINTER 76
#define TELMACCYCLE 77
#define VIPKEYOUT 78
#define VIPKEYEF 79
#define VIPEF2 80
#define VIPOUT4 81
#define PIXIEBACKGROUND 82
#define NANOEF4 83
#define ELF2EF3 84
#define SUPEREF3 85
#define ELFEF3 86
#define VIPOUT3 87
#define VIPOUT5 88
#define VIPKEYEF4 89
#define TMSCYCLE 90
#define MC6845BLINK 91
#define PECOMBANK 92
#define PECOMKEY 93
#define PECOMCYCLE 94
#define PECOMEF1 95
#define PECOMEF2 96
#define PECOMEF3 97
#define PECOMEF4 98
#define MC6847CYCLE 99
#define MC6847OUT 100
#define MC6847EF 101
#define I8275CYCLE 102
#define I8275CREGWRITE 103
#define I8275SREGREAD 104
#define I8275PREGWRITE 105
#define I8275PREGREAD 106
#define I8275EF 107
#define PS2GPIOIN 108
#define PS2GPIOEF 109
#define PS2GPIOCYCLE 110
#define ELF2KOUT 111
#define ELF2KIN 112
#define ELF2KEF 113
#define ELF2KEF3 114
#define ELF2KDISKREADSTATUS 115
#define ELF2KDISKSELECTREGISTER 116
#define ELF2KDISKREADREGISTER 117
#define ELF2KDISKWRITEREGISTER 118
#define ELF2KDISKCYCLE 119
#define ELF2KCYCLE 120
#define ELF2KGPIO 121
#define ETIPIAOUT 122
#define ETIPIAIN 123
#define ETICOLOURRAM 124
#define ELF2EF2 125
#define UARTIN 126
#define UARTOUT 127
#define UARTSTATUS 128
#define UARTCONTROL 129
#define VP550CYCLE 130
#define RCADISKIN 131
#define RCADISKOUT 132
#define COSMICOSOUT 133
#define COSMICOSIN 134
#define COSMICOSEF 135
#define COSMICOSHEX 136
#define COSMICOSREQ 137
#define COSMICOSDEC 138
#define COSMICOSRET 139
#define COSMICOS7SEG 140
#define COSMICOSTONE 141
#define VIPIIKEYCYCLE 142
#define VIPIIOUT7 143
#define COMXEXPIN5 144
#define COMXRTCIN1 145
#define COMXEXPIN6 146
#define EF1UNDEFINED 147
#define EF2UNDEFINED 148
#define EF3UNDEFINED 149
#define ROMMAPPEROUT 150
#define EMSMAPPEROUT 151
#define ELFPRINTEREF 152
#define INPUTUNDEFINED 153
#define MS2000IOGROUP 154
#define MS2000IO2 155
#define MS2000IO3 156
#define MS2000IO4 157
#define MS2000IO5 158
#define MS2000IO6 159
#define MS2000IO7 160
#define MS2000EF 161
#define MS2000CASEF 162
#define MS2000PRINTEF 163
#define MICROTUTORIN 164
#define MICROTUTOROUT 165
#define MICROTUTOREF 166
#define COMXDIAGIN1 167
#define COMXDIAGIN2 168
#define COMXDIAGOUT1 169
#define LEDCYCLE 170
#define VTSERIALEF 171
#define VTSERIALCYCLE 172

#define COMPUTERCYCLE 0
#define VIDEOCYCLE 1
#define BLINKCYCLE 2
#define PRINTCYCLE 3
#define DISKCYCLEFDC 4
#define DISKCYCLEIDE 5
#define VTCYCLE 6
#define VIDEOCYCLE1870 7
#define KEYCYCLE 8
#define MAXCYCLE 9

#ifndef IODEVICE_H
#define IODEVICE_H

class IoDevice 
{
public:
	IoDevice();
	~IoDevice() {};

	void initIo();
	virtual Byte ef(int flag) = 0;
	virtual Byte in(Byte port, Word address) = 0;
	virtual void out(Byte port, Word address, Byte value) = 0;
	virtual void cycle(int type) = 0;

	void setEfType(int number, int efType) {efType_[number] = efType;};
	int getEfType(int number) {return efType_[number];};
	void setInType(int number, int inType) {inType_[number] = inType;};
	int getInType(int number) {return inType_[number];};
	void setOutType(int number, int outType) {outType_[number] = outType;};
	void setCycleType(int number, int outCycleType) {cycleType_[number] = outCycleType;};

protected:
	int efType_[5];
	int inType_[8];
	int outType_[8];
	int cycleType_[MAXCYCLE];

private:

};

#endif  // IODEVICE_H
