#ifndef HD44780_H
#define HD44780_H

#include "video.h"

class HD44780 : public Video
{
public:
    HD44780(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double clock, Hd44780Configuration hd44780Configuration);
    ~HD44780();

    void configureHd44780();
    void initHd44780();
    bool ioGroup(int ioGroup);
    Byte efHd44780();
    void cycleHd44780();
    void blinkHd44780();

    void setClock(double clock);
    void setCycle();
    void writeCommand(Byte value, int showTrace);
    void writeData(Byte value, int showTrace);
    Byte readStatus();
    Byte readData();
    void drawScreen();
    void drawCharacter(int col, int row, Byte value);
    void drawCursor(int col, int row, bool status);
    void drawCursor(wxDC &dc, int col, int row, bool status);
    bool readCharRomFile(wxString romDir, wxString romFile);
    void setFullScreen(bool fullScreenSet);
    void onF3();
    void reBlit(wxDC &dc);

private:
    bool ddramAddressToPosition(Byte addr, int *col, int *row);
    void updateAddressCounter(int dir);

    Hd44780Configuration hd44780Configuration_;

    // HD44780 CGROM: 256 characters × 16 bytes/char = 4096 bytes
    Byte cgrom_[4096];
    // CGRAM: 8 user-defined characters × 8 bytes = 64 bytes  
    Byte cgram_[64];
    // DDRAM: 80 bytes (line 1: 0x00-0x27, line 2: 0x40-0x67)
    Byte ddram_[128];

    // HD44780 internal state
    Byte addressCounter_;       // Current DDRAM/CGRAM address
    bool activeRamIsDdram_;     // true = DDRAM, false = CGRAM
    bool displayOn_;
    bool cursorOn_;
    bool blinkOn_;
    bool shiftOn_;              // Entry mode: display shift on write
    int direction_;             // Entry mode: +1 or -1
    int dataLen_;               // Interface data length: 4 or 8
    int numLines_;              // 1 or 2 lines
    int charSize_;              // 8 or 10 (character height in rows)
    int displayShift_;          // Display shift offset

    bool busyFlag_;
    int busyCycles_;            // Countdown for busy flag

    bool cursorBlinkState_;     // Current blink phase (on/off)

    // Timing
    int cycleValue_;
    int cycleSize_;
    int blinkValue_;
    int blinkSize_;

    // Display geometry
    int cols_;                  // Number of visible columns (16)
    int rows_;                  // Number of visible rows (2)
    int charW_;                 // Character width in pixels (5)
    int charH_;                 // Character height in pixels (8)
};

#endif  // HD44780_H
