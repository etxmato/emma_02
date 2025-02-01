#ifndef SOUND_H
#define SOUND_H

#include "waveWriter.h"
#include "waveReader.h"
#include "Blip_Buffer.h"
#include "Sync_Audio.h"
#if defined (__WXMSW__) || defined(__linux__)
#include "SDL_audioin.h"
#endif
#include "panel.h"

#define MAX_NUMBER_OF_TONE_CHANNELS 12
#define MAX_NUMBER_OF_NOISE_CHANNELS 12

/* Structure for loaded sounds. */
typedef struct sound_s
{
    Uint8 *samples;        /* raw PCM sample data */
    Uint32 length;        /* size of sound data in bytes */
} sound_t, *sound_p;

class Sound 
{
public:

    Sound();
    ~Sound();

    void initSound(double percentageClock, int bass, int treble, int toneChannels, int noiseChannels, int stereo, ComputerConfiguration computerConfiguration);

    void setTonePeriod(int channel, int period, bool toneOn);
    void setToneFrequency(int channel, int frequency, bool toneOn);
    void setToneAmplitude(int channel, int amplitude, bool toneOn);
    void startTone(int channel, bool toneOn);
    void startToneDecay(int channel, int startFrequency, int endFrequecny, int decay);
    void startQTone(int channel, bool toneOn);
    
    void setNoisePeriod(int channel, int period);
    void setNoiseAmplitude(int channel, int amplitude);
    int startNoise(int channel, bool noiseOn, int noisePeriod);

    void setEnvelopePeriod(int period);
    void ayEnvelopeActive(int channel, bool envelopeActive);
    void ayEnvelopeContinues(bool envelopeContinues);
    void ayEnvelopeAttack(bool envelopeAttack);
    void ayEnvelopeAlternate(bool envelopeAlternate);
    void ayEnvelopeHold(bool envelopeHold);
    
    void amplitudeSuper(int channel, int amplitude);
    void frequencySuper(int channel, int frequency);
    void octaveSuper(int channel, int octave);
    void toneSuper();  
    void toneSuperOff();
    void changeBeepFrequency(int frequency);
    void soundCycle();

    void toneSoundCycle(int channel);
    void noiseSoundCycle(int channel, int noisePeriod);
    void playSound();
    void playSoundBuffer();
    void psaveAmplitudeChange(int q);
    void psaveAmplitudeZero();
    void playSaveLoad();
    void convertTo8Bit(const short* in, int count, unsigned char* out);
    bool ploadStartTape(wxString fileName, wxString tapeNumber);
    void startWavSound(wxString fileName);
    void readWav(Uint8* inputBuffer, short* outBuffer, size_t remaining, float gain);
    int LoadAndConvertSound(wxString filename, SDL_AudioSpec *spec);
    void startSaveTape(wxString fileName, wxString tapeNumber);
    void startSaveTapeHw(wxString fileName, wxString tapeNumber);
    void outSaveTapeHw(Byte value);
    void writeSilenceTapeHw();
    int writeSaveTapeHw(Byte value, Byte stopBits);
    void stopTape();
    void pauseTape();
    void restartTapeSave(int tapeState);
    void restartHwTapeSave(int tapeState);
    void restartTapeLoad(int tapeState);
    void stopPausedLoad();
    void stopPausedSave();
    void stopSaveLoad();
    void forwardTape(int tapeState);
    void rewindTape(int tapeState);
    void setVolume(int volume);
    void setClockRate(double clock);
    void setPercentageClock(double percentage);
    bool isSaving();
    bool isLoading();
    void setEqualization(int bass, int treble);
    void record_pause(bool status);
    void setPsaveSettings();

    void setSoundType(int status) {activeSoundType_ = status;};
    bool getAudioInStatus() {return audioIn_;};

protected:
    Byte flipFlopQ_;
    bool realCassetteLoad_;
    uint64_t cpuCycles_;
    uint64_t instructionCounter_;
    double soundClock_;
    double percentageClock_;
    char threshold8_;
    wxInt16 threshold16_;
    wxInt32 threshold24_;
    float fredFreq_;
    long sampleRate_;
    int forwardSpeed_;
    double remainingForwardSpeed_;
    int rewindSpeed_;
    double remainingRewindSpeed_;
    bool useXmlThreshold_;

    Byte tapeHwReadyToReceive_;
    long long tapeCounterStep_;
    long long tapePeriod_;
    bool tapeRecording_;
    long stopTapeCounter_;
    wxString tapeNumber_;

    int activeSoundType_;

private:
    ComputerConfiguration computerConfiguration_;
    short ploadSamples_[256];

    Blip_Buffer *soundBufferPointerLeft;
    Blip_Buffer *soundBufferPointerRight;
    Blip_Buffer *tapeBufferPointer;
    Blip_Synth<blip_high_quality,30> *toneSynthPointer[MAX_NUMBER_OF_TONE_CHANNELS];
    Blip_Synth<blip_high_quality,30> *noiseSynthPointer[MAX_NUMBER_OF_NOISE_CHANNELS];
    Blip_Synth<blip_high_quality,30> *psaveSynthPointer[2];
    Blip_Synth<blip_high_quality,30> *tapeSynthPointer;
    Sync_Audio *audioPointer;

    WaveWriter *psaveWavePointer;
    WaveReader *ploadWavePointer;
    WaveReader *wavSoundPointer;
    
    int soundTime_;
    float gain_;

    int tonePeriod_[MAX_NUMBER_OF_TONE_CHANNELS];
    int toneTime_[MAX_NUMBER_OF_TONE_CHANNELS];
    bool toneOn_[MAX_NUMBER_OF_TONE_CHANNELS];
    int toneAmplitude_[MAX_NUMBER_OF_TONE_CHANNELS];

    int noisePeriod_[MAX_NUMBER_OF_NOISE_CHANNELS];
    int noiseTime_[MAX_NUMBER_OF_NOISE_CHANNELS];
    bool noiseOn_[MAX_NUMBER_OF_NOISE_CHANNELS];
    int noiseAmplitude_[MAX_NUMBER_OF_NOISE_CHANNELS];
    
    bool envelopeActive_[MAX_NUMBER_OF_TONE_CHANNELS];
    int envelopePeriod_;
    int envelopeAmplitude_;
    bool envelopeContinues_;
    int envelopeAttack_;
    bool envelopeAlternate_;
    bool envelopeHold_;

    bool stopTheTape_;
    bool hwSaveOn_;
    bool startNewRecording_;
    bool somethingSaved_;
    bool ploadPaused_;
    bool hwSavePaused_;
    bool psaveOn_;
    bool ploadOn_;
    bool forwardOn_;
    bool rewindOn_;
    bool wavOn_;
    int psaveAmplitude_;
    int psaveVolume_;
    int psaveBitRate_;
    int psaveBitsPerSample_;

    int beepPeriod_[MAX_NUMBER_OF_TONE_CHANNELS];
    int decayPeriod_[MAX_NUMBER_OF_TONE_CHANNELS];
    int targetPeriod_[MAX_NUMBER_OF_TONE_CHANNELS];
    int decayTime_[MAX_NUMBER_OF_TONE_CHANNELS];
    int saveDecayPeriod_[MAX_NUMBER_OF_TONE_CHANNELS];
    bool decayBeep_[MAX_NUMBER_OF_TONE_CHANNELS];

    Byte frequencySuper_[8];
    int octaveSuper_[8];

    int toneChannels_;
    int noiseChannels_;
    int stereo_;
    bool inputChannel_;
    
    Byte tapeHwOutputValue_;
    int numberOfSamples_;
    
    bool audioIn_;
    bool audioOut_;
};

#endif  // SOUND_H
