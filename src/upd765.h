#ifndef UPD_H
#define UPD_H

#include "elfconfiguration.h"

#define FIRST_CLUSTER 10	// First cluster to be used for PC HD direct access
#define MAX_CLUSTER 630		// Max number of clusters per disk, used in PC HD direct access
#define BUFFER_CLUSTER 70	// Buffer of 70 clusters in case of attempt to access >630

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

class Upd765 : public Cdp1802
{
public:
	Upd765();
	~Upd765() {};

	void configureUpd765(int fdcType);
	Byte efInterrupt();
	Byte inputMasterStatus();
    void outputCommand(Byte value);
    Byte inputCommandStatus(void);
    void outputDmaCounter(Byte value);
    void outputDmaControl(Byte value);
    
    void cycleUpd765();
    void setDiskName(int disk, wxString dirName, wxString fileName);
    void initializeCat(int drive);
    
protected:
	ClusterInfo clusterInfo_[4][MAX_CLUSTER + BUFFER_CLUSTER];
	int hdCommand_;
    bool resetHdData_;

private:
    void doCommand();
    void doRead();
    void doWrite();
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
    int commandReturnCounter__;
    Byte interrupt_;
    Word dmaCounter_;
    Byte dmaControl_;
    Byte commandPacket_[16];
    int pcn;
    int commandPacketIndex;
    
    long fdcCycles_;
    Word diskBufferPos_;
    int drive_;
    int offset_;
    
    int updActivity_;
    int sectorsPerTrack_;
    int fdcType_;
};

#endif  // UPD_H

