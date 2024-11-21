#ifndef TIL313F_H
#define TIL313F_H

class Tilfull
{
public:
    Tilfull(int type);
    ~Tilfull();

    void init(wxDC& dc, int x, int p);
    void onPaint(wxDC& dc);
    void update(wxDC& dc, Byte NewNumber, int segNumber);

private:
    Byte displayedNumber_;
    int    x_;
    int    y_;
    int    type_;

    wxBitmap *tilBitmapPointer;
    wxBitmap *tilCopyBitmapPointer;
    wxMemoryDC dcMemory;
    wxMemoryDC dcCopy;
};

#endif    // TIL313F_H
