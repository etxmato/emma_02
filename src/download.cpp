/*
 *******************************************************************
 *** This software is copyright 2008 by Marcel van Tongeren      ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "main.h"
#include "download.h"

#include "wx/xrc/xmlres.h"             
#include "wx/animate.h"
#include "wx/wfstream.h"

void *downloadThread::Entry()
{
    int computer = p_Main->getSelectedComputerId();
    wxString fileName = p_Main->getWaveDir(computer) + p_Main->getWaveFile(computer);
    
    wxFileOutputStream html_stream(fileName);

    wxString url = "https://www.emma02.hobby-site.com/wave/" + p_Main->getWaveFile(computer);
    url.Replace(" ", "%20");
    wxCurlHTTP http(url);
    
    if (!http.Get(html_stream))
            wxMessageBox( "Download failed", "Emma 02", wxICON_ERROR | wxOK );

    p_Main->setDownloadStatus(false);
    return NULL;
}

BEGIN_EVENT_TABLE(DownloadDialog, wxDialog)
    EVT_BUTTON(XRCID("DownloadYes"), DownloadDialog::onYesButton)
    EVT_BUTTON(XRCID("DownloadNo"), DownloadDialog::onNoButton)
    EVT_BUTTON(XRCID("DownloadCancel"), DownloadDialog::onNoButton)

    EVT_TIMER(wxID_ANY, DownloadDialog::onTimer)

END_EVENT_TABLE()

DownloadDialog::DownloadDialog(wxWindow* parent)
{
	wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"download_" + p_Main->getFontSize() + ".xrc");
	wxXmlResource::Get()->LoadDialog(this, parent, wxT("DownloadDialog"));

	XRCCTRL(*this, "DownloadThrobber", wxAnimationCtrl)->Show(false);
    XRCCTRL(*this, "DownloadCancel", wxButton)->Enable(false);

    XRCCTRL(*this, "DownloadMessage", wxStaticText)->SetLabel(p_Main->getWaveFile(p_Main->getSelectedComputerId()));

    timerPointer = new wxTimer(this, 10001);
    
    downlaodThreadPointer = new downloadThread();
    downloadThreadRunning_ = false;
}

DownloadDialog::~DownloadDialog()
{
    if (downloadThreadRunning_)
    {
        timerPointer->Stop();
        downlaodThreadPointer->Kill();
        
        int computer = p_Main->getSelectedComputerId();
        wxString fileName = p_Main->getWaveDir(computer) + p_Main->getWaveFile(computer);
        
        if (wxFile::Exists(fileName))
        {
            wxFile wavFile;
            wavFile.Open(fileName, wxFile::write);
            wavFile.Write("download");
            wavFile.Close();
        }
    }
    delete timerPointer;
}

void DownloadDialog::onYesButton( wxCommandEvent& WXUNUSED(event) )
{
    timerPointer->Start(100, wxTIMER_CONTINUOUS);
    
	XRCCTRL(*this, "DownloadThrobber", wxAnimationCtrl)->Show();
	XRCCTRL(*this, "DownloadThrobber", wxAnimationCtrl)->Play();
	
    XRCCTRL(*this, "DownloadNo", wxButton)->Enable(false);
    XRCCTRL(*this, "DownloadYes", wxButton)->Enable(false);
    XRCCTRL(*this, "DownloadCancel", wxButton)->Enable(true);

    p_Main->setDownloadStatus(true);

    if ( downlaodThreadPointer->Create() != wxTHREAD_NO_ERROR )
    {
        p_Main->message("Can't create thread");
        return;
    }
    downlaodThreadPointer->SetPriority(WXTHREAD_DEFAULT_PRIORITY);
    downlaodThreadPointer->Run();
    downloadThreadRunning_ = true;
}

void DownloadDialog::onNoButton( wxCommandEvent& WXUNUSED(event) )
{
	EndModal( wxID_OK );
}

void DownloadDialog::onCancelButton( wxCommandEvent& WXUNUSED(event) )
{
    EndModal( wxID_OK );
}

void DownloadDialog::onTimer(wxTimerEvent&WXUNUSED(event))
{
    if (!p_Main->downloadStatus())
    {
        timerPointer->Stop();
        downloadThreadRunning_ = false;
                
        EndModal( wxID_OK );
    }
}
