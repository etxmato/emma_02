#ifndef KEYBOARD_H
#define KEYBOARD_H

class Keyboard 
{
public:
	Keyboard ();
	~Keyboard () {};

	void configureKeyboard(int computerType, ElfPortConfiguration portConf);
	void charEventKeyboard(int keycode);
	void keyboardUp();

	Byte efKeyboard();
	Byte inKeyboard();
	void cycleKeyboard();

	void startElfKeyFile(wxString elfTypeStr);
	void closeElfKeyFile();
	void setForceUpperCaseKeyboard(bool status);
	void startElfRun(bool load);

private:
	char keyboardValue_;
	Byte keyboardEf_;
	int keyCycles_;
	Byte lastKeyCode_;
	int rawKeyCode_;

	wxFile elfKeyFile_;
	bool elfKeyFileOpen_;
	bool forceUpperCase_;

	bool load_;
	size_t elfRunCommand_;
	wxString commandText_; 

};

#endif  // KEYBOARD_H