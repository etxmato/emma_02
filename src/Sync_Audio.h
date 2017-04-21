
// Simple synchronous sound interface for SDL multimedia library

#ifndef SYNC_AUDIO_H
#define SYNC_AUDIO_H

#include "SDL.h"
#include "waveWriter.h"

// Simple SDL sound wrapper that has a synchronous interface
class Sync_Audio {
	enum { gain_bits = 16 };
public:
	// Initialize with specified sample rate, channel count, and latency.
	// Returns NULL on success, otherwise error string.
	const char* play_start( int sample_rate, int latency_msec = 200 );
	const char* record_start( int sample_rate, double gain, bool inputChannel);
	void setTapeConfig(bool inputChannel, bool playback);
	void setAudioInGain( double gain);
	void record_pause( bool status );
	void wavFile();

	// Set gain, where 1.0 leaves sound unaltered
	void set_gain( double g ) { gain = (long) (g *(1L << gain_bits)); }
	
	// Number of samples in buffer waiting to be played
	int sample_count() const;
//	int record_sample_count() const;
	
	// Write samples to buffer, first waiting until enough space is available
//	typedef uint8_t Uint8;
	typedef short play_sample_t;
	typedef short record_sample_t;
	void play_write( const play_sample_t*, int count );
	void startTape();

	// Stop audio output
	void stop();
	
	Sync_Audio();
	~Sync_Audio();
	
private:
	SDL_sem* volatile free_sem;
	SDL_sem* volatile record_free_sem;
	int volatile buf_count;
	long gain;

	play_sample_t* volatile play_bufs;
	int volatile play_read_buf;
	int play_write_buf;
	int play_write_pos;
	int play_open;
	double audioInGain_;

	record_sample_t* volatile record_bufs; 
	int volatile record_read_buf;
	int record_write_buf;
	int record_read_pos;
	int record_write_pos;
	int record_open;

	play_sample_t* play_buf( int index );
	void Play_fill_buffer( Uint8*, int );
	static void play_fill_buffer_( void*, Uint8*, int );

	record_sample_t* record_buf( int index );
	void Record_fill_buffer( Uint8*, int );
	static void record_fill_buffer_( void*, Uint8*, int );

	bool record_on;
	bool channel_;
	bool inputChannel_;
	bool playback_;

	WaveWriter *psaveWavePointer;
	bool waveFileOpen_;
	long long record_buf_size;
	bool inputHalted_;
	bool inputPlaying_;
	long sampleRate_;
};

#endif

