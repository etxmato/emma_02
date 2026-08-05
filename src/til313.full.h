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

    void init(wxDC& dc, int x, int p) override;
    void onPaint(wxDC& dc) override;
    void update(wxDC& dc, Word NewNumber, int segNumber) override;

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

    void init(wxDC& dc, int x, int p) override;
    void onPaint(wxDC& dc) override;
    void update(wxDC& dc, Word NewNumber, int segNumber) override;

private:
    void drawSegments(Word segMask);

    Word displayedNumber_;
    int x_;
    int y_;

    wxBitmap *tilBitmapPointer;
    wxMemoryDC dcMemory;
};

#endif    // TIL313F_H
