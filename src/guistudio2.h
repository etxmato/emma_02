#ifndef GUISTUDIO2_H
#define GUISTUDIO2_H

#include "guivip.h"

class GuiStudio2: public GuiVip
{
public:

	GuiStudio2(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
	~GuiStudio2() {};

	void readStudioConfig();
    void writeStudioDirConfig();
    void writeStudioConfig();
    void readStudioWindowConfig();
    void writeStudioWindowConfig();
	void onMultiCartStudio(wxCommandEvent&event); 
	void onDisableSystemRomStudio(wxCommandEvent&event);
	void onMsbStudio(wxSpinEvent&event);
    void onLsbStudio(wxSpinEvent&event);
    
    void readCoinArcadeConfig();
    void writeCoinArcadeDirConfig();
    void writeCoinArcadeConfig();
    void readCoinArcadeWindowConfig();
    void writeCoinArcadeWindowConfig();

    void readVisicomConfig();
    void writeVisicomDirConfig();
    void writeVisicomConfig();
    void readVisicomWindowConfig();
    void writeVisicomWindowConfig();
    
	void readVictoryConfig();
    void writeVictoryDirConfig();
    void writeVictoryConfig();
    void readVictoryWindowConfig();
    void writeVictoryWindowConfig();
	void onMultiCartVictory(wxCommandEvent&event);
	void onDisableSystemRomVictory(wxCommandEvent&event);
	void onMsbVictory(wxSpinEvent&event);
	void onLsbVictory(wxSpinEvent&event);

    void readStudioIVConfig();
    void writeStudioIVDirConfig();
    void writeStudioIVConfig();
    void readStudioIVWindowConfig();
    void writeStudioIVWindowConfig();
    
	void onSt2Rom(wxCommandEvent& event);
	void onSt2RomEject(wxCommandEvent& event);

private:
	wxString studio2St2RomDir_;

	DECLARE_EVENT_TABLE()
};

#endif // GUISTUDIO2_H
