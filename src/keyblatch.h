#ifndef KEYBLATCH_H
#define KEYBLATCH_H

class KeybLatch
{
public:
    KeybLatch();
    ~KeybLatch() {};

    void configure(KeyLatchConfiguration keyLatchConfiguration, AddressLocationConfiguration addressLocationConfiguration, wxString type, wxString saveCommand);
    void reDefineHexKeys(int hexKeyDef1[], int hexKeyDef2[], bool simDef2);
    void keyDown(int keycode, wxKeyEvent& event);
    void keyDown(int keycode);
    void keyDownNoShift(int keycode);
    void keyUp(int keycode, wxKeyEvent& event);
    void keyUp(int keycode);

    Byte ef();
    void out(Byte value);

    void resetKeybLatch();
    void cycleKeybLatch();
    int translateKey(int key);
    int getCtrlvChar();
    void startKeyFile();
    void closeKeyFile();
    bool keyDown();
    void startLatchRun(bool load, wxString command);
    void startCtrlV(wxString command);
    void checkCaps();
    void switchCaps();

private:
    KeyLatchConfiguration keyLatchConfiguration_;
    AddressLocationConfiguration addressLocations_;

    Byte keyState_[255];
    int keyLatch_;
    
    int keyDef1_[16];
    int keyDef2_[16];
    bool simDef2_;

    bool ctrlAltLeft_;
    bool shiftPressed_;
    bool altPressed_;
    bool ctrlPressed_;
    bool capsPressed_;
    
    bool forceShiftActive_;
    bool forceNoShiftActive_;

    int shiftKey_;

    wxFile keyFile_;
    bool keyFileOpened_;
    int cycleValue_;
    int keyboardCode_;
    
    bool load_;
    bool fileToBeLoaded_;
    size_t ctrlvText_;
    bool ctrlvFound_;
    wxString commandText_;

    wxString saveCommand_;
};

#endif  // KEYBLATCH_H
