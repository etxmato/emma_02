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
#include "datadir.h"

#include "wx/xrc/xmlres.h"             
#include "wx/filepicker.h"
#include "wx/dir.h"
#include "wx/animate.h"

DEFINE_EVENT_TYPE(MOVE_FINISHED)

BEGIN_EVENT_TABLE(DatadirDialog, wxDialog)
    EVT_BUTTON(XRCID("DataDirMove"), DatadirDialog::onMoveButton)
    EVT_BUTTON(XRCID("DataDirSet"), DatadirDialog::onSetButton)
END_EVENT_TABLE()

DatadirDialog::DatadirDialog(wxWindow* parent)
{
	wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"datadir.xrc");
	wxXmlResource::Get()->LoadDialog(this, parent, wxT("DataDirDialog"));

	XRCCTRL(*this, "DataDir", wxDirPickerCtrl)->SetPath(p_Main->getDataDir());
	XRCCTRL(*this, "DataDirThrobber", wxAnimationCtrl)->Show(false);
}

void DatadirDialog::onMoveButton( wxCommandEvent& WXUNUSED(event) )
{
	XRCCTRL(*this, "DataDirThrobber", wxAnimationCtrl)->Show();
	XRCCTRL(*this, "DataDirMove", wxButton)->Enable(false);
	XRCCTRL(*this, "DataDirSet", wxButton)->Enable(false);
	XRCCTRL(*this, "DataDir", wxDirPickerCtrl)->Enable(false);

	XRCCTRL(*this, "DataDirThrobber", wxAnimationCtrl)->Play();

	wxString filename;
	wxString dataDir = p_Main->getDataDir();
	wxString newDataDir = XRCCTRL(*this, "DataDir", wxDirPickerCtrl)->GetPath() + p_Main->getPathSep();

	wxFileName destination(newDataDir);
	wxDir dir (dataDir);
	bool cont = dir.GetFirst(&filename);

	while ( cont )
	{
        if (wxDir::Exists(dataDir + filename))
            filename += p_Main->getPathSep();

        wxFileName source(dataDir + filename);

		copyTree(&source, &destination);
		cont = dir.GetNext(&filename);
	}
	p_Main->setDataDir(newDataDir);
	p_Main->setDefaultSettings();

	EndModal( wxID_OK );
}

void DatadirDialog::onSetButton( wxCommandEvent& WXUNUSED(event) )
{
	wxString newDataDir = XRCCTRL(*this, "DataDir", wxDirPickerCtrl)->GetPath() + p_Main->getPathSep();
	p_Main->setDataDir(newDataDir);
	p_Main->setDefaultSettings();

	EndModal( wxID_OK );
}

bool DatadirDialog::copyTree( wxFileName* source, wxFileName* destination ) 
{ 
    // Copy file if it isn't a directory. 
    if ( ! wxDir::Exists(source->GetFullPath()) ) 
    { 
 		if ( !wxRenameFile (source->GetFullPath(), destination->GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + source->GetFullName() ))
		{
			if ( ! wxCopyFile(source->GetFullPath(), destination->GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + source->GetFullName() )) 
				return false; 

			wxRemoveFile (source->GetFullPath());
		}
        wxYield ();
        return true; 
    } 
    else 
    {
#if defined (__WXMAC__)
        if( ! wxFileName::Mkdir(destination->GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + source->GetDirs()[source->GetDirCount() - 1], 0777, wxPATH_MKDIR_FULL) )
                return false;
#else
        if( ! wxFileName::Mkdir(destination->GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + source->GetDirs()[source->GetDirCount() - 1] + p_Main->getPathSep(), 0777, wxPATH_MKDIR_FULL) )
            return false;
#endif
    }

    // Deal sequentially with each child file & subdir. 
    wxDir dir( source->GetPath() ); 
    if ( !dir.IsOpened() )        return false; 

    // Go thru the dir, cloning 1 child @ a time. 
    wxString filename; 
    bool cont = dir.GetFirst( &filename ); 
    while ( cont ) 
    { 
        wxString childPath = source->GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + filename; 
//#if defined (__WXMAC__)
//        wxString newDestinationPath = destination->GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + source->GetDirs()[source->GetDirCount() - 1];
//#else
        wxString newDestinationPath = destination->GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + source->GetDirs()[source->GetDirCount() - 1] + p_Main->getPathSep();
//#endif
        wxFileName child;

        if ( wxDir::Exists(childPath) ) 
        { 
            child.Assign( childPath + p_Main->getPathSep());
        }
        else 
        { 
            child.Assign( source->GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR), filename ); 
        } 

        // Make a new wxFileName for child 
        wxFileName newDestination( newDestinationPath ); 

        // Clone it by recursion, whether it's a dir or file. 
        if ( ! copyTree( &child, &newDestination ) ) 
            // If this fails, bug out. 
            return false; 

        cont = dir.GetNext(&filename); 
    } 

	wxRmDir (source->GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR));
    return true; 
} 
