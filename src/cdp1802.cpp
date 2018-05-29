/*
 *******************************************************************
 *** This software is copyright 2008 by Marcel van Tongeren      ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 ***                                                             ***
 *** 1802 Code based on elf emulator by Michael H Riley with     **
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

#include "wx/textfile.h"

#include "main.h"

Cdp1802::Cdp1802()
{
}

void Cdp1802::initCpu(int computerType)
{
	cpuType_ = p_Main->getCpuType();
	computerType_ = computerType;

    if (p_Main->getCpuStartupRegisters() == STARTUP_ZEROED)
    {
        for (int i=1; i<16; i++)
            scratchpadRegister_[i] = 0;
        dataFlag_ = 0;
        accumulator_ = 0;
        registerT_ = 0;
   }
    else
    {
        for (int i=1; i<16; i++)
            scratchpadRegister_[i] = rand() % 0x10000;
        dataFlag_ = rand() % 2;
        accumulator_ = rand() % 0x100;
        registerT_ = rand() % 0x100;
   }
	efFlags_ = 0xf;

	cpuMode_ = RESET;
	clear_ = 0;
	wait_ = 1;
	initIo();
	steps_ = -1;
	baseGiantBoard_ = 0x10000;
	baseQuestLoader_ = 0x10000;

	for (int i=0; i<8; i++) outValues_[i] = 0;
	for (int i=0; i<8; i++) inValues_[i] = 0;

	debugMode_ = false;
	chip8DebugMode_ = false;
	trace_ = false;
	traceDma_ = false;
	traceInt_ = false;
	traceChip8Int_ = false;
}

void Cdp1802::resetCpu()
{
	flipFlopQ_ = 0;
	interruptEnable_ = 1;
	ci_ = 0;
	xi_ = 0;
	dataPointer_ = 0;
	programCounter_ = 0;
	scratchpadRegister_[0] = 0;
    ctrPre_ = 32;
	tq_ = 0;
	ctrRunning_ = 0;
	idle_ = 0;
	address_ = 0;
	colourMask_ = 0;
	if (p_Video != NULL)
		p_Video->reset();
}

void Cdp1802::machineCycle()
{
	for (int i=1; i<5; i++)
	{
		setEf(i, ef(i));
	}

	for (int i=0; i<MAXCYCLE; i++)
	{
		cycle(i);
	}
    if (ctrRunning_ == 1 && ctrMode_ == 3)
    {
        if (--ctrPre_ == 0)
        {
            decCounter();
            ctrPre_ = 32;
        }
    }
    soundCycle();
}

void Cdp1802::setMode()
{
	if (clear_ == 0 && wait_==1)
	{
		cpuMode_ = RESET;
        if (cpuType_ != CPU1801)
            resetCpu();
	}
	if (clear_ == 1 && wait_==1) cpuMode_ = RUN;
	if (clear_ == 0 && wait_==0) cpuMode_ = LOAD;
	if (clear_ == 1 && wait_==0) cpuMode_ = PAUSE;

	if (cpuMode_ != RUN)
	{
		if (p_Video != NULL)
			p_Video->reset();
	}
}

void Cdp1802::setClear(int value)
{
	clear_= (value)?1:0;
	setMode();
}

void Cdp1802::setWait(int value)
{
	wait_= (value)?1:0;
	setMode();
}

void Cdp1802::dmaIn(Byte value)
{
	if (traceDma_)
	{
		wxString traceText;
		traceText.Printf("----  DMA in    R0=%04X", scratchpadRegister_[0]);
		p_Main->debugTrace(traceText);
	}
	if (cpuMode_ != RUN && cpuMode_ != LOAD && computerType_ != FRED) return;
	writeMem(scratchpadRegister_[0], value, false);
	address_ = scratchpadRegister_[0]++;
	idle_=0;
	cpuCycles_++;
//	machineCycle(); // Using this will crash Elfs when tying in keys with Q sound on 'Hardware'
}

Byte Cdp1802::dmaOut()
{
	Byte ret;
	ret = 255;
	if (traceDma_)
	{
		wxString traceText;
		traceText.Printf("----  DMA out   R0=%04X", scratchpadRegister_[0]);
		p_Main->debugTrace(traceText);
	}
	ret=readMem(scratchpadRegister_[0]);
	scratchpadRegister_[0]++;
	idle_=0;
	cpuCycles_++;
	machineCycle();

	return ret;
}

Byte Cdp1802::pixieDmaOut(int *color)
{
	Byte ret;
	ret = 255;
	if (traceDma_)
	{
		wxString traceText;
		traceText.Printf("----  DMA out   R0=%04X", scratchpadRegister_[0]);
		p_Main->debugTrace(traceText);
	}
	ret=readMem(scratchpadRegister_[0]);
	switch (computerType_)
	{
		case ETI:
			*color = colorMemory1864_[((scratchpadRegister_[0] >> 1) & 0xf8) + (scratchpadRegister_[0] & 0x7)] & 0x7;
		break;
		case VIP:
		case VIPII:
			if (colourMask_ == 0)
				*color = 7;
			else
				*color = colorMemory1864_[scratchpadRegister_[0] & colourMask_] & 0x7;
		break;
        case VICTORY:
            if (colourMask_ == 0)
                *color = 7;
            else
                *color = colorMemory1864_[((scratchpadRegister_[0] >> 2) & 0x38) + (scratchpadRegister_[0] & 0x7)] & 0x7;
		break;
		case VELF:
        case STUDIO:
        case COINARCADE:
        case FRED:
		case ELF:
		case ELFII:
		case SUPERELF:
            *color = 0;
        break;
		default:
			*color = colorMemory1864_[scratchpadRegister_[0] & 0x3ff] & 0x7;
		break;
	}
	scratchpadRegister_[0]++;
	idle_=0;
	cpuCycles_++;
	soundCycle();

	return ret;
}

void Cdp1802::visicomDmaOut(Byte *vram1, Byte *vram2)
{
	if (traceDma_)
	{
		wxString traceText;
		traceText.Printf("----  DMA out   R0=%04X", scratchpadRegister_[0]);
		p_Main->debugTrace(traceText);
	}
	*vram1 = readMem(scratchpadRegister_[0]);
	*vram2 = readMem(scratchpadRegister_[0]+0x200);

	scratchpadRegister_[0]++;
	idle_=0;
	cpuCycles_++;
	soundCycle();
}

Byte Cdp1802::pixieDmaOut()
{
	Byte ret;
	ret = 255;
	if (traceDma_)
	{
		wxString traceText;
		traceText.Printf("----  DMA out   R0=%04X", scratchpadRegister_[0]);
		p_Main->debugTrace(traceText);
	}
	ret=readMem(scratchpadRegister_[0]);
	scratchpadRegister_[0]++;
	idle_=0;
	cpuCycles_++;
	soundCycle();

	return ret;
}

void Cdp1802::decCounter()
{
	if (--counter_ == 0)
	{
		if (tq_)
		{
			flipFlopQ_ = (flipFlopQ_) ? 0 : 1;
		}
		ci_ = (cie_) ? 1 : 0;
		counter_ = ch_;
        if (ci_)
            interrupt();
	}
}

void Cdp1802::setEf(int flag,int value)
{
	if (flag == 1)
	{
		if (ctrMode_ == 1 && ctrRunning_)
		{
			if (value == 0 &&(efFlags_ & 1)) decCounter();
		}
		if (value) efFlags_ |= 1;
		else efFlags_ &= 0xe;
	}
	if (flag == 2)
	{
		if (ctrMode_ == 2 && ctrRunning_)
		{
 			if (value == 0 &&(efFlags_ & 2)) decCounter();
		}
		if (value) efFlags_ |= 2;
		else efFlags_ &= 0xd;
	}
	if (flag == 3)
	{
		if (value) efFlags_ |= 4;
		else efFlags_ &= 0xb;
	}
	if (flag == 4)
	{
 		if (value) efFlags_ |= 8;
		else efFlags_ &= 7;
	}
}

void Cdp1802::interrupt()
{
	if (p_Main->isDiagActive(COMX) && computerType_ == COMX)
	{
		if (interruptEnable_ && (clear_ == 1))
		{
			p_Video->updateDiagLedStatus(3, false); //INT
			p_Video->updateDiagLedStatus(4, true); //INTACK
		}
		else
		{
			p_Video->updateDiagLedStatus(3, true); //INT
			p_Video->updateDiagLedStatus(4, false); //INTACK
		}
	}
	if (interruptEnable_ && (clear_ == 1) && (getDmaCounter() != -100))
	{
		if (traceInt_)
		{
			p_Main->debugTrace("----  Interrupt");
		}
		if (traceChip8Int_)
		{
			p_Main->chip8DebugTrace("--- 1802 Interrupt");
		}
		registerT_= (dataPointer_<<4) | programCounter_;
		dataPointer_=2;
		programCounter_=1;
		interruptEnable_=0;
		cpuCycles_++;
		machineCycle();
	}
	idle_=0;
}

void Cdp1802::pixieInterrupt()
{
	if (interruptEnable_)
	{
		if (traceInt_)
		{
			p_Main->debugTrace("----  Interrupt");
		}
		if (traceChip8Int_)
		{
			p_Main->chip8DebugTrace("--- 1802 Interrupt");
		}
		registerT_= (dataPointer_<<4) | programCounter_;
		dataPointer_=2;
		programCounter_=1;
		interruptEnable_=0;
		cpuCycles_++;
  //      machineCycle(); - I put this in to get correct timeing for something; however this messes up the Pixie screens on at least the Elfs.
    }
	idle_=0;
}

void Cdp1802::inst1805(wxString *tr)
{
	wxString buffer;

	Byte i, h, l, n, memValue, df1;
	int  a,b;
	int w;
	w = 0;
	i=readMem(scratchpadRegister_[programCounter_]);
	n = i & 15;
	i = i>>4;
	p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
	scratchpadRegister_[programCounter_]++;
	switch(i)
	{
		case 0:
			switch(n)
			{
				case 0: // 1804
					cpuCycles_ += 1;
					ctrRunning_ = 0;
                    ctrPre_ = 32;
					ctrMode_ = 0;
 					if (trace_)
					{
						buffer.Printf("STPC");
						*tr = *tr + buffer;
					}
				break;
				case 1: // 1804
					cpuCycles_ += 1;
					decCounter();
					if (trace_)
					{
						buffer.Printf("DTC       CNTR=%02X", counter_);
 	//		      					   XXX       R
						*tr = *tr + buffer;
					}
				break;
				case 2: // 1804
					cpuCycles_ += 1;
					ctrMode_ = 5;
					ctrRunning_ = 1;
                    ctrPre_ = 32;
					if (trace_)
					{
						buffer.Printf("SPM2");
						*tr = *tr + buffer;
					}
				break;
				case 3: // 1804
					cpuCycles_ += 1;
					ctrMode_ = 2;
					ctrRunning_ = 1;
                    ctrPre_ = 32;
					if (trace_)
					{
						buffer.Printf("SCM2");
						*tr = *tr + buffer;
					}
				break;
				case 4: // 1804
					cpuCycles_ += 1;
					ctrMode_ = 4;
					ctrRunning_ = 1;
                    ctrPre_ = 32;
					if (trace_)
					{
						buffer.Printf("SPM1");
						*tr = *tr + buffer;
					}
				break;
				case 5: // 1804
					cpuCycles_ += 1;
					ctrMode_ = 1;
					ctrRunning_ = 1;
                    ctrPre_ = 32;
                    if (trace_)
					{
						buffer.Printf("SCM1");
						*tr = *tr + buffer;
					}
				break;
				case 6: // 1804
					cpuCycles_ += 1;
					ch_ = accumulator_;
					if (!ctrRunning_)
					{
  						counter_ = accumulator_;
  						ci_ = 0;
						if (trace_)
						{
							buffer.Printf("LDC       CH/CNTR=%02X", ch_);
							*tr = *tr + buffer;
						}
					}
					else
					{
						if (trace_)
						{
							buffer.Printf("LDC       CH=%02X", ch_);
							*tr = *tr + buffer;
						}
					}
				break;
				case 7: // 1804
					cpuCycles_ += 1;
					ctrMode_ = 3;
					ctrRunning_ = 1;
                    ctrPre_ = 32;
					if (trace_)
					{
						buffer.Printf("STM");
						*tr = *tr + buffer;
					}
				break;
				case 8: // 1804
					cpuCycles_ += 1;
					accumulator_ = counter_;
					if (trace_)
					{
						buffer.Printf("GEC       D=%02X", accumulator_);
						*tr = *tr + buffer;
					}
				break;
				case 9: // 1804
					cpuCycles_ += 1;
					tq_ = 1;
					if (trace_)
					{
						buffer.Printf("ETQ");
						*tr = *tr + buffer;
					}
				break;
				case 0xa: // 1804
					cpuCycles_ += 1;
					xie_ = 1;
					if (trace_)
					{
						buffer.Printf("XIE");
						*tr = *tr + buffer;
					}
				break;
				case 0xb: // 1804
					cpuCycles_ += 1;
					xie_ = 0;
					if (trace_)
					{
						buffer.Printf("XID");
						*tr = *tr + buffer;
					}
				break;
				case 0xc: // 1804
					cpuCycles_ += 1;
					cie_ = 1;
					if (trace_)
					{
						buffer.Printf("CIE");
						*tr = *tr + buffer;
					}
				break;
				case 0xd: // 1804
					cpuCycles_ += 1;
					cie_ = 0;
					if (trace_)
					{
						buffer.Printf("CID");
						*tr = *tr + buffer;
					}
				break;
				default:
					if (trace_)
					{
						buffer.Printf("Illegal code");
						*tr = *tr + buffer;
					}
				break;
			}
		break;

		case 2:
			if (cpuType_ == CPU1805)
			{
				cpuCycles_ += 3;
				p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
				h=readMem(scratchpadRegister_[programCounter_]);
				l=readMem((Word) (scratchpadRegister_[programCounter_]+1));
				w= (h<<8)|l;
				if (--scratchpadRegister_[n] != 0) 
					scratchpadRegister_[programCounter_] = w;
				else 
					scratchpadRegister_[programCounter_]+=2;
				if (trace_)
				{
					buffer.Printf("DBNZ R%X,%02X%02X R%X=%04X", n, h, l, n,scratchpadRegister_[n]);
					*tr = *tr + buffer;
				}
			}
			else
			{
				if (trace_)
				{
					buffer.Printf("Illegal code");
					*tr = *tr + buffer;
				}
			}
		break;

		case 3:
			switch(n)
			{
				case 0xe: // 1804
					cpuCycles_ += 1;
					i=readMem(scratchpadRegister_[programCounter_]);
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
					if (ci_)
                    {
						scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | i;
                        ci_ = 0;
                    }
					else
						scratchpadRegister_[programCounter_]++;
					if (trace_)
					{
						buffer.Printf("BCI  %02X",i);
						*tr = *tr + buffer;
					}
				break;
				case 0xf: // 1804
					cpuCycles_ += 1;
					i=readMem(scratchpadRegister_[programCounter_]);
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
					if (xi_)
						scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | i;
					else
						scratchpadRegister_[programCounter_]++;
					if (trace_)
					{
						buffer.Printf("BXI  %02X",i);
						*tr = *tr + buffer;
					}
				break;
				default:
					if (trace_)
					{
						buffer.Printf("Illegal code");
						*tr = *tr + buffer;
					}
				break;
			}
 		break;

		case 6: // 1804
			cpuCycles_ += 3;
			w = readMem(scratchpadRegister_[dataPointer_]++) << 8;
			w |= readMem(scratchpadRegister_[dataPointer_]++);
			scratchpadRegister_[n] = w;
			if (trace_)
			{
 				buffer.Printf("RLXA R%X   R%X=%04X", n, n, scratchpadRegister_[n]);
 				*tr = *tr + buffer;
 			}
  		break;

		case 7:
			if (cpuType_ == CPU1805)
			{
				switch(n)
				{
					case 4:
						cpuCycles_ += 2;
						w = readMem(scratchpadRegister_[dataPointer_]);
						a = (((w >> 4) > 9) ? 9 : w >> 4) *10;
						a += ((w & 0xf) > 9) ? 9 :(w & 0xf);
						w = accumulator_;
						b = (((w >> 4) > 9) ? 9 : w >> 4) *10;
						b += ((w & 0xf) > 9) ? 9 :(w & 0xf);
						a += b + dataFlag_;
						dataFlag_ = (a > 99) ? 1 : 0;
						accumulator_ = (((a / 10)%10) << 4) | a % 10;
						if (trace_)  {
							buffer.Printf("DADC      D=%02X", accumulator_);
							*tr = *tr + buffer;
						}
					break;
					case 6:
						cpuCycles_ += 4;
						writeMem(--scratchpadRegister_[dataPointer_], registerT_, false);
						writeMem(--scratchpadRegister_[dataPointer_], accumulator_, false);
						df1= (dataFlag_) ? 128 : 0;
						dataFlag_ = (accumulator_ & 1) ? 1 : 0;
						accumulator_ = (accumulator_ >> 1) | df1;
						writeMem(--scratchpadRegister_[dataPointer_], accumulator_, false);
						if (trace_)  
						{
      						buffer.Printf("DSAV      D=%02X", accumulator_);
							*tr = *tr + buffer;
						}
					break;
					case 7:
						cpuCycles_ += 2;
						w = readMem(scratchpadRegister_[dataPointer_]);
						a = (((w >> 4) > 9) ? 9 : w >> 4) *10;
						a += ((w & 0xf) > 9) ? 9 :(w & 0xf);
						w = accumulator_;
						b = (((w >> 4) > 9) ? 9 : w >> 4) *10;
						b += ((w & 0xf) > 9) ? 9 :(w & 0xf);
						df1 = (dataFlag_)?0:1;
						a = b - a - df1;
						if (a < 0)
						{
							dataFlag_ = 0;
							a += 100;
						}
						else
						{
							dataFlag_ = 1;
						}
						accumulator_ = (((a / 10)%10) << 4) | a % 10;
						if (trace_)  
						{
							buffer.Printf("DSMB      D=%02X", accumulator_);
							*tr = *tr + buffer;
						}
					break;
					case 0xc:
						cpuCycles_ += 2;
						p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
						memValue = readMem(scratchpadRegister_[programCounter_]++);
						w = memValue;
						a = (((w >> 4) > 9) ? 9 : w >> 4) *10;
						a += ((w & 0xf) > 9) ? 9 :(w & 0xf);
						w = accumulator_;
						b = (((w >> 4) > 9) ? 9 : w >> 4) *10;
						b += ((w & 0xf) > 9) ? 9 : w&0xf;
						a += b + dataFlag_;
						dataFlag_ = (a > 99) ? 1 : 0;
						accumulator_ = (((a / 10)%10) << 4) | a % 10;
						if (trace_)  
						{
							buffer.Printf("DACI %02X   D=%02X", memValue, accumulator_);
							*tr = *tr + buffer;
						}
					break;
					case 0xf:
						cpuCycles_ += 2;
						p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
						memValue = readMem(scratchpadRegister_[programCounter_]++);
						w = memValue;
						a = (((w >> 4) > 9) ? 9 : w >> 4) *10;
						a += ((w & 0xf) > 9) ? 9 :(w & 0xf);
						w = accumulator_;
						b = (((w >> 4) > 9) ? 9 : w >> 4) *10;
						b += ((w & 0xf) > 9) ? 9 :(w & 0xf);
						df1 = (dataFlag_)?0:1;
						a = b - a - df1;
						if (a < 0)
						{
							dataFlag_ = 0;
							a += 100;
						}
						else
						{
							dataFlag_ = 1;
						}
						accumulator_ = (((a / 10)%10) << 4) | a % 10;
						if (trace_)  
						{
							buffer.Printf("DSBI %02X   D=%02X", memValue, accumulator_);
							*tr = *tr + buffer;
						}
					break;
					default:
						if (trace_)
						{
							buffer.Printf("Illegal code");
							*tr = *tr + buffer;
						}
					break;
				}
			}
 			else
			{
				if (trace_)
				{
					buffer.Printf("Illegal code");
					*tr = *tr + buffer;
				}
			}
		break;

		case 8: // 1804
			cpuCycles_ += 8;
			writeMem(scratchpadRegister_[dataPointer_]--,(Byte) (scratchpadRegister_[n] & 0xff), false);
			writeMem(scratchpadRegister_[dataPointer_]--,(Byte) (scratchpadRegister_[n] >> 8), false);
			scratchpadRegister_[n] = scratchpadRegister_[programCounter_];
			w = readMem(scratchpadRegister_[n]++) << 8;
			w |= readMem(scratchpadRegister_[n]++);
			scratchpadRegister_[programCounter_] = w;
			if (trace_)
			{
 				buffer.Printf("SCAL R%X,%04X", n, scratchpadRegister_[programCounter_]);
 				*tr = *tr + buffer;
 			}
  		break;

		case 9: // 1804
			cpuCycles_ += 6;
			scratchpadRegister_[programCounter_] = scratchpadRegister_[n];
			w = readMem(++scratchpadRegister_[dataPointer_]) << 8;
			w |= readMem(++scratchpadRegister_[dataPointer_]);
			scratchpadRegister_[n] = w;
			if (trace_)
			{
 				buffer.Printf("SRET R%X   R%X(P)=%04X", n, programCounter_, scratchpadRegister_[programCounter_]);
 				*tr = *tr + buffer;
 			}
  		break;

		case 0xa: // 1804
			cpuCycles_ += 3;
			writeMem(scratchpadRegister_[dataPointer_]--,(Byte) (scratchpadRegister_[n] & 0xff), false);
			writeMem(scratchpadRegister_[dataPointer_]--,(Byte) (scratchpadRegister_[n] >> 8), false);
			if (trace_)
			{
 				buffer.Printf("RSXD R%X",n);
 				*tr = *tr + buffer;
 			}
  		break;

		case 0xb: // 1804
			cpuCycles_ += 2;
			scratchpadRegister_[dataPointer_] = scratchpadRegister_[n];
			if (trace_)
			{
 				buffer.Printf("RNX  R%X   R%X=%04X", n, dataPointer_, scratchpadRegister_[dataPointer_]);
				*tr = *tr + buffer;
 			}
  		break;

		case 0xc:  // 1804
			cpuCycles_ += 3;
			p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
			w = readMem(scratchpadRegister_[programCounter_]++) << 8;
			p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
			w |= readMem(scratchpadRegister_[programCounter_]++);
			scratchpadRegister_[n] = w;
			if (trace_)
			{
				if (p_Computer->readMemDataType(scratchpadRegister_[programCounter_]-3) == MEM_TYPE_OPCODE_RLDL)
 					buffer.Printf("RLDL R%X,%04X", n, w);
				else
 					buffer.Printf("RLDI R%X,%04X", n, w);
 				*tr = *tr + buffer;
 			}
  		break;

		case 0xf:
			if (cpuType_ == CPU1805)
			{
				switch(n)
				{
					case 4:
						cpuCycles_ += 2;
						w = readMem(scratchpadRegister_[dataPointer_]);
						a = (((w >> 4) > 9) ? 9 : w >> 4) *10;
						a += ((w & 0xf) > 9) ? 9 :(w & 0xf);
						w = accumulator_;
						b = (((w >> 4) > 9) ? 9 : w >> 4) *10;
						b += ((w & 0xf) > 9) ? 9 :(w & 0xf);
						a += b;
						dataFlag_ = (a > 99) ? 1 : 0;
						accumulator_ = (((a / 10)%10) << 4) | a % 10;
						if (trace_)
						{
							buffer.Printf("DADD      D=%02X", accumulator_);
							*tr = *tr + buffer;
						}
					break;
					case 7:
						cpuCycles_ += 2;
						w = readMem(scratchpadRegister_[dataPointer_]);
						a = (((w >> 4) > 9) ? 9 : w >> 4) *10;
						a += ((w & 0xf) > 9) ? 9 :(w & 0xf);
						w = accumulator_;
						b = (((w >> 4) > 9) ? 9 : w >> 4) *10;
						b += ((w & 0xf) > 9) ? 9 :(w & 0xf);
						a = b - a;
						if (a < 0)
						{
							dataFlag_ = 0;
							a += 100;
						}
						else
						{
							dataFlag_ = 1;
						}
						accumulator_ = (((a / 10)%10) << 4) | a % 10;
						if (trace_)
						{
							buffer.Printf("DSM       D=%02X", accumulator_);
							*tr = *tr + buffer;
						}
					break;
					case 0xc:
						cpuCycles_ += 2;
						p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
						memValue = readMem(scratchpadRegister_[programCounter_]++);
						w = memValue;
						a = (((w >> 4) > 9) ? 9 : w >> 4) *10;
						a += ((w & 0xf) > 9) ? 9 :(w & 0xf);
						w = accumulator_;
						b = (((w >> 4) > 9) ? 9 : w >> 4) *10;
						b += ((w & 0xf) > 9) ? 9 :(w & 0xf);
						a += b;
						dataFlag_ = (a > 99) ? 1 : 0;
						accumulator_ = (((a / 10)%10) << 4) | a % 10;
						if (trace_)
						{
							buffer.Printf("DADI %02X   D=%02X", memValue, accumulator_);
							*tr = *tr + buffer;
						}
					break;
					case 0xf:
						cpuCycles_ += 2;
						p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
						memValue = readMem(scratchpadRegister_[programCounter_]++);
						w = memValue;
						a = (((w >> 4) > 9) ? 9 : w >> 4) *10;
						a += ((w & 0xf) > 9) ? 9 :(w & 0xf);
						w = accumulator_;
						b = (((w >> 4) > 9) ? 9 : w >> 4) *10;
						b += ((w & 0xf) > 9) ? 9 :(w & 0xf);
						a = b - a;
						if (a < 0)
						{
							dataFlag_ = 0;
							a += 100;
						}
						else
						{
							dataFlag_ = 1;
						}
						accumulator_ = (((a / 10)%10) << 4) | a % 10;
						if (trace_)
						{
							buffer.Printf("DSMI %02X   D=%02X", memValue, accumulator_);
							*tr = *tr + buffer;
						}
					break;
					default:
						if (trace_)
						{
							buffer.Printf("Illegal code");
							*tr = *tr + buffer;
						}
					break;
				}
			}
			else
			{
				if (trace_)
				{
					buffer.Printf("Illegal code");
					*tr = *tr + buffer;
				}
			}
		break;
		default:
			if (trace_)
			{
				buffer.Printf("Illegal code");
				*tr = *tr + buffer;
			}
		break;
	}
}

void Cdp1802::cpuCycle()
{
	wxString tr, buffer;
	Byte i,h,l,n;
	int w;
	Byte df1;
	Byte tmp;

	if (cpuMode_ != RUN) return;
	if (idle_) return;
	if (trace_)
	{
		tr.Printf("%04X: ",scratchpadRegister_[programCounter_]);
	}
    
	i=readMem(scratchpadRegister_[programCounter_]);
	if (p_Computer->readMemDataType(scratchpadRegister_[programCounter_]) >= MEM_TYPE_OPCODE_RSHR)
	{
		switch (p_Computer->readMemDataType(scratchpadRegister_[programCounter_]))
		{
			case MEM_TYPE_OPCODE_RSHR:
				if (i != 0x76)
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPCODE);
			break;
			case MEM_TYPE_OPCODE_RSHL:
				if (i != 0x7E)
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPCODE);
			break;
			case MEM_TYPE_OPCODE_BPZ:
				if (i != 0x33)
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPCODE);
			break;
			case MEM_TYPE_OPCODE_BGE:
				if (i != 0x33)
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPCODE);
			break;
			case MEM_TYPE_OPCODE_BM:
				if (i != 0x3b)
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPCODE);
			break;
			case MEM_TYPE_OPCODE_BL:
				if (i != 0x3b)
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPCODE);
			break;
			case MEM_TYPE_OPCODE_LSKP:
				if (i != 0xc8)
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPCODE);
			break;
			case MEM_TYPE_OPCODE_SKP:
				if (i != 0x38)
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPCODE);
			break;
			case MEM_TYPE_OPCODE_LBR_SLOT:
				if ((i&0xf0) != 0xC0)
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPCODE);
			break;
			case MEM_TYPE_OPCODE_LDV:
			case MEM_TYPE_OPCODE_LDL:
			case MEM_TYPE_OPCODE_LDL_SLOT:
				if (i != 0xF8)
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPCODE);
			break;
			case MEM_TYPE_OPERAND_LD_2:
				if ((i&0xf0) != 0xB0)
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPCODE);
			break;
			case MEM_TYPE_OPERAND_LD_3:
				if (i != 0xF8)
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPCODE);
			break;
			case MEM_TYPE_OPERAND_LD_5:
				if ((i&0xf0) != 0xA0)
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPCODE);
			break;
			case MEM_TYPE_OPCODE_RLDL:
				if (i != 68 && (readMem(scratchpadRegister_[programCounter_]+1)&0xf0) != 0xc0)
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPCODE);
			break;
		}
	}
	else
		p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPCODE);
	n = i & 15;
	i = i>>4;

	scratchpadRegister_[programCounter_]++;
	switch(i)
	{
 		case 0:
            if (computerType_ == FRED)
            {
                idle_=1;
                p_Computer->showDataLeds(readMem(scratchpadRegister_[n]));
                if (trace_)
                {
                    buffer.Printf("IDL  R%X",n);
                    tr = tr + buffer;
                }
            }
            else
            {
                if (n == 0)
                {
                    idle_=1;
                    if (trace_)
                    {
                        tr = tr + "IDL";
                    }
                }
                else
                {
                    if (cpuType_ == CPU1801)
                    {
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            tr = tr + buffer;
                        }
                    }
                    else
                    {
                        accumulator_=readMem(scratchpadRegister_[n]);
                        if (trace_)
                        {
                            buffer.Printf("LDN  R%X   D=%02X",n,accumulator_);
                            tr = tr + buffer;
                        }
                    }
                }
            }
		break;

		case 1:
			scratchpadRegister_[n]++;
			if (trace_)
			{
				buffer.Printf("INC  R%X   R%X=%04X",n,n,scratchpadRegister_[n]);
				tr = tr + buffer;
			}
		break;

		case 2:
			scratchpadRegister_[n]--;
			if (trace_)
			{
				buffer.Printf("DEC  R%X   R%X=%04X",n,n,scratchpadRegister_[n]);
				tr = tr + buffer;
			}
		break;

		case 3:
			switch(n)
			{
				case 0:
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
					i=readMem(scratchpadRegister_[programCounter_]);
					scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | i;
                    p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | i, LABEL_TYPE_BRANCH);
                    if (trace_)
					{
						buffer.Printf("BR   %02X",i);
						tr = tr + buffer;
					}
				break;
				case 1:
                    if (cpuType_ == CPU1801)
                    {
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            tr = tr + buffer;
                        }
                    }
                    else
                    {
                        p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                        i=readMem(scratchpadRegister_[programCounter_]);
                        p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | i, LABEL_TYPE_BRANCH);
                        if (flipFlopQ_)
                        {
                            scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | i;
                        }
                        else scratchpadRegister_[programCounter_]++;
                        if (trace_)
                        {
                            buffer.Printf("BQ   %02X",i);
                            tr = tr + buffer;
                        }
                    }
				break;
				case 2:
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
					i=readMem(scratchpadRegister_[programCounter_]);
                    p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | i, LABEL_TYPE_BRANCH);
					if (!accumulator_)
					{
						scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | i;
					}
					else
						scratchpadRegister_[programCounter_]++;
					if (trace_)
					{
						buffer.Printf("BZ   %02X",i);
						tr = tr + buffer;
					}
				break;
				case 3:
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
					i=readMem(scratchpadRegister_[programCounter_]);
                    p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | i, LABEL_TYPE_BRANCH);
					if (dataFlag_)
					{
						scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | i;
					}
					else
						scratchpadRegister_[programCounter_]++;
					if (trace_)
					{
						if (p_Computer->readMemDataType(scratchpadRegister_[programCounter_]-2) == MEM_TYPE_OPCODE_BPZ)
								buffer.Printf("BPZ  %02X",i);
						else
						{
							if (p_Computer->readMemDataType(scratchpadRegister_[programCounter_]-2) == MEM_TYPE_OPCODE_BGE)
								buffer.Printf("BGE  %02X",i);
							else
								buffer.Printf("BDF  %02X",i);
						}
						tr = tr + buffer;
					}
				break;
				case 4:
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
					i=readMem(scratchpadRegister_[programCounter_]);
                    p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | i, LABEL_TYPE_BRANCH);
					readyToReceiveData[0] = true;
					if (!(efFlags_ & 1))
					{
						scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | i;
					}
					else
						scratchpadRegister_[programCounter_]++;
					if (trace_)
					{
						buffer.Printf("B1   %02X",i);
						tr = tr + buffer;
					}
				break;
				case 5:
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
					i=readMem(scratchpadRegister_[programCounter_]);
                    p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | i, LABEL_TYPE_BRANCH);
					readyToReceiveData[1] = true;
					if (!(efFlags_ & 2))
					{
						scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | i;
					}
					else
						scratchpadRegister_[programCounter_]++;
					if (trace_)
					{
						buffer.Printf("B2   %02X",i);
						tr = tr + buffer;
					}
				break;
				case 6:
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
					i=readMem(scratchpadRegister_[programCounter_]);
                    p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | i, LABEL_TYPE_BRANCH);
					readyToReceiveData[2] = true;
					if (!(efFlags_ & 4))
					{
						scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | i;
					}
					else
						scratchpadRegister_[programCounter_]++;
					if (trace_)
					{
						buffer.Printf("B3   %02X",i);
						tr = tr + buffer;
					}
				break;
				case 7:
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
					i=readMem(scratchpadRegister_[programCounter_]);
                    p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | i, LABEL_TYPE_BRANCH);
					readyToReceiveData[3] = true;
					if (!(efFlags_ & 8))
					{
						scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | i;
					}
					else
						scratchpadRegister_[programCounter_]++;
					if (trace_)
					{
						buffer.Printf("B4   %02X",i);
						tr = tr + buffer;
					}
				break;
				case 8:
					scratchpadRegister_[programCounter_]++;
					if (trace_)
					{
						if (p_Computer->readMemDataType(scratchpadRegister_[programCounter_]-1) == MEM_TYPE_OPCODE_SKP)
							buffer.Printf("SKP");
						else
							buffer.Printf("NBR");
						tr = tr + buffer;
					}
				break;
				case 9:
                    if (cpuType_ == CPU1801)
                    {
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            tr = tr + buffer;
                        }
                    }
                    else
                    {
                        p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                        i=readMem(scratchpadRegister_[programCounter_]);
                        p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | i, LABEL_TYPE_BRANCH);
                        if (!flipFlopQ_)
                        {
                            scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | i;
                        }
                        else
                            scratchpadRegister_[programCounter_]++;
                        if (trace_)
                        {
                            buffer.Printf("BNQ  %02X",i);
                            tr = tr + buffer;
                        }
                    }
				break;
				case 10:
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
					i=readMem(scratchpadRegister_[programCounter_]);
                    p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | i, LABEL_TYPE_BRANCH);
					if (accumulator_)
					{
						scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | i;
					}
					else
						scratchpadRegister_[programCounter_]++;
					if (trace_)
					{
						buffer.Printf("BNZ  %02X",i);
						tr = tr + buffer;
					}
				break;
				case 11:
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
					i=readMem(scratchpadRegister_[programCounter_]);
                    p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | i, LABEL_TYPE_BRANCH);
					if (!dataFlag_)
					{
						scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | i;
					}
					else
						scratchpadRegister_[programCounter_]++;
					if (trace_)
					{
						if (p_Computer->readMemDataType(scratchpadRegister_[programCounter_]-2) == MEM_TYPE_OPCODE_BM)
								buffer.Printf("BM   %02X",i);
						else
						{
							if (p_Computer->readMemDataType(scratchpadRegister_[programCounter_]-2) == MEM_TYPE_OPCODE_BL)
								buffer.Printf("BL   %02X",i);
							else
								buffer.Printf("BNF  %02X",i);
						}
						tr = tr + buffer;
					}
				break;
				case 12:
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
					i=readMem(scratchpadRegister_[programCounter_]);
                    p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | i, LABEL_TYPE_BRANCH);
					readyToReceiveData[0] = true;
					if (efFlags_ & 1)
					{
						scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | i;
					}
					else
						scratchpadRegister_[programCounter_]++;
					if (trace_)
					{
						buffer.Printf("BN1  %02X",i);
						tr = tr + buffer;
					}
				break;
				case 13:
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
					i=readMem(scratchpadRegister_[programCounter_]);
                    p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | i, LABEL_TYPE_BRANCH);
					readyToReceiveData[1] = true;
					if (efFlags_ & 2)
					{
						scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | i;
					}
					else
						scratchpadRegister_[programCounter_]++;
					if (trace_)
					{
						buffer.Printf("BN2  %02X",i);
						tr = tr + buffer;
					}
				break;
				case 14:
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
					i=readMem(scratchpadRegister_[programCounter_]);
                    p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | i, LABEL_TYPE_BRANCH);
					readyToReceiveData[2] = true;
					if (efFlags_ & 4)
					{
						scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | i;
					}
					else
						scratchpadRegister_[programCounter_]++;
					if (trace_)
					{
						buffer.Printf("BN3  %02X",i);
						tr = tr + buffer;
					}
				break;
				case 15:
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
					i=readMem(scratchpadRegister_[programCounter_]);
                    p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | i, LABEL_TYPE_BRANCH);
					readyToReceiveData[3] = true;
					if (efFlags_ & 8)
					{
						scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | i;
					}
					else
						scratchpadRegister_[programCounter_]++;
					if (trace_)
					{
						buffer.Printf("BN4  %02X",i);
						tr = tr + buffer;
					}
				break;
			}
		break;
		case 4:
			accumulator_=readMem(scratchpadRegister_[n]++);
			if (trace_)
			{
				buffer.Printf("LDA  R%X   D=M(%04X)=%02X",n,scratchpadRegister_[n]-1,accumulator_);
				tr = tr + buffer;
			}
		break;
		case 5:
			writeMem(scratchpadRegister_[n], accumulator_, false);
			if (trace_)
			{
				buffer.Printf("STR  R%X   M(%04X)=%02X",n,scratchpadRegister_[n],accumulator_);
 				tr = tr + buffer;
			}
		break;
		case 6:
			if (n == 0 && cpuType_ != CPU1801)
			{
				scratchpadRegister_[dataPointer_]++;
				if (trace_)
				{
					buffer.Printf("IRX       R%X=%04X",dataPointer_,scratchpadRegister_[dataPointer_]);
					tr = tr + buffer;
				}
				break;
			}
			if (n == 8 && (cpuType_ == CPU1805 || cpuType_ == CPU1804))
			{
				inst1805(&tr);
				break;
			}
			if (n <= 7)
			{
				tmp = readMem(scratchpadRegister_[dataPointer_]++);
				out(n, scratchpadRegister_[dataPointer_]-1, tmp);
				if (trace_)
				{
					switch (computerType_)
					{
						case COMX:
						case CIDELSA:
						case PECOM:
							if (n>3)
								buffer.Printf("OUT  %X    [%04X]",n,scratchpadRegister_[dataPointer_]-1);
							else
								buffer.Printf("OUT  %X    [%02X]",n,tmp);
						break;

						case TMC600:
							if (n==5 && (p_Computer->getOutValue(7) != 0x20) && (p_Computer->getOutValue(7) != 0x30))
								buffer.Printf("OUT  %X    [%04X]",n,scratchpadRegister_[dataPointer_]-1);
							else
								buffer.Printf("OUT  %X    [%02X]",n,tmp);
						break;

						default:
							buffer.Printf("OUT  %X    [%02X]",n,tmp);
						break;
					}
					tr = tr + buffer;
				}
				break;
			}
			i=in((Byte)(n-8), scratchpadRegister_[dataPointer_]);
			writeMem(scratchpadRegister_[dataPointer_], i, false);
			accumulator_=i;
			if (trace_)
			{
                buffer.Printf("INP  %X    D=M(%04X)=%02X",n-8,scratchpadRegister_[dataPointer_], i);
				tr = tr + buffer;
			}
		break;
		case 7:
			switch(n)
			{
 				case 0:
					i=readMem(scratchpadRegister_[dataPointer_]++);
					programCounter_=i & 15;
					dataPointer_= (i>>4);
					interruptEnable_=1;
					if (trace_)
					{
						buffer.Printf("RET       P=R%X, X=R%X", programCounter_, dataPointer_);
						tr = tr + buffer;
					}
				break;
				case 1:
					i=readMem(scratchpadRegister_[dataPointer_]++);
					programCounter_=i & 15;
					dataPointer_=i>>4;
					interruptEnable_=0;
					if (trace_)
					{
						buffer.Printf("DIS       P=R%X, X=R%X", programCounter_, dataPointer_);
						tr = tr + buffer;
					}
				break;
				case 2:
                    if (cpuType_ == CPU1801)
                    {
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            tr = tr + buffer;
                        }
                    }
                    else
                    {
                        accumulator_ = readMem(scratchpadRegister_[dataPointer_]++);
                        if (trace_)
                        {
                            buffer.Printf("LDXA      D=M(%04X)=%02X", scratchpadRegister_[dataPointer_]-1, accumulator_);
                            tr = tr + buffer;
                        }
                    }
				break;
				case 3:
                    if (cpuType_ == CPU1801)
                    {
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            tr = tr + buffer;
                        }
                    }
                    else
                    {
                        writeMem(scratchpadRegister_[dataPointer_]--, accumulator_, false);
                        if (trace_)
                        {
                            buffer.Printf("STXD      M(%04X)=%02X",scratchpadRegister_[dataPointer_]+1,accumulator_);
                            tr = tr + buffer;
                        }
                    }
				break;
				case 4:
                    if (cpuType_ == CPU1801)
                    {
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            tr = tr + buffer;
                        }
                    }
                    else
                    {
                        w=accumulator_ + readMem(scratchpadRegister_[dataPointer_]) + dataFlag_;
                        if (w>255)
                        {
                            accumulator_ = w & 255;
                            dataFlag_=1;
                        }
                        else
                        {
                            accumulator_=w;
                            dataFlag_=0;
                        }
                        if (trace_)
                        {
                            buffer.Printf("ADC       D=%02X", accumulator_);
                            tr = tr + buffer;
                        }
                    }
				break;
				case 5:
                    if (cpuType_ == CPU1801)
                    {
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            tr = tr + buffer;
                        }
                    }
                    else
                    {
                        w=readMem(scratchpadRegister_[dataPointer_]) +((~accumulator_)&0xff) + dataFlag_;
                        if (w>255)
                        {
                            accumulator_ = w&255;
                            dataFlag_=1;
                        }
                        else
                        {
                            accumulator_=w&255;
                            dataFlag_=0;
                        }
                        if (trace_)
                        {
                            buffer.Printf("SDB       D=%02X", accumulator_);
                            tr = tr + buffer;
                        }
                    }
 				break;
				case 6:
                    if (cpuType_ == CPU1801)
                    {
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            tr = tr + buffer;
                        }
                    }
                    else
                    {
                        df1= (dataFlag_) ? 128 : 0;
                        dataFlag_ = (accumulator_ & 1) ? 1 : 0;
                        accumulator_ = (accumulator_ >> 1) | df1;
                        if (trace_)
                        {
                            if (p_Computer->readMemDataType(scratchpadRegister_[programCounter_]-1) == MEM_TYPE_OPCODE_RSHR)
                                buffer.Printf("RSHR      D=%02X", accumulator_);
                            else
                                buffer.Printf("SHRC      D=%02X", accumulator_);
                            tr = tr + buffer;
                        }
                    }
				break;
				case 7:
                    if (cpuType_ == CPU1801)
                    {
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            tr = tr + buffer;
                        }
                    }
                    else
                    {
                        w=accumulator_ +((~readMem(scratchpadRegister_[dataPointer_]))&0xff) + dataFlag_;
                        if (w>255)
                        {
                            accumulator_ = w & 0xff;
                            dataFlag_=1;
                        }
                        else
                        {
                            accumulator_=w&255;
                            dataFlag_=0;
                        }
                        if (trace_)
                        {
                            buffer.Printf("SMB       D=%02X", accumulator_);
                            tr = tr + buffer;
                        }
                    }
				break;
				case 8:
					writeMem(scratchpadRegister_[dataPointer_], registerT_, false);
					if (trace_)
					{
						buffer.Printf("SAV       M(%04X)=%02X",scratchpadRegister_[dataPointer_], registerT_);
						tr = tr + buffer;
					}
				break;
				case 9:
                    if (cpuType_ == CPU1801)
                    {
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            tr = tr + buffer;
                        }
                    }
                    else
                    {
                        registerT_= (dataPointer_<<4) | programCounter_;
                        writeMem(scratchpadRegister_[2]--, registerT_, false);
                        dataPointer_=programCounter_;
                        if (trace_)
                        {
                            buffer.Printf("MARK");
                            tr = tr + buffer;
                        }
                    }
				break;
				case 10:
                    if (cpuType_ == CPU1801)
                    {
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            tr = tr + buffer;
                        }
                    }
                    else
                    {
                        flipFlopQ_=0;
                        if (trace_)
                        {
                            tr = tr + "REQ";
                        }
                        switchQ(0);
                        if (computerType_ != MS2000 && computerType_ != FRED)
                            psaveAmplitudeChange(0);
                    }
				break;
				case 11:
                    if (cpuType_ == CPU1801)
                    {
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            tr = tr + buffer;
                        }
                    }
                    else
                    {
                        flipFlopQ_=1;
                        if (trace_)
                        {
                            tr = tr + "SEQ";
                        }
                        switchQ(1);
                        if (computerType_ != MS2000 && computerType_ != FRED)
                            psaveAmplitudeChange(1);
                    }
				break;
				case 12:
                    if (cpuType_ == CPU1801)
                    {
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            tr = tr + buffer;
                        }
                    }
                    else
                    {
                        p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                        i=readMem(scratchpadRegister_[programCounter_]++);
                        w=accumulator_ + i + dataFlag_;
                        if (w>255)
                        {
                            accumulator_ = w & 255;
                            dataFlag_=1;
                        }
                        else
                        {
                            accumulator_=w;
                            dataFlag_=0;
                        }
                        if (trace_)
                        {
                            buffer.Printf("ADCI %02X   D=%02X", i, accumulator_);
                            tr = tr + buffer;
                        }
                    }
				break;
				case 13:
                    if (cpuType_ == CPU1801)
                    {
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            tr = tr + buffer;
                        }
                    }
                    else
                    {
                        p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                        i=readMem(scratchpadRegister_[programCounter_]++);
                        w=i +((~accumulator_)&0xff) + dataFlag_;
                        if (w>255)
                        {
                            accumulator_ = w & 0xff;
                            dataFlag_=1;
                        }
                        else
                        {
                            accumulator_=w&255;
                            dataFlag_=0;
                        }
                        if (trace_)
                        {
                            buffer.Printf("SDBI %02X   D=%02X", i, accumulator_);
                            tr = tr + buffer;
                        }
                    }
				break;
				case 14:
                    if (cpuType_ == CPU1801)
                    {
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            tr = tr + buffer;
                        }
                    }
                    else
                    {
                        df1= (dataFlag_ & 1) ? 1: 0;
                        dataFlag_ = (accumulator_ & 128) ? 1 : 0;
                        accumulator_ = (accumulator_ << 1) | df1;
                        if (trace_)
                        {
                            if (p_Computer->readMemDataType(scratchpadRegister_[programCounter_]-1) == MEM_TYPE_OPCODE_RSHL)
                                buffer.Printf("RSHL      D=%02X", accumulator_);
                            else
                                buffer.Printf("SHLC      D=%02X", accumulator_);
                            tr = tr + buffer;
                        }
                    }
				break;
				case 15:
                    if (cpuType_ == CPU1801)
                    {
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            tr = tr + buffer;
                        }
                    }
                    else
                    {
                        p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                        i= (~readMem(scratchpadRegister_[programCounter_]++)) & 0xff;
                        w=accumulator_ + i + dataFlag_;
                        if (w>255)
                        {
                            accumulator_ = w & 0xff;
                            dataFlag_=1;
                        }
                        else
                        {
                            accumulator_=w&255;
                            dataFlag_=0;
                        }
                        if (trace_)
                        {
                            buffer.Printf("SMBI %02X   D=%02X", i, accumulator_);
                            tr = tr + buffer;
                        }
                    }
				break;
            }
		break;
		case 8:
			accumulator_= (scratchpadRegister_[n] & 255);
			if (trace_)
			{
				buffer.Printf("GLO  R%X   D=%02X", n, accumulator_);
				tr = tr + buffer;
			}
		break;
		case 9:
			accumulator_= (scratchpadRegister_[n] >> 8);
			if (trace_)
			{
				buffer.Printf("GHI  R%X   D=%02X",n,accumulator_);
				tr = tr + buffer;
			}
		break;
		case 10:
			scratchpadRegister_[n]= (scratchpadRegister_[n] & 0xff00) | accumulator_;
			if (trace_)
			{
				buffer.Printf("PLO  R%X   R%X=%04X ",n,n,scratchpadRegister_[n]);
				tr = tr + buffer;
			}
		break;
		case 11:
			scratchpadRegister_[n]= (scratchpadRegister_[n] & 0x00ff) |(accumulator_<<8);
			if (trace_)
			{
				buffer.Printf("PHI  R%X   R%X=%04X ",n,n,scratchpadRegister_[n]);
				tr = tr + buffer;
			}
		break;
		case 12:
            if (cpuType_ == CPU1801)
            {
                if (trace_)
                {
                    buffer.Printf("Illegal code");
                    tr = tr + buffer;
                }
            }
            else
            {
			h=readMem(scratchpadRegister_[programCounter_]);
			l=readMem((Word) (scratchpadRegister_[programCounter_]+1));
			cpuCycles_ += 1;
			w= (h<<8)|l;
			switch(n)
			{
				case 0:
					if (p_Computer->readMemDataType(scratchpadRegister_[programCounter_]-1) != MEM_TYPE_OPCODE_LBR_SLOT)
					{
						p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
						p_Computer->writeMemDataType(scratchpadRegister_[programCounter_]+1, MEM_TYPE_OPERAND);
					}
					scratchpadRegister_[programCounter_]=w;
                    p_Computer->writeMemLabelType(w, LABEL_TYPE_JUMP);
					if (trace_)
					{
						buffer.Printf("LBR  %02X%02X",h,l);
						tr = tr + buffer;
					}
				break;
				case 1:
					if (p_Computer->readMemDataType(scratchpadRegister_[programCounter_]-1) != MEM_TYPE_OPCODE_LBR_SLOT)
					{
						p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
						p_Computer->writeMemDataType(scratchpadRegister_[programCounter_]+1, MEM_TYPE_OPERAND);
					}
					if (flipFlopQ_) scratchpadRegister_[programCounter_]=w;
					else scratchpadRegister_[programCounter_]+=2;
                    p_Computer->writeMemLabelType(w, LABEL_TYPE_JUMP);
					if (trace_)
					{
						buffer.Printf("LBQ  %02X%02X",h,l);
						tr = tr + buffer;
					}
				break;
				case 2:
					if (p_Computer->readMemDataType(scratchpadRegister_[programCounter_]-1) != MEM_TYPE_OPCODE_LBR_SLOT)
					{
						p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
						p_Computer->writeMemDataType(scratchpadRegister_[programCounter_]+1, MEM_TYPE_OPERAND);
					}
					if (!accumulator_) scratchpadRegister_[programCounter_]=w;
					else scratchpadRegister_[programCounter_]+=2;
                    p_Computer->writeMemLabelType(w, LABEL_TYPE_JUMP);
					if (trace_)
					{
						buffer.Printf("LBZ  %02X%02X",h,l);
						tr = tr + buffer;
					}
				break;
				case 3:
					if (p_Computer->readMemDataType(scratchpadRegister_[programCounter_]-1) != MEM_TYPE_OPCODE_LBR_SLOT)
					{
						p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
						p_Computer->writeMemDataType(scratchpadRegister_[programCounter_]+1, MEM_TYPE_OPERAND);
					}
					if (dataFlag_) scratchpadRegister_[programCounter_]=w;
					else scratchpadRegister_[programCounter_]+=2;
                    p_Computer->writeMemLabelType(w, LABEL_TYPE_JUMP);
					if (trace_)
					{
						buffer.Printf("LBDF %02X%02X",h,l);
						tr = tr + buffer;
					}
				break;
				case 4:
					if (trace_)
					{
						buffer.Printf("NOP");
						tr = tr + buffer;
					}
				break;
				case 5:
					if (!flipFlopQ_) scratchpadRegister_[programCounter_]+=2;
					if (trace_)
					{
						buffer.Printf("LSNQ");
						tr = tr + buffer;
					}
				break;
				case 6:
					if (accumulator_) scratchpadRegister_[programCounter_]+=2;
					if (trace_)
					{
						buffer.Printf("LSNZ");
						tr = tr + buffer;
					}
				break;
				case 7:
					if (!dataFlag_) scratchpadRegister_[programCounter_]+=2;
					if (trace_)
					{
						buffer.Printf("LSNF");
						tr = tr + buffer;
					}
				break;
				case 8:
					scratchpadRegister_[programCounter_]+=2;
					if (trace_)
					{
						if (p_Computer->readMemDataType(scratchpadRegister_[programCounter_]-1) == MEM_TYPE_OPCODE_LSKP)
							buffer.Printf("LSKP");
						else
							buffer.Printf("NLBR");
						tr = tr + buffer;
					}
				break;
				case 9:
					if (p_Computer->readMemDataType(scratchpadRegister_[programCounter_]-1) != MEM_TYPE_OPCODE_LBR_SLOT)
					{
						p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
						p_Computer->writeMemDataType(scratchpadRegister_[programCounter_]+1, MEM_TYPE_OPERAND);
					}
					if (!flipFlopQ_) scratchpadRegister_[programCounter_]=w;
					else scratchpadRegister_[programCounter_]+=2;
                    p_Computer->writeMemLabelType(w, LABEL_TYPE_JUMP);
					if (trace_)
					{
						buffer.Printf("LBNQ %02X%02X",h,l);
						tr = tr + buffer;
					}
				break;
				case 10:
					if (p_Computer->readMemDataType(scratchpadRegister_[programCounter_]-1) != MEM_TYPE_OPCODE_LBR_SLOT)
					{
						p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
						p_Computer->writeMemDataType(scratchpadRegister_[programCounter_]+1, MEM_TYPE_OPERAND);
					}
					if (accumulator_) scratchpadRegister_[programCounter_]=w;
					else scratchpadRegister_[programCounter_]+=2;
                    p_Computer->writeMemLabelType(w, LABEL_TYPE_JUMP);
					if (trace_)
					{
						buffer.Printf("LBNZ %02X%02X",h,l);
						tr = tr + buffer;
					}
				break;
				case 11:
					if (p_Computer->readMemDataType(scratchpadRegister_[programCounter_]-1) != MEM_TYPE_OPCODE_LBR_SLOT)
					{
						p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
						p_Computer->writeMemDataType(scratchpadRegister_[programCounter_]+1, MEM_TYPE_OPERAND);
					}
					if (!dataFlag_) scratchpadRegister_[programCounter_]=w;
					else scratchpadRegister_[programCounter_]+=2;
                    p_Computer->writeMemLabelType(w, LABEL_TYPE_JUMP);
					if (trace_)
					{
						buffer.Printf("LBNF %02X%02X",h,l);
						tr = tr + buffer;
					}
				break;
				case 12:
					if (interruptEnable_) scratchpadRegister_[programCounter_]+=2;
					if (trace_)
					{
						buffer.Printf("LSIE");
						tr = tr + buffer;
					}
				break;
				case 13:
					if (flipFlopQ_) scratchpadRegister_[programCounter_]+=2;
					if (trace_)
					{
						buffer.Printf("LSQ");
						tr = tr + buffer;
					}
				break;
				case 14:
					if (!accumulator_) scratchpadRegister_[programCounter_]+=2;
					if (trace_)
					{
						buffer.Printf("LSZ");
						tr = tr + buffer;
					}
				break;
				case 15:
					if (dataFlag_) scratchpadRegister_[programCounter_]+=2;
					if (trace_)
					{
						buffer.Printf("LSDF");
						tr = tr + buffer;
					}
				break;
			}
			machineCycle();
            }
		break;
		case 13:
			programCounter_=n;
            p_Computer->writeMemLabelType(scratchpadRegister_[programCounter_], LABEL_TYPE_SUB);
			if (trace_)
			{
				buffer.Printf("SEP  R%X",n);
				tr = tr + buffer;
			}
		break;
		case 14:
			dataPointer_=n;
			if (trace_)
			{
				buffer.Printf("SEX  R%X",n);
				tr = tr + buffer;
			}
		break;
		case 15:
			switch(n)
			{
				case 0:
					accumulator_=readMem(scratchpadRegister_[dataPointer_]);
					if (trace_)
					{
						buffer.Printf("LDX       D=M(%04X)=%02X",scratchpadRegister_[dataPointer_],accumulator_);
						tr = tr + buffer;
					}
				break;
				case 1:
					accumulator_=readMem(scratchpadRegister_[dataPointer_]) | accumulator_;
					if (trace_)
					{
						buffer.Printf("OR        D=%02X",accumulator_);
						tr = tr + buffer;
					}
				break;
				case 2:
					accumulator_=readMem(scratchpadRegister_[dataPointer_]) & accumulator_;
					if (trace_) {
						buffer.Printf("AND       D=%02X",accumulator_);
						tr = tr + buffer;
					}
				break;
				case 3:
					accumulator_=readMem(scratchpadRegister_[dataPointer_]) ^ accumulator_;
					if (trace_)
					{
						buffer.Printf("XOR       D=%02X",accumulator_);
						tr = tr + buffer;
					}
				break;
				case 4:
					w=accumulator_ + readMem(scratchpadRegister_[dataPointer_]);
					if (w>255)
					{
						accumulator_ = w & 255;
						dataFlag_=1;
					}
					else
					{
						accumulator_=w;
						dataFlag_=0;
					}
					if (trace_)
					{
						buffer.Printf("ADD       D=%02X",accumulator_);
						tr = tr + buffer;
					}
				break;
				case 5:
					w=readMem(scratchpadRegister_[dataPointer_]) +((~accumulator_)&0xff)+1;
					if (w>255)
					{
						accumulator_ = w & 0xff;
						dataFlag_=1;
					}
					else
					{
						accumulator_=w&255;
						dataFlag_=0;
					}
					if (trace_)
					{
						buffer.Printf("SD        D=%02X", accumulator_);
						tr = tr + buffer;
					}
 				break;
				case 6:
					dataFlag_= (accumulator_ & 1)? 1 : 0;
					accumulator_=accumulator_>>1;
					if (trace_)
					{
						buffer.Printf("SHR       D=%02X",accumulator_);
						tr = tr + buffer;
					}
				break;
				case 7:
					w=accumulator_+((~readMem(scratchpadRegister_[dataPointer_]))&0xff)+1;
					if (w>255)
					{
						accumulator_ = w & 0xff;
						dataFlag_=1;
					}
					else
					{
						accumulator_=w&255;
						dataFlag_=0;
					}
					if (trace_)
					{
						buffer.Printf("SM        D=%02X", accumulator_);
						tr = tr + buffer;
					}
				break;
				case 8:
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
					accumulator_=readMem(scratchpadRegister_[programCounter_]++);
					if (trace_)
					{
						buffer.Printf("LDI  %02X",accumulator_);
						tr = tr + buffer;
					}
				break;
				case 9:
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
					i=readMem(scratchpadRegister_[programCounter_]++);
					accumulator_ |= i;
					if (trace_)
					{
						buffer.Printf("ORI  %02X   D=%02X", i, accumulator_);
						tr = tr + buffer;
					}
				break;
				case 10:
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
					i=readMem(scratchpadRegister_[programCounter_]++);
					accumulator_ &= i;
					if (trace_)
					{
						buffer.Printf("ANI  %02X   D=%02X",i,accumulator_);
						tr = tr + buffer;
					}
				break;
				case 11:
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
					i=readMem(scratchpadRegister_[programCounter_]++);
					accumulator_ ^= i;
					if (trace_)
					{
						buffer.Printf("XRI  %02X   D=%02X",i,accumulator_);
						tr = tr + buffer;
					}
				break;
				case 12:
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
					i=readMem(scratchpadRegister_[programCounter_]++);
					w=accumulator_ + i;
					if (w>255)
					{
						accumulator_ = w & 255;
						dataFlag_=1;
					}
					else
					{
						accumulator_=w;
						dataFlag_=0;
					}
					if (trace_)
					{
						buffer.Printf("ADI  %02X   D=%02X",i,accumulator_);
						tr = tr + buffer;
					}
 				break;
				case 13:
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
					i=readMem(scratchpadRegister_[programCounter_]++);
					w=i+((~accumulator_)&0xff)+1;
					if (w>255)
					{
						accumulator_ = w&0xff;
						dataFlag_=1;
					}
					else
					{
						accumulator_=w&255;
						dataFlag_=0;
					}
					if (trace_)
					{
						buffer.Printf("SDI  %02X   D=%02X",i, accumulator_);
						tr = tr + buffer;
					}
				break;
				case 14:
                    if (cpuType_ == CPU1801)
                    {
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            tr = tr + buffer;
                        }
                    }
                    else
                    {
                        dataFlag_= (accumulator_ & 128)? 1 : 0;
                        accumulator_=accumulator_<<1;
                        if (trace_)
                        {
                            buffer.Printf("SHL       D=%02X",accumulator_);
                            tr = tr + buffer;
                        }
                    }
				break;
				case 15:
					p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
					i= (~readMem(scratchpadRegister_[programCounter_]++))&0xff;
					w=accumulator_+i+1;
					if (w>255)
					{
						accumulator_ = w&0xff;
						dataFlag_=1;
					}
					else
					{
						accumulator_=w&255;
						dataFlag_=0;
					}
					if (trace_)
					{
						buffer.Printf("SMI  %02X   D=%02X",(~i)&0xff,accumulator_);
	//				      		       XXX       R
 						tr = tr + buffer;
					}
				break;
			}
		break;
	}
	if (trace_) p_Main->debugTrace(tr);
}

bool Cdp1802::readIntelFile(wxString fileName, int memoryType, long end, bool showFilename)
{
	wxTextFile inFile;
	wxString line, strValue;
	long count;
	long address;
	long value;
	int spaces;
	bool overloaded = false;
	Word start = 0xffff;
	Word last = 0;

	if (inFile.Open(fileName))
	{
		for (line=inFile.GetFirstLine(); !inFile.Eof(); line=inFile.GetNextLine())
		{
			spaces = 0;
			int maxSpaces = 6;
			if (line.Len() < 6)  maxSpaces = (int)line.Len();
			for (int i=0; i<maxSpaces; i++) if (line[i] == 32) spaces++;
			if (spaces == 0)
			{
				strValue = line.Mid(1, 2);
				if (!strValue.ToLong(&count, 16))
                    count = 0;

				strValue = line.Mid(3, 4);
				strValue.ToLong(&address, 16);

				strValue = line.Mid(7, 2);
				strValue.ToLong(&value, 16);

				if (value == 1)
				{
					inFile.Close();
					if (overloaded)
					{
						wxString endStr;
						endStr.Printf("%04X", (int)end);
						p_Main->errorMessage("Attempt to load after address " + endStr);
					}
					setAddress(showFilename, start, last);
					return true;
				}
				if (address < start)
					start = address;
				for (int i=0; i<count; i++)
				{
					strValue = line.Mid((i*2)+9, 2);
					strValue.ToLong(&value, 16);
					if (memoryType != NOCHANGE && memoryType != RAM)
						defineMemoryType(address, memoryType);
					if (address < end)
						writeMem(address,(Byte)value, true);
					else
						overloaded = true;
					address++;
				}
				if (address > last)
					last = address;
			}
			else
			{
				strValue = line.Mid(1, 4);
				strValue.ToLong(&address, 16);
				for (size_t i=5; i<line.Len(); i++)
				{
					if ((line[i] >= '0' && line [i] <= '9') ||
						(line[i] >= 'A' && line [i] <= 'F') ||
						(line[i] >= 'a' && line [i] <= 'f'))
					{
						strValue = line.Mid(i, 2);
						if (strValue.ToLong(&value, 16))
						{
							value &= 255;
							if (memoryType != NOCHANGE && memoryType != RAM)
								defineMemoryType(address, memoryType);
							if (address < end)
								writeMem(address,(Byte)value, true);
							else
								overloaded = true;
							address++;
							i++;
						}
					}
				}
			}
		}
		inFile.Close();
		if (overloaded)
		{
			wxString endStr;
			endStr.Printf("%04X", (int)end);
			p_Main->errorMessage("Attempt to load after address " + endStr);
		}
		setAddress(showFilename, start, last);
		return true;
	}
	else
	{
		p_Main->errorMessage("Error reading " + fileName);
		return false;
	}
}

bool Cdp1802::readIntelFile(wxString fileName, int memoryType, Word* lastAddress, long end, bool showFilename)
{
	wxTextFile inFile;
	wxString line, strValue;
	long count;
	long address=1;
	long value;
	int spaces;
	bool overloaded = false;
	Word start = 0xffff;
	Word last = 0;

	if (inFile.Open(fileName))
	{
		for (line = inFile.GetFirstLine(); !inFile.Eof(); line = inFile.GetNextLine())
		{
			spaces = 0;
			int maxSpaces = 6;
			if (line.Len() < 6)  maxSpaces = (int)line.Len();
			for (int i = 0; i<maxSpaces; i++) if (line[i] == 32) spaces++;
			if (spaces == 0)
			{
				strValue = line.Mid(1, 2);
				if (!strValue.ToLong(&count, 16))
					count = 0;

				strValue = line.Mid(3, 4);
				strValue.ToLong(&address, 16);

				strValue = line.Mid(7, 2);
				strValue.ToLong(&value, 16);

				if (value == 1)
				{
					inFile.Close();
					if (overloaded)
					{
						wxString endStr;
						endStr.Printf("%04X", (int)end);
						p_Main->errorMessage("Attempt to load after address " + endStr);
					}
                    setAddress(showFilename, start, last);
                    *lastAddress = address - 1;
					return true;
				}
				if (address < start)
					start = address;
				for (int i = 0; i<count; i++)
				{
					strValue = line.Mid((i * 2) + 9, 2);
					strValue.ToLong(&value, 16);
					if (memoryType != NOCHANGE && memoryType != RAM)
						defineMemoryType(address, memoryType);
					if (address < end)
						writeMem(address, (Byte)value, true);
					else
						overloaded = true;
					address++;
				}
				if (address > last)
					last = address;
			}
			else
			{
				strValue = line.Mid(1, 4);
				strValue.ToLong(&address, 16);
				for (size_t i = 5; i<line.Len(); i++)
				{
					if ((line[i] >= '0' && line[i] <= '9') ||
						(line[i] >= 'A' && line[i] <= 'F') ||
						(line[i] >= 'a' && line[i] <= 'f'))
					{
						strValue = line.Mid(i, 2);
						if (strValue.ToLong(&value, 16))
						{
							value &= 255;
							if (memoryType != NOCHANGE && memoryType != RAM)
								defineMemoryType(address, memoryType);
							if (address < end)
								writeMem(address, (Byte)value, true);
							else
								overloaded = true;
							address++;
							i++;
						}
					}
				}
			}
		}
		inFile.Close();
		*lastAddress = address - 1;
		if (overloaded)
		{
			wxString endStr;
			endStr.Printf("%04X", (int)end);
			p_Main->errorMessage("Attempt to load after address " + endStr);
		}
		setAddress(showFilename, start, last);
		return true;
	}
	else
	{
		p_Main->errorMessage("Error reading " + fileName);
		return false;
	}
}

bool Cdp1802::readLstFile(wxString fileName, int memoryType, long end, bool showFilename)
{
	wxTextFile inFile;
	wxString line, strValue;
	long address = 0;
	long value;
	bool overloaded = false;
	Word start = 0xffff;
	Word last = 0;

	if (inFile.Open(fileName))
	{
		for (line=inFile.GetFirstLine(); !inFile.Eof(); line=inFile.GetNextLine())
		{
			if (line.GetChar(0) != ' ')
			{
				strValue = line.Mid(0, 4);
				strValue.ToLong(&address, 16);
			}

			if (line.GetChar(4) != ' ')
			{
				size_t i=6;
				value = 0;
				Byte bit = 0x80;
				while ((line.GetChar(i) == ' ') || (line.GetChar(i) == 'x'))
				{
					if (line.GetChar(i) == 'x')
						value |= bit;
					bit >>= 1;
					i++;
				}
				if (memoryType != NOCHANGE && memoryType != RAM)
					defineMemoryType(address, memoryType);
				writeMem(address,(Byte)value, true);
				address++;
/*				size_t i=6;
				for (int j=0; j<10;j++)
				{
					value = 0;
					Byte bit = 0x80;
					while ((line.GetChar(i) == '0') || (line.GetChar(i) == '1'))
					{
						if (line.GetChar(i) == '1')
							value |= bit;
						bit >>= 1;
						i++;
					}
					defineMemoryType(address, memoryType);
					writeMem(address,(Byte)value, true);
					address++;
					i+=2;
				}*/
			}
			else
			{
				size_t i=5;
				while (line.GetChar(i) != ' ')
				{
					strValue = line.Mid(i, 2);
					if (strValue.ToLong(&value, 16))
					{
						value &= 255;
						if (memoryType != NOCHANGE && memoryType != RAM)
							defineMemoryType(address, memoryType);
						if (address < end)
							writeMem(address,(Byte)value, true);
						else
							overloaded = true;
						address++;
						i+=2;
					}
				}
			}
		}
		inFile.Close();
		if (overloaded)
		{
			wxString endStr;
			endStr.Printf("%04X", (int)end);
			p_Main->errorMessage("Attempt to load after address " + endStr);
		}
		setAddress(showFilename, start, last);
		return true;
	}
	else
	{
		p_Main->errorMessage("Error reading " + fileName);
		return false;
	}
}

void Cdp1802::saveIntelFile(wxString fileName, long start, long end)
{
	wxTextFile outputFile;
	wxString line, byteStr;
	int checkSum;

	if (!fileName.empty())
	{
		if (wxFile::Exists(fileName))
		{
			outputFile.Open(fileName);
			outputFile.Clear();
		}
		else
			outputFile.Create(fileName);

		while (start < end)
		{
			line.Printf(":%02X%04X%02X", 0x10, (int)start, 0x00);
			checkSum = 0x10+((start>>8)&0xff)+(start&0xff);
			for (int i = 0; i<16; i++)
			{
				checkSum += readMem(start);
				byteStr.Printf("%02X", readMem(start));
				line += byteStr;
				start++;
			}
			checkSum = ((checkSum ^ 0xff) + 1) & 0xff;
			byteStr.Printf("%02X", checkSum);
			line += byteStr;
			outputFile.AddLine(line);
		}
		outputFile.AddLine(":00000001FF");
		outputFile.Write();
	}
}

void Cdp1802::saveBinFile(wxString fileName, long start, long end)
{
	wxFile outputFile;
	size_t length;
	char buffer[65536];

	if (!fileName.empty())
	{
		if (outputFile.Create(fileName, true))
		{
			length = end-start+1;
			for (size_t i=0; i<length; i++)
			{
				buffer[i] = p_Computer->readMem(start);
				start++;
			}
			outputFile.Write(buffer, length);
			outputFile.Close();
		}
		else
		{
			p_Main->errorMessage("Error writing " + fileName);
			return;
		}
	}
}

bool Cdp1802::readBinFile(wxString fileName, int memoryType, Word address, long end, bool showFilename, bool showAddressPopup, Word specifiedStartAddress)
{
	wxFFile inFile;
	size_t length;
	char buffer[65535];
	bool overloaded = false;
	Word start;

	start = address;
	if (showAddressPopup && specifiedStartAddress != 0)
	{
		p_Main->eventShowAddressPopup(specifiedStartAddress);

		int answer = p_Main->getAddressPopupAnswer();
#if defined (__WXMAC__)
		if (answer == wxID_YES)
			start = specifiedStartAddress;
#else
		if (answer == wxID_NO)
			start = specifiedStartAddress;
#endif
		if (answer == wxID_CANCEL)
			return false;
	}
    
    address = start;
    
	if (inFile.Open(fileName, _("rb")))
	{
		length = inFile.Read(buffer, 65535);
		for (size_t i=0; i<length; i++)
		{
			if (memoryType != NOCHANGE && memoryType != RAM)
				defineMemoryType(address, memoryType);
			if (address < end)
				writeMem(address,(Byte)buffer[i], true);
			else
				overloaded = true;
			address++;
            
            if (computerType_ == STUDIO && address == 0x800 && start == 0x300)
                address =  0xc00;
		}
		inFile.Close();
		if (overloaded)
		{
			wxString endStr;
			endStr.Printf("%04X", (int)end);
			p_Main->errorMessage("Attempt to load after address " + endStr);
		}
		setAddress(showFilename, start, address-1);
		return true;
	}
	else
	{
		p_Main->errorMessage("Error reading " + fileName);
		return false;
	}
}

bool Cdp1802::readBinFile(wxString fileName, int memoryType, Word address, Word* lastAddress, long end, bool showFilename)
{
	wxFFile inFile;
	size_t length;
	char buffer[65535];
	bool overloaded = false;
	Word start;

	start = address;
	if (inFile.Open(fileName, _("rb")))
	{
		length = inFile.Read(buffer, 65535);
		for (size_t i=0; i<length; i++)
		{
			if (memoryType != NOCHANGE && memoryType != RAM)
				defineMemoryType(address, memoryType);
			if (address < end)
				writeMem(address,(Byte)buffer[i], true);
			else
				overloaded = true;
			address++;
		}
		inFile.Close();
		*lastAddress = address-1;
		if (overloaded)
		{
			wxString endStr;
			endStr.Printf("%04X", (int)end);
			p_Main->errorMessage("Attempt to load after address " + endStr);
		}
		setAddress(showFilename, start, address-1);
		return true;
	}
	else
	{
		p_Main->errorMessage("Error reading " + fileName);
		return false;
	}
}

bool Cdp1802::readRomMapperBinFile(wxString fileName)
{
    wxFFile inFile;
    wxFileOffset length=0;

    if (!inFile.Open(fileName, _("rb")))
    {
        p_Main->errorMessage("Error reading " + fileName);
        return false;
    }
    
    length = inFile.Length();
    allocRomMapperMemory(length);

    inFile.Read(expansionRom_, (size_t)length);
    
    inFile.Close();
    return true;
}

bool Cdp1802::readMultiCartBinFile(wxString dirName, wxString fileName)
{
    wxFFile inFile;
    size_t length=0;
    
    if (!inFile.Open(dirName + fileName, _("rb")))
    {
        p_Main->errorMessage("Error reading " + dirName + fileName);
        return false;
    }
    
    wxFileName FullPath = wxFileName(dirName + fileName, wxPATH_NATIVE);
    wxString ext = FullPath.GetExt();
    
    if (fileName == "" || ext == "st2" || ext == "ch8")
    {
        p_Main->errorMessage("No multi-cart ROM detected in CART");
        return false;
    }
 
    length = (size_t)inFile.Length();
    size_t allocSize = allocMultiCartMemory(length);
    
    if (allocSize < length)
        length = allocSize;
    
    inFile.Read(multiCartRom_, (size_t)length);
    
    inFile.Close();
    return true;
}

void Cdp1802::setAddress(bool showFilename, Word start, Word end)
{
	if (showFilename)
	{
		wxString gui = p_Main->getRunningComputerStr();
		wxString valueString;
		switch (computerType_) 
		{
			case ETI:
				p_Main->eventSaveStart(start);
				p_Main->eventSaveEnd(end);
				writeMem(0x400, (start>>8) & 0xff, false);
				writeMem(0x401, (start & 0xff), false);
				writeMem(0x402, ((end)>>8) & 0xff, false);
				writeMem(0x403, ((end) & 0xff), false);
			break;

			case CIDELSA:
			case STUDIO:
            case COINARCADE:
            case FRED:
			case VISICOM:
			case VICTORY:
            case STUDIOIV:
			break;

			default:
				p_Main->eventSaveStart(start);
				p_Main->eventSaveEnd(end);
			break;

		}
	}
	if ((computerType_ == ELF) || (computerType_ == ELFII) || (computerType_ == SUPERELF))
	{
		if ((mainMemory_[start] == 0x90) && (mainMemory_[start+1] == 0xa1) && (mainMemory_[start+2] == 0xb3))
		{
			baseGiantBoard_ = start;
		}
		if ((mainMemory_[start+0x55] == 0xd5) && (mainMemory_[start+0x56] == 0xb8) && (mainMemory_[start+0x57] == 0xd5) && (mainMemory_[start+0x58] == 0xa8))
		{
			baseQuestLoader_ = start;
		}
	}
	checkLoadedSoftware();
}

void Cdp1802::checkLoadedSoftware()
{
	if (loadedProgram_ == NOPROGRAM)
	{
		if ((computerType_ == ELFII) || (computerType_ == SUPERELF) || (computerType_ == ELF))
		{
			if ((mainMemory_[0x1c] == 0xd4) && (mainMemory_[0x1d] == 0x96) && (mainMemory_[0x1e] == 0xb7) && (mainMemory_[0x6f] == 0x05))
			{
				chip8baseVar_ = 0xef0;
				chip8mainLoop_ = 0x1d;
				chip8type_ = CHIP8;
			}
			if ((mainMemory_[0] == 0xc0) && (mainMemory_[1] == 0x25) && (mainMemory_[2] == 0xf4))
			{
				loadedProgram_ = SUPERBASICV1;
				basicExecAddress_[BASICADDR_KEY] = BASICADDR_KEY_SB1;
				basicExecAddress_[BASICADDR_READY] = BASICADDR_READY_SB1;
				basicExecAddress_[BASICADDR_KEY_VT_RESTART] = BASICADDR_VT_RESTART_SB1;
				basicExecAddress_[BASICADDR_KEY_VT_INPUT] = BASICADDR_VT_INPUT_SB1;
				p_Main->eventEnableMemAccess(true);
			}
			if ((mainMemory_[0] == 0xc0) && (mainMemory_[1] == 0x1d) && (mainMemory_[2] == 0x39) && (mainMemory_[3] == 0xc0))
			{
				loadedProgram_ = SUPERBASICV3;
				basicExecAddress_[BASICADDR_KEY] = BASICADDR_KEY_SB3;
				basicExecAddress_[BASICADDR_READY] = BASICADDR_READY_SB3;
				basicExecAddress_[BASICADDR_KEY_VT_RESTART] = BASICADDR_VT_RESTART_SB3;
				basicExecAddress_[BASICADDR_KEY_VT_INPUT] = BASICADDR_VT_INPUT_SB3;
				p_Main->eventEnableMemAccess(true);
			}
			if ((mainMemory_[0x100] == 0xc0) && (mainMemory_[0x101] == 0x18) && (mainMemory_[0x102] == 0x00) && (mainMemory_[0x103] == 0xc0))
			{
				loadedProgram_ = SUPERBASICV5;
				basicExecAddress_[BASICADDR_KEY] = BASICADDR_KEY_SB5;
				basicExecAddress_[BASICADDR_READY] = BASICADDR_READY_SB5;
				basicExecAddress_[BASICADDR_KEY_VT_RESTART] = BASICADDR_VT_RESTART_SB5;
				basicExecAddress_[BASICADDR_KEY_VT_INPUT] = BASICADDR_VT_INPUT_SB5;
				p_Main->eventEnableMemAccess(true);
			}
			if ((mainMemory_[0x100] == 0xc0) && (mainMemory_[0x101] == 0x2f) && (mainMemory_[0x102] == 0x00) && (mainMemory_[0x103] == 0xc0))
			{
				loadedProgram_ = SUPERBASICV6;
				basicExecAddress_[BASICADDR_KEY] = BASICADDR_KEY_SB6;
				basicExecAddress_[BASICADDR_READY] = BASICADDR_READY_SB6;
				basicExecAddress_[BASICADDR_KEY_VT_RESTART] = BASICADDR_VT_RESTART_SB6;
				basicExecAddress_[BASICADDR_KEY_VT_INPUT] = BASICADDR_VT_INPUT_SB6;
				p_Main->eventEnableMemAccess(true);
			}
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
			if ((mainMemory_[0xc000] == 0x90) && (mainMemory_[0xc001] == 0xb4) && (mainMemory_[0xc002] == 0xb5) && (mainMemory_[0xc003] == 0xfc))
			{
				loadedProgram_ = MINIMON;
				p_Main->eventEnableMemAccess(true);
			}
			if ((mainMemory_[0xc000] == 0xc4) && (mainMemory_[0xc001] == 0xb4) && (mainMemory_[0xc002] == 0xf8) && (mainMemory_[0xc003] == 0xc0))
			{
				loadedProgram_ = GOLDMON;
				p_Main->eventEnableMemAccess(true);
			}
			if ((mainMemory_[0x100] == 0xc4) && (mainMemory_[0x101] == 0x30) && (mainMemory_[0x102] == 0xb0))
			{
				loadedProgram_ = TINYBASIC;
				p_Main->eventEnableMemAccess(true);
			}
		}
		if (computerType_ == COSMICOS)
		{
			if ((mainMemory_[0xc0f7] == 0x22) && (mainMemory_[0xc0f8] == 0x73) && (mainMemory_[0xc0f9] == 0x3e) && (mainMemory_[0xc0fa] == 0))
				loadedProgram_ = HEXMON;
			if ((mainMemory_[0xc084] == 0x4e) && (mainMemory_[0xc085] == 0x4f) && (mainMemory_[0xc086] == 0x20) && (mainMemory_[0xc087] == 0x43))
				loadedProgram_ = ASCIIMON;
		}
		if (computerType_ == VIP || computerType_ == VELF)
		{
			if ((mainMemory_[0x1025] == 0x42) && (mainMemory_[0x1026] == 0x41) && (mainMemory_[0x1027] == 0x53) && (mainMemory_[0x1028] == 0x49))
			{
				loadedProgram_ = FPBBASIC;
				p_Main->eventEnableMemAccess(true);
			}
			else
			{
				if ((mainMemory_[0xa0] == 0xd3) && (mainMemory_[0xa1] == 0xf8) && (mainMemory_[0xa2] == 0x40) && (mainMemory_[0xa3] == 0xb9))
				loadedProgram_ = FPBBOOT;
			}
		}
		if (computerType_ == MEMBER)
		{
			if ((mainMemory_[0x2f] == 0xd3) && (mainMemory_[0x30] == 0xbf) && (mainMemory_[0x31] == 0xe2) && (mainMemory_[0x32] == 0x86))
			{
				loadedProgram_ = MONITOR_CHUCK_LOW;
			}
			if ((mainMemory_[0x802f] == 0xd3) && (mainMemory_[0x8030] == 0xbf) && (mainMemory_[0x8031] == 0xe2) && (mainMemory_[0x8032] == 0x86))
			{
				loadedProgram_ = MONITOR_CHUCK_HIGH;
			}
		}
	}
	if (loadedOs_ == NOOS)
	{
		if ((computerType_ == ELFII) || (computerType_ == SUPERELF) || (computerType_ == ELF))
		{
			if ((mainMemory_[0xf900] == 0xf8) && (mainMemory_[0xf901] == 0xf9) && (mainMemory_[0xf902] == 0xb6))
			{
				loadedOs_ = ELFOS;
			}
		}
	}
}

bool Cdp1802::readProgram(wxString romDir, wxString rom, int memoryType, Word address, bool showFilename)
{
	if (rom.Len() != 0)
	{
		return readFile(romDir+rom, memoryType, address, 0x10000, showFilename);
	}
	else return false;
}

bool Cdp1802::readProgram(wxString romDir, wxString rom, int memoryType, Word address, Word* lastAddress, bool showFilename)
{
	if (rom.Len() != 0)
	{
		return readFile(romDir+rom, memoryType, address, lastAddress, 0x10000, showFilename);
	}
	else return false;
}

bool Cdp1802::readProgramCidelsa(wxString romDir, wxString rom, int memoryType, Word address, bool showFilename)
{
	bool ret;

	if ((rom == "des a 2.ic4") || (rom == "des b 2.ic5") || (rom == "des c 2.ic6") || (rom == "des d 2.ic7"))
	{
		ret = readFile(romDir + "des a 2.ic4", memoryType, address, 0x10000, showFilename);
		ret = ret | readFile(romDir + "des b 2.ic5", memoryType, address+0x800, 0x10000, showFilename);
		ret = ret | readFile(romDir + "des c 2.ic6", memoryType, address+0x1000, 0x10000, showFilename);
		ret = ret | readFile(romDir + "des d 2.ic7", memoryType, address+0x1800, 0x10000, showFilename);
		return ret;
	}

	if ((rom == "destryea_1") || (rom == "destryea_2") || (rom == "destryea_3") || (rom == "destryea_4"))
	{
		ret = readFile(romDir + "destryea_1", memoryType, address, 0x10000, showFilename);
		ret = ret | readFile(romDir + "destryea_2", memoryType, address+0x800, 0x10000, showFilename);
		ret = ret | readFile(romDir + "destryea_3" , memoryType, address+0x1000, 0x10000, showFilename);
		ret = ret | readFile(romDir + "destryea_4", memoryType, address+0x1800, 0x10000, showFilename);
		return ret;
	}

	if ((rom == "alt a 1.ic7") || (rom == "alt b 1.ic8") || (rom == "alt c 1.ic9") || (rom == "alt d 1.ic10") || (rom == "alt e 1.ic11") || (rom == "alt f 1.ic12"))
	{
		ret = readFile(romDir + "alt a 1.ic7", memoryType, address, 0x10000, showFilename);
		ret = ret | readFile(romDir + "alt b 1.ic8", memoryType, address+0x800, 0x10000, showFilename);
		ret = ret | readFile(romDir + "alt c 1.ic9" , memoryType, address+0x1000, 0x10000, showFilename);
		ret = ret | readFile(romDir + "alt d 1.ic10" , memoryType, address+0x1800, 0x10000, showFilename);
		ret = ret | readFile(romDir + "alt e 1.ic11" , memoryType, address+0x2000, 0x10000, showFilename);
		ret = ret | readFile(romDir + "alt f 1.ic12", memoryType, address+0x2800, 0x10000, showFilename);
		return ret;
	}

	if ((rom == "dra a 1.ic10") || (rom == "dra b 1.ic11") || (rom == "dra c 1.ic12") || (rom == "dra d 1.ic13") || (rom == "dra e 1.ic14") || (rom == "dra f 1.ic15") || (rom == "dra g 1.ic16") || (rom == "dra h 1.ic17"))
	{
		ret = readFile(romDir + "dra a 1.ic10", memoryType, address, 0x10000, showFilename);
		ret = ret | readFile(romDir + "dra b 1.ic11", memoryType, address+0x800, 0x10000, showFilename);
		ret = ret | readFile(romDir + "dra c 1.ic12" , memoryType, address+0x1000, 0x10000, showFilename);
		ret = ret | readFile(romDir + "dra d 1.ic13" , memoryType, address+0x1800, 0x10000, showFilename);
		ret = ret | readFile(romDir + "dra e 1.ic14" , memoryType, address+0x2000, 0x10000, showFilename);
		ret = ret | readFile(romDir + "dra f 1.ic15" , memoryType, address+0x2800, 0x10000, showFilename);
		ret = ret | readFile(romDir + "dra g 1.ic16" , memoryType, address+0x3000, 0x10000, showFilename);
		ret = ret | readFile(romDir + "dra h 1.ic17", memoryType, address+0x3800, 0x10000, showFilename);
		return ret;
	}

	return readProgram(romDir, rom, memoryType, address, showFilename);
}

bool Cdp1802::readProgramTmc600(wxString romDir, wxString rom, int memoryType, Word address, bool showFilename)
{
	bool ret;

	if ((rom == "sb30") || (rom == "sb31") || (rom == "sb32") || (rom == "sb33") || (rom == "151182"))
	{
		ret = readFile(romDir + "sb30", memoryType, address, 0x10000, showFilename);
		ret = ret | readFile(romDir + "sb31", memoryType, address+0x1000, 0x10000, showFilename);
		ret = ret | readFile(romDir + "sb32", memoryType, address+0x2000, 0x10000, showFilename);
		ret = ret | readFile(romDir + "sb33", memoryType, address+0x3000, 0x10000, showFilename);
		return ret;
	}

	return readProgram(romDir, rom, memoryType, address, showFilename);
}

bool Cdp1802::readProgramPecom(wxString romDir, wxString rom, int memoryType, Word address, bool showFilename)
{
	bool ret;

	if ((rom == "pecom64-1.bin") || (rom == "pecom64-2.bin"))
	{
		ret = readFile(romDir + "pecom64-1.bin", memoryType, address, 0x10000, showFilename);
		ret = ret | readFile(romDir + "pecom64-2.bin", memoryType, address+0x4000, 0x10000, showFilename);
		return ret;
	}

	if ((rom == "170887-rom1.bin") || (rom == "170887-rom2.bin"))
	{
		ret = readFile(romDir + "170887-rom1.bin", memoryType, address, 0x10000, showFilename);
		ret = ret | readFile(romDir + "170887-rom2.bin", memoryType, address+0x4000, 0x10000, showFilename);
		return ret;
	}

	return readProgram(romDir, rom, memoryType, address, showFilename);
}

void Cdp1802::readSt2Program(int computerType)
{
	wxString fileName, file;
	wxFFile inFile;
	struct
	{
		char header[4];
		Byte numBlocks;
		Byte format;
		Byte video;
		Byte notUsed1;
		char author[2];
		char dumper[2];
		Byte notUsed2[4];
		char catalogue[10];
		Byte notUsed3[6];
		char title[32];
		Byte offsets[64];
		Byte notUsed4[128];
	} st2Header;

	fileName = p_Main->getRomDir(computerType, CARTROM);
	file = p_Main->getRomFile(computerType, CARTROM);
	fileName.operator += (file);

	if (file.Len() != 0)
	{
		if (wxFile::Exists(fileName))
		{
			if (inFile.Open(fileName, _(_("rb"))))
			{
				inFile.Read(&st2Header, 256);
                if (st2Header.offsets[0] == 0 && st2Header.offsets[4] == 0x24)
                    allocTestCartMemory();
				for (int i=1; i<st2Header.numBlocks; i++)
				{
                    if (st2Header.offsets[0] == 0 && st2Header.offsets[4] == 0x24)
                    {
                        inFile.Read((&testCartRom_[st2Header.offsets[i-1] << 8]),256);
                        defineMemoryType(st2Header.offsets[i-1] << 8, TESTCARTRIDGEROM);
                    }
                    else
                    {
                        inFile.Read((&mainMemory_[st2Header.offsets[i-1] << 8]),256);
                        defineMemoryType(st2Header.offsets[i-1] << 8, CARTRIDGEROM);
                    }
				}
				inFile.Close();
				wxFileName swFullPath = wxFileName(fileName, wxPATH_NATIVE);
				p_Main->setSwName (swFullPath.GetName());
                p_Main->updateTitle();
			}
			else
			{
				(void)wxMessageBox( "Error reading " + fileName,  // Works correct, via p_Main->errorMessage it will NOT
								    "Emma 02", wxICON_ERROR | wxOK );
			}
		}
		else
		{
			(void)wxMessageBox( "File " + fileName + " not found", // Works correct, via p_Main->errorMessage it will NOT
							    "Emma 02", wxICON_ERROR | wxOK );
		}
	}
}

bool Cdp1802::readFile(wxString fileName, int memoryType, Word address, long end, bool showFilename)
{
	wxFFile inFile;
	char buffer[4];

	if (wxFile::Exists(fileName))
	{
		if (inFile.Open(fileName, _("rb")))
		{
			inFile.Read(buffer, 4);
			inFile.Close();

			if (showFilename)
			{
				wxFileName swFullPath = wxFileName(fileName, wxPATH_NATIVE); 
				p_Main->setSwName (swFullPath.GetName());
                p_Main->updateTitle();
			}

			if (buffer[0] == ':' || (buffer[0] == 0x0d && buffer[1] == 0x0a && buffer[2] == ':'))
				return readIntelFile(fileName, memoryType, end, showFilename);
			else if (buffer[0] == '0' && buffer[1] == '0' && buffer[2] == '0' && buffer[3] == '0')
				return readLstFile(fileName, memoryType, end, showFilename);
			else
				return readBinFile(fileName, memoryType, address, end, showFilename, false, 0);
		}
		else
		{
			p_Main->errorMessage("Error reading " + fileName);
			return false;
		}
	}
	else
	{
		p_Main->errorMessage("File " + fileName + " not found");
		return false;
	}
}

bool Cdp1802::readFile(wxString fileName, int memoryType, Word address, long end, bool showFilename, bool showAddressPopup, Word specifiedStartAddress)
{
    wxFFile inFile;
    char buffer[4];
    
    if (wxFile::Exists(fileName))
    {
        if (inFile.Open(fileName, _("rb")))
        {
            inFile.Read(buffer, 4);
            inFile.Close();
            
            if (showFilename)
            {
                wxFileName swFullPath = wxFileName(fileName, wxPATH_NATIVE);
                p_Main->setSwName(swFullPath.GetName());
                p_Main->updateTitle();
            }
            
            if (buffer[0] == ':')
                return readIntelFile(fileName, memoryType, end, showFilename);
            else if (buffer[0] == '0' && buffer[1] == '0' && buffer[2] == '0' && buffer[3] == '0')
                return readLstFile(fileName, memoryType, end, showFilename);
            else
                return readBinFile(fileName, memoryType, address, end, showFilename, showAddressPopup, specifiedStartAddress);
        }
        else
        {
            p_Main->errorMessage("Error reading " + fileName);
            return false;
        }
    }
    else
    {
        p_Main->errorMessage("File " + fileName + " not found");
        return false;
    }
}

bool Cdp1802::readFile(wxString fileName, int memoryType, Word address, Word* lastAdress, long end, bool showFilename)
{
	wxFFile inFile;
	char buffer[4];

	if (wxFile::Exists(fileName))
	{
		if (inFile.Open(fileName, _("rb")))
		{
			inFile.Read(buffer, 4);
			inFile.Close();

			if (showFilename)
			{
				wxFileName swFullPath = wxFileName(fileName, wxPATH_NATIVE); 
				p_Main->setSwName (swFullPath.GetName());
                p_Main->updateTitle();
			}

			if (buffer[0] == ':')
				return readIntelFile(fileName, memoryType, lastAdress, end, showFilename);
			else if (buffer[0] == '0' && buffer[1] == '0' && buffer[2] == '0' && buffer[3] == '0')
				return readLstFile(fileName, memoryType, end, showFilename);
			else
				return readBinFile(fileName, memoryType, address, lastAdress, end, showFilename);
		}
		else
		{
			p_Main->errorMessage("Error reading " + fileName);
			return false;
		}
	}
	else
	{
		p_Main->errorMessage("File " + fileName + " not found");
		return false;
	}
}

void Cdp1802::setDebugMode (bool debugModeNew, bool debugChip8ModeNew, bool trace, bool traceDma, bool traceInt, bool traceChip8Int)
{
	if (debugMode_ && !debugModeNew)
	{
		trace_ = false;
		debugMode_ = false;
	}
	else
	{
		debugMode_ = debugModeNew;
		trace_ = trace;
		traceDma_ = traceDma;
		traceInt_ = traceInt;
	}

	if (chip8DebugMode_ && !debugChip8ModeNew)
	{
		chip8DebugMode_ = false;
	}
	else
	{
		chip8DebugMode_ = debugChip8ModeNew;
		traceChip8Int_ = traceChip8Int;
	}
}

void Cdp1802::debugTrace (wxString text)
{
	if (trace_)
		p_Main->debugTrace(text);
}

void Cdp1802::writeMemLabelType(Word address, Byte type)
{
	switch (memoryType_[address / 256])
	{
        case ROM:
		case COMXEXPROM:
		case NVRAM:
        case CARTRIDGEROM:
        case VP570RAM:
           switch (computerType_)
            {
                case ELFII:
                    if (elfConfiguration.giantBoardMapping)
                        if (address >= baseGiantBoard_)
                            address = (address & 0xff) | 0xf000;
                    break;
                
                case COSMICOS:
                case MCDS:
                case MS2000:
                case ELF2K:
                    address = address | bootstrap_;
                break;
                
                case PECOM:
                    address = address | addressLatch_;
                break;
                    
                case ETI:
                    address = address & ramMask_;
                break;

                case VELF:
                    if (address < 0x8000)
                        address = (address | addressLatch_);
                break;
                    
                case NANO:
                    if (address < 0x8000)
                        address = (address | addressLatch_) & 0xfff;
                break;
  
                case TMC1800:
                case TMC2000:
                    if (address < 0x8000)
                        address = (address | addressLatch_) & (ramMask_ | 0x8000);
                break;
                    
                case VIP:
                    if (address < setLatch_)
                        address = (address | addressLatch_);
                    else
                        address = address & romMask_;
                break;

                case VIPII:
                    if (address < 0x8000)
                        address = (address | addressLatch_);
                    else
                        address = address & 0x81ff;
                break;
            }
			if (type > mainMemoryLabelType_[address] || type == 0)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryLabelType_[address] = type;
            }
		break;

        case RAM:
            switch (computerType_)
            {
                case ELF:
                case ELFII:
                case SUPERELF:
                    address = (address & ramMask_) + ramStart_;
                break;
                
                case COSMICOS:
                case MCDS:
                case MS2000:
                case ELF2K:
                    address = address | bootstrap_;
                break;
                
                case PECOM:
                    address = address | addressLatch_;
                break;
                    
                case ETI:
                    address = address & ramMask_;
                break;
                
                case VELF:
                    if (address < 0x8000)
                        address = (address | addressLatch_);
                break;
                    
                case NANO:
                    if (address < 0x8000)
                        address = (address | addressLatch_) & 0xfff;
                break;
                    
                case TMC1800:
                case TMC2000:
                    if (address < 0x8000)
                        address = (address | addressLatch_) & (ramMask_ | 0x8000);
                break;
                    
                case VIP:
                    if (address < setLatch_)
                        address = (address | addressLatch_);
                    else
                        address = address & romMask_;
                    address = address & ramMask_;
               break;

                case VIPII:
                    if (address < 0x8000)
                        address = (address | addressLatch_);
                    else
                        address = address & 0x81ff;
                break;
            }
            if (type > mainMemoryLabelType_[address] || type == 0)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryLabelType_[address] = type;
            }
        break;
            
        case MAPPEDRAM:
            switch (computerType_)
            {
                case ELF:
                case ELFII:
                case SUPERELF:
                    address = (address & ramMask_) + ramStart_;
                break;
                
                case COSMICOS:
                    address = address | bootstrap_;
                break;

                case FRED:
                    address = address & 0x7ff;
                break;
       
                case STUDIO:
                case COINARCADE:
                case VICTORY:
                case VISICOM:
                    address = (address & 0x1ff) | 0x800;
                break;
                
                case VIP:
                    if (address < setLatch_)
                        address = (address | addressLatch_);
                    else
                        address = address & romMask_;
                    address = address & ramMask_;
                break;
            }
            if (type > mainMemoryLabelType_[address] || type == 0)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryLabelType_[address] = type;
            }
        break;
            
        case COMXEXPBOX:
			switch (expansionMemoryType_[expansionSlot_ * 32 + (address & 0x1fff) / 256])
			{
				case RAMBANK:
					switch (bankMemoryType_[ramBank_ * 32 + (address & 0x1fff) / 256])
					{
						case ROM:
						case RAM:
							if (type > expansionRamLabelType_[(ramBank_ * 0x2000) + (address & 0x1fff)] || type == 0)
							{
								p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
								expansionRamLabelType_[(ramBank_ * 0x2000) + (address & 0x1fff)] = type;
							}
						break;
					}
				break;

				case EPROMBANK:
					switch (epromBankMemoryType_[epromBank_ * 32 + (address & 0x1fff) / 256])
					{
						case ROM:
						case RAM:
							if (type > expansionEpromLabelType_[(epromBank_ * 0x2000) + (address & 0x1fff)] || type == 0)
							{
								p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
								expansionEpromLabelType_[(epromBank_ * 0x2000) + (address & 0x1fff)] = type;
							}
						break;
					}
				break;

				case SUPERBANK:
					switch (superBankMemoryType_[(epromBank_ + (8 * ramSwitched_)) * 32 + (address & 0x1fff) / 256])
					{
						case ROM:
						case RAM:
							if (type > expansionSuperLabelType_[((epromBank_ + (8 * ramSwitched_)) * 0x2000) + (address & 0x1fff)] || type == 0)
							{
								p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
								expansionSuperLabelType_[((epromBank_ + (8 * ramSwitched_)) * 0x2000) + (address & 0x1fff)] = type;
							}
						break;
					}
				break;

				case ROM:
					if (type > expansionRomLabelType_[(expansionSlot_ * 0x2000) + (address & 0x1fff)] || type == 0)
					{
						p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
						expansionRomLabelType_[(expansionSlot_ * 0x2000) + (address & 0x1fff)] = type;
					}
				break;
			}
		break;

		case PRAM1870:
        case CRAM1870:
            if (computerType_ == PECOM)
            {
                if (type > mainMemoryLabelType_[address] || type == 0)
                {
                    p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                    mainMemoryLabelType_[address] = type;
                }
            }
            else
            {
                if (epromSlot_ != 0xff || superSlot_ != 0xff)
                {
                    if (type > mainMemoryLabelType_[address] || type == 0)
                    {
                        p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                        mainMemoryLabelType_[address] = type;
                    }
                }
			}
		break;

		case COPYCOMXEXPROM:
			if (type > mainMemoryLabelType_[(address & 0xfff) | 0xe000] || type == 0)
			{
				p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
				mainMemoryLabelType_[(address & 0xfff) | 0xe000] = type;
			}
		break;

		case COPYFLOPROM:
			if (((address & 0xff) >= 0xd0) && ((address & 0xff) <= 0xdf))
			{
				if (type > expansionRomLabelType_[(expansionSlot_ * 0x2000) + (address & 0xfff)] || type == 0)
				{
					p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
					expansionRomLabelType_[(expansionSlot_ * 0x2000) + (address & 0xfff)] = type;
				}
			}
			else
			{
				if (type > mainMemoryLabelType_[address] || type == 0)
				{
					p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
					mainMemoryLabelType_[address] = type;
				}
			}
		break;

        case EMSMEMORY:
            switch (emsMemoryType_[((address & 0x3fff) |(emsPage_ << 14))/256])
            {
                case ROM:
                case RAM:
                if (type > emsRamLabelType_[(long) ((address & 0x3fff) |(emsPage_ << 14))] || type == 0)
                {
                    p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                    emsRamLabelType_[(long) ((address & 0x3fff) |(emsPage_ << 14))] = type;
                }
            break;
            }
        break;
            
        case ROMMAPPER:
            if (emsPage_ <= maxNumberOfPages_)
            {
                switch (romMapperMemoryType_[((address & 0x7fff) |(emsPage_ << 15))/256])
                {
                    case ROM:
                    case RAM:
                        if (type > expansionRomLabelType_[(long) ((address & 0x7fff) |(emsPage_ << 15))] || type == 0)
                        {
                            p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                            expansionRomDataType_[(long) ((address & 0x7fff) |(emsPage_ << 15))] = type;
                        }
                    break;
                }
            }
        break;
            
        case PAGER:
            switch (pagerMemoryType_[((getPager(address>>12) << 12) |(address &0xfff))/256])
            {
                case ROM:
                case RAM:
                    if (type > mainMemoryLabelType_[(getPager(address>>12) << 12) |(address &0xfff)] || type == 0)
                    {
                        p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                        mainMemoryDataType_[(getPager(address>>12) << 12) |(address &0xfff)] = type;
                    }
                break;
            }
        break;
            
        case MULTICART:
            if ((address < 0x400) && !disableSystemRom_)
            {
                if (type > mainMemoryLabelType_[address] || type == 0)
                {
                    p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                    mainMemoryLabelType_[address] = type;
                }
            }
            else
            {
                if (type > multiCartRomLabelType_[(address + multiCartLsb_ * 0x1000 + multiCartMsb_ * 0x10000)&multiCartMask_] || type == 0)
                {
                    p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                    multiCartRomLabelType_[(address + multiCartLsb_ * 0x1000 + multiCartMsb_ * 0x10000)&multiCartMask_] = type;
                }
            }
        break;

        case MAPPEDMULTICART:
            address = address & 0xfff;
            if ((address < 0x400) && !disableSystemRom_)
            {
                if (type > mainMemoryLabelType_[address] || type == 0)
                {
                    p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                    mainMemoryLabelType_[address] = type;
                }
            }
            else
            {
                if (type > multiCartRomLabelType_[(address + multiCartLsb_ * 0x1000 + multiCartMsb_ * 0x10000)&multiCartMask_] || type == 0)
                {
                    p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                    multiCartRomLabelType_[(address + multiCartLsb_ * 0x1000 + multiCartMsb_ * 0x10000)&multiCartMask_] = type;
                }
            }
        break;
    }
}

Byte Cdp1802::readMemLabelType(Word address)
{
    switch (memoryType_[address / 256])
	{
        case ROM:
        case NVRAM:
        case COMXEXPROM:
        case CARTRIDGEROM:
        case VP570RAM:
            switch (computerType_)
            {
                case ELFII:
                    if (elfConfiguration.giantBoardMapping)
                        if (address >= baseGiantBoard_)
                            address = (address & 0xff) | 0xf000;
                break;
                    
                case COSMICOS:
                case MCDS:
                case MS2000:
                case ELF2K:
                    address = address | bootstrap_;
                break;
                    
                case PECOM:
                    address = address | addressLatch_;
                break;
                    
                case ETI:
                    address = address & ramMask_;
                break;
                    
                case VELF:
                    if (address < 0x8000)
                        address = (address | addressLatch_);
                break;
                    
                case NANO:
                    if (address < 0x8000)
                        address = (address | addressLatch_) & 0xfff;
                break;
                    
                case TMC2000:
                case TMC1800:
                    if (address < 0x8000)
                        address = (address | addressLatch_) & (ramMask_ | 0x8000);
                    else
                        address = address & 0x81ff;
                break;

                case VIP:
                    if (address < setLatch_)
                        address = (address | addressLatch_);
                    else
                        address = address & romMask_;
                break;
                    
                case VIPII:
                    if (address < 0x8000)
                        address = (address | addressLatch_);
                    else
                        address = address & 0x81ff;
                break;
            }
            return mainMemoryLabelType_[address];
		break;
            
        case RAM:
            switch (computerType_)
            {
                case ELF:
                case ELFII:
                case SUPERELF:
                    address = (address & ramMask_) + ramStart_;
                break;
                
                case COSMICOS:
                case MCDS:
                case MS2000:
                case ELF2K:
                    address = address | bootstrap_;
                break;
                
                case PECOM:
                    address = address | addressLatch_;
                break;
                    
                case ETI:
                    address = address & ramMask_;
                break;
                    
                case VELF:
                    if (address < 0x8000)
                        address = (address | addressLatch_);
                break;
                    
                case NANO:
                    if (address < 0x8000)
                        address = (address | addressLatch_) & 0xfff;
                break;

                case TMC1800:
                    if (address < 0x8000)
                        address = (address | addressLatch_) & (ramMask_ | 0x8000);
                    else
                        address = address & 0x81ff;
                break;
                    
                case VIP:
                    if (address < setLatch_)
                        address = (address | addressLatch_);
                    else
                        address = address & romMask_;
                    address = address & (ramMask_ | 0x8000);
                break;
                    
                case VIPII:
                    if (address < 0x8000)
                        address = (address | addressLatch_);
                    else
                        address = address & 0x81ff;
                break;
            }
            return mainMemoryLabelType_[address];
        break;
            
        case MAPPEDRAM:
            switch (computerType_)
            {
                case ELF:
                case ELFII:
                case SUPERELF:
                    address = (address & ramMask_) + ramStart_;
                break;
                
                case COSMICOS:
                    address = address | bootstrap_;
                break;
                
                case FRED:
                    address = address & 0x7ff;
                break;
                    
                case STUDIO:
                case COINARCADE:
                case VICTORY:
                case VISICOM:
                    address = (address & 0x1ff) | 0x800;
                break;
                
                case VIP:
                    if (address < setLatch_)
                        address = (address | addressLatch_);
                    else
                        address = address & romMask_;
                    address = address & ramMask_;
                break;
            }
            return mainMemoryLabelType_[address];
        break;

        case COMXEXPBOX:
			switch (expansionMemoryType_[expansionSlot_ * 32 + (address & 0x1fff) / 256])
			{
                case RAMBANK:
                    switch (bankMemoryType_[ramBank_ * 32 + (address & 0x1fff) / 256])
                    {
                        case ROM:
                        case RAM:
                            return expansionRamLabelType_[(ramBank_ * 0x2000) + (address & 0x1fff)];
                        break;
                    }
                break;

                case EPROMBANK:
                    switch (epromBankMemoryType_[epromBank_ * 32 + (address & 0x1fff) / 256])
                    {
                        case ROM:
                        case RAM:
                            return expansionEpromLabelType_[(epromBank_ * 0x2000) + (address & 0x1fff)];
                        break;
                    }
                break;

                case SUPERBANK:
                    switch (superBankMemoryType_[(epromBank_ + (8 * ramSwitched_)) * 32 + (address & 0x1fff) / 256])
                    {
                        case ROM:
                        case RAM:
                            return expansionSuperLabelType_[((epromBank_ + (8 * ramSwitched_)) * 0x2000) + (address & 0x1fff)];
                        break;
                    }
                break;

                case ROM:
                case RAM:
                    return expansionRomLabelType_[(expansionSlot_ * 0x2000) + (address & 0x1fff)];
                break;
            }
        break;

		case PRAM1870:
        case CRAM1870:
            if (computerType_ == PECOM)
            {
                return mainMemoryLabelType_[address];
            }
            else
            {
                if (epromSlot_ != 0xff || superSlot_ != 0xff)
                    return mainMemoryLabelType_[address];
                else
                    return MEM_TYPE_UNDEFINED;
            }
        break;

		case COPYCOMXEXPROM:
			return mainMemoryLabelType_[(address & 0xfff) | 0xe000];
        break;

		case COPYFLOPROM:
			if (expansionSlot_ == fdcSlot_)
			{
				if (((address & 0xff) >= 0xd0) && ((address & 0xff) <= 0xdf))
					return expansionRomLabelType_[(expansionSlot_ * 0x2000) + (address & 0xfff)];
			}
			return mainMemoryLabelType_[address];
        break;
            
        case EMSMEMORY:
            switch (emsMemoryType_[((address & 0x3fff) |(emsPage_ << 14))/256])
            {
                case ROM:
                case RAM:
                    return emsRamLabelType_[(long) ((address & 0x3fff) |(emsPage_ << 14))];
                break;
            }
        break;
            
        case ROMMAPPER:
            if (emsPage_ <= maxNumberOfPages_)
            {
                switch (romMapperMemoryType_[((address & 0x7fff) |(emsPage_ << 15))/256])
                {
                    case ROM:
                    case RAM:
                        return expansionRomLabelType_[(long) ((address & 0x7fff) |(emsPage_ << 15))];
                    break;
                }
            }
            else
                return MEM_TYPE_UNDEFINED;
        break;
            
        case PAGER:
            switch (pagerMemoryType_[((getPager(address>>12) << 12) |(address &0xfff))/256])
            {
                case ROM:
                case RAM:
                    return mainMemoryLabelType_[(getPager(address>>12) << 12) |(address &0xfff)];
                break;
            }
        break;
            
        case MULTICART:
            if ((address < 0x400) && !disableSystemRom_)
                return mainMemoryLabelType_[address];
            else
                return multiCartRomLabelType_[(address+multiCartLsb_*0x1000+multiCartMsb_*0x10000)&multiCartMask_];
        break;

        case MAPPEDMULTICART:
            address = address & 0xfff;
            if ((address < 0x400) && !disableSystemRom_)
                return mainMemoryLabelType_[address];
            else
                return multiCartRomLabelType_[(address+multiCartLsb_*0x1000+multiCartMsb_*0x10000)&multiCartMask_];
        break;
    }
    return MEM_TYPE_UNDEFINED;
}

