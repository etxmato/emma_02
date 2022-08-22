#ifndef FDC_H
#define FDC_H

#include "elfconfiguration.h"
#define WD1793 0
#define WD1770 1

class Fdc
{
public:
    Fdc();
    ~Fdc() {};

    void configure1793(int sides, int tracks, int sectors, int sectorlength, int maxFmtCount, int computerType, ElfPortConfiguration portConf);
    Byte ef1793();
    Byte in1793();
    void selectRegister1793(Byte value);
    void writeRegister1793(Byte value);

    void configure1770(int sides, int tracks, int sectors, int sectorlength, int computerType);
    Byte ef1770();
    Byte in1770();
    Byte readRegister1770();
    Byte intrqStatus1770();
    void out1770(Byte value);
    void writeRegister1770(Byte value);
    void selectRegister1770(Byte value);
    void cycleFdc();
    void resetFdc(); 

    void setDiskName(int disk, wxString fileName);

protected:
    int registerSelect_;

private:
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
    int    driveTrack_[4];
    char stepDirection_;
    Byte formatTrack_;
    Byte formatSector_;
    int formatCount_;
    Byte formatSide_;
    long fdcCycles_;
};

#endif  // FDC_H

