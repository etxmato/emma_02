#ifndef VIDEO_CONFIG_H
#define VIDEO_CONFIG_H

#include "xmlbase.h"

class VideoConfig: public XmlBase
{
public:

    VideoConfig(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~VideoConfig();

    void videoConfigUsed(wxString panelName);
    void updateVideoPanel();
    void showTraceText(wxString function, wxString address, wxString value, int showTrace);
    void showTraceText(wxString function, wxString value, int showTrace);
    void showTraceTextRead(wxString function, wxString value, int showTrace);
    void showTraceText(wxString function, int showTrace);
    void showNotRunning();
    void videoTrace(wxString buffer);

protected:
    void videoConfigInit();

    wxTextCtrl *videoTraceWindowPointer;
    wxString videoTraceString_;

    wxPoint videoConfigGuiPosition_;
    int videoConfigWidth_;

    bool videoTrace_;

private:
    void onVideoTrace(wxCommandEvent&event);
    void onVideoClear(wxCommandEvent&event);

    DECLARE_EVENT_TABLE()
};

#endif // VIDEO_CONFIG_H
