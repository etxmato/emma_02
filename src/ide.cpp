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

enum {
    IDE_DH_HS0,
    IDE_DH_HS1,
    IDE_DH_HS2,
    IDE_DH_HS3,
    IDE_DH_DEV,
    IDE_DH_BIT5_RESERVED,
    IDE_DH_SECTOR_ADDRESS_MODE,
    IDE_DH_BIT7_RESERVED,
};

enum {
    IDE_CMD_READ_WITH_RETRY = 0x20,
    IDE_CMD_READ_WO_RETRY = 0x21,
    IDE_CMD_WRITE_WITH_RETRY = 0x30,
    IDE_CMD_WRITE_WO_RETRY = 0x31,
    IDE_CMD_RESET_BOOT = 0xdc,
    IDE_CMD_IDEMTIFY_DEVICE = 0xEC,
    IDE_CMD_SET_FEATURES = 0xEF
};

enum {
    IDE_CMD_BLK_DATA = 0,
    IDE_CMD_BLK_FEATURES = 1,
    IDE_CMD_BLK_SECTOR_COUNT = 2,
    IDE_CMD_BLK_SECTOR_NUMBER = 3,
    IDE_CMD_BLK_CYLINDER_LOW = 4,
    IDE_CMD_BLK_CYLINDER_HIGH = 5,
    IDE_CMD_BLK_DEVICE_HEAD = 6,
    IDE_CMD_BLK_COMMAND = 7,
    IDE_CONTROL_BLK_DEVICE_CONTROL= 0xE
};

enum {
    IDE_CMD_BLK_ERROR = 1,
    IDE_CMD_BLK_STATUS = 7,
    IDE_CONTROL_BLK_ALT_STATUS = 0xE,
    IDE_CONTROL_BLK_ACTIVE_STATUS = 0xF
};

enum {
    IDE_DC_NIEN = 1,
    IDE_DC_SRST = 2
};

enum {
    IDE_ERROR_AMNF,
    IDE_ERROR_TKNONF,
    IDE_ERROR_ABRT,
    IDE_ERROR_MCR,
    IDE_ERROR_IDNF,
    IDE_ERROR_MC,
    IDE_ERROR_UNC
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
    statusRegister_ = 0;
    statusRegister_[IDE_STAT_RDY] = 1;
    activeStatus_ = 1;
    errorRegister_ = 0;
    deviceControlRegister_ = 1;
    sectorCount_ = 1;
    startSector_ = 1;
    cylinder_ = 0;
    deviceHeadRegister_ = 0xe0;
    dataMode_ = 16;
    command_ = 0;
}

Byte Ide::inIde() 
{
    if (driveCreated_[deviceHeadRegister_[IDE_DH_DEV]])
        return readIdeRegister(registerSelect_);
    else
        return 0;
}

Byte Ide::readIdeStatus()
{
    if (driveCreated_[deviceHeadRegister_[IDE_DH_DEV]])
        return statusRegister_.to_ulong();
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
    
    if (deviceHeadRegister_[IDE_DH_SECTOR_ADDRESS_MODE])
    {
        ret = startSector_;
        ret |= (cylinder_ << 8);
        ret |= ((deviceHeadRegister_.to_ulong() & 0xf) << 24);
        if (ret >= geometry_[deviceHeadRegister_[IDE_DH_DEV]].maxLba)
        {
            errorRegister_[IDE_ERROR_ABRT] = 1;
            statusRegister_[IDE_STAT_ERROR] = 1;
            return -1;
        }
    } 
    else 
    {
        ret = cylinder_;
        if (ret >= geometry_[deviceHeadRegister_[IDE_DH_DEV]].cylinders)
        {
            errorRegister_[IDE_ERROR_ABRT] = 1;
            statusRegister_[IDE_STAT_ERROR] = 1;
            return -1;
        }
        sec = startSector_;
        sec--;
        if (sec >= geometry_[deviceHeadRegister_[IDE_DH_DEV]].sectors)
        {
            errorRegister_[IDE_ERROR_ABRT] = 1;
            statusRegister_[IDE_STAT_ERROR] = 1;
            return -1;
        }
        ret *= sec;
        sec = deviceHeadRegister_.to_ulong() & 0xf;
        if (sec >= geometry_[deviceHeadRegister_[IDE_DH_DEV]].heads)
        {
            errorRegister_[IDE_ERROR_ABRT] = 1;
            statusRegister_[IDE_STAT_ERROR] = 1;
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
    wxFileOffset offset;

    offset = getOffset();
    if (offset < 0) 
    {
        printf("seek error\n");
        return;
    }
    if (!driveCreated_[deviceHeadRegister_[IDE_DH_DEV]])
    {
        diskFile.Open(driveName_[deviceHeadRegister_[IDE_DH_DEV]], "wb");
         diskFile.Close();
        driveCreated_[deviceHeadRegister_[IDE_DH_DEV]] = true;
    }
    if (!diskFile.Open(driveName_[deviceHeadRegister_[IDE_DH_DEV]], "rb+"))
    {
        errorRegister_[IDE_ERROR_TKNONF] = 1;
        statusRegister_[IDE_STAT_ERROR] = 1;
        return;
    }
    diskFile.Seek(offset, wxFromStart);
    diskFile.Write(sectorBuffer_, 512);
    diskFile.Close();

}

void Ide::readSector() 
{
    wxFFile diskFile;
    wxFileOffset offset;

    offset = getOffset();
    if (offset < 0) 
        return;
    if (!driveCreated_[deviceHeadRegister_[IDE_DH_DEV]])
    {
        diskFile.Open(driveName_[deviceHeadRegister_[IDE_DH_DEV]], "wb+");
        diskFile.Close();
        driveCreated_[deviceHeadRegister_[IDE_DH_DEV]] = true;
    }
    if (!diskFile.Open(driveName_[deviceHeadRegister_[IDE_DH_DEV]], "rb+"))
    {
        errorRegister_[IDE_ERROR_TKNONF] = 1;
        statusRegister_[IDE_STAT_ERROR] = 1;
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
    wxString fileName = p_Main->getFloppyFile(FDCTYPE_TU58_IDE, deviceHeadRegister_[IDE_DH_DEV]);
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
    pIDD->wNumberOfCurrentCylinders = geometry_[deviceHeadRegister_[IDE_DH_DEV]].cylinders;
    pIDD->wNumberOfCurrentHeads = geometry_[deviceHeadRegister_[IDE_DH_DEV]].heads;
    pIDD->wCurrentSectorsPerTrack = geometry_[deviceHeadRegister_[IDE_DH_DEV]].sectors;
    pIDD->lCurrentCapacity = geometry_[deviceHeadRegister_[IDE_DH_DEV]].maxLba;
    pIDD->wATAreserved59 = 0;
    pIDD->lUserAddressableSectors = geometry_[deviceHeadRegister_[IDE_DH_DEV]].maxLba;
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
                statusRegister_[IDE_STAT_DRQ] = 0;
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
            deviceHeadRegister_ = value;
        break;

        case IDE_CMD_BLK_COMMAND:
            if (!statusRegister_[IDE_STAT_BSY])
            {                                       
                switch(value) 
                {
                    case IDE_CMD_READ_WITH_RETRY:
                    case IDE_CMD_READ_WO_RETRY:
                        command_ = value;
                        ideCycles_ = 100;
                        statusRegister_[IDE_STAT_BSY] = 1;
                    break;

                    case IDE_CMD_WRITE_WITH_RETRY:
                    case IDE_CMD_WRITE_WO_RETRY:
                        command_ = value;
                        ideCycles_ = 100;
                        statusRegister_[IDE_STAT_BSY] = 1;
                    break;

                    case IDE_CMD_IDEMTIFY_DEVICE:
                        command_ = value;
                        ideCycles_ = 100;
                        statusRegister_[IDE_STAT_BSY] = 1;
                    break;

                    case IDE_CMD_SET_FEATURES:
                        command_ = value;
                        ideCycles_ = 100;
                        statusRegister_[IDE_STAT_BSY] = 1;
                    break;
                }
            }
        break;

        case IDE_CONTROL_BLK_DEVICE_CONTROL:
            deviceControlRegister_ = value;
            if (deviceControlRegister_[IDE_DC_SRST])
            {
                command_ = IDE_CMD_RESET_BOOT;
                ideCycles_ = 1000;
                statusRegister_[IDE_STAT_BSY] = 1;
            }
        break;

        default:    // unknown
        break;
    }
    p_Computer->showStatusLed(DISKLED, statusRegister_[IDE_STAT_BSY]);
}

Word Ide::readIdeRegister(int reg) 
{
    Word ret = 0;
    switch(reg) 
    {
        case IDE_CMD_BLK_DATA:
            if (dataMode_ == 16)
            {
                ret = sectorBuffer_[bufferPosition_++];
                ret |= (sectorBuffer_[bufferPosition_++] << 8);
            } 
            else 
            {
                ret = sectorBuffer_[bufferPosition_++];
            }
            if (bufferPosition_ >= 512) 
                statusRegister_[IDE_STAT_DRQ] = 0;
        break;

        case IDE_CMD_BLK_ERROR:
            ret = errorRegister_.to_ulong();
        break;

        case IDE_CMD_BLK_SECTOR_COUNT:
            ret = sectorCount_;
        break;

        case IDE_CMD_BLK_SECTOR_NUMBER:
            ret = startSector_;
        break;

        case IDE_CMD_BLK_CYLINDER_LOW:
            ret = cylinder_ & 0xff;
        break;

        case IDE_CMD_BLK_CYLINDER_HIGH:
            ret = cylinder_ >> 8;
        break;

        case IDE_CMD_BLK_DEVICE_HEAD:
            ret = deviceHeadRegister_.to_ulong();
        break;

        case IDE_CMD_BLK_STATUS:
        case IDE_CONTROL_BLK_ALT_STATUS:
            ret = statusRegister_.to_ulong();
        break;

        case IDE_CONTROL_BLK_ACTIVE_STATUS:
            ret = activeStatus_;
        break;
    }
    return ret;
}

void Ide::onCommand() 
{
    statusRegister_[IDE_STAT_ERROR] = 0;
    switch(command_) 
    {
            case IDE_CMD_IDEMTIFY_DEVICE:
                command_ = IDE_CMD_READ_WO_RETRY;
                readId();
                bufferPosition_ = 0;
                statusRegister_[IDE_STAT_DRQ] = 1;
                statusRegister_[IDE_STAT_BSY] = 0;
            break;

            case IDE_CMD_READ_WITH_RETRY:
                command_ = IDE_CMD_READ_WO_RETRY;
                readSector();
                bufferPosition_ = 0;
                statusRegister_[IDE_STAT_DRQ] = 1;
                statusRegister_[IDE_STAT_BSY] = 0;
            break;

            case IDE_CMD_READ_WO_RETRY:
                if (bufferPosition_ >= 512) 
                {
                    statusRegister_[IDE_STAT_DRQ] = 0;
                    command_ = 0;
                }
            break;

            case IDE_CMD_WRITE_WITH_RETRY:
                command_ = IDE_CMD_WRITE_WO_RETRY;
                bufferPosition_ = 0;
                statusRegister_[IDE_STAT_DRQ] = 1;
                statusRegister_[IDE_STAT_BSY] = 0;
            break;

            case IDE_CMD_WRITE_WO_RETRY:
                if (bufferPosition_ >= 512) 
                {
                    errorRegister_ = 0;
                    writeSector();
                    command_ = 0;
                }
            break;

            case IDE_CMD_RESET_BOOT:
                command_ = 0;
                initIde();
            break;
 
            case IDE_CMD_SET_FEATURES:
                command_ = 0;
                statusRegister_ = 0;
                statusRegister_[IDE_STAT_RDY] = 1;
            break;
     }
     p_Computer->showStatusLed(DISKLED, statusRegister_[IDE_STAT_BSY]);
}
