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

#include "main.h"
#include "sound.h"

#if defined(__linux__)
#define CLOCK_FACTOR 2
#define SAMPLE_RATE 44100
#else
#define CLOCK_FACTOR 1
#define SAMPLE_RATE 44100
#endif

Sound::Sound()
{
    soundBufferPointerLeft = new Blip_Buffer();
    soundBufferPointerRight = new Blip_Buffer();
    tapeBufferPointer = new Blip_Buffer();
    audioPointer = new Sync_Audio();

    for (int i=0; i<MAX_NUMBER_OF_TONE_CHANNELS; i++)
    {
        toneSynthPointer[i] = new Blip_Synth<blip_high_quality,30>;
        frequencySuper_[i] = 0;
        toneAmplitude_[i] = 0;
        selectedToneAmplitude_[i] = 0;
        octaveSuper_[i] = 4;
        tonePeriod_[i] = 32;
        decayBeep_[i] = false;
    }
    for (int i=0; i<MAX_NUMBER_OF_NOISE_CHANNELS; i++)
    {
        noiseSynthPointer[i] = new Blip_Synth<blip_high_quality,30>;
        noisePeriod_[i] = 0;
    }
    for (int i=0; i<2; i++)
        psaveSynthPointer[i] = new Blip_Synth<blip_high_quality,30>;
    tapeSynthPointer = new Blip_Synth<blip_high_quality,30>;

    activeSoundType_ = SOUND_EXT_BEEPER;
    audioIn_ = false;
}

Sound::~Sound()
{
    if (hwSaveOn_ || hwSavePaused_)
    {
        if (tapeHwReadyToReceive_ == 1)
            writeSaveTapeHw(tapeHwOutputValue_, 1);

        ploadWavePointer->closeFile();
        delete ploadWavePointer;
        p_Main->eventHwTapeStateChange(HW_TAPE_STATE_OFF);
    }

    delete soundBufferPointerLeft;
    delete soundBufferPointerRight;
    delete tapeBufferPointer;
    delete audioPointer;
    for (int i=0; i<MAX_NUMBER_OF_TONE_CHANNELS; i++)
        delete toneSynthPointer[i];
    for (int i=0; i<MAX_NUMBER_OF_NOISE_CHANNELS; i++)
        delete noiseSynthPointer[i];
    for (int i=0; i<2; i++)
        delete psaveSynthPointer[i];
    delete tapeSynthPointer;

#if defined (__WXMSW__) || defined(__linux__)
    if (audioIn_)
        SDL_CloseAudioIn();
#endif
}

void Sound::initSound(double percentageClock, int bass, int treble, int toneChannels, int noiseChannels, int stereo, ComputerConfiguration computerConfiguration)
{
    soundClock_ = computerConfiguration.clockSpeed_;
    percentageClock_ = percentageClock;
    computerConfiguration_ = computerConfiguration;
    if ((percentageClock_*soundClock_) < 0.000045)  percentageClock_ = (double) 0.000045/soundClock_;
    gain_ = (float) computerConfiguration.soundConfiguration.volume / 100;
    stereo_ = stereo;
    realCassetteLoad_ = computerConfiguration.realCassetteLoad_;
    audioIn_ = true;
    audioOut_ = true;
    tapeNumber_ = "";

    setPsaveSettings();

    for (int i=0; i<MAX_NUMBER_OF_TONE_CHANNELS; i++)
    {
        envelopeActive_[i] = false;
        toneOn_[i] = false;
        decayTime_[i] = 0;
    }
    for (int i=0; i<MAX_NUMBER_OF_NOISE_CHANNELS; i++)
        noiseOn_[i] = false;
    
    envelopePeriod_ = 0;
    envelopeAmplitude_ = 0xf;
    envelopeContinues_ = true;
    envelopeAttack_ = 1;
    envelopeAlternate_ = true;
    envelopeHold_ = true;

    psaveOn_ = false;
    ploadOn_ = false;
    hwSaveOn_ = false;
    startNewRecording_ = true;
    somethingSaved_ = false;
    hwSavePaused_ = false;
    ploadPaused_ = false;
    forwardOn_ = false;
    rewindOn_ = false;
    wavOn_ = false;
    stopTheTape_ = false;
    tapeHwReadyToReceive_ = 0;
    numberOfSamples_ = 0;
    soundTime_ = 0;

    setToneAmplitude(0, 8, false);
    setToneAmplitude(1, 8, false);

    psaveAmplitude_ = 0;

    tapeBufferPointer->clock_rate((soundClock_ * 1000000));
    tapeSynthPointer->volume(1);
    tapeSynthPointer->output(tapeBufferPointer);

#if defined (__WXMAC__)
    audioIn_ = false;
#else
    int result = SDL_Init(SDL_INIT_AUDIO);
    if (result < 0)
        p_Main->message("SDL audio initialization failed");
    atexit(SDL_Quit);

    result = SDL_InitAudioIn();
    if(result < 0)
    {
        p_Main->message("SDL audio input initialization failed");
        audioIn_ = false;
    }
#endif

    setClockRate(computerConfiguration.clockSpeed_);
    if (soundBufferPointerLeft->set_sample_rate(SAMPLE_RATE))
        p_Main->message("Sound error: out of memory");
    if (soundBufferPointerRight->set_sample_rate(SAMPLE_RATE))
        p_Main->message("Sound error: out of memory");

    soundBufferPointerLeft->bass_freq(bass);
    soundBufferPointerRight->bass_freq(bass);
    if (stereo_ == 2)
    {
        toneChannels_ = toneChannels;
        noiseChannels_ = noiseChannels;
        toneSynthPointer[0]->treble_eq(treble);
        toneSynthPointer[0]->volume(gain_);
        toneSynthPointer[0]->output(soundBufferPointerLeft);
        toneSynthPointer[1]->treble_eq(treble);
        toneSynthPointer[1]->volume(gain_);
        toneSynthPointer[1]->output(soundBufferPointerRight);
        toneSynthPointer[2]->treble_eq(treble);
        toneSynthPointer[2]->volume(gain_);
        toneSynthPointer[2]->output(soundBufferPointerLeft);
        toneSynthPointer[3]->treble_eq(treble);
        toneSynthPointer[3]->volume(gain_);
        toneSynthPointer[3]->output(soundBufferPointerRight);
        noiseSynthPointer[0]->treble_eq(treble);
        noiseSynthPointer[0]->volume(gain_);
        noiseSynthPointer[0]->output(soundBufferPointerLeft);
        noiseSynthPointer[1]->treble_eq(treble);
        noiseSynthPointer[1]->volume(gain_);
        noiseSynthPointer[1]->output(soundBufferPointerRight);
    }
    else
    {
        toneChannels_ = toneChannels * 2;
        noiseChannels_ = noiseChannels * 2;
        for (int i=0; i<toneChannels_; i+=2)
        {
            toneSynthPointer[i]->treble_eq(treble);
            toneSynthPointer[i]->volume(gain_);
            toneSynthPointer[i]->output(soundBufferPointerLeft);
            toneSynthPointer[i+1]->treble_eq(treble);
            toneSynthPointer[i+1]->volume(gain_);
            toneSynthPointer[i+1]->output(soundBufferPointerRight);
        }
        for (int i=0; i<noiseChannels_; i+=2)
        {
            noiseSynthPointer[i]->treble_eq(treble);
            noiseSynthPointer[i]->volume(gain_);
            noiseSynthPointer[i]->output(soundBufferPointerLeft);
            noiseSynthPointer[i+1]->treble_eq(treble);
            noiseSynthPointer[i+1]->volume(gain_);
            noiseSynthPointer[i+1]->output(soundBufferPointerRight);
        }
    }

    for (int i=0; i<2; i++)
    {
        psaveSynthPointer[i]->treble_eq(treble);
        psaveSynthPointer[i]->volume(gain_);
    }
    psaveSynthPointer[0]->output(soundBufferPointerLeft);
    psaveSynthPointer[1]->output(soundBufferPointerRight);

    if (audioPointer->play_start(SAMPLE_RATE, 100) != 0)
    {
        if (p_Main->showSoundError())
        {
            int answer = wxMessageBox("SDL audio output could not be started\nCorrect audio configuration to run Emma 02 with correct speed\n\nShow this error next time?",
                "Emma 02", wxICON_ERROR | wxYES_NO);

            if (answer == wxNO)
            {
                p_Main->hideSoundError();
            }
        }
        else
            p_Main->message("SDL audio output could not be started");

        audioOut_ = false;
    }
#if defined (__WXMSW__) || defined(__linux__)
    if (audioPointer->record_start(SAMPLE_RATE, gain_, inputChannel_) != 0)
    {
        p_Main->message("SDL audio input could not be started\n");
        audioIn_ = false;
    }
#endif
    
    for (int i=0; i<toneChannels_; i++)
        toneSynthPointer[i]->update(soundTime_, 0);
    for (int i=0; i<noiseChannels_; i++)
        noiseSynthPointer[i]->update(soundTime_, 0);
    for (int i=0; i<2; i++)
        psaveSynthPointer[i]->update(soundTime_, 0);
    if (audioIn_)
        record_pause(computerConfiguration.realCassetteLoad_);
    else
    {
        if (realCassetteLoad_)
            p_Main->setRealCasOff();
        realCassetteLoad_ = false;
    }
}

void Sound::setTonePeriod(int channel, int period, bool toneOn)
{
    tonePeriod_[channel] = period;
    startTone(channel, toneOn);
}

void Sound::setToneFrequency(int channel, int frequency, bool toneOn)
{
    tonePeriod_[channel] = (int)((soundClock_ * 1000000) / frequency / 2);
    startTone(channel, toneOn);
}

void Sound::setToneAmplitude(int channel, int amplitude, bool toneOn)
{
    /*if (!toneOn_[channel])

    {
        if (toneAmplitude_[channel] >= 0)
            toneAmplitude_[channel] = amplitude;
        else
            toneAmplitude_[channel] = -(amplitude);
    }
    else
        toneAmplitude_[channel] = amplitude;
    */
    selectedToneAmplitude_[channel] = amplitude;
    if (selectedToneAmplitude_[channel] == 0)
        toneOn_[channel] = false;
    else
        startTone(channel, toneOn);
}

void Sound::startTone(int channel, bool toneOn)
{
    if (!toneOn || selectedToneAmplitude_[channel] == 0)
        toneOn_[channel] = false;
    else
     {
        if (!toneOn_[channel])
        {
            toneTime_[channel] = selectedToneAmplitude_[channel];
            toneSynthPointer[channel]->update(soundTime_, selectedToneAmplitude_[channel]);
        }
        toneOn_[channel] = true;
     }
}

void Sound::startToneDecay(int channel, int startFrequency, int endFrequecny, int decay)
{
    tonePeriod_[channel] = (int)((soundClock_ * 1000000) / startFrequency / 2);
    targetPeriod_[channel] = (int)((soundClock_ * 1000000) / endFrequecny / 2);

    toneTime_[channel] = tonePeriod_[channel] * 4 / 3;
    beepPeriod_[channel] = tonePeriod_[channel];
    
    decayTime_[channel] = decay;
    decayPeriod_[channel] = (decay / (targetPeriod_[channel] - beepPeriod_[channel]))*8;
    saveDecayPeriod_[channel] = decayPeriod_[channel];
    decayBeep_[channel] = true;
    startTone(channel, true);
}

void Sound::startQTone(int channel, bool toneOn)
{
    toneSynthPointer[channel]->update(soundTime_, toneAmplitude_[channel]);
    toneOn_[channel] = toneOn;
}

void Sound::setNoisePeriod(int channel, int period)
{
    noisePeriod_[channel] = period;
}

void Sound::setNoiseAmplitude(int channel, int amplitude)
{
    noiseAmplitude_[channel] = amplitude;
    
    if (noiseAmplitude_[channel] == 0)
        noiseOn_[channel] = false;
}

int Sound::startNoise(int channel, bool noiseOn, int previousNoisePeriod)
{
    int noisePeriod = -1;
    
    if (!noiseOn || noiseAmplitude_[channel] == 0)
        noiseOn_[channel] = false;
    else
    {
        if (!noiseOn_[channel])
        {
            if (previousNoisePeriod != -1)
                noiseTime_[channel] = previousNoisePeriod;
            else
                noiseTime_[channel] = noisePeriod_[channel]*((rand() % 7) + 1);
            noiseSynthPointer[channel]->update(soundTime_, noiseAmplitude_[channel]);
        }
        noiseOn_[channel] = true;
    }
    return noisePeriod;
}

void Sound::setEnvelopePeriod(int period)
{
    envelopePeriod_ = period;
}

void Sound::ayEnvelopeActive(int channel, bool envelopeActive)
{
    envelopeActive_[channel] = envelopeActive;
    selectedToneAmplitude_[channel] = envelopeAmplitude_;
    envelopeTime_[channel] = envelopePeriod_;
}

void Sound::ayEnvelopeShape(Byte envelopeShape)
{
    envelopeShape_ = envelopeShape;
}

void Sound::ayEnvelopeContinues(bool envelopeContinues)
{
    envelopeContinues_ = envelopeContinues;
}

void Sound::ayEnvelopeAttack(bool envelopeAttack)
{
    envelopeAttack ? envelopeAmplitude_ = 0 : envelopeAmplitude_ = 0xf;
    envelopeAttack ? envelopeAttack_ = 1 : envelopeAttack_ = -1;
}

void Sound::ayEnvelopeAlternate(bool envelopeAlternate)
{
    envelopeAlternate_ = envelopeAlternate;
}

void Sound::ayEnvelopeHold(bool envelopeHold)
{
    envelopeHold_ = envelopeHold;
}

void Sound::amplitudeSuper(int channel, int amplitude)
{
    if (stereo_ == 2)
    {
    /*    if (toneAmplitude_[channel] < 0)
            toneAmplitude_[channel] = -(amplitude & 0xf);
        else
            toneAmplitude_[channel] = amplitude & 0xf;*/
            selectedToneAmplitude_[channel] = amplitude & 0xf;
    }
    else
    {
        /*if (toneAmplitude_[channel*2] < 0)
            toneAmplitude_[channel*2] = toneAmplitude_[channel*2+1] = -(amplitude & 0xf);
        else
            toneAmplitude_[channel*2] = toneAmplitude_[channel*2+1] = amplitude & 0xf;*/
        selectedToneAmplitude_[channel*2] = amplitude & 0xf;
    }
}

void Sound::frequencySuper(int channel, int frequency)
{
    if (stereo_ == 2)
    {
        frequencySuper_[channel] = frequency;
        tonePeriod_[channel] = (32 * octaveSuper_[channel] * (frequencySuper_[channel] + 1));
    }
    else
    {
        frequencySuper_[channel*2] = frequencySuper_[channel*2+1] = frequency;
        tonePeriod_[channel*2] = tonePeriod_[channel*2+1] = (32 * octaveSuper_[channel*2] * (frequencySuper_[channel*2] + 1));
    }
}

void Sound::octaveSuper(int channel, int octave)
{
    if (stereo_ == 2)
    {
        octaveSuper_[channel] = ((octave&0x3) ^ 3) + 1;
        tonePeriod_[channel] = (32 * octaveSuper_[channel] * (frequencySuper_[channel] + 1));
    }
    else
    {
        octaveSuper_[channel*2] = octaveSuper_[channel*2+1] = ((octave&0x3) ^ 3) + 1;
        tonePeriod_[channel*2] = tonePeriod_[channel*2+1] = (32 * octaveSuper_[channel*2] * (frequencySuper_[channel*2] + 1));
    }
}

void Sound::toneSuper()
{
    for (int i=0; i<toneChannels_; i++)
    {
        if (!toneOn_[i])
        {
            toneTime_[i] = tonePeriod_[i];
            toneSynthPointer[i]->update(soundTime_, selectedToneAmplitude_[i]);
        }
        toneOn_[i] = true;
    }
}

void Sound::toneSuperOff()
{
    for (int i=0; i<toneChannels_; i++)
        toneOn_[i] = false;
}

void Sound::soundCycle()
{
    for (int channel=0; channel<toneChannels_; channel++)
    {
        if (toneOn_[channel])
            toneSoundCycle(channel);
        if (envelopeActive_[channel])
            envelopeSoundCycle(channel);
    }
    for (int channel=0; channel<noiseChannels_; channel+=2)
    {
        int noisePeriod = noisePeriod_[channel]*((rand() % 7) + 1);
        if (noiseOn_[channel])
            noiseSoundCycle(channel, noisePeriod);
        if (noiseOn_[channel+1])
            noiseSoundCycle(channel+1, noisePeriod);
    }
    soundTime_+=8;
    playSound();
}

void Sound::toneSoundCycle(int channel)
{
    if (decayTime_[channel] > 0)
    {
        decayTime_[channel]-=8;
        decayPeriod_[channel]-=8;
        
        if (decayPeriod_[channel] <= 0)
        {
            beepPeriod_[channel] += 8;
            tonePeriod_[channel] = beepPeriod_[channel];
            decayPeriod_[channel] = saveDecayPeriod_[channel];
        }
    }
    
    toneTime_[channel]-=8;
    if (toneTime_[channel] <= 0)
    {
        if (decayBeep_[channel])
        {
            if (toneAmplitude_[channel] > 0)
            {
                toneTime_[channel] = tonePeriod_[channel] * 2 / 3;
                toneAmplitude_[channel] = -8;
            }
            else
            {
                toneTime_[channel] = tonePeriod_[channel] * 4 / 3;
                toneAmplitude_[channel] = 4;
            }
        }
        else
        {
            toneTime_[channel] = tonePeriod_[channel];
            if (toneAmplitude_[channel] >= 0)
                toneAmplitude_[channel] = -selectedToneAmplitude_[channel];
            else
                toneAmplitude_[channel] = selectedToneAmplitude_[channel];
        }
        toneSynthPointer[channel]->update(soundTime_, toneAmplitude_[channel]);
    }
}

void Sound::envelopeSoundCycle(int channel)
{
    envelopeTime_[channel]-=8;
    if (envelopeTime_[channel] <= 0)
    {
        envelopeAmplitude_ = envelopeAmplitude_ + envelopeAttack_;
        if ((envelopeAmplitude_ & 0xf0) != 0)
        {
            switch (envelopeShape_)
            {
                case ENVELOPE_SHAPE_DOWN_ZERO_1:    // 0
                case ENVELOPE_SHAPE_UP_ZERO_1:      // 4
                case ENVELOPE_SHAPE_DOWN_ZERO_2:    // 9
                case ENVELOPE_SHAPE_UP_ZERO_2:      // F
                    envelopeActive_[channel] = false;
                    envelopeAmplitude_ = 0;
                break;
                    
                case ENVELOPE_SHAPE_DOWN_MAX:       // B
                case ENVELOPE_SHAPE_UP_MAX:         // D
                    envelopeActive_[channel] = false;
                    envelopeAmplitude_ = 0xf;
                break;

                case ENVELOPE_SHAPE_DOWN_CONT:      // 8
                    envelopeAmplitude_ = 0xf;
                break;

                case ENVELOPE_SHAPE_DOWN_CONT_ALT:  // A
                case ENVELOPE_SHAPE_UP_CONT_ALT:    // B
                    envelopeAttack_ = -envelopeAttack_;
                break;

                case ENVELOPE_SHAPE_UP_CONT:        // C
                    envelopeAmplitude_ = 0xf;
                break;
            }
        }
        
        selectedToneAmplitude_[channel] = envelopeAmplitude_;
        if (selectedToneAmplitude_[channel] != 0)
            toneOn_[channel] = true;

        envelopeTime_[channel] = envelopePeriod_;
    }
}

void Sound::noiseSoundCycle(int channel, int noisePeriod)
{
    noiseTime_[channel]-=8;
    if (noiseTime_[channel] <= 0)
    {
        noiseTime_[channel] = noisePeriod;
        noiseAmplitude_[channel] = -noiseAmplitude_[channel];
        noiseSynthPointer[channel]->update(soundTime_, noiseAmplitude_[channel]);
    }
}

void Sound::playSound()
{
    if (psaveOn_ || ploadOn_ || wavOn_ || hwSaveOn_)
        return;

    playSoundBuffer();
}

void Sound::playSoundBuffer()
{
    soundBufferPointerLeft->end_frame(soundTime_);
    soundBufferPointerRight->end_frame(soundTime_);

    int const soundBufferSize = 2048;
    short samples [soundBufferSize];
    while(true)
    {
        // Read as many samples as possible
        int sample_count = (int)soundBufferPointerLeft->read_samples(samples, soundBufferSize/2, 1);
        sample_count = (int)soundBufferPointerRight->read_samples(samples + 1, sample_count, 1);

        // Stop if no more samples are avialable
        if (sample_count == 0)
            break;

        // Play the samples
        if (audioOut_)
            audioPointer->play_write(samples, sample_count*2);
    }
    soundTime_ = 0;
}

void Sound::psaveAmplitudeChange(int q)
{
    if (!psaveOn_)
    {
        if (realCassetteLoad_)
        {
            if (q == 0)
                psaveAmplitude_ = psaveVolume_;
            else
                psaveAmplitude_ = -psaveVolume_;
            psaveSynthPointer[0]->update(soundTime_,(int)(psaveAmplitude_/2));
            psaveSynthPointer[1]->update(soundTime_,(int)(psaveAmplitude_/2));
        }
        else
        {
            switch (activeSoundType_)
            {
                case SOUND_Q_SW:
                    startQTone(0, q);
                    startQTone(0, q);
                break;

                case SOUND_EXT_BEEPER:
                    if (q)
                    {
                        setToneFrequency(0, computerConfiguration_.soundConfiguration.beepFrequency, true);
                        setToneFrequency(1, computerConfiguration_.soundConfiguration.beepFrequency, true);
                    }
                    else
                    {
                        startTone(0, false);
                        startTone(1, false);
                    }
                break;

                case SOUND_1863_1864:
                    if (q)
                    {
                        startTone(0, true);
                        startTone(1, true);
                    }
                    else
                    {
                        startTone(0, false);
                        startTone(1, false);
                    }
                break;

                case SOUND_STUDIO:
                    if (q)
                    {
                        startToneDecay(0, computerConfiguration_.soundConfiguration.beepFrequency,  computerConfiguration_.soundConfiguration.targetBeepFrequency, computerConfiguration_.soundConfiguration.decay);
                        startToneDecay(1, computerConfiguration_.soundConfiguration.beepFrequency,  computerConfiguration_.soundConfiguration.targetBeepFrequency, computerConfiguration_.soundConfiguration.decay);
                    }
                   else
                   {
                       startTone(0, false);
                       startTone(1, false);
                   }
                break;

                case SOUND_SUPER_VP550:
                case SOUND_SUPER_VP551:
                    if (q)
                        toneSuper();
                    else
                        toneSuperOff();
                break;
            }
            p_Computer->printOutPecom(q);
        }
    }
    else
    {
        if (q == 0)
            psaveAmplitude_ = psaveVolume_;
        else
            psaveAmplitude_ = -psaveVolume_;
        tapeSynthPointer->update(soundTime_, psaveAmplitude_);
        psaveSynthPointer[0]->update(soundTime_,(int)(psaveAmplitude_/2));
        psaveSynthPointer[1]->update(soundTime_,(int)(psaveAmplitude_/2));
    }
}

void Sound::psaveAmplitudeZero()
{
    tapeSynthPointer->update(soundTime_, 0);
    psaveSynthPointer[0]->update(soundTime_, 0);
    psaveSynthPointer[1]->update(soundTime_, 0);
}

void Sound::playSaveLoad()
{
    if (stopTheTape_)
    {
        if (p_Main->isTapeHwCybervision())
        {
 //           resetTape();
            pauseTape();
            if (somethingSaved_)
                ploadWavePointer->flush();
            p_Main->eventHwTapeStateChange(HW_TAPE_STATE_OFF);
            stopTheTape_ = false;
            somethingSaved_ = false;
        }
        else
            stopTape();
        return;
    }

    if (!psaveOn_ && !ploadOn_ && !wavOn_ && !hwSaveOn_)
        return;

    tapeBufferPointer->end_frame(soundTime_);

    int const soundBufferSize = 256;
    short samples [soundBufferSize];

    while(true)
    {
        // Read as many samples as possible
        int sample_count = (int)tapeBufferPointer->read_samples(samples, soundBufferSize);

        // Stop if no more samples are avialable
        if (sample_count == 0)
        {
            break;
        }

        if (hwSaveOn_)
        {
            numberOfSamples_ += sample_count;
            if (tapeHwReadyToReceive_ == 1)
            {
                if (numberOfSamples_ > (sampleRate_/401))
                {
                    if (startNewRecording_)
                    {
                        numberOfSamples_ -= writeSaveTapeHw(0xF8, 4);
                        numberOfSamples_ -= writeSaveTapeHw(0xBD, 1);
                        startNewRecording_ = false;
                    }
                    numberOfSamples_ -= writeSaveTapeHw(tapeHwOutputValue_, 1);
                }
            }
            else
            {
                while (numberOfSamples_ > 0)
                {
                    writeSilenceTapeHw();
                    numberOfSamples_--;
                }
            }
        }

        if (forwardOn_)
        {
            if (ploadWavePointer->eof())
            {
                if (p_Main->isTapeHwCybervision())
                    pauseTape();
                else
                    stopTape();
            }
            else
            {
                long in = ploadWavePointer->read(ploadSamples_, sample_count*forwardSpeed_, gain_);
                if (ploadOn_)
                {
                    soundBufferPointerLeft->mix_samples(ploadSamples_, in);
                    soundBufferPointerRight->mix_samples(ploadSamples_, in);
                }
            }
        }

        if (rewindOn_)
        {
            sample_count = (int)ploadWavePointer->rewind(rewindSpeed_);
            p_Computer->stepCassetteCounter(-rewindSpeed_);

            if (sample_count <= 0)
            {
                if (p_Main->isTapeHwCybervision())
                    pauseTape();
                else
                    stopTape();
            }
        }

        if (ploadOn_ && !forwardOn_ && !rewindOn_)
        {
            if (ploadWavePointer->eof())
            {
                if (p_Main->isTapeHwCybervision())
                    pauseTape();
                else
                    stopTape();
            }
            else
            {
                long in = ploadWavePointer->read(ploadSamples_, sample_count, gain_);
                if (ploadOn_)
                {
                    soundBufferPointerLeft->mix_samples(ploadSamples_, in);
                    soundBufferPointerRight->mix_samples(ploadSamples_, in);
                }
            }
        }

        if (wavOn_)
        {
            long in = wavSoundPointer->read(ploadSamples_, sample_count, gain_);
            soundBufferPointerLeft->mix_samples(ploadSamples_, in);
            soundBufferPointerRight->mix_samples(ploadSamples_, in);
            if (wavSoundPointer->eof())
            {
                delete wavSoundPointer;
                wavOn_ = false;
            }
        }
        
        if (psaveOn_)
        {
            if (stopTapeCounter_ > 0)
            {
                stopTapeCounter_--;
                if (stopTapeCounter_ == 0)
                {
                    p_Computer->stopTape();
                    return;
                }
            }

            if (psaveBitsPerSample_ == 0)
            {
                unsigned char samples8bit [soundBufferSize/2];
                convertTo8Bit(samples, sample_count, samples8bit);
                psaveWavePointer->write(samples8bit, sample_count);
            }
            else
                psaveWavePointer->write(samples, sample_count);
        }
        
    }
    playSoundBuffer();
}

void Sound::convertTo8Bit(const short* in, int count, unsigned char* out)
{
    for (int i=0; i<count; i++)
        out[i]= (unsigned char) ((in[i] >> 8) ^ 0x80);
}

bool Sound::ploadStartTape(wxString fileName, wxString tapeNumber)
{
    tapeNumber_ = tapeNumber;
    
    ploadWavePointer = new WaveReader();
    
    sampleRate_ = ploadWavePointer->openFile(fileName);
    if (sampleRate_ != 0)
    {
        if (tapeBufferPointer->set_sample_rate(sampleRate_))
        {
            p_Main->message("Cassette sound error: out of memory");
            delete ploadWavePointer;
            p_Main->eventSetTapeState(TAPE_STOP, tapeNumber_);
            return false;
        }
        else
        {
            ploadOn_ = true;
            p_Main->turboOn();
            return true;
        }
    }
    else
    {
        delete ploadWavePointer;
        p_Main->eventSetTapeState(TAPE_STOP, tapeNumber_);
        return false;
    }
}

void Sound::startWavSound(wxString fileName)
{
    wavSoundPointer = new WaveReader();
    
    sampleRate_ = wavSoundPointer->openFile(fileName);
    if (sampleRate_ != 0)
    {
        if (tapeBufferPointer->set_sample_rate(sampleRate_))
        {
            p_Main->message("Wav sound error: out of memory");
            delete wavSoundPointer;
        }
        else
            wavOn_ = true;
    }
    else
        delete wavSoundPointer;
}

void Sound::startSaveTape(wxString fileName, wxString tapeNumber)
{
    tapeNumber_ = tapeNumber;

    sampleRate_ = 22050;
    switch (psaveBitRate_)
    {
        case 0:
            sampleRate_ = 11025;
        break;
        case 1:
            sampleRate_ = 22050;
        break;
        case 2:
            sampleRate_ = 44100;
        break;
        case 3:
            sampleRate_ = 88200;
        break;
    }
    if (tapeBufferPointer->set_sample_rate(sampleRate_))
        p_Main->message("Cassette sound error: out of memory");
    if (psaveBitsPerSample_ == 0)
        psaveWavePointer = new WaveWriter(sampleRate_, 8);
    else
        psaveWavePointer = new WaveWriter(sampleRate_, 16);

    if (psaveWavePointer->openFile(fileName))
    {
        psaveOn_ = true;
        stopTapeCounter_ = 0;
        p_Main->turboOn();
    }
    else
    {
        p_Main->message("Cassette sound error: Can't open file");
        delete psaveWavePointer;
        p_Main->eventSetTapeState(TAPE_STOP, tapeNumber_);
    }
}

void Sound::startSaveTapeHw(wxString fileName, wxString tapeNumber)
{
    tapeNumber_ = tapeNumber;
    
    if (ploadPaused_)
    {
        hwSaveOn_ = true;
        tapeRecording_ = true;
        ploadPaused_ = false;
        return;
    }
    
    if (hwSavePaused_)
    {
        hwSaveOn_ = true;
        tapeRecording_ = true;
        hwSavePaused_ = false;
        return;
    }

    if (ploadOn_)
    {
        pauseTape();
        hwSaveOn_ = true;
        tapeRecording_ = true;
        return;
    }
    
    sampleRate_ = 22050;
    switch (psaveBitRate_)
    {
        case 0:
            sampleRate_ = 11025;
        break;
        case 1:
            sampleRate_ = 22050;
        break;
        case 2:
            sampleRate_ = 44100;
        break;
        case 3:
            sampleRate_ = 88200;
        break;
    }
    
    if (tapeBufferPointer->set_sample_rate(sampleRate_))
        p_Main->message("Cassette sound error: out of memory");
    ploadWavePointer = new WaveReader();
    
    if (!wxFile::Exists(fileName))
    {
        if (ploadWavePointer->createFile(fileName, sampleRate_, (psaveBitsPerSample_ + 1) * 8))
        {
            hwSaveOn_ = true;
            tapeRecording_ = true;
        }
        else
        {
            p_Main->message("Cassette sound error: Can't open file");
            delete ploadWavePointer;
            p_Main->eventSetTapeState(TAPE_STOP, tapeNumber_);
        }
    }
    else
    {
        sampleRate_ = ploadWavePointer->openFile(fileName);
        if (sampleRate_ != 0)
        {
            if (tapeBufferPointer->set_sample_rate(sampleRate_))
            {
                p_Main->message("Cassette sound error: out of memory");
                delete ploadWavePointer;
                p_Main->eventSetTapeState(TAPE_STOP, tapeNumber_);
            }
            else
            {
                hwSaveOn_ = true;
                tapeRecording_ = true;
            }
        }
        else
        {
            delete ploadWavePointer;
            p_Main->eventSetTapeState(TAPE_STOP, tapeNumber_);
        }
    }
}

void Sound::outSaveTapeHw(Byte value)
{
    tapeHwOutputValue_ = value;
    tapeHwReadyToReceive_ = 1;
}

void Sound::writeSilenceTapeHw()
{
    somethingSaved_ = true;
    
    short samples [1];

    samples[0] = 0;
    if (psaveBitsPerSample_ == 0)
    {
        unsigned char samples8bit [1];
        convertTo8Bit(samples, 1, samples8bit);
        ploadWavePointer->write(samples8bit[0]);
    }
    else
        ploadWavePointer->write(samples[0]);
    
    p_Computer->stepCassetteCounter(1);
}

int Sound::writeSaveTapeHw(Byte value, Byte numberOfStopBits)
{
    if (!hwSaveOn_)
        return 0;
    
    short samples [1];
    unsigned char samples8bit [1];
    int sample_count = 0;

    int const amplitude_max = 16384;
    int frequency[2];
    frequency[0] = p_Computer->getFrequency0();
    frequency[1] = p_Computer->getFrequency1();
    int const startBit = p_Computer->getStartBit()&1;
    int const dataBits = p_Computer->getDataBits();
    int const stopBit = p_Computer->getStopBit();
    float period;
    
    if (frequency[0] < frequency[1])
        period = (float)1/frequency[0];
    else
        period = (float)1/frequency[1];

    int freq = frequency[startBit];
        
    for (int bit = 0; bit <(1+dataBits+numberOfStopBits); bit++)
    {
        while((float)sample_count/sampleRate_ < (float)(bit+1) * period)
        {
            somethingSaved_ = true;

            samples[0] = (amplitude_max * sin(2 * freq * M_PI*((float)sample_count/sampleRate_ - (float)bit * period)));
            sample_count++;
            if (psaveBitsPerSample_ == 0)
            {
                convertTo8Bit(samples, 1, samples8bit);
                ploadWavePointer->write(samples8bit[0]);
            }
            else
                ploadWavePointer->write(samples[0]);
        }
        
        if (bit >= dataBits)
            freq = frequency[stopBit&1];
        else
        {
            freq = frequency[(value & 0x80)>>7];
            value = value << 1;
        }
    }
    p_Computer->stepCassetteCounter(sample_count);
    tapeHwReadyToReceive_ = 0;

    return sample_count;
}

void Sound::stopTape()
{
    if (!ploadOn_ && !psaveOn_ && !forwardOn_ && !rewindOn_ && !hwSaveOn_ && !hwSavePaused_ && !ploadPaused_)
        return;

    p_Computer->resetGaugeValue();
    tapeCounterStep_ = 0;
    tapePeriod_ = 0;
    stopTheTape_ = false;
    somethingSaved_ = false;
    forwardOn_ = false;
    rewindOn_ = false;
    p_Main->turboOff();
    if (ploadOn_ || ploadPaused_)
    {
        delete ploadWavePointer;
        ploadOn_ = false;
        ploadPaused_ = false;
//        if (computerType_ == ETI) TO BE CHECKED - is it ok to execute below on all states?
//            p_Computer->finishStopTape();
    }
    if (hwSaveOn_ || hwSavePaused_)
    {
        if (tapeHwReadyToReceive_ == 1)
            writeSaveTapeHw(tapeHwOutputValue_, 1);

        ploadWavePointer->closeFile();
        delete ploadWavePointer;
        hwSaveOn_ = false;
        hwSavePaused_ = false;
    }
    if (psaveOn_)
    {
        psaveWavePointer->closeFile();
        delete psaveWavePointer;
        psaveOn_ = false;
    }
    p_Main->eventSetTapeState(TAPE_STOP, tapeNumber_);
    p_Computer->finishStopTape();
    if (p_Vt100[UART1] != NULL)
        p_Vt100[UART1]->ResetIo();
    if (p_Vt100[UART2] != NULL)
        p_Vt100[UART2]->ResetIo();
}

void Sound::pauseTape()
{
    forwardOn_ = false;
    rewindOn_ = false;

    if (ploadOn_)
    {
//        p_Main->turboOff();
        ploadOn_ = false;
        ploadPaused_ = true;
        hwSavePaused_ = false;
    }
    if (psaveOn_)
    {
//        p_Main->turboOff();
        psaveOn_ = false;
    }
    if (hwSaveOn_)
    {
        if (tapeHwReadyToReceive_ == 1)
            writeSaveTapeHw(tapeHwOutputValue_, 1);

        if (somethingSaved_)
            ploadWavePointer->flush();
        somethingSaved_ = false;
        
        hwSaveOn_ = false;
        hwSavePaused_ = true;
        ploadPaused_ = false;
        startNewRecording_ = true;
    }

    if (p_Main->isTapeHwFred())
        p_Main->eventSetTapeState(TAPE_PAUSE, tapeNumber_);
    else
        p_Main->eventSetTapeState(TAPE_STOP, tapeNumber_);
    p_Computer->resetGaugeValue();
}

void Sound::restartTapeSave(int tapeState)
{
//    p_Main->turboOn();
    psaveOn_ = true;
    p_Main->eventSetTapeState(tapeState, tapeNumber_);
}

void Sound::restartHwTapeSave(int tapeState)
{
    forwardOn_ = false;
    rewindOn_ = false;
    ploadOn_ = false;
    ploadPaused_ = false;
    hwSaveOn_ = true;
    hwSavePaused_ = false;
    p_Main->eventSetTapeState(tapeState, tapeNumber_);
}

void Sound::restartTapeLoad(int tapeState)
{
//    p_Main->turboOn();
    forwardOn_ = false;
    rewindOn_ = false;
    ploadOn_ = true;
    ploadPaused_ = false;
    hwSavePaused_ = false;
    hwSaveOn_ = false;
    p_Main->eventSetTapeState(tapeState, tapeNumber_);
}

void Sound::stopPausedLoad()
{
    ploadOn_ = true;
    ploadPaused_ = false;
    stopTheTape_ = true;
}

void Sound::stopPausedSave()
{
    psaveOn_ = true;
    stopTheTape_ = true;
}

void Sound::stopSaveLoad()
{
    stopTheTape_ = true;
}

void Sound::forwardTape(int tapeState)
{
    rewindOn_ = false;
    forwardOn_ = (tapeState == TAPE_FF);
    ploadOn_ = (tapeState == TAPE_FF);
    p_Main->eventSetTapeState(tapeState, tapeNumber_);
}

void Sound::rewindTape(int tapeState)
{
    rewindOn_ = (tapeState == TAPE_RW);
    forwardOn_ = false;
    ploadOn_ = (tapeState == TAPE_RW);
    p_Main->eventSetTapeState(tapeState, tapeNumber_);
}

void Sound::setVolume(int volume)
{
    gain_ = (float) volume / 100;
    for (int i=0; i<toneChannels_; i++)
        toneSynthPointer[i]->volume(gain_);
    for (int i=0; i<2; i++)
    {
        noiseSynthPointer[i]->volume(gain_);
        psaveSynthPointer[i]->volume(gain_);
    }
    audioPointer->setAudioInGain(gain_);
}

void Sound::setClockRate(double clock)
{
    soundClock_ = clock;
    soundBufferPointerLeft->clock_rate((double)(soundClock_/CLOCK_FACTOR) * percentageClock_ * 1000000);
    soundBufferPointerRight->clock_rate((double)(soundClock_/CLOCK_FACTOR) * percentageClock_ * 1000000);
}

void Sound::setPercentageClock(double percentage)
{
    percentageClock_ = percentage;
    if ((percentageClock_*soundClock_) < 0.000045)  percentageClock_ = (double) 0.000045/soundClock_;
    setClockRate(soundClock_);
}

bool Sound::isSaving()
{
    return(psaveOn_);
}

bool Sound::isLoading()
{
    return(ploadOn_);
}

void Sound::setEqualization(int bass, int treble)
{
    soundBufferPointerLeft->bass_freq(bass);
    soundBufferPointerRight->bass_freq(bass);
    for (int i=0; i<toneChannels_; i++)
        toneSynthPointer[i]->treble_eq(treble);
    for (int i=0; i<noiseChannels_; i++)
        noiseSynthPointer[i]->treble_eq(treble);
    for (int i=0; i<2; i++)
    {
        psaveSynthPointer[i]->treble_eq(treble);
    }
}
void Sound::record_pause(bool status)
{
    audioPointer->record_pause(status);
    realCassetteLoad_ = status;
}

void Sound::setPsaveSettings()
{
    psaveVolume_ = p_Main->getPsaveData(0);
    psaveBitRate_ = p_Main->getPsaveData(1);
    psaveBitsPerSample_ = p_Main->getPsaveData(2);
    if (p_Main->getPsaveData(4) == 0)
        inputChannel_ = LEFTCHANNEL;
    else
        inputChannel_ = RIGHTCHANNEL;
    bool playback = (p_Main->getPsaveData(5) != 0);
    p_Computer->setTapePolarity(p_Main->getPsaveData(6));
    p_Computer->setConversionType(p_Main->getPsaveData(3), p_Main->getPsaveData(7));
    audioPointer->setTapeConfig(inputChannel_, playback);
    threshold8_ = p_Main->getPsaveData(8);
    threshold16_ = p_Main->getPsaveData(9);
    threshold24_ = p_Main->getPsaveData(14);
    fredFreq_ = (float) p_Main->getPsaveData(10)/10;
    useXmlThreshold_ = (p_Main->getPsaveData(13) != 0);
}

