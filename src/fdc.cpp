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
#include "main.h"
#include "fdc.h"

enum
{
    FDC1770,
    FDC1793,
};

Fdc::Fdc()
{
}

void Fdc::configure1793(FdcConfiguration fdcConfiguration)
{
    wxFFile diskFile;

    fdcConfiguration_ = fdcConfiguration;
    fdc17xxType_ = FDC1793;

    p_Main->configureMessage(&fdcConfiguration.ioGroupVector, "WD1793 Floppy Disk Controller");
    p_Computer->setInType(&fdcConfiguration.ioGroupVector, fdcConfiguration.readInput, FDC1793_READ_IN, "read register");
    p_Computer->setOutType(&fdcConfiguration.ioGroupVector, fdcConfiguration.selectOutput, FDC1793_SELECT_OUT, "register select");
    p_Computer->setOutType(&fdcConfiguration.ioGroupVector, fdcConfiguration.writeOutput, FDC1793_WRITE_OUT, "write register");
    p_Computer->setEfType(&fdcConfiguration.ioGroupVector, fdcConfiguration.ef, FDC1793_EF, "DRQ");
    p_Computer->setCycleType(CYCLE_TYPE_DISK_FDC, FDC_CYCLE);

    fdcRegisterSelect_ = 0;

    for (int i=0; i<4; i++)
        diskCreated_[i] = wxFile::Exists(diskName_[i]);
    for (int i=0; i<4; i++)
    {
        numberOfSides_[i] = fdcConfiguration.heads;
        numberOfTracks_[i] = fdcConfiguration.tracks;
    }
    
    numberOfSectors_ = fdcConfiguration.sectors;
    sectorLength_ = fdcConfiguration.sectorLength;
    maxFmtCount_ = fdcConfiguration.maxFmtCount;

    wxString printBuffer;
    printBuffer.Printf("	Disk geometry: %d side, %d tracks, %d sectors per track and %d bytes per sector\n", fdcConfiguration.heads, fdcConfiguration.tracks, fdcConfiguration.sectors, fdcConfiguration.sectorLength);
    p_Main->message(printBuffer);
}

Byte Fdc::ef1793() 
{
    return drq_^fdcConfiguration_.ef.reverse;
}

Byte Fdc::in1793() 
{
    switch(fdcRegisterSelect_&0x3) 
    {
        case 0:
            intrq_ = 0;
            return status_;

        case 1:
            return track_;

        case 2:
            return sector_;

        case 3:
            return readData();
    }
    return 0;
}

void Fdc::selectRegister1793(Byte value) 
{
    fdcRegisterSelect_ = value;
}

void Fdc::writeRegister1793(Byte value) 
{
    switch(fdcRegisterSelect_&0x3)
    {
        case 0:
            intrq_ = 0;
            onCommand(value); 
        break;

        case 1:
            track_ = value; 
        break;

        case 2:
            sector_ = value; 
        break;

        case 3:
            writeData(value); 
        break;

        case 4:
            drive_ = convertDriveNumber(value); 
        break;
    }
}

void Fdc::configure1770(FdcConfiguration fdcConfiguration)
{
    fdc17xxType_ = FDC1770;
    fdcConfiguration_ = fdcConfiguration;
    fdcRegisterSelect_ = 0;

    for (int i=0; i<4; i++)
    {
        diskCreated_[i] = false;
        numberOfSides_[i] = fdcConfiguration.heads;
        numberOfTracks_[i] = fdcConfiguration.tracks;
    }

    numberOfTracksPerSide_ = fdcConfiguration.tracks;
    numberOfSectors_ = fdcConfiguration.sectors;
    sectorLength_ = fdcConfiguration.sectorLength;
    maxFmtCount_ = fdcConfiguration.maxFmtCount;
    
    p_Main->configureMessage(&fdcConfiguration.ioGroupVector, "WD1770 Floppy Disk Controller");
    p_Computer->setOutType(&fdcConfiguration.ioGroupVector, fdcConfiguration.selectOutput, FDC1770_SELECT_OUT, "register select");
    p_Computer->setOutType(&fdcConfiguration.ioGroupVector, fdcConfiguration.writeOutput, FDC1770_WRITE_OUT, "write register");
    p_Computer->setInType(&fdcConfiguration.ioGroupVector, fdcConfiguration.intrqInput, FDC1770_INTRQ_IN, "INTRQ");
    p_Computer->setInType(&fdcConfiguration.ioGroupVector, fdcConfiguration.readInput, FDC1770_READ_IN, "read register");
    p_Computer->setEfType(&fdcConfiguration.ioGroupVector, fdcConfiguration.ef, FDC1770_EF, "DRQ");
    p_Computer->setCycleType(CYCLE_TYPE_DISK_FDC, FDC_CYCLE);

    wxString printBuffer;
    printBuffer.Printf("	Disk geometry: %d side, %d tracks, %d sectors per track and %d bytes per sector\n", fdcConfiguration.heads, fdcConfiguration.tracks, fdcConfiguration.sectors, fdcConfiguration.sectorLength);
    p_Main->message(printBuffer);
}

Byte Fdc::ef1770() 
{
    return drq_^fdcConfiguration_.ef.reverse;
}

Byte Fdc::in1770()
{
    if (p_Computer->getFlipFlopQ())
        return intrqStatus1770();
    else
        return readRegister1770();
}

Byte Fdc::readRegister1770() 
{
    switch(fdcRegisterSelect_&0x3) 
    {
        case 0:
            intrq_ = 0;
            return status_;

        case 1:
            return track_;

        case 2:
            return sector_;

        case 3:
            return readData();
    }
    return 0;
}

Byte Fdc::intrqStatus1770() 
{
    return intrq_;
}

void Fdc::out1770(Byte value)
{
    if (p_Computer->getFlipFlopQ())
        selectRegister1770(value);
    else
        writeRegister1770(value);
}

void Fdc::writeRegister1770(Byte value) 
{
    switch(fdcRegisterSelect_&0x3)
    {
        case 0:
            intrq_ = 0;
            onCommand(value); 
        break;

        case 1:
            track_ = value; 
        break;

        case 2:
            sector_ = value; 
        break;

        case 3:
            writeData(value); 
        break;
    }
}

void Fdc::selectRegister1770(Byte value) 
{
    if ((value & 0x10) == 0x10)
    {
        if ((value & 0x04) == 0x04)
            drive_ = 0;

        if ((value & 0x08) == 0x08)
            drive_ = 1;

        side_ = (value & 0x20) >> 5;
    }
    fdcRegisterSelect_ = value;
}

void Fdc::cycleFdc()
{
    Byte newStatus;

    if (exec_ == 0xff) return;
    if (--fdcCycles_ > 0) return;
    if (fdcCycles_ < 0) fdcCycles_ = 0;

    switch(exec_ & 0xf0) 
    {
        case 0:                                        /* restore */
            if (driveTrack_[drive_] == 0) 
            {
                track_ = 0;
                endCommand(4);
                return;
            }
            stepDirection_ = 'O';
            driveTrack_[drive_]--;
            fdcCycles_ = p_Main->setFdcStepRate(exec_ & 3);
            return;
        break;

        case 0x10:                                     /* seek */
            if (track_ == data_) 
            {
                newStatus = 0;
                if (driveTrack_[drive_] == 0)
                    newStatus |= 4;
                endCommand(newStatus);
                return;
            }
            fdcCycles_ = p_Main->setFdcStepRate(exec_ & 3);
            if (track_ < data_) 
            {
                track_++;
                stepDirection_ = 'I';
                driveTrack_[drive_]++;
                if (driveTrack_[drive_] > numberOfTracks_[drive_]) 
                    driveTrack_[drive_] = numberOfTracks_[drive_];
            } 
            else 
            { 
                stepDirection_ = 'O';
                track_--;
                driveTrack_[drive_]--;
                if (driveTrack_[drive_] < 0) 
                    driveTrack_[drive_] = 0;
            }
            return;
        break;

        case 0x20:                                     /* step, no update */
        case 0x30:                                     /* step, with update */
            if (stepDirection_ == 'I') 
            {
                driveTrack_[drive_]++;
                if (driveTrack_[drive_] > numberOfTracks_[drive_]) 
                    driveTrack_[drive_] = numberOfTracks_[drive_];
                if ((exec_ & 0xf0) == 0x30) 
                    track_++;
                if (track_ > 254) 
                    track_ = 254;
            } 
            else 
            {
                driveTrack_[drive_]--;
                if (driveTrack_[drive_] < 0) 
                    driveTrack_[drive_]=0;
                if ((exec_ & 0xf0) == 0x30) 
                    track_--;
                if (track_ < 0) 
                    track_ = 0;
            }
            newStatus = 0;
            if (driveTrack_[drive_] == 0) 
                newStatus |= 4;
            endCommand(newStatus);
            return;
        break;

        case 0x40:                                     /* step in, no update */
        case 0x50:                                     /* step in, with update */
            driveTrack_[drive_]++;
            if (driveTrack_[drive_] > numberOfTracks_[drive_]) 
                driveTrack_[drive_]=numberOfTracks_[drive_];
            if ((exec_ & 0xf0) == 0x50) 
                track_++;
            if (track_ > 254) 
                track_ = 254;
            newStatus = 0;
            if (driveTrack_[drive_] == 0) 
                newStatus += 4;
            endCommand(newStatus);
            return;
        break;

        case 0x60:                                     /* step out, no update */
        case 0x70:                                     /* step out, with update */
            driveTrack_[drive_]--;
            if (driveTrack_[drive_] < 0) 
                driveTrack_[drive_] = 0;
            if ((exec_ & 0xf0) == 0x70) 
                track_--;
            if (track_ < 0) 
                track_ = 0;
            newStatus = 0;
            if (driveTrack_[drive_] == 0) 
                newStatus |= 4;
            endCommand(newStatus);
            return;
        break;
    }
    switch(exec_) 
    {
        case 0x80:                                    /* initial portion of read */
            data_ = *(sectorBufferPointer++);

            exec_ = 0x81;
            drq_ = 1;
            status_ |= 0x02;
            fdcCycles_ = 90;
        break;

        case 0x81:                                    /* main body of read */
            if (drq_) 
            {
                status_ |= 0x04;
                p_Main->eventShowTextMessage("Computer: Missed FDC data");
            }
            fdcCycles_ = 90;
            if (sectorBufferPointer == &sectorBuffer_[sectorLength_]) 
                exec_ = 0x82;
            else 
            {
                data_ = *(sectorBufferPointer++);

                drq_ = 1;
                status_ |= 0x02;
             }
        break;

        case 0x82:                                    /* clean up read */
            if (multipleSector_ &&(sector_ <(numberOfSectors_-1)))
            {
                sector_++;
                exec_ = 0x80;
                fdcCycles_ = p_Main->setFdcStepRate(exec_ & 3);
                drq_ = 0;
                sectorBufferPointer = sectorBuffer_;
                status_ |= 1;
                updateFdcStatusLed();
                readSector();
            }
            else
            {
                drq_ = 0;
                endCommand(status_ & 0xfe);
            }
        break;

        case 0xa0:                                    /* initial portion of write */
            exec_=0xa1;
            drq_ = 1;
            status_ |=0x02;
            fdcCycles_ = 90;
        break;

        case 0xa1:                                    /* main body of write */
            if (drq_) 
            {
                status_ |= 0x04;
                p_Main->eventShowTextMessage("FDC: Missed data");
            }
            fdcCycles_ = 90;
            *(sectorBufferPointer++) = data_;
            if (sectorBufferPointer == &sectorBuffer_[sectorLength_]) 
                exec_ = 0xa2;
            else 
            {
                 drq_ = 1;
                 status_ |=0x02;
             }
        break;

        case 0xa2:                                    /* clean up write */
            writeSector();
            if (multipleSector_ &&(sector_ <(numberOfSectors_-1)))
            {
                sector_++;
                exec_ = 0xa0;
                fdcCycles_ = p_Main->setFdcStepRate(exec_ & 3);
                drq_ = 0;
                sectorBufferPointer = sectorBuffer_;
                status_ |= 1;
                updateFdcStatusLed();
            }
            else
            {
                drq_ = 0;
                endCommand(status_ & 0xfe);
            }
        break;

        case 0xc0:                                    /* read address*/
            if (drq_) 
            {
                status_ |= 0x04;
                p_Main->eventShowTextMessage("Computer: Missed FDC data");
            }
            data_ = *(sectorBufferPointer++);
            drq_ = 1;
            status_ |= 0x02;
            if (sectorBufferPointer == &sectorBuffer_[6])
            {
                drq_ = 0;
                endCommand(status_ & 0xfe);
            }
            else
                fdcCycles_ = 90;
        break;
    }
}

void Fdc::resetFdc() 
{
    intrq_ = 0;
    track_ = 0;
    sector_ = 0;
    side_ = 0;
    drive_ = 0;
    status_ = 4; 
    updateFdcStatusLed();
    command_ = 0xff;
    exec_ = 0xff;
    stepDirection_ = 'I';
    fdcCycles_ = 0;
    for (int i=0; i<4; i++) 
        driveTrack_[i] = 0;
}

void Fdc::setFdcDiskname(int disk, wxString fileName)
{
    wxFFile diskFile;
    size_t bytes;

    diskName_[disk-1] = fileName;
    diskCreated_[disk-1] = wxFile::Exists(diskName_[disk-1]);
    if (diskCreated_[disk-1])
    {
        diskFile.Open(diskName_[disk-1], _("rb"));
        diskFile.Seek(0, wxFromStart);
        bytes = diskFile.Read(sectorBuffer_, sectorLength_);
        diskFile.Close();

        if (bytes < sectorLength_)
        {
            p_Main->message("1. Error reading disk image");
            return;
        }

        numberOfTracks_[disk-1] = numberOfTracksPerSide_ +(sectorBuffer_[0x12] * numberOfTracksPerSide_);
        numberOfSides_[disk-1] = 1 + sectorBuffer_[0x13];
        if ((sectorBuffer_[0x12] == 0) &&(sectorBuffer_[0x13] == 0) &&((sectorBuffer_[0x70] == 0xff)))
            numberOfSides_[disk-1] = 2;
    }
    else
        diskName_[disk-1] = "";
}

int Fdc::convertDriveNumber(int drive) 
{
    switch(drive) 
    {
        case 1: return 0;
        case 2: return 1;
        case 4: return 2;
        default: return 3;
    }
}

void Fdc::readSector() 
{
    wxFFile diskFile;
    wxFileOffset offset;
    size_t bytes;

    if (!diskCreated_[drive_])
    {
        diskFile.Open(diskName_[drive_], "wb+");
         diskFile.Close();
        diskCreated_[drive_] = true;
    }
    if (!diskFile.Open(diskName_[drive_], "rb+"))
    {
        endCommand(0x80);
        return;
    }
    if (sector_ >= numberOfSectors_)
    {
        p_Main->message("Unknown Sector");
        endCommand(0x10);
        return;
    }
    if (track_ >= numberOfTracks_[drive_])
    {
        p_Main->message("Unknown Track");
        endCommand(0x10);
        return;
    }

    offset = (track_ * numberOfSides_[drive_]) + side_;
    offset *= numberOfSectors_;
    offset += sector_;
    offset *= sectorLength_;

    diskFile.Seek(offset, wxFromStart);
    bytes = diskFile.Read(sectorBuffer_, sectorLength_);
    diskFile.Close();

    if (bytes < sectorLength_)
    {
        p_Main->message("2. Error reading disk image");
        endCommand(0x80);
        return;
    }
}

void Fdc::writeSector() 
{
    wxFFile diskFile;
    wxFileOffset offset;

    if (!diskCreated_[drive_])
    {
        diskFile.Open(diskName_[drive_], "wb");
         diskFile.Close();
        diskCreated_[drive_] = true;
    }
    if (!diskFile.Open(diskName_[drive_], "rb+"))
    {
        endCommand(0x80);
        return;
    }
    if (sector_ >= numberOfSectors_)
    {
        p_Main->message("Unknown Sector");
        endCommand(0x10);
        return;
    }
    if (track_ >= numberOfTracks_[drive_])
    {
        p_Main->message("Unknown Track");
        endCommand(0x10);
        return;
    }

    if ((sector_ == 0) && (track_ == 0) && (side_ == 0) && (fdc17xxType_ == FDC1770))
    {
        numberOfTracks_[drive_] = numberOfTracksPerSide_ +(sectorBuffer_[0x12] * numberOfTracksPerSide_);
        numberOfSides_[drive_] = 1 + sectorBuffer_[0x13];
        if ((sectorBuffer_[0x12] == 0) &&(sectorBuffer_[0x13] == 0) &&((sectorBuffer_[0x70] == 0xff)))
            numberOfSides_[drive_] = 2;
    }

    offset = (track_ * numberOfSides_[drive_]) + side_;
    offset *= numberOfSectors_;
    offset += sector_;
    offset *= sectorLength_;

    diskFile.Seek(offset, wxFromStart);
    diskFile.Write(sectorBuffer_, sectorLength_);
    diskFile.Close();
}

void Fdc::formatSector() 
{
    wxFFile diskFile;
    wxFileOffset offset;

    if (!diskCreated_[drive_])
    {
        diskFile.Open(diskName_[drive_], "wb");
         diskFile.Close();
        diskCreated_[drive_] = true;
    }
    if (!diskFile.Open(diskName_[drive_], "rb+"))
    {
        endCommand(0x80);
        return;
    }

    offset = formatTrack_ +(formatSide_*numberOfTracksPerSide_);
    offset *= numberOfSectors_;
    offset += formatSector_;
    offset *= sectorLength_;

    diskFile.Seek(offset, wxFromStart);
    diskFile.Write(sectorBuffer_, sectorLength_);
    diskFile.Close();
}


void Fdc::endCommand(Byte status) 
{
    status_ = status&0xfc;
    updateFdcStatusLed();
    command_ = 0xff;
    exec_ = 0xff;
    intrq_ = 1;
}

void Fdc::onCommand(Byte command) 
{
    command_ = command;

    if (diskName_[drive_].Len() == 0)
    {
        p_Main->eventShowTextMessage("No Disk in drive");
        if (fdcConfiguration_.clearAddress != -1)
            p_Computer->writeMem(fdcConfiguration_.clearAddress, 0, false);
        endCommand(0x80);
        return;
    }
    switch(command_ & 0xf0) 
    {
        case 0:
            exec_ = command_;
            fdcCycles_ = p_Main->setFdcStepRate(command_ & 3);
            drq_ = 0;
            status_ = 1;
            updateFdcStatusLed();
        break;

        case 0x10:                                     /* seek */
            exec_ = command_;
            fdcCycles_ = p_Main->setFdcStepRate(command_ & 3);
            drq_ = 0;
            track_ = driveTrack_[drive_];
            status_ = 1;
            updateFdcStatusLed();
        break;

        case 0x20:                                     /* step, no update */
        case 0x30:                                     /* step, with update */
            exec_ = command_;
            fdcCycles_ = p_Main->setFdcStepRate(command_ & 3);
            drq_ = 0;
            status_ = 1;
            updateFdcStatusLed();
        break;

        case 0x40:                                     /* step in, no update */
        case 0x50:                                     /* step in, with update */
            exec_ = command_;
            fdcCycles_ = p_Main->setFdcStepRate(command_ & 3);
            drq_ = 0;
            stepDirection_ = 'I';
            status_ = 1;
            updateFdcStatusLed();
        break;

        case 0x60:                                     /* step out */
            exec_ = command_;
            fdcCycles_ = p_Main->setFdcStepRate(command_ & 3);
            drq_ = 0;
            stepDirection_ = 'O';
            status_ = 1;
            updateFdcStatusLed();
        break;

        case 0x80:                                     /* Read Sector */
            exec_ = 0x80;
            fdcCycles_ = p_Main->setFdcStepRate(command_ & 3);
            drq_ = 0;
            multipleSector_ = false;
            sectorBufferPointer = sectorBuffer_;
            status_ = 1;
            updateFdcStatusLed();
            readSector();
        break;

        case 0xa0:                                     /* Write Sector */
            exec_ = 0xa0;
            fdcCycles_ = p_Main->setFdcStepRate(command_ & 3);
            drq_ = 0;
            multipleSector_ = false;
            sectorBufferPointer = sectorBuffer_;
            status_ = 1;
            updateFdcStatusLed();
        break;

        case 0x90:                                     /* Read Multiple Sector */
            exec_ = 0x80;
            fdcCycles_ = p_Main->setFdcStepRate(command_ & 3);
            drq_ = 0;
            multipleSector_ = true;
            sectorBufferPointer = sectorBuffer_;
            status_ = 1;
            updateFdcStatusLed();
            readSector();
        break;

        case 0xb0:                                     /* Write Multiple Sector */
            exec_ = 0xa0;
            fdcCycles_ = p_Main->setFdcStepRate(command_ & 3);
            drq_ = 0;
            multipleSector_ = true;
            sectorBufferPointer = sectorBuffer_;
            status_ = 1;
            updateFdcStatusLed();
        break;

        case 0xc0:                                     /* Read Address */
            exec_ = 0xc0;
            fdcCycles_ = p_Main->setFdcStepRate(command_ & 3);
            drq_ = 0;
            sectorBuffer_ [0] = track_;
            sectorBuffer_ [1] = side_;
            sectorBuffer_ [2] = sector_;
            sectorBuffer_ [3] = (Byte)sectorLength_;
            sectorBuffer_ [4] = 0;
            sectorBuffer_ [5] = 0;
            sectorBufferPointer = sectorBuffer_;
            updateFdcStatusLed();
            status_ = 1;
        break;

        case 0xe0:                                     /* Read track */
            endCommand(0);
        break;

        case 0xf0:                                     /* Write track */
             exec_ = 0xf1;
             sectorBufferPointer = sectorBuffer_;
             drq_ = 1;
               status_ = 0x03;
            updateFdcStatusLed();
               idam_ = 0;
            formatCount_ = 0;
        break;

        case 0xd0:                                     /* Force Interrupt */
            if (command_ & 0xf)
                endCommand(0);
            else
            {
                status_ = 0;
                command_ = 0xff;
                exec_ = 0xff;
            }
        break;
    }
}

Byte Fdc::readData() 
{
    if (exec_ == 0x81) 
    {
        drq_ = 0;
        status_ &= 0xfd;
    }
    return data_;
}

void Fdc::writeData(Byte value) 
{
    data_ = value;
    if (exec_ == 0xa1) 
    {
        drq_ = 0;
        status_ &= 0xfd;
    }
    if (exec_ == 0xf1)
    {
        if (++formatCount_ == maxFmtCount_)   
        {
            status_ = 0;
            endCommand(0);
        }
        switch(idam_) 
        {
            case 0:                                        /* not in any fields */
                if (data_ == 0xfe) 
                {                                       /* begin idam */
                    idam_=1;
                }
                if (data_ >= 0xf8 && data_ <=0xfb) 
                {                                        /* data field */
                    sectorBufferPointer = sectorBuffer_;
                    idam_ = data_;
                     }
                   break;

            case 1:                                        /* track of idam */
                formatTrack_ = data_;
                idam_++;
            break;

            case 2:                                        /* side of idam*/
                formatSide_ = data_;
                idam_++;
            break;

            case 3:                                        /* sector of idam */
                formatSector_ = data_;
                idam_++;
            break;

            case 4:                                        /* should be zero */
                idam_++;
            break;

            case 5:                                        /* look for crc Byte */
                if (data_ == 0xf7) 
                {
                    idam_=0;
                }

            case 0xf8:                                    /* in data field */
            case 0xf9:
            case 0xfa:
            case 0xfb:
                *(sectorBufferPointer++) = data_;
                if (sectorBufferPointer == &sectorBuffer_[sectorLength_]) 
                {
                    formatSector();
                    idam_ = 0;
                }
            break;
        }
    }
}

void Fdc::updateFdcStatusLed()
{
    if (status_&1)
        p_Main->statusLedOnEvent();
    else
        p_Main->statusLedOffEvent();
}
