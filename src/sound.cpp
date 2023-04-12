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

#define DECAY_LENGTH 420000
#define TARGET_FREQ 320

Sound::Sound()
{
    soundBufferPointerLeft = new Blip_Buffer();
    soundBufferPointerRight = new Blip_Buffer();
    tapeBufferPointer = new Blip_Buffer();
    audioPointer = new Sync_Audio();

    for (int i=0; i<8; i++)
    {
        toneSynthPointer[i] = new Blip_Synth<blip_high_quality,30>;
        frequencySuper_[i] = 0;
        toneAmplitude_[i] = 0;
        octaveSuper_[i] = 4;
        tonePeriod_[i] = 32;
    }
    for (int i=0; i<2; i++)
    {
        noiseSynthPointer[i] = new Blip_Synth<blip_high_quality,30>;
        psaveSynthPointer[i] = new Blip_Synth<blip_high_quality,30>;
    }
    tapeSynthPointer = new Blip_Synth<blip_high_quality,30>;

    vipSound_ = VIP_BEEP;
    followQ_ = false;
    studioBeep_ = false;
    audioIn_ = false;
}

Sound::~Sound()
{
    if (hwSaveOn_ || hwSavePaused_)
    {
        psaveWavePointer->closeFile();
        delete psaveWavePointer;
        p_Main->eventHwTapeStateChange(HW_TAPE_STATE_OFF);
    }

    delete soundBufferPointerLeft;
    delete soundBufferPointerRight;
    delete tapeBufferPointer;
    delete audioPointer;
    for (int i=0; i<8; i++)
        delete toneSynthPointer[i];
    for (int i=0; i<2; i++)
    {
        delete noiseSynthPointer[i];
        delete psaveSynthPointer[i];
    }
    delete tapeSynthPointer;

#if defined (__WXMSW__) || defined(__linux__)
    if (audioIn_)
        SDL_CloseAudioIn();
#endif
}

void Sound::initSound(double clock, double percentageClock, int computerType, int volume, int bass, int treble, int toneChannels, int stereo, bool realCasLoad, int beepFrequency, int bellFrequency)
{
    soundClock_ = clock;
    percentageClock_ = percentageClock;
    if ((percentageClock_*soundClock_) < 0.000045)  percentageClock_ = (double) 0.000045/soundClock_;
    computerType_ = computerType;
    gain_ = (float) volume / 100;
    stereo_ = stereo;
    realCassetteLoad_ = realCasLoad;
    audioIn_ = true;
    audioOut_ = true;
    tapeNumber_ = "";

    setPsaveSettings();

    for (int i=0; i<8; i++)
        toneOn_[i] = false;
    noiseOn_ = false;
    psaveOn_ = false;
    ploadOn_ = false;
    hwSaveOn_ = false;
    startNewRecording_ = true;
    hwSavePaused_ = false;
    forwardOn_ = false;
    wavOn_ = false;
    stopTheTape_ = false;
    tapeHwReadyToReceive_ = 0;
    numberOfSamples_ = 0;
    soundTime_ = 0;
    decayTime_ = 0;

    audioLatch1864_ = 128;
    toneAmplitude_[0] = 8;
    toneAmplitude_[1] = 8;

    changeBeepFrequency(beepFrequency);
    bellFrequency_ = bellFrequency;

    psaveAmplitude_ = 0;

    tapeBufferPointer->clock_rate((soundClock_/CLOCK_FACTOR * 1000000));
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

    setClockRate(clock);
    if (soundBufferPointerLeft->set_sample_rate(SAMPLE_RATE))
        p_Main->message("Sound error: out of memory");
    if (soundBufferPointerRight->set_sample_rate(SAMPLE_RATE))
        p_Main->message("Sound error: out of memory");

    soundBufferPointerLeft->bass_freq(bass);
    soundBufferPointerRight->bass_freq(bass);
    if (stereo_ == 2)
    {
        toneChannels_ = toneChannels;
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
    }
    else
    {
        toneChannels_ = toneChannels * 2;
        for (int i=0; i<toneChannels_; i+=2)
        {
            toneSynthPointer[i]->treble_eq(treble);
            toneSynthPointer[i]->volume(gain_);
            toneSynthPointer[i]->output(soundBufferPointerLeft);
            toneSynthPointer[i+1]->treble_eq(treble);
            toneSynthPointer[i+1]->volume(gain_);
            toneSynthPointer[i+1]->output(soundBufferPointerRight);
        }
    }

    for (int i=0; i<2; i++)
    {
        noiseSynthPointer[i]->treble_eq(treble);
        noiseSynthPointer[i]->volume(gain_/100);

        psaveSynthPointer[i]->treble_eq(treble);
        psaveSynthPointer[i]->volume(gain_);
    }
    noiseSynthPointer[0]->output(soundBufferPointerLeft);
    psaveSynthPointer[0]->output(soundBufferPointerLeft);
    noiseSynthPointer[1]->output(soundBufferPointerRight);
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
    for (int i=0; i<2; i++)
    {
        noiseSynthPointer[i]->update(soundTime_, 0);
        psaveSynthPointer[i]->update(soundTime_, 0);
    }
    if (audioIn_)
        record_pause(realCasLoad);
    else
    {
        if (realCassetteLoad_)
            p_Main->setRealCasOff(computerType_);
        realCassetteLoad_ = false;
    }
}

void Sound::tone(short reg4)
{
    if (toneOn_[0])
        if (toneAmplitude_[0] >= 0)
            toneAmplitude_[0] = reg4 & 0xf;
        else
            toneAmplitude_[0] = -(reg4 & 0xf);
    else
        toneAmplitude_[0] = reg4 & 0xf;
    toneAmplitude_[1] = toneAmplitude_[0];

    if ((reg4 & 0x80) == 0x80 ||(toneAmplitude_[0] == 0))
    {
        toneOn_[0] = false;
        toneOn_[1] = false;
    }
    else
     {
        int ToneFreqSel = (reg4 & 0x70) >> 4;
        int ToneDiv = (2 <<((ToneFreqSel ^ 0x7) + 1));
        int ToneN = ((reg4) & 0x7f00) >> 8;
        tonePeriod_[0] = (ToneDiv *(ToneN + 1));
        tonePeriod_[1] = tonePeriod_[0];
        if (!toneOn_[0])
        {
            toneTime_[0] = tonePeriod_[0];
            toneTime_[1] = tonePeriod_[0];
            toneSynthPointer[0]->update(soundTime_, toneAmplitude_[0]);
            toneSynthPointer[1]->update(soundTime_, toneAmplitude_[1]);
        }
        toneOn_[0] = true;
        toneOn_[1] = true;
     }
}

void Sound::tone1864Latch(Byte audioLatch1864)
{
    if (audioLatch1864 == 0)  audioLatch1864 = 128;
    audioLatch1864_ = audioLatch1864;
}

void Sound::tone1864On()
{
    tonePeriod_[0] = tonePeriod_[1] = (32 *(audioLatch1864_ + 1));
    if (!toneOn_[0])
    {
        toneTime_[0] = toneTime_[1] = tonePeriod_[0];
        toneSynthPointer[0]->update(soundTime_, toneAmplitude_[0]);
        toneSynthPointer[1]->update(soundTime_, toneAmplitude_[1]);
    }
    toneOn_[0] = toneOn_[1] = true;
}

void Sound::amplitudeSuper(int channel, int amplitude)
{
    if (stereo_ == 2)
    {
        if (toneAmplitude_[channel] < 0)
            toneAmplitude_[channel] = -(amplitude & 0xf);
        else
            toneAmplitude_[channel] = amplitude & 0xf;
    }
    else
    {
        if (toneAmplitude_[channel*2] < 0)
            toneAmplitude_[channel*2] = toneAmplitude_[channel*2+1] = -(amplitude & 0xf);
        else
            toneAmplitude_[channel*2] = toneAmplitude_[channel*2+1] = amplitude & 0xf;
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
            toneSynthPointer[i]->update(soundTime_, toneAmplitude_[i]);
        }
        toneOn_[i] = true;
    }
}

void Sound::beepOn()
{
    beepPeriod_ = (int)((soundClock_ * 1000000) / beepFrequency_ / 2);
    tonePeriod_[0] = tonePeriod_[1] = beepPeriod_;
    if (!toneOn_[0])
    {
        toneTime_[0] = toneTime_[1] = tonePeriod_[0];
        toneSynthPointer[0]->update(soundTime_, toneAmplitude_[0]);
        toneSynthPointer[1]->update(soundTime_, toneAmplitude_[1]);
    }
    toneOn_[0] = toneOn_[1] = true;
}

void Sound::bellOn()
{
    tonePeriod_[0] = tonePeriod_[1] = (int)((soundClock_ * 1000000) / bellFrequency_ / 2);
    if (!toneOn_[0])
    {
        toneTime_[0] = toneTime_[1] = tonePeriod_[0];
        toneSynthPointer[0]->update(soundTime_, toneAmplitude_[0]);
        toneSynthPointer[1]->update(soundTime_, toneAmplitude_[1]);
    }
    toneOn_[0] = toneOn_[1] = true;
}

void Sound::keyClickOn()
{
    tonePeriod_[0] = tonePeriod_[1] = (int)((soundClock_ * 1000000) / 800 / 2);
    if (!toneOn_[0])
    {
        toneTime_[0] = toneTime_[1] = tonePeriod_[0];
        toneSynthPointer[0]->update(soundTime_, toneAmplitude_[0]);
        toneSynthPointer[1]->update(soundTime_, toneAmplitude_[1]);
    }
    toneOn_[0] = toneOn_[1] = true;
}

void Sound::beepOnStudio()
{
    beepPeriod_ = (int)((soundClock_ * 1000000) / beepFrequency_ / 2);
    tonePeriod_[0] = tonePeriod_[1] = beepPeriod_;
    toneAmplitude_[0] = toneAmplitude_[1] = 4;
    
    if (!toneOn_[0])
    {
        toneTime_[0] = toneTime_[1] = tonePeriod_[0] * 4 / 3;
        toneSynthPointer[0]->update(soundTime_, toneAmplitude_[0]);
        toneSynthPointer[1]->update(soundTime_, toneAmplitude_[1]);
    }
    toneOn_[0] = toneOn_[1] = true;

    decayTime_ = DECAY_LENGTH;
    targetPeriod_ = (int)((soundClock_ * 1000000) / TARGET_FREQ / 2);
    decayPeriod_ = (DECAY_LENGTH / (targetPeriod_ - beepPeriod_))*8;
    saveDecayPeriod_ = decayPeriod_;
    studioBeep_ = true;
}

void Sound::beepOff()
{
    for (int i=0; i<toneChannels_; i++)
        toneOn_[i] = false;
}

void Sound::changeBeepFrequency(int frequency)
{
    if (frequency == 0)
        frequency = 1;
    
    beepFrequency_ = frequency;
    beepPeriod_ = (int)((soundClock_ * 1000000) / beepFrequency_ / 2);
}

void Sound::toneElf2KOn()
{
//    toneAmplitude_[0] = toneAmplitude_[1] = 8;
    toneSynthPointer[0]->update(soundTime_, toneAmplitude_[0]);
    toneSynthPointer[1]->update(soundTime_, toneAmplitude_[1]);
    toneOn_[0] = toneOn_[1] = true;
}

void Sound::toneElf2KOff()
{
//    toneAmplitude_[0] = toneAmplitude_[1] = 0;
    toneSynthPointer[0]->update(soundTime_, toneAmplitude_[0]);
    toneSynthPointer[1]->update(soundTime_, toneAmplitude_[1]);
    toneOn_[0] = toneOn_[1] = false;
}

void Sound::noise(short reg5)
{
    noiseAmplitude_ = (reg5 & 0xf00) >> 8;

    if ((reg5 & 0x8000) == 0x8000 ||(noiseAmplitude_ == 0))
    {
        noiseOn_ = false;
    }
    else
    {
        int freq = (reg5 >> 12) & 0x7;
        int factor = (freq ^ 0x7) +1; // 8 - 1
        noisePeriod_ = (4 << factor); // 0: 512, 1: 256, 2: 128, 3:64, 4:32, 5:16, 6:8, 7:4
        if (!noiseOn_)
        {
            noiseTime_ = noisePeriod_;
            int noiseAmplitude = (rand() % (noiseAmplitude_*2)) + 1 - noiseAmplitude_;
            noiseSynthPointer[0]->update(soundTime_, noiseAmplitude*100);
            noiseSynthPointer[1]->update(soundTime_, noiseAmplitude*100);
        }
        noiseOn_ = true;
    }
}

void Sound::soundCycle()
{
    for (int i=0; i<toneChannels_; i++)
    {
        if (toneOn_[i])
        {
            if (i == 0 && decayTime_ > 0)
            {
                decayTime_-=8;
                decayPeriod_-=8;
                
                if (decayPeriod_ <= 0)
                {
                    beepPeriod_ += 8;
                    tonePeriod_[0] = tonePeriod_[1] = beepPeriod_;
                    decayPeriod_ = saveDecayPeriod_;
                }
                
            }
            
            toneTime_[i]-=8;            
            if (toneTime_[i] <= 0)
            {
                if (studioBeep_)
                {
                    if (toneAmplitude_[i] > 0)
                    {
                        toneTime_[i] = tonePeriod_[i] * 2 / 3;
                        toneAmplitude_[i] = -8;
                    }
                    else
                    {
                        toneTime_[i] = tonePeriod_[i] * 4 / 3;
                        toneAmplitude_[i] = 4;
                    }
                }
                else
                {
                    toneTime_[i] = tonePeriod_[i];
                    toneAmplitude_[i] = -toneAmplitude_[i];
                }
                toneSynthPointer[i]->update(soundTime_, toneAmplitude_[i]);
            }
        }
    }
    if (noiseOn_)
    {
        noiseTime_-=8;
        if (noiseTime_ <= 0)
        {
            noiseTime_ = noisePeriod_;
            int noiseAmplitude = (rand() % (noiseAmplitude_*2)) + 1 - noiseAmplitude_;
            noiseSynthPointer[0]->update(soundTime_, noiseAmplitude*100);
            noiseSynthPointer[1]->update(soundTime_, noiseAmplitude*100);
        }
    }
    soundTime_+=8;
    playSound();
}

void Sound::playSound()
{
    if (psaveOn_ || ploadOn_ || wavOn_)
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
            switch (computerType_)
            {
                case VIP:
                    if (q)
                    {
                        switch(vipSound_)
                        {
                            case VIP_BEEP:
                                beepOn();
                            break;
                            case VIP_1864:
                                tone1864On();
                            break;
                            case VIP_SUPER2:
                            case VIP_SUPER4:
                                toneSuper();
                            break;
                        }
                    }
                    else
                        beepOff();
                break;

                case FRED1:
                case FRED1_5:
                    if (q)
                        toneElf2KOn();
                    else
                        toneElf2KOff();
                break;

                case TMC1800:
                    if (q)
                        beepOn();
                    else
                        beepOff();
                break;
                    
                case VISICOM:
                case STUDIO:
                    if (q)
                        beepOnStudio();
                    else
                        beepOff();
                break;
                    
                case TMC2000:
                case ETI:
                case VICTORY:
                case STUDIOIV:
                case VIPII:
                    if (q)
                        tone1864On();
                    else
                        beepOff();
                break;

                case NANO:
                case COSMICOS:
                    if (followQ_)
                    {
                        if (q)
                            toneElf2KOn();
                        else
                            toneElf2KOff();
                    }
                    else
                    {
                        if (q)
                            tone1864On();
                        else
                            beepOff();
                    }
                break;

                case ELF2K:
                    if (followQ_)
                    {
                        if (q)
                            toneElf2KOn();
                        else
                            toneElf2KOff();
                    }
                break;

                case ELF:
                case ELFII:
                case SUPERELF:
                case PICO:
                    switch (qSound_)
                    {
                        case QSOUNDSW:
                            if (q)
                                toneElf2KOn();
                            else
                                toneElf2KOff();
                        break;

                        case QSOUNDEXT:
                            toneAmplitude_[0] = 8;
                            toneAmplitude_[1] = 8;
                            if (q)
                                beepOn();
                            else
                                beepOff();
                        break;
                    }
                break;

                case XML:
                    switch (qSound_)
                    {
                        case QSOUNDSW:
                            if (q)
                                toneElf2KOn();
                            else
                                toneElf2KOff();
                        break;

                        case QSOUNDEXT:
                            toneAmplitude_[0] = 8;
                            toneAmplitude_[1] = 8;
                            if (q)
                                beepOn();
                            else
                                beepOff();
                        break;
                    }
                    p_Computer->printOutPecom(q);
                break;

                case PECOM:
                    p_Computer->printOutPecom(q);
                break;
            }
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
    if (!psaveOn_ && !ploadOn_ && !wavOn_ && !hwSaveOn_)
        return;

    if (stopTheTape_)
    {
        stopTape();
        return;
    }

    int sample_count;

    if (hwSaveOn_)
    {
        if (numberOfSamples_ <= 0)
        {
            if (tapeHwReadyToReceive_ == 1)
            {
                if (startNewRecording_)
                {
                    writeSaveTapeHw(0xF8, 4);
                    writeSaveTapeHw(0xBD, 1);
                    startNewRecording_ = false;
                }
                sample_count = writeSaveTapeHw(tapeHwOutputValue_, 1);
                numberOfSamples_ += sample_count;
            }
            else
            {
                writeSilenceTapeHw();
                startNewRecording_ = true;
            }
        }
        else
            numberOfSamples_--;
        return;
    }

    tapeBufferPointer->end_frame(soundTime_);

    int const soundBufferSize = 256;
    short samples [soundBufferSize];
    short ploadSamples [soundBufferSize];

    while(true)
    {
        // Read as many samples as possible
        sample_count = (int)tapeBufferPointer->read_samples(samples, soundBufferSize);

        // Stop if no more samples are avialable
        if (sample_count == 0)
            break;

        if (forwardOn_)
        {
            long in = ploadWavePointer->read(ploadSamples, sample_count*forwardSpeed_, gain_);
            if (ploadOn_)
            {
                soundBufferPointerLeft->mix_samples(ploadSamples, in);
                soundBufferPointerRight->mix_samples(ploadSamples, in);
                if (ploadWavePointer->eof())
                    stopTape();
            }
        }

        if (ploadOn_ && !forwardOn_)
        {
            long in = ploadWavePointer->read(ploadSamples, sample_count, gain_);
            if (ploadOn_)
            {
                soundBufferPointerLeft->mix_samples(ploadSamples, in);
                soundBufferPointerRight->mix_samples(ploadSamples, in);
                if (ploadWavePointer->eof())
                    stopTape();
            }
        }
        
        if (wavOn_)
        {
            long in = wavSoundPointer->read(ploadSamples, sample_count, gain_);
            soundBufferPointerLeft->mix_samples(ploadSamples, in);
            soundBufferPointerRight->mix_samples(ploadSamples, in);
            if (wavSoundPointer->eof())
            {
                delete wavSoundPointer;
                wavOn_ = false;
            }
        }
        
        if (psaveOn_)
        {
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
        hwSaveOn_ = true;
    else
    {
        p_Main->message("Cassette sound error: Can't open file");
        delete psaveWavePointer;
        p_Main->eventSetTapeState(TAPE_STOP, tapeNumber_);
    }
}

void Sound::outSaveTapeHw(Byte value)
{
    tapeHwOutputValue_ = value;
    tapeHwReadyToReceive_ = 1;
}

void Sound::writeSilenceTapeHw()
{
    short samples [1];

    samples[0] = 0;
    if (psaveBitsPerSample_ == 0)
    {
        unsigned char samples8bit [1];
        convertTo8Bit(samples, 1, samples8bit);
        psaveWavePointer->write(samples8bit, 1);
    }
    else
        psaveWavePointer->write(samples, 1);
    
    stepCassetteCounter(1);
}

int Sound::writeSaveTapeHw(Byte value, Byte stopBits)
{
    if (!hwSaveOn_)
        return 0;
    
    tapeHwReadyToReceive_ = 0;

    short samples [1];
    unsigned char samples8bit [1];
    int sample_count = 0;

    int const amplitude_max = 16384;
    int const freq0 = 4000;
    int const freq1 = 2000;
    float const period = (float)1/2000;

    int freq;
        
    for (int bit = 0; bit <(9+stopBits); bit++)
    {
        if (bit == 0)
            freq = freq1;
        else if (bit >= 9)
            freq = freq0;
        else
        {
            if ((value & 0x80) == 0x80)
                freq = freq1;
            else
                freq = freq0;
            value = value << 1;
        }
  
        while((float)sample_count/sampleRate_ < (float)(bit+1) * period)
        {
            samples[0] = (amplitude_max * sin(2 * freq * M_PI*((float)sample_count/sampleRate_ - (float)bit * period)));
            sample_count++;
            if (psaveBitsPerSample_ == 0)
            {
                convertTo8Bit(samples, 1, samples8bit);
                psaveWavePointer->write(samples8bit, 1);
            }
            else
                psaveWavePointer->write(samples, 1);
        }
    }
    stepCassetteCounter(sample_count);

    return sample_count;
}

void Sound::stopTape()
{
    if (!ploadOn_ && !psaveOn_ && !forwardOn_ && !hwSaveOn_ && !hwSavePaused_)
        return;

    p_Computer->resetGaugeValue();
    tapeCounterStep_ = 0;
    tapePeriod_ = 0;
    stopTheTape_ = false;
    forwardOn_ = false;
    p_Main->turboOff();
    if (ploadOn_)
    {
        delete ploadWavePointer;
        ploadOn_ = false;
        p_Computer->checkLoadedSoftware();
        if (computerType_ == ETI)
            p_Computer->finishStopTape();
    }
    if (psaveOn_ || hwSaveOn_ || hwSavePaused_)
    {
        psaveWavePointer->closeFile();
        delete psaveWavePointer;
        psaveOn_ = false;
        hwSaveOn_ = false;
        hwSavePaused_ = false;
    }
    p_Main->eventSetTapeState(TAPE_STOP, tapeNumber_);
    if (computerType_ == XML || computerType_ == FRED1 || computerType_ == FRED1_5 || computerType_ == VIP || computerType_ == STUDIOIV)
        p_Computer->finishStopTape();
    if (p_Vt100[UART1] != NULL)
        p_Vt100[UART1]->ResetIo();
    if (p_Vt100[UART2] != NULL)
        p_Vt100[UART2]->ResetIo();
}

void Sound::pauseTape()
{
    if (ploadOn_)
    {
//        p_Main->turboOff();
        ploadOn_ = false;
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
        hwSaveOn_ = false;
        hwSavePaused_ = true;
    }

    if (computerType_ == FRED1 || computerType_ == FRED1_5)
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
    hwSaveOn_ = true;
    hwSavePaused_ = false;
    p_Main->eventSetTapeState(tapeState, tapeNumber_);
}

void Sound::restartTapeLoad(int tapeState)
{
//    p_Main->turboOn();
    forwardOn_ = false;
    ploadOn_ = true;
    p_Main->eventSetTapeState(tapeState, tapeNumber_);
}

void Sound::stopPausedLoad()
{
    ploadOn_ = true;
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
    forwardOn_ = true;
    ploadOn_ = true;
    p_Main->eventSetTapeState(tapeState, tapeNumber_);
}

void Sound::setVolume(int volume)
{
    gain_ = (float) volume / 100;
    for (int i=0; i<toneChannels_; i++)
        toneSynthPointer[i]->volume(gain_);
    for (int i=0; i<2; i++)
    {
        noiseSynthPointer[i]->volume(gain_/100);
        psaveSynthPointer[i]->volume(gain_);
    }
    audioPointer->setAudioInGain(gain_);
}

void Sound::setClockRate(double clock)
{
    soundClock_ = clock;
    changeBeepFrequency(beepFrequency_);
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
    for (int i=0; i<2; i++)
    {
        noiseSynthPointer[i]->treble_eq(treble);
        psaveSynthPointer[i]->treble_eq(treble);
    }
}
void Sound::record_pause(bool status)
{
    audioPointer->record_pause(status);
    realCassetteLoad_ = status;
}

void Sound::wavFile()
{
    audioPointer->wavFile();
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
    fredFreq_ = (float) p_Main->getPsaveData(10)/10;
}

