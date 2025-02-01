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

#if defined(__linux__)
#include "app_icon.xpm"
#endif

#include "main.h"
#include "ay-3-8912.h"

AY_3_8912Instance::AY_3_8912Instance()
{
    for (int channel = 0; channel < TONE_MAX; channel++)
    {
        toneOn_[channel] = 0xff;
        noiseOn_[channel] = 0xff;
        periodTone_[channel] = 0;
        periodEnvelope_ = 0;
    }
}

void AY_3_8912Instance::configureAY_3_8912(AY_3_8912Configuration ay_3_8912Configuration)
{
    ay_3_8912Configuration_ = ay_3_8912Configuration;
    p_Main->configureMessage(&ay_3_8912Configuration.ioGroupVector, "AY-3-8912 Programmable Sound Generator");

    p_Computer->setOutType(&ay_3_8912Configuration.ioGroupVector, ay_3_8912Configuration.registerAddressAy1, "write register AY 1");
    p_Computer->setOutType(&ay_3_8912Configuration.ioGroupVector, ay_3_8912Configuration.registerAddressAy2, "write register AY 2");
    p_Computer->setOutType(&ay_3_8912Configuration.ioGroupVector, ay_3_8912Configuration.dataAddress, "write data");
    p_Computer->setInType(&ay_3_8912Configuration.ioGroupVector, ay_3_8912Configuration.dataAddress, "read data");

    p_Main->message("");
}

bool AY_3_8912Instance::ioGroupAY_3_8912(int ioGroup)
{
    bool groupFound = false;
    
    if (ay_3_8912Configuration_.ioGroupVector.size() == 0)
        groupFound = true;
    else
    {
        for (std::vector<int>::iterator ioGroupIterator = ay_3_8912Configuration_.ioGroupVector.begin (); ioGroupIterator != ay_3_8912Configuration_.ioGroupVector.end (); ++ioGroupIterator)
        {
            if (*ioGroupIterator == ioGroup)
                groupFound = true;
        }
    }
    return groupFound;
}

void AY_3_8912Instance::writeRegister(int ayNumber, Byte value)
{
    selectedRegister_ = value;
    ayNumber_ = ayNumber;
    if (ayNumber_ >= MAX_AY)
        ayNumber_ = 0;
}

void AY_3_8912Instance::writeData(Byte value)
{
    int previousNoisePeriod;
    int amplitudeTone;
    Byte envelopeShape;

    switch (selectedRegister_)
    {
        case 0: // Channel A Tone Period low byte
            if (ay_3_8912Configuration_.channel_a_1 != -1)
            {
                periodTone_[ay_3_8912Configuration_.channel_a_1] = (periodTone_[ay_3_8912Configuration_.channel_a_1] & 0xF00) | value;
                p_Computer->setTonePeriod(ay_3_8912Configuration_.channel_a_1+(ayNumber_*TONE_MAX), 16 * periodTone_[ay_3_8912Configuration_.channel_a_1], toneOn_[ay_3_8912Configuration_.channel_a_1]);
            }
            if (ay_3_8912Configuration_.channel_a_2 != -1)
            {
                periodTone_[ay_3_8912Configuration_.channel_a_2] = (periodTone_[ay_3_8912Configuration_.channel_a_2] & 0xF00) | value;
                p_Computer->setTonePeriod(ay_3_8912Configuration_.channel_a_2+(ayNumber_*TONE_MAX), 16 * periodTone_[ay_3_8912Configuration_.channel_a_2], toneOn_[ay_3_8912Configuration_.channel_a_2]);
            }
        break;
            
        case 1: // Channel A Tone Period high nible
            if (ay_3_8912Configuration_.channel_a_1 != -1)
            {
                periodTone_[ay_3_8912Configuration_.channel_a_1] = (periodTone_[ay_3_8912Configuration_.channel_a_1] & 0xFF) | ((value & 0xf) << 8);
                p_Computer->setTonePeriod(ay_3_8912Configuration_.channel_a_1+(ayNumber_*TONE_MAX), 16 * periodTone_[ay_3_8912Configuration_.channel_a_1], toneOn_[ay_3_8912Configuration_.channel_a_1]);
            }
            if (ay_3_8912Configuration_.channel_a_2 != -1)
            {
                periodTone_[ay_3_8912Configuration_.channel_a_2] = (periodTone_[ay_3_8912Configuration_.channel_a_2] & 0xFF) | ((value & 0xf) << 8);
                p_Computer->setTonePeriod(ay_3_8912Configuration_.channel_a_2+(ayNumber_*TONE_MAX), 16 * periodTone_[ay_3_8912Configuration_.channel_a_2], toneOn_[ay_3_8912Configuration_.channel_a_2]);
            }
        break;
            
        case 2: // Channel B Tone Period low byte
            if (ay_3_8912Configuration_.channel_b_1 != -1)
            {
                periodTone_[ay_3_8912Configuration_.channel_b_1] = (periodTone_[ay_3_8912Configuration_.channel_b_1] & 0xF00) | value;
                p_Computer->setTonePeriod(ay_3_8912Configuration_.channel_b_1+(ayNumber_*TONE_MAX), 16 * periodTone_[ay_3_8912Configuration_.channel_b_1], toneOn_[ay_3_8912Configuration_.channel_b_1]);
            }
            if (ay_3_8912Configuration_.channel_b_2 != -1)
            {
                periodTone_[ay_3_8912Configuration_.channel_b_2] = (periodTone_[ay_3_8912Configuration_.channel_b_2] & 0xF00) | value;
                p_Computer->setTonePeriod(ay_3_8912Configuration_.channel_b_2+(ayNumber_*TONE_MAX), 16 * periodTone_[ay_3_8912Configuration_.channel_b_2], toneOn_[ay_3_8912Configuration_.channel_b_2]);
            }
        break;
            
        case 3: // Channel B Tone Period high nible
            if (ay_3_8912Configuration_.channel_b_1 != -1)
            {
                periodTone_[ay_3_8912Configuration_.channel_b_1] = (periodTone_[ay_3_8912Configuration_.channel_b_1] & 0xFF) | ((value & 0xf) << 8);
                p_Computer->setTonePeriod(ay_3_8912Configuration_.channel_b_1+(ayNumber_*TONE_MAX), 16 * periodTone_[ay_3_8912Configuration_.channel_b_1], toneOn_[ay_3_8912Configuration_.channel_b_1]);
            }
            if (ay_3_8912Configuration_.channel_b_2 != -1)
            {
                periodTone_[ay_3_8912Configuration_.channel_b_2] = (periodTone_[ay_3_8912Configuration_.channel_b_2] & 0xFF) | ((value & 0xf) << 8);
                p_Computer->setTonePeriod(ay_3_8912Configuration_.channel_b_2+(ayNumber_*TONE_MAX), 16 * periodTone_[ay_3_8912Configuration_.channel_b_2], toneOn_[ay_3_8912Configuration_.channel_b_2]);
            }
        break;
            
        case 4: // Channel C Tone Period low byte
            if (ay_3_8912Configuration_.channel_c_1 != -1)
            {
                periodTone_[ay_3_8912Configuration_.channel_c_1] = (periodTone_[ay_3_8912Configuration_.channel_c_1] & 0xF00) | value;
                p_Computer->setTonePeriod(ay_3_8912Configuration_.channel_c_1+(ayNumber_*TONE_MAX), 16 * periodTone_[ay_3_8912Configuration_.channel_c_1], toneOn_[ay_3_8912Configuration_.channel_c_1]);
            }
            if (ay_3_8912Configuration_.channel_c_2 != -1)
            {
                periodTone_[ay_3_8912Configuration_.channel_c_2] = (periodTone_[ay_3_8912Configuration_.channel_c_2] & 0xF00) | value;
                p_Computer->setTonePeriod(ay_3_8912Configuration_.channel_c_2+(ayNumber_*TONE_MAX), 16 * periodTone_[ay_3_8912Configuration_.channel_c_2], toneOn_[ay_3_8912Configuration_.channel_c_2]);
            }
        break;
            
        case 5: // Channel C Tone Period high nible
            if (ay_3_8912Configuration_.channel_c_1 != -1)
            {
                periodTone_[ay_3_8912Configuration_.channel_c_1] = (periodTone_[ay_3_8912Configuration_.channel_c_1] & 0xFF) | ((value & 0xf) << 8);
                p_Computer->setTonePeriod(ay_3_8912Configuration_.channel_c_1+(ayNumber_*TONE_MAX), 16 * periodTone_[ay_3_8912Configuration_.channel_c_1], toneOn_[ay_3_8912Configuration_.channel_c_1]);
            }
            if (ay_3_8912Configuration_.channel_c_2 != -1)
            {
                periodTone_[ay_3_8912Configuration_.channel_c_2] = (periodTone_[ay_3_8912Configuration_.channel_c_2] & 0xFF) | ((value & 0xf) << 8);
                p_Computer->setTonePeriod(ay_3_8912Configuration_.channel_c_2+(ayNumber_*TONE_MAX), 16 * periodTone_[ay_3_8912Configuration_.channel_c_2], toneOn_[ay_3_8912Configuration_.channel_c_2]);
            }
        break;
            
        case 6: // Noise Period
            if (ay_3_8912Configuration_.channel_a_1 != -1)
            {
                p_Computer->setNoisePeriod(ay_3_8912Configuration_.channel_a_1+(ayNumber_*TONE_MAX), 16 * (value & 0x1f));
            }
            if (ay_3_8912Configuration_.channel_a_2 != -1)
            {
                p_Computer->setNoisePeriod(ay_3_8912Configuration_.channel_a_2+(ayNumber_*TONE_MAX), 16 * (value & 0x1f));
            }
            if (ay_3_8912Configuration_.channel_b_1 != -1)
            {
                p_Computer->setNoisePeriod(ay_3_8912Configuration_.channel_b_1+(ayNumber_*TONE_MAX), 16 * (value & 0x1f));
            }
            if (ay_3_8912Configuration_.channel_b_2 != -1)
            {
                p_Computer->setNoisePeriod(ay_3_8912Configuration_.channel_b_2+(ayNumber_*TONE_MAX), 16 * (value & 0x1f));
            }
            if (ay_3_8912Configuration_.channel_c_1 != -1)
            {
                p_Computer->setNoisePeriod(ay_3_8912Configuration_.channel_c_1+(ayNumber_*TONE_MAX), 16 * (value & 0x1f));
            }
            if (ay_3_8912Configuration_.channel_c_2 != -1)
            {
                p_Computer->setNoisePeriod(ay_3_8912Configuration_.channel_c_2+(ayNumber_*TONE_MAX), 16 * (value & 0x1f));
            }
        break;
            
        case 7: // Mixer and I/O control
            portTypeInputA_[ayNumber_] = ((value & 0x40) == 0);
            portTypeInputB_[ayNumber_] = ((value & 0x80) == 0);
            
            previousNoisePeriod = -1;
            if (ay_3_8912Configuration_.channel_a_1 != -1)
            {
                toneOn_[ay_3_8912Configuration_.channel_a_1] = ((value & 1) == 0);
                p_Computer->startTone(ay_3_8912Configuration_.channel_a_1+(ayNumber_*TONE_MAX), toneOn_[ay_3_8912Configuration_.channel_a_1]);
                noiseOn_[ay_3_8912Configuration_.channel_a_1] = ((value & 0x8) == 0);
                previousNoisePeriod = p_Computer->startNoise(ay_3_8912Configuration_.channel_a_1+(ayNumber_*TONE_MAX), noiseOn_[ay_3_8912Configuration_.channel_a_1], -1);
            }
            if (ay_3_8912Configuration_.channel_a_2 != -1)
            {
                toneOn_[ay_3_8912Configuration_.channel_a_2] = ((value & 1) == 0);
                p_Computer->startTone(ay_3_8912Configuration_.channel_a_2+(ayNumber_*TONE_MAX), toneOn_[ay_3_8912Configuration_.channel_a_2]);
                noiseOn_[ay_3_8912Configuration_.channel_a_2] = ((value &  0x8) == 0);
                p_Computer->startNoise(ay_3_8912Configuration_.channel_a_2+(ayNumber_*TONE_MAX), noiseOn_[ay_3_8912Configuration_.channel_a_2], previousNoisePeriod);
            }

            previousNoisePeriod = -1;
            if (ay_3_8912Configuration_.channel_b_1 != -1)
            {
                toneOn_[ay_3_8912Configuration_.channel_b_1] = ((value & 2) == 0);
                p_Computer->startTone(ay_3_8912Configuration_.channel_b_1+(ayNumber_*TONE_MAX), toneOn_[ay_3_8912Configuration_.channel_b_1]);
                noiseOn_[ay_3_8912Configuration_.channel_b_1] = ((value & 0x10) == 0);
                previousNoisePeriod = p_Computer->startNoise(ay_3_8912Configuration_.channel_b_1+(ayNumber_*TONE_MAX), noiseOn_[ay_3_8912Configuration_.channel_b_1], 01);
            }
            if (ay_3_8912Configuration_.channel_b_2 != -1)
            {
                toneOn_[ay_3_8912Configuration_.channel_b_2] = ((value & 2) == 0);
                p_Computer->startTone(ay_3_8912Configuration_.channel_b_2+(ayNumber_*TONE_MAX), toneOn_[ay_3_8912Configuration_.channel_b_2]);
                noiseOn_[ay_3_8912Configuration_.channel_b_2] = ((value & 0x10) == 0);
                p_Computer->startNoise(ay_3_8912Configuration_.channel_b_2+(ayNumber_*TONE_MAX), noiseOn_[ay_3_8912Configuration_.channel_b_2], previousNoisePeriod);
            }

            previousNoisePeriod = -1;
            if (ay_3_8912Configuration_.channel_c_1 != -1)
            {
                toneOn_[ay_3_8912Configuration_.channel_c_1] = ((value & 4) == 0);
                p_Computer->startTone(ay_3_8912Configuration_.channel_c_1+(ayNumber_*TONE_MAX), toneOn_[ay_3_8912Configuration_.channel_c_1]);
                noiseOn_[ay_3_8912Configuration_.channel_c_1] = ((value & 0x20) == 0);
                previousNoisePeriod = p_Computer->startNoise(ay_3_8912Configuration_.channel_c_1+(ayNumber_*TONE_MAX), noiseOn_[ay_3_8912Configuration_.channel_c_1], -1);
            }
            if (ay_3_8912Configuration_.channel_c_2 != -1)
            {
                toneOn_[ay_3_8912Configuration_.channel_c_2] = ((value & 4) == 0);
                p_Computer->startTone(ay_3_8912Configuration_.channel_c_2+(ayNumber_*TONE_MAX), toneOn_[ay_3_8912Configuration_.channel_c_2]);
                noiseOn_[ay_3_8912Configuration_.channel_c_2] = ((value & 0x20) == 0);
                p_Computer->startNoise(ay_3_8912Configuration_.channel_c_2+(ayNumber_*TONE_MAX), noiseOn_[ay_3_8912Configuration_.channel_c_2], previousNoisePeriod);
            }
        break;
            
        case 8: // Channel A Amplitude
            if ((value & 0x10) == 0x10)
                envelopeAttack_ ? amplitudeTone = 0 : amplitudeTone = 0xf;
            else
                amplitudeTone = value & 0xf;

            if (ay_3_8912Configuration_.channel_a_1 != -1)
            {
                p_Computer->ayEnvelopeActive(ay_3_8912Configuration_.channel_a_1, (value & 0x10) == 0x10);
                p_Computer->setToneAmplitude(ay_3_8912Configuration_.channel_a_1+(ayNumber_*TONE_MAX), amplitudeTone, toneOn_[ay_3_8912Configuration_.channel_a_1]);
                p_Computer->setNoiseAmplitude(ay_3_8912Configuration_.channel_a_1+(ayNumber_*TONE_MAX), amplitudeTone);
            }

            if (ay_3_8912Configuration_.channel_a_2 != -1)
            {
                p_Computer->ayEnvelopeActive(ay_3_8912Configuration_.channel_a_2, (value & 0x10) == 0x10);
                p_Computer->setToneAmplitude(ay_3_8912Configuration_.channel_a_2+(ayNumber_*TONE_MAX), amplitudeTone, toneOn_[ay_3_8912Configuration_.channel_a_2]);
                p_Computer->setNoiseAmplitude(ay_3_8912Configuration_.channel_a_2+(ayNumber_*TONE_MAX), amplitudeTone);
            }
        break;
            
        case 9: // Channel B Amplitude
            if ((value & 0x10) == 0x10)
                envelopeAttack_ ? amplitudeTone = 0 : amplitudeTone = 0xf;
            else
                amplitudeTone = value & 0xf;

            if (ay_3_8912Configuration_.channel_b_1 != -1)
            {
                p_Computer->ayEnvelopeActive(ay_3_8912Configuration_.channel_b_1, (value & 0x10) == 0x10);
                p_Computer->setToneAmplitude(ay_3_8912Configuration_.channel_b_1+(ayNumber_*TONE_MAX), amplitudeTone, toneOn_[ay_3_8912Configuration_.channel_b_1]);
                p_Computer->setNoiseAmplitude(ay_3_8912Configuration_.channel_b_1+(ayNumber_*TONE_MAX), amplitudeTone);
            }

            if (ay_3_8912Configuration_.channel_b_2 != -1)
            {
                p_Computer->ayEnvelopeActive(ay_3_8912Configuration_.channel_b_2, (value & 0x10) == 0x10);
                p_Computer->setToneAmplitude(ay_3_8912Configuration_.channel_b_2+(ayNumber_*TONE_MAX), amplitudeTone, toneOn_[ay_3_8912Configuration_.channel_b_2]);
                p_Computer->setNoiseAmplitude(ay_3_8912Configuration_.channel_b_2+(ayNumber_*TONE_MAX), amplitudeTone);
            }
        break;
            
        case 10: // Channel C Amplitude
            if ((value & 0x10) == 0x10)
                envelopeAttack_ ? amplitudeTone = 0 : amplitudeTone = 0xf;
            else
                amplitudeTone = value & 0xf;

            if (ay_3_8912Configuration_.channel_c_1 != -1)
            {
                p_Computer->ayEnvelopeActive(ay_3_8912Configuration_.channel_c_1, (value & 0x10) == 0x10);
                p_Computer->setToneAmplitude(ay_3_8912Configuration_.channel_c_1+(ayNumber_*TONE_MAX), amplitudeTone, toneOn_[ay_3_8912Configuration_.channel_c_1]);
                p_Computer->setNoiseAmplitude(ay_3_8912Configuration_.channel_c_1+(ayNumber_*TONE_MAX), amplitudeTone);
            }
            
            if (ay_3_8912Configuration_.channel_c_2 != -1)
            {
                p_Computer->ayEnvelopeActive(ay_3_8912Configuration_.channel_c_2, (value & 0x10) == 0x10);
                p_Computer->setToneAmplitude(ay_3_8912Configuration_.channel_c_2+(ayNumber_*TONE_MAX), amplitudeTone, toneOn_[ay_3_8912Configuration_.channel_c_2]);
                p_Computer->setNoiseAmplitude(ay_3_8912Configuration_.channel_c_2+(ayNumber_*TONE_MAX), amplitudeTone);
            }
        break;
            
        case 11: // Envelope Period low byte
            periodEnvelope_ = (periodEnvelope_ & 0xF00) | value;
            p_Computer->setEnvelopePeriod(16 * periodEnvelope_);
        break;

        case 12: // Envelope Period high byte
            periodEnvelope_ = (periodEnvelope_ & 0xFF) | ((value & 0xff) << 8);
            p_Computer->setEnvelopePeriod(16 * periodEnvelope_);
        break;

        case 13: // Envelope Shape/Cycle
            envelopeShape = value & 0xf;
            if ((envelopeShape & 0x8) == 0x8)
            {
                envelopeShape &= 0xc;
            }
            p_Computer->ayEnvelopeShape(envelopeShape);
            p_Computer->ayEnvelopeContinues((value & 0x8) == 0x8);
            envelopeAttack_ = (value & 0x4) == 0x4;
            p_Computer->ayEnvelopeAttack(envelopeAttack_);
            p_Computer->ayEnvelopeAlternate((value & 0x2) == 0x2);
            p_Computer->ayEnvelopeHold((value & 0x1) == 0x1);
        break;

        case 14: // I/O Port A
            if (!portTypeInputA_[ayNumber_])
                portValueA_[ayNumber_] = value;
        break;

        case 15: // I/O Port B
            if (!portTypeInputB_[ayNumber_])
                portValueB_[ayNumber_] = value;
        break;
    }
}

Byte AY_3_8912Instance::readData()
{
    switch (selectedRegister_)
    {
        case 0: // Channel A Tone Period low byte
        break;
            
        case 1: // Channel A Tone Period high nible
        break;
            
        case 2: // Channel B Tone Period low byte
        break;
            
        case 3: // Channel B Tone Period high nible
        break;
            
        case 4: // Channel C Tone Period low byte
        break;
            
        case 5: // Channel C Tone Period high nible
        break;
            
        case 6: // Noise Period
        break;
            
        case 7: // Mixer and I/O control
        break;
            
        case 8: // Channel A Amplitude
        break;
            
        case 9: // Channel B Amplitude
        break;
            
        case 10: // Channel C Amplitude
        break;
            
        case 11: // Envelope Period low byte
        break;

        case 12: // Envelope Period high byte
        break;

        case 13: // Envelope Shape/Cycle
        break;

        case 14: // I/O Port A
            if (!portTypeInputA_[ayNumber_])
                return portValueA_[ayNumber_];
        break;

        case 15: // I/O Port B
            if (!portTypeInputB_[ayNumber_])
                return portValueB_[ayNumber_];
        break;
    }
    return 0;
}

