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

WaveReader::WaveReader()
{
}

WaveReader::~WaveReader()
{
    inFile_.Close();
}

long WaveReader::openFile(wxString fileName)
{
    long rate = 0;

    if (inFile_.Open(fileName, "rb"))
    {
        unsigned char Riff [12];
        inFile_.Read(Riff, 12);
        long fiePosition = 12;
        long dataPosition = 0;

        long fileSize = Riff [4] +(Riff [5]<<8) +(Riff [6]<<16) +(Riff [7]<<24);

        while(fiePosition < fileSize)
        {
            unsigned char chunkId [4];
            inFile_.Read(chunkId, 4);

            unsigned char chunkSize [4];
            inFile_.Read(chunkSize, 4);
            fiePosition += 8;

            long longChunkSize = chunkSize [0] +(chunkSize [1]<<8) +(chunkSize [2]<<16) +(chunkSize [3]<<24);

            if ((chunkId[0] == 'd') &&(chunkId[1] == 'a') &&(chunkId[2] == 't') &&(chunkId[3] == 'a'))
            {
                dataPosition = fiePosition;
                fiePosition += longChunkSize;
                inFile_.Seek(fiePosition, wxFromStart);
            }
            else
            {
                unsigned char* chunkData;
                chunkData = (unsigned char*) malloc( longChunkSize * sizeof *chunkData );
    
                fiePosition += longChunkSize;
                inFile_.Read(chunkData, longChunkSize);
    
                if ((chunkId[0] == 'f') &&(chunkId[1] == 'm') &&(chunkId[2] == 't') &&(chunkId[3] == ' '))
                {
                    int compressionCode = chunkData [0] +(chunkData [1]<<8);
                    if (compressionCode != 1)
                        p_Main->errorMessage("Wave file error: Only PCM files are supported");

                    channelCount_ = chunkData [2] +(chunkData [3]<<8);
                    if (channelCount_ > 2)
                        p_Main->errorMessage("Wave file error: Only mono and stereo files are supported");

                    rate = chunkData [4] +(chunkData [5]<<8) +(chunkData [6]<<16) +(chunkData [7]<<24);
                    frameSize_ = chunkData [12] +(chunkData [13]<<8);
                }
                free (chunkData);
            }
        }
        inFile_.Seek(dataPosition, wxFromStart);
    }
    return rate;
}

long WaveReader::read(sample_t* outBuffer, size_t remaining, float gain)
{
    unsigned char buffer [1024];
    char dataByte = 0, audioByte = 0;
    short dataWord = 0, audioWord = 0;
    wxUint32 data24 = 0, audio24 = 0;

    size_t in = inFile_.Read(buffer, remaining*frameSize_);
    if (in == wxInvalidOffset)
    {
        in = 0;
        p_Main->message("Wave file error: Read error");
    }
    in = in/frameSize_;
    remaining = in;

    unsigned char* bufferPointer = &buffer [0];

    switch (frameSize_/channelCount_)
    {
        case 3:
            while(in--)
            {
                if (p_Computer->isAudioChannelLeft())
                {
                    audio24 = *bufferPointer++;
                    audio24 += *bufferPointer++ << 8;
                    audio24 += *bufferPointer-- << 16;
                    bufferPointer--;
                    *outBuffer++ = (short)audio24*gain/2;
                }
                if (p_Computer->isDataChannelLeft())
                {
                    data24 = *bufferPointer++;
                    data24 += *bufferPointer++ << 8;
                    data24 += *bufferPointer-- << 16;
                    bufferPointer--;
                }
                bufferPointer += frameSize_/2;

                if (!p_Computer->isAudioChannelLeft()) // audio channel right
                {
                    audio24 = *bufferPointer++;
                    audio24 += *bufferPointer++ << 8;
                    audio24 += *bufferPointer-- << 16;
                    bufferPointer--;
                    *outBuffer++ = (short)audio24*gain/2;
                }
                if (!p_Computer->isDataChannelLeft()) // data channel right
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
                if (p_Computer->isAudioChannelLeft())
                {
                    audioWord = *bufferPointer++;
                    audioWord += *bufferPointer-- << 8;
                    *outBuffer++ = (short)audioWord*gain/2;
                }
                if (p_Computer->isDataChannelLeft())
                {
                    dataWord = *bufferPointer++;
                    dataWord += *bufferPointer-- << 8;
                }
                bufferPointer += frameSize_/2;

                if (!p_Computer->isAudioChannelLeft()) // audio channel right
                {
                    audioWord = *bufferPointer++;
                    audioWord += *bufferPointer-- << 8;
                    *outBuffer++ = (short)audioWord*gain/2;
                }
                if (!p_Computer->isDataChannelLeft()) // data channel right
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
                if (p_Computer->isAudioChannelLeft())
                {
                    audioByte = *bufferPointer;
                    *outBuffer++ = (short) ((audioByte ^ 0x80) << 8)*gain/2;
                }
                if (p_Computer->isDataChannelLeft())
                {
                    dataByte = *bufferPointer;
                }
                bufferPointer += frameSize_/2;

                if (!p_Computer->isAudioChannelLeft()) // audio channel right
                {
                    audioByte = *bufferPointer;
                    *outBuffer++ = (short) ((audioByte ^ 0x80) << 8)*gain/2;
                }
                if (!p_Computer->isDataChannelLeft()) // data channel right
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
    return remaining;
}

void WaveReader::seek(wxFileOffset ofs)
{
    inFile_.Seek(ofs*frameSize_);
    p_Computer->stepCassetteCounter((long)ofs);
}

bool WaveReader::eof()
{
    return inFile_.Eof();
}
