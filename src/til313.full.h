#ifndef TIL313F_H
#define TIL313F_H

class Til313full  
{
public:
	Til313full();
	~Til313full();

	void init(wxDC& dc, int x, int p);
	void onPaint(wxDC& dc);
	void update(wxDC& dc, Byte NewNumber); 

private:
	Byte displayedNumber_;
	int	x_;
	int	y_;
 
	wxBitmap *tilBitmapPointer;
	wxBitmap *tilCopyBitmapPointer;
	wxMemoryDC dcMemory;
	wxMemoryDC dcCopy;
};

#endif	// TIL313F_H