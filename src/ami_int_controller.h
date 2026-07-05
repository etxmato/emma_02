#ifndef AMI_INT_CONTROLLER_H
#define AMI_INT_CONTROLLER_H

/*
 *******************************************************************
 *** AMI 9217MLB Interrupt Controller Emulation                  ***
 ***                                                             ***
 *** Models the interrupt cause/acknowledge mechanism of the     ***
 *** AMI 9217MLB custom gate array as used in the PTC-701.       ***
 ***                                                             ***
 *** The AMI chip has multiple interrupt sources. When an        ***
 *** interrupt fires, the firmware reads a "cause" byte from     ***
 *** a memory-mapped register. After handling, the firmware      ***
 *** writes an ACK byte back to the same register.              ***
 ***                                                             ***
 *** Interrupt sources are prioritized (0 = lowest).             ***
 *** Cause value = source_number * causeMultiplier               ***
 *** ACK value = 1 << source_number                             ***
 ***                                                             ***
 *** This software is copyright 2026 by Marcel van Tongeren     ***
 *******************************************************************
*/

#include "computerconfig.h"

#define AMI_INT_MAX_SOURCES 8

class AmiIntController
{
public:
    AmiIntController();
    ~AmiIntController() {};
    
    void configure(AmiIntControllerConfiguration config);
    void reset();
    
    // Called when an interrupt source fires (e.g., timer tick, key press)
    void requestInterrupt(int sourceNumber);
    
    // Called when source condition clears (e.g., key released, timer ack'd)
    void clearInterrupt(int sourceNumber);
    
    // Memory-mapped register access (called from readMemDebug/writeMemDebug)
    Byte readCauseRegister();           // Returns cause byte for highest-priority pending source
    void writeAckRegister(Byte value);  // Acknowledges interrupt (clears source by ACK bit)
    
    // Called each CPU cycle to check if interrupt line should be asserted
    bool isInterruptPending();
    
    // Enable/disable individual sources (mirrors AMI control register bits)
    void enableSource(int sourceNumber, bool enable);
    
    // Check if address matches our cause/ack register
    bool matchesAddress(Word address);

private:
    AmiIntControllerConfiguration config_;
    
    bool pending_[AMI_INT_MAX_SOURCES];     // Source has fired, not yet acknowledged
    bool enabled_[AMI_INT_MAX_SOURCES];     // Source is enabled (not masked)
    
    int findHighestPriority();              // Returns highest-priority pending & enabled source, or -1
};

#endif  // AMI_INT_CONTROLLER_H
