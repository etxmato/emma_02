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
#include "pecom.h"

#define SHIFT 1
#define CTRL 1

#define PRIDLE 0
#define PRW_START 1
#define PRW_STOP 2
#define PRBUSY 3

#define DMACYCLE 5
#define PRINTCYCLEVALUE 6

int inputKeyValue[] =
{
	0, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, //64
	18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 1, 0, 0, 0, 0, //16
	10, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, //32
	2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, //48
	0, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, //64
	18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 0, 0, 0, 0, 0, //80
	0, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, //96
	18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 0, 0, 0, 0, 25, //112
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //128
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //144
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //160
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //176
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //192
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //208
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //224
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //240
};

int bitValue[] =
{
	0, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2,
	1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 1, 0, 0, 0, 0,
	1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2,
	1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2,
	0, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2,
	1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 0, 2, 0, 0, 0,
	0, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2,
	1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 0, 0, 0, 0, 2,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

int shiftKey[] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, SHIFT, 0, SHIFT, SHIFT,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, SHIFT, SHIFT, 0, SHIFT, 0, 0,
	SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT,
	SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, 0, SHIFT, 0, 0, SHIFT,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

int shiftKey2[] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, SHIFT, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

int ctrlKey[] =
{
	0, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL,
	CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

Pecom::Pecom(const wxString& title, const wxPoint& pos, const wxSize& size, double zoomLevel, int computerType, double clock)
:V1870(title, pos, size, zoomLevel, computerType, clock)
{
	p_Printer = new Printer();
	p_Printer->initPecom(p_Printer);
}

Pecom::~Pecom()
{
	p_Printer->closeFrames();
	delete p_Printer;
	p_Main->setMainPos(PECOM, GetPosition());
/*	if (!capsPressedOnStart_ && wxGetKeyState(WXK_CAPITAL))
	{
#ifdef __WXMSW__
		::keybd_event( VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY, 0 );
		::keybd_event( VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 1 );
#endif
    }*/
}

void Pecom::charEvent(int keycode)
{
	if (keyDown_) return;
	if (keycode < 256)
	{
        if (keycode >= 'a' && keycode <='z')
            keycode-=32;
        else
        {
            if (keycode >= 'A' && keycode <='Z')
                keycode+=32;
        }
		keyValue_[inputKeyValue[keycode]] |= (bitValue[keycode]&0xff);
		if (keyValue_[inputKeyValue[keycode]] != 0)
		{
			keyDown_ = true;
			keyboardCode_ = keycode;
			ctrlEf1_ = ctrlKey[keycode];
			shiftEf2_ = shiftKey[keycode];
			shiftEf3_ = shiftKey2[keycode];
		}
	}
}

bool Pecom::keyDownExtended(int keycode, wxKeyEvent& event)
{
	if (pecomRunCommand_ != 0)
	{
		if (keyDown_)
		{
			keyDown_ = false;
			ctrlEf1_ = 0;
			shiftEf2_ = 0;
			shiftEf3_ = 0;
			if (keyboardCode_ == 13)
				keyValue_[0] &= 0xfe; 

			if (keyboardCode_ > 26)
				keyValue_[inputKeyValue[keyboardCode_]]  &= ((bitValue[keyboardCode_]&0xff)  ^ 0xff); 
			keyboardCode_ = 0;
		}
		pecomRunCommand_ = 0;
	}

	if (keyDown_) return false;

	Byte shiftPressed = 0;
	switch (event.GetModifiers())
	{
		case wxMOD_SHIFT :
			shiftPressed = 1;
		break;
	}

	switch (keycode)
	{
        case WXK_ESCAPE:
			escEf4_ = 1;
			return true;
		break;

        case WXK_RETURN:
			keyValue_[0] |= 1;
			keyDown_ = true;
			shiftEf2_ = shiftPressed;
			return true;
		break;

        case WXK_END:
			keyValue_[1] |= 1;
			keyDown_ = true;
			shiftEf2_ = shiftPressed;
			return true;
		break;

		case WXK_HOME:
			keyValue_[0] |= 2; 
			keyDown_ = true;
			shiftEf2_ = shiftPressed;
			return true;
		break;

		case WXK_DOWN:
			keyValue_[23] |= 2; 
			keyDown_ = true;
			shiftEf2_ = shiftPressed;
			return true;
		break;

		case WXK_LEFT:
			keyValue_[24] |= 1; 
			keyDown_ = true;
			shiftEf2_ = shiftPressed;
			return true;
		break;

		case WXK_RIGHT:
			keyValue_[24] |= 2; 
			keyDown_ = true;
			shiftEf2_ = shiftPressed;
			return true;
		break;

		case WXK_UP:
			keyValue_[25] |= 1; 
			keyDown_ = true;
			shiftEf2_ = shiftPressed;
			return true;
		break;
	}
	return false;
}

void Pecom::keyUpExtended(int keycode, wxKeyEvent& WXUNUSED(event))
{
	switch (keycode)
	{
        case WXK_ESCAPE:
			escEf4_ = 0;
			return;
		break;

        case WXK_RETURN:
			keyValue_[0] &= 0xfe;
			keyDown_ = false;
			shiftEf2_ = 0;
			return;
		break;

        case WXK_END:
			keyValue_[1] &= 0xfe;
			keyDown_ = false;
			shiftEf2_ = 0;
			return;
		break;

		case WXK_HOME:
			keyValue_[0] &= 0xfd; 
			keyDown_ = false;
			shiftEf2_ = 0;
			return;
		break;

		case WXK_DOWN:
			keyValue_[23] &= 0xfd; 
			keyDown_ = false;
			shiftEf2_ = 0;
			return;
		break;

		case WXK_LEFT:
			keyValue_[24] &= 0xfe; 
			keyDown_ = false;
			shiftEf2_ = 0;
			return;
		break;

		case WXK_RIGHT:
			keyValue_[24] &= 0xfd; 
			keyDown_ = false;
			shiftEf2_ = 0;
			return;
		break;

		case WXK_UP:
			keyValue_[25] &= 0xfe; 
			keyDown_ = false;
			shiftEf2_ = 0;
			return;
		break;
	}

	if (keyDown_)
	{
		keyDown_ = false;
		ctrlEf1_ = 0;
		shiftEf2_ = 0;
		shiftEf3_ = 0;
		keyValue_[inputKeyValue[keyboardCode_]]  &= ((bitValue[keyboardCode_]&0xff) ^ 0xff); 
		keyboardCode_ = 0;
	}
}

void Pecom::keyDownFile()
{
	if (keyboardCode_ == 13)
	{
		keyDown_ = true;
		keyValue_[0] |= 1; 
	}

	if (keyboardCode_ > 26)
	{
		keyValue_[inputKeyValue[keyboardCode_]] |= (bitValue[keyboardCode_]&0xff); 
		if (keyValue_[inputKeyValue[keyboardCode_]] != 0)
		{
			keyDown_ = true;
			ctrlEf1_ = ctrlKey[keyboardCode_];
			shiftEf2_ = shiftKey[keyboardCode_];
			shiftEf3_ = shiftKey2[keyboardCode_];
		}
	}
}

void Pecom::keyUpFile()
{
	if (keyDown_)
	{
		keyDown_ = false;
		ctrlEf1_ = 0;
		shiftEf2_ = 0;
		shiftEf3_ = 0;
		if (keyboardCode_ == 13)
			keyValue_[0] &= 0xfe; 

		if (keyboardCode_ > 26)
			keyValue_[inputKeyValue[keyboardCode_]]  &= ((bitValue[keyboardCode_]&0xff)  ^ 0xff); 
		keyboardCode_ = 0;
	}
}

void Pecom::configureComputer()
{
	outType_[1] = PECOMBANK;
	inType_[3] = PECOMKEY;
	cycleType_[COMPUTERCYCLE] = PECOMCYCLE;

	efType_[1] = PECOMEF1;
	efType_[2] = PECOMEF2;
	efType_[3] = PECOMEF3;
	efType_[4] = PECOMEF4;

	p_Main->message("Configuring Pecom 32/64");
	p_Main->message("	Input 3: keyboard, output 1: bank switch");
	p_Main->message("	EF 1: CTRL or display/non display period, depending on OUT1 state");
	p_Main->message("	EF 2: cassette in, printer ready and SHIFT, depending on OUT1 state");
	p_Main->message("	EF 3: CAPS, EF 4: ESC \n");

	resetCpu();
}

void Pecom::initComputer()
{
	init1870();
	setClear(1);
	setWait(1);
	keyboardCode_ = 0;
	keyDown_ = false;
	cycleValue_ = 1;

	for (int i=0; i<26; i++)
		keyValue_[i]=0;

	addressLatch_ = 0x8000;
	keyboardCount_ = 0;
	videoRAM_ = false;
	ctrlEf1_ = 0;
	shiftEf2_ = 0;
	cassetteEf_ = 0;
	shiftEf3_ = 0;
	escEf4_ = 0;
//	tapeRunning_ = false;
	pecomKeyFileOpened_ = false;
	dmaCounter_ = -100;
	printerStatus_ = PRIDLE;
	cycleCount_ = PRINTCYCLEVALUE;
	pecomRunCommand_ = 0;
	pecomRunState_ = BASICSTATE;

/*	if (!wxGetKeyState (WXK_CAPITAL))
	{
		capsPressedOnStart_ = false;
#ifdef __WXMSW__
		::keybd_event( VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY, 0 );
		::keybd_event( VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0 );
#endif
	}
	else
		capsPressedOnStart_ = true;*/
}

Byte Pecom::ef(int flag)
{
	switch(efType_[flag])
	{
		case 0:
			return 1;
		break;

		case PECOMEF1:
			if (videoRAM_)
				return ef1_1870();
			else
				return !ctrlEf1_;
		break;

		case PECOMEF2:
			if (videoRAM_)
			{
				if (printerStatus_ != PRIDLE)
					return true;
				else
					return cassetteEf_;
			}
			else
				return shiftEf2_;
		break;

		case PECOMEF3:
			return shiftEf3_;
		break;

		case PECOMEF4:
			return !escEf4_;
		break;

		default:
			return 1;
	}
}

Byte Pecom::in(Byte port, Word address)
{
	Byte ret;

//	p_Main->messageInt(port);
	switch(inType_[port])
	{
		case 0:
			ret = 255;
		break;

		case PECOMKEY:
//			ret = keyValue_[address - 0x7cca] & 0x3;
			ret = keyValue_[(address&0x3f) - 0xa] & 0x3;

/*			if (inputKeyValue[keyboardCode_] == ((address & 0x3f) - 0xa))
			{
				if (!wxGetKeyState((wxKeyCode)keyboardCode_))
				{
					keyValue_[(address & 0x3f) - 0xa] = keyValue_[(address & 0x3f) - 0xa] & 0xfc;
					keyboardCode_ = 0;
				}
			}*/

			if (pecomRunCommand_ != 0)
			{
				cycleValue_--;
				if (cycleValue_ <= 0)
				{
					cycleValue_ = 52;
					if (keyDown_)
					{
						keyUpFile();
						if (pecomRunCommand_ >= 255)
							pecomRunCommand_ = 0;
					}
					else
					{
						if (pecomRunCommand_ == 1)
						{
							int saveExec = p_Main->pload();
							if (saveExec == 1)
								pecomRunCommand_ = 0;
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
								pecomRunCommand_++;
							}
						}
						else
						{
							if (load_)
								pecomRunCommand_ = 0;
							else
							{
								if ((pecomRunCommand_-1) <= commandText_.Len())
								{
									keyboardCode_ = commandText_.GetChar(pecomRunCommand_-2);
									keyDownFile();
									pecomRunCommand_++;
								}
								else
								{
									keyboardCode_ = 13;
									keyDownFile();
									pecomRunCommand_ = 255;
								}
							}
						}
					}
				}
			}

			if (pecomKeyFileOpened_)
			{
				cycleValue_--;
				if (cycleValue_ <= 0)
				{
					cycleValue_ = 52;
					if (keyDown_)
						keyUpFile();
					else
					{
						if (pecomKeyFile_.Read(&keyboardCode_, 1) == 0)
						{
							pecomKeyFileOpened_ = false;
							pecomKeyFile_.Close();
						}
						else
							keyDownFile();
					}
				}
			}

		break;

		default:
			ret = 255;
	}
	inValues_[port] = ret;
	return ret;
}

void Pecom::out(Byte port, Word address, Byte value)
{
	outValues_[port] = value;
	switch(outType_[port])
	{
		case 0:
			return;
		break;

		case PECOMBANK:
			if (dmaCounter_ == -100)
				dmaCounter_ = DMACYCLE ;
			addressLatch_ = 0;
			if (value == 2)
			{
				videoRAM_ = true;
//				p_Main->message("V");
//				p_Main->messageInt(value);
			}
			else
			{
				videoRAM_ = false;
	//			p_Main->message("R");
	//			p_Main->messageInt(value);
			}
		break;

		case V1870OUT3:
			out3_1870(value);
		break;

		case V1870OUT4:
			outValues_[port] = address;
			out4_1870(address);
		break;

		case V1870OUT5:
			outValues_[port] = address;
			out5_1870(address);
		break;

		case V1870OUT6:
			outValues_[port] = address;
			out6_1870(address);
		break;

		case V1870OUT7:
			outValues_[port] = address;
			out7_1870(address);
		break;
	}
}

void Pecom::cycle(int type)
{
	switch(cycleType_[type])
	{
		case 0:
			return;
		break;

		case PECOMCYCLE:
			cyclePecom();
		break;

		case V1870CYCLE:
			cycle1870();
		break;

		case V1870BLINK:
			blink1870();
		break;
	}
}

void Pecom::cyclePecom()
{
	if (dmaCounter_ > 0)
	{
		dmaCounter_--;
		if (dmaCounter_ <= 0)
		{
			dmaOut();
			dmaCounter_ = DMACYCLE;
		}
	}
/*	cycleCount_--;
	if (cycleCount_ <= 0)
	{
		cycleCount_ = PRINTCYCLEVALUE;
		printEf_ = !printEf_;
	}*/
}

void Pecom::startComputer()
{
	startPecomKeyFile();
	resetPressed_ = false;

	p_Main->setSwName("");
	p_Main->updateTitle();

    p_Main->checkAndReInstallMainRom(PECOM);
	readProgramPecom(p_Main->getRomDir(PECOM, MAINROM1), p_Main->getRomFile(PECOM, MAINROM1), ROM, 0x8000, NONAME);
    
    if (mainMemory_[0x80bd] == 0x90 && mainMemory_[0x80be] == 0x05)
    {
        pecom32_ = true;
        extendedRAM_ = true;
        defineMemoryType(0xC000, 0xf3ff, RAM);
    }
    else
    {
        pecom32_ = false;
        extendedRAM_ = false;
    }
   
	defineMemoryType(0xf400, 0xf7ff, CRAM1870);
	defineMemoryType(0xf800, 0xffff, PRAM1870);

/*	if (p_Main->getRomFile(PECOM, MAINROM1) == "pecom32.bin")
	{
		defineMemoryType(0x0000, 0x3fff, RAM);
		defineMemoryType(0x4000, 0x7fff, MAPPEDRAM);
	}
	else*/ 
	// Pecom 32 I guess had 32 KB in total as the 64 has 32K ROM and 32KRAM. Pecom 32 has 16KB ROM, so 16KB RAM? 
	// However not sure how it would have worked
	// The mapping above doesn't work

	defineMemoryType(0x0000, 0x7fff, RAM);
    initRam(0x0, 0x7fff);
    p_Main->assDefault("mycode", 0x200, 0x11FF);
    
	configure1870Pecom();
	Show(true);

	cpuCycles_ = 0;
	p_Main->startTime();

	threadPointer->Run();
}

void Pecom::writeMemDataType(Word address, Byte type)
{
	address = address | addressLatch_;
	switch (memoryType_[address/256])
	{
		case RAM:
		case MAPPEDRAM:
		case ROM:
        case PRAM1870:
        case CRAM1870:
			if (mainMemoryDataType_[address] != type)
			{
				p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
				mainMemoryDataType_[address] = type;
			}
		break;
	}
}

Byte Pecom::readMemDataType(Word address)
{
	address = address | addressLatch_;
	switch (memoryType_[address/256])
	{
		case RAM:
		case MAPPEDRAM:
		case ROM:
        case PRAM1870:
        case CRAM1870:
            return mainMemoryDataType_[address];
		break;
	}
	return MEM_TYPE_UNDEFINED;
}

Byte Pecom::readMem(Word address)
{
	address_ = address | addressLatch_;
	switch (memoryType_[address_/256])
	{
		case UNDEFINED:
			return 255;
		break;

		case PRAM1870:
			if (videoRAM_)
				return readPram(address_);
			else
				return mainMemory_[address_];
		break;

		case CRAM1870:
			if (videoRAM_)
				return readCram(address_);
			else
				return mainMemory_[address_];
		break;

		case ROM:
		case RAM:
			return mainMemory_[address_];
		break;

		case MAPPEDRAM:
			return mainMemory_[address_ & 0x3fff];
		break;

		default:
			return 255;
		break;
	}
}

void Pecom::writeMem(Word address, Byte value, bool writeRom)
{
	address_ = address;
	if (writeRom)
	{
		mainMemory_[address]=value;
		return;
	}
	switch (memoryType_[address/256])
	{
		case UNDEFINED:
		case ROM:
		break;

		case PRAM1870:
			if (videoRAM_)
				writePram(address, value);
            if (extendedRAM_)
            {
                if (mainMemory_[address]==value)
                    return;
                mainMemory_[address]=value;
                if (address>= memoryStart_ && address<(memoryStart_+256))
                    p_Main->updateDebugMemory(address);
                p_Main->updateAssTabCheck(address);
            }
		break;

		case CRAM1870:
			if (videoRAM_)
				writeCram(address, value);
            if (extendedRAM_)
            {
                if (mainMemory_[address]==value)
                    return;
                mainMemory_[address]=value;
                if (address>= memoryStart_ && address<(memoryStart_+256))
                    p_Main->updateDebugMemory(address);
                p_Main->updateAssTabCheck(address);
            }
        break;

		case MAPPEDRAM:
			if (mainMemory_[address&0x3fff]==value)
				return;
			mainMemory_[address&0x3fff]=value;
			if ((address&0x3fff)>= memoryStart_ && (address&0x3fff)<(memoryStart_+256)) 
				p_Main->updateDebugMemory(address&0x3fff);
			p_Main->updateAssTabCheck(address&0x3fff);
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

void Pecom::cpuInstruction()
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
		checkPecomFunction();

		if (resetPressed_)
		{
			closePecomKeyFile();
			keyDown_ = false;
			videoRAM_ = false;
			ctrlEf1_ = 0;
			shiftEf2_ = 0;
			shiftEf3_ = 0;
			escEf4_ = 0;
			addressLatch_ = 0x8000;
			resetCpu();
			resetPressed_ = false;
			dmaCounter_ = -100;
/*			if (!wxGetKeyState (WXK_CAPITAL))
			{
#ifdef __WXMSW__
				::keybd_event( VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY, 0 );
				::keybd_event( VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0 );
#endif
			}*/
			p_Main->setSwName("");
            p_Main->eventUpdateTitle();
			pecomRunCommand_ = 0;
			startPecomKeyFile();
		}
		if (debugMode_)
			p_Main->cycleDebug();
	}
}

void Pecom::onReset()
{
	resetPressed_ = true;
}

void Pecom::printOutPecom(int q)
{
	cassetteEf_ = false;
	switch (printerStatus_)
	{
		case PRIDLE:
			if (q == 0)
			{
				printerStatus_ = PRW_START;
//				p_Main->message("start");
			}
//			else
//				p_Main->message("idle - none");
		break;

		case PRW_START:
			if (q)
			{
//				p_Main->message("busy");
				printerStatus_ = PRBUSY;
				p_Printer->startPecomChar();
				printBit_ = 0;
                printValue_ = 0;
            }
//			else
//				p_Main->message("start - none");
		break;

		case PRBUSY:
//			p_Main->message("busy - bit");
//			p_Main->messageInt(printBit_);
            if (q == 0)
                printValue_ += (1 <<(printBit_));
            if (printBit_ == 8)
			{ 
//				p_Main->message("char finished");
                p_Main->eventPrintPecom(printValue_);
                printerStatus_ = PRIDLE;
				keyValue_[0] &= 0xfe;
			}
			printBit_++;
			break;

/*		case PRW_STOP:
			if (q == 0)
			{
				p_Main->message("idle");
				printerStatus_ = PRIDLE;
			}
			else
				p_Main->message("stop - none");
		break;*/
	}
}

void Pecom::checkPecomFunction()
{
    if (pecom32_)
    {
        switch(scratchpadRegister_[programCounter_])
        {
            case 0x904A:
                p_Main->stopCassette();
#if defined(__WXMSW__ )
                Iconize(false);
                Raise();
                Show(true);
                Maximize(false);
#endif
            break;
                
            case 0x80BF:	// Basic input
                p_Main->stopCassette();
                if (pecomRunState_ != BASICSTATE)
                    if (mainMemory_[0xa0] == 0)
                        pecomRunState_ = BASICSTATE;
            break;
                
            case 0xA076:	// RUN
                if (pecomRunState_ != RUNSTATE)
                    pecomRunState_ = RUNSTATE;
            break;
                
            case 0xA96B:	// CALL
                if (pecomRunState_ != RUNSTATE)
                    pecomRunState_ = RUNSTATE;
            break;
                
            case 0x953F:	// PSAVE
            case 0x9542:	// DSAVE
                p_Main->startCassetteSave(0);
            break;
                
            case 0x9324:	// PLOAD
                p_Main->setSwName ("");
                p_Main->eventUpdateTitle();
                p_Main->startCassetteLoad(0);
            break;
                
            case 0x9327:	// DLOAD
                p_Main->startCassetteLoad(0);
            break;
                
            case 0x009f:
            case 0x00a3:
                p_Main->eventShowMessage(mainMemory_[scratchpadRegister_[programCounter_]+1]*256+mainMemory_[scratchpadRegister_[programCounter_]+2]);
                p_Main->eventShowMessage(mainMemory_[scratchpadRegister_[programCounter_]+5]*256+mainMemory_[scratchpadRegister_[programCounter_]+6]);
            break;
        }
    }
    else
    {
        switch(scratchpadRegister_[programCounter_])
	{
		case 0x904A:
			p_Main->stopCassette();
#if defined(__WXMSW__ )
			Iconize(false);
			Raise();
			Show(true);
			Maximize(false);
#endif
		break;

		case 0x80D0:	// Basic input
			p_Main->stopCassette();
			if (pecomRunState_ != BASICSTATE)
				if (mainMemory_[0xa0] == 0)
					pecomRunState_ = BASICSTATE;
		break;

		case 0xA076:	// RUN
			if (pecomRunState_ != RUNSTATE)
				pecomRunState_ = RUNSTATE;
		break;

		case 0xA96B:	// CALL
			if (pecomRunState_ != RUNSTATE)
				pecomRunState_ = RUNSTATE;
		break;

		case 0x953F:	// PSAVE
		case 0x9542:	// DSAVE
		case 0xCAAB:	// ED, SAVE
		case 0xCACF:	// ED, BSAVE
		case 0x8532:	// M+, W-SAVE
			p_Main->startCassetteSave(0);
		break;

		case 0x8E8B:	// PLOAD
		case 0xCB50:	// ED, LOAD
		case 0x860B:	// M+, W-LOAD
			p_Main->setSwName ("");
            p_Main->eventUpdateTitle();
			p_Main->startCassetteLoad(0);
		break;

		case 0x8E8E:	// DLOAD
			p_Main->startCassetteLoad(0);
		break;

//        case 0x009f:
//        case 0x00a3:
//             p_Main->eventShowMessage(mainMemory_[scratchpadRegister_[programCounter_]+1]*256+mainMemory_[scratchpadRegister_[programCounter_]+2]);
//             p_Main->eventShowMessage(mainMemory_[scratchpadRegister_[programCounter_]+5]*256+mainMemory_[scratchpadRegister_[programCounter_]+6]);
//        break;
    }
    }
}

void Pecom::startPecomKeyFile()
{
	pecomKeyFileOpened_ = false;

	wxString fileName = p_Main->getKeyFile();
	if (fileName.Len() != 0)
	{
		fileName = p_Main->getKeyFileDir() + fileName;
		if (wxFile::Exists(fileName))
		{
			if (pecomKeyFile_.Open(fileName))
			{
				pecomKeyFileOpened_ = true;
			}
		}
	}
}

void Pecom::startComputerRun(bool load)
{
	load_ = load;
	pecomRunCommand_ = 1;
}

bool Pecom::isComputerRunning()
{
	if (pecomRunState_ == RUNSTATE)
		return true;
	else
		return false;
}

void Pecom::sleepComputer(long ms)
{
	threadPointer->Sleep(ms);
}

void Pecom::closePecomKeyFile()
{
	if (pecomKeyFileOpened_)
	{
		pecomKeyFileOpened_ = false;
		pecomKeyFile_.Close();
		if (keyDown_)
		{
			keyUpFile();
		}
	}
	if (pecomRunCommand_ != 0)
	{
		if (keyDown_)
		{
			keyUpFile();
		}
		pecomRunCommand_ = 0;
	}
	keyboardCode_ = 0;
}
