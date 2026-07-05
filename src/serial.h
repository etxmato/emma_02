#ifndef SERIAL_H
#define SERIAL_H

#include <bitset>
using namespace std;

class Serial
{
public:

    Serial(int computerType, double clock, ComputerConfiguration computerConfig, int uartNumber);
    ~Serial();

    void configure(int selectedBaudR, int selectedBaudT, VideoTerminalConfiguration videoTerminalConfiguration);
    void configureUart1854(VideoTerminalConfiguration videoTerminalConfiguration);
    void configureUart16450(VideoTerminalConfiguration videoTerminalConfiguration);
    void startSerial();
    void startLoopBack();
    void setNumberOfBits();
    Byte ef();
    Byte efInterrupt();
    void out(Byte value);
    void cycleVt();

    void externalUartTransmit();
    void externalUartReceive();
    void externalSerialTransmit();
    void externalSerialReceive();
    Byte readReceiverHoldingRegister();
    void serialDataOutput(Byte transmitterHoldingRegister);
    void switchQ(int value);
    void setCycle();
    int Parity(int value);
    void dataAvailable();
    void dataAvailable(Byte value);
    void dataAvailableUart(bool data);
    void framingError(bool data);
    void selectUart16450Register(Byte value);
    void uartOut(Byte value);
    void uart16450Out(Byte value);
    void uartControl(Byte value);
    Byte uartIn(); 
    Byte uart16450In();
    Byte uartStatus();
    void thrStatusUart16450(bool data);
    void uartInterrupt();
    void clearUartInterrupt();

private:
    ComputerConfiguration currentComputerConfiguration;

    double clock_;

    int computerType_;

    int cycleValue_;
    int cycleSize_;

    int baudRateT_;
    int selectedBaudT_;
    int baudRateR_;
    int selectedBaudR_;
    
    int totalNumberOfBitsPerCharacter_;
    double stopBitFactor_;
    
    int externalReceiveTotalBitCounter_;
    int externalReceiveDataBitCounter_;
    long externalReceiveTimeCounter_;
    Byte externalReceiveValue_;
    
    int externalTransmitTotalBitCounter_;
    int externalTransmitDataBitCounter_;
    long externalTransmitTimeCounter_;
    Byte externalTransmitValue_;
    bool externalTransmitValueSet_;
    
    Byte flipFlopQ_;

    int parity_;

    Byte serialEf_;
    Byte serialEfInterrupt_;
    Byte vtEnabled_;

    bool uartEf_;
    int reverseEf_;
    int reverseQ_;

    int terminalType_;
    
    Byte uartControl_;
    bitset<8> uartStatus_;
    bool serialOpen_;
    bool loopBack_;
    Byte loopInput_;
    
    int dataReadyFlag_;

    int uart_da_bit_;
    int uart_fe_bit_;
    int uart_tsre_bit_;
    int uart_thre_bit_;
    
    bitset<8> modemControlRegister_;
    bitset<8> modemStatusRegister_;
    bitset<8> lineStatusRegister_;
    Byte thr_;
    
    int registerSelect_;
    int uartNumber_;
};

#endif  // SERIAL_H
