#ifndef UPD_H
#define UPD_H

#include "elfconfiguration.h"

#define FIRST_CLUSTER 10    // First cluster to be used for PC HD direct access
#define MAX_CLUSTER 630        // Max number of clusters per disk, used in PC HD direct access
#define BUFFER_CLUSTER 70    // Buffer of 70 clusters in case of attempt to access >630

class ClusterInfo
{
public:
    wxString fileName;
    bool sdwCluster;
    int startCluster;
    bool filenameDefined;
    bool readCluster;
    bool sdwClusterDefined;
};

class Upd765
{
public:
    Upd765();
    ~Upd765() {};

    void configureUpd765(int fdcType, int efnumber);
    Byte efInterrupt();
    Byte inputMasterStatus();
    void outputCommand(Byte value);
    Byte inputCommandStatus(void);
    void outputDmaCounter(Byte value);
    void outputDmaControl(Byte value);
    
    void cycleUpd765();
    void setUpdDiskname(int disk, wxString dirName, wxString fileName);
    void initializeCat(int drive);
    
protected:
    ClusterInfo clusterInfo_[4][MAX_CLUSTER + BUFFER_CLUSTER];
    int hdCommand_;
    bool resetHdData_;

private:
    void doCommand();
    void doRead();
    void hdRead();
    void doWrite();
    void hdWrite();
    void doFormatWrite();
    int selectHdCommand();
    void startHdCommand(int commandPos);
    void setCatMask(int cluster);
    int getCatMask(int cluster);
    int getFreeCatCluster();
    int get20FreeCatCluster();
    int hash(const char* dp);
    void buildDirectoryClusters(int clusterRequest);
    
    wxString diskName_[4];
    wxString diskDir_[4];
    bool diskCreated_[4];
    
    Byte diskBuffer_[4][322560];

    Byte dirBuffer_[0x1000];

    wxString writeFileName_;
    Byte writeFileAttribute_;
    
    Byte masterStatus_;
    Byte statusRegister0_;
    Byte statusRegister1_;
    Byte statusRegister2_;
    Byte statusRegister3_;
    int commandReturnCounter_;
    int commandReturnValue_;
    Byte interrupt_;
    Word dmaCounter_;
    Byte dmaControl_;
    Byte commandPacket_[16];
    Byte lastCommand_;
    int pcn;
    int commandPacketIndex;
    
    long fdcCycles_;
    Word diskBufferPos_;
    int drive_;
    int offset_;
    
    int updActivity_;
    int sectorsPerTrack_;
    int fdcType_;
    
    bool startTrace_;
};

#endif  // UPD_H

