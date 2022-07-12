#ifndef TIL313_H
#define TIL313_H

class Til313  
{
public:
    Til313();
    ~Til313();

    void init(wxDC& dc, int x, int p);
    void onPaint(wxDC& dc);
    void update(wxDC& dc, int NewNumber);

private:
    int    displayedNumber_;
    int    x_;
    int    y_;
 
    wxBitmap *til313BitmapPointer;
    wxMemoryDC dcMemory;
};

class Til313Italic
{
public:
    Til313Italic(bool upsideDown);
    ~Til313Italic();
    
    void init(wxDC& dc, int x, int p);
    void onPaint(wxDC& dc);
    void update(wxDC& dc, int NewNumber);
    void dp(wxDC& dc, bool status);
    void turnOff(wxDC& dc, bool status);
    
private:
    int    displayedNumber_;
    int    x_;
    int    y_;
    
    wxBitmap *til313BitmapPointer_led_off;
    wxBitmap *til313BitmapPointer_led_on;
    wxBitmap *til313BitmapPointer_off;
    wxMemoryDC dcMemory;
};

#endif    // TIL313_H
