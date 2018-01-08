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

#if defined(__linux__)
#include "app_icon.xpm"
#endif

#include "main.h"
#include "serial.h"

extern "C" 
{
#include <libserialport.h>
}

struct sp_port *port;

int baudRateValueSerial_[] =
{
	19200, 9600, 4800, 3600, 2400, 2000, 1800, 1200, 600, 300, 200, 150, 134, 110, 75, 50
};

#define UART_DA_SERIAL 0
#define UART_FE_SERIAL 3
#define UART_THRE_SERIAL 7

Serial::Serial(int computerType, double clock, ElfConfiguration elfConf)
{
    elfConfiguration_ = elfConf;
	computerType_ = computerType;
	clock_ = clock;

	switch(computerType_)
	{
		case ELF:
			computerTypeStr_ = "Elf";
		break;

		case ELFII:
			computerTypeStr_ = "ElfII";
		break;

		case ELF2K:
			computerTypeStr_ = "Elf2K";
		break;

		case SUPERELF:
			computerTypeStr_ = "SuperElf";
		break;

		case COSMICOS:
			computerTypeStr_ = "Cosmicos";
		break;
		case MEMBER:
			computerTypeStr_ = "Membership";
		break;
		case VIP:
			computerTypeStr_ = "Vip";
		break;
        case VELF:
            computerTypeStr_ = "Velf";
        break;
		case MCDS:
			computerTypeStr_ = "MCDS";
		break;
		case MS2000:
            computerTypeStr_ = "MS2000";
        break;
    }
    uart_ = elfConfiguration_.useUart;
    SetUpFeature_ = elfConfiguration_.vtExternalSetUpFeature_;

	setCycle();

    serialOpen_ = false;
}

Serial::~Serial()
{
    if (serialOpen_)
        sp_close(port);
}

void Serial::configure(int selectedBaudR, int selectedBaudT, ElfPortConfiguration elfPortConf)
{
	wxString printBuffer;

	selectedBaudT_ = selectedBaudT;
	selectedBaudR_ = selectedBaudR;

	if (!uart_)
	{
		selectedBaudT_ += 3;
		selectedBaudR_ += 3;
	}

    baudRateT_ = (int) (((clock_ * 1000000) / 8) / baudRateValueSerial_[selectedBaudT_]);
    baudRateR_ = (int) (((clock_ * 1000000) / 8) / baudRateValueSerial_[selectedBaudR_]);

    if (uart_)
	{
		configureUart(elfPortConf);
	}
	else
	{
        reverseEf_ = elfPortConf.vt100ReverseEf;
        reverseQ_ = elfPortConf.vt100ReverseQ; 

		p_Computer->setCycleType(VTCYCLE, VTSERIALCYCLE);
		p_Computer->setOutType(elfPortConf.vt100Output, VT100OUT);

		dataReadyFlag_ = elfPortConf.vt100Ef;
		if (p_Computer->getEfType(dataReadyFlag_) == ELFINEF)
			p_Computer->setEfType(dataReadyFlag_, VTINEF);
		else
			p_Computer->setEfType(dataReadyFlag_, VTSERIALEF);

		if (reverseQ_) p_Computer->setFlipFlopQ(1);

		wxString printEfReverse = ", ";
		wxString printQ = "Serial out: Q";

		if (reverseEf_ == 0)
			printEfReverse = "(reversed), ";

		if (reverseQ_ == 1)
			printQ = "Serial out: reversed Q";

        p_Main->message("Configuring external terminal");
        startSerial();

		printBuffer.Printf("	Output %d: vtEnable, EF %d: serial input", elfPortConf.vt100Output, elfPortConf.vt100Ef);
		printBuffer = printBuffer + printEfReverse + printQ;
		p_Main->message(printBuffer);
	}

    printBuffer.Printf("	Transmit baud rate: %d, receive baud rate: %d\n", baudRateValueSerial_[selectedBaudT_], baudRateValueSerial_[selectedBaudR_]);
	p_Main->message(printBuffer);

	vtEnabled_ = 1;
	vtCount_ = -1;
	vtOutCount_ = -1;
	vtOut_ = 0;
	serialEf_ = 1;
}

void Serial::configureMember(int selectedBaudR, int selectedBaudT)
{
    wxString printBuffer;
    
    selectedBaudT_ = selectedBaudT;
    selectedBaudR_ = selectedBaudR;
    
    baudRateT_ = (int) (((clock_ * 1000000) / 8) / baudRateValueSerial_[selectedBaudT_]);
    baudRateR_ = baudRateT_;
    
    p_Computer->setCycleType(VTCYCLE, VTSERIALCYCLE);

	dataReadyFlag_ = 3;
	p_Computer->setEfType(dataReadyFlag_, VTSERIALEF);
    
    p_Main->message("Configuring external terminal");
    startSerial();
    
	configureQandEfPolarity(dataReadyFlag_, false);

    printBuffer.Printf("	Transmit baud rate: %d, receive baud rate: %d\n", baudRateValueSerial_[selectedBaudT_], baudRateValueSerial_[selectedBaudR_]);
    p_Main->message(printBuffer);
    
    vtEnabled_ = 1;
    vtCount_ = -1;
    vtOutCount_ = -1;
    vtOut_ = 0;
    serialEf_ = 1;
}

void Serial::configureMcds(int selectedBaudR, int selectedBaudT)
{
	wxString printBuffer;

	selectedBaudT_ = selectedBaudT + 7;
	selectedBaudR_ = selectedBaudR + 7;

    baudRateT_ = (int) (((clock_ * 1000000) / 8) / baudRateValueSerial_[selectedBaudT_]);
    baudRateR_ = baudRateT_;

	p_Computer->setCycleType(VTCYCLE, VTSERIALCYCLE);

	dataReadyFlag_ = 4;
	p_Computer->setEfType(dataReadyFlag_, VTSERIALEF);

    p_Main->message("Configuring external terminal");
    startSerial();

    elfConfiguration_.vtEf = false;
    elfConfiguration_.vtQ = true;
    configureQandEfPolarity(dataReadyFlag_, false);

	printBuffer.Printf("	Transmit baud rate: %d, receive baud rate: %d\n", baudRateValueSerial_[selectedBaudT_], baudRateValueSerial_[selectedBaudR_]);
	p_Main->message(printBuffer);

	vtEnabled_ = 1;
	vtCount_ = -1;
	vtOutCount_ = -1;
	vtOut_ = 0;
	serialEf_ = 1;
}

void Serial::configureCosmicos(int selectedBaudR, int selectedBaudT)
{
	wxString printBuffer;

	selectedBaudT_ = selectedBaudT;
	selectedBaudR_ = selectedBaudR;

	if (!uart_)
	{
		selectedBaudT_ += 4;
		selectedBaudR_ += 4;
	}

    baudRateT_ = (int) (((clock_ * 1000000) / 8) / baudRateValueSerial_[selectedBaudT_]);
    baudRateR_ = baudRateT_;
    
    p_Computer->setCycleType(VTCYCLE, VTSERIALCYCLE);
	dataReadyFlag_ = 4;
	p_Computer->setEfType(dataReadyFlag_, VTSERIALEF);

    p_Main->message("Configuring external terminal");
    startSerial();

	configureQandEfPolarity(dataReadyFlag_, false);

	printBuffer.Printf("	Transmit baud rate: %d, receive baud rate: %d\n", baudRateValueSerial_[selectedBaudT_], baudRateValueSerial_[selectedBaudR_]);
	p_Main->message(printBuffer);

	vtEnabled_ = 1;
	vtCount_ = -1;
	vtOutCount_ = -1;
	vtOut_ = 0;
	serialEf_ = 1;
}

void Serial::configureVip(int selectedBaudR, int selectedBaudT)
{
    wxString printBuffer;
    
    selectedBaudT_ = selectedBaudT;
    selectedBaudR_ = selectedBaudR;
    
	if (!uart_)
	{
		selectedBaudT_ += 4;
		selectedBaudR_ += 4;
	}

    baudRateT_ = (int) (((clock_ * 1000000) / 16) / baudRateValueSerial_[selectedBaudT_]);
    baudRateR_ = baudRateT_;
    
    p_Computer->setCycleType(VTCYCLE, VTSERIALCYCLE);
	dataReadyFlag_ = 4;
	p_Computer->setEfType(dataReadyFlag_, VTSERIALEF);
    
    p_Main->message("Configuring external terminal");
    startSerial();

    configureQandEfPolarity(dataReadyFlag_, false);
    
    printBuffer.Printf("	Transmit baud rate: %d, receive baud rate: %d\n", baudRateValueSerial_[selectedBaudT_], baudRateValueSerial_[selectedBaudR_]);
    p_Main->message(printBuffer);
    
    vtEnabled_ = 1;
    vtCount_ = -1;
    vtOutCount_ = -1;
    vtOut_ = 0;
    serialEf_ = 1;
}

void Serial::configureVelf(int selectedBaudR, int selectedBaudT)
{
    wxString printBuffer;
    
    selectedBaudT_ = selectedBaudT;
    selectedBaudR_ = selectedBaudR;
    
	selectedBaudT_ ++;
	selectedBaudR_ ++;
    
    baudRateT_ = (int) (((clock_ * 1000000) / 16) / baudRateValueSerial_[selectedBaudT_]);
    baudRateR_ = baudRateT_;
    
    p_Computer->setCycleType(VTCYCLE, VTSERIALCYCLE);
	dataReadyFlag_ = 2;
	p_Computer->setEfType(dataReadyFlag_, VTSERIALEF);
    
    p_Main->message("Configuring external terminal");
    startSerial();

	configureQandEfPolarity(dataReadyFlag_, false);
    
    printBuffer.Printf("	Transmit baud rate: %d, receive baud rate: %d\n", baudRateValueSerial_[selectedBaudT_], baudRateValueSerial_[selectedBaudR_]);
    p_Main->message(printBuffer);
    
    vtEnabled_ = 1;
    vtCount_ = -10;
    vtOutCount_ = -1;
    vtOut_ = 0;
    serialEf_ = 1;
}

void Serial::configureUart(ElfPortConfiguration elfPortConf)
{
	p_Computer->setOutType(elfPortConf.uartOut, UARTOUT);
	p_Computer->setInType(elfPortConf.uartIn, UARTIN);
	p_Computer->setOutType(elfPortConf.uartControl, UARTCONTROL);
	p_Computer->setInType(elfPortConf.uartStatus, UARTSTATUS);
	p_Computer->setCycleType(VTCYCLE, VTSERIALCYCLE);

	wxString printBuffer;

    p_Main->message("Configuring external terminal");
    startSerial();

	printBuffer.Printf("	Output %d: load transmitter, input %d: read receiver", elfPortConf.uartOut, elfPortConf.uartIn);
	p_Main->message(printBuffer);

	printBuffer.Printf("	Output %d: load control, input %d: read status", elfPortConf.uartControl, elfPortConf.uartStatus);
	p_Main->message(printBuffer);
	rs232_ = 0;
}

void Serial::configureMs2000(int selectedBaudR, int selectedBaudT)
{
    selectedBaudT_ = selectedBaudT;
    selectedBaudR_ = selectedBaudR;
    
    baudRateT_ = (int) (((clock_ * 1000000) / 8) / baudRateValueSerial_[selectedBaudT_]);
    baudRateR_ = (int) (((clock_ * 1000000) / 8) / baudRateValueSerial_[selectedBaudR_]);
    
    p_Computer->setEfType(4, VTSERIALEF);
    p_Computer->setCycleType(VTCYCLE, VTSERIALCYCLE);
    
    p_Main->message("Configuring external terminal");
    startSerial();

    p_Main->message("	Output 2: load transmitter, input 2: read receiver");
    p_Main->message("	Output 3: load control, input 3: read status");
    p_Main->message("	EF 4: serial input");
    
    wxString printBuffer;
    printBuffer.Printf("	Transmit baud rate: %d, receive baud rate: %d\n", baudRateValueSerial_[selectedBaudT_], baudRateValueSerial_[selectedBaudR_]);
    p_Main->message(printBuffer);
    
    rs232_ = 0;
    vtEnabled_ = 1;
    vtCount_ = -1;
    vtOutCount_ = -1;
    vtOut_ = 0;
    serialEf_ = 1;
    reverseEf_ = true;
    dataReadyFlag_ = 0;
}

void Serial::configureVt2K(int selectedBaudR, int selectedBaudT, ElfPortConfiguration elfPortConf)
{
	wxString printBuffer;

	selectedBaudT_ = selectedBaudT;
	selectedBaudR_ = selectedBaudR;

	if (!uart_)
	{
		selectedBaudT_ += 3;
		selectedBaudR_ += 3;
	}

    baudRateT_ = (int) (((clock_ * 1000000) / 8) / baudRateValueSerial_[selectedBaudT_]);
    baudRateR_ = (int) (((clock_ * 1000000) / 8) / baudRateValueSerial_[selectedBaudR_]);

	if (uart_)
	{
        p_Computer->setCycleType(VTCYCLE, VTSERIALCYCLE);

        p_Main->message("Configuring external terminal");
        startSerial();

		rs232_ = 0;
	}
	else
	{
		reverseEf_ = elfPortConf.vt100ReverseEf;
		reverseQ_ = elfPortConf.vt100ReverseQ;

		p_Computer->setCycleType(VTCYCLE, VTSERIALCYCLE);
		dataReadyFlag_ = elfPortConf.vt100Ef;
		p_Computer->setEfType(elfPortConf.vt100Ef, VTSERIALEF);
		if (reverseQ_) p_Computer->setFlipFlopQ(1);

		wxString printEfReverse = ", ";
		wxString printQ = "Serial out: Q";

		if (reverseEf_ == 0)
			printEfReverse = "(reversed), ";

		if (reverseQ_ == 1)
			printQ = "Serial out: reversed Q";

        p_Main->message("Configuring external terminal");
        startSerial();

		printBuffer.Printf("	EF %d: serial input", elfPortConf.vt100Ef);
		printBuffer = printBuffer + printEfReverse + printQ;
		p_Main->message(printBuffer);
	}

    printBuffer.Printf("	Transmit baud rate: %d, receive baud rate: %d\n", baudRateValueSerial_[selectedBaudT_], baudRateValueSerial_[selectedBaudR_]);
    p_Main->message(printBuffer);

	vtEnabled_ = 1;
	vtCount_ = -1;
	vtOutCount_ = -1;
	vtOut_ = 0;
	serialEf_ = 1;
}

void Serial::startSerial()
{
    sp_return error = sp_get_port_by_name(elfConfiguration_.serialPort_, &port);
    if (error == SP_OK)
    {
        error = sp_open(port, SP_MODE_READ_WRITE);
        if (error == SP_OK)
        {
            sp_set_baudrate(port, baudRateValueSerial_[selectedBaudT_]); 
            if (SetUpFeature_[VTBITS])
				sp_set_bits	(port, 8);
			else
				sp_set_bits	(port, 7);
			sp_set_stopbits(port, 1);
			sp_set_xon_xoff(port, SP_XONXOFF_DISABLED);
			sp_set_flowcontrol(port, SP_FLOWCONTROL_NONE);
			if (SetUpFeature_[VTPARITY])
			{
				if (SetUpFeature_[VTPARITYSENSE])
					sp_set_parity(port, SP_PARITY_EVEN);
				else
					sp_set_parity(port, SP_PARITY_ODD);
			}	
			else
					sp_set_parity(port, SP_PARITY_NONE);

            serialOpen_ = true;
        }
        else
            p_Main->message("Error opening serial device");
    }
    else
        p_Main->message("Error finding serial device");
}

void Serial::configureQandEfPolarity(int ef, bool vtEnable)
{
	wxString efPolarity, qPolarity;

	if (elfConfiguration_.vtEf)
	{
		efPolarity = " (reversed)";
		reverseEf_ = 0;
	}
	else
	{
		efPolarity = "";
		reverseEf_ = 1;
	}

	if (elfConfiguration_.vtQ)
	{
		qPolarity = "";
		reverseQ_ = 0;
	}
	else
	{
		qPolarity = " (reversed)";
		reverseQ_ = 1;
	}

	wxString messageText="";

	if (vtEnable)
		messageText.Printf("	Output 7: vtEnable, EF %d: serial input", ef);
	else
		messageText.Printf("	EF %d: serial input", ef);
	p_Main->message(messageText + efPolarity + ", Serial out: Q" + qPolarity);
}

Byte Serial::ef()
{
	return(reverseEf_^serialEf_);
}

void Serial::out(Byte value)
{
	if (value == 0x40) vtEnabled_ = 0;
	if (value == 0x80) vtEnabled_ = 1;
}

void Serial::cycleVt()
{
	cycleValue_--;
	if (cycleValue_ <= 0)
	{
		size_t numberOfBytes;
		Byte input;
		numberOfBytes = sp_nonblocking_read(port, &input, 1);

		if (numberOfBytes >= 1)
			vtOut_ = input;
		cycleValue_ = cycleSize_;
	}

/*	if (uart_)
	{
		vtCount_--;
		if (vtCount_ <= 0)
		{
//			if (rs232_ != 0)
//				Display(rs232_ & 0x7f, false);
            
            rs232_ = 0;
            p_Computer->thrStatus(0);
            uartStatus_[UART_THRE_SERIAL] = 1;
			
			vtCount_ = baudRateR_ * 9;
		}
		if (vtOutCount_ > 0)
		{
			vtOutCount_--;
			if (vtOutCount_ == 0)
			{
				p_Computer->dataAvailable(1);
				uartStatus_[UART_DA_SERIAL] = 1;
				vtOutCount_ = -1;
			}
            if (vtOutCount_ <= 0)
            {
                serialEf_ = (vtOut_ & 1) ? 1 : 0;
                vtOut_ = (vtOut_ >> 1) | 128;
                vtOutCount_ = baudRateT_;
                if (SetUpFeature_[VTPARITY])
                {
                    if (vtOutBits_ == 3)
                        serialEf_ = parity_;
                    if (vtOutBits_ == 2)
                        serialEf_ = 1;
                }
                else
                {
                    if (vtOutBits_ == 2)
                        serialEf_ = 1;
                }
                if (--vtOutBits_ == 0)
                {
                    vtOut_ = 0;
                    p_Computer->setNotReadyToReceiveData(dataReadyFlag_-1);
                    vtOutCount_ = -1;
                }
            }
        }
    }
    else  // if !uart
    {*/
        if (vtOutCount_ > 0)
		{
			vtOutCount_--;
			if (vtOutCount_ <= 0)
			{ // input from terminal
				serialEf_ = (vtOut_ & 1) ? 1 : 0;
				vtOut_ = (vtOut_ >> 1) | 128;
				vtOutCount_ = baudRateT_;
				if (SetUpFeature_[VTPARITY])
				{
					if (vtOutBits_ == 3)
						serialEf_ = parity_;
					if (vtOutBits_ == 2)
						serialEf_ = 1;
				}
				else
				{
					if (vtOutBits_ == 2) 
						serialEf_ = 1;
				}
				if (--vtOutBits_ == 0)
				{
					vtOut_ = 0;
                    p_Computer->setNotReadyToReceiveData(dataReadyFlag_-1);
					vtOutCount_ = -1;
				}
//				else
//					p_Main->messageInt(serialEf_);
//                p_Computer->setGreenLed(serialEf_ ^ 1);
			}
		}
		else
        {
/*            if (terminalLoad_)
            {
                if (vtOut_ == 0)
                {
                    if (p_Computer->isReadyToReceiveData(dataReadyFlag_-1))
                    {
                        bool eof=false;
                        size_t numberOfBytes;
                        int seek = -2;
                            
                        numberOfBytes = inputTerminalFile.Read(saveBuffer, 3);

                        if (inputTerminalFile.Eof())
                        {
                            switch(numberOfBytes)
                            {
                                case 0:
                                    eof = true;
                                break;
                                        
                                case 1:
                                    if (saveBuffer[0] == 0xa)
                                    {
                                        saveBuffer[0] = 0xd;
                                        eof = true;
                                    }
                                    seek = 0;
                                break;
                                        
                                case 2:
                                    if (saveBuffer[0] == 0xd && saveBuffer[1] == 0xa)
                                        eof = true;
                                    seek = -1;
                                break;
                            }
                        }
                            
                        if (saveBuffer[0] == 0xa && previousByte_ != 0xd)
                            saveBuffer[0] = 0xd;
                            
                        if (eof)
                        {
                            terminalLoad_ = false;
                            inputTerminalFile.Close();
                            p_Main->stopTerminal();
                        }
                        else
                        {
                            inputTerminalFile.Seek(seek, wxFromCurrent);
                            vtOut_ = saveBuffer[0];
                            previousByte_ = vtOut_;
                        }
                    }
                }
            }*/
            
            if (vtOut_ != 0 && vtEnabled_)
            {
                serialEf_ = 0;
                parity_ = Parity(vtOut_);
                vtOutCount_ = baudRateT_;
                if (SetUpFeature_[VTBITS])
                    vtOutBits_ = 10;
                else
                    vtOutBits_ = 9;
                if (SetUpFeature_[VTPARITY])
                    vtOutBits_++;
                p_Computer->setGreenLed(serialEf_ ^ 1);
			}
        }

		if (vtCount_ >= 0)
		{ // output to terminal

			vtCount_--; 
			if (vtCount_ <= 0)
			{
				if (SetUpFeature_[VTPARITY])
				{
					if (vtBits_ > 2)
					{
						rs232_ >>= 1;
						rs232_ |= (p_Computer->getFlipFlopQ() ^ reverseQ_) ? 0 : 128;
					}
					if (vtBits_ == 2)
					{
						if (!SetUpFeature_[VTBITS])
							rs232_ >>= 1;
						if (Parity(rs232_) != p_Computer->getFlipFlopQ())
							rs232_ = 2;
					}
				}
				else
				{
					if (vtBits_ > 1)
					{
						rs232_ >>= 1;
						rs232_ |= (p_Computer->getFlipFlopQ() ^ reverseQ_) ? 0 : 128;
					}
					if (vtBits_ == 1)
					{
						if (!SetUpFeature_[VTBITS])
							rs232_ >>= 1;
					}
				}
				vtCount_ = baudRateR_;
				if (--vtBits_ == 0)
				{
					vtCount_ = -1;
					rs232_ = rs232_ & 0x7f;
					if (serialOpen_)
						sp_nonblocking_write(port, &rs232_, 1);
				}
			}
		}
//	}
}

void Serial::switchQ(int value)
{
    if (vtCount_ < 0)
    {
        if (value ^ reverseQ_)
        {
            vtCount_ = baudRateR_ + baudRateR_ / 2;
            if (SetUpFeature_[VTBITS])
                vtBits_ = 9;
            else
                vtBits_ = 8;
            if (SetUpFeature_[VTPARITY])
                vtBits_++;
            rs232_ = 0;
        }
    }
}

void Serial::setClock(double clock)
{
	clock_ = clock;
}

void Serial::setCycle()
{   
	cycleSize_ = (int) (((clock_ * 1000000) / 8) / 10);
	cycleValue_ = cycleSize_;
}

int Serial::Parity(int value)
{
	int i;
	int par;
	par = 0;
	int numberOfBits;
	if (SetUpFeature_[VTBITS])
		numberOfBits = 8;
	else
		numberOfBits = 7;
	for (i=0; i<numberOfBits; i++)
	{
		if (value & 1) par++;
		value >>= 1;
	}
	if (SetUpFeature_[VTPARITYSENSE])
		return(par & 1);
	else
		return((par & 1) ^ 1);
}

char* Serial::getInteger(char* buffer,int* dest)
{
	*dest = 0;
	while(*buffer >= '0' && *buffer <= '9')
	{
		*dest = *dest * 10 +(*buffer - '0');
		buffer++;
	}
	return buffer;
}

void Serial::dataAvailable()
{
	if (uart_)
		vtOutCount_ = baudRateT_ * 9;
}

void Serial::framingError(bool data)
{
	uartStatus_[UART_FE_SERIAL] = data;
}

void Serial::ResetVt()
{
	vtEnabled_ = 1;
}

void Serial::ResetIo()
{
/*	escPosition_ = -1;
	vtEnabled_ = 1;
	vtCount_ = -1;
	vtOutCount_ = -1;
	vtOut_ = 0;
	serialEf_ = 1;
	elfRunCommand_ = 0;*/
}
