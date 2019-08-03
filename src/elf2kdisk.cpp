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

#include "wx/ffile.h"

#include "main.h"
#include "elf2kdisk.h"

#define IDE_STAT_ERROR       1
#define IDE_STAT_INDEX       2
#define IDE_STAT_ECC         4
#define IDE_STAT_DRQ         8
#define IDE_STAT_SKC        16
#define IDE_STAT_WFT        32
#define IDE_STAT_RDY        64
#define IDE_STAT_BSY       128

#define IDE_CMD_RESET        1
#define IDE_CMD_SETF         2
#define IDE_CMD_WRITE        3
#define IDE_CMD_WRITE_1      4
#define IDE_CMD_READ         5
#define IDE_CMD_READ_1       6
#define IDE_CMD_ID           7

#define IDE_SELECTED         1
#define UART_SELECTED        2
#define RTC_SELECTED         3

#define UART_MCR_DTR 0
#define UART_MCR_RTS 1
#define UART_MCR_OUT1 2
#define UART_MCR_OUT2 3
#define UART_MCR_LOOP 4

#define UART_MSR_CTSD 0
#define UART_MSR_DSRD 1
#define UART_MSR_RID 2
#define UART_MSR_CDD 3
#define UART_MSR_CTS 4
#define UART_MSR_DSR 5
#define UART_MSR_RI 6
#define UART_MSR_CD 7

#define UART_LSR_DR 0
#define UART_LSR_OE 1
#define UART_LSR_PE 2
#define UART_LSR_FE 3
#define UART_LSR_BI 4
#define UART_LSR_THRE 5
#define UART_LSR_TRE 6
#define UART_LSR_FIFOE 7


Elf2KDisk::Elf2KDisk()
{
}

void Elf2KDisk::configureDisk(wxString ideFile1, wxString ideFile2, bool rtc, bool uart, ElfPortConfiguration portConf, bool use8275)
{
//	int input, status, selectOutput, writeOutput;
	wxString runningComp = p_Main->getRunningComputerStr();

    initializeIde(ideFile1);

	rtc_ = rtc;
	uart_ = uart;
    use8275_ = use8275;
    ideChecked_ = false;

//	input = p_Main->getConfigItem(runningComp +"/IdeInput", 3l);
//	status = p_Main->getConfigItem(runningComp +"/IdeStatus", 2l);
//	selectOutput = p_Main->getConfigItem(runningComp +"/IdeSelectOutput", 2l);
//	writeOutput = p_Main->getConfigItem(runningComp +"/IdeWriteOutput", 3l);
//	int tr = p_Main->getConfigItem(runningComp +"/IdeTracks", 512l);
//	int hd = p_Main->getConfigItem(runningComp +"/IdeHeads", 4l);
//	int sc = p_Main->getConfigItem(runningComp +"/IdeSectors", 26l);

	p_Computer->setInType(portConf.ideInput, ELF2KDISKREADREGISTER);
	p_Computer->setInType(portConf.ideStatus, ELF2KDISKREADSTATUS);
	p_Computer->setOutType(portConf.ideSelectOutput, ELF2KDISKSELECTREGISTER);
	p_Computer->setCycleType(DISKCYCLEIDE, ELF2KDISKCYCLE);
	p_Computer->setOutType(portConf.ideWriteOutput, ELF2KDISKWRITEREGISTER);

	wxString printBuffer;
	p_Main->message("Configuring Disk Expansion Board");

	printBuffer.Printf("	Output %d: device/register select, output %d: write selected", portConf.ideSelectOutput, portConf.ideWriteOutput);
	p_Main->message(printBuffer);

	printBuffer.Printf("	Input %d: read status, input %d: read selected", portConf.ideStatus, portConf.ideInput);
	p_Main->message(printBuffer);

	printBuffer.Printf("	Disk geometry: %d tracks, %d heads, %d sectors\n", portConf.ideTracks, portConf.ideHeads, portConf.ideSectors);
	p_Main->message(printBuffer);

	setGeometry(portConf.ideTracks, portConf.ideHeads, portConf.ideSectors);

	driveName_[0] = ideFile1;
	driveName_[1] = ideFile2;

	for (int i=0; i<2; i++)
	{
		driveCreated_[i] = wxFile::Exists(driveName_[i]);
	}

	initDisk();
}

void Elf2KDisk::initializeIde(wxString ideFile)
{
    if (!wxFile::Exists(ideFile))
    {
        wxString ideInstallationFile;
        ideInstallationFile = ideFile.Left(ideFile.Len()-4) + "-installation.ide";
        
        if (wxFile::Exists(ideInstallationFile))
            wxRenameFile(ideInstallationFile, ideFile, false);
    }
}

void Elf2KDisk::initDisk()
{
	status_ = 0x40;
	activeStatus_ = 1;
	error_ = 0;
	inter_ = 1;
	sectorCount_ = 1;
	startSector_ = 1;
	cylinder_ = 0;
	headDevice_ = 0xe0;
	dataMode_ = 16;
	command_ = 0;
	deviceSelect_ = 0;
	registerSelect_ = 0;
	for (int i = 0; i<128; i++)
		rtcRam_[i] = 0xff;
	rtcRam_[0xa] = 0;
	rtcRam_[0xb] = 0x6;
	rtcRam_[0xc] = 0;
	rtcRam_[0xd] = 0x80;

	modemControlRegister_ = 0;
	modemStatusRegister_ = 0;
	lineStatusRegister_ = 0xe0;
}

Byte Elf2KDisk::inDisk()
{
	Byte ret;
	switch (deviceSelect_)
	{
		case IDE_SELECTED:
//			p_Main->message("IDE IN");
//			p_Main->messageInt(registerSelect_);
			ret = readIdeRegister(registerSelect_);
//			p_Main->messageInt(ret);
			return ret;
		break;

		case UART_SELECTED:
			if (!uart_)
				return 0;
			switch (registerSelect_)
			{
				case 0: // RHR
					if (modemControlRegister_[UART_MCR_LOOP])
					{
						lineStatusRegister_[UART_LSR_DR] = 0;
						return thr_;
					}
					else
					{
						if (p_Vt100 != NULL)
							return p_Vt100->uartIn();
						if (p_Serial != NULL)
							return p_Serial->uartIn();
					}
				break;

				case 5: // LSR
					return lineStatusRegister_.to_ulong();
				break;

				case 6: // MSR
					return modemStatusRegister_.to_ulong();
				break;

				default:
					return 0;
				break;
			}
		break;

		case RTC_SELECTED:
//			p_Main->message("IN");
//			p_Main->messageInt(registerSelect_);
//			p_Main->messageInt(rtcRam_[registerSelect_]);
			if (!rtc_)
				return 0xff;

			if (registerSelect_ == 0xa)
			{
				if (rtcRam_[registerSelect_] & 0x80)
					rtcRam_[registerSelect_] &= 0x7f;
				else
					rtcRam_[registerSelect_] |= 0x80;
			}

			ret = rtcRam_[registerSelect_];
			if (registerSelect_ == 0xc)
				rtcRam_[registerSelect_] &= 0xbf;
			return ret;
		break;
	}
	return 0xff;
}

void Elf2KDisk::outDisk(Byte value)
{
	switch (deviceSelect_)
	{
		case IDE_SELECTED:
//			p_Main->message("IDE OUT");
//			p_Main->messageInt(registerSelect_);
//			p_Main->messageInt(value);
			writeIdeRegister(registerSelect_, value);
		break;

		case UART_SELECTED:
			if (!uart_)
				return;
			switch (registerSelect_)
			{
				case 0: // THR
					if (modemControlRegister_[UART_MCR_LOOP])
					{
						thr_ = value;
						lineStatusRegister_[UART_LSR_DR] = 1;
					}
					else 
					{
						if (p_Vt100 != NULL)
							p_Vt100->uartOut(value);
						if (p_Serial != NULL)
							p_Serial->uartOut(value);
					}
				break;

				case 4: // MCR
					modemControlRegister_ = value;
					if (modemControlRegister_[UART_MCR_LOOP])
					{
						modemStatusRegister_[UART_MSR_CTS] = modemControlRegister_[UART_MCR_RTS];
						modemStatusRegister_[UART_MSR_DSR] = modemControlRegister_[UART_MCR_DTR];
						modemStatusRegister_[UART_MSR_RI] = modemControlRegister_[UART_MCR_OUT1];
						modemStatusRegister_[UART_MSR_CD] = modemControlRegister_[UART_MCR_OUT2];
					}
					else
						modemStatusRegister_ = 0;
				break;
			}
		break;

		case RTC_SELECTED:
			writeRtc(registerSelect_, value);
			if (registerSelect_ == 0xa)
				p_Computer->setElf2KDivider(value&0xf);
//			p_Main->message("OUT");
//			p_Main->messageInt(registerSelect_);
//			p_Main->messageInt(value);
		break;
	}
}

void Elf2KDisk::selectDiskRegister(Byte value)
{
	switch (value & 0x90)
	{
		case 0:  // IDE
			registerSelect_ = value &0xf;
			deviceSelect_ = IDE_SELECTED;
		break;

		case 0x10: // UART
			registerSelect_ = value &0x7;
			deviceSelect_ = UART_SELECTED;
		break;

		case 0x80: // RTC/NVR
		case 0x90:
			registerSelect_ = value &0x7f;
			deviceSelect_ = RTC_SELECTED;
//			p_Main->message("RTC");
		break;

	}
}

void Elf2KDisk::cycleDisk()
{
	if (--ideCycles_ > 0) return;
	if (ideCycles_ < 0) ideCycles_ = 0;
	if (command_ != 0)  onCommand();
}

long Elf2KDisk::getOffset()
{
	long ret;
	Byte sec;

	int drive = (headDevice_ & 16) ? 1 : 0;
	if (headDevice_ & 64)
	{
		ret = startSector_;
		ret |= (cylinder_ << 8);
		ret |= ((headDevice_ & 0xf) << 24);
		if (ret >= geometry_[drive].maxLba)
		{
			error_ |= 4;
			status_ |= 1;
			return -1;
		}
	}
	else
	{
		ret = cylinder_;
		if (ret >= geometry_[drive].cylinders)
		{
			error_ |= 4;
			status_ |= 1;
			return -1;
		}
		sec = startSector_;
		sec--;
		if (sec >= geometry_[drive].sectors)
		{
			error_ |= 4;
			status_ |= 1;
			return -1;
		}
		ret *= sec;
		sec = headDevice_ & 0xf;
		if (sec >= geometry_[drive].heads)
		{
			error_ |= 4;
			status_ |= 1;
			return -1;
		}
		ret *= sec;
	}
	ret *= 512;
	return ret;
}

void Elf2KDisk::writeSector()
{
	wxFFile diskFile;
	int drive;
	long offset;

	offset = getOffset();
	if (offset < 0)
	{
		p_Main->message("IDE Seek error\n");
		return;
	}
	drive = (headDevice_ & 16) ? 1 : 0;
	if (!driveCreated_[drive])
	{
		diskFile.Open(driveName_[drive], "wb");
 		diskFile.Close();
		driveCreated_[drive] = true;
	}
	if (!diskFile.Open(driveName_[drive], "rb+"))
	{
		error_ |= 1;
		status_ |= 1;
        p_Main->message("IDE File open error\n");
		return;
	}
	diskFile.Seek(offset, wxFromStart);
	diskFile.Write(sectorBuffer_, 512);
	diskFile.Close();

}

void Elf2KDisk::readSector()
{
	wxFFile diskFile;
	int drive;
	long offset;

	offset = getOffset();
	if (offset < 0)
    {
        p_Main->message("IDE Seek error\n");
        return;
    }
	drive = (headDevice_ & 16) ? 1 : 0;
	if (!driveCreated_[drive])
	{
		diskFile.Open(driveName_[drive], "wb+");
 		diskFile.Close();
		driveCreated_[drive] = true;
	}
	if (!diskFile.Open(driveName_[drive], "rb+"))
	{
		error_ |= 1;
		status_ |= 1;
        p_Main->message("IDE File open error\n");
		return;
	}
	diskFile.Seek(offset, wxFromStart);
	diskFile.Read(sectorBuffer_, 512);
    
    if (offset == 0 && use8275_ && !ideChecked_)
    {
        ideChecked_ = true;
        if (sectorBuffer_[0x3a] == 0xf8 && sectorBuffer_[0x3c] == 0xb0)
        {
            if (p_Main->repairIde())
            {
                sectorBuffer_[0x3a] = 0xc0;
                sectorBuffer_[0x3b] = 0x03;
                sectorBuffer_[0x3c] = 0x0;
                writeSector();
            }
        }
    }
	diskFile.Close();
}

void Elf2KDisk::readId()
{
	for (int i=0; i<512; i++)
		sectorBuffer_[i] = 0;
	sectorBuffer_[2] = geometry_[0].cylinders % 256;
	sectorBuffer_[3] = geometry_[0].cylinders / 256;
	sectorBuffer_[6] = geometry_[0].heads % 256;
	sectorBuffer_[7] = geometry_[0].heads / 256;
	sectorBuffer_[12] = geometry_[0].sectors % 256;
	sectorBuffer_[13] = geometry_[0].sectors / 256;
	sectorBuffer_[108] = geometry_[0].cylinders % 256;
	sectorBuffer_[109] = geometry_[0].cylinders / 256;
	sectorBuffer_[110] = geometry_[0].heads % 256;
	sectorBuffer_[111] = geometry_[0].heads / 256;
	sectorBuffer_[112] = geometry_[0].sectors % 256;
	sectorBuffer_[113] = geometry_[0].sectors / 256;
	sectorBuffer_[114] = geometry_[0].maxLba & 255;
	sectorBuffer_[115] = (geometry_[0].maxLba >> 8) & 255;
	sectorBuffer_[116] = (geometry_[0].maxLba >> 16) & 255;
	sectorBuffer_[117] = (geometry_[0].maxLba >> 24) & 255;
	sectorBuffer_[120] = geometry_[0].maxLba & 255;
	sectorBuffer_[121] = (geometry_[0].maxLba >> 8) & 255;
	sectorBuffer_[122] = (geometry_[0].maxLba >> 16) & 255;
	sectorBuffer_[123] = (geometry_[0].maxLba >> 24) & 255;
}

void Elf2KDisk::setGeometry(int cyl, int hd, int sc)
{
	geometry_[0].cylinders = cyl;
	geometry_[1].cylinders = cyl;
	geometry_[0].heads = hd;
	geometry_[1].heads = hd;
	geometry_[0].sectors = sc;
	geometry_[1].sectors = sc;
	geometry_[0].maxLba = geometry_[0].cylinders * geometry_[0].heads * geometry_[0].sectors;
	geometry_[1].maxLba = geometry_[1].cylinders * geometry_[1].heads * geometry_[1].sectors;
}

void Elf2KDisk::writeRtc(int address, Byte value)
{
	rtcRam_[address] = value;
}

void Elf2KDisk::writeIdeRegister(int reg, Word value)
{
	switch(reg)
	{
		case 0x00:
			if (bufferPosition_ >= 512)
				break;							 /* Data */
			if (dataMode_ == 16)
			{
				sectorBuffer_[bufferPosition_++] = value & 0xff;
				sectorBuffer_[bufferPosition_++] = value >> 8;
			}
			else
			{
				sectorBuffer_[bufferPosition_++] = value & 0xff;
			}
			if (bufferPosition_ >= 512)
				status_ &= (~IDE_STAT_DRQ);
		break;

		case 0x01:
			features_ = value;											/* precomp */
		break;

		case 0x02:
			sectorCount_ = value;											/* sector count */
		break;

		case 0x03:
			startSector_ = value;
										/* start sector */
		case 0x04:
			cylinder_ = (cylinder_ & 0xff00) | (value & 0xff); /* cylinder lo */
		break;

		case 0x05:
			cylinder_ = (cylinder_ & 0xff) | ((value & 0xff) << 8);	 /* cylinder hi */
		break;

		case 0x06:
			headDevice_ = value;
											 /* head/device */
		case 0x07:
			if (!(status_ & 128))
			{										/* command */
				switch(value)
				{
					case 0x20:
						command_ = IDE_CMD_READ;
						ideCycles_ = 100;
						status_ = IDE_STAT_RDY | IDE_STAT_BSY;
					break;

					case 0x30:
						command_ = IDE_CMD_WRITE;
						ideCycles_ = 100;
						status_ = IDE_STAT_RDY | IDE_STAT_BSY;
					break;

					case 0xec:
						command_ = IDE_CMD_ID;
						ideCycles_ = 100;
						status_ = IDE_STAT_RDY | IDE_STAT_BSY;
					break;

					case 0xef:
						command_ = IDE_CMD_SETF;
						ideCycles_ = 100;
						status_ = IDE_STAT_RDY | IDE_STAT_BSY;
					break;
				}
			}
		break;

		case 0x0e:
			inter_ = value;													 // set inter
			if (value & 2)
			{
				command_ = IDE_CMD_RESET;
				ideCycles_ = 1000;
				status_ = IDE_STAT_RDY | IDE_STAT_BSY;
			}
		break;

		case 0x0f:
		break;			// unknown
	}
}

Word Elf2KDisk::readIdeRegister(int reg)
{
	Word ret = 0;
	switch(reg)
	{
		case 0x00:
			if (dataMode_ == 16)
			{									 /* Data */
				ret = sectorBuffer_[bufferPosition_++];
				ret |= (sectorBuffer_[bufferPosition_++] << 8);
			}
			else
			{
				ret = sectorBuffer_[bufferPosition_++];
			}
			if (bufferPosition_ >= 512)
				status_ &= (~IDE_STAT_DRQ);
		break;

		case 0x01:
			ret = error_;				/* Error */
		break;

		case 0x02:
			ret = sectorCount_;			/* sector count */
		break;

		case 0x03:
			ret = startSector_;			/* start sector */
		break;

		case 0x04:
			ret = cylinder_ & 0xff;		/* cylinder lo */
		break;

		case 0x05:
			ret = cylinder_ >> 8;		/* cylinder hi */
		break;

		case 0x06:
			ret = headDevice_;			/* head/device */
		break;

		case 0x07:
			ret = status_;				/* status */
		break;

		case 0x0e:
			ret = status_;				// status
		break;

		case 0x0f:
			ret = activeStatus_;			// active status
		break;
	}
	return ret;
}

void Elf2KDisk::onCommand()
{
	status_ &= 0xfe;
	switch(command_)
	{
			case IDE_CMD_ID:
				command_ = IDE_CMD_READ_1;
				readId();
				bufferPosition_ = 0;
				status_ |= IDE_STAT_DRQ;
			break;

			case IDE_CMD_READ:
				command_ = IDE_CMD_READ_1;
				readSector();
				bufferPosition_ = 0;
				status_ |= IDE_STAT_DRQ;
			break;

			case IDE_CMD_READ_1:
				if (bufferPosition_ >= 512)
				{
						status_ &= (~IDE_STAT_BSY);
						status_ &= (~IDE_STAT_DRQ);
						command_ = 0;
				}
			break;

			case IDE_CMD_WRITE:
				command_ = IDE_CMD_WRITE_1;
				bufferPosition_ = 0;
				status_ |= IDE_STAT_DRQ;
			break;

			case IDE_CMD_WRITE_1:
				if (bufferPosition_ >= 512)
				{
						error_ = 0;
						writeSector();
						status_ &= (~IDE_STAT_BSY);
						command_ = 0;
				}
			break;

			case IDE_CMD_RESET:
				command_ = 0;
				status_ = IDE_STAT_RDY;
				initDisk();
			break;

			case IDE_CMD_SETF:
				switch(features_)
				{
							case 0x01:dataMode_ = 8; break;
							case 0x81:dataMode_ = 16; break;
					}
				command_ = 0;
				status_ = IDE_STAT_RDY;
			break;
 	}
}

void Elf2KDisk::dataAvailableUart(bool data)
{
	lineStatusRegister_[UART_LSR_DR] = data;
}

void Elf2KDisk::thrStatusUart(bool data)
{
	lineStatusRegister_[UART_LSR_THRE] = !data;
	lineStatusRegister_[UART_LSR_TRE] = data;
}

