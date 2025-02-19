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

#define IDE_STAT_ERROR       1
#define IDE_STAT_INDEX       2
#define IDE_STAT_ECC         4
#define IDE_STAT_DRQ         8
#define IDE_STAT_SKC        16
#define IDE_STAT_WFT        32
#define IDE_STAT_RDY        64
#define IDE_STAT_BSY       128

#define IDE_CMD_RESET        1
#define IDE_CMD_SETF         2
#define IDE_CMD_WRITE        3
#define IDE_CMD_WRITE_1      4
#define IDE_CMD_READ         5
#define IDE_CMD_READ_1       6
#define IDE_CMD_ID           7

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
    status_ = 0x40;
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
    return readIdeRegister(registerSelect_);
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
            status_ |= 1;
            return -1;
        }
    } 
    else 
    {
        ret = cylinder_;
        if (ret >= geometry_[drive].cylinders) 
        {
            error_ |= 4;
            status_ |= 1;
            return -1;
        }
        sec = startSector_;
        sec--;
        if (sec >= geometry_[drive].sectors) 
        {
            error_ |= 4;
            status_ |= 1;
            return -1;
        }
        ret *= sec;
        sec = headDevice_ & 0xf;
        if (sec >= geometry_[drive].heads) 
        {
            error_ |= 4;
            status_ |= 1;
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
        status_ |= 1;
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
        status_ |= 1;
        return;
    }
    diskFile.Seek(offset, wxFromStart);
    diskFile.Read(sectorBuffer_, 512);
    diskFile.Close();
}

void Ide::readId() 
{
    for (int i=0; i<512; i++) 
        sectorBuffer_[i] = 0;
    sectorBuffer_[2] = geometry_[0].cylinders % 256;
    sectorBuffer_[3] = geometry_[0].cylinders / 256;
    sectorBuffer_[6] = geometry_[0].heads % 256;
    sectorBuffer_[7] = geometry_[0].heads / 256;
    sectorBuffer_[12] = geometry_[0].sectors % 256;
    sectorBuffer_[13] = geometry_[0].sectors / 256;
    sectorBuffer_[108] = geometry_[0].cylinders % 256;
    sectorBuffer_[109] = geometry_[0].cylinders / 256;
    sectorBuffer_[110] = geometry_[0].heads % 256;
    sectorBuffer_[111] = geometry_[0].heads / 256;
    sectorBuffer_[112] = geometry_[0].sectors % 256;
    sectorBuffer_[113] = geometry_[0].sectors / 256;
    sectorBuffer_[114] = geometry_[0].maxLba & 255;
    sectorBuffer_[115] = (geometry_[0].maxLba >> 8) & 255;
    sectorBuffer_[116] = (geometry_[0].maxLba >> 16) & 255;
    sectorBuffer_[117] = (geometry_[0].maxLba >> 24) & 255;
    sectorBuffer_[120] = geometry_[0].maxLba & 255;
    sectorBuffer_[121] = (geometry_[0].maxLba >> 8) & 255;
    sectorBuffer_[122] = (geometry_[0].maxLba >> 16) & 255;
    sectorBuffer_[123] = (geometry_[0].maxLba >> 24) & 255;
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
        case 0x00:
            if (bufferPosition_ >= 512) 
                break;                             /* Data */
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
                status_ &= (~IDE_STAT_DRQ);
        break;

        case 0x01:
            features_ = value;                                            /* precomp */
        break;

        case 0x02:
            sectorCount_ = value;                                            /* sector count */
        break;

        case 0x03:
            startSector_ = value;    
                                        /* start sector */
        case 0x04:
            cylinder_ = (cylinder_ & 0xff00) | (value & 0xff); /* cylinder lo */
        break;

        case 0x05:
            cylinder_ = (cylinder_ & 0xff) | ((value & 0xff) << 8);     /* cylinder hi */
        break;

        case 0x06:
            headDevice_ = value;
                                             /* head/device */
        case 0x07:
            if (!(status_ & 128)) 
            {                                        /* command */
                switch(value) 
                {
                    case 0x20:
                        command_ = IDE_CMD_READ;
                        ideCycles_ = 100;
                        status_ = IDE_STAT_RDY | IDE_STAT_BSY;
                    break;

                    case 0x30:
                        command_ = IDE_CMD_WRITE;
                        ideCycles_ = 100;
                        status_ = IDE_STAT_RDY | IDE_STAT_BSY;
                    break;

                    case 0xec:
                        command_ = IDE_CMD_ID;
                        ideCycles_ = 100;
                        status_ = IDE_STAT_RDY | IDE_STAT_BSY;
                    break;

                    case 0xef:
                        command_ = IDE_CMD_SETF;
                        ideCycles_ = 100;
                        status_ = IDE_STAT_RDY | IDE_STAT_BSY;
                    break;
                }
            }
        break;

        case 0x0e:
            inter_ = value;                                                     /* set inter */
            if (value & 2) 
            {
                command_ = IDE_CMD_RESET;
                ideCycles_ = 1000;
                status_ = IDE_STAT_RDY | IDE_STAT_BSY;
            }
        break;

        case 0x0f:
        break;            /* unknown */
    }
    if ((status_ & IDE_STAT_BSY) == IDE_STAT_BSY)
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
                status_ &= (~IDE_STAT_DRQ);
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
    status_ &= 0xfe;
    switch(command_) 
    {
            case IDE_CMD_ID:
                command_ = IDE_CMD_READ_1;
                readId();
                bufferPosition_ = 0;
                status_ |= IDE_STAT_DRQ;
                status_ &= (~IDE_STAT_BSY);
            break;

            case IDE_CMD_READ:
                command_ = IDE_CMD_READ_1;
                readSector();
                bufferPosition_ = 0;
                status_ |= IDE_STAT_DRQ;
                status_ &= (~IDE_STAT_BSY);
            break;

            case IDE_CMD_READ_1:
                if (bufferPosition_ >= 512) 
                {
                    status_ &= (~IDE_STAT_DRQ);
                    command_ = 0;
                }
            break;

            case IDE_CMD_WRITE:
                command_ = IDE_CMD_WRITE_1;
                bufferPosition_ = 0;
                status_ |= IDE_STAT_DRQ;
                status_ &= (~IDE_STAT_BSY);
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
                status_ = IDE_STAT_RDY;
                initIde(); 
            break;
 
            case IDE_CMD_SETF:
                switch(features_) 
                {
                            case 0x01:dataMode_ = 8; break;
                            case 0x81:dataMode_ = 16; break;
                    }
                command_ = 0;
                status_ = IDE_STAT_RDY;
            break;
     }
    if ((status_ & IDE_STAT_BSY) == IDE_STAT_BSY)
        p_Computer->showStatusLed(DISKLED, 1);
    else
        p_Computer->showStatusLed(DISKLED, 0);
}
