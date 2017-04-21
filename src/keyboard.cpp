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

/*
 *******************************************************************
 *** This software is copyright 2006 by Michael H Riley          ***
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

#include "main.h"
#include "keyboard.h"

Keyboard::Keyboard()
{
}

void Keyboard::configureKeyboard(int computerType, ElfPortConfiguration portConf) 
{
//	int input, efPort;

	switch(computerType)
	{
		case ELF:
		case ELFII:
		case SUPERELF:
			forceUpperCase_ = p_Main->getUpperCase(computerType);
		break;

		default:
			forceUpperCase_ = false;
		break;
	}

	keyboardEf_ = 1;
	keyboardValue_ = 0;
	rawKeyCode_ = 0;
	elfRunCommand_ = 0;
	elfKeyFileOpen_ = false;

	wxString runningComp = p_Main->getRunningComputerStr();

//	input = p_Main->getConfigItem(runningComp +"/KeyboardInput", 7l);
//	efPort = p_Main->getConfigItem(runningComp +"/KeyboardEf", 3l);

	p_Computer->setInType(portConf.keyboardInput, KEYBRDIN);
	p_Computer->setEfType(portConf.keyboardEf, KEYBRDEF); 
	p_Computer->setCycleType(KEYCYCLE, KEYBRDCYCLE);

	wxString printBuffer;
	p_Main->message("Configuring Ascii Keyboard");

	printBuffer.Printf("	Input %d: read data, EF %d: data ready flag\n", portConf.keyboardInput , portConf.keyboardEf);
	p_Main->message(printBuffer);

	keyCycles_ = 500000;
}

void Keyboard::charEventKeyboard(int keycode)
{
	rawKeyCode_ = keycode;
}

void Keyboard::keyboardUp() 
{
	keyboardEf_ = 1;
}

Byte Keyboard::efKeyboard() 
{
	return keyboardEf_;
}

Byte Keyboard::inKeyboard() 
{
	keyboardEf_ = 1;
	return keyboardValue_;
}

void Keyboard::cycleKeyboard() 
{
	if ((elfRunCommand_ != 0) && (keyboardEf_ == 1))
	{
		if (p_Computer->getScratchpadRegister(p_Computer->getProgramCounter()) == 0xf1c0)
		{
			rawKeyCode_ = 13;
			keyboardEf_ = 0;
		}
		if ((p_Computer->getScratchpadRegister(p_Computer->getProgramCounter()) == p_Computer->getBasicExecAddr(BASICADDR_KEY)) || (p_Computer->getScratchpadRegister(p_Computer->getProgramCounter()) == 0xf63d))
		{
			if (elfRunCommand_ == 1)
			{
					rawKeyCode_ = 'C';
					keyboardEf_ = 0;
					elfRunCommand_++;
			}
			else if (elfRunCommand_ == 2)
			{
				int saveExec = p_Main->pload();
				if (saveExec == 1)
					elfRunCommand_ = 0;
				else
				{
					if (saveExec == 0)
						commandText_ = "run";
					else
					{
						wxString buffer;
						buffer.Printf("%04x", saveExec);
						commandText_ = "call(@" + buffer + ")";
					}
					rawKeyCode_ = 0;
					elfRunCommand_++;
				}
			}
			else
			{
				if (load_)
					elfRunCommand_ = 0;
				else
				{
					if ((elfRunCommand_-2) <= commandText_.Len())
					{
						rawKeyCode_ = commandText_.GetChar(elfRunCommand_-3);
						keyboardEf_ = 0;
						elfRunCommand_++;
					}
					else
					{
						rawKeyCode_ = 13;
						keyboardEf_ = 0;
						elfRunCommand_ = 0;
					}
				}
			}
		}
	}

	if (elfKeyFileOpen_ && keyboardEf_ == 1 && keyCycles_ == 0) 
	{
		if (elfKeyFile_.Read(&rawKeyCode_, 1) == 0)
		{
			elfKeyFileOpen_ = false;
			elfKeyFile_.Close();
		}
		else
		{
			keyCycles_ = 1000;
			if (rawKeyCode_ == 10) rawKeyCode_ = 13;
			if (rawKeyCode_ == 13 && lastKeyCode_ == 13) rawKeyCode_ = 0;
			lastKeyCode_ = rawKeyCode_;
			if (rawKeyCode_ == 13) keyCycles_ = 130000;
		}
	}
	if (keyCycles_ > 0 && keyboardEf_ == 1) 
		keyCycles_--;
	if (rawKeyCode_<0) 
		rawKeyCode_ = 0;
	if (rawKeyCode_ != 0 &&(Byte)rawKeyCode_ <= 127) 
	{
		if (forceUpperCase_ && rawKeyCode_ >= 'a' && rawKeyCode_ <= 'z')
 			rawKeyCode_ -= 32;
 		keyboardValue_ = rawKeyCode_;
		rawKeyCode_ = 0;
		keyboardEf_ = 0;
	}
}

void Keyboard::startElfKeyFile(wxString elfTypeStr)
{
	elfKeyFileOpen_ = false;

	wxString fileName = p_Main->getKeyFile();
	if (fileName.Len() != 0)
	{
		fileName = p_Main->getKeyFileDir() + fileName;
		if (wxFile::Exists(fileName)) 
		{
			if (elfKeyFile_.Open(fileName))
			{
				elfKeyFileOpen_ = true;
			}
		}
	}
}

void Keyboard::closeElfKeyFile()
{
	if (elfKeyFileOpen_)
	{
		elfKeyFileOpen_ = false;
		elfKeyFile_.Close();
	}
}

void Keyboard::startElfRun(bool load)
{
	load_ = load;
	if ((p_Computer->getRunState() == RESETSTATE) && (p_Computer->getLoadedProgram() != RCABASIC4))
		elfRunCommand_ = 1;
	else
		elfRunCommand_ = 2;
}

void Keyboard::setForceUpperCaseKeyboard(bool status)
{
	forceUpperCase_ = status;
}