#ifndef SERIAL_H
#define SERIAL_H

#include <bitset>
using namespace std;

class Serial
{
public:

    Serial(int computerType, double clock, ElfConfiguration elfConfiguration);
    ~Serial();

    void configure(int selectedBaudR, int selectedBaudT, IoConfiguration ioConfiguration);
    void configureStandard(int selectedBaudR, int selectedBaudT, int dataReadyFlag);
    void configureUart(IoConfiguration ioConfiguration);
    void configureRcasbc(int selectedBaudR, int selectedBaudT);
    void configureMs2000(int selectedBaudR, int selectedBaudT);
    void configureVt2K(int SelectedBaudR, int SelectedBaudT, IoConfiguration ioConfiguration);
    void startSerial();
    void configureQandEfPolarity(int ef, bool vtEnable);
    Byte ef();
    void out(Byte value);
    void cycleVt();

    void switchQ(int value);
    void setCycle();
    int Parity(int value);
    void dataAvailable();
    void dataAvailable(Byte value);
    void framingError(bool data); 
    void uartOut(Byte value); 
    void uartControl(Byte value); 
    Byte uartIn(); 
    Byte uartStatus(); 

private:
    ElfConfiguration elfConfiguration_;

    double clock_;

    int computerType_;

    int cycleValue_;
    int cycleSize_;

    long vtCount_;
    int baudRateT_;
    int selectedBaudT_;
    int baudRateR_;
    int selectedBaudR_;
    int vtBits_;
    Byte rs232_;
    int vtOutBits_;
    long vtOutCount_;
    Byte vtOut_;

    int parity_;

    Byte serialEf_;
    Byte vtEnabled_;

    bool uartEf_;
    int reverseEf_;
    int reverseQ_;

    Byte uartControl_;
    bitset<8> uartStatus_;
    bool uart_;
    bool uart16450_;
    bool serialOpen_;

    bitset<32> SetUpFeature_;
    
    int dataReadyFlag_;

    int uart_da_bit_;
    int uart_fe_bit_;
    int uart_tsre_bit_;
    int uart_thre_bit_;
};

#endif  // SERIAL_H
