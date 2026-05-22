#ifndef GUIDEBUGGER_H
#define GUIDEBUGGER_H

#include "guicomx.h"

class BreakPointAndTrap
{
public:
    Byte breakPointAndTrapType;
    Byte type;

    Word addressStart;
    Word addressEnd;
    int value;

    Byte count;
    Byte b1;
    Byte b2;
    Byte b3;
    Byte b4;
    Byte b5;
    Byte b6;
    Byte b7;
    Byte typeOpcode;
    
    bool selected;
};

class GuiDebugger: public GuiComx
{
public:
    GuiDebugger(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~GuiDebugger();

    void readGuiDebuggerConfig();
    void cycleDebugger();
    void checkRegisterTrap(Byte registerType, Word execAddress, Word value, bool noTrace);
    void checkMemoryTrap(Word execAddress, Word address, Byte value, int type, bool dmaReadWrite = false);
    void switchBreakPointAndTrap(int item);

protected:
    int numberOfBreakPointsAndTraps_;
    wxListCtrl *breakPointAndTrapWindowPointer;

    bool breakTrap_;
    bool performStep_;
    bool trace_;
    bool traceDma_;
    bool traceDmaMt_;
    bool showInstructionTrap_;
    Word showInstructionTrapAddress_;
    long steps_;

private:
    void deleteBreakPointAndTrap(wxListEvent&event);
    void editBreakPointAndTrap(wxListEvent&event);
    void selectBreakPointAndTrap(wxListEvent&event);
    void deselectBreakPointAndTrap(wxListEvent&event);
    void keyBreakPointAndTrap(wxListEvent&event);

    void onBreakPointSet(wxCommandEvent&event);
    void addBreakPoint();
    bool editBreakPoint(long selectedItem, wxString fullStringValue);

    void onRegisterTrapSet(wxCommandEvent&event);
    void addRegisterTrap();
    bool editRegisterTrap(long selectedItem, wxString fullStringValue);
    int getRegister(wxString buffer);

    void onInstructionTrapSet(wxCommandEvent&event);
    void onInstructionTrapCommand(wxCommandEvent&event);
    void addInstructionTrap();
    bool editInstructionTrap(long selectedItem, wxString fullStringValue);

    void onMemoryTrapSet(wxCommandEvent&event);
    void addMemoryTrap();
    bool editMemoryTrap(long selectedItem, wxString fullStringValue);

    long get16BitFromHexString(wxString strValue, wxString type);
    long get8BitFromHexString(wxString strValue, wxString type);

    vector <BreakPointAndTrap> breakPointAndTraps;
    int selectedBreakPointAndTrap_;

    wxBitmap uncheckBitmap_;
    wxBitmap checkedBitmap_;
    int uncheckButton_;
    int checkedButton_;
    wxImageList *imageList_;

    DECLARE_EVENT_TABLE()
};

#endif // GUIDEBUGGER_H
