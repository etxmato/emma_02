/*
 *******************************************************************
 *** This software is copyright 2008 by Marcel van Tongeren      ***
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

MainElf::MainElf()
{
    p_Main->stopTerminal();
}

MainElf::~MainElf()
{
    switch (loadedProgram_)
    {
        case NOPROGRAM:
            if (loadedOs_ == ELFOS)
                p_Main->saveScrtValues("ElfOs");
            else
                p_Main->saveScrtValues("");
        break;

        case MINIMON:
            p_Main->saveScrtValues("MINIMON");
        break;
            
        case GOLDMON:
            p_Main->saveScrtValues("GOLDMON");
        break;
            
        case TINYBASIC:
            p_Main->saveScrtValues("TINYBASIC");
        break;
            
        case SUPERBASICV1:
            p_Main->saveScrtValues("SUPERBASICV1");
        break;
            
        case SUPERBASICV3:
            p_Main->saveScrtValues("SUPERBASICV3");
        break;
            
        case SUPERBASICV5:
            p_Main->saveScrtValues("SUPERBASICV5");
        break;
            
        case SUPERBASICV6:
            p_Main->saveScrtValues("SUPERBASICV6");
        break;
            
        case RCABASIC3:
            p_Main->saveScrtValues("RCABASIC3");
        break;
            
        case RCABASIC4:
            p_Main->saveScrtValues("RCABASIC4");
        break;
    }
}

void MainElf::checkComputerFunction()
{
    if (romMapperDefined_)
    {
        switch (scratchpadRegister_[programCounter_])
        {
            case 0x0A28:	// SAVE
            case 0x0A59:	// LOAD
                if ((mainMemory_[0x9fa] == 0xc0) && (mainMemory_[0x9fb] == 0x0a) && (mainMemory_[0x9fc] == 0x29))
                    p_Main->stopCassette();
            break;
                
            case 0x09FD:	// SAVE
                if ((mainMemory_[0x9fa] == 0xc0) && (mainMemory_[0x9fb] == 0x0a) && (mainMemory_[0x9fc] == 0x29))
                    p_Main->startCassetteSave(0);
            break;
                
            case 0x09FA:	// LOAD
                if ((mainMemory_[0x9fa] == 0xc0) && (mainMemory_[0x9fb] == 0x0a) && (mainMemory_[0x9fc] == 0x29))
                {
                    p_Main->setSwName ("");
                    p_Main->eventUpdateTitle();
                    p_Main->startCassetteLoad(0);
                }
            break;
                
            case 0x7f11:    // ROM MAPPER MOVE
                Word address = scratchpadRegister_[0x8] - 9;
                fetchFileName(address, 9);
            break;
        }

    }
    
	if (baseGiantBoard_ != 0x10000)
	{
		if (scratchpadRegister_[programCounter_] == baseGiantBoard_ + 0x29)
			p_Main->stopCassette();

		if (scratchpadRegister_[programCounter_] == baseGiantBoard_ + 0x8d)
			p_Main->startCassetteSave(0);

		if (scratchpadRegister_[programCounter_] == baseGiantBoard_ + 0xcd)
			p_Main->startCassetteLoad(0);
	}

	if (baseQuestLoader_ != 0x10000)
	{
		if (scratchpadRegister_[programCounter_] == baseQuestLoader_ + 0x5d)
		{
			p_Main->eventSaveStart(scratchpadRegister_[8]);
			p_Main->eventSaveEnd(scratchpadRegister_[8]+scratchpadRegister_[9]-1);
		}
	}

	if (loadedOs_ == ELFOS)
	{
		if (scratchpadRegister_[programCounter_] == 0x7c40)
		{
			if ((mainMemory_[0x7c00] == 0x30) && (mainMemory_[0x7c01] == 0x06) && (mainMemory_[0x7c02] == 0x0c))
			{
				Word saveStart = (mainMemory_[0x7cae] << 8) + mainMemory_[0x7caf];
				Word saveEnd = saveStart + (mainMemory_[0x7cb0] << 8) + mainMemory_[0x7cb1];
				Word saveExec = (mainMemory_[0x7cb2] << 8) + mainMemory_[0x7cb3];
				p_Main->eventSetLocation(true, saveStart, saveEnd, saveExec);
			}
		}
		if (scratchpadRegister_[programCounter_] == 0)
		{
			if ((mainMemory_[0x2202] == 0xc0) && (mainMemory_[0x2203] == 0x28) && (mainMemory_[0x2204] == 0x65) && (mainMemory_[0x226f] == 0x52))
			{
				loadedProgram_ = RCABASIC3;
				basicExecAddress_[BASICADDR_READY] = BASICADDR_READY_RCA3;
				basicExecAddress_[BASICADDR_KEY_VT_RESTART] = BASICADDR_VT_RESTART_RCA;
				basicExecAddress_[BASICADDR_KEY_VT_INPUT] = BASICADDR_VT_INPUT_RCA;
				p_Main->eventEnableMemAccess(true);
			}
			if ((mainMemory_[0x2202] == 0xc0) && (mainMemory_[0x2203] == 0x28) && (mainMemory_[0x2204] == 0x65) && (mainMemory_[0x226f] == 0x55))
			{
				loadedProgram_ = RCABASIC4;
				basicExecAddress_[BASICADDR_READY] = BASICADDR_READY_RCA4;
				basicExecAddress_[BASICADDR_KEY_VT_RESTART] = BASICADDR_VT_RESTART_RCA;
				basicExecAddress_[BASICADDR_KEY_VT_INPUT] = BASICADDR_VT_INPUT_RCA;
				p_Main->eventEnableMemAccess(true);
			}
		}
		if (scratchpadRegister_[programCounter_] == 0x2300)
		{
            if ((mainMemory_[0x201c] == 0xd4) && (mainMemory_[0x201d] == 0x96) && (mainMemory_[0x201e] == 0xb7) && (mainMemory_[0x206f] == 0x05))
            {
                chip8mainLoop_ = 0x201d;
                activateElfOsChip8();
            }
            if ((mainMemory_[0x201f] == 0xd4) && (mainMemory_[0x2020] == 0x96) && (mainMemory_[0x2021] == 0xb7) && (mainMemory_[0x206f] == 0x05))
            {
                chip8mainLoop_ = 0x2020;
                activateElfOsChip8();
            }
		}
	}

	switch (loadedProgram_)
	{
		case NOPROGRAM:
			switch (scratchpadRegister_[programCounter_])
			{
				case 0x13D5:		// START CHUCKs MONITOR
				case 0xfc0b:		// START KEY INPUT ELFOS
				case 0xC0C0:		// MINIMON
				case 0xC000:		// MINIMON
				case 0x8000:		// START CHUCKs MONITOR
					checkLoadedSoftware();
				break;
			}
			if (scratchpadRegister_[programCounter_] == p_Computer->getBasicExecAddr(BASICADDR_READY))
				checkLoadedSoftware();
		break;

		case SUPERBASICV1:
			switch (scratchpadRegister_[programCounter_])
			{
				case BASICADDR_READY_SB1:
					p_Main->stopCassette();
					elfRunState_ = BASICSTATE;
				break;

				case BASICADDR_RUN_SB1:
				case BASICADDR_CALL_SB1:
					elfRunState_ = RUNSTATE;
				break;

				case BASICADDR_PSAVE_SB1:
				case BASICADDR_DSAVE_SB1:
					p_Main->startCassetteSave(0);
				break;

				case BASICADDR_PLOAD_SB1:
				case BASICADDR_DLOAD_SB1:
					p_Main->setSwName ("");
                    p_Main->eventUpdateTitle();
					p_Main->startCassetteLoad(0);
				break;

		/*		case BASICADDR_ROUTINE1_SB1: 
				case BASICADDR_ROUTINE2_SB1: 
						p_Main->messageHex(mainMemory_[scratchpadRegister_[programCounter_]+1]*256+mainMemory_[scratchpadRegister_[programCounter_]+2]);
				break; */
			}
		break;

		case SUPERBASICV3:
			switch (scratchpadRegister_[programCounter_])
			{
				case BASICADDR_READY_SB3:
					p_Main->stopCassette();
					elfRunState_ = BASICSTATE;
				break;

				case BASICADDR_RUN_SB3:
				case BASICADDR_CALL_SB3:
					elfRunState_ = RUNSTATE;
				break;

				case BASICADDR_PSAVE_SB3:
				case BASICADDR_DSAVE_SB3:
					p_Main->startCassetteSave(0);
				break;

				case BASICADDR_PLOAD_SB3:
				case BASICADDR_DLOAD_SB3:
					p_Main->setSwName ("");
                    p_Main->eventUpdateTitle();
					p_Main->startCassetteLoad(0);
				break;

		/*		case BASICADDR_ROUTINE1_SB3: 
				case BASICADDR_ROUTINE2_SB3: 
						p_Main->messageHex(mainMemory_[scratchpadRegister_[programCounter_]+1]*256+mainMemory_[scratchpadRegister_[programCounter_]+2]);
				break; */
			}
		break;

		case SUPERBASICV5:
			switch (scratchpadRegister_[programCounter_])
			{
				case 0xC000:	// MINIMON
					if ((mainMemory_[0xc000] == 0x90) && (mainMemory_[0xc001] == 0xb4) && (mainMemory_[0xc002] == 0xb5) && (mainMemory_[0xc003] == 0xfc))
					{
						loadedProgram_ = MINIMON;
						p_Main->eventEnableMemAccess(true);
						basicExecAddress_[BASICADDR_KEY] = -1;
						basicExecAddress_[BASICADDR_READY] = -1;
						basicExecAddress_[BASICADDR_KEY_VT_RESTART] = -1;
						basicExecAddress_[BASICADDR_KEY_VT_INPUT] = -1;
					}
				break;

				case BASICADDR_READY_SB5:
					p_Main->stopCassette();
					elfRunState_ = BASICSTATE;
				break;

				case BASICADDR_RUN_SB5:
				case BASICADDR_CALL_SB5:
					elfRunState_ = RUNSTATE;
				break;

				case BASICADDR_PSAVE_SB5:
				case BASICADDR_DSAVE_SB5:
					p_Main->startCassetteSave(0);
				break;

				case BASICADDR_PLOAD_SB5:
				case BASICADDR_DLOAD_SB5:
					p_Main->setSwName ("");
                    p_Main->eventUpdateTitle();
					p_Main->startCassetteLoad(0);
				break;

		/*		case BASICADDR_ROUTINE1_SB5: 
				case BASICADDR_ROUTINE2_SB5: 
						p_Main->messageHex(mainMemory_[scratchpadRegister_[programCounter_]+1]*256+mainMemory_[scratchpadRegister_[programCounter_]+2]);
				break; */
			}
		break;

		case SUPERBASICV6:
			switch (scratchpadRegister_[programCounter_])
			{
				case BASICADDR_READY_SB6:
					p_Main->stopCassette();
					elfRunState_ = BASICSTATE;
				break;

				case BASICADDR_RUN_SB6:
				case BASICADDR_CALL_SB6:
					elfRunState_ = RUNSTATE;
				break;

				case BASICADDR_PSAVE_SB6:
				case BASICADDR_DSAVE_SB6:
					p_Main->startCassetteSave(0);
				break;

				case BASICADDR_PLOAD_SB6:
				case BASICADDR_DLOAD_SB6:
					p_Main->setSwName ("");
                    p_Main->eventUpdateTitle();
					p_Main->startCassetteLoad(0);
				break;

     //           case BASICADDR_ROUTINE1_SB6:
     //           case BASICADDR_ROUTINE2_SB6:
     //               p_Main->eventShowMessage(mainMemory_[scratchpadRegister_[programCounter_]+1]*256+mainMemory_[scratchpadRegister_[programCounter_]+2]);
	 //			  break;
			}
		break;

		case RCABASIC3:
			switch (scratchpadRegister_[programCounter_])
			{
				case BASICADDR_READY_RCA3:
					elfRunState_ = BASICSTATE;
				break;

				case BASICADDR_RUN_RCA3:
//				case BASICADDR_CALL_RCA3:
					elfRunState_ = RUNSTATE;
				break;

		/*		case BASICADDR_ROUTINE1_RCA3: 
				case BASICADDR_ROUTINE2_RCA3: 
						p_Main->messageHex(mainMemory_[scratchpadRegister_[programCounter_]+1]*256+mainMemory_[scratchpadRegister_[programCounter_]+2]);
				break; */
			}
		break;

		case RCABASIC4:
			switch (scratchpadRegister_[programCounter_])
			{
				case BASICADDR_READY_RCA4:
					elfRunState_ = BASICSTATE;
				break;

				case BASICADDR_RUN_RCA4:
//				case BASICADDR_CALL_RCA4:
					elfRunState_ = RUNSTATE;
				break;

		/*		case BASICADDR_ROUTINE1_RCA4: 
				case BASICADDR_ROUTINE2_RCA4: 
						p_Main->messageHex(mainMemory_[scratchpadRegister_[programCounter_]+1]*256+mainMemory_[scratchpadRegister_[programCounter_]+2]);
				break; */
			}
		break;

		case TINYBASIC:
            if (!romMapperDefined_)
            {
                switch (scratchpadRegister_[programCounter_])
                {
                    case 0x0A28:	// SAVE
                    case 0x0A59:	// LOAD
                        p_Main->stopCassette();
                        break;

                    case 0x09FD:	// SAVE
                        p_Main->startCassetteSave(0);
                        break;

                    case 0x09FA:	// LOAD
                        p_Main->setSwName ("");
                        p_Main->eventUpdateTitle();
                        p_Main->startCassetteLoad(0);
                        break;
                }
            }
		break;

		case MINIMON:
			switch (scratchpadRegister_[programCounter_])
			{
				case 0x0100:	// BASIC
					if ((mainMemory_[0x100] == 0xc0) && (mainMemory_[0x101] == 0x18) && (mainMemory_[0x102] == 0x00) && (mainMemory_[0x103] == 0xc0))
					{
						loadedProgram_ = SUPERBASICV5;
						p_Main->eventEnableMemAccess(true);
						basicExecAddress_[BASICADDR_KEY] = BASICADDR_KEY_SB5;
						basicExecAddress_[BASICADDR_READY] = BASICADDR_READY_SB5;
						basicExecAddress_[BASICADDR_KEY_VT_RESTART] = BASICADDR_VT_RESTART_SB5;
						basicExecAddress_[BASICADDR_KEY_VT_INPUT] = BASICADDR_VT_INPUT_SB5;
						elfRunState_ = RESETSTATE;
					}
				break;

				case 0xc237:	// SAVE
				case 0xc2cd:	// LOAD
					p_Main->stopCassette();
				break;

				case 0xc20c:	// SAVE
					p_Main->startCassetteSave(0);
				break;

				case 0xc28d:	// LOAD
					p_Main->setSwName ("");
                    p_Main->eventUpdateTitle();
					p_Main->startCassetteLoad(0);
				break;
			}
		break;

		case GOLDMON:
			switch (scratchpadRegister_[programCounter_])
			{
				case 0xc5d3:	// SAVE ASCII MON
				case 0xc6c3:	// LOAD ASCII MON
				case 0xD237:	// SAVE
				case 0xD2cd:	// SAVE
					p_Main->stopCassette();
				break;

				case 0xc571:	// SAVE ASCII MON
				case 0xd20c:	// SAVE
					p_Main->startCassetteSave(0);
				break;

				case 0xc643:	// LOAD ASCII MON
				case 0xd28d:	// LOAD GOLD
					p_Main->setSwName ("");
                    p_Main->eventUpdateTitle();
					p_Main->startCassetteLoad(0);
				break;
			}
		break;

		case MONITOR_CHUCK_LOW:
			switch (scratchpadRegister_[programCounter_])
			{
				case 0xf4e:		// LOAD L
					p_Main->startAutoTerminalLoad(false);
				break;

                case 0x1300:	// LOAD I
                    p_Main->startAutoTerminalLoad(false);
                break;
                    
                case 0x102D:	// SAVE S
                    if (scratchpadRegister_[7]==0x2fd)
                        p_Main->startAutoTerminalSave();
                break;
                    
                case 0x11EA:	// SAVE C
                    if (scratchpadRegister_[7]==0x2fd)
                        p_Main->startAutoTerminalSave();
                break;

                case 0x10C5:	// STOP SAVE S & C
                    p_Main->stopAutoTerminal();
                break;

            }
		break;

		case MONITOR_CHUCK_HIGH:
			switch (scratchpadRegister_[programCounter_])
			{
				case 0x8f4e:	// LOAD L
					p_Main->startAutoTerminalLoad(false);
				break;

                case 0x9300:	// LOAD I
                    p_Main->startAutoTerminalLoad(false);
                break;
                    
				case 0x902D:	// SAVE S
                    if (scratchpadRegister_[7]==0x2fd)
                        p_Main->startAutoTerminalSave();
				break;

                case 0x91EA:	// SAVE C
                    if (scratchpadRegister_[7]==0x2fd)
                        p_Main->startAutoTerminalSave();
                break;
                    
				case 0x90C5:	// STOP SAVE S & C
					p_Main->stopAutoTerminal();
				break;
			}
		break;
	}
}

void MainElf::activateElfOsChip8()
{
    pseudoType_ = p_Main->getPseudoDefinition(&chip8baseVar_, &chip8mainLoop_, &chip8register12bit_, &pseudoLoaded_);
    
    Word address = scratchpadRegister_[0xA];
    
    fetchFileName(address, 100);
}

void MainElf::fetchFileName(Word address, size_t length)
{
    wxString fileName_ = "";
    Byte MemoryValue = readMem(address++);
    
    while (MemoryValue != 0 && length > 0)
    {
        if (MemoryValue >= 32 && MemoryValue < 128)
            fileName_ += wxString::FromAscii(MemoryValue);
        MemoryValue = readMem(address++);
        length--;
    }
    fileName_.MakeLower();
    fileName_.Trim();
    
    reLoadKeyDefinition(fileName_);
    p_Main->eventSetSwName(fileName_);
}

void MainElf::startComputerRun(bool load)
{
	if (elfConfiguration.useKeyboard)
		startElfRun(load);
	if (elfConfiguration.vtType != VTNONE)
	{
		if (cpuMode_ != RUN)
		{
			onInButtonPress();
			onRun();
			vtPointer->startElfRun(load, true);
		}
		else
			vtPointer->startElfRun(load, false);
	}
}

bool MainElf::isComputerRunning()
{
	if (elfRunState_ == RUNSTATE)
		return true;
	else
		return false;
}

