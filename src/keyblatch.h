#ifndef KEYBLATCH_H
#define KEYBLATCH_H

class KeybLatch
{
public:
    KeybLatch();
    ~KeybLatch() {};

    void configure(IoConfiguration portConf, wxString type, wxString saveCommand, int pad);
    void keyDown(int keycode, wxKeyEvent& event);
    void keyDown(int keycode);
    void keyUp(int keycode, wxKeyEvent& event);
    void keyUp(int keycode);

    Byte ef();
    void out(Byte value);

    void resetKeybLatch();
    void cycleKeybLatch();
    int getCtrlvChar();
    void startKeyFile();
    void closeKeyFile();
    bool keyDown();
    void startLatchRun(bool load);
    void checkCaps();
    void switchCaps();

private:
    IoConfiguration ioConfiguration_;

    Byte keyState_[255];
    int keyLatch_;
    
    bool ctrlAltLeft_;
    bool shiftPressed_;
    bool altPressed_;
    bool ctrlPressed_;
    bool capsPressed_;

    wxFile keyFile_;
    bool keyFileOpened_;
    int cycleValue_;
    int keyboardCode_;
    
    bool load_;
    size_t runCommand_;
    size_t ctrlvText_;
    wxString commandText_;

    wxString saveCommand_;
    int pad_;
};

#endif  // KEYBLATCH_H
