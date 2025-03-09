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

/*
 *******************************************************************
 *** This software is copyright 2006 by Michael H Riley          ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/ffile.h"
#include "wx/filefn.h"

#include "main.h"
#include "ide.h"

enum {
    IDE_STAT_ERROR,
    IDE_STAT_INDEX,
    IDE_STAT_ECC,
    IDE_STAT_DRQ,
    IDE_STAT_SKC,
    IDE_STAT_WFT,
    IDE_STAT_RDY,
    IDE_STAT_BSY
};

#define IDE_CMD_RESET        1
#define IDE_CMD_SETF         2
#define IDE_CMD_WRITE        3
#define IDE_CMD_WRITE_1      4
#define IDE_CMD_READ         5
#define IDE_CMD_READ_1       6
#define IDE_CMD_ID           7

enum {
    IDE_CMD_BLK_DATA = 0,
    IDE_CMD_BLK_FEATURES = 1,
    IDE_CMD_BLK_SECTOR_COUNT = 2,
    IDE_CMD_BLK_SECTOR_NUMBER = 3,
    IDE_CMD_BLK_CYLINDER_LOW = 4,
    IDE_CMD_BLK_CYLINDER_HIGH = 5,
    IDE_CMD_BLK_DEVICE_HEAD = 6,
    IDE_CMD_BLK_COMMAND = 7,
    IDE_CMD_BLK_DEVICE_CONTROL= 0xE
};

Ide::Ide()
{
}

void Ide::configureIde(wxString ideFile1, wxString ideFile2, IdeConfiguration ideConfiguration)
{
    initializeIde(ideFile1);

    p_Main->configureMessage(&ideConfiguration.ioGroupVector, "IDE" );
    p_Computer->setInType(&ideConfiguration.ioGroupVector, ideConfiguration.input, "read selected");
    p_Computer->setInType(&ideConfiguration.ioGroupVector, ideConfiguration.status, "read status");
    p_Computer->setOutType(&ideConfiguration.ioGroupVector, ideConfiguration.selectOutput, "select port");
    p_Computer->setOutType(&ideConfiguration.ioGroupVector, ideConfiguration.writeOutput, "write selected");
    p_Computer->setCycleType(CYCLE_TYPE_DISK_IDE, IDE_CYCLE);

    wxString printBuffer;
    printBuffer.Printf("	Disk geometry: %d tracks, %d heads, %d sectors\n", ideConfiguration.tracks, ideConfiguration.heads, ideConfiguration.sectors);
    p_Main->message(printBuffer);

    setGeometry(ideConfiguration.tracks, ideConfiguration.heads, ideConfiguration.sectors);

    driveName_[0] = ideFile1;
    driveName_[1] = ideFile2;

    for (int i=0; i<2; i++)
    {
        driveCreated_[i] = wxFile::Exists(driveName_[i]);
    }

    initIde();
}

void Ide::setIdeDiskname(int disk, wxString fileName)
{
    driveName_[disk] = fileName;
    driveCreated_[disk] = wxFile::Exists(driveName_[disk]);
}

void Ide::initializeIde(wxString ideFile)
{
    if (!wxFile::Exists(ideFile))
    {
        wxString ideInstallationFile;
        ideInstallationFile = ideFile.Left(ideFile.Len()-4) + "-installation.ide";
        
        if (wxFile::Exists(ideInstallationFile))
            wxRenameFile(ideInstallationFile, ideFile, false);
    }
}

void Ide::initIde()
{
    status_ = 0;
    status_[IDE_STAT_RDY] = 1;
    activeStatus_ = 1;
    error_ = 0;
    inter_ = 1;
    sectorCount_ = 1;
    startSector_ = 1;
    cylinder_ = 0;
    headDevice_ = 0xe0;
    dataMode_ = 16;
    command_ = 0;
}

Byte Ide::inIde() 
{
    int drive = (headDevice_ & 16) ? 1 : 0;
    if (driveCreated_[drive])
        return readIdeRegister(registerSelect_);
    else
        return 0;
}

Byte Ide::readIdeStatus()
{
    int drive = (headDevice_ & 16) ? 1 : 0;
    if (driveCreated_[drive])
        return 255;
    else
        return 0;
}

void Ide::outIde(Byte value)
{
    writeIdeRegister(registerSelect_, value);
}

void Ide::selectIdeRegister(Byte value) 
{
    registerSelect_ = value;
}

void Ide::cycleIde() 
{
    if (--ideCycles_ > 0) return;
    if (ideCycles_ < 0) ideCycles_ = 0;
    if (command_ != 0)  onCommand();
}

wxFileOffset Ide::getOffset() 
{
    wxFileOffset ret;
    Byte sec;
    
    int drive = (headDevice_ & 16) ? 1 : 0;
    if (headDevice_ & 64) 
    {
        ret = startSector_;
        ret |= (cylinder_ << 8);
        ret |= ((headDevice_ & 0xf) << 24);
        if (ret >= geometry_[drive].maxLba) 
        {
            error_ |= 4;
            status_[IDE_STAT_ERROR] = 1;
            return -1;
        }
    } 
    else 
    {
        ret = cylinder_;
        if (ret >= geometry_[drive].cylinders) 
        {
            error_ |= 4;
            status_[IDE_STAT_ERROR] = 1;
            return -1;
        }
        sec = startSector_;
        sec--;
        if (sec >= geometry_[drive].sectors) 
        {
            error_ |= 4;
            status_[IDE_STAT_ERROR] = 1;
            return -1;
        }
        ret *= sec;
        sec = headDevice_ & 0xf;
        if (sec >= geometry_[drive].heads) 
        {
            error_ |= 4;
            status_[IDE_STAT_ERROR] = 1;
            return -1;
        }
        ret *= sec;
    }
    ret *= 512;
    return ret;
}

void Ide::writeSector() 
{
    wxFFile diskFile;
    int drive;
    wxFileOffset offset;

    offset = getOffset();
    if (offset < 0) 
    {
        printf("seek error\n");
        return;
    }
    drive = (headDevice_ & 16) ? 1 : 0;
    if (!driveCreated_[drive])
    {
        diskFile.Open(driveName_[drive], "wb");
         diskFile.Close();
        driveCreated_[drive] = true;
    }
    if (!diskFile.Open(driveName_[drive], "rb+"))
    {
        error_ |= 1;
        status_[IDE_STAT_ERROR] = 1;
        return;
    }
    diskFile.Seek(offset, wxFromStart);
    diskFile.Write(sectorBuffer_, 512);
    diskFile.Close();

}

void Ide::readSector() 
{
    wxFFile diskFile;
    int drive;
    wxFileOffset offset;

    offset = getOffset();
    if (offset < 0) 
        return;
    drive = (headDevice_ & 16) ? 1 : 0;
    if (!driveCreated_[drive])
    {
        diskFile.Open(driveName_[drive], "wb+");
         diskFile.Close();
        driveCreated_[drive] = true;
    }
    if (!diskFile.Open(driveName_[drive], "rb+"))
    {
        error_ |= 1;
        status_[IDE_STAT_ERROR] = 1;
        return;
    }
    diskFile.Seek(offset, wxFromStart);
    diskFile.Read(sectorBuffer_, 512);
    diskFile.Close();
}

void Ide::readId()
{
    IDENTIFY_DEVICE_DATA *pIDD = (IDENTIFY_DEVICE_DATA *) &sectorBuffer_;

    pIDD->wGeneralConfiguration = 1<<6;
    pIDD->wNumberOfCylinders = geometry_[0].cylinders;
    pIDD->wSpecificConfiguration = 0;
    pIDD->wNumberOfHeads = geometry_[0].heads;
    pIDD->wUnformattedBytesPerTrack = 0;
    pIDD->wUnformattedBytesPerSector = 0;
    pIDD->wSectorsPerTrack = geometry_[0].sectors;
    for (size_t i=0; i<3; i++)
        pIDD->awATAreserved7[i] = 0;
    for (size_t i=0; i<20; i++)
        pIDD->abSerialNumber[i] = 0;
    pIDD->wBufferType = 0;
    pIDD->wBufferSize = 0;
    pIDD->wECCbytes = 0;
    wxString serialNumber = "            02222025";
    for (size_t i=0; i<20; i++)
    {
        if (i<(serialNumber.Len()))
            pIDD->abSerialNumber[i] = serialNumber.GetChar(i);
    }
    wxString fwRevision = "V0.0.0  ";
    for (size_t i=0; i<8; i++)
    {
        if (i<(fwRevision.Len()))
            pIDD->abFirmwareRevision[i] = fwRevision.GetChar(i);
    }
    int drive = (headDevice_ & 16) ? 1 : 0;
    wxString fileName = p_Main->getFloppyFile(FDCTYPE_TU58_IDE, drive);
    for (size_t i=0; i<40; i++)
        pIDD->abModelNumber[i] = 32;
    for (size_t i=0; i<40; i++)
    {
        if (i<(fileName.Len()))
        {
            if (i&1)
                pIDD->abModelNumber[i-1] = fileName.GetChar(i);
            else
                pIDD->abModelNumber[i+1] = fileName.GetChar(i);
        }
    }
    pIDD->wReadWriteMultiple = 0;
    pIDD->wTrustedComputing = 0;
    pIDD->wCapabilities = 0;
    pIDD->wATAreserved50 = 0;
    pIDD->wPIOtimingMode = 0;
    pIDD->wDMAtimingMode = 0;
    pIDD->wATAreserved53 = 0;
    pIDD->wNumberOfCurrentCylinders = geometry_[drive].cylinders;
    pIDD->wNumberOfCurrentHeads = geometry_[drive].heads;
    pIDD->wCurrentSectorsPerTrack = geometry_[drive].sectors;
    pIDD->lCurrentCapacity = geometry_[drive].maxLba;
    pIDD->wATAreserved59 = 0;
    pIDD->lUserAddressableSectors = geometry_[drive].maxLba;
    pIDD->wSingleWordDMA = 0;
    pIDD->wMultiWordDMA = 0;
    
//    pIDD->awATAreserved64[64]
//    pIDD->awVendorReserved[32]
//    pIDD->awATAreserved160[96]
}

void Ide::setGeometry(int cyl, int hd, int sc) 
{
    geometry_[0].cylinders = cyl;
    geometry_[1].cylinders = cyl;
    geometry_[0].heads = hd;
    geometry_[1].heads = hd;
    geometry_[0].sectors = sc;
    geometry_[1].sectors = sc;
    geometry_[0].maxLba = geometry_[0].cylinders * geometry_[0].heads * geometry_[0].sectors;
    geometry_[1].maxLba = geometry_[1].cylinders * geometry_[1].heads * geometry_[1].sectors;
}

void Ide::writeIdeRegister(int reg, Word value) 
{
    switch(reg) 
    {
        case IDE_CMD_BLK_DATA:
            if (bufferPosition_ >= 512) 
                break;                           
            if (dataMode_ == 16) 
            {
                sectorBuffer_[bufferPosition_++] = value & 0xff;                
                sectorBuffer_[bufferPosition_++] = value >> 8;                
            } 
            else 
            {
                sectorBuffer_[bufferPosition_++] = value & 0xff;                
            }
            if (bufferPosition_ >= 512) 
                status_[IDE_STAT_DRQ] = 0;
        break;

        case IDE_CMD_BLK_FEATURES:
            switch(value) 
            {
                case 0x01:dataMode_ = 8; break;
                case 0x81:dataMode_ = 16; break;
            }
        break;

        case IDE_CMD_BLK_SECTOR_COUNT:
            sectorCount_ = value;                                        
        break;

        case IDE_CMD_BLK_SECTOR_NUMBER:
            startSector_ = value; 
        break;

        case IDE_CMD_BLK_CYLINDER_LOW:
            cylinder_ = (cylinder_ & 0xff00) | (value & 0xff);
        break;

        case IDE_CMD_BLK_CYLINDER_HIGH:
            cylinder_ = (cylinder_ & 0xff) | ((value & 0xff) << 8);
        break;

        case IDE_CMD_BLK_DEVICE_HEAD:
            headDevice_ = value;
        break;

        case IDE_CMD_BLK_COMMAND:
            if (!status_[IDE_STAT_BSY])
            {                                       
                switch(value) 
                {
                    case 0x20:
                        command_ = IDE_CMD_READ;
                        ideCycles_ = 100;
                        status_[IDE_STAT_BSY] = 1;
                    break;

                    case 0x30:
                        command_ = IDE_CMD_WRITE;
                        ideCycles_ = 100;
                        status_[IDE_STAT_BSY] = 1;
                    break;

                    case 0xec:
                        command_ = IDE_CMD_ID;
                        ideCycles_ = 100;
                        status_[IDE_STAT_BSY] = 1;
                    break;

                    case 0xef:
                        command_ = IDE_CMD_SETF;
                        ideCycles_ = 100;
                        status_[IDE_STAT_BSY] = 1;
                    break;
                }
            }
        break;

        case IDE_CMD_BLK_DEVICE_CONTROL:
            inter_ = value;  // set interrupt
            if (value & 4)
            {
                command_ = IDE_CMD_RESET;
                ideCycles_ = 1000;
                status_[IDE_STAT_BSY] = 1;
            }
        break;

        default:    // unknown
        break;
    }
    if (status_[IDE_STAT_BSY])
        p_Computer->showStatusLed(DISKLED, 1);
    else
        p_Computer->showStatusLed(DISKLED, 0);
}

Word Ide::readIdeRegister(int reg) 
{
    Word ret = 0;
    switch(reg) 
    {
        case 0x00:
            if (dataMode_ == 16) 
            {                                     /* Data */
                ret = sectorBuffer_[bufferPosition_++];
                ret |= (sectorBuffer_[bufferPosition_++] << 8);
            } 
            else 
            {
                ret = sectorBuffer_[bufferPosition_++];
            }
            if (bufferPosition_ >= 512) 
                status_[IDE_STAT_DRQ] = 0;
        break;

        case 0x01:
            ret = error_;                /* Error */
        break;

        case 0x02:
            ret = sectorCount_;            /* sector count */
        break;

        case 0x03:
            ret = startSector_;            /* start sector */
        break;

        case 0x04:
            ret = cylinder_ & 0xff;        /* cylinder lo */
        break;

        case 0x05:
            ret = cylinder_ >> 8;        /* cylinder hi */
        break;

        case 0x06:
            ret = headDevice_;            /* head/device */
        break;

        case 0x07:
            ret = status_;                /* status */
        break;

        case 0x0e:
            ret = status_;                /* status */
        break;

        case 0x0f:
            ret = activeStatus_;            /* active status */
        break;
    }
    return ret;
}

void Ide::onCommand() 
{
    status_[IDE_STAT_ERROR] = 0;
    switch(command_) 
    {
            case IDE_CMD_ID:
                command_ = IDE_CMD_READ_1;
                readId();
                bufferPosition_ = 0;
                status_[IDE_STAT_DRQ] = 1;
                status_[IDE_STAT_BSY] = 0;
            break;

            case IDE_CMD_READ:
                command_ = IDE_CMD_READ_1;
                readSector();
                bufferPosition_ = 0;
                status_[IDE_STAT_DRQ] = 1;
                status_[IDE_STAT_BSY] = 0;
            break;

            case IDE_CMD_READ_1:
                if (bufferPosition_ >= 512) 
                {
                    status_[IDE_STAT_DRQ] = 0;
                    command_ = 0;
                }
            break;

            case IDE_CMD_WRITE:
                command_ = IDE_CMD_WRITE_1;
                bufferPosition_ = 0;
                status_[IDE_STAT_DRQ] = 1;
                status_[IDE_STAT_BSY] = 0;
            break;

            case IDE_CMD_WRITE_1:
                if (bufferPosition_ >= 512) 
                {
                    error_ = 0;
                    writeSector();
                    command_ = 0;
                }
            break;

            case IDE_CMD_RESET:
                command_ = 0;
                status_ = 0;
                status_[IDE_STAT_RDY] = 1;
                initIde(); 
            break;
 
            case IDE_CMD_SETF:
                command_ = 0;
                status_ = 0;
                status_[IDE_STAT_RDY] = 1;
            break;
     }
    if (status_[IDE_STAT_BSY])
        p_Computer->showStatusLed(DISKLED, 1);
    else
        p_Computer->showStatusLed(DISKLED, 0);
}
