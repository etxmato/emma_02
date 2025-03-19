#ifndef MM57109_H
#define MM57109_H

struct NcuRegister
{
    unsigned int mantissa;
    double decimal;
    int exponent;
    unsigned int decimalPoint;
    int mantissaSign;
    int exponentSign;
};

class Mm57109Instance
{
private:
    enum _OpCodes
    {
        OP_CODE_DIGIT_0,    // 0
        OP_CODE_DIGIT_1,    // 1
        OP_CODE_DIGIT_2,    // 2
        OP_CODE_DIGIT_3,    // 3
        OP_CODE_DIGIT_4,    // 4
        OP_CODE_DIGIT_5,    // 5
        OP_CODE_DIGIT_6,    // 6
        OP_CODE_DIGIT_7,    // 7
        OP_CODE_DIGIT_8,    // 10
        OP_CODE_DIGIT_9,    // 11
        OP_CODE_DP,         // 12
        OP_CODE_EE,         // 13
        OP_CODE_CS,         // 14
        OP_CODE_PI,         // 15
        OP_CODE_AIN,        // 16
        OP_CODE_HALT,       // 17
        OP_CODE_TJC,        // 20
        OP_CODE_TX0,        // 21
        OP_CODE_TXLT0,      // 22
        OP_CODE_TXF,        // 23
        OP_CODE_TERR,       // 24
        OP_CODE_JMP,        // 25
        OP_CODE_OUT,        // 26
        OP_CODE_IN,         // 27
        OP_CODE_SMDC,       // 30
        OP_CODE_IBNZ,       // 31
        OP_CODE_DBNZ,       // 32
        OP_CODE_XEM,        // 33
        OP_CODE_MS,         // 34
        OP_CODE_MR,         // 35
        OP_CODE_LSH,        // 36
        OP_CODE_RSH,        // 37
        OP_CODE_INV,        // 40
        OP_CODE_ENTER,      // 41
        OP_CODE_TOGM,       // 42
        OP_CODE_ROLL,       // 43
        OP_CODE_SIN,        // 44
        OP_CODE_COS,        // 45
        OP_CODE_TAN,        // 46
        OP_CODE_SF1,        // 47
        OP_CODE_PF1,        // 50
        OP_CODE_SF2,        // 51
        OP_CODE_PF2,        // 52
        OP_CODE_ECLR,       // 53
        OP_CODE_RTD,        // 54
        OP_CODE_DTR,        // 55
        OP_CODE_POP,        // 56
        OP_CODE_MCLR,       // 57
        OP_CODE_XEY,        // 60
        OP_CODE_EX,         // 61
        OP_CODE_10X,        // 62
        OP_CODE_SQ,         // 63
        OP_CODE_SQRT,       // 64
        OP_CODE_LN,         // 65
        OP_CODE_LOG,        // 66
        OP_CODE_1X,         // 67
        OP_CODE_YX,         // 70
        OP_CODE_PLUS,       // 71
        OP_CODE_MINUS,      // 72
        OP_CODE_TIMES,      // 73
        OP_CODE_DIVIDE,     // 74
        OP_CODE_PRW1,       // 75
        OP_CODE_PRW2,       // 76
        OP_CODE_NOP,        // 77
        OP_CODE_SIN_INV,
        OP_CODE_COS_INV,
        OP_CODE_TAN_INV,
        OP_CODE_PLUS_INV,
        OP_CODE_MINUS_INV,
        OP_CODE_TIMES_INV,
        OP_CODE_DIVIDE_INV
};
typedef enum _OpCodes OpCodes;

public:
    Mm57109Instance();
    ~Mm57109Instance() {};
    
    void configureMm57109(Mm57109Configuration mm57109Configuration);
    void setSpeedFactor(double cpuClock, double ncuClock);
    bool ioGroupMm57109(int ioGroup);
    
    void output(Byte value);
    Byte input();
    void cycle();
    Byte ef();
    void hold(Byte value);
    
private:
    void firsInstructionWord(OpCodes opCode);
    void secondInstrucionWord(OpCodes opCode);
    void invCommand(OpCodes opCode);

    void readyPulse();
    void masterClear();
    void pushStack();
    void popStack();
    void rollStack();
    void exchangeXY();
    void exchangeXM();
    void shiftLeft();
    void shiftRight();
    void clearX();
    void convert(double reg);
    void mantissaConvert(double reg);
    void exponentConvert(double reg);
    int count_digit(int number);
    void digitEntry(int number);
    void calculateDigitEntry();
    void mantissaEntry(int number);
    void exponentEntry(int number);
    void exponentConvertAboveOne(double reg, int numberOfDigits);
    void exponentConvertBelowOne(double reg);
    void stopDigitEntry();
    Mm57109Configuration mm57109Configuration_;
    
    int instructionCycleCounter_;
    int rdyCycleCounter_;

    NcuRegister inputRegisterX;
    Byte outputRegister[12];
    double registerForOutput;
    double registerX;
    double registerY;
    double registerZ;
    double registerT;
    double registerM;

    OpCodes lastOpCode_;
    OpCodes secondOpCode_;
    OpCodes mathOpCode_;
    bool firstInstructionWord_;

    Byte dataReady_;
    Byte rdy_;
    Byte error_;
    Byte hold_;
    Byte mdc_;
    Byte newMdc_;
    int inputDigitNumber_;
    int outputDigitNumber_;
    int dpNumber_;
    int eeNumber_;
    Byte linkDigit_;

    bool floatingPointMode_;
    bool returnDigits_;
    double speedFactor_;
};


#endif  // MM57109_H
