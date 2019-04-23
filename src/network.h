#ifndef NETWORK_H
#define NETWORK_H

#include "elfconfiguration.h"
#define WD1793 0
#define WD1770 1

class Network
{
public:
	Network();
	~Network() {};

	void configureNetwork(int sides, int tracks, int sectors, int sectorlength, int computerType, double clock);
	Byte efNetwork();
	Byte inNetwork();
    void outNetwork(Byte value);

    void framingError(bool data);
    void uartOut(Byte value);
    void uartControl(Byte value);
    Byte uartIn();
    Byte uartStatus();

	Byte readRegisterNetwork();
	Byte intrqStatusNetwork();
	void writeRegisterNetwork(Byte value);
	void selectRegisterNetwork(Byte value);
    
	void cycleNetwork();
	void resetNetwork(); 

	void setDiskNameNetwork(int disk, wxString fileName);

protected:
	int registerSelectNetwork_;

private:
    Byte uartControl_;
    bitset<8> uartStatus_;
    Byte rs232_;
    
    int inpValue_[255];
    size_t inputNumber_;
    
    long inCount_;
    long outCount_;
    int baudRate_;

	int convertDriveNumber(int drive);
	void readSector(); 
	void writeSector(); 
	void formatSector(); 
	void endCommand(Byte stat); 
	void onCommand(Byte command);
	Byte readData();
	void writeData(Byte value);
	void updateFdcStatusLed();

	Byte *sectorBufferPointer;

	int localComputerType_;
	wxString diskName_[4];
	bool diskCreated_[4];
	int numberOfSides_[4];
	int numberOfTracksPerSide_;
	int numberOfTracks_[4];
	int numberOfSectors_;
	size_t sectorLength_;
	int intrq_;
	bool multipleSector_;
	int maxFmtCount_;

	Byte idam_;
	Byte track_;
	Byte sector_;
	Byte drive_;
	Byte side_;
	Byte status_;
	Byte command_;
	Byte data_;
	Byte exec_;
	Byte sectorBuffer_[1024];
	Byte drq_;
	int	driveTrack_[4];
	char stepDirection_;
	Byte formatTrack_;
	Byte formatSector_;
	int formatCount_;
	Byte formatSide_;
	long fdcCycles_;
};

#endif  // NETWORK_H

