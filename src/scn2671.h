#ifndef SCN2671_H
#define SCN2671_H

class Scn2671Instance
{
public:
    Scn2671Instance(int scn2671Number);
    ~Scn2671Instance() {};
    
    void configureScn2671(Scn2671Configuration scn2671Configuration, double clock, long int numberOfScn2671s);
    bool ioGroupScn2671(int ioGroup);
    
    void resetCommand(Byte value);
    void commandMisc(Byte value);

    void uartCts(Byte value);
    void writeCommunicationModeRegister(Byte value);
    Byte readCommunicationModeRegister();
    void writeBaudRateControlRegister(Byte value, double clock);
    Byte readCommunicationsStatusRegister();
    void writeInterruptMaskRegister(Byte value);
    Byte readInterruptMaskRegister();
    Byte readInterruptStatusRegister();
    void writeKeyboardModeRegister(Byte value);
    Byte readKeyboardModeRegister();
    Byte readKeyboardStatusRegister();
    void writeTransmitterHoldingRegister(Byte value);
    Byte readReceiverHoldingRegister();
    void charEventKeyboardScn2671(wxKeyEvent& event, int keycode);
    Byte readKeyboardHoldingRegister();
    void cycle();
    
    Byte threStatus();
    void interrupt(int type);
    void clearInterrupt(int type);
    
    void serialDataInput();
    void writeTransmitterShiftRegister_();
    Byte uartThreStatus();
    void framingError(bool data);
    void dataAvailable();
    void dataAvailable(Byte value);
    void dataAvailableUart(bool data);
    void setSendPacket(bool status);
    void setTerminalLoad(bool status);
    void setTerminalSave(bool status);

private:
    int scn2671Number_;
    
    Scn2671Configuration scn2671Configuration_;
    
    bitset<8> communicationModeRegister_;
    bitset<8> communicationsStatusRegister_;
    bitset<8> miscCommandRegister_;
    bitset<8> interruptMaskRegister_;
    bitset<8> interruptStatusRegister_;
    bitset<8> keyboardModeRegister_;
    bitset<8> keyboardStatusRegister_;
    Byte KeyboardHoldingRegister_;
    
    Byte serialDataInputEf_;
    Byte interruptEf_;

    Byte transmitterHoldingRegister_;
    bool clearToSend_;
    
    int baudRateT_;
    int baudRateR_;

    long serialDataOutputCount_;
    int vtOutBits_;
    long serialDataInputCount_;
    Byte receiverHoldingRegister_;

    bool sendPacket_;
    bool terminalLoad_;
    bool terminalSave_;
    int toneFrequency_;
};

#endif  // SCN2671_H
