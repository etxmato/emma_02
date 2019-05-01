/*
 *******************************************************************
 *** This software is copyright 2008 by Marcel van Tongeren      ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 ***                                                             ***
 *** 1802 Code based on elf emulator by Michael H Riley with     ***
 *** copyright as below                                          ***
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

#include "main.h"
#include "tmc600.h"

#define SHIFT 1

int defaultInputPortFile[] =
{
	8, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 7, 3, 3,
	4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 8, 8, 8, 8, 8, //16
    6, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, //32
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 8, 8, 8, 8, 8, 8, //48
	8, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, //64
	4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 8, 8, 8, 8, 8, //80
	8, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, //96
	4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 8, 8, 8, 8, 8, //112
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, //128
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, //144
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, //160
	8, 8, 8, 8, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, //176
	8, 8, 8, 8, 5, 5, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, //192
	8, 8, 8, 8, 8, 8, 5, 8, 8, 8, 8, 8, 8, 8, 8, 8, //208
	8, 8, 8, 8, 5, 5, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, //224
	8, 8, 8, 8, 8, 8, 5, 8, 8, 8, 8, 8, 8, 8, 8, 7  //240
};

int defaultBitNumberFile[] =
{
	0, 2, 4, 8, 16, 32, 64, 128, 1, 2, 4, 8, 16, 32, 64, 128,
	1, 2, 4, 8, 16, 32, 64, 128, 1, 2, 4, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 2, 4, 8, 16, 32, 64, 128, 1, 2, 0, 0, 0, 0, 0, 0,
	0, 2, 4, 8, 16, 32, 64, 128, 1, 2, 4, 8, 16, 32, 64, 128,
	1, 2, 4, 8, 16, 32, 64, 128, 1, 2, 4, 0, 0, 0, 0, 0,
	0, 2, 4, 8, 16, 32, 64, 128, 1, 2, 4, 8, 16, 32, 64, 128,
	1, 2, 4, 8, 16, 32, 64, 128, 1, 2, 4, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 16, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 16, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 1
};

int specialKey[] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, 0, SHIFT, SHIFT, SHIFT, SHIFT, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, SHIFT, SHIFT, SHIFT, SHIFT,
	SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT,
	SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, 0, SHIFT, 0, SHIFT, SHIFT,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, SHIFT, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, SHIFT, SHIFT, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, SHIFT, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

Tmc600::Tmc600(const wxString& title, const wxPoint& pos, const wxSize& size, double zoomLevel, int computerType, double clock)
:V1870(title, pos, size, zoomLevel, computerType, clock)
{
	p_Printer = new Printer();
	p_Printer->initTelmac(p_Printer);

	clockSize_ = (int) (((clock * 1000000) / 8)/3.13);
    
    reDefineKeys();
}

Tmc600::~Tmc600()
{
	p_Printer->closeFrames();
	delete p_Printer;
	p_Main->setMainPos(TMC600, GetPosition());
#ifdef __WXMSW__
	if (!capsPressedOnStart_ && wxGetKeyState(WXK_CAPITAL))
	{
		::keybd_event( VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY, 0 );
		::keybd_event( VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 1 );
	}
#endif
}

void Tmc600::reDefineKeys()
{
    for (int i=0; i<256; i++)
    {
        inputPortFile[i] = defaultInputPortFile[i];
        bitNumberFile[i] = defaultBitNumberFile[i];
    }
    p_Main->getTmc600Keys(keyDefNormal_, keyDefShift_);

    int offsetNumber = 0;
    for (int i=0; i<=20; i++)
    {
        if (i >= 17)
            offsetNumber = 26;
        
        if (keyDefNormal_[i] != 0)
        {
            inputPortFile[keyDefNormal_[i]]=((i+offsetNumber)>>3)&0x7;
            bitNumberFile[keyDefNormal_[i]]=1<<((i+offsetNumber)&0x7);
        }
    }
    
    offsetNumber = 0;
    for (int i=0; i<=20; i++)
    {
        if (i >= 17)
            offsetNumber = 26;
        
        if (keyDefShift_[i] != 0)
        {
            inputPortFile[keyDefShift_[i]]=((i+offsetNumber)>>3)&0x7;
            bitNumberFile[keyDefShift_[i]]=1<<((i+offsetNumber)&0x7);
        }
    }
}

void Tmc600::charEvent(int keycode)
{
	if (keyboardCode_ != 0)
	{
		nextKeyboardCode_ = keycode;
		return;
	}

    if (keycode == 8364)
        keycode = 36;
    
	keyDown_ = true;
	cycleValue_ = -1;
	keyboardCode_ = keycode;
	pressKey();
}

bool Tmc600::keyDownExtended(int keycode, wxKeyEvent& event)
{
	int modifier = event.GetModifiers();

	switch (modifier)
	{
        case wxMOD_SHIFT:
            shiftPressed_ = true;
            keyValue_[6] |= 0x80;
        break;
            
        case wxMOD_CONTROL:
            ctrlPressed_ = true;
            if (ctrlAltLeft_)
                keyValue_[6] |= 16;
            else
                keyValue_[6] |= 32;
        break;
            
		case wxMOD_ALT:
			altPressed_ = true;
			if (ctrlAltLeft_)
				keyValue_[6] |= 64;
			else
				keyValue_[6] |= 8;
		break;

	}

	switch (keycode)
	{
		case WXK_CAPITAL:
			switchCaps();
			return true;
		break;

		case WXK_TAB:
			ctrlAltLeft_ = !ctrlAltLeft_;
			return true;
		break;

        case WXK_ESCAPE:
			keyValue_[5] |= 128;
			return true;
		break;

		case WXK_BACK:
			keyValue_[6] |= 2;
			return true;
		break;

        case WXK_END:
			keyValue_[6] |= 4;
			return true;
		break;

		case WXK_HOME:
			keyValue_[7] |= 4;
			return true;
		break;

		case WXK_UP:
			keyValue_[7] |= 8;
			return true;
		break;

		case WXK_RIGHT:
			keyValue_[7] |= 16;
			return true;
		break;

		case WXK_DOWN:
			keyValue_[7] |= 64;
			return true;
		break;

		case WXK_LEFT:
			keyValue_[7] |= 128;
			return true;
		break;
	}
	return false;
}

void Tmc600::keyUpExtended(int keycode, wxKeyEvent& event)
{
	if (ctrlPressed_ || altPressed_ || shiftPressed_)
	{
		int modifier = event.GetModifiers();

        if (shiftPressed_ && (modifier != wxMOD_SHIFT))
            keyValue_[6] &= 0x7f;
        
		if (ctrlPressed_ && (modifier != wxMOD_CONTROL))
		{
			ctrlPressed_ = false;
			if (ctrlAltLeft_)
				keyValue_[6] &= 0xef;
			else
				keyValue_[6] &= 0xdf;
		}

		if (altPressed_ && (modifier != wxMOD_ALT))
		{
			altPressed_ = false;
			if (ctrlAltLeft_)
				keyValue_[6] &= 0xbf;
			else
				keyValue_[6] &= 0xf7;
		}
	}

	switch (keycode)
	{
        case WXK_ESCAPE:
			keyValue_[5] &= 0x7f;
			return;
		break;

		case WXK_BACK:
			keyValue_[6] &= 0xfd;
			return;
		break;

        case WXK_END:
			keyValue_[6] &= 0xfb;
			return;
		break;

		case WXK_HOME:
			keyValue_[7] &= 0xfb;
			return;
		break;

		case WXK_UP:
			keyValue_[7] &= 0xf7;
			return;
		break;

		case WXK_RIGHT:
			keyValue_[7] &= 0xef;
			return;
		break;

		case WXK_DOWN:
			keyValue_[7] &= 0xbf;
			return;
		break;

		case WXK_LEFT:
			keyValue_[7] &= 0x7f;
			return;
		break;

	}
	releaseKey();
}

void Tmc600::keyDownFile()
{
	if (keyboardCode_ == 255)
	{
        keyValue_[7] |= 1;
        capsPressed_ = !capsPressed_;
	}
    else
        pressKey();
}

void Tmc600::keyUpFile()
{
	releaseKey();
}

void Tmc600::pressKey()
{
    keyValue_[inputPortFile[keyboardCode_]] |= (bitNumberFile[keyboardCode_]&0xff);
}

void Tmc600::releaseKey()
{
	keyDown_ = false;
	keyValue_[inputPortFile[keyboardCode_]]  &= ((bitNumberFile[keyboardCode_]&0xff) ^ 0xff);

	if (nextKeyboardCode_ != 0)
	{
		keyDown_ = true;
		cycleValue_ = 50000;
		keyboardCode_ = nextKeyboardCode_;
		pressKey();
		nextKeyboardCode_ = 0;
	}
	else
		keyboardCode_ = 0;
}

void Tmc600::onRun()
{
	if (getClear()==0)
	{
		setClear(1);
		p_Main->eventUpdateTitle();
		p_Main->startTime();
	}
	else
	{
		setClear(0);
		p_Main->eventUpdateTitle();
	}
}

void Tmc600::configureComputer()
{
	outType_[3] = TELMACOUTKEY;
	outType_[4] = TELMAPRINTER;

	efType_[3] = TELMACEF3;
	efType_[2] = TELMACEF2;
	cycleType_[COMPUTERCYCLE] = TELMACCYCLE;

	p_Main->message("Configuring Telmac TMC-600");
	p_Main->message("	Output 3: keyboard latch, output 4: printer data");
	p_Main->message("	EF 2: cassette in, EF 3: keyboard\n");

	if (p_Main->getRealTimeClock())
		clockValue_ = clockSize_;
	else 
		clockValue_ = -1;

	resetCpu();
}

void Tmc600::initComputer()
{
	init1870();
	setClear(1);
	setWait(1);
	cassetteEf_ = 0;
	keyLatch_ = 0;
	altPressed_ = false;
    ctrlPressed_ = false;
    shiftPressed_ = false;
	ctrlAltLeft_ = true;
	capsCorrection_ = false;
	keyboardCode_ = 0;
	nextKeyboardCode_ = 0;
	keyDown_ = false;
	cycleValue_ = 1;

	for (int i=0; i<9; i++)
		keyValue_[i]=0;

	for (int i=0; i<16; i++)
	{
		adiArray_[i]=0;
		adsArray_[i]=0;
	}
	adiChannel_ = 0;
	adiValue_ = 0;


	if (!wxGetKeyState (WXK_CAPITAL))
	{
		capsPressed_ = false;
		capsPressedOnStart_ = false;
#ifdef __WXMSW__
		::keybd_event( VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY, 0 );
		::keybd_event( VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0 );
#endif
	}
	else
	{
		capsPressed_ = true;
		capsPressedOnStart_ = false;
	}

    telmacRunCommand_ = 0;
	telmacRunState_ = RESETSTATE;

	output3Function_ = TELMACSELECT;
}

Byte Tmc600::ef(int flag)
{
	int ef3, port;

	switch(efType_[flag])
	{
		case 0:
			return 1;
		break;

		case V1870EF:
			return ef1_1870();
		break;

		case TELMACEF3:
			port = (int) (keyLatch_ / 8);
			ef3 = (~keyValue_[port] & (1 << (keyLatch_ % 8))) ? 1 : 0;
            return ef3;
		break;

		case TELMACEF2:
			return cassetteEf_;
		break;

		default:
			return 1;
	}
}

Byte Tmc600::in(Byte port, Word WXUNUSED(address))
{
	Byte ret;

//	p_Main->messageInt(port);
	switch(inType_[port])
	{
		case 0:
			ret = 255;
		break;

		case TELMACIN:
			ret = 0;
			switch (inputNumber_)
			{
				case 1:
					adiValue_ = adiArray_[adiChannel_];
				break;
				case 2:
					if (adiValue_ < 0)
						ret = 10;
					else
						ret = int(adiValue_/100) & 0xf;
				break;
				case 4:
					if (adiValue_ < 0)
						ret = int(-adiValue_-int(-adiValue_/10)*10) & 0xf;
					else
						ret = int(adiValue_-int(adiValue_/10)*10) & 0xf;
				break;
				case 6:
					if (adiValue_ < 0)
						ret = int( (-adiValue_-int(-adiValue_/100)*100) / 10) & 0xf;
					else
						ret = int( (adiValue_-int(adiValue_/100)*100) / 10) & 0xf;
				break;
			}
			if (adiValue_ < -99)
				ret = 10;
			if (adiValue_ > 999)
				ret = 11;
			inputNumber_++;
		break;

		default:
			ret = 255;
	}
	inValues_[port] = ret;
	return ret;
}

void Tmc600::out(Byte port, Word address, Byte value)
{
	outValues_[port] = value;
	switch(outType_[port])
	{
		case 0:
			return;
		break;

		case TELMACOUTKEY:
			keyLatch_ = value;
		break;

		case TELMAPRINTER:
			switch (output3Function_)
			{
				case TELMACSELECT:
					if (value < 15)
					{
						output3Function_ = TELMACSELECT;
						inputNumber_ = 1;
						adiChannel_ = value;
					}
					if (value == 15)
						output3Function_ = TELMACPRINT;
				break;

				case TELMACPRINT:
					output3Function_ = TELMACSELECT;
//					p_Main->eventPrintDefault(value);
					p_Printer->printerOut(value);
				break;
			}
		break;

		case TELMACOUTREGISTER:
			vismacRegisterLatch = value;
		break;

		case TELMACOUTDATA:
			switch (vismacRegisterLatch)
			{
				case 0x20:
					out2_1870(value);
				break;

				case 0x30:
					out3_1870(value);
				break;

				case 0x40:
					outValues_[port] = address;
					out4_1870(address);
				break;

				case 0x50:
					outValues_[port] = address;
					out5_1870(address);
				break;

				case 0x60:
					outValues_[port] = address;
					out6_1870(address);
				break;

				case 0x70:
					outValues_[port] = address;
					out7_1870(address);
				break;
			}
	}
//	if (port == 3 || port == 5)  return;
//	if (port == 7 && (outValues_[port] &1) == 0) return;
//	p_Main->messageInt(port);
//	p_Main->messageHex(outValues_[port]);
}

void Tmc600::cycle(int type)
{
	switch(cycleType_[type])
	{
		case 0:
			return;
		break;

		case TELMACCYCLE:
			cycleTelmac();
		break;

		case V1870CYCLE:
			cycle1870();
		break;

		case V1870BLINK:
			blink1870();
		break;
	}
}

void Tmc600::cycleTelmac()
{
	if (clockValue_ >= 0)
	{
		clockValue_--;
		if (clockValue_ == 0)
		{
			clockValue_ = clockSize_;
			p_Computer->interrupt();
		}
	}

	if (keyDown_ && (cycleValue_ >= 0))
	{
		cycleValue_--;
		if (cycleValue_ == 0)
		{
			keyDown_ = false;
			if (telmacKeyFileOpened_)
			{
				keyUpFile();
			}
			else
			{
				releaseKey();
			}
			checkCaps();
		}
		return;
	}

	if (capsCorrection_ && !keyDown_)
	{
		switchCaps();
		return;
	}

	if (telmacRunCommand_ != 0)
	{
		if ((keyLatch_ == 0x3f) && !keyDown_)
		{
			if (telmacRunCommand_ == 1)
			{
				keyboardCode_ = 'C';
				keyDown_ = true;
				cycleValue_ = 50000;
				keyDownFile();
				telmacRunCommand_++;
			}
			else if (telmacRunCommand_ == 2)
			{
				int saveExec = p_Main->pload();
				if (saveExec == 1)
					telmacRunCommand_ = 0;
				else
				{
					if (saveExec == 0)
						commandText_ = "RUN";
					else
					{
						wxString buffer;
						buffer.Printf("%d", saveExec);
						commandText_ = "CALL(" + buffer + ")";
					}
					keyboardCode_ = 0;
					keyDown_ = true;
					cycleValue_ = 50000;
					keyDownFile();
					telmacRunCommand_++;
				}
			}
			else
			{
				if (load_)
					telmacRunCommand_ = 0;
				else
				{
					if ((telmacRunCommand_-2) <= commandText_.Len())
					{
						keyboardCode_ = commandText_.GetChar(telmacRunCommand_-3);
						keyDown_ = true;
						cycleValue_ = 50000;
						keyDownFile();
						telmacRunCommand_++;
					}
					else
					{
						keyboardCode_ = 13;
						keyDown_ = true;
						cycleValue_ = 50000;
						keyDownFile();
						telmacRunCommand_ = 0;
					}
				}
			}
		}
	}

	if (telmacKeyFileOpened_)
	{
		if ((keyLatch_ == 0x3f) && !keyDown_)
		{
			if (telmacKeyFile_.Read(&keyboardCode_, 1) == 0)
			{
				telmacKeyFileOpened_ = false;
				telmacKeyFile_.Close();
			}
			else
			{
				keyDown_ = true;
				cycleValue_ = 50000;
				keyDownFile();
			}
		}
	}
}

void Tmc600::startComputer()
{
	startTelmacKeyFile();
	resetPressed_ = false;

	p_Main->setSwName("");
	p_Main->updateTitle();

    p_Main->checkAndReInstallMainRom(TMC600);
    readProgramTmc600(p_Main->getRomDir(TMC600, MAINROM1), p_Main->getRomFile(TMC600, MAINROM1), ROM, 0, NONAME);

    p_Main->checkAndReInstallFile(TMC600, "EXP ROM", EXPROM);
    readProgram(p_Main->getRomDir(TMC600, EXPROM), p_Main->getRomFile(TMC600, EXPROM), ROM, 0x4000, NONAME);
    
	if ((mainMemory_[0x4017] == 0x31) && (mainMemory_[0x4018] == 0x35) && (mainMemory_[0x4019] == 0x31) && (mainMemory_[0x401A] == 0x31) && (mainMemory_[0x401B] == 0x38) && (mainMemory_[0x401C] == 0x32))
	{
		inType_[4] = TELMACIN;
		p_Main->message("Configuring 151182 Expansion Rom");
		p_Main->message("	Output 4: select channel/printer data (channel F)");
		p_Main->message("	Input 4: AD/I Input (channel 0 to E)");
		p_Main->message("	@F400-@F40F: AD/S Input (channel 0 to F)\n");
		p_Main->enableIoGui();
	}
    p_Main->checkAndReInstallCharFile(TMC600, "Character", EXPROM);
	readChargenFile(p_Main->getCharRomDir(TMC600), p_Main->getCharRomFile(TMC600));

	defineMemoryType(0xf400, 0xf7ff, CRAM1870);
	defineMemoryType(0xf800, 0xffff, PRAM1870);

    p_Main->assDefault("mycode", 0x6300, 0x72FF);
    switch(p_Main->getRamType(TMC600))
	{
		case 0:
			defineMemoryType(0x6000, 0x7fff, RAM);
            initRam(0x6000, 0x7fff);
		break;

		case 1:
			defineMemoryType(0x6000, 0x9fff, RAM);
            initRam(0x6000, 0x9fff);
		break;

		case 2:
			defineMemoryType(0x6000, 0xbfff, RAM);
            initRam(0x6000, 0xbfff);
		break;

		case 3:
			defineMemoryType(0x6000, 0xf3ff, RAM);
            initRam(0x6000, 0xf3ff);
		break;
	}


	configure1870Telmac();
	Show(true);

	cpuCycles_ = 0;
	p_Main->startTime();

	threadPointer->Run();
}

void Tmc600::writeMemDataType(Word address, Byte type)
{
	switch (memoryType_[address/256])
	{
		case RAM:
		case ROM:
			if (mainMemoryDataType_[address] != type)
			{
				p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
				mainMemoryDataType_[address] = type;
			}
		break;
	}
}

Byte Tmc600::readMemDataType(Word address)
{
	switch (memoryType_[address/256])
	{
		case RAM:
		case ROM:
			return mainMemoryDataType_[address];
		break;
	}
	return MEM_TYPE_UNDEFINED;
}

Byte Tmc600::readMem(Word address)
{
	address_ = address;
	switch (memoryType_[address/256])
	{
		case UNDEFINED:
			return 255;
		break;

		case PRAM1870:
			return readPram(address);
		break;

		case CRAM1870:
			if (address < 0xf410)
			{
				return adsArray_[address & 0xf]/20;
			}
			else
				return readCram(address);
		break;

		case ROM:
		case RAM:
			return mainMemory_[address];
		break;

		default:
			return 255;
		break;
	}
}

void Tmc600::writeMem(Word address, Byte value, bool writeRom)
{
	address_ = address;
	switch (memoryType_[address/256])
	{
		case UNDEFINED:
		case ROM:
			if (writeRom)
				mainMemory_[address]=value;
		break;

		case PRAM1870:
			writePram(address, value);
		break;

		case RAM:
			if (mainMemory_[address]==value)
				return;
			mainMemory_[address]=value;
			if (address>= memoryStart_ && address<(memoryStart_+256))
				p_Main->updateDebugMemory(address);
			p_Main->updateAssTabCheck(address);
		break;
	}
}

void Tmc600::cpuInstruction()
{
	if (cpuMode_ == RUN)
	{
		if (steps_ != 0)
		{
			machineCycle();
			machineCycle();
			if (steps_ != 0)
			{
				cpuCycle();
				cpuCycles_ += 2;
			}
			if (debugMode_)
				p_Main->showInstructionTrace();
		}
		else
			soundCycle();

		playSaveLoad();
		checkTelmacFunction();

		if (resetPressed_)
		{
			closeTelmacKeyFile();
			keyDown_ = false;
			resetCpu();
			resetPressed_ = false;
			if (!wxGetKeyState (WXK_CAPITAL))
			{
				capsPressed_ = false;
#ifdef __WXMSW__
				::keybd_event( VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY, 0 );
				::keybd_event( VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0 );
#endif
			}
			p_Main->setSwName("");
            p_Main->eventUpdateTitle();
			telmacRunCommand_ = 0;
			telmacRunState_ = RESETSTATE;
			startTelmacKeyFile();
		}
		if (debugMode_)
			p_Main->cycleDebug();
	}
}

void Tmc600::onReset()
{
	resetPressed_ = true;
}

void Tmc600::checkTelmacFunction()
{
	switch(scratchpadRegister_[programCounter_])
	{
		case 0x0154:
			if (telmacRunState_ != BASICSTATE)
				if (mainMemory_[0x61a0] == 0)
				{
					telmacRunState_ = BASICSTATE;
//					p_Main->message("BASIC");
				}
		break;

		case 0x3550:
			p_Main->stopCassette();
		break;

		case 0x1076:	// RUN
			if (telmacRunState_ != RUNSTATE)
			{
				telmacRunState_ = RUNSTATE;
//				p_Main->message("RUN");
			}
		break;

		case 0x1f01:	// CALL
			if (telmacRunState_ != RUNSTATE)
			{
				telmacRunState_ = RUNSTATE;
//				p_Main->message("RUN");
			}
		break;


		case 0x0506:	// PSAVE
			p_Main->startCassetteSave(0);
		break;

		case 0x0500:	// PLOAD
			p_Main->setSwName ("");
            p_Main->eventUpdateTitle();
			p_Main->startCassetteLoad(0);
		break;

		case 0x0509:	// DSAVE
			p_Main->startCassetteSave(0);
		break;

		case 0x0503:	// DLOAD
			p_Main->startCassetteLoad(0);
		break;
	}
}

void Tmc600::startTelmacKeyFile()
{
	telmacKeyFileOpened_ = false;

	wxString fileName = p_Main->getKeyFile();
	if (fileName.Len() != 0)
	{
		fileName = p_Main->getKeyFileDir() + fileName;
		if (wxFile::Exists(fileName))
		{
			if (telmacKeyFile_.Open(fileName))
			{
				telmacKeyFileOpened_ = true;
			}
		}
	}
}

void Tmc600::closeTelmacKeyFile()
{
	nextKeyboardCode_ = 0;
	if (telmacKeyFileOpened_)
	{
		telmacKeyFileOpened_ = false;
		telmacKeyFile_.Close();
		if (keyDown_)
		{
			keyDown_ = false;
			keyUpFile();
		}
	}
	keyboardCode_ = 0;
}

void Tmc600::checkCaps()
{
	if (wxGetKeyState(WXK_CAPITAL) != capsPressed_)
	{
		switchCaps();
	}
}

void Tmc600::switchCaps()
{
	if (keyDown_)
	{
		capsCorrection_ = true;
	}
	else
	{
		capsCorrection_ = false;
		keyDown_ = true;
		cycleValue_ = 50000;
		keyboardCode_ = 255;
		keyDownFile();
	}
}

void Tmc600::sleepComputer(long ms)
{
	threadPointer->Sleep(ms);
}

void Tmc600::startComputerRun(bool load)
{
	load_ = load;
	if (telmacRunState_ == RESETSTATE)
		telmacRunCommand_ = 1;
	else
		telmacRunCommand_ = 2;
}

bool Tmc600::isComputerRunning()
{
	if (telmacRunState_ == RUNSTATE)
		return true;
	else
		return false;
}
