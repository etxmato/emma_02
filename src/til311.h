#ifndef TIL_H
#define TIL_H

class Til311  
{
public:
    Til311();
    ~Til311();

    void init(wxDC& dc, int x, int p);
    void onPaint(wxDC& dc);
    void update(wxDC& dc, int NewNumber); 

private:
    int    displayedNumber_;
    int    x_;
    int    y_;
 
    wxBitmap *til311BitmapPointer;
    wxMemoryDC dcMemory;
};

#endif    // TIL_H
