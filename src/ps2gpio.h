#ifndef PS2GPIO_H
#define PS2GPIO_H

class Ps2gpio 
{
public:
	Ps2gpio ();
	~Ps2gpio () {};

	void configurePs2gpioElf2K(int computerType);
	void charEventPs2gpio(int keycode);

	Byte efPs2gpio();
	Byte inPs2gpio();
	void cyclePs2gpio();

	void startPs2gpioKeyFile();
	void closePs2gpioKeyFile();
	void setForceUpperCasePs2gpio(bool status);
	void resetPs2gpio();

private:
	int keyboardValue_;
	Byte keyboardEf_;
	int keyCycles_;
	Byte lastKeyCode_;
	int rawKeyCode_;
	int escKey_;
	int escKey2_;

	wxFile elfKeyFile_;
	bool elfKeyFileOpen_;
	bool forceUpperCase_;
	int startUp_;
};

#endif  // PS2GPIO_H