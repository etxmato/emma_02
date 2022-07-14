#ifndef EXPBOX_H
#define EXPBOX_H

#include "fdc.h"
#include "network.h"
#include "joycard.h"
#include "usb.h"
#include "v1870.h"

class Expansion: public V1870, public Fdc, public Joycard, public Usbcard, public Network
{
public:
    Expansion (const wxString& title, const wxPoint& pos, const wxSize& size, double zoomLevel, int computerType, double clock);
    ~Expansion () {};

    void configureExpansion();
    void configureCard(int slot);
    Byte expansionIn2();
    Byte expansionIn4();
    void bankOut(Byte value);
    void expansionOut(Byte value);
    void onComxF4();
    int getComxExpansionType(int card);
    bool isPrintRomLoaded(int type);
    bool isRamCardActive();

    int getComxExpansionSlot() {return expansionSlot_;};
    int getComxExpansionRamBank() {return ramBank_;};
    int getComxExpansionEpromBank() {return epromBank_ | (ramSwitched_ << 3);};
    bool checkExpansionRomLoaded() {return expansionRomLoaded_;};
    bool isColumnRomLoaded()  {return (columnSlot_ != 0xff);};
    bool isEpromBoardLoaded() { return (epromSlot_ != 0xff); };
    bool isSuperBoardLoaded() { return (superSlot_ != 0xff); };
    bool isDosCardSelected();
    void readEpromFile(wxString dir, wxString fileName, int start);
    void readSuperFile(wxString dir, wxString fileName);

protected:
    bool thermalPrinting_;
    Byte out1value_;
    int thermalSlot_;
    int serialSlot_;
    int printerSlot_;
    int columnSlot_;
    int diagSlot_;
    bool useExpansionRam_;
    int expansionRamSlot_;
    int comxExpansionType_[4];
    bool expansionRomLoaded_;
    bool diskRomLoaded_;

private:
    bool printRomLoaded_[3];
};

#endif  // EXPBOX_H
