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

//++
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
// DESCRIPTION:
//   This module emulates a DEC TU58 RSP tape drive.  The TU58 is 1/8"
// cartridge tape drive.  The data is block addressable and block replaceable
// and the TU58 is effectively a mass storage device like a disk or diskette.
// Remember the old Exatron "stringy floppy"?  The TU58 was something like
// that.
//
//   The TU58 connects to a host using a standard asynchronous, RS232, serial
// port.  The protocol used DEC calls RSP, "Radial Serial Protocol" and is
// fairly simple.  The only tricky bit about RSP is that it makes extensive
// use of BREAK (RS232 long space) for synchronizing both ends, and your serial
// port needs to be able to handle both BREAK detection on receive and BREAK
// generation while sending.
//
//   An original DEC TU58 drive contained two transports, unit 0 and unit 1.
// Each tape held 512 blocks of 512 bytes each, for a total tape capacity of
// 256K bytes.  Since it's block addressable and block replaceable, each tape
// is logically a disk drive with one cylinder, one head, and 512 sectors per
// track.  The block/sector size is fairly well fixed at 512 bytes, however
// the RSP protocol can easily accomodate up to 65536 blocks per tape and 256
// units per drive.  It's fairly easy to patch the drivers for RT11 or RSX11
// to extend the standard TU58 tape size, and TU58 emulators commonly allow
// for this.  A TU58 with 65536 sectors has a capacity of 32Mb, which is a
// respectable size for a disk drive.
//
// IMPLEMENTATION NOTES
//   The implementation of this CTU58 object is a bit odd compared to the other
// device emulators you'll find here.  CTU58 is not a CDevice derived object,
// and it implements the CVirtualConsole interface instead.  Remember that the
// real TU58 interfaces to the host via a standard serial port, and CTU58 is
// intended to connect to a UART emulator (INS8250, CDP1854, DC319, etc) which
// is in turn a device connected to the host.  All host control of the TU58 is
// thru the UART interface.
//
//   This also means that the TU58 has no concepts of ports or interrupts.
// Those all belong to the UART we're connected to.  Since CTU58 is not a
// CDevice object this also means that we can't schedule events or get timed
// callbacks.  Instead this module is basically a large state machine that
// implements the RSP protocol.  Timing is entirely driven by polling from the
// UART device.  We take output as fast as the UART gives it to us, and we
// provide input as fast as the UART asks for it.  This is not 100% realistic,
// but most host software doesn't seem to care.
//
//   This object keeps an array (well, actually an std::vector) of CDiskImageFile
// objects, one for each tape unit.  Even though the TU58 is physically a tape
// drive, it acts like a disk and a disk image file is what we want, not a tape.
// Even though the TU58 can have multiple tape units, the drive itself, and the
// RSP protocol, can only do one thing at a time.  That means we only need one
// sector buffer, one protocol buffer, and one set of protocol state variables
// for the entire object.
//
// RSP PROTOCOL NOTES
//   * RSP is essentially half duplex.  There is some handshaking back and
//   forth, but at any moment we're either sending a packet or receiving a
//   packet (or doing nothing!).  Never both at the same time, so one set of
//   buffers will suffice.
//
//   * The host can send us a BREAK (an RS232 space condition longer than one
//   character time) to get our attention and resynchronize the protocol, but
//   we never need to send the host a BREAK.
//
//   * RSP messages are generally either a single byte (INIT, CONTINUE) or they
//   have the same defined packet structure (DATA, COMMAND), EXCEPT for the
//   BOOTSTRAP message.  BOOTSTRAP is two bytes (the flag byte and a unit byte)
//   and does not follow the standard packet format.
//
//   * The RSP command packet is just a special case of the data packet, so
//   the routines that transmit or receive data packets will also suffice
//   for sending or receiving commands or end packets.
//
// STUFF NOT IMPLEMENTED!
//   BOOTSTRAP
//   MRSP
//   XOFF
//   SPECIAL ADDRESSING MODE
//   MAINTENANCE MODE
//   ASSUMES THIS HOST IS LITTLE ENDIAN!
//
//--

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

Tu58::Tu58(uint8_t nUnits) : m_Images(nUnits)
{
    m_nUnits = nUnits;
    m_initialized = false;
}

Tu58::~Tu58()
{
    //++
    //--
    if (!m_initialized)
        return;
    
    assert(m_nUnits > 0);
    DetachAll();
    for (uint16_t i = 0; i < m_nUnits; ++i) delete m_Images[i];
}

void Tu58::configureTu58(Tu58FileConfiguration tu58FileConfiguration[2], Tu58Configuration tu58Configuration)
{
    for (int drive = 0; drive < 2; drive++)
        tu58FileConfiguration_[drive] = tu58FileConfiguration[drive];
    tu58Configuration_ = tu58Configuration;

    p_Main->message("Configuring TU58 connected to CDP1854 UART");

    initTu58();
    m_initialized = true;
}

void Tu58::initTu58()
{
    m_nUnits = 2;
    m_Images.resize(m_nUnits);
    
    for (uint16_t i = 0; i < m_nUnits; ++i) {
      m_Images[i] = DBGNEW CDiskImageFile(RSP_BLOCKSIZE);
    }
    m_nState = STA_POWERUP;
    memset(&m_RSPcommand, 0, sizeof(m_RSPcommand));
    memset(&m_RSPbuffer, 0, sizeof(m_RSPbuffer));
    m_cbRSPpacket = m_bChecksumH = m_bChecksumL = 0;
    m_cbTransfer = m_wCurrentBlock = m_cbSector = 0;
    memset(m_abSector, 0, sizeof(m_abSector));

    wxString fileName;
    char cstringFileName[1024];
    for (int drive = 0; drive < m_nUnits; drive++)
    {
        if (p_Main->getUpdFloppyFile(FDCTYPE_TU58, drive) != "")
        {
            fileName = p_Main->getUpdFloppyDir(FDCTYPE_TU58, drive) + p_Main->getUpdFloppyFile(FDCTYPE_TU58, drive);
            strncpy(cstringFileName, (const char*)fileName.mb_str(wxConvUTF8), 1023);
        
            Attach(drive, cstringFileName, false, tu58Configuration_.numberOfBlocks[drive]);
        }
    }
}

//
// VIRTUAL CONSOLE ROUTINES
//

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
    LOGF(DEBUGLVL, "TU58 BREAK asserted - old state was " + StateToString(m_nState));
    m_nState = STA_BREAK;
  } else if (m_nState == STA_BREAK) {
    LOGF(DEBUGLVL, "TU58 BREAK deasserted");
    m_nState = STA_INIT1;
  }
}

//
// RECEIVER ROUTINES
//

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
    m_uartNumber = uartNumber;
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
                //p_Computer->dataAvailableUart(1, uartNumber);
            }
            else if (bData == RSP_F_CONTROL)
            {
                // Start the process of receiving a command packet ...
                RxPacketStart(bData);
                m_nState = STA_RXCOMMAND;
                //p_Computer->dataAvailableUart(1, uartNumber);
            }
            else
            {
                // And anything else is bad!
                LOGF(WARNING, "TU58 protocol error, received=0x%02X while IDLE", bData);
                m_nState = STA_ERROR;
            }
        break;

        //   In the RXCOMMAND state we accumulate bytes in the buffer until the
        // checksum has been received.  If the checksum is good then we execute the
        // command, and if the checksum is bad then it's off to the ERROR state.
        case STA_RXCOMMAND:
            nRet = RxPacketData(bData);
            if (nRet == 1)
            {
                DoCommand();
                //p_Computer->dataAvailableUart(1, uartNumber);
            }
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
                p_Computer->dataAvailableUart(1, m_uartNumber);
                m_nState = STA_RXDATA;
            }
            else
            {
                LOGF(WARNING, "TU58 protocol error, received=0x%02X, when expecting DATA", bData);
                m_nState = STA_ERROR;
            }
        break;

        //   The RXDATA state is essentially the same as RXCOMMAND, however when
        // we're done receiving the packet this time we need to process the data.
        case STA_RXDATA:
            nRet = RxPacketData(bData);
            if (nRet == 1)
            {
                WriteData();
                p_Computer->dataAvailableUart(1, m_uartNumber);
            }
            else if (nRet == -1)
                m_nState = STA_ERROR;
        break;

        //   At this point, anything else we may receive is a protocol error.
        // Change to the ERROR state, which will transmit a continuous stream of
        // INITs to the host.  The only way out is for the host to send us a BREAK,
        // which will reinitialize everything.
        default:
            LOGF(WARNING, "TU58 protocol error, received=0x%02X, old state=", bData, StateToString(m_nState));
            m_nState = STA_ERROR;
        break;
    }
    LOGF(TRACE, "TU58 receive from host 0x%02X - old state " + StateToString(OldState) + " - new state " + StateToString(m_nState), bData);
}

void Tu58::RxPacketStart (uint8_t bFlag)
{
    //++
    //   This method is called when we need to start receiving either a data or
    // a command packet.  It will zero the packet buffer (just in case), store
    // the flag byte in the buffer, and initialize the received byte counter.
    //--
    memset(&m_RSPbuffer, 0, sizeof(m_RSPbuffer));
    m_RSPbuffer.bFlag = bFlag;  m_cbRSPpacket = 1;
}

int32_t Tu58::RxPacketData (uint8_t bData)
{
    //++
    //   And this routine is called for each byte in a command or data packet
    // after the first (flag) byte.  It will store the bytes received in the
    // m_RSPbuffer and, when we get to the end, it will receive ad verify the
    // checksum.  It returns 0 if the packet isn't finished yet (i.e. this is
    // just another data byte in the middle), +1 if this is the end of the packet
    // AND the checksum is good, or -1 if this is the end and the checksum is
    // bad.
    //
    //   Note that the second byte of the packet, which is always the first
    // one we get here, tells us the length of the packet!
    //--
    assert(m_cbRSPpacket > 0);
    if (m_cbRSPpacket == 1) {
      // Second packet byte - this is the payload count ...
      m_RSPbuffer.bCount = bData;  ++m_cbRSPpacket;  return 0;
    } else if (m_cbRSPpacket < (m_RSPbuffer.bCount+2)) {
      // All the payload bytes come here ...
      m_RSPbuffer.abData[m_cbRSPpacket-2] = bData;  ++m_cbRSPpacket;  return 0;
    } else if (m_cbRSPpacket == (m_RSPbuffer.bCount+2)) {
      // First checksum byte  ...
      m_bChecksumL = bData;  ++m_cbRSPpacket;  return 0;
    } else {
      // Second checksum byte (and the end of the packet!) ...
      m_bChecksumH = bData;  ++m_cbRSPpacket;
      uint16_t wChecksum = ComputeChecksum(&m_RSPbuffer);
      if (wChecksum == MKWORD(m_bChecksumH, m_bChecksumL)) return 1;
      LOGF(WARNING, "TU58 received bad checksum 0x%02X%02X != 0x%04X", m_bChecksumH, m_bChecksumL, wChecksum);
      return -1;
    }
}

//
// TRANSMITTER ROUTINES
//

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
    m_uartNumber = uartNumber;

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
            p_Computer->dataAvailableUart(1, m_uartNumber);
            m_nState = STA_TXEND2;
        break;

        //   And the TXEND2 state sends the rest of the packet, including the job
        // of computing and transmitting a checksum at the end.  Once we're done
        // sending we go back to the IDLE state.
        case STA_TXEND2:
            if (!TxPacketData(bData))
                m_nState = STA_IDLE;
            else
                p_Computer->dataAvailableUart(1, m_uartNumber);
        break;

        //   The TXDATA1 and TXDATA2 states are identical to TXEND1/TXEND2 with
        // the exception of what happens when we're finished sending.  TXEND2 goes
        // to the idle state (because we're done after sending an END), but TXDATA2
        // tries to transmit the next data block.
        case STA_TXDATA1:
            bData = m_RSPbuffer.bFlag;
            p_Computer->dataAvailableUart(1, m_uartNumber);
            m_nState = STA_TXDATA2;
        break;
            
        case STA_TXDATA2:
            if (!TxPacketData(bData))
            {
                if (ReadData()) 
                {
                    m_nState = STA_TXDATA1;
                }
            }
            p_Computer->dataAvailableUart(1, m_uartNumber);
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
    LOGF(TRACE, "TU58 send to host 0x%02X - old state " + StateToString(OldState) + " - new state " + StateToString(m_nState), bData);
    return true;
}

void Tu58::TxPacketStart (uint8_t bFlag, uint8_t bCount)
{
    //++
    //   This routine sets things up to start transmitting either a data packet
    // or a command packet to the host.  The caller must specify the flag byte
    // (which must be either RSP_F_CONTROL or RSP_F_DATA) and the payload byte
    // count.  Note that the latter does not include the flag or count bytes
    // themselves.
    //
    //   WARNING - this zeros the RSP buffer, so be sure to call it BEFORE you
    // start populating the message!
    //
    //   Oh, and FWIW, the only command/control packet that a TU58 can transmit
    // to the host is END.
    //--
    memset(&m_RSPbuffer, 0, sizeof(m_RSPbuffer));
    m_RSPbuffer.bFlag = bFlag;  m_RSPbuffer.bCount = bCount;
    m_cbRSPpacket = 1;
}

bool Tu58::TxPacketData (uint8_t &bData)
{
    //++
    //   And this routine is called after TxPacketStart().  It will dole out
    // all the remaining bytes in the packet, one at a time, until we've sent
    // the entire thing.  At the very end it will compute the packet checksum
    // and transmit that too.  When everything has been sent, including the
    // checksum bytes, it returns false.
    //--
    if (m_cbRSPpacket == 1) {
      // Send the count byte ...
      bData = m_RSPbuffer.bCount;  ++m_cbRSPpacket;  return true;
    } else if (m_cbRSPpacket < (m_RSPbuffer.bCount+2)) {
      // Here to send all the payload bytes ...
      bData = m_RSPbuffer.abData[m_cbRSPpacket-2];  ++m_cbRSPpacket;  return true;
    } else if (m_cbRSPpacket == (m_RSPbuffer.bCount+2)) {
      // Compute the checksum and send the first (low) checksum byte ...
      uint16_t wChecksum = ComputeChecksum(&m_RSPbuffer);
      m_bChecksumH = HIBYTE(wChecksum);  m_bChecksumL = LOBYTE(wChecksum);
      bData = m_bChecksumL;  ++m_cbRSPpacket;  return true;
    } else {
      // Send the last (high) checksum byte and we're done ...
      bData = m_bChecksumH;  ++m_cbRSPpacket;  return false;
    }
}

void Tu58::TxEndPacket (uint8_t bSuccess, uint16_t wCount)
{
  //++
  //   This method sets things up to send an END packet up to the host.  The
  // bSuccess parameter is the success code for the packet (zero is no errors!)
  // and wCount is the actual byte count we'll send.  This routine also copies
  // the unit number from m_RSPcommand, and the remainder of the END packet is
  // left zero.
  //
  //   BTW, the END packet is the only command/control packet that the TU58
  // can send to the host.
  //--
  RSP_COMMAND *pEnd = (RSP_COMMAND *) &m_RSPbuffer.abData;
  TxPacketStart(RSP_F_CONTROL, sizeof(RSP_COMMAND));
  pEnd->bOpcode   = RSP_O_END;
  pEnd->bModifier = bSuccess;
  pEnd->bUnit     = m_RSPcommand.bUnit;
  pEnd->wCount    = wCount;
  LOGF(DEBUGLVL, "TU58 sending END, error=%d, count=%d", bSuccess, wCount);
  m_nState = STA_TXEND1;
}

//
// TU58 COMMANDS
//

bool Tu58::CheckUnit (bool fWrite)
{
  //++
  //   This method will verify that the unit number selected by the current
  // command in m_RSPcommand is a) valid and b) online.  It also verifies that
  // the initial block number given in the command is legal and, if the fWrite
  // parameter is true, that the drive is not write locked.  If any of those
  // tests fail then we'll send an END packet with the appropriate error code
  // and return false.
  //
  //   Remember that read and write operations can span multiple blocks and
  // this method only validates the starting block.  The WriteData() and
  // ReadData() routines still need to verify that the block is valid if they
  // need to increment it.
  //
  //   This method also checks for a couple of things that are legal but which
  // we don't currently implement, including special addressing mode and
  // maintentance mode.  Special addressing mode will return an END packet with
  // the "seek error" status, but maintenance mode just prints a warning and
  // is otherwise ignored.
  //--
  uint8_t nUnit = m_RSPcommand.bUnit;

  // Check that the unit number is valid and online ..
  if (nUnit >= m_nUnits) {
    TxEndPacket(RSP_E_BADUNIT);  return false;
  } else if (!IsAttached(nUnit)) {
    TxEndPacket(RSP_E_NOTAPE);  return false;
  }

  // Check the starting block number and return BADBLOCK if it's illegal ...
  if (m_RSPcommand.wBlock >= GetCapacity(nUnit)) {
    TxEndPacket(RSP_E_BADBLOCK);  return false;
  }

  // If this is a WRITE command, make sure the drive is writable ...
  if (fWrite && IsReadOnly(nUnit)) {
    TxEndPacket(RSP_E_WLOCK);  return false;
  }

  // Check for special addressing mode.  That's legal, but we don't do it...
  if (ISSET(m_RSPcommand.bModifier, RSP_M_SAM)) {
    LOGF(WARNING, "TU58 special addressing mode selected on unit %d", nUnit);
    TxEndPacket(RSP_E_SEEKFAIL);  return false;
  }

  //   And check for maintenance mode - this is supposed to report tape read
  // errors back to the host.  Since we don't every have any tape errors we
  // can safely ignore this, but we'll print a warning message.
  if (ISSET(m_RSPcommand.bSwitches, RSP_S_MAINTENANCE))
  {
    LOGF(WARNING, "TU58 maintenance mode selected on unit %d", nUnit);
  }
  // Everything looks good!
  return true;
}

void Tu58::DoCommand()
{
    //++
    //--

    // Verify that the packet size is correct.  Commands are always 10 bytes ...
    if (m_RSPbuffer.bCount != sizeof(RSP_COMMAND)) {
      LOGF(WARNING, "TU58 command packet has wrong length - %d", m_RSPbuffer.bCount);
      m_nState = STA_ERROR;
      return;
    }

    // Copy the command from the data buffer to m_RSPcommand for later use...
    memcpy(&m_RSPcommand, &m_RSPbuffer.abData, sizeof(RSP_COMMAND));
    LOGF(DEBUGLVL, "TU58 command 0x%02X, unit=%d, block=%d, count=%d", m_RSPcommand.bOpcode, m_RSPcommand.bUnit, m_RSPcommand.wBlock, m_RSPcommand.wCount);

    // And figure out what needs to happen next!
    switch (m_RSPcommand.bOpcode) {

      //   For WRITE we initialize our internal counters and then ask the host to
      // send us the first data block.  That eventually ends up in WriteData(),
      // which will save it to the image file ...
      case RSP_O_WRITE:
        if (CheckUnit(true)) {
          m_cbSector = m_cbTransfer = 0;  m_wCurrentBlock = m_RSPcommand.wBlock;
          m_nState = STA_REQUESTDATA;
        }
        break;

      //   The READ command is similar, however this time we just send data to
      // host as fast as we can; there's no handshaking with CONTINUE as there is
      // for WRITE.  We need to explicitly call ReadData() here to send the first
      // record, and that will automatically recycle until all requested records
      // have been sent.  Note that we initialise m_cbSector to RSP_BLOCKSIZE
      // here to force ReadData() read the first block from the image!
      case RSP_O_READ:
        if (CheckUnit()) {
          m_cbSector = RSP_BLOCKSIZE;  m_wCurrentBlock = m_RSPcommand.wBlock;
          m_cbTransfer = 0;  ReadData();  m_nState = STA_TXDATA1;
          p_Computer->dataAvailableUart(1, m_uartNumber);
        }
        break;

      //   The POSITION command just seeks the tape to a particular block.
      // That's pretty much useless here, and all we do is verify that the unit
      // and block number are valid and then send an END packet with success.
      case RSP_O_POSITION:
        if (CheckUnit()) TxEndPacket(RSP_E_SUCCESS);
        break;

      //   These commands all return an END packet with a success status.
      // They're all either NOPs or not implemented (even on a real TU58!)
      case RSP_O_NOP:
      case RSP_O_INITIALIZE:
      case RSP_O_DIAGNOSE:
      case RSP_O_GETSTATUS:
      case RSP_O_SETSTATUS:
        TxEndPacket(RSP_E_SUCCESS);  break;

      // All other commands return an END packet with the INVALID OPCODE status.
      default:
        LOGF(WARNING, "TU58 unimplemented opcode 0x%02X", m_RSPcommand.bOpcode);
        TxEndPacket(RSP_E_BADOPCODE);
        break;
    }
}

void Tu58::WriteData()
{
    //++
    //   This method is called for every data packet while the host is writing
    // to the TU58.  This is slightly complicated because the host is sending
    // data in 128 byte blocks, and we have to pack up four of those to make a
    // 512 bit image sector/block.  Every four data packets we can write a new
    // sector to the image.
    //
    //   The protocol allows for multiple 512 byte blocks to be written with a
    // single WRITE command and this does happen on occasion.  RSP actually
    // allows for up to 64K bytes to be transferred in a single WRITE command.
    // This would require 512 of the 128 byte data packets and would write 128
    // disk sectors.  I wouldn't recommend trying...
    //
    //   FWIW, the real TU58 tapes actually have 128 byte records, but there's
    // nothing in the protocol to write an individual record rather an group
    // of four.  If the byte count given in the WRITE command is not a multiple
    // of 512 then the TU58 will zero fill the remaining bytes and records in
    // the four record grouping.
    //
    //   When we reach the end of the transfer, this routine automatically
    // generates and starts sending an END packet.
    //--
    bool fNeedWrite = false;  uint8_t nUnit = m_RSPcommand.bUnit;
    LOGF(DEBUGLVL, "TU58 received data, length=%d", m_RSPbuffer.bCount);

    // Stuff this packet into the sector buffer ...
    uint16_t cbFree = RSP_BLOCKSIZE - m_cbSector;
    uint16_t cbCopy = MIN(cbFree, m_RSPbuffer.bCount);
    if (cbCopy < m_RSPbuffer.bCount)
    {
      LOGF(WARNING, "TU58 data truncated, packet size=%d, sector free=%d", m_RSPbuffer.bCount, cbFree);
    }
    memcpy(&m_abSector[m_cbSector], &m_RSPbuffer.abData, cbCopy);
    m_cbSector += cbCopy;  m_cbTransfer += m_RSPbuffer.bCount;
    if (m_cbSector >= RSP_BLOCKSIZE) fNeedWrite = true;

    //   See if we've finished the entire transfer.  If we have, then send an END
    // packet now, and if we haven't then ask for more data ...
    if (m_cbTransfer >= m_RSPcommand.wCount) {
      LOGF(DEBUGLVL, "TU58 write operation finished, %d bytes transferred", m_cbTransfer);
      TxEndPacket(RSP_E_SUCCESS, m_cbTransfer);
      fNeedWrite = true;
    } else
      // Send a CONTINUE to ask the host to send more data ...
      m_nState = STA_REQUESTDATA;

    //   If we need to write this block to the image file, then do that too.
    // Note that if we've run off the end of the tape, then this is where we
    // notice and send an END packet with an error status.
    if (fNeedWrite) {
      if (m_wCurrentBlock >= GetCapacity(nUnit)) {
        TxEndPacket(RSP_E_EOT, m_cbTransfer);
      } else {
        LOGF(DEBUGLVL, "TU58 writing block %d to image", m_wCurrentBlock);
        if (m_Images[nUnit]->WriteSector(m_wCurrentBlock, m_abSector)) {
          ++m_wCurrentBlock;  m_cbSector = 0;  memset(m_abSector, 0, sizeof(m_abSector));
        } else
          TxEndPacket(RSP_E_DATACHECK, m_cbTransfer);
      }
    }
}

bool Tu58::ReadData()
{
    //++
    //   This method is called when the host wants to read data from the TU58
    // tape.  We have the same problem here that we do when writing in that the
    // image sectors are 512 bytes but the RSP data packets only hold 128 bytes.
    // As with WRITE, it's also possible to read more than one 512 byte block
    // in a single transfer, and it's also possible to read less than 512 bytes.
    // In the latter case we simply discard the unused data.
    //
    //   When the transfer is finished, this routine will automatically generate
    // and start sending an END packet.
    //--
    uint8_t nUnit = m_RSPcommand.bUnit;
    
    //   Figure out how many more bytes we need to send first.  If it's zero,
    // then send an END packet now and quit ...
    assert(m_cbTransfer <= m_RSPcommand.wCount);
    uint16_t cbRemaining = m_RSPcommand.wCount - m_cbTransfer;
    if (cbRemaining == 0) {
      TxEndPacket(RSP_E_SUCCESS, m_cbTransfer);
      return false;
    }

    //   If we need to read another block from the image file, do it now.  Note
    // that if we've run off the end of the tape then this is where we notice and
    // send an END packet with an error status ...
    if (m_cbSector >= RSP_BLOCKSIZE) {
      if (m_wCurrentBlock >= GetCapacity(nUnit)) {
        TxEndPacket(RSP_E_EOT);  return false;
      } else {
        LOGF(DEBUGLVL, "TU58 reading image block %d", m_wCurrentBlock);
        if (m_Images[nUnit]->ReadSector(m_wCurrentBlock, m_abSector)) {
          ++m_wCurrentBlock;  m_cbSector = 0;
        } else {
          TxEndPacket(RSP_E_DATACHECK, m_cbTransfer);  return false;
        }
      }
    }

    // Fill up the RSP data packet with as many bytes as we can ...
    uint16_t cbData = MIN(cbRemaining, RSP_RECORDSIZE);
    TxPacketStart(RSP_F_DATA, LOBYTE(cbData));
    memcpy(&m_RSPbuffer.abData, &m_abSector[m_cbSector], cbData);
    m_cbSector += cbData;  m_cbTransfer += cbData;

    // Ok, send this data packet to the host ...
    LOGF(DEBUGLVL, "TU58 sending data, length=%d", m_RSPbuffer.bCount);
    m_nState = STA_TXDATA1; 
    p_Computer->dataAvailableUart(1, m_uartNumber);
    return true;
}

//
// IMAGE FILE ROUTINES
//

bool Tu58::Attach (uint8_t nUnit, const string &sFileName, bool fReadOnly, uint32_t lCapacity)
{
  //++
  //   This method will attach one TU58 unit to an image file.  If opening the
  // image file is successful, then the drive immediately becomes online for
  // the host and we return true.  If opening the image file fails for any
  // reason, then we return false.
  //
  //   Unlike IDE drives, TU58s have a write protect switch on each cartridge,
  // and a virtual unit will be write protected if the fReadOnly parameter is
  // true.  Note that the CImageFile::Open() method also checks the file
  // permissions and if the file itself is read only then the unit will also be
  // set read only, regardless of our fReadOnly parameter.  The only way to
  // find out for sure is to call the IsReadOnly() method on the unit after
  // it has been opened.
  //
  //   And it's also possible to set the tape capacity here.  A real TU58 tape
  // is exactly 262,144 bytes (512 blocks of 512 bytes),  but the RSP protocol
  // allows for up to 65536 blocks per tape.  That's 32Mb per tape!  If the
  // lCapacity parameter is zero then we'll use the current file size to set
  // the tape capacity, UNLESS the file is being created in which case we'll
  // set the file size to the TU58 standard 512 blocks.
  //
  //   However if fCapacity is NOT zero then is specifies the minimum tape
  // size, IN BLOCKS (NOT bytes!).  If the file opened is already that big or
  // bigger, then we'll leave it alone.  But if the file is smaller than the
  // lCapacity specified, or if the file is being created now, then we'll
  // extend it to the size specified.
  //--
  assert(nUnit < m_nUnits);
  assert(!sFileName.empty());

  // Try to open the image file ...
  if (IsAttached(nUnit)) Detach(nUnit);
  if (!m_Images[nUnit]->Open(sFileName, fReadOnly)) return false;

  // Set the tape capacity as necessary ...
  if (GetCapacity(nUnit) == 0) {
    // This is an empty file, so it was probably just created ...
    m_Images[nUnit]->SetCapacity((lCapacity != 0) ? lCapacity : RSP_CAPACITY);
  } else if (lCapacity > GetCapacity(nUnit)) {
    // The file exists, but it's too short - extend it ...
    m_Images[nUnit]->SetCapacity(lCapacity);
  }

  // Success!
  LOGF(DEBUGLVL, "TU58 unit %d attached to " + GetFileName(nUnit), nUnit);
  LOGF(DEBUGLVL, "TU58 unit %d capacity %d blocks", nUnit, GetCapacity(nUnit));
  return true;
}

void Tu58::Detach (uint8_t nUnit)
{
  //++
  // Take the unit offline and close the image file associated with it.
  //--
  assert(nUnit < m_nUnits);
  if (!IsAttached(nUnit)) return;
  LOGF(DEBUGLVL, "TU58 unit %d detached from " + GetFileName(nUnit), nUnit);
  m_Images[nUnit]->Close();
}

void Tu58::DetachAll()
{
  //++
  // Detach ALL units ...
  //--
  for (uint8_t i = 0;  i < m_nUnits;  ++i) {
    if (IsAttached(i)) Detach(i);
  }
}


//
// MISCELLANEOUS ROUTINES
//

uint16_t Tu58::ComputeChecksum (const RSP_DATA* pPacket)
{
  //++
  //   This method computes a sixteen bit TU58 style checksum of the buffer
  // specified.  The TU58 computes the sum of pairs of bytes in the buffer,
  // with end around carry from the MSB back to the LSB.  It's not clear what
  // the TU58 does if the buffer contains an odd number of bytes, but here we
  // just pad the last byte with zeros to make a full word.  All real TU58
  // messages have an even length, so in theory this issue should never come
  // up.
  //
  //   Note that we get the length of the payload from the packet header...
  //--
  uint32_t lChecksum = MKWORD(pPacket->bCount, pPacket->bFlag);
  //uint8_t cbPayload = pPacket->bCount;
  for (uint8_t i = 0; i < pPacket->bCount; i += 2) {
    lChecksum += MKWORD(pPacket->abData[i + 1], pPacket->abData[i]);
    if (HIWORD(lChecksum) != 0)
      lChecksum = (lChecksum + 1) & 0xFFFF;
  }
  return LOWORD(lChecksum);
}

wxString Tu58::StateToString (RSP_STATE nState)
{
  //++
  //   Convert an RSP state number into a meaningful ASCII string.  This
  // implementation is pretty simple minded, but it's really useful for
  // debugging.
  //--
    wxString returnString;
  switch (nState) {
    case STA_POWERUP:     return "POWERUP";
    case STA_IDLE:        return "IDLE";
    case STA_BREAK:       return "BREAK";
    case STA_INIT1:       return "INIT1";
    case STA_INIT2:       return "INIT2";
    case STA_RXCOMMAND:   return "RX COMMAND";
    case STA_RXDATA:      return "RX DATA";
    case STA_REQUESTDATA: return "REQUEST DATA";
    case STA_WAITDATA:    return "WAIT FOR DATA";
    case STA_RXBOOTSTRAP: return "RX BOOT";
    case STA_TXBOOTSTRAP: return "TX BOOT";
    case STA_TXEND1:      return "TX END1";
    case STA_TXEND2:      return "TX END2";
    case STA_TXDATA1:     return "TX DATA1";
    case STA_TXDATA2:     return "TX DATA2";
    case STA_ERROR:       return "ERROR";
          
    default:
          returnString.Printf("%d", nState);
    return returnString;
  }
}

wxString Tu58::LevelToString (SEVERITY nLevel)
{
  //++
  //   Return a simple string corresponding to nLevel.   This is used to
  // put the message level into the log file...
  //--
  switch (nLevel) {
    case CMDOUT:  return string("CMDOUT");
    case CMDERR:  return string("CMDERR");
    case TRACE:   return string("TRACE");
    case DEBUG:   return string("DEBUG");
    case WARNING: return string("WARN");
    case ERROR:   return string("ERROR");
    case ABORT:   return string("ABORT");
    default:      return string("UNKNOWN");
  }
}

void Tu58::LOGF(SEVERITY level, wxString text, int data, wxString extraString)
{
    return;
    
    text = text + extraString;
    text.Printf(text, data);

    text = LevelToString(level) + " " + text;
    p_Main->eventShowTextMessage(text);
}

void Tu58::LOGF(SEVERITY level, wxString text, int data1, int data2, int data3, int data4)
{
    return;

    if (data1 != -1 && data2 == -1 && data3 == -1 && data4 == -1)
        text.Printf(text, data1);
    if (data1 != -1 && data2 != -1 && data3 == -1 && data4 == -1)
        text.Printf(text, data1, data2);
    if (data1 != -1 && data2 != -1 && data3 != -1 && data4 == -1)
        text.Printf(text, data1, data2, data3);
    if (data1 != -1 && data2 != -1 && data3 != -1 && data4 != -1)
        text.Printf(text, data1, data2, data3, data4);

    text = LevelToString(level) + " " + text;
    p_Main->eventShowTextMessage(text);
}


