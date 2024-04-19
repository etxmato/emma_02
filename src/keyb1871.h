#ifndef KEYB1871_H
#define KEYB1871_H

class Keyb1871
{
public:
    Keyb1871 ();
    ~Keyb1871 () {};

    void configureKeyb1871(int computerType, IoConfiguration portConf, Locations addressLocations, wxString saveCommand);
    void charEventKeyb1871(int keycode);
    bool keyDownExtended1871(int keycode, wxKeyEvent& event);
    bool keyCheck(int keycode, int modifiers);
    void keyUp1871(int keycode);

    Byte efKeyb1871();
    Byte efKeybRepeat1871();
    void keyClear1871();
    Byte inKeyb1871();
    void cycleKeyb1871();

    void start1871KeyFile();
    void close1871KeyFile();
    void start1871Run(bool load, wxString command);
    void start1871CtrlV(wxString command);
    Byte getDiagInput();
    void diagOut(Byte value);

private:
    Byte lastKeyCode_;
    int keyboardCode_;
    int secondKeyboardCodes[5];
    wxKeyCode previousKeyCode_;

    int keyCycles_;
    int rawKeyCode_;
    int debounceCounter_;

    Byte keyboardEf_;
    Byte keyboardRepeatEf_;

    wxFile keyFile_;
    bool keyFileOpen_;

    bool load_;
    bool fileToBeLoaded_;
    size_t ctrlvText_;
    wxString commandText_;
    
    Locations addressLocations_;
    wxString saveCommand_;
};

#endif  // KEYB1871_H
