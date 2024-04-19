/*
 *******************************************************************
 *** This software is copyright 2008 by Marcel van Tongeren      ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/*
 *******************************************************************
 *** This module is based on fdc.c code from the 1802sim written ***
 *** by David W. Schultz.                                        ***
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
#include "upd765.h"

#define SPCMD 3                    // uPD765 commands
#define RCCMD 7
#define SKCMD 15
#define RDCMD 0x46
#define WTCMD 0x45
#define FORMATCMD 0x4d
#define SISCMD 8
#define INVCMD 0

#define UPD_NONE 0                    // updActivity, used when simulating FDC step delay
#define UPD_READ 1
#define UPD_WRITE 2
#define UPD_FORMAT_WRITE 3

#define HD_UPD_NONE 0                // Direct PC HD access commands
#define HD_UPD_WRITE 1
#define HD_UPD_DEL 2
#define HD_UPD_RENAME 3
#define HD_UPD_ATTRIBUTE 4

#define MS_FDD0_BUSY 0x01
#define MS_FDD1_BUSY 0x02
#define MS_FDD2_BUSY 0x04
#define MS_FDD3_BUSY 0x08
#define MS_FDC_BUSY 0x10
#define MS_EXECUTION_MODE 0x20
#define MS_DATA_IO 0x40
#define MS_REQUEST_FOR_MASTER 0x80

#define SR_HEAD_ADDRESS 0x04
#define SR_NOT_READY 0x08
#define SR_EQUIPMENT_CHECK 0x10
#define SR_SEEK_END 0x20
#define SR_AT 0x40
#define SR_INVALID_COMMAND 0x80
#define SR_ABNORMAL_TERMINATION 0xc0

#define DISK_SIZE (512*9*70)

/*
 Not all commands are implemented and unimplemented commands are treated as
 invalid. This is a table of the number of bytes in the command packets.
 */
static Byte packetCount[32] = {
    1,  // 0  - invalid
    1,  // 1  - invalid
    9,  // 2  - read track
    3,  // 3  - specify
    1,  // 4  - invalid
    9,  // 5  - write data
    9,  // 6  - read data
    2,  // 7  - recalibrate
    1,  // 8  - sense fdc_fdc_interrupt status
    1,  // 9  - invalid
    1,  // 10 - invalid
    1,  // 11 - invalid
    1,  // 12 - invalid
    6,  // 13 - format write
    1,  // 14 - invalid
    3,  // 15 - seek
    1,  // 16 - invalid
    1,  // 17 - invalid
    1,  // 18 - invalid
    1,  // 19 - invalid
    1,  // 20 - invalid
    1,  // 21 - invalid
    1,  // 22 - invalid
    1,  // 23 - invalid
    1,  // 24 - invalid
    1,  // 25 - invalid
    1,  // 26 - invalid
    1,  // 27 - invalid
    1,  // 28 - invalid
    1,  // 29 - invalid
    1,  // 30 - invalid
    1   // 31 - invalid
};

Upd765::Upd765()
{
}

/*
 Configure UPD 765 for use with MS2000 and initialize startup values for internal variables
*/
void Upd765::configureUpd765(int fdcType, int efnumber)
{
    fdcType_ = fdcType;
    
    p_Computer->setCycleType(DISKCYCLEFDC, FDCCYCLE);
    
    for (int i=0; i<4; i++)
    {
        diskCreated_[i] = false;
    }
    
    p_Computer->setEfType(3, efnumber);

    p_Main->message("Configuring 18S651 and uPD765 Disk Controller");
    p_Main->message("	Output 4: DMA control, output 7: DMA count, input 4: uPD765 status");
    p_Main->message("	Output 5: uPD765 command register, input 5: uPD765 command register");
    p_Main->message("	EF 3: interrupt\n");

    masterStatus_ = MS_REQUEST_FOR_MASTER;
    interrupt_ = 1;
    commandPacketIndex = 0;
    
    fdcCycles_ = p_Main->getFdcCpms()*15;
    updActivity_ = UPD_NONE;
    hdCommand_ = HD_UPD_NONE;
    
    writeFileName_ = "";
    writeFileAttribute_ = 2;
    
    for (int drive = 0; drive < 4; drive++)
    {
        initializeCat(drive);
        for (int cluster = FIRST_CLUSTER; cluster < (MAX_CLUSTER + BUFFER_CLUSTER); cluster++)
        {
            clusterInfo_[drive][cluster].filenameDefined = false;
            clusterInfo_[drive][cluster].readCluster = true;
            clusterInfo_[drive][cluster].sdwClusterDefined = false;
        }
    }

    statusRegister0_ = 0;
    statusRegister1_ = 0;
    statusRegister2_ = 0;
    statusRegister3_ = 0;
    lastCommand_ = 0;
}

void Upd765::configureUpd765(int fdcType, Upd765Io upd765Io)
{
    fdcType_ = fdcType;
    
    p_Computer->setCycleType(DISKCYCLEFDC, UPD765_CYCLE);
    
    for (int i=0; i<4; i++)
    {
        diskCreated_[i] = false;
    }

    wxString ioGroup = "";
    if (upd765Io.ioGroup != -1)
        ioGroup.Printf(" on group %d", upd765Io.ioGroup);

    p_Main->message("Configuring uPD765 Disk Controller" + ioGroup);

    p_Computer->setOutTypeAndNumber(upd765Io.ioGroup+1, upd765Io.dmaControl, UPD765_DMA_CONTROL, 0, "DMA control");
    p_Computer->setOutTypeAndNumber(upd765Io.ioGroup+1, upd765Io.dmaCount, UPD765_DMA_COUNT, 0, "DMA count");
    p_Computer->setInTypeAndNumber(upd765Io.ioGroup+1, upd765Io.readStatus, UPD765_READ_STATUS, 0, "uPD765 status");
    p_Computer->setOutTypeAndNumber(upd765Io.ioGroup+1, upd765Io.writeCommand, UPD765_WRITE_COMMAND, 0, "write uPD765 command register");
    p_Computer->setInTypeAndNumber(upd765Io.ioGroup+1, upd765Io.readCommand, UPD765_READ_COMMAND, 0, "read uPD765 command register");
    p_Computer->setEfTypeAndNumber(-1, upd765Io.ioGroup+1, upd765Io.efInterrupt, UPD765_EF, 0, "interrupt");

    p_Main->message("");

    masterStatus_ = MS_REQUEST_FOR_MASTER;
    interrupt_ = 1;
    commandPacketIndex = 0;
    
    fdcCycles_ = p_Main->getFdcCpms()*15;
    updActivity_ = UPD_NONE;
    hdCommand_ = HD_UPD_NONE;
    
    writeFileName_ = "";
    writeFileAttribute_ = 2;
    
    for (int drive = 0; drive < 4; drive++)
    {
        initializeCat(drive);
        for (int cluster = FIRST_CLUSTER; cluster < (MAX_CLUSTER + BUFFER_CLUSTER); cluster++)
        {
            clusterInfo_[drive][cluster].filenameDefined = false;
            clusterInfo_[drive][cluster].readCluster = true;
            clusterInfo_[drive][cluster].sdwClusterDefined = false;
        }
    }

    statusRegister0_ = 0;
    statusRegister1_ = 0;
    statusRegister2_ = 0;
    statusRegister3_ = 0;
    lastCommand_ = 0;
}

/*
 Return interrupt state
*/
Byte Upd765::efInterrupt()
{
    return interrupt_;
}

/*
 Transfer the data from the simulated file system. Data is transfered to/from the location at R[0].
 This happens directly when using the direct access to PC HD or via updActivity_ via 'cycleUpd765' to simulate FDC step 
 delay
 */
void Upd765::doRead()
{
    int offset;
    wxFFile diskFile;
    
    interrupt_ = 0;
    
    // Calculate location from cylinder and record number (head ignored)
    offset = (commandPacket_[2]*9 + commandPacket_[4]-1);
    
    // The hardware can simulate data transfers for the FDC
    // without actually moving data. This is used to verify
    // that data has been written correctly. (CRC check)¥
    
    if(dmaControl_ == 3)   // actual transfer?
    {
        if (!p_Main->getDirectoryMode(fdcType_, drive_))
        {
            // Folloing 3 commented lines are for debug purpose showing current data in debug message window
            // wxString textMessage;
            // textMessage.Printf("READ, Cluster %02X, drive %i, image %04X-%04X, size %i", offset, drive_, offset*512, offset * 512+dmaCounter_, dmaCounter_);
            // p_Main->eventShowTextMessage(textMessage);
            if (!diskCreated_[drive_] || diskName_[drive_] == "")
            {
                statusRegister0_ = SR_ABNORMAL_TERMINATION | SR_HEAD_ADDRESS | drive_;
                commandReturnCounter_ = 1;
                commandReturnValue_ = 0;
                masterStatus_ = MS_FDC_BUSY | MS_DATA_IO | MS_REQUEST_FOR_MASTER;
                return;
            }
            
            if (!diskFile.Open(diskDir_[drive_]+diskName_[drive_], "rb+"))
            {
                statusRegister0_ = SR_ABNORMAL_TERMINATION | SR_HEAD_ADDRESS | drive_;
                // drive not ready error if not open
                commandReturnCounter_ = 1;
                commandReturnValue_ = 0;
                masterStatus_ = MS_FDC_BUSY | MS_DATA_IO | MS_REQUEST_FOR_MASTER;
                return;
            }
        
            diskFile.Seek(offset*512, wxFromStart);
            diskFile.Read(&diskBuffer_[drive_][0], dmaCounter_);
            diskBufferPos_ = 0;
            masterStatus_ = MS_FDC_BUSY;
            updActivity_ = UPD_READ;
            statusRegister0_ = SR_NOT_READY;
            diskFile.Close();
        }
        else
        {
            // Folloing 3 commented lines are for debug purpose showing current data in debug message window
            // wxString textMessage;
            // textMessage.Printf("READ, Cluster %i, drive %i", offset, drive_);
            // p_Main->eventShowTextMessage(textMessage);

            if (offset == 0)
            {
                for (int pos = 0; pos < dmaCounter_; pos++)
                    diskBuffer_[drive_][pos] = 0;
                
                wxFileName setectedDirFile = wxFileName(diskDir_[drive_]);
                wxDateTime modTime = setectedDirFile.GetModificationTime();
                wxString date = modTime.FormatISODate();
                
                int temp_pos = 0xc;
                diskBuffer_[drive_][temp_pos++] = date.GetChar(5);
                diskBuffer_[drive_][temp_pos++] = date.GetChar(6);
                diskBuffer_[drive_][temp_pos++] = '/';
                diskBuffer_[drive_][temp_pos++] = date.GetChar(8);
                diskBuffer_[drive_][temp_pos++] = date.GetChar(9);
                diskBuffer_[drive_][temp_pos++] = '/';
                diskBuffer_[drive_][temp_pos++] = date.GetChar(2);
                diskBuffer_[drive_][temp_pos++] = date.GetChar(3);
                
                wxDir selectedDir(diskDir_[drive_]);
                wxString dirName = selectedDir.GetName();
                
                if (dirName.Len()>44)
                {
                    wxArrayString dirArray = setectedDirFile.GetDirs();
                    dirName = dirArray.Last();
                }
                
                while (temp_pos < 0x40 && dirName.Len() > 0)
                {
                    diskBuffer_[drive_][temp_pos++] = dirName.GetChar(0);
                    dirName = dirName.Right(dirName.Len()-1);
                }

                for (int pos = 0; pos < dmaCounter_; pos++)
                    p_Computer->writeMem(p_Computer->getAndIncRegister0(), diskBuffer_[drive_][pos], false);
            }

            if (offset >= 1 && offset <= 8)
            {
                buildDirectoryClusters(offset);
                for (int pos = offset * 512; pos < (offset * 512) + dmaCounter_; pos++)
                    p_Computer->writeMem(p_Computer->getAndIncRegister0(), diskBuffer_[drive_][pos], false);
            }

            if (offset == 9)
            {
                for (int pos = offset * 512; pos < (offset * 512) + dmaCounter_; pos++)
                    p_Computer->writeMem(p_Computer->getAndIncRegister0(), diskBuffer_[drive_][pos], false);
            }

            if (offset >= FIRST_CLUSTER)
            {
                if (!clusterInfo_[drive_][offset].readCluster)
                {
                    for (int pos = offset * 512; pos < (offset * 512) + dmaCounter_; pos++)
                        p_Computer->writeMem(p_Computer->getAndIncRegister0(), diskBuffer_[drive_][pos], false);

                    updActivity_ = UPD_NONE;
                    statusRegister0_ = drive_;
                    commandReturnCounter_ = 7;
                    commandReturnValue_ = 0;
                    masterStatus_ = MS_FDC_BUSY | MS_DATA_IO | MS_REQUEST_FOR_MASTER;

                    return;
                }
                if (dmaCounter_ >= 3584)
                {
                    if (wxFile::Exists(diskDir_[drive_] + "OP.SYS"))
                    {
                        if (!diskFile.Open(diskDir_[drive_] + "OP.SYS", "rb+"))
                        {
                            statusRegister0_ = SR_ABNORMAL_TERMINATION | SR_HEAD_ADDRESS | drive_;
                            // drive not ready error if not open
                            commandReturnCounter_ = 1;
                            commandReturnValue_ = 0;
                            masterStatus_ = MS_FDC_BUSY | MS_DATA_IO | MS_REQUEST_FOR_MASTER;
                            return;
                        }

                        diskFile.Seek((offset - 11) * 512, wxFromStart);
                        diskFile.Read(&diskBuffer_[drive_][offset * 512], dmaCounter_);

                        for (int pos = offset * 512; pos < (offset * 512) + dmaCounter_; pos++)
                            p_Computer->writeMem(p_Computer->getAndIncRegister0(), diskBuffer_[drive_][pos], false);

                        updActivity_ = UPD_NONE;
                        statusRegister0_ = drive_;
                        commandReturnCounter_ = 7;
                        commandReturnValue_ = 0;
                        masterStatus_ = MS_FDC_BUSY | MS_DATA_IO | MS_REQUEST_FOR_MASTER;

                        return;
                    }
                    else
                    {
                        statusRegister0_ = SR_ABNORMAL_TERMINATION | SR_HEAD_ADDRESS | drive_;
                        // drive not ready error if not open
                        commandReturnCounter_ = 1;
                        commandReturnValue_ = 0;
                        masterStatus_ = MS_FDC_BUSY | MS_DATA_IO | MS_REQUEST_FOR_MASTER;
                        return;
                    }
                }

                if (clusterInfo_[drive_][offset].sdwCluster)
                {
                    int numberOfClusters;

                    if (!diskFile.Open(clusterInfo_[drive_][offset].fileName, "rb+"))
                    {
                        statusRegister0_ = SR_ABNORMAL_TERMINATION | SR_HEAD_ADDRESS | drive_;
                        // drive not ready error if not open
                        commandReturnCounter_ = 1;
                        commandReturnValue_ = 0;
                        masterStatus_ = MS_FDC_BUSY | MS_DATA_IO | MS_REQUEST_FOR_MASTER;
                        return;
                    }

                    if (!clusterInfo_[drive_][offset].sdwClusterDefined)
                    {
                        clusterInfo_[drive_][offset].sdwClusterDefined = true;
                        wxFileOffset fileSize = diskFile.Length();
                        if (fileSize % 512 == 0)
                            numberOfClusters = (int)(fileSize / 512);
                        else
                            numberOfClusters = (int)(fileSize / 512) + 1;
                        for (int pos = offset * 512; pos < (offset * 512) + dmaCounter_; pos++)
                            diskBuffer_[drive_][pos] = 0;
                        
                        int clusterPointer = 0, subNumberOfClusters = numberOfClusters, subClusterStart = offset, sdwCorrection = 1, subNextClusterNumber, subLastClusterNumber;
                        
                        subNextClusterNumber = 0;
                        while (subNextClusterNumber < (subClusterStart + 1))
                        {
                            subNextClusterNumber = getFreeCatCluster();
                            setCatMask(subNextClusterNumber);
                        }
                        
                        while (subNumberOfClusters > 0)
                        {
                            int tempClusterCounter = sdwCorrection;
                            
                            subLastClusterNumber = subNextClusterNumber - 1;
                            while (subNextClusterNumber == (subLastClusterNumber + 1) && tempClusterCounter <= 0x7e && tempClusterCounter <= subNumberOfClusters)
                            {
                                setCatMask(subNextClusterNumber);
                                clusterInfo_[drive_][subNextClusterNumber].fileName = clusterInfo_[drive_][offset].fileName;
                                clusterInfo_[drive_][subNextClusterNumber].sdwCluster = false;
                                clusterInfo_[drive_][subNextClusterNumber].startCluster = offset + 1;
                                subLastClusterNumber = subNextClusterNumber;
                                subNextClusterNumber = getFreeCatCluster();
                                tempClusterCounter++;
                            }
                            diskBuffer_[drive_][offset * 512 + clusterPointer++] = tempClusterCounter - 1;
                            diskBuffer_[drive_][offset * 512 + clusterPointer++] = (subClusterStart & 0xff00) >> 8;
                            diskBuffer_[drive_][offset * 512 + clusterPointer++] = subClusterStart & 0xff;
                            
                            subClusterStart += tempClusterCounter;
                            subNumberOfClusters -= tempClusterCounter;
                            sdwCorrection = 0;
                            
                            /*                        if (subNumberOfClusters > 0x7e)
                             diskBuffer_[drive_][offset * 512 + clusterPointer++] = 0x7e;
                             else
                             diskBuffer_[drive_][offset * 512 + clusterPointer++] = subNumberOfClusters;
                             diskBuffer_[drive_][offset * 512 + clusterPointer++] = (subClusterStart & 0xff00) >> 8;
                             diskBuffer_[drive_][offset * 512 + clusterPointer++] = subClusterStart & 0xff;
                             
                             subClusterStart += 0x7f;
                             subNumberOfClusters -= 0x7e + sdwCorrection;
                             sdwCorrection = 0;*/
                        }
                        diskBuffer_[drive_][offset * 512 + clusterPointer++] = 0x80 + ((numberOfClusters & 0xff00) >> 8);
                        diskBuffer_[drive_][offset * 512 + clusterPointer] = numberOfClusters & 0xff;
                        
                        wxFileName FullPath = wxFileName(clusterInfo_[drive_][offset].fileName);
                        wxString name = FullPath.GetFullName();
                        wxString path = FullPath.GetPath(true);
                        
                        if (wxFile::Exists(path + "." + name))
                        {
                            wxFFile infoFile;
                            infoFile.Open(path + "." + name, "rb+");
                            infoFile.Read(&diskBuffer_[drive_][offset * 512 + 0x1F4], 12);
                            infoFile.Close();
                        }
                    }
                    for (int pos = offset * 512; pos < (offset * 512) + dmaCounter_; pos++)
                        p_Computer->writeMem(p_Computer->getAndIncRegister0(), diskBuffer_[drive_][pos], false);

    /*                for (int cluster = offset + 1; cluster <= (offset + numberOfClusters); cluster++)
                    {
                        setCatMask(cluster);
                        clusterInfo_[drive_][cluster].fileName = clusterInfo_[drive_][offset].fileName;
                        clusterInfo_[drive_][cluster].sdwCluster = false;
                        clusterInfo_[drive_][cluster].startCluster = offset + 1;
                    }*/
                }
                else
                {
                    if (!diskFile.Open(clusterInfo_[drive_][offset].fileName, "rb+"))
                    {
                        statusRegister0_ = SR_ABNORMAL_TERMINATION | SR_HEAD_ADDRESS | drive_;
                        // drive not ready error if not open
                        commandReturnCounter_ = 1;
                        commandReturnValue_ = 0;
                        masterStatus_ = MS_FDC_BUSY | MS_DATA_IO | MS_REQUEST_FOR_MASTER;
                        return;
                    }
                    
                    int seek = offset - clusterInfo_[drive_][offset].startCluster;
                    if (seek < 0)
                        seek = 0;
                    diskFile.Seek(seek * 512, wxFromStart);
                    size_t bytesRead = diskFile.Read(&diskBuffer_[drive_][offset * 512], dmaCounter_);

                    if (bytesRead < 512)
                    {
                        for (size_t pos = (size_t)(offset * 512 + bytesRead); pos < (size_t)((offset + 1) * 512); pos++)
                            diskBuffer_[drive_][pos] = 0;
                    }
                    for (int pos = offset * 512; pos < (offset * 512) + dmaCounter_; pos++)
                        p_Computer->writeMem(p_Computer->getAndIncRegister0(), diskBuffer_[drive_][pos], false);
                }
                diskFile.Close();
            }
            updActivity_ = UPD_NONE;
            statusRegister0_ = drive_;
            commandReturnCounter_ = 7;
            commandReturnValue_ = 0;
            masterStatus_ = MS_FDC_BUSY | MS_DATA_IO | MS_REQUEST_FOR_MASTER;
        }
    }
}

void Upd765::doWrite()
{
    wxFFile diskFile;
    
    interrupt_ = 0;
    
    // Calculate location from cylinder and record number (head ignored)
    offset_ = (commandPacket_[2]*9 + commandPacket_[4]-1);

    if (!p_Main->getDirectoryMode(fdcType_, drive_))
    {
        // Folloing 3 commented lines are for debug purpose showing current data in debug message window
        // wxString textMessage;
        // textMessage.Printf("WRITE, Cluster %02X, drive %i", offset_, drive_);
        // p_Main->eventShowTextMessage(textMessage);
        if (!diskCreated_[drive_] || diskName_[drive_] == "")
        {
            statusRegister0_ = SR_ABNORMAL_TERMINATION | SR_HEAD_ADDRESS | drive_;
            commandReturnCounter_ = 1;
            commandReturnValue_ = 0;
            masterStatus_ = MS_FDC_BUSY | MS_DATA_IO | MS_REQUEST_FOR_MASTER;
            return;
        }
        
        if (!diskFile.Open(diskDir_[drive_]+diskName_[drive_], "rb+"))
        {
            statusRegister0_ = SR_ABNORMAL_TERMINATION | SR_HEAD_ADDRESS | drive_;
            // drive not ready error if not open
            commandReturnCounter_ = 1;
            commandReturnValue_ = 0;
            masterStatus_ = MS_FDC_BUSY | MS_DATA_IO | MS_REQUEST_FOR_MASTER;
            return;
        }
        diskFile.Close();
    
        diskBufferPos_ = 0;
        masterStatus_ = MS_FDC_BUSY;
        updActivity_ = UPD_WRITE;
        statusRegister0_ = SR_NOT_READY;

    }
    else
    {
        // Folloing 3 commented lines are for debug purpose showing current data in debug message window
        // wxString textMessage;
        // textMessage.Printf("WRITE, Cluster %i, drive %i", offset_, drive_);
        // p_Main->eventShowTextMessage(textMessage);
        if (offset_ >= 1 && offset_ <= 8)
        {
            bool clusterEmpty=true;
            for (int pos=0; pos < dmaCounter_; pos++)
            {
                dirBuffer_[pos] = p_Computer->readMem(p_Computer->getAndIncRegister0());
                if (dirBuffer_[pos] != 0)
                    clusterEmpty = false;
            }
            
            if (!clusterEmpty)
                startHdCommand(selectHdCommand());
        }

        if (offset_ == 9)
        {
            for (int pos = offset_ * 512; pos < (offset_ * 512) + dmaCounter_; pos++)
                diskBuffer_[drive_][pos] = p_Computer->readMem(p_Computer->getAndIncRegister0());
        }

        if (offset_ >= FIRST_CLUSTER)
        {
            for (int pos = offset_ * 512; pos < (offset_ * 512) + dmaCounter_; pos++)
                diskBuffer_[drive_][pos] = p_Computer->readMem(p_Computer->getAndIncRegister0());

            if (clusterInfo_[drive_][offset_].readCluster)
            {
//        updActivity_ = UPD_NONE;
//        statusRegister0_ = drive_;
//        commandReturnCounter_ = 7;
//        commandReturnValue_ = 0;
//        masterStatus_ = MS_FDC_BUSY | MS_DATA_IO | MS_REQUEST_FOR_MASTER;
                return;
            }

            if (clusterInfo_[drive_][offset_].sdwCluster)
            {
                int clusterPointer = 0, numberOfClusters = 0, cluster;

                numberOfClusters = diskBuffer_[drive_][offset_ * 512 + clusterPointer++];
                cluster = diskBuffer_[drive_][offset_ * 512 + clusterPointer] * 256 + diskBuffer_[drive_][offset_ * 512 + clusterPointer + 1] + 1;
                clusterPointer += 2;

                while ((numberOfClusters & 0x80) == 0)
                {
                    for (int clusterCount = cluster; clusterCount < (cluster + numberOfClusters); clusterCount++)
                    {
                        clusterInfo_[drive_][clusterCount].sdwCluster = false;
                        clusterInfo_[drive_][clusterCount].filenameDefined = clusterInfo_[drive_][offset_].filenameDefined;
                        clusterInfo_[drive_][clusterCount].startCluster = offset_;
                        clusterInfo_[drive_][clusterCount].readCluster = false;
                    }
                    numberOfClusters = diskBuffer_[drive_][offset_ * 512 + clusterPointer++] + 1;
                    cluster = diskBuffer_[drive_][offset_ * 512 + clusterPointer] * 256 + diskBuffer_[drive_][offset_ * 512 + clusterPointer + 1];
                    clusterPointer += 2;
                }

                // Folloing 2 commented lines are for debug purpose showing current data in debug message window
                // textMessage.Printf("WRITE SDW, number of clusters %i, start cluster %i", diskBuffer_[drive_][offset_ * 512], diskBuffer_[drive_][offset_ * 512 + 1] * 256 + diskBuffer_[drive_][offset_ * 512 + 2]);
                // p_Main->eventShowTextMessage(textMessage);

                bool writeInfoFile = false;
                for (int i = 0x1f5; i < 0x200; i++)
                {
                    if (diskBuffer_[drive_][offset_ * 512 + i] != 0)
                        writeInfoFile = true;
                }
                
                if (writeFileAttribute_ != 2)
                    writeInfoFile = true;
                
                if (writeInfoFile)
                {
                    wxFileName FullPath = wxFileName(clusterInfo_[drive_][offset_].fileName);
                    wxString name = FullPath.GetFullName();
                    wxString path = FullPath.GetPath(true);
                    
                    if (wxFile::Exists(path + "." + name))
                    {
                        diskFile.Open(path + "." + name, "rb+");
                        diskFile.Seek(0, wxFromStart);
                        diskFile.Write(&diskBuffer_[drive_][offset_ * 512 + 0x1f4], 12);
                        diskFile.Write(&writeFileAttribute_, 1);
                        diskFile.Close();
                    }
                    else
                    {
                        wxFile createDiskFile;
                        createDiskFile.Create(path + "." + name);
                        createDiskFile.Write(&diskBuffer_[drive_][offset_ * 512 + 0x1f4], 12);
                        createDiskFile.Write(&writeFileAttribute_, 1);
                        createDiskFile.Close();
                    }
                }
            }
            
            if (clusterInfo_[drive_][offset_].filenameDefined)
            {
                offset_ = clusterInfo_[drive_][offset_].startCluster;
                
                if (wxFile::Exists(clusterInfo_[drive_][offset_].fileName))
                    wxRemoveFile(clusterInfo_[drive_][offset_].fileName);
                
                int clusterPointer = 0, numberOfClusters = 0, cluster;
                
                wxFile createDiskFile;
                createDiskFile.Create(clusterInfo_[drive_][offset_].fileName);
                
                numberOfClusters = diskBuffer_[drive_][offset_ * 512 + clusterPointer++];
                cluster = diskBuffer_[drive_][offset_ * 512 + clusterPointer] * 256 + diskBuffer_[drive_][offset_ * 512 + clusterPointer+1] + 1;
                clusterPointer += 2;

                while ((numberOfClusters & 0x80) == 0)
                {
                    createDiskFile.Write(&diskBuffer_[drive_][cluster * 512], numberOfClusters * 512);
                    
                    numberOfClusters = diskBuffer_[drive_][offset_ * 512 + clusterPointer++] + 1;
                    cluster = diskBuffer_[drive_][offset_ * 512 + clusterPointer] * 256 + diskBuffer_[drive_][offset_ * 512 + clusterPointer+1];
                    clusterPointer += 2;
                    
                }
                createDiskFile.Close();
            }
        }
        updActivity_ = UPD_NONE;
        statusRegister0_ = drive_;
        commandReturnCounter_ = 7;
        commandReturnValue_ = 0;
        masterStatus_ = MS_FDC_BUSY | MS_DATA_IO | MS_REQUEST_FOR_MASTER;
    }
}

void Upd765::doFormatWrite()
{
    wxFFile diskFile;
    
    interrupt_ = 0;
    
    if (!p_Main->getDirectoryMode(fdcType_, drive_))
    {
        if (diskDir_[drive_]+diskName_[drive_] == "" || diskName_[drive_] == "")
        {
            statusRegister0_ = SR_ABNORMAL_TERMINATION | SR_HEAD_ADDRESS | drive_;
            // drive not ready error if disk filename is empty
            commandReturnCounter_ = 1;
            commandReturnValue_ = 0;
            masterStatus_ = MS_FDC_BUSY | MS_DATA_IO | MS_REQUEST_FOR_MASTER;
            return;
        }
        
        if (!diskCreated_[drive_])
        {
            diskFile.Open(diskDir_[drive_]+diskName_[drive_], "wb+");
            diskFile.Close();
            diskCreated_[drive_] = true;
        }
        
        if (!diskFile.Open(diskDir_[drive_]+diskName_[drive_], "rb+"))
        {
            statusRegister0_ = SR_ABNORMAL_TERMINATION | SR_HEAD_ADDRESS | drive_;
            // drive not ready error if not open
            commandReturnCounter_ = 1;
            commandReturnValue_ = 0;
            masterStatus_ = MS_FDC_BUSY | MS_DATA_IO | MS_REQUEST_FOR_MASTER;
            return;
        }
        diskFile.Close();
        
        diskBufferPos_ = 0;
        masterStatus_ = MS_FDC_BUSY;
        updActivity_ = UPD_FORMAT_WRITE;
        statusRegister0_ = SR_NOT_READY;
        sectorsPerTrack_ = commandPacket_[3];
    }
    else
    {
        updActivity_ = UPD_NONE;
        statusRegister0_ = drive_;
        commandReturnCounter_ = 7;
        commandReturnValue_ = 0;
        masterStatus_ = MS_FDC_BUSY | MS_DATA_IO | MS_REQUEST_FOR_MASTER;
    }
}

int Upd765::selectHdCommand()
{
    int commandPos = 0, directoryPos = offset_ * 512;
    for (int pos = 0; pos <0x200; pos += 16)
    {
        if (dirBuffer_[pos] == 0xff)
        {
            if (hdCommand_ == HD_UPD_WRITE)
                hdCommand_ = HD_UPD_RENAME;
            else
                hdCommand_ = HD_UPD_DEL;
            commandPos = pos;
        }
        if (dirBuffer_[pos + 0xc] != diskBuffer_[drive_][directoryPos + 0xc])
        {
            hdCommand_ = HD_UPD_ATTRIBUTE;
            writeFileAttribute_ = dirBuffer_[pos + 0xc];
            commandPos = pos;
        }
        if (dirBuffer_[pos] != diskBuffer_[drive_][directoryPos] && diskBuffer_[drive_][directoryPos] == 0)
        {
            hdCommand_ = HD_UPD_WRITE;
            commandPos = pos;
        }
        directoryPos += 16;
    } 
    return commandPos;
}

void Upd765::startHdCommand(int commandPos)
{
    wxString fileName, ext;

    switch (hdCommand_)
    {
        case HD_UPD_WRITE:
        case HD_UPD_ATTRIBUTE:
            writeFileName_ = "";
            for (int i = 1; i <= 6; i++)
                writeFileName_.Append(dirBuffer_[commandPos++]);
            writeFileName_.Trim();

            ext = "";
            for (int i = 1; i <= 3; i++)
                ext.Append(dirBuffer_[commandPos++]);
            ext.Trim();

            if (ext != "")
                writeFileName_ = writeFileName_ + '.' + ext;
        break;

        case HD_UPD_DEL:
        case HD_UPD_RENAME:
            commandPos = offset_ * 512 + commandPos;
            fileName = "";
            for (int i = 1; i <= 6; i++)
                fileName.Append(diskBuffer_[drive_][commandPos++]);
            fileName.Trim();

            ext = "";
            for (int i = 1; i <= 3; i++)
                ext.Append(diskBuffer_[drive_][commandPos++]);
            ext.Trim();

            if (ext != "")
                fileName = fileName + '.' + ext;
        break;
    }
        
    wxFFile diskFile;
    int swdClusterNumber;
    int clusterPointer = 0, numberOfClusters = 0, cluster;

    switch (hdCommand_)
    {
        case HD_UPD_WRITE:
            swdClusterNumber = dirBuffer_[commandPos + 1] * 256 + dirBuffer_[commandPos + 2];
            clusterInfo_[drive_][swdClusterNumber].sdwCluster = true;
            clusterInfo_[drive_][swdClusterNumber].fileName = diskDir_[drive_] + writeFileName_;
            clusterInfo_[drive_][swdClusterNumber].filenameDefined = true;
            clusterInfo_[drive_][swdClusterNumber].startCluster = swdClusterNumber;
            clusterInfo_[drive_][swdClusterNumber].readCluster = false;

            clusterPointer = 0;
            numberOfClusters = diskBuffer_[drive_][swdClusterNumber * 512 + clusterPointer++];
            cluster = diskBuffer_[drive_][swdClusterNumber * 512 + clusterPointer] * 256 + diskBuffer_[drive_][swdClusterNumber * 512 + clusterPointer + 1] + 1;
            clusterPointer += 2;

            while ((numberOfClusters & 0x80) == 0 && cluster < MAX_CLUSTER)
            {
                for (int clusterCount = cluster; clusterCount < (cluster + numberOfClusters); clusterCount++)
                {
                    clusterInfo_[drive_][clusterCount].sdwCluster = false;
                    clusterInfo_[drive_][clusterCount].filenameDefined = clusterInfo_[drive_][swdClusterNumber].filenameDefined;
                    clusterInfo_[drive_][clusterCount].startCluster = swdClusterNumber;
                    clusterInfo_[drive_][clusterCount].readCluster = false;
                }
                numberOfClusters = diskBuffer_[drive_][swdClusterNumber * 512 + clusterPointer++] + 1;
                cluster = diskBuffer_[drive_][swdClusterNumber * 512 + clusterPointer] * 256 + diskBuffer_[drive_][swdClusterNumber * 512 + clusterPointer + 1];
                clusterPointer += 2;
            }
        break;

        case HD_UPD_DEL:
            if (wxFile::Exists(diskDir_[drive_] + "." + fileName))
                wxRemoveFile(diskDir_[drive_] + "." + fileName);

            if (wxFile::Exists(diskDir_[drive_] + fileName))
                wxRemoveFile(diskDir_[drive_] + fileName);
            hdCommand_ = HD_UPD_NONE;
        break;

        case HD_UPD_RENAME:
            if (wxFile::Exists(diskDir_[drive_] + "." + fileName))
                wxRenameFile(diskDir_[drive_] + "." + fileName, diskDir_[drive_] + "." + writeFileName_);

            if (wxFile::Exists(diskDir_[drive_] + fileName))
                wxRenameFile(diskDir_[drive_] + fileName, diskDir_[drive_] + writeFileName_);
            hdCommand_ = HD_UPD_NONE;
        break;

        case HD_UPD_ATTRIBUTE:
            Byte infoBuffer[13];
            if (wxFile::Exists(diskDir_[drive_] + "." + writeFileName_))
            {
                diskFile.Open(diskDir_[drive_] + "." + writeFileName_, "rb+");
                diskFile.Seek(12, wxFromStart);
                diskFile.Write(&writeFileAttribute_, 1);
                diskFile.Close();
            }
            else
            {
                wxFile createDiskFile;
                createDiskFile.Create(diskDir_[drive_] + "." + writeFileName_);
                for (int i = 0; i<12; i++)
                    infoBuffer[i] = 0;
                createDiskFile.Write(&infoBuffer, 12);
                createDiskFile.Write(&writeFileAttribute_, 1);
                createDiskFile.Close();
            }

            if (diskFile.Open(diskDir_[drive_] + "." + writeFileName_))
            {
                diskFile.Read(&infoBuffer, 13);
                diskFile.Close();

                bool writeInfoFile = false;
                for (int i = 0; i<12; i++)
                {
                    if (infoBuffer[i] != 0)
                        writeInfoFile = true;
                }
                if (infoBuffer[12] != 2)
                    writeInfoFile = true;

                if (!writeInfoFile)
                    wxRemoveFile(diskDir_[drive_] + "." + writeFileName_);
            }
            hdCommand_ = HD_UPD_NONE;
        break;
    }
}

void Upd765::setCatMask(int cluster)
{
    int mask = 0x80;
    int catPos = 0x1200;
    int bitNumber = cluster - 7;
    while (bitNumber != 0)
    {
        bitNumber--;
        mask = mask >> 1;
        if (mask == 0)
        {
            mask = 0x80;
            catPos++;
        }
    }
    diskBuffer_[drive_][catPos] = diskBuffer_[drive_][catPos] | mask;
}

int Upd765::getCatMask(int cluster)
{
    int mask = 0x80;
    int catPos = 0x1200;
    int bitNumber = cluster - 7;
    while (bitNumber != 0)
    {
        bitNumber--;
        mask = mask >> 1;
        if (mask == 0)
        {
            mask = 0x80;
            catPos++;
        }
    }
    return (diskBuffer_[drive_][catPos] | mask);
}

int Upd765::getFreeCatCluster()
{
    int cluster = 7, catPos = 0x1200;
    while (diskBuffer_[drive_][catPos] == 0xff)
    {
        cluster += 8;
        catPos++;
    }
    int value = diskBuffer_[drive_][catPos];
    while ((value & 0x80) == 0x80)
    {
        cluster ++;
        value = value << 1;
    }
    if (cluster > (MAX_CLUSTER - 1))
        return (MAX_CLUSTER - 1);
    return cluster;
}

int Upd765::get20FreeCatCluster()
{
    int cluster = 7, catPos = 0x1200;
    bool emptySpaceFound = false;
    while (!emptySpaceFound)
    {
        if (diskBuffer_[drive_][catPos] != 0xff && diskBuffer_[drive_][catPos+1] == 0 && diskBuffer_[drive_][catPos+2] == 0)
            emptySpaceFound = true;
        else
        {
            cluster += 8;
            catPos++;
        }
    }
    int value = diskBuffer_[drive_][catPos];
    while ((value & 0x80) == 0x80)
    {
        cluster ++;
        value = value << 1;
    }
    if (cluster > (MAX_CLUSTER - 1))
        return (MAX_CLUSTER - 1);
    
    return cluster;
}

/*
 Peform a command as specified by the data packet. Only a few commands
 are implemented. All others are invalid.
 */
void Upd765::doCommand()
{
    wxFFile diskFile;

    drive_ = commandPacket_[1] & 3;                 // extract drive #
    resetHdData_ = true;

    switch(commandPacket_[0])
    {
        case SPCMD:                  // specify
            //p_Main->eventShowTextMessage("specify ");
            // Nothing to do
        break;
            
        case RCCMD:                  // recalibrate            
            //p_Main->eventShowTextMessage("recalibrate ");
            if (!diskCreated_[drive_] && !p_Main->getDirectoryMode(fdcType_, drive_))
            {
                if (diskName_[drive_] == "")
                {
                    pcn = 0;
                    statusRegister0_ = SR_SEEK_END | (commandPacket_[1] & 3);
                    // no, signal an error
                }
                else
                {
                    diskFile.Open(diskDir_[drive_] + diskName_[drive_], "wb+");
                    diskFile.Close();
                    diskCreated_[drive_] = true;
                }
            }
            
            if(diskCreated_[drive_])  // image file opened?
            {
                pcn = 0;
                statusRegister0_ = SR_SEEK_END | (commandPacket_[1]&3);
                                                        // no, signal an error
            }
            else
            {
                if (p_Main->getDirectoryMode(fdcType_, drive_))
                {
                    pcn = 0;
                    statusRegister0_ = SR_SEEK_END | (commandPacket_[1]&3);
                    // no, signal an error
                }
                else
                {
                    pcn = 0;
                    statusRegister0_ = SR_AT;
                }
            }
            interrupt_ = 0;
        break;
            
        case SKCMD:                  // seek
            //p_Main->eventShowTextMessage("seek ");
            // store some values in case this is followed by sense interrupt
            if(diskCreated_[drive_])
            {
                pcn = commandPacket_[2];
                statusRegister0_ = SR_SEEK_END | (commandPacket_[1]&3);
            }
            else
            {
                if (p_Main->getDirectoryMode(fdcType_, drive_))
                {
                    pcn = 0;
                    statusRegister0_ = SR_SEEK_END | (commandPacket_[1]&3);
                    // no, signal an error
                }
                else
                {
                    pcn = 0;
                    statusRegister0_ = SR_AT;
                }
            }
            interrupt_ = 0;
        break;
            
        case RDCMD:                  // read
            //p_Main->eventShowTextMessage("read ");
            doRead();
        break;
            
        case WTCMD:                  // write
            //p_Main->eventShowTextMessage("write ");
            doWrite();
        break;
            
        case FORMATCMD:              // format
            doFormatWrite();
        break;
            
        case SISCMD:                 // sense interrupt status
            if (lastCommand_ != commandPacket_[0])
            {
                //p_Main->eventShowTextMessage("sense ");
                interrupt_ = 0;
                masterStatus_ = MS_FDC_BUSY | MS_DATA_IO | MS_REQUEST_FOR_MASTER;
                commandReturnCounter_ = 2;
                commandReturnValue_ = 0;
            }
            else
            {
                //p_Main->eventShowTextMessage("invalid/second sense ");
                statusRegister0_ = SR_INVALID_COMMAND;
                commandReturnCounter_ = 1;
                commandReturnValue_ = 0;
                masterStatus_ = MS_DATA_IO | MS_REQUEST_FOR_MASTER;
            }
        break;
            
        default:                 // Invalid command
            //p_Main->eventShowTextMessage("invalid ");
            statusRegister0_ = SR_INVALID_COMMAND;
            commandReturnCounter_ = 1;
            commandReturnValue_ = 0;
            masterStatus_ = MS_DATA_IO | MS_REQUEST_FOR_MASTER;
        break;
    }
    lastCommand_ = commandPacket_[0];
}

/*
 Data written to the command port is processed here.
 */
void Upd765::outputCommand(Byte value)
{
    fflush(stdout);
    
    if (masterStatus_ == MS_REQUEST_FOR_MASTER)
    {
        commandPacket_[commandPacketIndex++] = value;
        
        if(commandPacketIndex == packetCount[commandPacket_[0]&0x1f])
        {
            doCommand();
            commandPacketIndex = 0;
        }
    }
}

/*
 Reads from the data port are handled here.
 
 This is read to get the results of commands. UT71 only looks at the first
 byte (ST0).
 
 ST0 -
 D7   - interrupt_ code, normal termination is 00, but an invalid
 D6     command will result in 10.
 D5   - seek end, set after a seek completes
 D4   - equipment check
 D3   - not ready
 D2   - head address
 D1   - unit select
 D0
 
 UT71 only ever looks at ST0. So the rest don't matter, it is all just
 copies of ST0.
 */
Byte Upd765::inputCommandStatus(void)
{
    int returnValue;

    if((masterStatus_&0xc0) == (MS_DATA_IO | MS_REQUEST_FOR_MASTER))
    {
        interrupt_ = 1;               // clear interrupt
        if(commandReturnCounter_-- == 1) // last byte, change to ready for command
            masterStatus_ = MS_REQUEST_FOR_MASTER;
        
        switch (commandReturnValue_)
        {
            case 0:
                returnValue = statusRegister0_;
            break;
            case 1:
                if (commandPacket_[0] == SISCMD)
                    returnValue = pcn;
                else
                    returnValue = statusRegister1_;
            break;
            case 2:
                returnValue = statusRegister2_;
            break;
            case 3:
                returnValue = statusRegister3_;
            break;
            default:
                returnValue = commandPacket_[commandReturnValue_-1];
            break;
        }

        commandReturnValue_++;
        return returnValue;
    }
    return 0;
}

/*
 Reads of the uPD765 master status port come here.
 
 This is complicated by its interaction with the command and data functions.
 
 The 5 least significant bits indicate if the FDC is busy with some task.
 
 Mostly UT71 tests the MRQ (master request) bit. But it also checks the
 DIO (direction) bit in a couple of spots.
 
 */
Byte Upd765::inputMasterStatus()
{
    return masterStatus_;
}

/*
 Writes to the DMA transfer counter.
 
 The hardware counter expects a count of the number of sectors (single
 density 128bytes/sector) to transfer.
 */

void Upd765::outputDmaCounter(Byte value)
{
    dmaCounter_ = value*128;
}

/*
 Writes to the DMA control register.
 
 This tells the hardware what direction the transfer is in. We can figure
 that out from the command so that bit is ignored. It also has a bit used
 to cause dummy DMA cycles. The uPD765 is fed DMA acks but the 1802
 is not asked to generate actual DMA cycles.
 */
void Upd765::outputDmaControl(Byte value)
{
    dmaControl_ = value;
}

void Upd765::cycleUpd765()
{
    if (--fdcCycles_ > 0) return;
    if (fdcCycles_ < 0) fdcCycles_ = 0;
    
    fdcCycles_ = p_Main->getFdcCpms()*15;
    
    switch (updActivity_)
    {
        case UPD_READ:
            for (int pos=diskBufferPos_; pos < diskBufferPos_+128; pos++)
                p_Computer->dmaIn(diskBuffer_[drive_][pos]);
            diskBufferPos_ += 128;
            if (diskBufferPos_ >= dmaCounter_)
            {
                updActivity_ = UPD_NONE;
                statusRegister0_ = drive_;
                commandReturnCounter_ = 7;
                commandReturnValue_ = 0;
                masterStatus_ = MS_FDC_BUSY | MS_DATA_IO | MS_REQUEST_FOR_MASTER;
            }
        break;
            
        case UPD_WRITE:
            for (int pos=diskBufferPos_; pos < diskBufferPos_+128; pos++)
                diskBuffer_[drive_][pos] = p_Computer->dmaOut();
            diskBufferPos_ += 128;
            if (diskBufferPos_ >= dmaCounter_)
            {
                wxFFile diskFile;
                
                if (!diskCreated_[drive_])
                {
                    statusRegister0_ = SR_ABNORMAL_TERMINATION | SR_HEAD_ADDRESS | drive_;      // drive not ready error if disk not created
                    commandReturnCounter_ = 1;
                    commandReturnValue_ = 0;
                    masterStatus_ = MS_FDC_BUSY | MS_DATA_IO | MS_REQUEST_FOR_MASTER;
                    updActivity_ = UPD_NONE;
                    return;
                }
            
                if (!diskFile.Open(diskDir_[drive_]+diskName_[drive_], "rb+"))
                {
                    statusRegister0_ = SR_ABNORMAL_TERMINATION | SR_HEAD_ADDRESS | drive_;      // drive not ready error if not open
                    commandReturnCounter_ = 1;
                    commandReturnValue_ = 0;
                    masterStatus_ = MS_FDC_BUSY | MS_DATA_IO | MS_REQUEST_FOR_MASTER;
                    updActivity_ = UPD_NONE;
                    return;
                }
                diskFile.Seek(offset_*512, wxFromStart);
            
                diskFile.Write(&diskBuffer_[drive_][0], dmaCounter_);
            
                updActivity_ = UPD_NONE;
                statusRegister0_ = drive_;
                commandReturnCounter_ = 7;
                commandReturnValue_ = 0;
                masterStatus_ = MS_FDC_BUSY | MS_DATA_IO | MS_REQUEST_FOR_MASTER;
                diskFile.Close();
            }
        break;

        case UPD_FORMAT_WRITE:
            for (int pos=diskBufferPos_; pos < diskBufferPos_+128; pos++)
                diskBuffer_[drive_][pos] = 0;
            diskBufferPos_ += 128;
            if (diskBufferPos_ >= 512*9)
            {
                wxFFile diskFile;
                
                if (!diskCreated_[drive_])
                {
                    statusRegister0_ = SR_ABNORMAL_TERMINATION | SR_HEAD_ADDRESS | drive_;      // drive not ready error if disk not created
                    commandReturnCounter_ = 1;
                    commandReturnValue_ = 0;
                    masterStatus_ = MS_FDC_BUSY | MS_DATA_IO | MS_REQUEST_FOR_MASTER;
                    updActivity_ = UPD_NONE;
                    return;
                }
                
                if (!diskFile.Open(diskDir_[drive_]+diskName_[drive_], "rb+"))
                {
                    statusRegister0_ = SR_ABNORMAL_TERMINATION | SR_HEAD_ADDRESS | drive_;      // drive not ready error if not open
                    commandReturnCounter_ = 1;
                    commandReturnValue_ = 0;
                    masterStatus_ = MS_FDC_BUSY | MS_DATA_IO | MS_REQUEST_FOR_MASTER;
                    updActivity_ = UPD_NONE;
                    return;
                }
                diskFile.Seek(pcn*sectorsPerTrack_*512, wxFromStart);
                diskFile.Write(&diskBuffer_[drive_][0], dmaCounter_);
                
                updActivity_ = UPD_NONE;
                statusRegister0_ = drive_;
                commandReturnCounter_ = 7;
                commandReturnValue_ = 0;
                masterStatus_ = MS_FDC_BUSY | MS_DATA_IO | MS_REQUEST_FOR_MASTER;
                diskFile.Close();
            }
        break;
    }
}

void Upd765::setUpdDiskname(int disk, wxString dirName, wxString fileName)
{
    diskDir_[disk-1] = dirName;
    diskName_[disk-1] = fileName;
    diskCreated_[disk-1] = wxFile::Exists(diskDir_[disk-1] + diskName_[disk-1]);
}

/*
 MicroDOS uses a hashing function to decide which directory sector to
 use/search for an entry. If the directory entry isn't where it is
 expected to be, it will not be found.
 
 The key bit of trickiness here is that MicroDOS uses an add with carry
 while adding the characters in the filename together.
 
 The lower 3 bits of that result are used to index into the eight
 directory sectors.
 */
int Upd765::hash(const char* dp)
{
    unsigned int i, h, carry;
    
    carry = 0;
    for(i = 0, h = 0; i < 9; i++)
    {
        h += carry + *dp++;
        
        if(h&0x100)
        {
            carry = 1;
            h &= 0xff;
        }
        else
            carry = 0;
    }
    h &= 0x07;
    
    return h;
}

void Upd765::buildDirectoryClusters(int clusterRequest)
{
    wxString filename;
    int firstCluster = get20FreeCatCluster();

    // Folloing 3 commented lines are for debug purpose showing current data in debug message window
    // wxString textMessage;
    // textMessage.Printf("DIR, Cluster %i, drive %i", firstCluster, drive_);
    // p_Main->eventShowTextMessage(textMessage);

    wxDir dir (diskDir_[drive_]);
    bool cont = dir.GetFirst(&filename);
    
    for (int i=0x200; i<0x1200; i++)
        diskBuffer_[drive_][i] = 0;
    
    while (cont)
    {
        wxFileName FullPath = wxFileName(diskDir_[drive_] + filename, wxPATH_NATIVE);
        wxString ext = FullPath.GetExt();
        wxString name = FullPath.GetName();

        while (name.Len() < 6)
            name = name + " ";
        while (ext.Len() < 3)
            ext = ext + " ";
        
        name = name.MakeUpper();
        name = name.Left(6);
        ext = ext.MakeUpper();
        ext = ext.Left(3);

        wxString filenameAdapted = name + ext;
        int directoryCluster = hash(filenameAdapted);
        
        int bufferPointer = (directoryCluster + 1) * 512;
        int clusterPointer = 0;
        int numberOfFiles = 0;
        while (diskBuffer_[drive_][bufferPointer] != 0 && numberOfFiles <= 128)
        {
            bufferPointer += 16;
            clusterPointer++;
            numberOfFiles++;

            if (clusterPointer == 16)
            {
                clusterPointer = 0;
                directoryCluster++;
                directoryCluster = directoryCluster & 0x7;
                bufferPointer = directoryCluster * 512;
            }
        }
        
        directoryCluster++;
        if (numberOfFiles <= 128 && name.Left(1) != ".")
        {
            for (int i=0; i < 9; i++)
                diskBuffer_[drive_][bufferPointer++] = filenameAdapted[i];
            
            bufferPointer ++;
            clusterInfo_[drive_][firstCluster].fileName = diskDir_[drive_] + filename;
            clusterInfo_[drive_][firstCluster].sdwCluster = true;
            
            diskBuffer_[drive_][bufferPointer++] = (firstCluster & 0xff00) >> 8;
            diskBuffer_[drive_][bufferPointer++] = firstCluster & 0xff;
            if (directoryCluster == clusterRequest)
                firstCluster++;
 
            diskBuffer_[drive_][bufferPointer] = 0x2;
      
            Byte infoBuffer[13];
            size_t numberOfBytesRead = 0;
            if (wxFile::Exists(diskDir_[drive_]+"."+filename))
            {
                wxFFile infoFile;
                infoFile.Open(diskDir_[drive_]+"."+filename);
                numberOfBytesRead = infoFile.Read(&infoBuffer, 13);
                infoFile.Close();
            
                if (numberOfBytesRead == 13)
                    diskBuffer_[drive_][bufferPointer] = infoBuffer[12];
                else
                {
                    if (ext == "CM " || ext.Left(2) == "OV")
                        diskBuffer_[drive_][bufferPointer] = 0xf1;
                    
                    if (ext == "SYS")
                        diskBuffer_[drive_][bufferPointer] = 0xf3;
                }
            }
        }
 
        cont = dir.GetNext(&filename);
    }
}

void Upd765::initializeCat(int drive)
{
    diskBuffer_[drive][512*9] = 0xe0;
    for (int pos = (512 * 9) + 1; pos < (512 * 9) + 512; pos++)
        diskBuffer_[drive][pos] = 0;
}
