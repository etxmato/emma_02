#ifndef IDE_H
#define IDE_H

#include "computerconfig.h"

class DriveGeometry
{
public:
    Word cylinders;
    Word heads;
    Word sectors;
    uint32_t maxLba;
};

#pragma pack(push,1)
struct _IDENTIFY_DEVICE_DATA {
  uint16_t wGeneralConfiguration;       // 
  uint16_t wNumberOfCylinders;          //
  uint16_t wSpecificConfiguration;      //
  uint16_t wNumberOfHeads;              //
  uint16_t wUnformattedBytesPerTrack;   //
  uint16_t wUnformattedBytesPerSector;  //
  uint16_t wSectorsPerTrack;            //
  uint16_t awATAreserved7[3];           //
  uint8_t  abSerialNumber[20];          //
  uint16_t wBufferType;                 //
  uint16_t wBufferSize;                 //
  uint16_t wECCbytes;                   //
  uint8_t  abFirmwareRevision[8];       //
  uint8_t  abModelNumber[40];           //
  uint16_t wReadWriteMultiple;          //
  uint16_t wTrustedComputing;           //
  uint16_t wCapabilities;               //
  uint16_t wATAreserved50;              //
  uint16_t wPIOtimingMode;              //
  uint16_t wDMAtimingMode;              //
  uint16_t wATAreserved53;              //
  uint16_t wNumberOfCurrentCylinders;   //
  uint16_t wNumberOfCurrentHeads;       //
  uint16_t wCurrentSectorsPerTrack;     //
  uint32_t lCurrentCapacity;            //
  uint16_t wATAreserved59;              //
  uint32_t lUserAddressableSectors;     //
  uint16_t wSingleWordDMA;              //
  uint16_t wMultiWordDMA;               //
  uint16_t awATAreserved64[64];         //
  uint16_t awVendorReserved[32];        //
  uint16_t awATAreserved160[96];        //
};
#pragma pack(pop)
typedef struct _IDENTIFY_DEVICE_DATA IDENTIFY_DEVICE_DATA;

class Ide
{
public:
    Ide();
    ~Ide() {};

    void configureIde(wxString ideFile1, wxString ideFile2, IdeConfiguration ideConfiguration);
    void setIdeDiskname(int disk, wxString fileName);
    void initializeIde(wxString ideFile);
    Byte inIde();
    void outIde(Byte value);
    void selectIdeRegister(Byte value);
    void cycleIde();
    Byte readIdeStatus();

private:
    void initIde();
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
    Byte command_;
    Byte activeStatus_;
    Byte inter_;
    int registerSelect_;
    Byte dataMode_;
    long ideCycles_;
};

#endif  // IDE_H
