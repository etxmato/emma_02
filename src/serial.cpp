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

extern "C" 
{
#include "libserialport.h"
}

struct sp_port *port;

int baudRateValueSerial_[] =
{
    38400, 19200, 9600, 4800, 3600, 2400, 2000, 1800, 1200, 600, 300, 200, 150, 134, 110, 75, 50
};

Serial::Serial(int computerType, double clock, ElfConfiguration elfConf)
{
    elfConfiguration_ = elfConf;
    computerType_ = computerType;
    clock_ = clock;

    uart_ = elfConfiguration_.useUart;
    uart16450_ = elfConfiguration_.useUart16450;
    SetUpFeature_ = elfConfiguration_.vtExternalSetUpFeature_;

    setCycle();
    uartEf_ = false;
    uartControl_ = 0;

    serialOpen_ = false;
    
    uart_da_bit_ = 0;
    uart_fe_bit_ = 3;
    uart_tsre_bit_ = 6;
    uart_thre_bit_ = 7;
    numberOfBitsPerByte_ = 9;
}

Serial::~Serial()
{
    if (serialOpen_)
        sp_close(port);
}

void Serial::configure(int selectedBaudR, int selectedBaudT, IoConfiguration ioConfiguration)
{
    wxString printBuffer;
    
    selectedBaudT_ = selectedBaudT;
    selectedBaudR_ = selectedBaudR;
    
    baudRateT_ = (int) ((((clock_ * 1000000) / 8) / baudRateValueSerial_[selectedBaudT_])+0.5);
    baudRateR_ = (int) ((((clock_ * 1000000) / 8) / baudRateValueSerial_[selectedBaudR_])+0.5);

    if (uart_)
    {
        configureUart(ioConfiguration);
    }
    else
    {
        if (uart16450_)
        {
            p_Computer->setCycleType(VTCYCLE, VTSERIALCYCLE);
            
            p_Main->message("Configuring external terminal with 16450/550 UART");
            startSerial();
            rs232_ = 0;
        }
        else
        {
            reverseEf_ = ioConfiguration.vt100ReverseEf;
            reverseQ_ = ioConfiguration.vt100ReverseQ;
            
            p_Computer->setCycleType(VTCYCLE, VTSERIALCYCLE);
            p_Computer->setOutType(ioConfiguration.vt100Output, VTOUTSERIAL);
            
            dataReadyFlag_ = ioConfiguration.vt100Ef;
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
            
            printBuffer.Printf("	Output %d: vtEnable, EF %d: serial input", ioConfiguration.vt100Output, ioConfiguration.vt100Ef);
            printBuffer = printBuffer + printEfReverse + printQ;
            p_Main->message(printBuffer);
        }
    }
    
    printBuffer.Printf("	Transmit baud rate: %d, receive baud rate: %d\n", baudRateValueSerial_[selectedBaudT_], baudRateValueSerial_[selectedBaudR_]);
    p_Main->message(printBuffer);
    
    vtEnabled_ = 1;
    vtCount_ = -1;
    vtOutCount_ = -1;
    vtOut_ = 0;
    vtOutSet_ = false;
    serialEf_ = 1;
}

void Serial::configureStandard(int selectedBaudR, int selectedBaudT, int dataReadyFlag)
{
    wxString printBuffer;
    
    selectedBaudT_ = selectedBaudT;
    selectedBaudR_ = selectedBaudR;
    dataReadyFlag_ = dataReadyFlag; // Velf = 2, Member = 3, Mcds, Cosmicos, VIP = 4
    
    if (computerType_ == VELF || computerType_ == VIP || computerType_ == VIP2K)
        baudRateT_ = (int) ((((clock_ * 1000000) / 16) / baudRateValueSerial_[selectedBaudT_])+0.5);
    else
        baudRateT_ = (int) ((((clock_ * 1000000) / 8) / baudRateValueSerial_[selectedBaudT_])+0.5);
    baudRateR_ = baudRateT_;
    
    p_Computer->setCycleType(VTCYCLE, VTSERIALCYCLE);
    p_Computer->setEfType(dataReadyFlag_, VTSERIALEF);
    
    startSerial();
    
    p_Main->message("Configuring external terminal");
    configureQandEfPolarity(dataReadyFlag_, false);
    printBuffer.Printf("	Transmit baud rate: %d, receive baud rate: %d\n", baudRateValueSerial_[selectedBaudT_], baudRateValueSerial_[selectedBaudR_]);
    p_Main->message(printBuffer);
    
    vtEnabled_ = 1;
    vtCount_ = -1;
    vtOutCount_ = -1;
    vtOut_ = 0;
    vtOutSet_ = false;
    serialEf_ = 1;
}

void Serial::configureUart(IoConfiguration ioConfiguration)
{
    p_Computer->setOutType(ioConfiguration.uartOut, UARTOUTSERIAL);
    p_Computer->setInType(ioConfiguration.uartIn, UARTINSERIAL);
    p_Computer->setOutType(ioConfiguration.uartControl, UARTCONTROLSERIAL);
    p_Computer->setInType(ioConfiguration.uartStatus, UARTSTATUSSERIAL);
    p_Computer->setCycleType(VTCYCLE, VTSERIALCYCLE);
    
    wxString printBuffer;
    
    p_Main->message("Configuring external terminal connected to CDP1854/UART");
    startSerial();
    
    printBuffer.Printf("	Output %d: load transmitter, input %d: read receiver", ioConfiguration.uartOut, ioConfiguration.uartIn);
    p_Main->message(printBuffer);
    
    printBuffer.Printf("	Output %d: load control, input %d: read status", ioConfiguration.uartControl, ioConfiguration.uartStatus);
    p_Main->message(printBuffer);
    rs232_ = 0;
}

void Serial::configureUart16450(IoConfiguration ioConfiguration)
{
    wxString runningComp = p_Main->getRunningComputerStr();

    wxString ioGroup = "";
    if (ioConfiguration.uartIoGroup != -1)
        ioGroup.Printf(" on group %d", ioConfiguration.uartIoGroup);

    uart16450_ = true;

    p_Computer->setOutType(ioConfiguration.uartIoGroup+1, ioConfiguration.uartOut, UART16450_OUTSERIAL);
    p_Computer->setInType(ioConfiguration.uartIoGroup+1, ioConfiguration.uartIn, UART16450_INSERIAL);
    p_Computer->setOutType(ioConfiguration.uartIoGroup+1, ioConfiguration.uartControl, UART16450_CONTROLSERIAL);
    p_Computer->setInType(ioConfiguration.uartIoGroup+1, ioConfiguration.uartStatus, UART16450_STATUSSERIAL);

    wxString printBuffer;
    p_Main->message("Configuring external terminal connected to 16450 Uart" + ioGroup);

    printBuffer.Printf("	Output %d: register select, output %d: write selected", ioConfiguration.uartControl, ioConfiguration.uartOut);
    p_Main->message(printBuffer);

    printBuffer.Printf("	Input %d: read status, input %d: read selected\n", ioConfiguration.uartStatus, ioConfiguration.uartIn);
    p_Main->message(printBuffer);

    registerSelect_ = 0;
    modemControlRegister_ = 0;
    modemStatusRegister_ = 0;
    lineStatusRegister_ = 0xe0;
}

void Serial::configureRcasbc(int selectedBaudR, int selectedBaudT)
{
    uartEf_ = true;
    selectedBaudT_ = selectedBaudT;
    selectedBaudR_ = selectedBaudR;
    
    baudRateT_ = (int) ((((clock_ * 1000000) / 8) / baudRateValueSerial_[selectedBaudT_])+0.5);
    baudRateR_ = (int) ((((clock_ * 1000000) / 8) / baudRateValueSerial_[selectedBaudR_])+0.5);

    p_Computer->setCycleType(SERIALCYCLE, VTSERIALCYCLE);
    
    p_Main->message("Configuring external terminal connected to UART1 with MSM82C51");
    startSerial();
    
    p_Main->message("	A000-AFFF, 0: Data, 1: Status/Control Word");
    p_Main->message("	EF4/INT: RxRDY (reversed), EF3: TxRDY");

    wxString printBuffer;
    printBuffer.Printf("	Transmit baud rate: %d, receive baud rate: %d\n", baudRateValueSerial_[selectedBaudT_], baudRateValueSerial_[selectedBaudR_]);
    p_Main->message(printBuffer);
    
    rs232_ = 0;
    vtEnabled_ = 1;
    vtCount_ = -1;
    vtOutCount_ = -1;
    vtOut_ = 0;
    vtOutSet_ = false;
    serialEf_ = 1;
    reverseEf_ = true;
    dataReadyFlag_ = 0;
    
    uart_da_bit_ = 7;
    uart_fe_bit_ = 5;
    uart_tsre_bit_ = 0;
    uart_thre_bit_ = 2;
}

void Serial::configureMs2000(int selectedBaudR, int selectedBaudT)
{
    uartEf_ = true;
    selectedBaudT_ = selectedBaudT;
    selectedBaudR_ = selectedBaudR;
    
    baudRateT_ = (int) ((((clock_ * 1000000) / 8) / baudRateValueSerial_[selectedBaudT_])+0.5);
    baudRateR_ = (int) ((((clock_ * 1000000) / 8) / baudRateValueSerial_[selectedBaudR_])+0.5);

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
    vtOutSet_ = false;
    serialEf_ = 1;
    reverseEf_ = true;
    dataReadyFlag_ = 0;
}

void Serial::configureVt2K(int selectedBaudR, int selectedBaudT, IoConfiguration ioConfiguration)
{
    wxString printBuffer;
    
    selectedBaudT_ = selectedBaudT;
    selectedBaudR_ = selectedBaudR;
    
    baudRateT_ = (int) ((((clock_ * 1000000) / 8) / baudRateValueSerial_[selectedBaudT_])+0.5);
    baudRateR_ = (int) ((((clock_ * 1000000) / 8) / baudRateValueSerial_[selectedBaudR_])+0.5);

    if (uart_)
    {
        p_Computer->setCycleType(VTCYCLE, VTSERIALCYCLE);
        
        p_Main->message("Configuring external terminal");
        startSerial();
        
        rs232_ = 0;
    }
    else
    {
        reverseEf_ = ioConfiguration.vt100ReverseEf;
        reverseQ_ = ioConfiguration.vt100ReverseQ;
        
        p_Computer->setCycleType(VTCYCLE, VTSERIALCYCLE);
        dataReadyFlag_ = ioConfiguration.vt100Ef;
        p_Computer->setEfType(ioConfiguration.vt100Ef, VTSERIALEF);
        if (reverseQ_) p_Computer->setFlipFlopQ(1);
        
        wxString printEfReverse = ", ";
        wxString printQ = "Serial out: Q";
        
        if (reverseEf_ == 0)
            printEfReverse = "(reversed), ";
        
        if (reverseQ_ == 1)
            printQ = "Serial out: reversed Q";
        
        p_Main->message("Configuring external terminal");
        startSerial();
        
        printBuffer.Printf("	EF %d: serial input", ioConfiguration.vt100Ef);
        printBuffer = printBuffer + printEfReverse + printQ;
        p_Main->message(printBuffer);
    }
    
    printBuffer.Printf("	Transmit baud rate: %d, receive baud rate: %d\n", baudRateValueSerial_[selectedBaudT_], baudRateValueSerial_[selectedBaudR_]);
    p_Main->message(printBuffer);
    
    vtEnabled_ = 1;
    vtCount_ = -1;
    vtOutCount_ = -1;
    vtOut_ = 0;
    vtOutSet_ = false;
    serialEf_ = 1;
}

void Serial::startSerial()
{
    numberOfBitsPerByte_ = 9;
    sp_return error = sp_get_port_by_name(elfConfiguration_.serialPort_, &port);
    if (error == SP_OK)
    {
        error = sp_open(port, (sp_mode)(SP_MODE_READ | SP_MODE_WRITE));
        if (error == SP_OK)
        {
            sp_set_baudrate(port, baudRateValueSerial_[selectedBaudT_]);
            if (SetUpFeature_[VTBITS])
                sp_set_bits (port, 8);
            else
            {
                sp_set_bits (port, 7);
                numberOfBitsPerByte_--;
            }
            sp_set_stopbits(port, 1);
            sp_set_xon_xoff(port, SP_XONXOFF_DISABLED);
            sp_set_flowcontrol(port, SP_FLOWCONTROL_NONE);
            if (SetUpFeature_[VTPARITY])
            {
                numberOfBitsPerByte_++;
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
        if (uart_ || uart16450_)
        {
            numberOfBytes = sp_input_waiting(port);
            if (numberOfBytes >= 1)
                dataAvailable();
        }
        else
        {
            Byte input;
            numberOfBytes = sp_nonblocking_read(port, &input, 1);

            if (numberOfBytes >= 1)
            {
                vtOut_ = input;
                vtOutSet_ = true;
            }
        }
        cycleValue_ = cycleSize_;
    }

    if (uart_ || uart16450_)
    {
        vtCount_--;
        if (vtCount_ <= 0)
        {
            if (sp_output_waiting(port) == 0 && uartStatus_[uart_thre_bit_] == 0 && serialOpen_)
            {
#if defined(__ARM64__)
                sp_blocking_write(port, &rs232_, 1, 60);
#else
                sp_nonblocking_write(port, &rs232_, 1);
#endif
                p_Computer->thrStatusSerial(0);
                uartStatus_[uart_thre_bit_] = 1;
                uartStatus_[uart_tsre_bit_] = 1;
            }
            vtCount_ = baudRateR_ * numberOfBitsPerByte_;
            uartInterrupt();
        }
        if (vtOutCount_ > 0)
        {
            vtOutCount_--;
            if (uartEf_)
            {
                if (vtOutCount_ <= 0)
                {
                    if (vtOutBits_ == 10)
                        serialEf_ = 0;
                    else
                    {
                        serialEf_ = (vtOut_ & 1) ? 1 : 0;
                        if (vtOutBits_ > 10)
                            vtOut_ = 0;
                        else
                            vtOut_ = (vtOut_ >> 1) | 128;
                    }
                    vtOutCount_ = baudRateT_;
                    if (vtOutBits_ == 2)
                        serialEf_ = 1;
                    if (--vtOutBits_ == 0)
                    {
                        vtOut_ = 0;
                        p_Computer->dataAvailableSerial(1);
                        uartStatus_[uart_da_bit_] = 1;
                        vtOutCount_ = -1;
                        vtOutBits_=10;
                        uartInterrupt();
                    }
                    if (vtOutBits_ == 11)
                    {
                        serialEf_ = 1;
                        vtOutCount_ = -1;
                        vtOutBits_=10;
                    }
                }
            }
            else
            {
                if (vtOutCount_ == 0)
                {
                    p_Computer->dataAvailableSerial(1);
                    uartStatus_[uart_da_bit_] = 1;
                    vtOutCount_ = -1;
                    uartInterrupt();
                }
            }
        }
    }
    else  // if !uart
    {
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
                    vtOutSet_ = false;
                    p_Computer->setNotReadyToReceiveData(dataReadyFlag_-1);
                    vtOutCount_ = -1;
                }
            }
        }
        else
        {
            if (vtOutSet_ && vtEnabled_)
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
                    {
#if defined(__ARM64__)
                        sp_blocking_write(port, &rs232_, 1, 60);
#else
                        sp_nonblocking_write(port, &rs232_, 1);
#endif
                        while (sp_output_waiting(port) > 0)
                                       ;
                    }
                }
            }
        }
    }
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

void Serial::dataAvailable()
{
    if (!uart_ && !uart16450_)
        return;
    
    if (uartEf_)
        vtOutCount_ = baudRateT_ * 9;
    else
        vtOutCount_ = baudRateT_;
}

void Serial::dataAvailable(Byte value)
{
    if (!uart_ && !uart16450_)
        return;
    
    vtOut_ = value;
    if (uartEf_)
    {
        if (vtOut_ == WXK_ESCAPE)
        {
            vtOut_ = 0;
            vtOutBits_ = (58240/baudRateT_)+11;
        }
        vtOutCount_ = baudRateT_ * 9;
    }
    else
        vtOutCount_ = baudRateT_;
}

void Serial::dataAvailableUart16450(bool data)
{
    lineStatusRegister_[UART_LSR_DR] = data;
}

void Serial::framingError(bool data)
{
    uartStatus_[uart_fe_bit_] = data;
}

void Serial::selectUart16450Register(Byte value)
{
    registerSelect_ = value &0x7;
}

void Serial::uartOut(Byte value)
{
    if (sp_output_waiting(port) == 0)
    {
#if defined(__ARM64__)
        sp_blocking_write(port, &value, 1, 60);
#else
        sp_nonblocking_write(port, &rs232_, 1);
#endif
        vtCount_ = baudRateR_ * numberOfBitsPerByte_;
    }
    else
    {
        rs232_ = value;
        
        p_Computer->thrStatusSerial(1);
        uartStatus_[uart_thre_bit_] = 0;
        uartStatus_[uart_tsre_bit_] = 0;
    }
}

void Serial::uart16450Out(Byte value)
{
    switch (registerSelect_)
    {
        case 0: // THR
            if (modemControlRegister_[UART_MCR_LOOP])
            {
                thr_ = value;
                lineStatusRegister_[UART_LSR_DR] = 1;
            }
            else
                uartOut(value);
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
}

void Serial::uartControl(Byte value)
{
    uartControl_ = value;
    uartStatus_ = 0x80;
}

Byte Serial::uartIn()
{
    framingError(0);
    uartStatus_[uart_da_bit_] = 0;
    p_Computer->dataAvailableSerial(0);
        
    Byte input = 0;
    sp_nonblocking_read(port, &input, 1);

    return input;
}

Byte Serial::uart16450In()
{
    switch (registerSelect_)
    {
        case 0: // RHR
            if (modemControlRegister_[UART_MCR_LOOP])
            {
                lineStatusRegister_[UART_LSR_DR] = 0;
                return thr_;
            }
            else
                return uartIn();
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
}

Byte Serial::uartStatus()
{
    return uartStatus_.to_ulong();
}

void Serial::thrStatusUart16450(bool data)
{
    lineStatusRegister_[UART_LSR_THRE] = !data;
    lineStatusRegister_[UART_LSR_TRE] = data;
}

void Serial::uartInterrupt()
{
    if (uart16450_)
        return;
    
    if ((uartControl_ & 0x20) == 0x20)
        p_Computer->interrupt();
}

