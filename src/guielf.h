#ifndef GUIELF_H
#define GUIELF_H

#include "guielf2k.h"
#include "elfconfiguration.h"

class GuiElf: public GuiElf2K
{
public:

	GuiElf(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
	~GuiElf() {};

    void readElfConfig(int elfType, wxString elfTypeStr);
	void writeElfDirConfig(int elfType, wxString elfTypeStr);
	void writeElfConfig(int elfType, wxString elfTypeStr);
    void readElfWindowConfig(int elfType, wxString elfTypeStr);
    void writeElfWindowConfig(int elfType, wxString elfTypeStr);

	void onDiskType(wxCommandEvent& event);
	void onMemory(wxCommandEvent& event);
	void onAutoBoot(wxCommandEvent& event);
	void onStartRam(wxCommandEvent& event);
	void onEndRam(wxCommandEvent& event);
	void onVideoType(wxCommandEvent& event);
	void onElfKeyboard(wxCommandEvent& event);
    void onForceUpperCase(wxCommandEvent& event);
    void onGiantBoardMapping(wxCommandEvent& event);
    void onEfButtons(wxCommandEvent& event);
 	void onUsePortExtender(wxCommandEvent& event);
	void onLedModule(wxCommandEvent& event);
	void onElfControlWindows(wxCommandEvent& event);
	void onRom1(wxCommandEvent& event);
	void onRom2(wxCommandEvent& event);
	void onElfScreenDump(wxCommandEvent& event);
	void onTape(wxCommandEvent& event);
	void onQsound(wxCommandEvent&event);
    void onHexEf(wxCommandEvent&event);
    void onBootStrap(wxCommandEvent&event);
    void onTilType(wxCommandEvent&event);

	int getLoadromMode(int elfType, int num);
	long getStartRam(wxString elfTypeStr, int elfType);
	long getEndRam(wxString elfTypeStr, int elfType);
	void setDiskType(wxString elfTypeStr, int elfType, int Selection);
	void setMemory(wxString elfTypeStr, int elfType, int Selection);
	void setElfKeyboard(wxString elfTypeStr, int elfType, int Selection, bool hexEf);
	void setVideoType(wxString elfTypeStr, int elfType, int Selection);
	void setTapeType(wxString elfTypeStr, int elfType);

    bool getUpperCase(int elfType) {return elfConfiguration[elfType].forceUpperCase;};
    bool getUseXmodem(int elfType);
    bool getUseHexModem(int elfType);
    bool getUsePacketSize1K(int elfType) {return elfConfiguration[elfType].usePacketSize1K;};
	bool getUseElfControlWindows(int elfType) {return elfConfiguration[elfType].useElfControlWindows;};
	bool getUseSwitch(int elfType) {return elfConfiguration[elfType].useSwitch;};
	bool getUseHex(int elfType) {return elfConfiguration[elfType].useHex;};
	bool getUseTape(int elfType) {return elfConfiguration[elfType].useTape;};
	wxString getVtWaveFile(int elfType) {return elfConfiguration[elfType].vtWavFile_;};

	wxPoint getLedModulePos();
	void setLedModulePos(wxPoint position);
	void reset6847ConfigItem(int num);

protected:
	int ledModuleX_, ledModuleY_;
    long startRam_[3];
    long endRam_[3];
    
private:

	int loadromMode_[3][2];

	wxBitmap tapeOffBitmap;
	wxBitmap tapeOnBitmap;

	DECLARE_EVENT_TABLE()
};

#endif // GUIELF_H
