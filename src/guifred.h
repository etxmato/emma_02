#ifndef GUIFRED_H
#define GUIFRED_H

#include "guivip.h"

#define FRED1_CARDS 24

class GuiFred: public GuiVip
{
public:

    GuiFred(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~GuiFred() {};

    void readFred1Config();
    void writeFred1DirConfig();
    void writeFred1Config();
    void readFred1WindowConfig();
    void writeFred1WindowConfig();
    void onFred1ControlWindows(wxCommandEvent&event);

    void readFred2Config();
    void writeFred2DirConfig();
    void writeFred2Config();
    void readFred2WindowConfig();
    void writeFred2WindowConfig();
    void onFred2ControlWindows(wxCommandEvent&event);

    bool getUseFredControlWindows();
    void onAutoBoot(wxCommandEvent&event);
    void onCoinArcadeControl(wxCommandEvent&event);
    void onStopTone(wxCommandEvent&event);
    void onTapeStart(wxCommandEvent&event);
    void onChoiceKeyPadMode(wxCommandEvent&event);
    void onTapeFormat(wxCommandEvent&event);
    void onCardButton(wxCommandEvent&event);
    
    void checkGameFred2(wxString gameName);
    void setGameId(wxString gameName);
    void setGame();
    void setCurrentCardValue();

    void downloadWavFiles(int computer);
    
private:
    int currentCardValue[FRED1_CARDS];
    bool guiStartup_;
    
    DECLARE_EVENT_TABLE()
};

#endif // GUIFRED_H
