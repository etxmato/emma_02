#ifndef IDE_H
#define IDE_H

#include "elfconfiguration.h"

class DriveGeometry
{
public:
    Word cylinders;
    Word heads;
    Word sectors;
    long maxLba;
};

class Ide
{
public:
    Ide();
    ~Ide() {};

    void configureIde(wxString ideFile1, wxString ideFile2, IoConfiguration portConf);
    void initializeIde(wxString ideFile);
    Byte inIde();
    void outIde(Byte value);
    void selectIdeRegister(Byte value);
    void cycleIde();

private:
    void initIde();
    long getOffset();
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
    Byte dataMode_;
    long ideCycles_;
};

#endif  // IDE_H
