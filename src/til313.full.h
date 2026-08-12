#ifndef TIL313F_H
#define TIL313F_H

class TilMultiDisplay
{
public:
    virtual ~TilMultiDisplay() {}
    virtual void init(wxDC& dc, int x, int y) = 0;
    virtual void onPaint(wxDC& dc) = 0;
    virtual void update(wxDC& dc, Word NewNumber, int segNumber) = 0;
};

class Tilfull : public TilMultiDisplay
{
public:
    Tilfull(int type);
    virtual ~Tilfull();

    void init(wxDC& dc, int x, int p);
    void onPaint(wxDC& dc);
    void update(wxDC& dc, Word NewNumber, int segNumber);

private:
    Word displayedNumber_;
    int    x_;
    int    y_;
    int    type_;

    wxBitmap *tilBitmapPointer;
    wxBitmap *tilCopyBitmapPointer;
    wxMemoryDC dcMemory;
    wxMemoryDC dcCopy;
};

class TilMan2815 : public TilMultiDisplay
{
public:
    TilMan2815();
    virtual ~TilMan2815();

    void init(wxDC& dc, int x, int p);
    void onPaint(wxDC& dc);
    void update(wxDC& dc, Word NewNumber, int segNumber);

private:
    void drawSegments(Word segMask);

    wxBrush getBrush(int bit);
    wxPen getPen(int bit);
    void drawRectSeg(int bit, int x, int y, int w, int h);
    void drawQuadSeg(int bit, wxPoint pts[4]);

    Word segMask_;
    wxColour onColor_;
    wxColour offColor_;

    Word displayedNumber_;
    int x_;
    int y_;

    wxBitmap *tilBitmapPointer;
    wxMemoryDC dcMemory;
};

#endif    // TIL313F_H
