#ifndef GUIMICROTUTOR_H
#define GUIMICROTUTOR_H

#include "guiuc1800.h"
#include "elfconfiguration.h"

class GuiMicrotutor: public GuiUc1800
{
public:

    GuiMicrotutor(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~GuiMicrotutor() {};

    void readMicrotutorConfig();
    void writeMicrotutorDirConfig();
    void writeMicrotutorConfig();
    void readMicrotutorWindowConfig();
    void writeMicrotutorWindowConfig();

    void onMicrotutorClock(wxCommandEvent& event);

protected:
    int switchX_, switchY_;

private:

    DECLARE_EVENT_TABLE()
};

class GuiMicrotutor2: public GuiMicrotutor
{
public:
    
    GuiMicrotutor2(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~GuiMicrotutor2() {};
    
    void readMicrotutor2Config();
    void writeMicrotutor2DirConfig();
    void writeMicrotutor2Config();
    void readMicrotutor2WindowConfig();
    void writeMicrotutor2WindowConfig();
    void onUtilityMemory(wxCommandEvent&event);
    
    void onMainRamM(wxCommandEvent& event);
    void onMainRamMText(wxCommandEvent& event);
    void onMainRamE(wxCommandEvent& event);
    void onMainRamEText(wxCommandEvent& event);

    void onMicrotutor2Clock(wxCommandEvent& event);
    
protected:
    int switchX_, switchY_;
    
private:
    
    DECLARE_EVENT_TABLE()
};

#endif // GUIMICROTUTOR_H
