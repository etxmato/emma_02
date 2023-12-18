#ifndef ELF2KDISK_H
#define ELF2KDISK_H

#include <bitset>
using namespace std;

class Elf2KDisk
{
public:
    Elf2KDisk();
    ~Elf2KDisk() {};

    void configureDisk(wxString ideFile1, wxString ideFile2, bool rtc, bool uart, IoConfiguration ioConfiguration, bool use8275);
    void configureUart16450(IoConfiguration ioConfiguration);
    void initializeIde(wxString ideFile);
    Byte inDisk();
    Byte readDiskStatus() {return 255;};
    void outDisk(Byte value);
    void selectDiskRegister(Byte value);
    void cycleDisk();
    void writeRtc(int address, Byte value);
    void dataAvailableUart(bool data);
    void thrStatusUart(bool data); 

protected:
    Byte rtcRam_[128];

private:
    void initDisk();
    wxFileOffset getOffset();
    void writeSector();
    void readSector();
    void readId();
    void setGeometry(int cyl,int hd,int sc);
    void writeIdeRegister(int reg,Word value);
    Word readIdeRegister(int reg);
    void onCommand();

    DriveGeometry geometry_[2];

    wxString driveName_[2];
    bool driveCreated_[2];

    Byte sectorBuffer_[512];
    Word bufferPosition_;
    Byte error_;
    Byte sectorCount_;
    Byte startSector_;
    Word cylinder_;
    Byte headDevice_;
    Byte status_;
    Byte features_;
    Byte command_;
    Byte activeStatus_;
    Byte inter_;
    int registerSelect_;
    int deviceSelect_;
    Byte dataMode_;
    long ideCycles_;

    bool rtc_;
    bool uart_;
    bool disk_;
    bool use8275_;
    bool ideChecked_;

    bitset<8> modemControlRegister_;
    bitset<8> modemStatusRegister_;
    bitset<8> lineStatusRegister_;
    Byte thr_;
};

#endif  // ELF2KDISK_H
