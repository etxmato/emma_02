#ifndef WAVEREADER_H
#define WAVEREADER_H

#include <stddef.h>  
#include <stdio.h>
#include "wx/ffile.h"

#define SOUND_HIGH 0
#define SOUND_MID 1
#define SOUND_LOW 2

#define SOUND_UP 2
#define SOUND_DOWN 1
#define SOUND_SILENT 0

class WaveReader {
public:
	typedef short sample_t;
	
	WaveReader();
	~WaveReader();

	long openFile(wxString fileName);
	long read(sample_t* OutBuffer, size_t Count, float Gain);
	bool eof();

private:
	wxFFile inFile_;
	int channelCount_;
	int frameSize_;
    
    size_t length_;
    size_t lengthMid_;
    size_t lengthHigh_;
    size_t lengthLow_;
    
    int currentState_;
    short lastSample_;
    int pulseCount_;
    int bitValue_;
    
};

#endif

