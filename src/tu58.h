#ifndef TU58_H
#define TU58_H

#include "computerconfig.h"
#include "ImageFile.hpp"        // image file methods

//   Sadly, windef.h defines these macros too - fortunately Microsoft's
// definition is compatible with ours, so it's not a fatal problem.
// Unfortunately, Microsoft's header doesn't check to see whether these
// things are defined before redefining them, and that leads to a
// compiler error message.
#if !(defined(_WIN32) && defined(LOBYTE))
#define LOBYTE(x)     ((uint8_t)  ((x) & 0xFF))
#define HIBYTE(x)     ((uint8_t)  (((x) >> 8) & 0xFF))
#define LOWORD(x)     ((uint16_t  ( (x) & 0xFFFF)))
#define HIWORD(x)    ((uint16_t) (((x) & 0xFFFF0000L) >> 16) )
#endif

// Assemble and disassemble nibbles, bytes, words and longwords...
#define MKWORD(h,l)    ((uint16_t) ((((h) & 0xFF) << 8) | ((l) & 0xFF)))

// Bit set, clear and test macros ...
#define ISSET(x,b)    (((x) & (b)) != 0)

// Useful arithmetic macros ...
#define MIN(a,b)  ((a) < (b) ? (a) : (b))

//   Define the C++ new operator to use the debug version. This enables tracing
// of memory leaks via _CrtDumpMemoryLeaks(), et al.  It'd be really nice if
// Visual Studio was smart enough to just do this for us, but it isn't ...
#if defined(_DEBUG) && defined(_MSC_VER)
#define DBGNEW new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
#define DBGNEW new
#endif

class Tu58
{
    // RSP magic numbers, bits and structures ...
public:
    enum {
      // TU58 General Parameters ...
      RSP_CAPACITY      =  512, // standard TU58 holds 512 blocks
      RSP_BLOCKSIZE     =  512, // and a standard block is 512 bytes
      RSP_RECORDSIZE    =  128, // but individual tape records are 128 bytes
      // TU58 Packet Types ...
      //   NOTE: in the TU58 documentation, these are referred to as "flags"!
      RSP_F_DATA        =   1,  // data packets for read/write
      RSP_F_CONTROL     =   2,  // control (command) packet
      RSP_F_INITIALIZE  =   4,  // forces controller initialization
      RSP_F_BOOTSTRAP   =   8,  // sends unit 0/block 0 w/o RSP
      RSP_F_CONTINUE    =  16,  // continue with the next data packet
      RSP_F_XOFF        =  19,  // pause sending to the host
      // TU58 Command OpCodes ...
      RSP_O_NOP         =   0,  // no operation (returns an END packet)
      RSP_O_INITIALIZE  =   1,  // (re)intialize the drive
      RSP_O_READ        =   2,  // reads data from the tape
      RSP_O_WRITE       =   3,  // writes data to the tape
      RSP_O_POSITION    =   5,  // positions the tape at a given block
      RSP_O_DIAGNOSE    =   7,  // executes built in self test
      RSP_O_GETSTATUS   =   8,  // (treated as a NOP by TU58)
      RSP_O_SETSTATUS   =   9,  //     "    "  "  "   "   "
      RSP_O_END         =  64,  // acknowledgement (end) packet type
      // TU58 Command Modifiers ...
      RSP_M_SAM         = 128,  // selects special addressing mode
      RSP_M_CHECK       =   1,  // decreased sensitivity (read command)
                                // read and verify (write command)
      // TU58 Switches Flags ...
      RSP_S_MRSP        =   8,  // MRSP mode ON
      RSP_S_MAINTENANCE =  16,  // maintenance (inhibit read retries) mode
      // TU58 Error Codes ...
      RSP_E_SUCCESS     =   0,  // operation completed without errors
      RSP_E_WRETRY      =   1,  // success, but retries were needed
      RSP_E_POSTFAIL    = 255,  // self test failed
      RSP_E_EOT         = 254,  // partial operation - end of tape
      RSP_E_BADUNIT     = 248,  // unit number not 0 or 1
      RSP_E_NOTAPE      = 247,  // no cartridge in drive
      RSP_E_WLOCK       = 245,  // tape is write protected
      RSP_E_DATACHECK   = 239,  // bad data (checksum failure)
      RSP_E_SEEKFAIL    = 224,  // block not found (bad tape format?)
      RSP_E_JAMMED      = 223,  // motor stopped (jammed cartridge?)
      RSP_E_BADOPCODE   = 208,  // bad opcode in command packet
      RSP_E_BADBLOCK    = 201,  // block number .GT. 511
    };

    // RSP Command packet structure...
    struct _RSP_COMMAND {
      uint8_t  bOpcode;         // operation code
      uint8_t  bModifier;       // modifier flags
      uint8_t  bUnit;           // drive unit number requested
      uint8_t  bSwitches;       // special mode selections
      uint16_t wSequence;       // sequence number
      uint16_t wCount;          // byte count
      uint16_t wBlock;          // logical block number
    };
    typedef struct _RSP_COMMAND RSP_COMMAND;

    // RSP Data packet structure ...
    struct _RSP_DATA {
      uint8_t bFlag;                  // what RSP calls the flag byte
      uint8_t bCount;                 // message length (NOT counting flag/count)
      uint8_t abData[RSP_RECORDSIZE]; // the actual data being sent
    };
    typedef struct _RSP_DATA RSP_DATA;

    enum _RSP_STATES {
      STA_POWERUP,              // initial power up state - send CONTINUE
      STA_IDLE,                 // waiting for a command fromt the host
      STA_BREAK,                // host is currently sending BREAK
      STA_INIT1,                // waiting for the first INIT command
      STA_INIT2,                //  "  "    "   "  second  "     "
      STA_RXCOMMAND,            // receiving a command packet
      STA_RXDATA,               //  "    "   " data      "
      STA_REQUESTDATA,          // send CONTINUE, ask for data packet
      STA_WAITDATA,             // CONTINUE sent, wait for data packet
      STA_TXEND1,               // start transmitting an END packet
      STA_TXEND2,               // finish  "     "    "   "     "
      STA_TXDATA1,              // start transmitting a data packet
      STA_TXDATA2,              // finish  "     "    "  "     "
      STA_RXBOOTSTRAP,          // BOOTSTRAP received, waiting for unit number
      STA_TXBOOTSTRAP,          // transmitting bootstrap packet
      STA_ERROR,                // protocol error - send INIT continuously
    };
    typedef enum _RSP_STATES RSP_STATE;

  enum _XSTATES {
    // XMODEM state machine states ...
    XIDLE,                      // no XMODEM transfer in progress
                                // Receiver states ...
    SEND_NAK_START,             //  send a NAK and wait to receive data
    WAIT_BLOCK,                 //  waiting for SOH or EOT
    WAIT_BLKNO_1,               //  waiting for first byte of block number
    WAIT_BLKNO_2,               //   "    "  second  "   "   "     "
    WAIT_DATA,                  //  waiting for 128 byte data block
    WAIT_CKSUM,                 //  waiting for checksum byte
    SEND_ACK,                   //  send an ACK and wait for another block
    SEND_ACK_FINISH,            //    "   "  "   "  then finish the transfer
                                // Transmitter states ...
    WAIT_NAK_START,             //  wait fo a NAK and then start sending data
    WAIT_ACK_NAK,               //  waiting for ACK/NAK
    SEND_BLOCK,                 //  send either SOH or EOT
    SEND_BLKNO_1,               //  send first byte of block number
    SEND_BLKNO_2,               //    "  second  "  "    "     "
    SEND_DATA,                  //  send 128 byte data block
    SEND_CKSUM,                 //  send checksum
    WAIT_ACK_FINISH,            //  wait for ACK and finish the transfer
  };
  typedef enum _XSTATES XSTATE;

    enum SEVERITY {
      //   These are the severity level of all log messages.  Needless to say,
      // the order of these is important since the test for logging is whether
      // the message's level is .GE. the current logging level.
      //
      //   Note that CMDOUT and CMDERR are special cases - these are used
      // exclusively for output and errors generated in response to something
      // the operator just typed.  This text is handled specially.
      CMDOUT   = -2,        // output generated by operator commands
      CMDERR   = -1,        // errors generated by operator commands
      TRACE    = 0,         // I/O trace messages
      DEBUGLVL = TRACE+1,   // debugging messages
      WARNING  = DEBUG+1,   // warning messages
      ERROR    = WARNING+1, // severe error messages
      ABORT    = ERROR+1,   // severe error that aborts this program
      NOLOG    = 99999,     // imaginary level that disables all logging
    };

public:
    Tu58();
    ~Tu58() {};

    void configureTu58(Tu58FileConfiguration tu58FileConfiguration[2], Tu58Configuration tu58Configuration);
    void initTu58();
    void SendSerialBreak (bool fBreak);

    // Other public CTU58 methods ...
  public:
    // Return the number of units supported ...
    uint8_t GetUnits() const {return m_nUnits;}
    // Attach the emulated drive to an image file ...
    bool Attach (uint8_t nUnit, const string &sFileName, bool fReadOnly=false, uint32_t lCapacity=0);
    void Detach (uint8_t nUnit);
    void DetachAll();
    // Return TRUE if the drive is attached (online) ...
    bool IsAttached (uint8_t nUnit) const
      {assert(nUnit < m_nUnits);  return m_Images[nUnit]->IsOpen();}
    // Return TRUE if the drive/file is read only (not supported) ...
    bool IsReadOnly (uint8_t nUnit) const
      {assert(nUnit < m_nUnits);  return m_Images[nUnit]->IsReadOnly();}
    // Return the external file that we're attached to ...
    string GetFileName (uint8_t nUnit) const
      {assert(nUnit < m_nUnits);  return m_Images[nUnit]->GetFileName();}
    // Return the capacity (in records!) of the specified drive ...
    uint32_t GetCapacity (uint8_t nUnit) const
      {assert(nUnit < m_nUnits);  return m_Images[nUnit]->GetCapacity();}
    wxString StateToString (RSP_STATE nState);
    wxString LevelToString (SEVERITY nLevel);
    void LOGF(SEVERITY level, wxString text, int data, wxString extraString);
    void LOGF(SEVERITY level, wxString text, int data1=-1, int data2=-1, int data3=-1, int data4=-1);

protected:
    // Calculate a TU58 style checksum ...
    static uint16_t ComputeChecksum (const RSP_DATA *pPacket);
    // Receiver state machine ...
    void RxFromHost (uint8_t bData, int uartNumber);
    void RxPacketStart (uint8_t bFlag);
    int32_t RxPacketData (uint8_t bData);
    // Transmitter state machine ...
    bool TxToHost(uint8_t &bData, int uartNumber);
    void TxPacketStart (uint8_t bFlag, uint8_t bCount);
    bool TxPacketData (uint8_t &bData);
    void TxEndPacket (uint8_t bSuccess, uint16_t wCount=0);
    // Command and read/write packet processing ...
    bool CheckUnit (bool fWrite=false);
    void DoCommand();
    void WriteData();
    bool ReadData();
    
private:
    Tu58FileConfiguration tu58FileConfiguration_[4];
    Tu58Configuration tu58Configuration_;
    
    uint8_t       m_nUnits;         // number of units on this drive
    RSP_STATE     m_nState;         // current state of the RSP protocol
    RSP_DATA      m_RSPbuffer;      // packet being sent or received
    RSP_COMMAND   m_RSPcommand;     // last command packet received
    uint8_t       m_cbRSPpacket;    // bytes sent or received in this packet
    uint8_t       m_bChecksumH;     // packet checksum, high order byte
    uint8_t       m_bChecksumL;     //   "      "   "   low    "    "
    uint16_t      m_cbTransfer;     // total bytes in this read or write
    uint16_t      m_wCurrentBlock;  // current TU58 logical block number
    uint16_t      m_cbSector;       // bytes used in current sector
    uint8_t m_abSector[RSP_BLOCKSIZE];    // image file sector buffer
    vector<CDiskImageFile *> m_Images;    // CDiskImageFiles one per unit
    
    int m_uartNumber;
};

#endif  // TU58_H
