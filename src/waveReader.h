#ifndef WAVEREADER_H
#define WAVEREADER_H

#include <stddef.h>  
#include <stdio.h>
#include "wx/ffile.h"

class WaveReader {
public:
    WaveReader();
    ~WaveReader();

    long openFile(wxString fileName);
    bool createFile(wxString fileName, long sampleRate, int bitsPerSample);
    long readHeader();

    // Enable stereo output
    void stereo( int );

    // Append 'count' samples to file. Use every 'skip'th source sample; allows
    // one channel of stereo sample pairs to be written by specifying a skip of 2.
    void write(wxInt16 InBuffer);
    void write(unsigned char InBuffer);
    
    // Number of samples written so far
    long sample_count() const;

    long read(wxInt16* OutBuffer, size_t Count, float Gain);
    bool eof();
    bool seek(wxFileOffset ofs, wxSeekMode mode = wxFromStart);
    long rewind(long step);

    // Write sound file header and close file. If no samples were written,
    void closeFile();
    
    void flush();
    void writeHeader();

private:
    wxFFile wavFile_;
    int channelCount_;
    int frameSize_;

    enum { bufferSize_ = 32768 * 2 };
    unsigned char* buffer_;
    FILE*   file_;
    long sampleCount_;
    long sampleRate_;
    long fileSize_;
    long sizeOfSampleData_;
    long dataPosition_;
    int bitsPerSample_;
    wxString fileName_;
};

inline void WaveReader::stereo( int s ) {
    channelCount_ = s ? 2 : 1;
}

inline long WaveReader::sample_count() const {
    return sampleCount_;
}

#endif

