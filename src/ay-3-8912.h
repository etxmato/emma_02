#ifndef AY_3_8912_H
#define AY_3_8912_H

#define MAX_AY 2
#define TONE_MAX 6

class AY_3_8912Instance
{
public:
    AY_3_8912Instance();
    ~AY_3_8912Instance() {};
  
    void configureAY_3_8912(AY_3_8912Configuration ay_3_8912Configuration);
    bool ioGroupAY_3_8912(int ioGroup);

    void writeRegister(int ayNumber, Byte value);
    void writeData(Byte value);
    Byte readData();

private:
    AY_3_8912Configuration ay_3_8912Configuration_;
    
    Byte selectedRegister_;
    int ayNumber_;
    
    bool portTypeInputA_[MAX_AY];
    Byte portValueA_[MAX_AY];

    bool portTypeInputB_[MAX_AY];
    Byte portValueB_[MAX_AY];
    
    int periodTone_[TONE_MAX];
    int periodEnvelope_;
    bool toneOn_[TONE_MAX];
    bool noiseOn_[TONE_MAX];
    
    bool envelopeAttack_;
};


#endif  // AY_3_8912_H
