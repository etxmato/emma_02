/*
 *******************************************************************
 *** This software is copyright 2008 by Marcel van Tongeren      ***
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

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

#if !wxUSE_COMBOCTRL
    #error "Please set wxUSE_COMBOCTRL to 1 and rebuild the library."
#endif

#include "waveReader.h"
#include "main.h"

const int header_size = 0x2C;

WaveReader::WaveReader()
{
}

WaveReader::~WaveReader()
{
    wavFile_.Close();
}

long WaveReader::openFile(wxString fileName)
{
    if (wavFile_.Open(fileName, "rb+"))
        return readHeader();

    return 0;
}

bool WaveReader::createFile(wxString fileName, long sampleRate, int bitsPerSample)
{
    sampleCount_ = 0;
    sampleRate_ = sampleRate;
    bufferPosition_ = header_size;
    bitsPerSample_ = bitsPerSample;
    stereo( 0 );

    return wavFile_.Open(fileName, "wb");
}

long WaveReader::readHeader()
{
    sampleRate_ = 0;

    unsigned char Riff [12];
    wavFile_.Read(Riff, 12);
    long fiePosition = 12;
    dataPosition_ = 0;

    long fileSize_ = Riff [4] +(Riff [5]<<8) +(Riff [6]<<16) +(Riff [7]<<24);

    while(fiePosition < fileSize_)
    {
        unsigned char chunkId [4];
        wavFile_.Read(chunkId, 4);

        unsigned char chunkSize [4];
        wavFile_.Read(chunkSize, 4);
        fiePosition += 8;

        long longChunkSize = chunkSize [0] +(chunkSize [1]<<8) +(chunkSize [2]<<16) +(chunkSize [3]<<24);

        if ((chunkId[0] == 'd') &&(chunkId[1] == 'a') &&(chunkId[2] == 't') &&(chunkId[3] == 'a'))
        {
            sizeOfSampleData_ = longChunkSize;
            dataPosition_ = fiePosition;
            fiePosition += longChunkSize;
            wavFile_.Seek(fiePosition, wxFromStart);
        }
        else
        {
            unsigned char* chunkData;
            chunkData = (unsigned char*) malloc( longChunkSize * sizeof *chunkData );

            fiePosition += longChunkSize;
            wavFile_.Read(chunkData, longChunkSize);

            if ((chunkId[0] == 'f') &&(chunkId[1] == 'm') &&(chunkId[2] == 't') &&(chunkId[3] == ' '))
            {
                int compressionCode = chunkData [0] +(chunkData [1]<<8);
                if (compressionCode != 1)
                    p_Main->errorMessage("Wave file error: Only PCM files are supported");

                channelCount_ = chunkData [2] +(chunkData [3]<<8);
                if (channelCount_ > 2)
                    p_Main->errorMessage("Wave file error: Only mono and stereo files are supported");

                sampleRate_ = chunkData [4] +(chunkData [5]<<8) +(chunkData [6]<<16) +(chunkData [7]<<24);
                frameSize_ = chunkData [12] +(chunkData [13]<<8);
                bitsPerSample_ = chunkData [14];
            }
            free (chunkData);
        }
    }
    wavFile_.Seek(dataPosition_, wxFromStart);

    return sampleRate_;
}

long WaveReader::read(sample_t* outBuffer, size_t remaining, float gain)
{
    unsigned char buffer [1024];
    char dataByte = 0, audioByte = 0;
    short dataWord = 0, audioWord = 0;
    wxUint32 data24 = 0, audio24 = 0;

    size_t in = wavFile_.Read(buffer, remaining*frameSize_);
    if (in == wxInvalidOffset)
    {
        in = 0;
        p_Main->message("Wave file error: Read error");
    }
    in = in/frameSize_;
    remaining = in;

    unsigned char* bufferPointer = &buffer [0];

    bool dataChannelLeft, audioChannelLeft;
    
    if (channelCount_ == 1)
    {
        dataChannelLeft = true;
        audioChannelLeft = true;
    }
    else
    {
        dataChannelLeft = p_Computer->isDataChannelLeft();
        audioChannelLeft = p_Computer->isAudioChannelLeft();
    }
    
    switch (frameSize_/channelCount_)
    {
        case 3:
            while(in--)
            {
                if (audioChannelLeft)
                {
                    audio24 = *bufferPointer++;
                    audio24 += *bufferPointer++ << 8;
                    audio24 += *bufferPointer-- << 16;
                    bufferPointer--;
                    *outBuffer++ = (short)audio24*gain/2;
                }
                if (dataChannelLeft)
                {
                    data24 = *bufferPointer++;
                    data24 += *bufferPointer++ << 8;
                    data24 += *bufferPointer-- << 16;
                    bufferPointer--;
                }
                bufferPointer += frameSize_/2;

                if (!audioChannelLeft) // audio channel right
                {
                    audio24 = *bufferPointer++;
                    audio24 += *bufferPointer++ << 8;
                    audio24 += *bufferPointer-- << 16;
                    bufferPointer--;
                    *outBuffer++ = (short)audio24*gain/2;
                }
                if (!dataChannelLeft) // data channel right
                {
                    data24 = *bufferPointer++;
                    data24 += *bufferPointer++ << 8;
                    data24 += *bufferPointer-- << 16;
                    bufferPointer--;
                }
                bufferPointer += frameSize_/2;
            }
            p_Computer->cassette(data24);
            p_Computer->cassetteFred(data24);
            p_Computer->cassetteXmlHw(data24, remaining);
        break;
            
        case 2:
            while(in--)
            {
                if (audioChannelLeft)
                {
                    audioWord = *bufferPointer++;
                    audioWord += *bufferPointer-- << 8;
                    *outBuffer++ = (short)audioWord*gain/2;
                }
                if (dataChannelLeft)
                {
                    dataWord = *bufferPointer++;
                    dataWord += *bufferPointer-- << 8;
                }
                bufferPointer += frameSize_/2;

                if (!audioChannelLeft) // audio channel right
                {
                    audioWord = *bufferPointer++;
                    audioWord += *bufferPointer-- << 8;
                    *outBuffer++ = (short)audioWord*gain/2;
                }
                if (!dataChannelLeft) // data channel right
                {
                    dataWord = *bufferPointer++;
                    dataWord += *bufferPointer-- << 8;
                }
                bufferPointer += frameSize_/2;
            }
            p_Computer->cassette(dataWord);
            p_Computer->cassetteFred(dataWord);
            p_Computer->cassetteXmlHw(dataWord, remaining);
        break;
            
        case 1:
            while(in--)
            {
                if (audioChannelLeft)
                {
                    audioByte = *bufferPointer;
                    *outBuffer++ = (short) ((audioByte ^ 0x80) << 8)*gain/2;
                }
                if (dataChannelLeft)
                {
                    dataByte = *bufferPointer;
                }
                bufferPointer += frameSize_/2;

                if (!audioChannelLeft) // audio channel right
                {
                    audioByte = *bufferPointer;
                    *outBuffer++ = (short) ((audioByte ^ 0x80) << 8)*gain/2;
                }
                if (!dataChannelLeft) // data channel right
                {
                    dataByte = *bufferPointer;
                }
                bufferPointer += frameSize_/2;
            }
            p_Computer->cassette(dataByte);
            p_Computer->cassetteFred(dataByte);
            p_Computer->cassetteXmlHw(dataByte, remaining);
        break;
    }
    sampleCount_ += remaining;
    return remaining;
}

void WaveReader::write(sample_t inBuffer)
{  
    frameSize_ = channelCount_ * sizeof(sample_t);

    if (channelCount_ == 1)
    {
        wavFile_.Seek (dataPosition_ + sampleCount_ * frameSize_);
        wavFile_.Write(&inBuffer, frameSize_);
    }
    else
    {
        if (p_Computer->isDataChannelLeft())
        {
            wavFile_.Seek (dataPosition_ + sampleCount_ * frameSize_);
            wavFile_.Write(&inBuffer, frameSize_/2);
        }
        else
        {
            wavFile_.Seek (dataPosition_ + sampleCount_ * frameSize_ + frameSize_/2);
            wavFile_.Write(&inBuffer, frameSize_/2);
        }
    }

    sampleCount_++;
}

void WaveReader::write(unsigned char inBuffer)
{
    frameSize_ = channelCount_ * sizeof(sample_t);

    if (channelCount_ == 1)
    {
        wavFile_.Seek (dataPosition_ + sampleCount_ * frameSize_);
        wavFile_.Write(&inBuffer, frameSize_);
    }
    else
    {
        if (p_Computer->isDataChannelLeft())
        {
            wavFile_.Seek (dataPosition_ + sampleCount_ * frameSize_);
            wavFile_.Write(&inBuffer, frameSize_/2);
        }
        else
        {
            wavFile_.Seek (dataPosition_ + sampleCount_ * frameSize_ + frameSize_/2);
            wavFile_.Write(&inBuffer, frameSize_/2);
        }
    }

    sampleCount_++;
}

void WaveReader::closeFile()
{
    long ds, rs, bps;

    if (bitsPerSample_ == 8)
    {
        // generate header
        ds = sampleCount_ * sizeof(unsigned char) * channelCount_;
        rs = header_size - 8 + ds;
        bps = sampleRate_ * frameSize_;
    }
    else
    {
        // generate header
        ds = sampleCount_ * sizeof(sample_t) * channelCount_;
        rs = header_size - 8 + ds;
        bps = sampleRate_ * frameSize_;
    }

    if (rs < fileSize_)
        rs = fileSize_;
    
    if (ds < sizeOfSampleData_)
        ds = sizeOfSampleData_;
    
    unsigned char header [header_size] = {
        'R','I','F','F',
        rs,rs>>8,           // length of rest of file
        rs>>16,rs>>24,
        'W','A','V','E',
        'f','m','t',' ',
        0x10,0,0,0,         // size of fmt chunk
        1,0,                // uncompressed format
        channelCount_,0,       // channel count
        sampleRate_,sampleRate_ >> 8,     // sample rate
        sampleRate_>>16,sampleRate_>>24,
        bps,bps>>8,         // bytes per second
        bps>>16,bps>>24,
        frameSize_,0,       // bytes per sample frame
        bitsPerSample_, 0,   // bits per sample
        'd','a','t','a',
        ds,ds>>8,ds>>16,ds>>24// size of sample data
        // ...              // sample data
    };

    // write header
    wavFile_.Seek(0);
    wavFile_.Write(header, sizeof header);

    wavFile_.Close();
}

bool WaveReader::seek(wxFileOffset ofs, wxSeekMode mode)
{
    return wavFile_.Seek(ofs, mode);
}

bool WaveReader::eof()
{
    return wavFile_.Eof();
}
