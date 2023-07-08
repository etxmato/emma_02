#ifndef TIL_H
#define TIL_H

class Til
{
public:
    Til();
    Til(bool upsideDown);
    ~Til();

    virtual void init(wxDC& dc, int x, int p);
    virtual void onPaint(wxDC& dc);
    virtual void update(wxDC& dc, int NewNumber);
    virtual void dp(wxDC& dc, bool status);
    virtual void turnOff(wxDC& dc, bool status);

protected:
    int    displayedNumber_;
    int    x_;
    int    y_;
 
    wxBitmap *tilBitmapPointer;
    wxMemoryDC dcMemory;
};

class Til311 : public Til
{
public:
    Til311();

    void init(wxDC& dc, int x, int p);
    void onPaint(wxDC& dc);
    void update(wxDC& dc, int NewNumber); 
};

class Til313 : public Til
{
public:
    Til313();

    void init(wxDC& dc, int x, int p);
    void onPaint(wxDC& dc);
    void update(wxDC& dc, int NewNumber);
};

class Til313Italic : public Til
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
    wxBitmap *til313BitmapPointer_led_off;
    wxBitmap *til313BitmapPointer_led_on;
    wxBitmap *til313BitmapPointer_off;
    wxMemoryDC dcMemory;
};

#endif    // TIL_H
