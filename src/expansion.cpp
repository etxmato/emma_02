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
#include "expansion.h"
#include "printer.h"

Expansion::Expansion(const wxString& title, const wxPoint& pos, const wxSize& size, double zoomLevel, int computerType, double clock)
:V1870(title, pos, size, zoomLevel, computerType, clock)
{
}

void Expansion::configureExpansion()
{
	for (int i=0; i<4; i++)
		comxExpansionType_[i] = COMXEMPTY;

	inType_[2] = COMXEXPIN2;
	inType_[4] = COMXEXPIN4;
	outType_[2] = COMXEXPOUT;

	out1value_ = 0;

	printerSlot_ = 0xff;
	serialSlot_ = 0xff;
	thermalSlot_ = 0xff;
	columnSlot_ = 0xff;
	diagSlot_ = 0xff;
	epromSlot_ = 0xff;
	superSlot_ = 0xff;
	networkSlot_ = 0xff;
	fdcSlot_ = 0xff;
	ramBank_ = 0;
	diskRomLoaded_ = false;
	printRomLoaded_[COMXPARALLEL] = false;
	printRomLoaded_[COMXSERIAL] = false;
	printRomLoaded_[COMXTHERMAL] = false;

	defineMemoryType(0xc000, 0xdfff, COMXEXPBOX);
	if (expansionRomLoaded_)
	{
		p_Main->message("Configuring Expansion Box");
		p_Main->message("	Output 1: Select expansion slot(bit 1 to 4)\n");
	}

	expansionSlot_ = 0;
	configureCard(0);
	if (expansionRomLoaded_)
	{
		configureCard(1);
		configureCard(2);
		configureCard(3);
		expansionSlot_ = 3;
	}
	setOutValue(1, 0x10);
	p_Main->setComxExpLedOn (true);
}

void Expansion::configureCard(int slot)
{
	wxString print_buffer;
	Byte value;
	wxString slotString = "";

	useExpansionRam_ = p_Main->getUseExpansionRam();
	expansionRamSlot_ = p_Main->getExpansionRamSlot();

	if (expansionRomLoaded_)
	{
		slotString.Printf(" in Slot %d",slot+1);
	}
	if (((expansionRamSlot_-1) == slot) && useExpansionRam_)
	{
		expansionSlot_ = slot;
		comxExpansionType_ [slot] = COMXRAM;
		print_buffer = "Configuring 32K RAM Card" + slotString;
		p_Main->message(print_buffer);
		p_Main->message("	Output 1: Select RAM bank(bit 5 and 6)\n");
		defineExpansionMemoryType(slot, 0, 0x1fff, RAMBANK);
	}
	else
	{
		expansionSlot_ = slot;
//		defineExpansionMemoryType(slot, 0, 0x1fff, UNDEFINED);
		defineExpansionMemoryType(slot, 0, 0x1fff, ROM);

		wxString slotStr;
		slotStr.Printf("%d",slot+1);
		readProgram(p_Main->getRomDir(COMX, slot+2), p_Main->getRomFile(COMX, slot + CARTROM1), COMXEXPBOX, 0xC000, NONAME);

		comxExpansionType_ [slot] = expansionRom_[(slot*0x2000)+1];

		if (expansionRom_[slot*0x2000] == 0x30)
			comxExpansionType_ [slot] = COMXSUPERBOARD;

		if (comxExpansionType_[slot] == COMXPRINTER)
			if (expansionRom_[(slot*0x2000)+0x14] == 0x52)
				comxExpansionType_[slot] = COMXRS232;

		switch(comxExpansionType_[slot])
		{
			case COMXFLOP:
				if (fdcSlot_ == 0xff)
				{
					fdcSlot_ = slot;
					diskRomLoaded_ = true;
					configure1770(2, 35, 16, 128, COMX);
					resetFdc();

					print_buffer = "Configuring WD1770 Floppy Disk Controller" + slotString;
					p_Main->message(print_buffer);
					p_Main->message("	Q = 1, Output 2: register select,	Q = 0, Output 2: write register");
					p_Main->message("	Q = 1, Input 2: INTRQ,		Q = 0, Input 2: read register");
					p_Main->message("	EF 4: DRQ");

					p_Main->message("	Disk geometry: 2 sides, 35/70 tracks, 16 sectors per track and 128 bytes per sector\n");
					defineMemoryType(0x0d00, 0x0dff, COPYFLOPROM);
				}
			break;

			case COMXPRINTER:
				if (printerSlot_ == 0xff)
				{
					printerSlot_ = slot;
					printRomLoaded_[COMXPARALLEL] = true;
					p_PrinterParallel = new Printer();
					p_PrinterParallel->initComx(p_PrinterParallel);
					print_buffer = "Configuring Parallel Printer Card" + slotString;
					p_Main->message(print_buffer);
					p_Main->message("	Output 2: printer output, input 2: printer status\n");
				}
			break;

			case COMXTHPRINTER:
				if (thermalSlot_ == 0xff)
				{
					thermalSlot_ = slot;
					printRomLoaded_[COMXTHERMAL] = true;
					p_PrinterThermal = new Printer();
					p_PrinterThermal->initComx(p_PrinterThermal);
					p_PrinterThermal->configureThermalPrinter();
					print_buffer = "Configuring Thermal Printer Card" + slotString;
					p_Main->message(print_buffer);
					p_Main->message("	Q = mode, Output 2: control head, input 2: printer status\n");
				}
			break;

			case COMXJOY:
				print_buffer = "Configuring F&M Joy Card" + slotString;
				p_Main->message(print_buffer);
				p_Main->message("	Input 2: joystick 1, input 4: joystick 2 and fire buttons\n");
				defineExpansionMemoryType(slot, 0, 0x1fff, UNDEFINED);
			break;

			case COMXRS232:
				if (serialSlot_ == 0xff)
				{
					serialSlot_ = slot;
					printRomLoaded_[COMXSERIAL] = true;
					p_PrinterSerial = new Printer();
					p_PrinterSerial->initComx(p_PrinterSerial);
					print_buffer = "Configuring Serial Printer Card" + slotString;
					p_Main->message(print_buffer);
					p_Main->message("	Output 2: printer output, input 2: printer status\n");
				}
			break;

			case COMX80COLUMN:
				if (columnSlot_ == 0xff)
				{
					columnSlot_ = slot;
					print_buffer = "Configuring 80 Column Card" + slotString;
					p_Main->message(print_buffer);
					p_Main->message("	EF 4: Display enable");
					p_Main->message("	@D000-@D7FF: Video RAM");
					p_Main->message("	@D800: CRTC address register, @D801: CRTC data register\n");
					defineExpansionMemoryType(slot, 0, 0x7ff, ROM);
					defineExpansionMemoryType(slot, 0x800, 0xfff, UNDEFINED);
					defineExpansionMemoryType(slot, 0x1000, 0x17ff, MC6845RAM);
					defineExpansionMemoryType(slot, 0x1800, 0x18ff, MC6845REGISTERS);
					defineExpansionMemoryType(slot, 0x1900, 0x1fff, UNDEFINED);

					for (int i=0x0; i<0x800; i++)
					{
						mc6845CharRom_[i] = expansionRom_[(columnSlot_*0x2000) + (0x800+i)];
						expansionRom_[(columnSlot_*0x2000) + (0x800+i)] = 0xff;
					}
				}
			break;

			case NETWORK:
				if (networkSlot_ == 0xff)
				{
					networkSlot_ = slot;

					print_buffer = "Configuring Network Card" + slotString;
					p_Main->message(print_buffer);
//					p_Main->message("	Output 1: Select EPROM bank (bit 5, 6 and 7)\n");

					readProgram(p_Main->getRomDir(COMX, slot+2), "network.sv32.part.bin", ROM, 0xF000, NONAME);
				}
			break;

			case COMXEPROMBOARD:
				if (epromSlot_ == 0xff)
				{
					epromSlot_ = slot;
					print_buffer = "Configuring F&M EPROM Switch Board" + slotString;
					p_Main->message(print_buffer);
					p_Main->message("	Output 1, bit 765: Select EPROM bank (0-7)\n");
					defineExpansionMemoryType(slot, 0, 0x1fff, EPROMBANK);
					for (int i=0; i<0x800; i++)
					{
						value = expansionRom_[(slot*0x2000)+i];
						mainMemory_[i+0xf800] = value;
						value = expansionRomDataType_[(slot*0x2000)+i];
						mainMemoryDataType_[i+0xf800] = value;
					}

					epromBank_ = 4;
					readEpromFile(p_Main->getEpromRomDirectory(4), p_Main->getEpromRom(4), 0xC000);
					epromBank_ = 3;
					readEpromFile(p_Main->getEpromRomDirectory(3), p_Main->getEpromRom(3), 0xC000);
					epromBank_ = 2;
					readEpromFile(p_Main->getEpromRomDirectory(2), p_Main->getEpromRom(2), 0xC000);
					epromBank_ = 1;
					readEpromFile(p_Main->getEpromRomDirectory(1), p_Main->getEpromRom(1), 0xC000);
					epromBank_ = 0;
					readEpromFile(p_Main->getEpromRomDirectory(0), p_Main->getEpromRom(0), 0xC000);
				}
			break;

			case COMXSUPERBOARD:
				if (superSlot_ == 0xff)
				{
					superSlot_ = slot;
					inType_[1] = COMXRTCIN1;
					inType_[5] = COMXEXPIN5;
					inType_[6] = COMXEXPIN6;

					print_buffer = "Configuring Super Board" + slotString;
					p_Main->message(print_buffer);
					p_Main->message("	Output 1, bit 0: Select EPROM = 0, RAM = 1");
					p_Main->message("	Output 1, bit 765: Select EPROM bank (0-7)");
					p_Main->message("	Output 1, bit 765: Select RAM bank (0-7)");
					p_Main->message("	Output 2: UART out, Input 5: UART in, Q UART register select");
					p_Main->message("	Input 1: NVRAM write enable/disable");
					p_Main->message("	Input 2: joystick 1, input 4: joystick 2 and fire buttons");
					p_Main->message("	NVRAM F000-F3F7, RTC F3F8-F3FF\n");
					defineExpansionMemoryType(slot, 0, 0x1fff, SUPERBANK);
					defineMemoryType(0xf000, 0xf3ff, NVRAM);
					for (int i=0; i<0x800; i++)
					{
						value = expansionRom_[(slot*0x2000)+i];
						mainMemory_[i+0xf800] = value;
						value = expansionRomDataType_[(slot*0x2000)+i];
						mainMemoryDataType_[i+0xf800] = value;
					}

					ramSwitched_ = 0;
					epromBank_ = 7;
					readEpromFile(p_Main->getSbRomDirectory(10), p_Main->getSbRom(10), 0xC000);
					epromBank_ = 6;
					readEpromFile(p_Main->getSbRomDirectory(9), p_Main->getSbRom(9), 0xC000);
					epromBank_ = 5;
					readEpromFile(p_Main->getSbRomDirectory(8), p_Main->getSbRom(8), 0xC000);
					epromBank_ = 4;
					readEpromFile(p_Main->getSbRomDirectory(7), p_Main->getSbRom(7), 0xC000);
					epromBank_ = 3;
					readEpromFile(p_Main->getSbRomDirectory(6), p_Main->getSbRom(6), 0xC000);
					epromBank_ = 2;
					readEpromFile(p_Main->getSbRomDirectory(5), p_Main->getSbRom(5), 0xC000);
					epromBank_ = 1;
					readEpromFile(p_Main->getSbRomDirectory(4), p_Main->getSbRom(4), 0xC000);
					epromBank_ = 0;
					readEpromFile(p_Main->getSbRomDirectory(3), p_Main->getSbRom(3), 0xC000);
				}
			break;

			case COMXDIAG:
				if (diagSlot_ == 0xff)
				{
					diagSlot_ = slot;
					print_buffer = "Configuring Diagnose Card" + slotString;
					p_Main->message(print_buffer);
					p_Main->message("	...");
					p_Main->message("	@D800-@DFFF: RAM");
					defineExpansionMemoryType(slot, 0, 0x17ff, ROM);
					defineExpansionMemoryType(slot, 0x1800, 0x1fff, RAM);
				}
			break;

			default:
				defineExpansionMemoryType(slot, 0, 0x1fff, UNDEFINED);
			break;
		}
	}
}

void Expansion::readEpromFile(wxString dir, wxString fileName, int start)
{
	if (fileName.Len() != 0)
		readFile(dir + fileName, COMXEXPBOX, start, 0x10000, NONAME);
}

Byte Expansion::expansionIn2()
{
	switch(comxExpansionType_[expansionSlot_])
	{
		case COMXFLOP:
			return in1770();
		break;

		case COMXJOY:
		case COMXSUPERBOARD:
			return stick1();
		break;

		case COMXPRINTER:
			return p_PrinterParallel->inParallel();
		break;

		case COMXTHPRINTER:
			return p_PrinterThermal->inThermal();
		break;

		case COMXRS232:
			return p_PrinterSerial->inSerial();
		break;
	}
	return 0;
}

Byte Expansion::expansionIn4()
{
	switch(comxExpansionType_[expansionSlot_])
	{
		case COMXJOY:
		case COMXSUPERBOARD:
			return stick2();
		break;
	}
	return 0;
}

void Expansion::bankOut(Byte value)
{
	if (expansionRomLoaded_)
	{
		if ((out1value_ & 0x1e) != (value & 0x1e))
			p_Main->expLedOffEvent();
		if ((value & 0x1e) == 0x2)	expansionSlot_ = 0;
		if ((value & 0x1e) == 0x4)	expansionSlot_ = 1;
		if ((value & 0x1e) == 0x8)	expansionSlot_ = 2;
		if ((value & 0x1e) == 0x10)  expansionSlot_ = 3;
		if ((out1value_ & 0x1e) != (value & 0x1e))
			p_Main->expLedOnEvent();
	}
	ramBank_ = (value & 0x60) >> 5;
	epromBank_ = (value & 0xe0) >> 5;
	ramSwitched_ = value & 0x1;

	p_Main->updateSlotInfo();
	p_Main->setComxStatusLedOn (false);
	out1value_ = value;
}

void Expansion::expansionOut(Byte value)
{
	switch(comxExpansionType_[expansionSlot_])
	{
		case COMXFLOP:
			out1770(value);
		break;

		case COMXSUPERBOARD:
			usbOut(value);
		//	usbOutVspe(value);
		break;

		case COMXPRINTER:
			p_Main->eventPrintParallel(value);
		break;

		case COMXTHPRINTER:
			thermalPrinting_ = true;
			p_Main->eventPrintThermal(value, getFlipFlopQ());
		break;

		case COMXRS232:
			p_Main->eventPrintSerial(value);
		break;
	}
}

void Expansion::onComxF4()
{
	switch(comxExpansionType_[expansionSlot_])
	{
		case COMXPRINTER:
			p_PrinterParallel->onF4Parallel();
			return;
		break;

		case COMXTHPRINTER:
			p_PrinterThermal->onF4Thermal();
			return;
		break;

		case COMXRS232:
			p_PrinterSerial->onF4Serial();
			return;
		break;
	}
	if (printRomLoaded_[COMXPARALLEL])
	{
		p_PrinterParallel->onF4Parallel();
		return;
	}
	if (printRomLoaded_[COMXSERIAL])
	{
		p_PrinterSerial->onF4Serial();
		return;
	}
	if (printRomLoaded_[COMXTHERMAL])
	{
		p_PrinterThermal->onF4Thermal();
		return;
	}
}

int Expansion::getComxExpansionType(int card)
{
	return comxExpansionType_ [card];
}

bool Expansion::isPrintRomLoaded(int type)
{
	return printRomLoaded_[type];
}

bool Expansion::isRamCardActive()
{
	if (expansionRomLoaded_)
		return useExpansionRam_;
	else
		return (comxExpansionType_ [0] == COMXRAM);
}

bool Expansion::isDosCardSelected()
{
	return (comxExpansionType_[expansionSlot_] == COMXFLOP);
}

