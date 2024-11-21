#ifndef TMS9918_H
#define TMS9918_H

#include "video.h"
#include "wx/overlay.h"

class TileList
{
public:
    wxCoord x;
    wxCoord y;
    int size;
    TileList *nextTile;
};

class Tms9918 : public Video
{
public:

    Tms9918(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double clock, TmsConfiguration tmsConfiguration);
    ~Tms9918();

    void configure();
    Byte readEf();
    Byte readDataPort();
    Byte readVRAM();
    void modeHighOut(Byte value);
    void writeVRAM(Byte value);
    void cycleTms();
    void writeRegister(Byte reg, Byte value);

    void setClock(double clock);
    void setCycle();
    void copyScreen();
    
    void drawSprites();
    void drawSprite(Byte namePointer, Word spritePatternTableAddress, wxCoord x, wxCoord y, int numberOfLines, bool earlyClock);
    void drawSpriteMagnify(Byte namePointer, Word spritePatternTableAddress, wxCoord x, wxCoord y, int numberOfLines, bool earlyClock);

    void setColourMutexMainPlane(int clr);
    void drawPointMainPlane(wxCoord x, wxCoord y);
    void drawPointSpritePlane(wxCoord x, wxCoord y);
    void drawRectangleMainPlane(wxCoord x, wxCoord y, wxCoord width, wxCoord height);

    void drawTile(Word tile);
    void drawTileMultiColor(Word tile);
    void drawTilePatternUpdate(Word tile, Word address);
    void drawScreen();
    void drawScreenMultiColor();

    Byte getTmsMemory(int address) {return tmsMemory_[address];}
    void setTmsMemory(int address, Byte value) {tmsMemory_[address] = value;}

    void setFullScreen(bool fullScreenSet);
    void onF3();
    void reBlit(wxDC &dc);

private:
    TileList *tileListPointer;
    TmsConfiguration tmsConfiguration_;
    
    wxOverlay spriteOverlay;
    
    Byte tmsMemory_[16384];
    Byte numberOfSpritesOnline_[192];
    bitset<256> scanLineMap_[192];
    wxCoord lastSpriteX_[32];
    wxCoord lastSpriteY_[32];
    Byte multiColour_[512][384];
    
    Byte statusRegister_;
    Byte registers_[8];
    Byte mode_;
    Word nameAddress_;
    Word colorAddress_;
    Word patternAddress_;
    Word currentWriteAddress_;
    Word currentReadAddress_;
    Word spriteAttributeTableAddress_;
    Word spritePatternTableAddress_;
    
    Byte textColor_;
    Byte backgroundColor_;
    
    bool reDrawSprites_;
    bool spriteSize16_;
    bool spriteMagnify_;
    int spriteMagnifyFactor_;
    int spriteSelect_;
    int spriteSize_;
    bool enableInterrupt_;
    bool disableScreen_;

    bool toggle_;
    Byte value_;

    wxCoord backGroundX_;
    wxCoord backGroundY_;
    int updateTile_;


    int cycleValue_;
    int cycleSize_;
};

#endif  // TMS9918_H
