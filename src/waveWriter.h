
// WAVE sound file writer for recording 16-bit output during program development

#ifndef WAVE_WRITER_H
#define WAVE_WRITER_H

#include <stddef.h> 
#include <stdio.h>

class WaveWriter {
public:    
    // Create sound file with given sample rate(in Hz) and fileName.
    WaveWriter( long sampleRate, int bitsPerSample);
    bool openFile(char const* fileName = "out.wav" );

    // Enable stereo output
    void stereo( int );
    
    // Append 'count' samples to file. Use every 'skip'th source sample; allows
    // one channel of stereo sample pairs to be written by specifying a skip of 2.
    void write( const wxInt16*, long count, int skip = 1 );
    void write( const unsigned char*, long count, int skip = 1 );
    
    // Number of samples written so far
    long sample_count() const;
    
    // Write sound file header and close file. If no samples were written,
    void closeFile();

    // deletes buf.
    ~WaveWriter();
    
private:
    enum { bufferSize_ = 32768 * 2 };
    unsigned char* buffer_;
    FILE*   file_;
    long    sampleCount_;
    long    sampleRate_;
    long    bufferPosition_;
    int     channelCount_;
    int        bitsPerSample_;

    void flush();
};

inline void WaveWriter::stereo( int s ) {
    channelCount_ = s ? 2 : 1;
}

inline long WaveWriter::sample_count() const {
    return sampleCount_;
}

#endif

