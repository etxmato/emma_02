#ifndef KEYBOARD_H
#define KEYBOARD_H

class KeyDef
{
public:
    bool defined;
    bool shift;
    int player;
    int key;
};

class Keyboard
{
public:
    Keyboard ();
    ~Keyboard () {};

    void configureKeyboard(AsciiKeyboardConfiguration asciiKeyboardConfiguration, AddressLocationConfiguration addressLocationConfiguration, wxString saveCommand);
    void charEventKeyboard(int keycode);
    void keyboardUp();

    Byte efKeyboard();
    Byte inKeyboard();
    void cycleKeyboard();

    void startElfKeyFile(wxString elfTypeStr);
    void closeElfKeyFile();
    void setForceUpperCaseKeyboard(bool status);
    void startElfRun(bool load, wxString command);
    void startCtrlV(wxString command);

private:
    AsciiKeyboardConfiguration asciiKeyboardConfiguration_;
    
    char keyboardValue_;
    Byte keyboardEf_;
    int keyCycles_;
    Byte lastKeyCode_;
    int rawKeyCode_;

    wxFile elfKeyFile_;
    bool elfKeyFileOpen_;
    bool forceUpperCase_;

    bool load_;
    bool fileToBeLoaded_;
    size_t elfRunCommand_;
    wxString commandText_; 

    AddressLocationConfiguration addressLocations_;
    wxString saveCommand_;

};

#endif  // KEYBOARD_H
