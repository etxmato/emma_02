#ifndef OTHER_CONFIG_H
#define OTHER_CONFIG_H

#include "xmlbase.h"

class OtherConfig: public XmlBase
{
public:

    OtherConfig(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~OtherConfig();

    void otherConfigUsed(wxString panelName);
    void updateOtherPanel();
    void showOtherTraceText(wxString function, wxString address, wxString value, int showTrace);
    void showOtherTraceText(wxString function, wxString value, int showTrace);
    void showOtherTraceTextRead(wxString function, wxString value, int showTrace);
    void showOtherTraceText(wxString function, int showTrace);
    void otherTimeTrace();
    void showOtherNotRunning();
    void otherTrace(wxString buffer);
    long getOtherRegisterValue(wxString registerReference);

protected:
    void otherConfigInit();

    wxTextCtrl *otherTraceWindowPointer;
    wxString otherTraceString_;

    wxPoint otherConfigGuiPosition_;
    int otherConfigWidth_;

    bool otherTrace_;
    bool otherTime_;
    bool restartOtherTime_;

private:
    void onOtherTrace(wxCommandEvent&event);
    void onOtherClear(wxCommandEvent&event);
    void onOtherTime(wxCommandEvent&event);

    DECLARE_EVENT_TABLE()
};

#endif // OTHER_CONFIG_H
