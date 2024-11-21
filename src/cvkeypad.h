#ifndef CVKEYPAD_H
#define CVKEYPAD_H

class CvKeypad
{
public:
    CvKeypad ();
    ~CvKeypad () {};

    void configure(CvKeypadConfiguration cvKeypadConfiguration);
    
    void keyDown(int keycode, wxKeyEvent& event);
    void keyUp(int keycode, wxKeyEvent& event);

    Byte in();
    Byte ef();

private:
    CvKeypadConfiguration cvKeypadConfiguration_;

    Byte keyboardCode_;
    Byte keyPressed_;
    Byte unreadInput_;


    int secondKeyboardCodes[6];
};

#endif  // CVKEYPAD_H
