#ifndef LED_H
#define LED_H

class Led  
{
public:
    Led(wxDC& dc, int x, int y, int computerType, bool reversePol=false);
    ~Led();

    void onPaint(wxDC& dc);
    void setStatus(wxDC& dc, int status);

private:
    wxBitmap *ledOnBitmapPointer;
    wxBitmap *ledOffBitmapPointer;
    wxBitmap *ledOnGreenBitmapPointer;
    wxBitmap *ledOnOrangeBitmapPointer;
    wxMask *maskOn;
    wxMask *maskOff;

    int ledType_;
    int    status_;
    int    x_;
    int    y_;
    bool reversePol_;
};

#endif  // LED_H
