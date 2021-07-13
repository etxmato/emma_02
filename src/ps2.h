#ifndef PS2_H
#define PS2_H

class Ps2
{
public:
	Ps2();
	~Ps2() {};

	bool keyDownPs2(int keycode);
	bool keyUpPs2(int keycode);

	void configurePs2(bool ps2Interrupt, ElfPortConfiguration portConf);
	Byte efPs2();
	Byte inPs2();
	void outPs2(Byte value); 
	void cyclePs2();

	Word getScanCode(Word value);
	void addToBuffer(Byte value);

private:
	char ps2KValue_;
	char ps2CValue_;
	Byte ps2Value_;
	Byte ps2Port_;
	int ps2KeyCycle_;
	Byte ps2Buffer_[256];
	int ps2KeyStart_;
	int ps2KeyEnd_;
	char ps2KeyboardMode_;
	uint64_t ps2Cycles_;
	Byte ps2Parity_;
	bool usePS2_;
	bool ps2Interrupt_;	
};

#endif  // PS2_H