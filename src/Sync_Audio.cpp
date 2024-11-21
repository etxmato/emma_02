
// Blip_Buffer 0.4.0. http://www.slack.net/~ant/

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
#include "Sync_Audio.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define AUDIO_SAMPLERATE        11025 
#define AUDIO_SAMPLES            1024
#define AUDIO_CHANNELS            2
#define AUDIO_FORMAT            AUDIO_S16
#define CHUNK_SIZE                512

enum { buf_size = 1024*2 };

/* Copyright(C) 2005-2006 by Shay Green. Permission is hereby granted, free of
charge, to any person obtaining a copy of this software module and associated
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

// Return current SDL_GetError() string, or str if SDL didn't have a string

static const char* sdl_error( const char* str )
{
    const char* sdl_str = SDL_GetError();
    if ( sdl_str && *sdl_str )
        str = sdl_str;
    return str;
}

Sync_Audio::Sync_Audio()
{
    buf_count = 2;
    play_bufs = 0;
    free_sem = 0;
    record_free_sem = 0;
    play_open = 0;

    record_bufs = 0;
    record_open = 0;
    set_gain( 1.0 );
    record_on = false;
    waveFileOpen_ = false;
    inputHalted_ = false;
    inputPlaying_ = false;
    record_buf_size = 1024*1024;
}

Sync_Audio::~Sync_Audio()
{
    stop();
}

const char* Sync_Audio::play_start( int sample_rate, int latency )
{
    play_write_buf = 0;
    play_write_pos = 0;
    play_read_buf = 0;

    long sample_latency = latency * sample_rate * AUDIO_CHANNELS / 1000;
    buf_count = (int)(sample_latency / buf_size);
    if ( buf_count < 2 )
        buf_count = 2;
    
    play_bufs = (play_sample_t*) malloc((long) buf_size * buf_count * sizeof *play_bufs );
    if ( !play_bufs )
        return "out of memory";
    
    free_sem = SDL_CreateSemaphore( buf_count - 1 );
    if ( !free_sem )
        return sdl_error( "Couldn't create semaphore" );
    
    SDL_AudioSpec as;
    as.freq = sample_rate;
    as.format = AUDIO_S16SYS;
    as.channels = AUDIO_CHANNELS;
    as.silence = 0;
    as.samples = (Uint16) (buf_size / AUDIO_CHANNELS);
    as.size = 0;
    as.callback = play_fill_buffer_;
    as.userdata = this;
    if ( SDL_OpenAudio( &as, 0 ) < 0 )
        return sdl_error( "Couldn't open SDL audio" );
    SDL_PauseAudio( 0 ); 
    play_open = 1;
    
    return 0; // success
}

void Sync_Audio::stop()
{
    record_on = false;

    if ( play_open )
        SDL_PauseAudio( 1 );
#if defined (__WXMSW__)
    if ( record_open )
        SDL_PauseAudioIn( 1 );
#endif

    if ( play_open )
    {
        play_open = 0;
        SDL_CloseAudio();
    }
    
#if defined (__WXMSW__)
    if ( record_open )
    {
        record_open = 0;
        SDL_QuitAudioIn();
    }
#endif

    if ( free_sem )
    {
        SDL_DestroySemaphore( free_sem );
        free_sem = 0;
    }
    
    if ( record_free_sem )
    {
        SDL_DestroySemaphore( record_free_sem );
        record_free_sem = 0;
    }
    
    free( record_bufs );
    record_bufs = 0;

    free( play_bufs );
    play_bufs = 0;
}
/*
int Sync_Audio::sample_count() const
{
    if ( !free_sem )
        return 0;
    
    int buf_free = SDL_SemValue( free_sem ) * buf_size +(buf_size - play_write_pos);
    return buf_size * buf_count - buf_free;
}*/

inline Sync_Audio::play_sample_t* Sync_Audio::play_buf( int index )
{
    assert((unsigned) (index < buf_count) );
    return play_bufs +(long) index * buf_size;
}

inline Sync_Audio::record_sample_t* Sync_Audio::record_buf( int index )
{
    assert((unsigned) (index < buf_count) );
    return record_bufs +(long) index * record_buf_size;
}

void Sync_Audio::play_write( const play_sample_t* in, int remain )
{
    short value;
    record_sample_t* record_in;

    while( remain )
    {
        int count = buf_size - play_write_pos;
        if ( count > remain )
            count = remain;
        
        play_sample_t* out = play_buf( play_write_buf ) + play_write_pos;
        if ( gain != (1L << gain_bits) )
        {
            register long gain = this->gain;
            for ( int n = count; n--; )
                *out++ = (play_sample_t) ((*in++ * gain) >> gain_bits);
        }
        else
        {
            if (record_on && inputPlaying_)
            {
                record_in = record_buf( record_read_buf ) + record_read_pos;
                for ( int n = count; n--; )
                {
                    if (channel_ == inputChannel_)
                    {
//                        if (playback_)
//                            *out++ = *in++ + (*record_in * (4*audioInGain_));
//                        else
                        *out++ = *in++;
                        value = (short)*record_in++;
                        p_Computer->realCassette(value); 
//                        if (waveFileOpen_)
//                            psaveWavePointer->write(value, 1);
                    }
                    else
                    {
                        *out++ = *in++;
                        record_in++;
                    }
                    channel_ = !channel_;
                    record_read_pos ++;
                    if ( record_read_pos >= record_buf_size )
                    {
                        record_read_pos = 0;
                        record_read_buf = (record_read_buf + 1) % buf_count;
                    }
                    if (record_read_pos == record_write_pos)
                    {
                        inputHalted_ = true;
                        SDL_SemWait( free_sem );
                        if (inputHalted_)
                            SDL_SemWait( free_sem );
                    }
                }
            }
            else
            {
                memcpy( out, in, count * sizeof(play_sample_t) );
                in += count;
            }
        }
        
        play_write_pos += count;
        remain -= count;
        
        if ( play_write_pos >= buf_size )
        {
            play_write_pos = 0;
            play_write_buf = (play_write_buf + 1) % buf_count;
            SDL_SemWait( free_sem );
        }
    }
}
/*
void Sync_Audio::play_fill_buffer( Uint8* out, int byte_count )
{
        memcpy( out, play_buf( play_read_buf ), byte_count );
        play_read_buf = (play_read_buf + 1) % buf_count;
}*/

void Sync_Audio::Play_fill_buffer( Uint8* out, int byte_count )
{ 
    if ( SDL_SemValue( free_sem ) <(Uint32) (buf_count - 1) )
    {
        memcpy( out, play_buf( play_read_buf ), byte_count );
        play_read_buf = (play_read_buf + 1) % buf_count;
        SDL_SemPost( free_sem );
    }
    else
    {
        memset( out, 0, byte_count );
    }
}

void Sync_Audio::Record_fill_buffer( Uint8* in, int byte_count )
{
    if (inputHalted_)
    {
        SDL_SemPost( free_sem );
        inputHalted_ = false;
    }

    memcpy( record_buf( record_write_buf )+ record_write_pos, in, byte_count);
    for (int i=0; i<byte_count/2; i++)
    {
        record_write_pos++;
        if ( record_write_pos >= record_buf_size )
        { 
            record_write_pos = 0;
            record_write_buf = (record_write_buf + 1) % buf_count;
        }
        if (record_write_pos == record_read_pos)
        {
            record_buf_size *= 2;
            record_bufs = (record_sample_t*) realloc( record_bufs, (size_t)(record_buf_size * buf_count * sizeof *record_bufs));
//            p_Main->message("buffer overflow");
//            p_Main->messageInt((int)(record_buf_size/1024));
        }
    }
    inputPlaying_ = true;
}

void Sync_Audio::play_fill_buffer_( void* user_data, Uint8* out, int byte_count )
{
    ((Sync_Audio*) user_data)->Play_fill_buffer( out, byte_count );
}

void Sync_Audio::setAudioInGain( double gain)
{
    audioInGain_ = gain;
}

const char* Sync_Audio::record_start( int sample_rate , double gain, bool inputChannel)
{
#if defined (__WXMSW__)

    SDL_AudioSpec expected;
    audioInGain_ = gain;
    inputChannel_ = inputChannel;
    sampleRate_ = sample_rate;

    record_write_buf = 0;
    record_write_pos = 0;
    record_read_buf = 0;
    record_read_pos = 0;
    record_bufs = (record_sample_t*) malloc( (size_t)(record_buf_size * buf_count * sizeof *record_bufs ));
    if ( !record_bufs )
        return "out of memory";

    record_free_sem = SDL_CreateSemaphore( buf_count - 1 );
    if ( !record_free_sem )
        return sdl_error( "Couldn't create semaphore" );
    
    expected.format=AUDIO_S16LSB;
    expected.freq=sample_rate;
    expected.callback=record_fill_buffer_;
    expected.samples=buf_size/AUDIO_CHANNELS;
    expected.channels=AUDIO_CHANNELS;
    expected.userdata=this;

    if(SDL_OpenAudioIn(&expected, 0)<0)
    {
//        SDL_QuitAudioIn();
        return sdl_error( "Couldn't open SDL audio" );
    }

    record_open = 1;
#endif
    return 0; // success
}

void Sync_Audio::record_pause (bool status)
{
#if defined (__WXMSW__)
    if (status)
    {
        record_write_pos = 0;
        record_read_pos = 0;
        record_write_buf = 0;
        record_read_buf = 0;
        inputHalted_ = false;
        inputPlaying_ = false;
        channel_ = false;
        SDL_PauseAudioIn(0);
    }
    else
    {
        SDL_PauseAudioIn(1); 
        p_Computer->resetGaugeValue();
    }
//    SDL_Delay((512 * 1000) / 11025);
    record_on = status;
#endif
}

void Sync_Audio::record_fill_buffer_ ( void* user_data, Uint8* stream, int byte_count )
{
    ((Sync_Audio*) user_data)->Record_fill_buffer( stream, byte_count );
}

void Sync_Audio::setTapeConfig(bool inputChannel, bool playback) 
{
    inputChannel_ = inputChannel;
    playback_ = playback;
}
