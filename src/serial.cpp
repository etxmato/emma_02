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

Serial::Serial(int computerType, double clock, ComputerConfiguration computerConfig, int uartNumber)
{
    currentComputerConfiguration = computerConfig;
    computerType_ = computerType;
    clock_ = clock;
    uartNumber_ = uartNumber;

    terminalType_ = TERMINAL_SERIAL;
    if (currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].uart1854_defined)
        terminalType_ = TERMINAL_UART1854;
    if (currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].uart16450_defined)
        terminalType_ = TERMINAL_UART16450;
    if (currentComputerConfiguration.videoTerminalConfiguration.scn2671_defined)
        terminalType_ = TERMINAL_SCN2671;
    
    loopBack_ = false;
    loopInput_ = 0;

    setCycle();
    uartEf_ = false;
    uartControl_ = 0;
    uartStatus_ = 0xc0;

    serialOpen_ = false;
    
    uart_da_bit_ = 0;
    uart_fe_bit_ = 3;
    uart_tsre_bit_ = 6;
    uart_thre_bit_ = 7;
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

    reverseEf_ = currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].efReverse^1;

    switch (terminalType_)
    {
        case TERMINAL_UART1854:
            configureUart1854(videoTerminalConfiguration);
        break;

        case TERMINAL_UART16450:
            configureUart16450(videoTerminalConfiguration);
        break;

        case TERMINAL_SCN2671:
            p_Computer->setCycleType(CYCLE_TYPE_VIDEO_TERMINAL, EXTERNAL_VIDEO_TERMINAL_CYCLE);
            if (!videoTerminalConfiguration.external)
                startLoopBack();
            externalReceiveValue_ = 0;
        break;

        default:
            reverseQ_ = videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].reverseQ;
            if (reverseQ_) p_Computer->setFlipFlopQ(1);

            dataReadyFlag_ = videoTerminalConfiguration.ef.flagNumber;
            
            if (videoTerminalConfiguration.external)
                p_Main->configureMessage(&videoTerminalConfiguration.ioGroupVector, "external terminal");
            else
                p_Main->configureMessage(&videoTerminalConfiguration.ioGroupVector, "terminal loop back");

            printBuffer = "	Serial out: Q";
            if (reverseQ_ == 1)
                printBuffer =  "	Serial out: reversed Q";

            if (videoTerminalConfiguration.qOutput.portNumber[0] != -1)
            {
                p_Computer->setOutType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.qOutput, "serial out");
            }
            else
                p_Main->message(printBuffer);


            p_Computer->setOutType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.output, EXTERNAL_VIDEO_TERMINAL_OUT, "vtEnable");
            p_Computer->setEfType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.ef, EXTERNAL_VIDEO_TERMINAL_EF, "serial input");
            p_Computer->setCycleType(CYCLE_TYPE_VIDEO_TERMINAL, EXTERNAL_VIDEO_TERMINAL_CYCLE);
            if (videoTerminalConfiguration.external)
                startSerial();
            else
                startLoopBack();
        break;
    }
    
    printBuffer.Printf("	Transmit baud rate: %d, receive baud rate: %d\n", baudRateValueSerial_[selectedBaudT_], baudRateValueSerial_[selectedBaudR_]);
    p_Main->message(printBuffer);
    
    vtEnabled_ = 1;
    externalReceiveTimeCounter_ = -1;
    externalTransmitTimeCounter_ = -1;
    externalTransmitValue_ = 0;
    externalTransmitValueSet_ = false;
    serialEf_ = 1;
    serialEfInterrupt_ = 1;
}

void Serial::configureUart1854(VideoTerminalConfiguration videoTerminalConfiguration)
{
    wxString printBuffer;
    
    if (videoTerminalConfiguration.external)
        p_Main->configureMessage(&videoTerminalConfiguration.ioGroupVector, "external terminal connected to CDP1854 UART");
    else
        p_Main->configureMessage(&videoTerminalConfiguration.ioGroupVector, "terminal loop back connected to CDP1854 UART");

    if (videoTerminalConfiguration.uartOut.addressMode)
    {
        printBuffer.Printf("	Write address %04X: load transmitter",  videoTerminalConfiguration.uartOut.portNumber[0]);
        p_Main->message(printBuffer);
    }
    else
        p_Computer->setOutType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.uartOut, EXTERNAL_VT_UART1854_LOAD_TRANSMITTER_OUT, "load transmitter");
    
    if (videoTerminalConfiguration.uartIn.addressMode)
    {
        printBuffer.Printf("	Read address %04X: read receiver",  videoTerminalConfiguration.uartIn.portNumber[0]);
        p_Main->message(printBuffer);
    }
    else
        p_Computer->setInType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.uartIn, EXTERNAL_VT_UART1854_READ_RECEIVER_IN, "read receiver");
    
    if (videoTerminalConfiguration.uartControl.addressMode)
    {
        printBuffer.Printf("    Write address %04X: load control",  videoTerminalConfiguration.uartControl.portNumber[0]);
        p_Main->message(printBuffer);
    }
    else
        p_Computer->setOutType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.uartControl, EXTERNAL_VT_UART1854_LOAD_CONTROL_OUT, "load control");
    
    if (videoTerminalConfiguration.uartStatus.addressMode)
    {
        printBuffer.Printf("    Read address %04X: read status",  videoTerminalConfiguration.uartStatus.portNumber[0]);
        p_Main->message(printBuffer);
    }
    else
        p_Computer->setInType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.uartStatus, EXTERNAL_VT_UART1854_READ_STATUS_IN, "read status");
    
    if (currentComputerConfiguration.videoTerminalConfiguration.efInterrupt.flagNumber != -1)
        p_Computer->setEfType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.efInterrupt, EXTERNAL_VIDEO_TERMINAL_EF_INTERRUPT, "UART interrupt");
    if (currentComputerConfiguration.videoTerminalConfiguration.ef.flagNumber != -1)
    {
        p_Computer->setEfType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.ef, EXTERNAL_VIDEO_TERMINAL_EF, "serial input");
        uartEf_ = true;
    }
    p_Computer->setCycleType(CYCLE_TYPE_VIDEO_TERMINAL, EXTERNAL_VIDEO_TERMINAL_CYCLE);

    if (videoTerminalConfiguration.external)
        startSerial();
    else
        startLoopBack();
    externalReceiveValue_ = 0;
}

void Serial::configureUart16450(VideoTerminalConfiguration videoTerminalConfiguration)
{
    if (videoTerminalConfiguration.external)
        p_Main->configureMessage(&videoTerminalConfiguration.ioGroupVector, "external terminal connected to 16450/550 UART");
    else
        p_Main->configureMessage(&videoTerminalConfiguration.ioGroupVector, "terminal loop back connected to 16450/550 UART");

    p_Computer->setOutType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.uartOut, EXTERNAL_UART16450_WRITE_REGISTER_OUT, "write selected");
    p_Computer->setOutType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.uartControl, EXTERNAL_UART16450_CONTROL_OUT, "register select");
    p_Computer->setInType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.uartIn, EXTERNAL_UART16450_READ_REGISTER_IN, "read selected");
    p_Computer->setInType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.uartStatus, EXTERNAL_UART16450_READ_STATUS_IN, "read status");
    p_Computer->setCycleType(CYCLE_TYPE_VIDEO_TERMINAL, EXTERNAL_VIDEO_TERMINAL_CYCLE);

    if (videoTerminalConfiguration.external)
        startSerial();
    else
        startLoopBack();
    externalReceiveValue_ = 0;

    registerSelect_ = 0;
    modemControlRegister_ = 0;
    modemStatusRegister_ = 0;
    lineStatusRegister_ = 0xe0;
}

void Serial::startSerial()
{
    setNumberOfBits();
    sp_return error = sp_get_port_by_name(currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].serialPort, &port);
    if (error == SP_OK)
    {
        error = sp_open(port, (sp_mode)(SP_MODE_READ | SP_MODE_WRITE));
        if (error == SP_OK)
        {
            sp_set_baudrate(port, baudRateValueSerial_[selectedBaudT_]);
            sp_set_bits (port, currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].bitsPerCharacter);
            sp_set_stopbits(port, (int) currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].stopBit);
            sp_set_xon_xoff(port, SP_XONXOFF_DISABLED);
            sp_set_flowcontrol(port, SP_FLOWCONTROL_NONE);
            if (currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].parity)
            {
                if (currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].paritySense)
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

void Serial::startLoopBack()
{
    setNumberOfBits();
    loopBack_ = true;
}

void Serial::setNumberOfBits()
{
    double intpart;
    stopBitFactor_ = modf(currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].stopBit, &intpart);
    totalNumberOfBitsPerCharacter_ = (int) (currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].bitsPerCharacter + intpart);
    
    if (currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].parity)
        totalNumberOfBitsPerCharacter_++;
}

Byte Serial::ef()
{
    return(reverseEf_^serialEf_);
}

Byte Serial::efInterrupt()
{
    return (currentComputerConfiguration.videoTerminalConfiguration.efInterrupt.reverse^serialEfInterrupt_);
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
        size_t numberOfBytes = 0;
        if (terminalType_ != TERMINAL_SERIAL)
        {
            if (serialOpen_)
                numberOfBytes = sp_input_waiting(port);
            if (loopBack_)
            {
                if (loopInput_ != 0)
                    numberOfBytes = 1;
            }
            if (numberOfBytes >= 1)
                p_Computer->dataAvailableUart(1, uartNumber_);
        }
        else
        {
            Byte input = 0;
            if (serialOpen_)
                numberOfBytes = sp_nonblocking_read(port, &input, 1);
            if (loopBack_)
            {
                if (loopInput_ != 0)
                {
                    numberOfBytes = 1;
                    input = loopInput_;
                    loopInput_ = 0;
                }
            }
                
            if (numberOfBytes >= 1)
            {
                externalTransmitValue_ = input;
                externalTransmitValueSet_ = true;
            }
        }
        cycleValue_ = cycleSize_;
    }

    if (terminalType_ != TERMINAL_SERIAL)
    {
        if (externalTransmitTimeCounter_ > 0)
            externalUartTransmit();
            
        externalUartReceive();
    }
    else  // if !uart
    {
        if (externalTransmitTimeCounter_ > 0)
        {
            externalSerialTransmit();
        }
        else
        {
            if (externalTransmitValueSet_ && vtEnabled_)
            {
                serialEf_ = 0;
                parity_ = Parity(externalTransmitValue_);
                externalTransmitTimeCounter_ = baudRateT_;
                externalTransmitTotalBitCounter_ = totalNumberOfBitsPerCharacter_;
                externalTransmitDataBitCounter_ = currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].bitsPerCharacter;
                p_Computer->setGreenLed(serialEf_ ^ 1);
            }
        }
        
        if (externalReceiveTimeCounter_ >= 0)
            externalSerialReceive();
    }
}

void Serial::externalUartTransmit() // input from external device
{
    externalTransmitTimeCounter_--;
    if (uartEf_)
    {
        if (externalTransmitTimeCounter_ <= 0)
        {
            if (externalTransmitTotalBitCounter_ == 10)
                serialEf_ = 0;
            else
            {
                serialEf_ = (externalTransmitValue_ & 1) ? 1 : 0;
                if (externalTransmitTotalBitCounter_ > 10)
                    externalTransmitValue_ = 0;
                else
                    externalTransmitValue_ = (externalTransmitValue_ >> 1) | 128;
            }
            externalTransmitTimeCounter_ = baudRateT_;
            if (externalTransmitTotalBitCounter_ == 2)
                serialEf_ = 1;
            if (--externalTransmitTotalBitCounter_ == 0)
            {
                externalTransmitValue_ = 0;
                dataAvailableUart(1);
                externalTransmitTimeCounter_ = -1;
                externalTransmitTotalBitCounter_=10;
            }
            if (externalTransmitTotalBitCounter_ == 11)
            {
                serialEf_ = 1;
                externalTransmitTimeCounter_ = -1;
                externalTransmitTotalBitCounter_=10;
            }
        }
    }
    else
    {
        if (externalTransmitTimeCounter_ == 0)
        {
            dataAvailableUart(1);
            externalTransmitTimeCounter_ = -1;
        }
    }
}

void Serial::externalUartReceive() // output from external device
{
    externalReceiveTimeCounter_--;
    if (externalReceiveTimeCounter_ <= 0)
    {
        if (sp_output_waiting(port) == 0 && uartStatus_[uart_thre_bit_] == 0 && serialOpen_)
        {
            if (currentComputerConfiguration.videoTerminalConfiguration.externalBlockingWrite)
                sp_blocking_write(port, &externalReceiveValue_, 1, 60);
            else
                sp_nonblocking_write(port, &externalReceiveValue_, 1);
            p_Computer->thrStatusSerial(0);
            uartStatus_[uart_thre_bit_] = 1;
            uartStatus_[uart_tsre_bit_] = 1;
        }
        if (loopBack_ && uartStatus_[uart_thre_bit_] == 0)
        {
            loopInput_ = externalReceiveValue_;
            externalReceiveValue_ = 0;

            p_Computer->thrStatusSerial(0);
            uartStatus_[uart_thre_bit_] = 1;
            uartStatus_[uart_tsre_bit_] = 1;
        }
        externalReceiveTimeCounter_ = baudRateR_ * totalNumberOfBitsPerCharacter_;
    }
}

void Serial::externalSerialTransmit() // input from external device
{
    externalTransmitTimeCounter_--;
    if (externalTransmitTimeCounter_ <= 0)
    {
        if (externalTransmitDataBitCounter_-- > 0)
        {
            serialEf_ = (externalTransmitValue_ & 1) ? 1 : 0;
            externalTransmitValue_ = (externalTransmitValue_ >> 1) | 128;
        }
        else
        {
            if (currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].parity && externalTransmitDataBitCounter_ == -1)
                serialEf_ = parity_;
            else
                serialEf_ = 1;
        }
        externalTransmitTimeCounter_ = baudRateT_;
        if (--externalTransmitTotalBitCounter_ == 0)
        {
            externalTransmitValue_ = 0;
            externalTransmitValueSet_ = false;
            p_Computer->setNotReadyToReceiveData(dataReadyFlag_-1);
            externalTransmitTimeCounter_ = -1;
        }
    }
}

void Serial::externalSerialReceive() // output to external device
{
        externalReceiveTimeCounter_--;
        if (externalReceiveTimeCounter_ <= 0)
        {
            if (externalReceiveDataBitCounter_ > 0)
            {
                externalReceiveValue_ >>= 1;
                externalReceiveValue_ |= (flipFlopQ_ ^ reverseQ_) ? 128 : 0;

                if (--externalReceiveDataBitCounter_ == 0)
                {
                    for (int i=0; i<(8-currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].bitsPerCharacter); i++)
                        externalReceiveValue_ >>= 1;
                }
            }

            externalReceiveTimeCounter_ = baudRateR_;
            if (externalReceiveTotalBitCounter_ == 2 && stopBitFactor_ != 0)
                externalReceiveTimeCounter_ = baudRateR_ * (1+ stopBitFactor_);

            if (--externalReceiveTotalBitCounter_ == 0)
            {
                externalReceiveTimeCounter_ = -1;
                if (serialOpen_)
                {
                    if (currentComputerConfiguration.videoTerminalConfiguration.externalBlockingWrite)
                        sp_blocking_write(port, &externalReceiveValue_, 1, 60);
                    else
                        sp_nonblocking_write(port, &externalReceiveValue_, 1);
                    
                    while (sp_output_waiting(port) > 0);
                }
                if (loopBack_)
                {
                    loopInput_ = externalReceiveValue_;
                    externalReceiveValue_ = 0;
                }
            }
        }
}

Byte Serial::readReceiverHoldingRegister()
{
    Byte input = 0;
    if (serialOpen_)
        sp_nonblocking_read(port, &input, 1);
    if (loopBack_ && loopInput_ != 0)
    {
        input = loopInput_;
        loopInput_ = 0;
    }

    return input;
}

void Serial::serialDataOutput(Byte transmitterHoldingRegister)
{
    if (serialOpen_)
    {
        if (currentComputerConfiguration.videoTerminalConfiguration.externalBlockingWrite)
            sp_blocking_write(port, &transmitterHoldingRegister, 1, 60);
        else
            sp_nonblocking_write(port, &transmitterHoldingRegister, 1);
        while (sp_output_waiting(port) > 0);
    }
    if (loopBack_)
        loopInput_ = transmitterHoldingRegister;
}

void Serial::switchQ(int value)
{
    flipFlopQ_ = value;

    if (externalReceiveTimeCounter_ < 0)
    {
        if ((value ^ reverseQ_) == 0)
        {
            externalReceiveTimeCounter_ = baudRateR_ + baudRateR_ / 2;
            externalReceiveTotalBitCounter_ = totalNumberOfBitsPerCharacter_;
            externalReceiveDataBitCounter_ = currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].bitsPerCharacter;
            externalReceiveValue_ = 0;
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
    int par=0;
    for (int i=0; i<currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].bitsPerCharacter; i++)
    {
        if (value & 1) par++;
        value >>= 1;
    }
    if (currentComputerConfiguration.videoTerminalConfiguration.terminalInterfaceSetting[currentComputerConfiguration.videoTerminalConfiguration.selectedTerminalSetting].paritySense)
        return(par & 1);
    else
        return((par & 1) ^ 1);
}

void Serial::dataAvailable()
{
    if (terminalType_ == TERMINAL_SERIAL)
        return;
    
    if (uartEf_)
        externalTransmitTimeCounter_ = baudRateT_ * 9;
    else
        externalTransmitTimeCounter_ = baudRateT_;
}

void Serial::dataAvailable(Byte value)
{
    if (terminalType_ == TERMINAL_SERIAL)
        return;
    
    externalTransmitValue_ = value;
    if (uartEf_)
    {
        if (externalTransmitValue_ == WXK_ESCAPE)
        {
            externalTransmitValue_ = 0;
            externalTransmitTotalBitCounter_ = (58240/baudRateT_)+11;
        }
        externalTransmitTimeCounter_ = baudRateT_ * 9;
    }
    else
        externalTransmitTimeCounter_ = baudRateT_;

    uartStatus_[uart_da_bit_] = 1;
}

void Serial::dataAvailableUart(bool data)
{
    lineStatusRegister_[UART_LSR_DR] = data;
    uartStatus_[uart_da_bit_] = data;
    if (data)
        uartInterrupt();
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
    if (serialOpen_)
    {
        if (sp_output_waiting(port) == 0)
        {
            if (currentComputerConfiguration.videoTerminalConfiguration.externalBlockingWrite)
                sp_blocking_write(port, &value, 1, 60);
            else
                sp_nonblocking_write(port, &value, 1);
            externalReceiveTimeCounter_ = baudRateR_ * totalNumberOfBitsPerCharacter_;
        }
        else
        {
            externalReceiveValue_ = value;
            
            p_Computer->thrStatusSerial(1);
            uartStatus_[uart_thre_bit_] = 0;
            uartStatus_[uart_tsre_bit_] = 0;
        }
    }
    if (loopBack_)
    {
        externalReceiveValue_ = value;

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
    if ((value & 0x80) == 0x80)
    {
        uartInterrupt();
    }
    else
        uartControl_ = value;
    
    if (!currentComputerConfiguration.videoTerminalConfiguration.threUnchangedAtControl)
        uartStatus_[uart_thre_bit_] = 1;
}

Byte Serial::uartIn()
{
    framingError(0);
    dataAvailableUart(0);

    Byte input = 0;
    if (serialOpen_)
        sp_nonblocking_read(port, &input, 1);
    if (loopBack_ && loopInput_ != 0)
    {
        input = loopInput_;
        loopInput_ = 0;
    }

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
    clearUartInterrupt();
    return uartStatus_.to_ulong();
}

void Serial::thrStatusUart16450(bool data)
{
    lineStatusRegister_[UART_LSR_THRE] = !data;
    lineStatusRegister_[UART_LSR_TRE] = data;
}

void Serial::uartInterrupt()
{
    if (terminalType_ == TERMINAL_UART16450)
        return;
    
    if ((uartControl_ & 0x20) == 0x20 && currentComputerConfiguration.videoTerminalConfiguration.interrupt)
        p_Computer->requestInterrupt(INTERRUPT_TYPE_UART, true, currentComputerConfiguration.videoTerminalConfiguration.picInterrupt);
    if (currentComputerConfiguration.videoTerminalConfiguration.efInterrupt.flagNumber != -1)
        serialEfInterrupt_ = 0;
}

void Serial::clearUartInterrupt()
{
    if (terminalType_ == TERMINAL_UART16450)
        return;
    
    p_Computer->requestInterrupt(INTERRUPT_TYPE_UART, false, currentComputerConfiguration.videoTerminalConfiguration.picInterrupt);
    if (currentComputerConfiguration.videoTerminalConfiguration.efInterrupt.flagNumber != -1)
        serialEfInterrupt_ = 1;
}

