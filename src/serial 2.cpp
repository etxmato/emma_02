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

Serial::Serial(int computerType, double clock, ComputerConfiguration computerConfig)
{
    currentComputerConfiguration = computerConfig;
    computerType_ = computerType;
    clock_ = clock;

    uart1854_ = currentComputerConfiguration.videoTerminalConfiguration.uart1854_defined;
    uart16450_ = currentComputerConfiguration.videoTerminalConfiguration.uart16450_defined;
    SetUpFeature_ = currentComputerConfiguration.videoTerminalConfiguration.vtExternalSetUpFeature;

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

void Serial::configure(int selectedBaudR, int selectedBaudT, VideoTerminalConfiguration videoTerminalConfiguration)
{
    wxString printBuffer;
    
    selectedBaudT_ = selectedBaudT;
    selectedBaudR_ = selectedBaudR;
    
    baudRateT_ = (int) ((((clock_ * 1000000) / 8) / baudRateValueSerial_[selectedBaudT_])+0.5);
    baudRateR_ = (int) ((((clock_ * 1000000) / 8) / baudRateValueSerial_[selectedBaudR_])+0.5);

    if (uart1854_)
        configureUart1854(videoTerminalConfiguration);
    else
    {
        if (uart16450_)
            configureUart16450(videoTerminalConfiguration);
        else
        {
            reverseEf_ = videoTerminalConfiguration.ef.reverse^1;
            reverseQ_ = videoTerminalConfiguration.reverseQ^1;
            
            dataReadyFlag_ = videoTerminalConfiguration.ef.flagNumber;

            if (reverseQ_) p_Computer->setFlipFlopQ(1);
            
            p_Main->configureMessage(&videoTerminalConfiguration.ioGroupVector, "external terminal");
            
            printBuffer = "	Serial out: Q";
            if (reverseQ_ == 1)
                printBuffer = "	Serial out: reversed Q";
            p_Main->message(printBuffer);

            p_Computer->setOutType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.output, EXTERNAL_VIDEO_TERMINAL_OUT, "vtEnable");
            p_Computer->setEfType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.ef, EXTERNAL_VIDEO_TERMINAL_EF, "serial input");
            p_Computer->setCycleType(CYCLE_TYPE_VIDEO_TERMINAL, EXTERNAL_VIDEO_TERMINAL_CYCLE);
            startSerial();
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

void Serial::configureUart1854(VideoTerminalConfiguration videoTerminalConfiguration)
{
    p_Main->configureMessage(&videoTerminalConfiguration.ioGroupVector, "external terminal connected to CDP1854/UART");
    p_Computer->setOutType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.uartOut, EXTERNAL_UART1854_LOAD_TRANSMITTER_OUT, "load transmitter");
    p_Computer->setInType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.uartIn, EXTERNAL_UART1854_READ_RECEIVER_IN, "read receiver");
    p_Computer->setOutType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.uartControl, EXTERNAL_UART1854_LOAD_CONTROL_OUT, "load control");
    p_Computer->setInType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.uartStatus, EXTERNAL_UART1854_READ_STATUS_IN, "read status");
    p_Computer->setEfType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.ef, EXTERNAL_VIDEO_TERMINAL_EF, "serial input");
    p_Computer->setCycleType(CYCLE_TYPE_VIDEO_TERMINAL, EXTERNAL_VIDEO_TERMINAL_CYCLE);

    startSerial();
    rs232_ = 0;
}

void Serial::configureUart16450(VideoTerminalConfiguration videoTerminalConfiguration)
{
    uart16450_ = true;

    p_Main->configureMessage(&videoTerminalConfiguration.ioGroupVector, "external terminal connected to 16450/550 UART");
    p_Computer->setOutType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.uartOut, EXTERNAL_UART16450_WRITE_REGISTER_OUT, "write selected");
    p_Computer->setOutType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.uartControl, EXTERNAL_UART16450_CONTROL_OUT, "register select");
    p_Computer->setInType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.uartIn, EXTERNAL_UART16450_READ_REGISTER_IN, "read selected");
    p_Computer->setInType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.uartStatus, EXTERNAL_UART16450_READ_STATUS_IN, "read status");
    p_Computer->setCycleType(CYCLE_TYPE_VIDEO_TERMINAL, EXTERNAL_VIDEO_TERMINAL_CYCLE);

    startSerial();
    rs232_ = 0;

    registerSelect_ = 0;
    modemControlRegister_ = 0;
    modemStatusRegister_ = 0;
    lineStatusRegister_ = 0xe0;
}

void Serial::startSerial()
{
    numberOfBitsPerByte_ = 9;
    sp_return error = sp_get_port_by_name(currentComputerConfiguration.videoTerminalConfiguration.serialPort, &port);
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
        if (uart1854_ || uart16450_)
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

    if (uart1854_ || uart16450_)
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
    if (!uart1854_ && !uart16450_)
        return;
    
    if (uartEf_)
        vtOutCount_ = baudRateT_ * 9;
    else
        vtOutCount_ = baudRateT_;
}

void Serial::dataAvailable(Byte value)
{
    if (!uart1854_ && !uart16450_)
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

