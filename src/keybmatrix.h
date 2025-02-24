#ifndef KEYBMATRIX_H
#define KEYBMATRIX_H

class KeybMatrix
{
public:
    KeybMatrix();
    ~KeybMatrix() {};

    void configure(MatrixKeyboardConfiguration matrixKeyboardConfiguration, wxString saveCommand);
    void charEvent(int keycode);
    bool keyDownExtended(int keycode, wxKeyEvent& event);
    void keyUpExtended(int keycode);
    void keyDownFile();
    void keyUpFile();
    void clearReturn();

    int efKey(Byte efNumber);
    Byte in(Word address);
    Byte in();

    void setRow(Byte row);
    void resetKeyboard();
    int getCtrlvChar();
    void startKeyFile();
    void closeKeyFile();
    bool keyDown();
    void startRun(bool load, wxString command);
    void startCtrlV(wxString command);
    void checkCaps();

private:
    MatrixKeyboardConfiguration matrixKeyboardConfiguration_;
    
    Byte keyValue_[255];
    Byte efKeyValue_[5];

    bool capsPressed_;

    wxString saveCommand_;
    int cycleValue_;

    int keyboardCode_;

    bool keyFileOpened_;
    wxFile keyFile_;
    bool keyDown_;
    int secondKeyboardCodes[5];

    bool load_;
    bool fileToBeLoaded_;
    size_t ctrlvText_;
    wxString commandText_;
    Byte row_;
};

#endif  // KEYBMATRIX_H
