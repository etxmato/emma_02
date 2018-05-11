#ifndef WAVEREADER_H
#define WAVEREADER_H

#include <stddef.h>  
#include <stdio.h>
#include "wx/ffile.h"

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
};

#endif

