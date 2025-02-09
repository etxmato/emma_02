#ifndef TU58_H
#define TU58_H

#include "computerconfig.h"

// RSP magic numbers, bits and structures ...

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

class Tu58
{
public:
    Tu58();
    ~Tu58() {};

    void configureTu58(Tu58FileConfiguration tu58FileConfiguration, Tu58Configuration tu58Configuration);
//    void initializeTu58(wxString fileName);
    void initTu58();
    void SendSerialBreak (bool fBreak);
    void RxPacketStart (uint8_t bFlag);
    int32_t RxPacketData (uint8_t bData);
    void TxPacketStart (uint8_t bFlag, uint8_t bCount);
    bool TxPacketData (uint8_t &bData);
    bool TxToHost(uint8_t &bData, int uartNumber);
    void RxFromHost (uint8_t bData, int uartNumber);
    void DoCommand();
    void WriteData();
    bool ReadData();
    
private:
    Tu58FileConfiguration tu58FileConfiguration_;
    Tu58Configuration tu58Configuration_;
    
    bool driveCreated_;
    Byte *recordBuffer_;

    Byte m_bdata;
    RSP_STATE     m_nState;         // current state of the RSP protocol
    RSP_DATA      m_RSPbuffer;      // packet being sent or received
    RSP_COMMAND   m_RSPcommand;     // last command packet received

    uint8_t m_abSector[RSP_BLOCKSIZE];    // image file sector buffer
};

#endif  // TU58_H
