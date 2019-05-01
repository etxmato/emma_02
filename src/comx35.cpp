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
#include "wx/utils.h"

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

#if !wxUSE_THREADS
    #error "This sw requires thread support!"
#endif // wxUSE_THREADS

#include "main.h"
#include "comx35.h"
#include "printer.h"

#define DMACYCLE 5

Comx::Comx(const wxString& title, const wxPoint& pos, const wxSize& size, double zoomLevel, int computerType, double clock)
:Expansion(title, pos, size, zoomLevel, computerType, clock)
{
}

Comx::~Comx()
{
    saveRam();

	int numberOfSlots = 4;

	if (!expansionRomLoaded_)  numberOfSlots = 1;
	for (int slot=0; slot<numberOfSlots; slot++)
	{
		switch(comxExpansionType_[slot])
		{
			case COMXPRINTER:
				if (slot == printerSlot_)
				{
					p_PrinterParallel->closeFrames();
					delete p_PrinterParallel;
				}
			break;

			case COMXTHPRINTER:
				if (slot == thermalSlot_)
				{
					p_PrinterThermal->closeFrames();
					delete p_PrinterThermal;
				}
			break;

			case COMXRS232:
				if (slot == serialSlot_)
				{
					p_PrinterSerial->closeFrames();
					delete p_PrinterSerial;
				}
			break;
		}
	}
	p_Main->setMainPos(COMX, GetPosition());
//    keyLogFilePc_.Close();
//    keyLogFile1802_.Close();
//    keyLogFileCycle_.Close();
}

void Comx::configureComputer()
{
	inType_[3] = COMXIN;
	outType_[1] = COMXOUT;
	efType_[2] = COMXEF2;
	efType_[3] = COMXEF3;
	efType_[4] = COMXEF4;
    cycleType_[COMPUTERCYCLE] = COMXCYCLE;
//    cycleType_[KEYCYCLE] = KEYBRDCYCLE;

	p_Main->message("Configuring Comx");
	p_Main->message("	Input 3: keyboard input");
	p_Main->message("	EF 2: PAL/NTSC and keyboard RPT, EF 3: keyboard DA, EF 4: cassette in\n");

	resetCpu();
}

void Comx::initComputer()
{
	init1870();
	qMode_ = 0;
    setClear(1);
    setWait(1);
    
	thermalPrinting_ = false;
	thermalEF_ = 0;

	keyboardEf2_ = 1;
	keyboardEf3_ = 1;
	cassetteEf_ = 0;

    keyCycles_ = 500000;
    rawKeyCode_ = 0;

    keyboardCode_ = 0;
	previousKeyCode_ = (wxKeyCode) 0;

	dmaCounter_ = -100;
	debounceCounter_ = 0;
	comxRunCommand_ = 0;
	comxRunState_ = RESETSTATE;
	nvramWriteProtected_ = true;

	systemTime_ = wxDateTime::Now();
	comxTime_ = wxDateTime::Now();

/*    wxString fileName_pc = p_Main->getDataDir() + "key_pc_in.log";
    if (wxFile::Exists(fileName_pc))
        keyLogFilePc_.Open(fileName_pc, wxFile::write);
    else
        keyLogFilePc_.Create(fileName_pc);
    wxString fileName_1802 = p_Main->getDataDir() + "key_1802.log";
    if (wxFile::Exists(fileName_1802))
        keyLogFile1802_.Open(fileName_1802, wxFile::write);
    else
        keyLogFile1802_.Create(fileName_1802);*/
//    wxString fileName_cycle = p_Main->getDataDir() + "key_cycle.log";
//    if (wxFile::Exists(fileName_cycle))
//        keyLogFileCycle_.Open(fileName_cycle, wxFile::write);
//    else
//        keyLogFileCycle_.Create(fileName_cycle);
}

Byte Comx::ef(int flag)
{
	switch(efType_[flag])
	{
		case 0:
			return 1;
		break;

		case COMXEF2:
			return ef2();
		break;

		case COMXEF3:
			return ef3();
		break;

		case COMXEF4:
			return ef4();
		break;

		case V1870EF:
			return ef1_1870();
		break;

		default:
			return 1;
	}
}

Byte Comx::ef2()
{
	if (qMode_)
	{
		return keyboardEf2_;
	}
	if (videoMode_ == NTSC) return 0;
	return 1;
}

Byte Comx::ef3()
{
	return keyboardEf3_;
}

Byte Comx::ef4()
{
	switch(comxExpansionType_[expansionSlot_])
	{
        case NETWORK:
            return efNetwork();
        break;
            
        case COMXFLOP:
			if ((registerSelect_&0x10) == 0x10)
				return ef1770();
		break;

		case COMXSUPERBOARD:
			if (nvramWriteProtected_)
				return cassetteEf_;
			else
				return usb_ef();
		break;

		case COMX80COLUMN:
			if (isLoading() || realCassetteLoad_)
				return cassetteEf_;
			else
				return ef6845();
		break;

		case COMXTHPRINTER:
/*			if (p_PrinterThermal->getThermalEf4())
				return 1;
			else
				return cassetteEf_;*/
			if (thermalPrinting_)
			{
				if (p_Main->getThermalEf())
					return 1;
				else
				{
					thermalEF_ = !thermalEF_;
					return thermalEF_;
				}
			}
			else
				return cassetteEf_;
		break;

    }
	return cassetteEf_;
}

void Comx::switchQ(int value)
{
    if (p_Main->isDiagActive(COMX))
    {
        if (p_Main->getDiagCassetteCables() == 1)
            cassetteEf_ = value;
    }
}

Byte Comx::in()
{
	Byte ret;

	if (p_Main->isDiagOn(COMX) == 1)
	{
		switch (keyboardCode_)
		{
			case 'a':
			case 'r':
			case 's':
				return 0;
			break;
		}
        if ((outValues_[1]) != 0)
            keyboardEf2_ = 0;
    }
	keyboardEf3_ = 1;
	ret = keyboardCode_;

//	wxMutexGuiEnter();
	if (wxGetKeyState(previousKeyCode_))
	{
		keyboardEf2_ = 0;
	}
//	wxMutexGuiLeave();
	switch(ret)
	{
		case '@':ret = 0x20; break;
		case '#':ret = 0x23; break;
		case '\'': ret = 0x27; break;
		case '[':ret = 0x28; break;
		case ']':ret = 0x29; break;
		case ':':ret = 0x2a; break;
		case ';':ret = 0x2b; break;
		case '<':ret = 0x2c; break;
		case '=':ret = 0x2d; break;
		case '>':ret = 0x2e; break;
		case '\\':ret = 0x2f; break;
		case '.':ret = 0x3a; break;
		case ',':ret = 0x3b; break;
		case '(':ret = 0x3c; break;
		case '^':ret = 0x3d; break;
		case ')':ret = 0x3e; break;
		case '_':ret = 0x3f; break;
		case '?':ret = 0x40; break;
		case '+':ret = 0x5b; break;
		case '-':ret = 0x5c; break;
		case '*':ret = 0x5d; break;
		case '/':ret = 0x5e; break;
		case ' ':ret = 0x5f; break;
	}
	if (ret >= 0x90)  ret &= 0x7f;
//    keyLogFile1802_.Write(&ret, 1);
	return ret;
//    keyboardEf3_ = 1;
//    keyLogFile1802_.Write(&keyboardCode_, 1);
//    return keyboardCode_;
}

Byte Comx::in(Byte port, Word WXUNUSED(address))
{
	Byte ret;

	switch(inType_[port])
	{
		case 0:
			ret = 255;
		break;

		case COMXIN:
			ret = in();
		break;

		case COMXRTCIN1:
			nvramWriteProtected_ = !nvramWriteProtected_;
			if (nvramWriteProtected_)
				return 1;
			else
				return 0;
		break;

		case COMXEXPIN2:
			ret = expansionIn2();
		break;

		case COMXEXPIN4:
			ret = expansionIn4();
		break;

		case COMXEXPIN5:
			ret = usbIn();
		// 	ret = usbInVspe();
		break;

		case COMXEXPIN6:
			ret = usbIn6();
		break;

        case COMXDIAGIN1:
            if (diagRomActive_)
            {
                diagRomActive_ = false;
                updateDiagLedStatus(1, diagRomActive_);
            }
            ret = 0xff;
            if (keyboardEf3_ == 0)
            {
                switch(keyboardCode_)
                {
                    case 's':
                        ret = 0xfb;
                    break;
                    case 'a':
                        ret = 0xbf;
                    break;
                    case 'r':
                        ret = 0x7f;
                    break;
                }
            }
			if (debounceCounter_ > 0)
				ret = ret & 0xfd;
        break;
            
        case COMXDIAGIN2:
            ret = 0;
            if (p_Main->getDiagRomChecksum() == 0)
                ret = 2;
            if (dmaCounter_ == -100)
                ret = ret ^ 4;
            if (p_Main->getDiagFactory() == 1)
                ret = ret ^ 8;
            // bit 1 ROM checksum
            // bit 2 IDEN itself
            // bit 3 keyboard debounce, repeat and IDEN
        break;
 
        default:
			ret = 255;
	}
	inValues_[port] = ret;
	return ret;
}

void Comx::out(Byte port, Word address, Byte value)
{
	outValues_[port] = value;
	switch(outType_[port])
	{
		case 0:
			return;
		break;

		case COMXOUT:
			bankOut(value);
		break;

		case COMXEXPOUT:
			expansionOut(value);
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
            
        case COMXDIAGOUT1:
			if (p_Main->getDiagFactory() == 0)
				return;
            switch (value)
            {
                case 0:
					debounceCounter_ = 4000;
					p_Main->eventDebounceTimer();
                break;
                    
				case 0x10:
				case 0x11: //set breakpoint onn ce8f / CEE8
				case 0x12:
				case 0x13:
				case 0x14:
				case 0x15:
				case 0x16:
				case 0x17:
					if (keyboardEf3_ == 1)
                    {
                        keyboardCode_ = (value & 0xf)+ 0x30;
                        keyboardEf2_ = 1;
                        keyboardEf3_ = 0;
                    }
                break;

				case 0x09:
                    if (keyboardEf3_ == 1)
                    {
                        keyboardCode_ = '8';
                        keyboardEf2_ = 1;
                        keyboardEf3_ = 0;
                    }
                break;
                    
                case 0x0A:
                    if (keyboardEf3_ == 1)
                    {
                        keyboardCode_ = '?';
                        keyboardEf2_ = 1;
                        keyboardEf3_ = 0;
                    }
                break;

                case 0x0B:
                    if (keyboardEf3_ == 1)
                    {
                        keyboardCode_ = 'h';
                        keyboardEf2_ = 1;
                        keyboardEf3_ = 0;
                    }
                break;

                case 0x0C:
                    if (keyboardEf3_ == 1)
                    {
                        keyboardCode_ = 'p';
                        keyboardEf2_ = 1;
                        keyboardEf3_ = 0;
                    }
                    break;

                case 0x0D:
                    if (keyboardEf3_ == 1)
                    {
                        keyboardCode_ = 'x';
                        keyboardEf2_ = 1;
                        keyboardEf3_ = 0;
                    }
                    break;

                case 0x0E:
                    if (keyboardEf3_ == 1)
                    {
                        keyboardCode_ = 0x80;
                        keyboardEf2_ = 1;
                        keyboardEf3_ = 0;
                    }
                break;

                case 0x2D:
                    if (keyboardEf3_ == 1)
                    {
                        keyboardCode_ = 0x58;
                        keyboardEf2_ = 1;
                        keyboardEf3_ = 0;
                    }
                break;
                    
                case 0x4D:
                    if (keyboardEf3_ == 1)
                    {
                        keyboardCode_ = 0x18;
                        keyboardEf2_ = 1;
                        keyboardEf3_ = 0;
                    }
                break;
                    
                default:
                break;
            }
        break;
	}
}

void Comx::cycle(int type)
{
	switch(cycleType_[type])
	{
		case 0:
			return;
		break;

//        case KEYBRDCYCLE:
//            cycleKeyboard();
//        break;
            
		case COMXCYCLE:
			cycleComx();
		break;

		case V1870CYCLE:
			cycle1870();
		break;

		case MC6845BLINK:
			blink6845();
		break;

		case MC6845CYCLE:
			cycle6845();
		break;

		case FDCCYCLE:
			cycleFdc();
		break;

        case NETWORKCYCLE:
            cycleNetwork();
        break;
            
//		case THERMALCYCLE:
//			p_PrinterThermal->cycleThermal();
//		break;
	}
}

void Comx::cycleComx()
{
	if (flipFlopQ_)
	{
		qMode_ = 1;
		if (dmaCounter_ == -100)
			dmaCounter_ = DMACYCLE;
	}

	if (debounceCounter_ > 0)
		debounceCounter_--;

	if (dmaCounter_ > 0)
	{
		dmaCounter_--;
		if (dmaCounter_ <= 0)
		{
			dmaOut();
			dmaCounter_ = DMACYCLE;
			if (!diagDmaLedOn_)
			{
				diagDmaLedOn_ = true;
				updateDiagLedStatus(2, diagDmaLedOn_);
			}
		}
	}

	if ((comxRunCommand_ != 0) && (keyboardEf3_ == 1))
	{
		if ((scratchpadRegister_[programCounter_] == 0x039a) || (scratchpadRegister_[programCounter_] == 0xeeb5) || (scratchpadRegister_[programCounter_] == 0x01bc) || (scratchpadRegister_[programCounter_] == 0x0193) || (scratchpadRegister_[programCounter_] == 0x5344) || (scratchpadRegister_[programCounter_] == 0xc10c))
		{
			if (comxRunCommand_ == 1)
			{
				keyboardCode_ = 3;
				keyboardEf3_ = 0;
				comxRunCommand_++;
			}
			else if (comxRunCommand_ == 2)
			{
				int saveExec = p_Main->pload();
				if (saveExec == 1)
					comxRunCommand_ = 0;
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
					keyboardCode_ = 0;
					keyboardEf3_ = 0;
					comxRunCommand_++;
				}
			}
			else
			{
				if (load_)
					comxRunCommand_ = 0;
				else
				{
					if ((comxRunCommand_ - 2) <= commandText_.Len())
					{
						keyboardCode_ = commandText_.GetChar(comxRunCommand_ - 3);
						keyboardEf3_ = 0;
						comxRunCommand_++;
					}
					else
					{
						keyboardCode_ = 128;
						keyboardEf3_ = 0;
						comxRunCommand_ = 0;
					}
				}
			}
		}
	}

	if ((comxKeyFileOpened_) && (keyboardEf3_ == 1))
	{
		if ((scratchpadRegister_[programCounter_] == 0xd2e0 && expansionSuper_[(epromBank_ * 0x2000) + 2] == 0xB0) // USB TENNIS
			|| (scratchpadRegister_[programCounter_] == 0xd11e && expansionSuper_[(epromBank_ * 0x2000) + 2] == 0xB0) // USB TENNIS
			|| (scratchpadRegister_[programCounter_] == 0xde10 && expansionSuper_[(epromBank_ * 0x2000) + 2] == 0xB0) // USB TENNIS
			|| (scratchpadRegister_[programCounter_] == 0xc2e8 && expansionSuper_[(epromBank_ * 0x2000) + 2] == 0xB0) // USB BROWSER
			|| (scratchpadRegister_[programCounter_] == 0xc5a5 && expansionSuper_[(epromBank_ * 0x2000) + 2] == 0xD0) // USB DMON
			|| (scratchpadRegister_[programCounter_] == 0xc6c0 && expansionSuper_[(epromBank_ * 0x2000) + 2] == 0xD0) // USB DMON
			|| (scratchpadRegister_[programCounter_] == 0xc4b9 && expansionSuper_[(epromBank_ * 0x2000) + 2] == 0xD0) // USB DMON
			|| (scratchpadRegister_[programCounter_] == 0xc38b && expansionSuper_[(epromBank_ * 0x2000) + 2] == 0xD0) // USB DMON
			|| (scratchpadRegister_[programCounter_] == 0xc45b && expansionSuper_[(epromBank_ * 0x2000) + 2] == 0xD0) // USB DMON
			|| (scratchpadRegister_[programCounter_] == 0xc8ba && expansionSuper_[(epromBank_ * 0x2000) + 2] == 0xD0) // USB DMON
			|| (scratchpadRegister_[programCounter_] == 0xd503 && expansionSuper_[(epromBank_ * 0x2000) + 2] == 0x90) // USB MON
			|| (scratchpadRegister_[programCounter_] == 0xc7b3 && expansionSuper_[(epromBank_ * 0x2000) + 2] == 0x90) // USB MON
			|| (scratchpadRegister_[programCounter_] == 0xd3e6 && expansionSuper_[(epromBank_ * 0x2000) + 2] == 0x90) // USB MON
			|| (scratchpadRegister_[programCounter_] == 0xc91d && expansionSuper_[(epromBank_ * 0x2000) + 2] == 0x90) // USB MON
			|| (scratchpadRegister_[programCounter_] == 0xcfad && expansionSuper_[(epromBank_ * 0x2000) + 2] == 0x90) // USB MON
			|| (scratchpadRegister_[programCounter_] == 0xd318 && expansionSuper_[(epromBank_ * 0x2000) + 2] == 0xF0) // USB SPACE
			|| (scratchpadRegister_[programCounter_] == 0xdd5a && expansionSuper_[(epromBank_ * 0x2000) + 2] == 0xF0) // USB SPACE
			|| (scratchpadRegister_[programCounter_] == 0xcb84 && expansionSuper_[(epromBank_ * 0x2000) + 2] == 0xD0) // USB WORM
			|| (scratchpadRegister_[programCounter_] == 0xd06e && expansionSuper_[(epromBank_ * 0x2000) + 2] == 0xD0) // USB WORM
			|| (scratchpadRegister_[programCounter_] == 0xb042)
			|| (scratchpadRegister_[programCounter_] == 0x039a)
			|| (scratchpadRegister_[programCounter_] == 0xeeb5)
			|| (scratchpadRegister_[programCounter_] == 0x01bc)
			|| (scratchpadRegister_[programCounter_] == 0x0193)
			|| (scratchpadRegister_[programCounter_] == 0x5344)
			|| (scratchpadRegister_[programCounter_] == 0xc10c))
		{
			if (comxKeyFile_.Read(&keyboardCode_, 1) == 0)
			{
				comxKeyFileOpened_ = false;
				comxKeyFile_.Close();
			}
			else
			{
				if (keyboardCode_ == 13) keyboardCode_ = 128;
				if (keyboardCode_ == 10 && lastKeyCode_ == 128) keyboardCode_ = 0;
				if (keyboardCode_ == 0xb6)  keyboardCode_ = 1;
				if (keyboardCode_ >= 'A' && keyboardCode_ <= 'Z')
					keyboardCode_ += 32;
				else
					if (keyboardCode_ >= 'a' && keyboardCode_ <= 'z')
						keyboardCode_ -= 32;

				lastKeyCode_ = keyboardCode_;
				keyboardEf3_ = 0;
			}
		}
	}
}

void Comx::cycleKeyboard()
{
    if (keyCycles_ > 0 && keyboardEf3_ == 1)
        keyCycles_--;
    if (rawKeyCode_<0)
        rawKeyCode_ = 0;
    if (rawKeyCode_ != 0)
    {
        keyboardCode_ = rawKeyCode_;
//        keyLogFileCycle_.Write(&keyboardCode_, 1);
        switch(keyboardCode_)
        {
            case WXK_RETURN:
                keyboardCode_ = 0x80;
            break;
            case WXK_NUMPAD_ENTER:
                keyboardCode_ = 0x80;
            break;
            case WXK_ESCAPE:
                keyboardCode_ = 0x81;
            break;
            case WXK_BACK:
                keyboardCode_ = 0x86;
            break;
            case WXK_DELETE:
                keyboardCode_ = 0x86;
            break;
            case WXK_LEFT:
                keyboardCode_ = 0x84;
            break;
            case WXK_RIGHT:
                keyboardCode_ = 0x83;
            break;
            case WXK_UP:
                keyboardCode_ = 0x82;
            break;
            case WXK_DOWN:
                keyboardCode_ = 0x85;
            break;
            case '@':keyboardCode_ = 0x20; break;
            case '#':keyboardCode_ = 0x23; break;
            case '\'': keyboardCode_ = 0x27; break;
            case '[':keyboardCode_ = 0x28; break;
            case ']':keyboardCode_ = 0x29; break;
            case ':':keyboardCode_ = 0x2a; break;
            case ';':keyboardCode_ = 0x2b; break;
            case '<':keyboardCode_ = 0x2c; break;
            case '=':keyboardCode_ = 0x2d; break;
            case '>':keyboardCode_ = 0x2e; break;
            case '\\':keyboardCode_ = 0x2f; break;
            case '.':keyboardCode_ = 0x3a; break;
            case ',':keyboardCode_ = 0x3b; break;
            case '(':keyboardCode_ = 0x3c; break;
            case '^':keyboardCode_ = 0x3d; break;
            case ')':keyboardCode_ = 0x3e; break;
            case '_':keyboardCode_ = 0x3f; break;
            case '?':keyboardCode_ = 0x40; break;
            case '+':keyboardCode_ = 0x5b; break;
            case '-':keyboardCode_ = 0x5c; break;
            case '*':keyboardCode_ = 0x5d; break;
            case '/':keyboardCode_ = 0x5e; break;
            case ' ':keyboardCode_ = 0x5f; break;
        }
        if (keyboardCode_ >= 0x90)  keyboardCode_ &= 0x7f;
        rawKeyCode_ = 0;
        keyboardEf3_ = 0;
    }
}

void Comx::startComputer()
{
	resetPressed_ = false;

	startComxKeyFile();
	p_Main->setSwName("");
	p_Main->updateTitle();

	fAndMBasicRunning_ = false;

	allocComxExpansionMemory();
    
    p_Main->checkAndReInstallMainRom(COMX);
	readProgram(p_Main->getRomDir(COMX, MAINROM1), p_Main->getRomFile(COMX, MAINROM1), ROM, 0, NONAME);

	if (p_Main->isDiagActive(COMX))
	{
		diagRomActive_ = true;
        p_Main->checkAndReInstallFile(COMX, "EXP ROM", EXPROM);
		readProgram(p_Main->getRomDir(COMX, EXPROM), p_Main->getRomFile(COMX, EXPROM), DIAGROM, 0, NONAME);
	}
    else
    {
        p_Main->checkAndReInstallFile(COMX, "EXP ROM", EXPROM);
        readProgram(p_Main->getRomDir(COMX, EXPROM), p_Main->getRomFile(COMX, EXPROM), COMXEXPROM, 0xE000, NONAME);
    }
    
	if (p_Main->isDiagOn(COMX) == 1)
		diagRomActive_ = true;
	else
		diagRomActive_ = false;
    
	diagDmaLedOn_ = false;
	expansionRomLoaded_ = false;
	if (mainMemory_[0xE000] != 0x0)
	{
		defineMemoryType(0x1000, 0x17ff, COPYCOMXEXPROM);
		expansionRomLoaded_ = true;
	}
	else
	{
		if (mainMemory_[0xE001] == 0x82)
			expansionRomLoaded_ = true;
	}

	p_Main->setExtRomStatus(expansionRomLoaded_);

	if (videoMode_ == NTSC)
		if (mainMemory_[0xEE8F] == 0xc8)
			mainMemory_[0xEE8F] = 0xc7;

	expansionSlot_ = 0;
	defineExpansionMemoryType(0, 0, 0x1fff, ROM);
    if (!((p_Main->getExpansionRamSlot()-1) == 0) && useExpansionRam_)
        readProgram(p_Main->getRomDir(COMX, CARTROM1), p_Main->getRomFile(COMX, CARTROM1), COMXEXPBOX, 0xC000, NONAME);
	configure1870Comx(expansionRomLoaded_, expansionRom_[1]);

	Show(true);

	configureExpansion();
	loadRam();

	defineMemoryType(0x4000, 0xbfff, RAM);
    initRam(0x4000, 0xbfff);
	defineMemoryType(0xf400, 0xf7ff, CRAM1870);
	defineMemoryType(0xf800, 0xffff, PRAM1870);

    p_Main->assDefault("mycode", 0x4400, 0x53FF);

	reDrawBar();
	updateExpansionLed(true);
	p_Main->enableDebugGuiMemory();

	if (diskRomLoaded_)
	{
		wxString fileName = p_Main->getFloppyFile(0);
		if (fileName.Len() == 0)
			setDiskName(1, "");
		else
			setDiskName(1, p_Main->getFloppyDir(0)+p_Main->getFloppyFile(0));
		fileName = p_Main->getFloppyFile(1);
		if (fileName.Len() == 0)
			setDiskName(2, "");
		else
			setDiskName(2, p_Main->getFloppyDir(1)+p_Main->getFloppyFile(1));
	}
	p_Main->enableDiskRomGui(diskRomLoaded_);

	cpuCycles_ = 0;
	p_Main->startTime();

	day_ = comxTime_.GetDay();
	month_ = comxTime_.GetMonth();
	year_ = comxTime_.GetYear();

    updateDiagLedStatus(1, diagRomActive_);
    updateDiagLedStatus(2, diagDmaLedOn_);
    if (threadPointer->Run() != wxTHREAD_NO_ERROR )
	{
		p_Main->message("Can't start thread!");
	}
}

void Comx::writeMemDataType(Word address, Byte type)
{
	switch (memoryType_[address/256])
	{
		case COMXEXPBOX:
			switch (expansionMemoryType_[expansionSlot_*32 + (address & 0x1fff)/256])
			{
				case RAMBANK:
					switch (bankMemoryType_[ramBank_*32 + (address & 0x1fff)/256])
					{
						case ROM:
						case RAM:
							if (expansionRamDataType_[(ramBank_*0x2000) + (address & 0x1fff)] != type)
							{
								p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
								expansionRamDataType_[(ramBank_*0x2000) + (address & 0x1fff)] = type;
							}
						break;
					}
				break;

				case EPROMBANK:
					switch (epromBankMemoryType_[epromBank_*32 + (address & 0x1fff)/256])
					{
						case ROM:
						case RAM:
							if (expansionEpromDataType_[(epromBank_*0x2000) + (address & 0x1fff)] != type)
							{
								p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
								expansionEpromDataType_[(epromBank_*0x2000) + (address & 0x1fff)] = type;
							}
						break;
					}
				break;

				case SUPERBANK:
					switch (superBankMemoryType_[(epromBank_+(8*ramSwitched_))*32 + (address & 0x1fff)/256])
					{
						case ROM:
						case RAM:
							if (expansionSuperDataType_[((epromBank_+(8*ramSwitched_))*0x2000) + (address & 0x1fff)] != type)
							{
								p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
								expansionSuperDataType_[((epromBank_+(8*ramSwitched_))*0x2000) + (address & 0x1fff)] = type;
							}
						break;
					}
				break;

				case ROM:
					if (expansionRomDataType_[(expansionSlot_*0x2000) + (address & 0x1fff)] != type)
					{
						p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
						expansionRomDataType_[(expansionSlot_*0x2000) + (address & 0x1fff)] = type;
					}
				break;
			}
		break;

		case PRAM1870:
			if (epromSlot_ != 0xff || superSlot_ != 0xff)
			{
				if (mainMemoryDataType_[address] != type)
				{
					p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
					mainMemoryDataType_[address] = type;
				}
			}
		break;

		case COMXEXPROM:
        case ROM:
        case DIAGROM:
        case RAM:
		case NVRAM:
			if (mainMemoryDataType_[address] != type)
			{
				p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
				mainMemoryDataType_[address] = type;
			}
		break;

		case COPYCOMXEXPROM:
			if (mainMemoryDataType_[(address & 0xfff) | 0xe000] != type)
			{
				p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
				mainMemoryDataType_[(address & 0xfff) | 0xe000] = type;
			}
		break;

		case COPYFLOPROM: 
			if (((address & 0xff) >= 0xd0) &&((address & 0xff) <= 0xdf))
			{
                if (expansionSlot_ == fdcSlot_)
                {
                    if (expansionRomDataType_[(expansionSlot_*0x2000) + (address & 0xfff)] != type)
                    {
                        p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                        expansionRomDataType_[(expansionSlot_*0x2000) + (address & 0xfff)] = type;
                    }
                }
                if (expansionSlot_ == networkSlot_)
                {
                    if (mainMemoryDataType_[address + 0xE000] != type)
                    {
                        p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                        mainMemoryDataType_[address + 0xE000] = type;
                    }
                }
			}
			else
			{
				if (mainMemoryDataType_[address] != type)
				{
					p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
					mainMemoryDataType_[address] = type;
				}
			}
		break;
	}
}

Byte Comx::readMemDataType(Word address)
{
	switch (memoryType_[address/256])
	{
		case COMXEXPBOX:
			switch (expansionMemoryType_[expansionSlot_*32 + (address & 0x1fff)/256])
			{
				case RAMBANK:
					switch (bankMemoryType_[ramBank_*32 + (address & 0x1fff)/256])
					{
						case ROM:
						case RAM:
							return expansionRamDataType_[(ramBank_*0x2000) + (address & 0x1fff)];
						break;
					}
				break;

				case EPROMBANK:
					switch (epromBankMemoryType_[epromBank_*32 + (address & 0x1fff)/256])
					{
						case ROM:
						case RAM:
							return expansionEpromDataType_[(epromBank_*0x2000) + (address & 0x1fff)];
						break;
					}
				break;

				case SUPERBANK:
					switch (superBankMemoryType_[(epromBank_+(8*ramSwitched_))*32 + (address & 0x1fff)/256])
					{
						case ROM:
						case RAM:
							return expansionSuperDataType_[((epromBank_+(8*ramSwitched_))*0x2000) + (address & 0x1fff)];
						break;
					}
				break;

				case ROM:
				case RAM:
					return expansionRomDataType_[(expansionSlot_*0x2000) + (address & 0x1fff)];
				break;
			}
		break;

		case ROM:
        case DIAGROM:
		case NVRAM:
		case RAM:
		case COMXEXPROM:
			return mainMemoryDataType_[address];
		break;

		case PRAM1870:
			if (epromSlot_ != 0xff || superSlot_ != 0xff)
				return mainMemoryDataType_ [address];
			else
				return MEM_TYPE_UNDEFINED;
		break;

		case COPYCOMXEXPROM:
			return mainMemoryDataType_[(address & 0xfff) | 0xe000];
		break;

		case COPYFLOPROM:
            if (expansionSlot_ == fdcSlot_)
            {
                if (((address & 0xff) >= 0xd0) &&((address & 0xff) <= 0xdf))
                    return expansionRomDataType_[(expansionSlot_*0x2000) + (address & 0xfff)];
            }
            if (expansionSlot_ == networkSlot_)
            {
                if (((address & 0xff) >= 0xd0) &&((address & 0xff) <= 0xdf))
                    return mainMemoryDataType_[address + 0xE000];
            }
            return mainMemoryDataType_[address];
		break;
	}			
	return MEM_TYPE_UNDEFINED;
}

Byte Comx::readMem(Word address)
{
	address_ = address;
	wxDateTime systemNow;
	wxDateTime now;
	wxTimeSpan timeDiff;
	Byte value;
	int year;
	Byte high, low, rtcControl;
	
	switch (memoryType_[address/256])
	{
		case UNDEFINED:
			return 255;
		break;

		case PRAM1870:
			return readPram(address);
		break;

		case CRAM1870:
			return readCram(address);
		break;

        case DIAGROM:
            if (diagRomActive_)
                return diagRomReplacement_[address];
            else
                return mainMemory_[address];
        break;
            
        case COMXEXPBOX:
			switch (expansionMemoryType_[expansionSlot_*32 + (address & 0x1fff)/256])
			{
				case RAMBANK:
					switch (bankMemoryType_[ramBank_*32 + (address & 0x1fff)/256])
					{
						case ROM:
						case RAM:
							return expansionRam_[(ramBank_*0x2000) + (address & 0x1fff)];
						break;

						default:
							return 255;
						break;
					}
				break;

				case EPROMBANK:
					switch (epromBankMemoryType_[epromBank_*32 + (address & 0x1fff)/256])
					{
						case ROM:
						case RAM:
							return expansionEprom_[(epromBank_*0x2000) + (address & 0x1fff)];
						break;

						default:
							return 255;
						break;
					}
				break;

				case SUPERBANK:
					switch (superBankMemoryType_[(epromBank_+(8*ramSwitched_))*32 + (address & 0x1fff)/256])
					{
						case ROM:
						case RAM:
							return expansionSuper_[((epromBank_+(8*ramSwitched_))*0x2000) + (address & 0x1fff)];
						break;

						default:
							return 255;
						break;
					}
				break;

				case RAM:
				case ROM:
					return expansionRom_[(expansionSlot_*0x2000) + (address & 0x1fff)];
				break;

				case MC6845RAM:
					return read6845(address & 0x7ff);
				break;

				case MC6845REGISTERS:
					if ((address & 0xff) == 0x01)
						return readData6845();
					else
						return 255;
				break;

				default:
					return 255;
				break;
			}
		break;

		case NVRAM:
//			return 0xff;
//		break;

			if (address > 0xf3f8)
			{
				if ((mainMemory_[0xf3f8]&0x40) != 0x40)
				{
					rtcControl = 0;
					systemNow = wxDateTime::Now();
					timeDiff = systemNow.Subtract(systemTime_);

					now = comxTime_;
					now.Add(timeDiff);
					switch (address)
					{
				 		case 0xf3f9:
							value = now.GetSecond();
							rtcControl = mainMemory_[address] & 0x80;
						break;
						case 0xf3fa:
							value = now.GetMinute();
						break;
						case 0xf3fb:
							value = now.GetHour();
						break;
						case 0xf3fc:
							value = now.GetWeekDay();
							rtcControl = mainMemory_[address] & 0x70;
						break; 
						case 0xf3fd:
							value = now.GetDay();
							rtcControl = mainMemory_[address] & 0xc0;
						break;
						case 0xf3fe:
							value = now.GetMonth()+1;
						break;
						case 0xf3ff:
							year = now.GetYear();
							value = year % 100;
						break;
						default:
							value = 0;
						break;
					}
					high = (int)(value/10)*16;
					low = value - (int)(value/10)*10;
					mainMemory_[address] = high + low + rtcControl;
				}
			}
			return mainMemory_[address];
		break;

		case RAM:
		case ROM:
		case COMXEXPROM:
			return mainMemory_[address];
		break;

		case COPYCOMXEXPROM:
			return mainMemory_[(address & 0xfff) | 0xe000];
		break;

		case COPYFLOPROM:
			if (((address & 0xff) >= 0xd0) &&((address & 0xff) <= 0xdf))
			{
				if (expansionSlot_ == fdcSlot_)
					return expansionRom_[(fdcSlot_*0x2000) + (address & 0xfff)];
                if (expansionSlot_ == networkSlot_)
                    return mainMemory_[address + 0xE000];
			}
			return mainMemory_[address];
		break;

		default:
			return 255;
		break;
	}
}

void Comx::writeMem(Word address, Byte value, bool writeRom)
{
	address_ = address;
	wxDateTime systemNow;
	wxDateTime now;
	wxTimeSpan timeDiff;
	Byte high, low;

	switch (memoryType_[address/256])
	{
		case UNDEFINED:
		case COMXEXPROM:
		case COPYCOMXEXPROM:
        case COPYFLOPROM:
		case ROM:
			if (writeRom)
				mainMemory_[address]=value;
		break;

        case DIAGROM:
            if (writeRom)
            {
                if (diagRomActive_)
                    diagRomReplacement_[address]=value;
                else
                    mainMemory_[address]=value;
            }
        break;
            
		case COMXEXPBOX:
			switch (expansionMemoryType_[expansionSlot_*32 + (address & 0x1fff)/256])
			{
				case RAMBANK:
					switch (bankMemoryType_[ramBank_*32 + (address & 0x1fff)/256])
					{
						case ROM:
							if (writeRom)
								expansionRam_[(ramBank_*0x2000) + (address & 0x1fff)] = value;
						break;

						case RAM:
							expansionRam_[(ramBank_*0x2000) + (address & 0x1fff)] = value;
						break;
					}
				break;

				case EPROMBANK:
					switch (epromBankMemoryType_[epromBank_*32 + (address & 0x1fff)/256])
					{
						case ROM:
							if (writeRom)
								expansionEprom_[(epromBank_*0x2000) + (address & 0x1fff)] = value;
						break;

						case RAM:
							expansionEprom_[(epromBank_*0x2000) + (address & 0x1fff)] = value;
						break;
					}
				break;

				case SUPERBANK:
					switch (superBankMemoryType_[(epromBank_+(8*ramSwitched_))*32 + (address & 0x1fff)/256])
					{
						case ROM:
							if (writeRom)
								expansionSuper_[((epromBank_+(8*ramSwitched_))*0x2000) + (address & 0x1fff)] = value;
						break;

						case RAM:
							expansionSuper_[((epromBank_+(8*ramSwitched_))*0x2000) + (address & 0x1fff)] = value;
						break;
					}
				break;

				case ROM:
					if (writeRom)
						expansionRom_[(expansionSlot_*0x2000) + (address & 0x1fff)] = value;
				break;

				case RAM:
					expansionRom_[(expansionSlot_*0x2000) + (address & 0x1fff)] = value;
//                    if (address == 0xdff8)
//                        p_Main->messageHex(scratchpadRegister_[programCounter_]);
				break;

				case MC6845RAM:
					write6845(address & 0x7ff, value);
				break;

				case MC6845REGISTERS:
					if ((address & 0xff) == 0)
						writeRegister6845(value);
					if ((address & 0xff) == 1)
						writeData6845(value);
				break;
			}
			if (address>= memoryStart_ && address<(memoryStart_+256))
				p_Main->updateDebugMemory(address);
			p_Main->updateAssTabCheck(address);
		break;

		case PRAM1870:
			if (writeRom)
				mainMemory_[address]=value;
			else
				writePram(address, value);
		break;

		case CRAM1870:
			writeCram(address, value);
		break;

		case NVRAM:
			if (address == 0xf3f8)
			{
				if (value == 0)
				{
					systemNow = wxDateTime::Now();
					timeDiff = systemNow.Subtract(systemTime_);

					now = comxTime_;
					now.Add(timeDiff);

					now.SetDay(1);
					now.SetYear(year_);
					now.SetMonth(wxDateTime::Month(month_));
					now.SetDay(day_);

					comxTime_ = now.Subtract(timeDiff);
				}
			}
			if (nvramWriteProtected_)
				return;
			if (address > 0xf3f8 && address != 0xf3fc)
			{
				if ((mainMemory_[0xf3f8]&0x80) == 0x80)
				{
					mainMemory_[address]=value;
					switch (address)
					{
						case 0xf3f9:
							value &= 0x7f;
						break;
						case 0xf3fa:
							value &= 0x7f;
						break;
						case 0xf3fb:
							value &= 0x3f;
						break;
						case 0xf3fd:
							value &= 0x3f;
						break;
						case 0xf3fe:
							value &= 0x1f;
						break;
					}
					high = ((value&0xf0)/16)*10;
					low = value&0xf;
					value = high + low;

					systemNow = wxDateTime::Now();
					timeDiff = systemNow.Subtract(systemTime_);

					now = comxTime_;
					now.Add(timeDiff);

					switch (address)
					{
						case 0xf3f9:
							now.SetSecond(value);
						break;
						case 0xf3fa:
				 			now.SetMinute(value);
						break;
						case 0xf3fb:
							now.SetHour(value);
						break;   
						case 0xf3fd:
							day_=value;
						break;   
						case 0xf3fe:
							month_=value-1;
						break;   
						case 0xf3ff:
							year_=value+2000;
						break;   
					}
					comxTime_ = now.Subtract(timeDiff);
					return;
				}
			}
			if (mainMemory_[address]==value)
				return;
			mainMemory_[address]=value;
			if (address>= memoryStart_ && address<(memoryStart_+256))
				p_Main->updateDebugMemory(address);
			p_Main->updateAssTabCheck(address);
		break;

		case RAM:
			switch (address)
			{
				case 0x428b:
					switch(value)
					{
						case 0x53:
							fAndMBasicRunning_ = true;
							p_Main->eventSetFandMBasicGui();
						break;

						case 0xc1:
							thermalPrinting_ = true;
						break;

						case 0x14:
							stop6845();
//							if (fAndMBasicRunning_ && comxKeyFileOpened_)
//								keyboardEf3_ = 0;
							fAndMBasicRunning_ = false;
							p_Main->eventSetFandMBasicGui();
							if (expansionSlot_ == printerSlot_)
							{
								p_Main->eventPrintParallelFinished();
							}
							if (expansionSlot_ == serialSlot_)
							{
								p_Main->eventPrintSerialFinished();
							}
							if (expansionSlot_ == thermalSlot_)
							{
								p_Main->eventPrintThermalFinished();
							}
							if (out1value_ == 0x50)
							{
								if (printerSlot_ != 0xff)
									p_Main->eventPrintParallelFinished();
								else
								{
									if (serialSlot_ != 0xff)
										p_Main->eventPrintSerialFinished();
									else
									{
										if (thermalSlot_ != 0xff)
											p_Main->eventPrintThermalFinished();
									}
								}
							}
						break;
					}
				break;

/*				case 0x41a3:
					if (value == 0 && comxKeyFileOpened_ && !fAndMBasicRunning_ && comxRunState_ != LISTSTATE)
					{
						keyboardEf3_ = 0;
					}
				break;*/
			}

//			if (address == 0x42a0)
//				p_Main->messageInt(value);
//			if (address == 0x428b)
//			{
//				p_Main->messageHex(scratchpadRegister_[programCounter_]);
//			}

			if (mainMemory_[address]==value)
				return;
			mainMemory_[address]=value;
			if (address>= memoryStart_ && address<(memoryStart_+256))
				p_Main->updateDebugMemory(address);
			p_Main->updateAssTabCheck(address);
		break;
	}
}

void Comx::cpuInstruction()
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
	checkComxFunction();

	if (resetPressed_)
	{
		stop6845();
		out5_1870(0x0080);
		//mc6845started_ = false;
		setCycleType(BLINKCYCLE, 0);
		setCycleType(VIDEOCYCLE, 0);
		startComxKeyFile();
		if (expansionRomLoaded_)
			out(1, 0, 0x10);
		resetCpu();
		init1870();
        initComputer();
        
        p_Main->v1870BarSizeEvent();
		writeMem(0xbf42, 0, false);
		writeMem(0xbf44, 0, false);
		p_Main->setSwName("");
        p_Main->eventUpdateTitle();
		comxRunCommand_ = 0;
		resetPressed_ = false;
		if (p_Main->isDiagOn(COMX) == 1)
			diagRomActive_ = true;
		else
			diagRomActive_ = false;
        updateDiagLedStatus(1, diagRomActive_);
		diagDmaLedOn_ = false;
		updateDiagLedStatus(2, diagDmaLedOn_);
        updateDiagLedStatus(5, false);
	}
	if (debugMode_)
		p_Main->cycleDebug();
}

void Comx::charEvent(int keycode)
{
//	if (keyboardEf3_ == 0)  return;
//
	keyboardCode_ = keycode;
	keyboardEf3_ = 0;
//    rawKeyCode_ = keycode;
//    interrupt();
//    keyLogFilePc_.Write(&keyboardCode_, 1);
}

bool Comx::keyDownExtended(int keycode, wxKeyEvent& event)
{
//	if (keyboardEf3_ == 0)  return true;
	previousKeyCode_ = (wxKeyCode) keycode;

 	switch(keycode)
	{
		case WXK_RETURN:
			keyboardCode_ = 0x80;
			keyboardEf3_ = 0;
    //        keyLogFilePc_.Write(&keyboardCode_, 1);
			return true;
		break;

		case WXK_NUMPAD_ENTER:
			keyboardCode_ = 0x80;
			keyboardEf3_ = 0;
    //        keyLogFilePc_.Write(&keyboardCode_, 1);
			return true;
		break;

		case WXK_ESCAPE:
			keyboardCode_ = 0x81;
			keyboardEf3_ = 0;
     //       keyLogFilePc_.Write(&keyboardCode_, 1);
			return true;
		break;

		case WXK_BACK:
			keyboardCode_ = 0x86;
			keyboardEf3_ = 0;
     //       keyLogFilePc_.Write(&keyboardCode_, 1);
			return true;
		break;

		case WXK_DELETE:
			keyboardCode_ = 0x86;
			keyboardEf3_ = 0;
      //      keyLogFilePc_.Write(&keyboardCode_, 1);
			return true;
		break;

		case WXK_LEFT:
			keyboardCode_ = 0x84;
			keyboardEf3_ = 0;
      //      keyLogFilePc_.Write(&keyboardCode_, 1);
			return true;
		break;

		case WXK_RIGHT:
			keyboardCode_ = 0x83;
			keyboardEf3_ = 0;
      //      keyLogFilePc_.Write(&keyboardCode_, 1);
			return true;
		break;

		case WXK_UP:
			keyboardCode_ = 0x82;
			keyboardEf3_ = 0;
       //     keyLogFilePc_.Write(&keyboardCode_, 1);
			return true;
		break;

		case WXK_DOWN:
			keyboardCode_ = 0x85;
			keyboardEf3_ = 0;
     //       keyLogFilePc_.Write(&keyboardCode_, 1);
			return true;
		break;

		case WXK_NUMPAD_ADD:
			if (event.GetModifiers() == wxMOD_SHIFT)
				keyboardCode_ = 0xfb;
			else
				keyboardCode_ = 0xdb;
			keyboardEf3_ = 0;
      //      keyLogFilePc_.Write(&keyboardCode_, 1);
			return true;
		break;

		case WXK_NUMPAD_SUBTRACT:
			if (event.GetModifiers() == wxMOD_SHIFT)
				keyboardCode_ = 0xfc;
			else
				keyboardCode_ = 0xdc;
			keyboardEf3_ = 0;
      //      keyLogFilePc_.Write(&keyboardCode_, 1);
			return true;
		break;
	}
	return false;
}

void Comx::keyUp(int WXUNUSED(keycode))
{
    if (p_Main->isDiagOn(COMX) == 1)
    {
        if ((outValues_[1]) != 0)
            return;
    }
	keyboardEf2_ = 1;
//	keyboardEf3_ = 1;
//	keyboardCode_ = 0;
	previousKeyCode_ = (wxKeyCode) 0;
}

void Comx::keyClear()
{
	keyboardEf2_ = 1;
	keyboardEf3_ = 1;
	keyboardCode_ = 0;
	previousKeyCode_ = (wxKeyCode) 0;
}

void Comx::checkComxFunction()
{
#if defined(__WXMSW__ )
	bool maximize;
#endif
	switch (scratchpadRegister_[programCounter_])
	{
		case 0x1031:
			if (isSuperBoardLoaded())
			{
				if (mainMemory_[0xea91] >= 0x32)
				{
					p_Main->stopCassette();
					if (mainMemory_[0x428b] == 0x14)
						thermalPrinting_ = false;
	#if defined(__WXMSW__ )
					maximize = IsMaximized();
					Iconize(false);
					Raise();
					Show(true);
					Maximize(maximize);
	#endif
				}
			}
		break;


		case 0x1047:
			p_Main->stopCassette();
			if (mainMemory_[0x428b] == 0x14)
				thermalPrinting_ = false;
	#if defined(__WXMSW__ )
			maximize = IsMaximized();
			Iconize(false);
			Raise();
			Show(true);
			Maximize(maximize);
	#endif
		break;

		case 0x01bc:	// Reset input
			if (comxRunState_ != RESETSTATE)
			{
				comxRunState_ = RESETSTATE;
			}
		break;

		case 0x0193:	// Reset input
			if (isSuperBoardLoaded())
			{
				if (comxRunState_ != RESETSTATE)
				{
					comxRunState_ = RESETSTATE;
				}
			}
		break;

		case 0xeeb5:	// Basic input EXP BOX
			if (comxRunState_ != BASICSTATE)
				if (mainMemory_[0x42a0] == 0)
				{
					comxRunState_ = BASICSTATE;
				}
		break;

		case 0xc10c:	// Basic input 80 COL
			if (comxRunState_ != BASICSTATE)
				if (mainMemory_[0x42a0] == 0)
				{
					comxRunState_ = BASICSTATE;
				}
		break;

		case 0x039a:	// Basic input
			if (comxRunState_ != BASICSTATE)
				if (mainMemory_[0x42a0] == 0)
				{
					comxRunState_ = BASICSTATE;
				}
		break;

		case 0x5344:	// F&M BASIC input
			if (comxRunState_ != BASICSTATE)
				if (mainMemory_[0x42a0] == 0)
				{
					comxRunState_ = BASICSTATE;
				}
		break;

		case 0x1F76:	// RUN
			if (comxRunState_ != RUNSTATE)
				comxRunState_ = RUNSTATE;
		break;

		case 0x2c00:	// CALL
			if (comxRunState_ != RUNSTATE)
				comxRunState_ = RUNSTATE;
		break;

		case 0x1672:	// PSAVE
		case 0x1675:	// DSAVE
			p_Main->startCassetteSave(0);
		break;

		case 0x4dc3:	// BSAVE
			if (fAndMBasicRunning_)
				p_Main->startCassetteSave(0);
		break;

		case 0xbf00:	// TURBO PSAVE V1.00
			if ((mainMemory_[0xbfd0] == 0x54) &&(mainMemory_[0xbfd1] == 0x41) &&(mainMemory_[0xbf02] == 0xc3) &&(mainMemory_[0xbfac] == 0x91))
				p_Main->startCassetteSave(0);
		break;

		case 0xbee7:	// TURBO PSAVE V2.00
			if ((mainMemory_[0xbfe3] == 0x54) &&(mainMemory_[0xbfe4] == 0x41) &&(mainMemory_[0xbe93] == 0x4c) &&(mainMemory_[0xbe94] == 0x4f))
				p_Main->startCassetteSave(0);
		break;

		case 0xbae7:	// TOS PSAVE V3.00
			if ((mainMemory_[0xbd52] == 0x54) &&(mainMemory_[0xbd53] == 0x4f) &&(mainMemory_[0xb934] == 0x3a) &&(mainMemory_[0xb935] == 0xb6))
				p_Main->startCassetteSave(0);
		break;

		case 0xbce1:	// TOS DSAVE V3.00
			if ((mainMemory_[0xbd52] == 0x54) &&(mainMemory_[0xbd53] == 0x4f) &&(mainMemory_[0xb934] == 0x3a) &&(mainMemory_[0xb935] == 0xb6))
				p_Main->startCassetteSave(0);
		break;

		case 0xcbe2:	// TURBO PSAVE @c800
		case 0xcde2: 	// TURBO PSAVE+ @c800
		case 0xccb1:	// TURBO DSAVE @c820
		case 0xceb1:	// TURBO DSAVE+ @c820
			if (expansionSlot_ == printerSlot_)
				p_Main->startCassetteSave(0);
			if (expansionSlot_ == epromSlot_)
			{
				if (expansionEprom_[(epromBank_*0x2000) + 0x804] == 0x50 && expansionEprom_[(epromBank_*0x2000) + 0x805] == 0x53 && expansionEprom_[(epromBank_*0x2000) + 0x806] == 0x41 && expansionEprom_[(epromBank_*0x2000) + 0x807] == 0x56)
					p_Main->startCassetteSave(0);
				if (expansionSuper_[(epromBank_*0x2000) + 0x804] == 0x50 && expansionSuper_[(epromBank_*0x2000) + 0x805] == 0x53 && expansionSuper_[(epromBank_*0x2000) + 0x806] == 0x41 && expansionSuper_[(epromBank_*0x2000) + 0x807] == 0x56)
					p_Main->startCassetteSave(0);
			}
		break;

		case 0xdea0:	// USB TSAVE
		case 0xdea3:	// USB DTSAVE 
			if (expansionSlot_ == superSlot_ && epromBank_ == 3)
				p_Main->startCassetteSave(0);
		break;

		case 0xdea6: 	// USB TLOAD
		case 0xdea9:	// USB DTLOAD
			if (expansionSlot_ == superSlot_ && epromBank_ == 3)
			{
				p_Main->setSwName ("");
                p_Main->eventUpdateTitle();
				p_Main->startCassetteLoad(0);
			}
		break;

		case 0x0e00:	// PLOAD
			p_Main->setSwName ("");
            p_Main->eventUpdateTitle();
			p_Main->startCassetteLoad(0);
		break;

		case 0x0e03:	// DLOAD
			p_Main->startCassetteLoad(0);
		break;

		case 0x4daa:	// BLOAD
			if (fAndMBasicRunning_)
				p_Main->startCassetteLoad(0);
		break;

		case 0xbf70:	// TURBO PLOAD V1.00
			if ((mainMemory_[0xbfd0] == 0x54) &&(mainMemory_[0xbfd1] == 0x41) &&(mainMemory_[0xbf02] == 0xc3) &&(mainMemory_[0xbfac] == 0x91))
			{
				p_Main->setSwName ("");
                p_Main->eventUpdateTitle();
				p_Main->startCassetteLoad(0);
			}
		break;

		case 0xbf78:	// TURBO PLOAD V2.00
			if ((mainMemory_[0xbfe3] == 0x54) &&(mainMemory_[0xbfe4] == 0x41) &&(mainMemory_[0xbe93] == 0x4c) &&(mainMemory_[0xbe94] == 0x4f))
			{
				p_Main->setSwName ("");
                p_Main->eventUpdateTitle();
				p_Main->startCassetteLoad(0);
			}
		break;

		case 0xbb78:	// TOS PLOAD V3.00
			if ((mainMemory_[0xbd52] == 0x54) &&(mainMemory_[0xbd53] == 0x4f) &&(mainMemory_[0xb934] == 0x3a) &&(mainMemory_[0xb935] == 0xb6))
			{
				p_Main->setSwName ("");
                p_Main->eventUpdateTitle();
				p_Main->startCassetteLoad(0);
			}
		break;

		case 0xbcf4:	// TOS DLOAD V3.00
			if ((mainMemory_[0xbd52] == 0x54) &&(mainMemory_[0xbd53] == 0x4f) &&(mainMemory_[0xb934] == 0x3a) &&(mainMemory_[0xb935] == 0xb6))
				p_Main->startCassetteLoad(0);
		break;

		case 0xcc41:	// TURBO PLOAD @c810
		case 0xce41: 	// TURBO PLOAD+ @c810
			if (expansionSlot_ == printerSlot_)
			{
				p_Main->setSwName ("");
                p_Main->eventUpdateTitle();
				p_Main->startCassetteLoad(0);
			}
			if (expansionSlot_ == epromSlot_)
			{
				if (expansionEprom_[(epromBank_*0x2000) + 0x804] == 0x50 && expansionEprom_[(epromBank_*0x2000) + 0x805] == 0x53 && expansionEprom_[(epromBank_*0x2000) + 0x806] == 0x41 && expansionEprom_[(epromBank_*0x2000) + 0x807] == 0x56)
				{
					p_Main->setSwName("");
                    p_Main->eventUpdateTitle();
					p_Main->startCassetteLoad(0);
				}
				if (expansionSuper_[(epromBank_*0x2000) + 0x804] == 0x50 && expansionSuper_[(epromBank_*0x2000) + 0x805] == 0x53 && expansionSuper_[(epromBank_*0x2000) + 0x806] == 0x41 && expansionSuper_[(epromBank_*0x2000) + 0x807] == 0x56)
				{
					p_Main->setSwName ("");
                    p_Main->eventUpdateTitle();
					p_Main->startCassetteLoad(0);
				}
			}
		break;

		case 0xcd00:	// TURBO DLOAD @c830
		case 0xcf00:	// TURBO DLOAD+ @c830
			if (expansionSlot_ == printerSlot_)
				p_Main->startCassetteLoad(0);
			if (expansionSlot_ == epromSlot_)
			{
				if (expansionEprom_[(epromBank_*0x2000) + 0x804] == 0x50 && expansionEprom_[(epromBank_*0x2000) + 0x805] == 0x53 && expansionEprom_[(epromBank_*0x2000) + 0x806] == 0x41 && expansionEprom_[(epromBank_*0x2000) + 0x807] == 0x56)
					p_Main->startCassetteLoad(0);
				if (expansionSuper_[(epromBank_*0x2000) + 0x804] == 0x50 && expansionSuper_[(epromBank_*0x2000) + 0x805] == 0x53 && expansionSuper_[(epromBank_*0x2000) + 0x806] == 0x41 && expansionSuper_[(epromBank_*0x2000) + 0x807] == 0x56)
					p_Main->startCassetteLoad(0);
			}
		break;

		case 0xd527:	// END DOS Command
			if (isDosCardSelected())
			{
		  		if ((mainMemory_[0x42d2] == 0x4c) && (mainMemory_[0x42d4] == 0x4f) && (mainMemory_[0x42d6] == 0x41) && (mainMemory_[0x42d8] == 0x44))
					setDosFileName(0x42db);
		  		if ((mainMemory_[0x42d2] == 0x53) && (mainMemory_[0x42d4] == 0x41) && (mainMemory_[0x42d6] == 0x56) && (mainMemory_[0x42d8] == 0x45))
					setDosFileName(0x42db);
			}
		break;

		case 0xcda1:	// END DOS RUN Command
			if (isDosCardSelected())
			{
		  		if ((mainMemory_[0x42d2] == 0xc2) && (mainMemory_[0x42d3] == 0xcf))
					setDosFileName(0x42d4);
			}
		break;

		case 0xdec9:	// END DOS URUN Command
			if (isDosCardSelected())
			{
		  		if ((mainMemory_[0x42d2] == 0x55) && (mainMemory_[0x42d3] == 0x83))
					setDosFileName(0x42d4);
			}
		break;

		case 0xc010:	// 80 Colums
			if (expansionSlot_ == columnSlot_)
				configure6845();
		break;
            
/*		case 0x429f:
		case 0x42a3: 
				p_Main->messageHex(mainMemory_[scratchpadRegister_[programCounter_]+1]*256+mainMemory_[scratchpadRegister_[programCounter_]+2]);
				p_Main->messageHex(mainMemory_[scratchpadRegister_[programCounter_]+5]*256+mainMemory_[scratchpadRegister_[programCounter_]+6]);
		break; */

//		case 0xeb0a:	// Quit 80 Colums
//			stop6845();
//		break;
	}
}

void Comx::sleepComputer(long ms)
{
	threadPointer->Sleep(ms);
}

void Comx::startComputerRun(bool load)
{
	load_ = load;
	if (comxRunState_ == RESETSTATE)
		comxRunCommand_ = 1;
	else
		comxRunCommand_ = 2;
}

bool Comx::isComputerRunning()
{
	if (comxRunState_ == RUNSTATE)
		return true;
	else
		return false;
}

void Comx::startComxKeyFile()
{
	comxKeyFileOpened_ = false;

	wxString fileName = p_Main->getKeyFile();
	if (fileName.Len() != 0)
	{
		fileName = p_Main->getKeyFileDir() + fileName;
		if (wxFile::Exists(fileName))
		{
			if (comxKeyFile_.Open(fileName))
			{
				comxKeyFileOpened_ = true;
				keyboardEf3_ = 0;
			}
		}
	}
}

void Comx::closeComxKeyFile()
{
	if (comxKeyFileOpened_)
	{
		comxKeyFileOpened_ = false;
		comxKeyFile_.Close();
	}
}

void Comx::onReset()
{
    updateDiagLedStatus(5, true);
	resetPressed_ = true;
}

bool Comx::isFAndMBasicRunning()
{
	return fAndMBasicRunning_;
}

void Comx::setDosFileName(int addr)
{
	wxString name;
	name = "";
	addr = 0xbc03;
	while ((mainMemory_[addr] != 0xff) && (addr < 0xbc15))
	{
		name = name + (char)mainMemory_[addr];
		addr++;
	}
	if (name[0] == 0 && name[1] == 0)
		return;

	p_Main->eventSetSwName(name);
	p_Main->setPloadFileName(name + ".comx");
	if (mainMemory_[0xbc17] == 2)
	{
		Word saveStart, saveEnd, saveExec;

		saveStart = mainMemory_[0xbc29] << 8;
		saveEnd = saveStart + (mainMemory_[0xbc25] << 8) + mainMemory_[0xbc26] - 4;
		saveExec = mainMemory_[0xbc29] << 8;
		p_Main->eventSetLocation(true, saveStart, saveEnd, saveExec);
	}
	else
		p_Main->eventSetLocation(false, 0, 0, 0);
}

void Comx::saveRam()
{
	if (memoryType_[0xf000/256] != NVRAM)
		return;

	Byte value;
	wxFile outputFile;
	wxString fileName;

	fileName = p_Main->getMainDir()+"nvramdump.bin";

	if (wxFile::Exists(fileName))
		outputFile.Open(fileName, wxFile::write);
	else
		outputFile.Create(fileName);
	for (long address = 0xf000; address <= 0xf3f7; address++)
	{
		value = mainMemory_[address];
		outputFile.Write(&value, 1);
	}
	outputFile.Close();
}

void Comx::loadRam()
{
	if (memoryType_[0xf000/256] != NVRAM)
		return;

	wxFFile inFile;
	size_t length;
	char buffer[0x3ff];

	wxString dir = p_Main->getMainDir();
	if (wxFile::Exists(p_Main->getMainDir()+"nvramdump.bin"))
	{
		if (inFile.Open(p_Main->getMainDir()+"nvramdump.bin", _("rb")))
		{
			length = inFile.Read(buffer, 0x3f8);
			for (size_t i=0; i<length; i++)
				mainMemory_[i+0xf000] = (Byte)buffer[i];
			inFile.Close();
		}
	}
}

