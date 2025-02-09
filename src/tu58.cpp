/*
 *******************************************************************
 *** This software is copyright 2008 by Marcel van Tongeren      ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 ***                                                             ***
 *** TU58 Code based on emulib by SPARE TIME GIZMOS with         ***
 *** copyright as below                                          ***
 *******************************************************************
*/

//
// TU58.cpp -> DEC TU58 RSP Mass Storage Drive emulation
//
//   COPYRIGHT (C) 2015-2024 BY SPARE TIME GIZMOS.  ALL RIGHTS RESERVED.
//
// LICENSE:
//    This file is part of the emulator library project.  EMULIB is free
// software; you may redistribute it and/or modify it under the terms of
// the GNU Affero General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any
// later version.
//
//    EMULIB is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License
// for more details.  You should have received a copy of the GNU Affero General
// Public License along with EMULIB.  If not, see http://www.gnu.org/licenses/.
//

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/ffile.h"
#include "wx/filefn.h"

#include "main.h"
#include "tu58.h"

Tu58::Tu58()
{
}

void Tu58::configureTu58(Tu58FileConfiguration tu58FileConfiguration, Tu58Configuration tu58Configuration)
{
    tu58FileConfiguration_ = tu58FileConfiguration;
    tu58Configuration_ = tu58Configuration;
//    initializeTu58(tu58FileConfiguration.directory + tu58FileConfiguration.fileName);

    p_Main->message("Configuring TU58 with CDP1854 UART");

    wxString printBuffer;
    printBuffer.Printf("	Disk geometry: %d blocks, block size %d bytes, record size %d bytes\n", tu58Configuration.capacity, tu58Configuration.block, tu58Configuration.record);
    p_Main->message(printBuffer);

    driveCreated_ = wxFile::Exists(tu58FileConfiguration.directory + tu58FileConfiguration.fileName);

    initTu58();
}

/*void Tu58::initializeTu58(wxString fileName)
{
    if (!wxFile::Exists(fileName))
    {
        wxString tu58InstallationFile;
        tu58InstallationFile = fileName.Left(fileName.Len()-4) + "-installation.img";
        
        if (wxFile::Exists(tu58InstallationFile))
            wxRenameFile(tu58InstallationFile, fileName, false);
    }
}*/

void Tu58::initTu58()
{
    recordBuffer_ = (Byte*)malloc(tu58Configuration_.record);

    m_nState = STA_POWERUP;
}

void Tu58::SendSerialBreak(bool fBreak)
{
  //++
  //   This CVirtualConsole method is called when the host wants to change the
  // RS232 BREAK condition status.  The fBreak parameter indicates whether the
  // host has started sending a BREAK (true) or stopped sending one (false).
  //
  //   As long as the host is sending the BREAK the TU58 is forced into the
  // BREAK state.  Once the BREAK is removed we change to the INIT1 state
  // and wait for the host to send an INIT command.
  //
  //   Note that if we're called with fBreak false and we AREN'T currently in
  // the BREAK state, then we should just ignore it.  This isn't supposed to
  // happen, but just in case...
  //--
  if (fBreak) {
//    LOGS(DEBUG, "TU58 BREAK asserted - old state was " << StateToString(m_nState));
    m_nState = STA_BREAK;
  } else if (m_nState == STA_BREAK) {
//    LOGS(DEBUG, "TU58 BREAK deasserted");
    m_nState = STA_INIT1;
  }
}


bool Tu58::TxToHost(uint8_t &bData, int uartNumber)
{
    //++
    //   This routine is called when the host wants to receive data from the us,
    // the TU58.  It figures out what data we want to send and returns it, all
    // while possibly (probably) advancing the state machine accordingly.
    //
    //   Note that it's entirely possible that we may have nothing to send when
    // this routine is called.  If the protocol is currently idle or receiving
    // a packet from the host, then we have nothing to say.  That's not an error,
    // and we should just return false and otherwise do nothing.
    //--
    RSP_STATE OldState = m_nState;
    
    switch (m_nState)
    {
        //   At the completion of the power up cycle, the TU58 sends one CONTINUE
        // byte to the host and then goes to the IDLE state ...
        case STA_POWERUP:
            bData = RSP_F_CONTINUE;
            m_nState = STA_IDLE;
        break;

        //   The REQUESTDATA state is used during a write command to tell the host
        // that we're ready for more data.  We send a CONTINUE and then wait for
        // him to send a data packet ...
        case STA_REQUESTDATA:
            bData = RSP_F_CONTINUE;
            m_nState = STA_WAITDATA;
        break;

        //   In the TXEND1 state we want to send an END packet to the host.  The
        // m_RSPbuffer should be filled in with the complete message, except the
        // checksum.  We first send the flag byte, then switch to the TXEND2 state
        // which will send the rest of the packet (including the checksum).
        case STA_TXEND1:
            bData = m_RSPbuffer.bFlag;
            m_nState = STA_TXEND2;
        break;

        //   And the TXEND2 state sends the rest of the packet, including the job
        // of computing and transmitting a checksum at the end.  Once we're done
        // sending we go back to the IDLE state.
        case STA_TXEND2:
            if (!TxPacketData(bData))
                m_nState = STA_IDLE;
        break;

        //   The TXDATA1 and TXDATA2 states are identical to TXEND1/TXEND2 with
        // the exception of what happens when we're finished sending.  TXEND2 goes
        // to the idle state (because we're done after sending an END), but TXDATA2
        // tries to transmit the next data block.
        case STA_TXDATA1:
            bData = m_RSPbuffer.bFlag;
            m_nState = STA_TXDATA2;
        break;
            
        case STA_TXDATA2:
            if (!TxPacketData(bData))
            {
                if (ReadData()) 
                    m_nState = STA_TXDATA1;
            }
        break;

        //   In the protocol error state, we just send a continuous stream of INITs
        // until the host sends a BREAK.  The state doesn't change.
        case STA_ERROR:
            bData = RSP_F_INITIALIZE;
        break;

        // And in all other states we have nothing to say ...
        default:
        return false;
    }
    return true;
}

void Tu58::RxFromHost (uint8_t bData, int uartNumber)
{
    //++
    //   This routine is called whenever the host sends a byte to us, the TU58.
    // It will deal with the byte (somehow!) and advance the state machine as
    // necessary.  We always have to receive the byte and, depending on what
    // we're doing, at this point receiving something may be a protocol error.
    // For example, if we're in the middle of sending a packet to the host, then
    // he's not supposed to say anything until we're done.
    //
    //   In the event of any protocol error the action is simple - change the
    // state to ERROR and discard the received byte.  The ERROR state will send
    // a continuous stream of INITs to the host and discard any data we receive.
    // The only way out is for the host to send us a BREAK, which will then
    // advance the state to BREAK and re-initialize the protocol.
    //--
    int32_t nRet;  
    RSP_STATE OldState = m_nState;
    switch (m_nState)
    {
        //   In the BREAK state we ignore anything received from the host.  Of
        // course, a real TU58 can't receive anything in this state because the
        // RS232 signal is clamped to a spacing level, but in this simulation we
        // can and commonly do.  That's because most DEC software will set the DL11
        // BREAK bit and then transmit a couple of NULLs for timing, and we'll
        // actually receive those nulls.
        case STA_BREAK:
        return;

        //   In the INIT1 state we're looking for the first INIT from the host,
        // which we'll ignore and go to INIT2.  Then in INIT2 we're looking for
        // the second INIT, which will send us to the POWERUP state.  That'll
        // send a CONTINUE and go to the IDLE state, and we're ready to go.
        case STA_INIT1:
            if (bData == RSP_F_INITIALIZE)
                m_nState = STA_INIT2;
        break;
            
        case STA_INIT2:
            if (bData == RSP_F_INITIALIZE)
            {
                m_nState = STA_POWERUP;
                p_Computer->dataAvailableUart(1, uartNumber);
            }
        break;

        //   In the idle state, the only legal things we can receive are INIT,
        // BOOTSTRAP, or a command packet.  At least I hope that's right!
        case STA_IDLE:
            if (bData == RSP_F_BOOTSTRAP)
            {
                // For BOOTSTRAP we need to wait for the unit number byte ...
                m_nState = STA_RXBOOTSTRAP;
            } 
            else if (bData == RSP_F_INITIALIZE)
            {
                // INIT puts us back into the POWERUP handshaking state ...
                m_nState = STA_POWERUP;
                p_Computer->dataAvailableUart(1, uartNumber);
            }
            else if (bData == RSP_F_CONTROL)
            {
                // Start the process of receiving a command packet ...
                RxPacketStart(bData);
                m_nState = STA_RXCOMMAND;
            } 
            else
            {
                // And anything else is bad!
                // LOGF(WARNING, "TU58 protocol error, received=0x%02X while IDLE", bData);
                m_nState = STA_ERROR;
            }
        break;

        //   In the RXCOMMAND state we accumulate bytes in the buffer until the
        // checksum has been received.  If the checksum is good then we execute the
        // command, and if the checksum is bad then it's off to the ERROR state.
        case STA_RXCOMMAND:
            nRet = RxPacketData(bData);
            if (nRet == 1)
                DoCommand();
            else if (nRet == -1)
                m_nState = STA_ERROR;
        break;

        //   The WAITDATA state is used during write operations to wait for the
        // host to send us another data packet.  The only thing that's legal
        // here is a data packet, and this is the only time a data packet is legal!
        case STA_WAITDATA:
            if (bData == RSP_F_DATA)
            {
                RxPacketStart(bData);
                m_nState = STA_RXDATA;
            } 
            else
            {
                //LOGF(WARNING, "TU58 protocol error, received=0x%02X, when expecting DATA", bData);
                m_nState = STA_ERROR;
            }
        break;

        //   The RXDATA state is essentially the same as RXCOMMAND, however when
        // we're done receiving the packet this time we need to process the data.
        case STA_RXDATA:
            nRet = RxPacketData(bData);
            if (nRet == 1)
                WriteData();
            else if (nRet == -1)
                m_nState = STA_ERROR;
        break;

        //   At this point, anything else we may receive is a protocol error.
        // Change to the ERROR state, which will transmit a continuous stream of
        // INITs to the host.  The only way out is for the host to send us a BREAK,
        // which will reinitialize everything.
        default:
            //LOGF(WARNING, "TU58 protocol error, received=0x%02X, old state=%s", bData, StateToString(m_nState).c_str());
            m_nState = STA_ERROR;
        break;
    }
}

void Tu58::RxPacketStart (uint8_t bFlag)
{
    
}

int32_t Tu58::RxPacketData (uint8_t bData)
{
    return -1;
}

void Tu58::TxPacketStart (uint8_t bFlag, uint8_t bCount)
{
    
}

bool Tu58::TxPacketData (uint8_t &bData)
{
    return false;
}

void Tu58::DoCommand()
{
    
}

void Tu58::WriteData()
{
    
}

bool Tu58::ReadData()
{
    return false;
}
