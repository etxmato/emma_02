
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

#include "main.h"

// Blip_Buffer 0.4.0. http://www.slack.net/~ant/

#include "waveWriter.h" 

#include <assert.h>
#include <stdlib.h>

/* Copyright(C) 2003-2006 by Shay Green. Permission is hereby granted, free
of charge, to any person obtaining a copy of this software and associated
documentation files(the "Software", to deal in the Software without
restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and
to permit persons to whom the Software is furnished to do so, subject to the
following conditions: The above copyright notice and this permission notice
shall be included in all copies or substantial portions of the Software. THE
SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

const int header_size = 0x2C;

WaveWriter::WaveWriter(long sampleRate, int bitsPerSample)
{
	sampleCount_ = 0;
	sampleRate_ = sampleRate;
	bufferPosition_ = header_size;
	bitsPerSample_ = bitsPerSample;
	stereo( 0 );
}

bool WaveWriter::openFile(const char* fileName)
{
	buffer_ = (unsigned char*) malloc( bufferSize_ * sizeof *buffer_ );
	if ( !buffer_ )
		return false;
	
	file_ = fopen( fileName, "wb" );
	if ( !file_ )
		return false;
	
	return true;
}

void WaveWriter::flush()
{
	if ( bufferPosition_ && !fwrite( buffer_, bufferPosition_, 1, file_ ) )
		p_Main->message( "Wave file error: Couldn't write data" );
	bufferPosition_ = 0;
}

void WaveWriter::write( const sample_t* in, long remain, int skip )
{
	sampleCount_ += remain;
	while( remain )
	{
		if ( bufferPosition_ >= bufferSize_ )
			flush();
		
		long n = (unsigned long) (bufferSize_ - bufferPosition_) / sizeof(sample_t);
		if ( n > remain )
			n = remain;
		remain -= n;
		
		// convert to lsb first format
		unsigned char* p = &buffer_ [bufferPosition_];
		while( n-- )
		{
			int s = *in;
			in += skip;
			*p++ = (unsigned char) s;
			*p++ = (unsigned char) (s >> 8);
		}
		
		bufferPosition_ = p - buffer_;
		assert( bufferPosition_ <= bufferSize_ );
	}
}

void WaveWriter::write( const unsigned char* in, long remain, int skip )
{
	sampleCount_ += remain;
	while( remain )
	{
		if ( bufferPosition_ >= bufferSize_ )
			flush();
		
		long n = (unsigned long) (bufferSize_ - bufferPosition_) / sizeof(unsigned char);
		if ( n > remain )
			n = remain;
		remain -= n;
		
		// convert to lsb first format
		unsigned char* p = &buffer_ [bufferPosition_];
		while( n-- )
		{
			unsigned char s = *in;
			in += skip;
			*p++ = (unsigned char) s;
		}
		
		bufferPosition_ = p - buffer_;
		assert( bufferPosition_ <= bufferSize_ );
	}
}

void WaveWriter::closeFile()
{
	flush();
	
	long ds, rs, bps;
	int frame_size;

	if (bitsPerSample_ == 8)
	{
		// generate header
		ds = sampleCount_ * sizeof(unsigned char);
		rs = header_size - 8 + ds;
		frame_size = channelCount_ * sizeof(unsigned char);
		bps = sampleRate_ * frame_size;
	}
	else
	{
		// generate header
		ds = sampleCount_ * sizeof(sample_t);
		rs = header_size - 8 + ds;
		frame_size = channelCount_ * sizeof(sample_t);
		bps = sampleRate_ * frame_size;
	}

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
		frame_size,0,       // bytes per sample frame
		bitsPerSample_, 0,   // bits per sample
		'd','a','t','a',
		ds,ds>>8,ds>>16,ds>>24// size of sample data
		// ...              // sample data
	};

	// write header
	fseek( file_, 0, SEEK_SET );
	fwrite( header, sizeof header, 1, file_ );
	
	fclose( file_ );
}

WaveWriter::~WaveWriter()
{
	free( buffer_ );
}

