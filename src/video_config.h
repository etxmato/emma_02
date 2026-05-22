#ifndef VIDEO_CONFIG_H
#define VIDEO_CONFIG_H

#include "ct2425_config.h"

class VideoConfig: public Ct2425Config
{
public:

    VideoConfig(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~VideoConfig();

    void videoConfigUsed(wxString panelName);
    void updateVideoPanel();
    void showVideoTraceText(wxString function, wxString address, wxString value, int showTrace);
    void showVideoTraceText(wxString function, wxString value, int showTrace);
    void showVideoTraceTextRead(wxString function, wxString value, int showTrace);
    void showVideoTraceText(wxString function, int showTrace);
    void videoTimeTrace();
    void showVideoNotRunning();
    void videoTrace(wxString buffer);
    long getVideoRegisterValue(wxString registerReference);

protected:
    void videoConfigInit();

    wxTextCtrl *videoTraceWindowPointer;
    wxString videoTraceString_;

    wxPoint videoConfigGuiPosition_;
    int videoConfigWidth_;

    bool videoTrace_;
    bool videoTime_;
    bool restartVideoTime_;

private:
    void onVideoTrace(wxCommandEvent&event);
    void onVideoTime(wxCommandEvent&event);
    void onVideoClear(wxCommandEvent&event);

    DECLARE_EVENT_TABLE()
};

#endif // VIDEO_CONFIG_H
