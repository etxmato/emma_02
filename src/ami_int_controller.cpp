/*
 *******************************************************************
 *** AMI 9217MLB Interrupt Controller Emulation                  ***
 ***                                                             ***
 *** This software is copyright 2026 by Marcel van Tongeren      ***
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

#include "main.h"
#include "ami_int_controller.h"

AmiIntController::AmiIntController()
{
    reset();
}

void AmiIntController::configure(AmiIntControllerConfiguration config)
{
    config_ = config;
    reset();
    
    wxString printBuffer;
    p_Main->message("AMI Interrupt Controller");
    printBuffer.Printf("	Cause/ACK register address: %04X, address mask: %04X",
                       config_.causeRegister, config_.causeRegisterMask);
    p_Main->message(printBuffer);
    printBuffer.Printf("	Cause multiplier: %d, number of sources: %d",
                       config_.causeMultiplier, config_.numberOfSources);
    p_Main->message(printBuffer);
    p_Main->message("");
}

void AmiIntController::reset()
{
    for (int i = 0; i < AMI_INT_MAX_SOURCES; i++)
    {
        pending_[i] = false;
        enabled_[i] = true;     // All sources enabled by default
    }
}

void AmiIntController::requestInterrupt(int sourceNumber)
{
    if (sourceNumber >= 0 && sourceNumber < config_.numberOfSources)
    {
        pending_[sourceNumber] = true;
    }
}

void AmiIntController::clearInterrupt(int sourceNumber)
{
    if (sourceNumber >= 0 && sourceNumber < config_.numberOfSources)
    {
        pending_[sourceNumber] = false;
    }
}

Byte AmiIntController::readCauseRegister()
{
    int source = findHighestPriority();
    if (source < 0)
        return 0;   // No pending interrupt — return 0 (maps to timer/default handler)
    
    return (Byte)(source * config_.causeMultiplier);
}

void AmiIntController::writeAckRegister(Byte value)
{
    // ACK value = 1 << source_number
    // Clear the corresponding source(s)
    for (int i = 0; i < config_.numberOfSources; i++)
    {
        if (value & (1 << i))
        {
            pending_[i] = false;
        }
    }
}

bool AmiIntController::isInterruptPending()
{
    return (findHighestPriority() >= 0);
}

void AmiIntController::enableSource(int sourceNumber, bool enable)
{
    if (sourceNumber >= 0 && sourceNumber < AMI_INT_MAX_SOURCES)
    {
        enabled_[sourceNumber] = enable;
    }
}

bool AmiIntController::matchesAddress(Word address)
{
    return (address & config_.causeRegisterMask) == (config_.causeRegister & config_.causeRegisterMask);
}

int AmiIntController::findHighestPriority()
{
    // Higher source number = higher priority
    // (keyboard=1 > timer=0, serial=3 > keyboard=1, etc.)
    for (int i = config_.numberOfSources - 1; i >= 0; i--)
    {
        if (pending_[i] && enabled_[i])
            return i;
    }
    return -1;
}
