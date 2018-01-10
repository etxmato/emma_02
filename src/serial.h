#ifndef SERIAL_H
#define SERIAL_H

#include <bitset>
using namespace std;

class Serial
{
public:

	Serial(int computerType, double clock, ElfConfiguration elfConfiguration);
	~Serial();

	void configure(int selectedBaudR, int selectedBaudT, ElfPortConfiguration elfPortConf);
	void configureMember(int selectedBaudR, int selectedBaudT); 
	void configureMcds(int selectedBaudR, int selectedBaudT);
	void configureCosmicos(int selectedBaudR, int selectedBaudT);
    void configureVip(int selectedBaudR, int selectedBaudT);
    void configureVelf(int selectedBaudR, int selectedBaudT);
	void configureUart(ElfPortConfiguration elfPortConf);
    void configureMs2000(int selectedBaudR, int selectedBaudT);
    void configureVt2K(int SelectedBaudR, int SelectedBaudT, ElfPortConfiguration elfPortConf);
    void startSerial();
    void configureQandEfPolarity(int ef, bool vtEnable);
	Byte ef();
	void out(Byte value);
	void cycleVt();

    void switchQ(int value);
	void setCycle();
	int Parity(int value);
	void dataAvailable();
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

	int reverseEf_;
	int reverseQ_;

	Byte uartControl_;
	bitset<8> uartStatus_;
	bool uart_;
    bool serialOpen_;

	bitset<32> SetUpFeature_;
    
	int dataReadyFlag_;
};

#endif  // SERIAL_H
