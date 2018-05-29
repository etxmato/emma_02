#ifndef SOUND_H
#define SOUND_H

#include "waveWriter.h"
#include "waveReader.h"
#include "Blip_Buffer.h"
#include "Sync_Audio.h"
#if defined (__WXMSW__) || defined(__linux__)
#include "SDL_audioin.h"
#endif
#include "computer.h"

/* Structure for loaded sounds. */
typedef struct sound_s
{
    Uint8 *samples;		/* raw PCM sample data */
    Uint32 length;		/* size of sound data in bytes */
} sound_t, *sound_p;

class Sound: public Computer
{
public: 

	Sound();
	~Sound();

	void initSound(double clock, double percentageClock, int computerType, int volume, int bass, int treble, int toneChannels, int stereo, bool realCasLoad, int beepFrequency, int bellFrequency);
	void tone(short reg4);  
	void tone1864Latch(Byte audioLatch1864);
	void tone1864On();  
	void setVipSound (int vipSound){vipSound_ = vipSound;};
	void amplitudeSuper(int channel, int amplitude);
	void frequencySuper(int channel, int frequency);
	void octaveSuper(int channel, int octave);
	void toneSuper();  
	void beepOn();
    void bellOn();
    void keyClickOn();
	void beepOnStudio();
	void beepOff();
	void changeBeepFrequency(int frequency);
	void toneElf2KOn();
	void toneElf2KOff();
	void noise(short reg5); 
	void soundCycle();

	void playSound();
	void playSoundBuffer();
    void psaveAmplitudeChange(int q);
    void psaveAmplitudeZero();
	void playSaveLoad();
	void convertTo8Bit(const short* in, int count, unsigned char* out);
    bool ploadStartTape(wxString fileName);
    void startWavSound(wxString fileName);
    void readWav(Uint8* inputBuffer, short* outBuffer, size_t remaining, float gain);
    int LoadAndConvertSound(wxString filename, SDL_AudioSpec *spec);
	void psaveStartTape(wxString fileName);
    void stopTape();
    void pauseTape();
    void restartTapeSave();
    void restartTapeLoad();
    void stopPausedLoad();
    void stopPausedSave();
    void stopSaveLoad();
    void setVolume(int volume);
	void setClockRate(double clock);
	void setPercentageClock(double percentage);
	bool isSaving();
	bool isLoading();
	void setEqualization(int bass, int treble);
	void setSoundFollowQ(bool status) {followQ_ = status;};
	void record_pause(bool status);
	void wavFile();
	void setPsaveSettings();

protected:
	Byte flipFlopQ_;
	int vipSound_;
	bool realCassetteLoad_;
	long cpuCycles_;
	double soundClock_;
	double percentageClock_;
    int computerType_;

private:
	Blip_Buffer *soundBufferPointerLeft;
	Blip_Buffer *soundBufferPointerRight;
	Blip_Buffer *tapeBufferPointer;
	Blip_Synth<blip_high_quality,30> *toneSynthPointer[8];
	Blip_Synth<blip_high_quality,30> *noiseSynthPointer[2];
	Blip_Synth<blip_high_quality,30> *psaveSynthPointer[2];
	Blip_Synth<blip_high_quality,30> *tapeSynthPointer;
	Sync_Audio *audioPointer;

	WaveWriter *psaveWavePointer;
    WaveReader *ploadWavePointer;
    WaveReader *wavSoundPointer;
    
	int soundTime_;
	float gain_;

	int tonePeriod_[8];
	int toneTime_[8];
	bool toneOn_[8];
    int toneAmplitude_[8];

	int noisePeriod_;
	int noiseTime_;
	bool noiseOn_;
	int noiseAmplitude_;

	bool stopTheTape_;
	bool psaveOn_;
    bool ploadOn_;
    bool wavOn_;
	int psaveAmplitude_;
	int psaveVolume_;
	int psaveBitRate_;
	int psaveBitsPerSample_;
	bool followQ_;
	int beepPeriod_;
	int beepFrequency_;
	int bellFrequency_;

    int decayPeriod_;
    int targetPeriod_;

    int decayTime_;
    int saveDecayPeriod_;

	Byte audioLatch1864_;

	Byte frequencySuper_[8];
	int octaveSuper_[8];

	int toneChannels_;
	int stereo_;
	bool inputChannel_;
	bool studioBeep_;
};

#endif  // SOUND_H
