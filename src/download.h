#ifndef DOWNLOADDLG_H
#define DOWNLOADDLG_H

#include "wxcurl_http.h"

class downloadThread : public wxThread
{
public:
    downloadThread() {};
    virtual void *Entry();
};

class DownloadDialog : public wxDialog
{
public:
    DownloadDialog(wxWindow* parent);
    ~DownloadDialog();

private:
    void onYesButton( wxCommandEvent &event );
    void onNoButton( wxCommandEvent &event );
    void onCancelButton( wxCommandEvent &event );
    void onTimer( wxTimerEvent &event );

    wxTimer *timerPointer;

    downloadThread *downlaodThreadPointer;
    bool downloadThreadRunning_;

    DECLARE_EVENT_TABLE()

};

#endif  // DOWNLOADDLG_H
