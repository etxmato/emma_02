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
#include "psave.h"

#include "wx/xrc/xmlres.h"             
#include <wx/spinctrl.h>

BEGIN_EVENT_TABLE(PsaveDialog, wxDialog)
    EVT_BUTTON(XRCID("PsaveSave"), PsaveDialog::onSaveButton)
    EVT_CHECKBOX(XRCID("UseXml"), PsaveDialog::onuseXml)
END_EVENT_TABLE()

PsaveDialog::PsaveDialog(wxWindow* parent)
{
    wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"cassette_" + p_Main->getFontSize() + ".xrc");
    wxXmlResource::Get()->LoadDialog(this, parent, wxT("PsaveDialog"));

    XRCCTRL(*this, "PsaveVolume", wxSpinCtrl)->SetValue(p_Main->getPsaveData(0));
    XRCCTRL(*this, "PsaveBitRate", wxChoice)->SetSelection(p_Main->getPsaveData(1));
    XRCCTRL(*this, "PsaveBitsPerSample", wxChoice)->SetSelection(p_Main->getPsaveData(2));
    XRCCTRL(*this, "ConversionTypeWav", wxChoice)->SetSelection(p_Main->getPsaveData(3));
    XRCCTRL(*this, "Channel", wxChoice)->SetSelection(p_Main->getPsaveData(4));
    XRCCTRL(*this, "ConversionTypeCassette", wxChoice)->SetSelection(p_Main->getPsaveData(7));
    XRCCTRL(*this, "Playback", wxCheckBox)->SetValue(p_Main->getPsaveData(5) != 0);
    XRCCTRL(*this, "ReversedPolarity", wxCheckBox)->SetValue(p_Main->getPsaveData(6) != 0);
    
    int useXml = p_Main->getPsaveData(13);
    XRCCTRL(*this, "UseXml", wxCheckBox)->SetValue(useXml != 0);
    
    if (useXml == 1)
    {
        XRCCTRL(*this, "FREDThreshold8", wxTextCtrl)->Enable(false);
        XRCCTRL(*this, "FREDThreshold16", wxTextCtrl)->Enable(false);
        XRCCTRL(*this, "FREDThreshold24", wxTextCtrl)->Enable(false);
        XRCCTRL(*this, "FREDThresholdText8", wxStaticText)->Enable(false);
        XRCCTRL(*this, "FREDThresholdText16", wxStaticText)->Enable(false);
        XRCCTRL(*this, "FREDThresholdText24", wxStaticText)->Enable(false);
    }
    wxString threshold;
    threshold.Printf("%d", p_Main->getPsaveData(8));
    XRCCTRL(*this, "FREDThreshold8", wxTextCtrl)->SetValue(threshold);
    threshold.Printf("%d", p_Main->getPsaveData(9));
    XRCCTRL(*this, "FREDThreshold16", wxTextCtrl)->SetValue(threshold);
    threshold.Printf("%d", p_Main->getPsaveData(14));
    XRCCTRL(*this, "FREDThreshold24", wxTextCtrl)->SetValue(threshold);
    threshold.Printf("%1.1f", (float)p_Main->getPsaveData(10)/10);
    XRCCTRL(*this, "FREDFreq", wxTextCtrl)->SetValue(threshold);
    wxString stringCvValue;
    stringCvValue.Printf("%d", p_Main->getPsaveData(11));
    XRCCTRL(*this, "CVa", wxTextCtrl)->SetValue(stringCvValue);
    stringCvValue.Printf("%1.2f", (float)p_Main->getPsaveData(12)/100);
    XRCCTRL(*this, "CVb", wxTextCtrl)->SetValue(stringCvValue);
}

void PsaveDialog::onuseXml(wxCommandEvent&event)
{
    bool checked = event.IsChecked();
    
    XRCCTRL(*this, "FREDThreshold8", wxTextCtrl)->Enable(!checked);
    XRCCTRL(*this, "FREDThreshold16", wxTextCtrl)->Enable(!checked);
    XRCCTRL(*this, "FREDThreshold24", wxTextCtrl)->Enable(!checked);
    XRCCTRL(*this, "FREDThresholdText8", wxStaticText)->Enable(!checked);
    XRCCTRL(*this, "FREDThresholdText16", wxStaticText)->Enable(!checked);
    XRCCTRL(*this, "FREDThresholdText24", wxStaticText)->Enable(!checked);
}

void PsaveDialog::onSaveButton( wxCommandEvent& WXUNUSED(event) )
{
    if (p_Main->isSaving())
        return;

    p_Main->setPsaveData(0, XRCCTRL(*this, "PsaveVolume", wxSpinCtrl)->GetValue());
    p_Main->setPsaveData(1, XRCCTRL(*this, "PsaveBitRate", wxChoice)->GetSelection());
    p_Main->setPsaveData(2, XRCCTRL(*this, "PsaveBitsPerSample", wxChoice)->GetSelection());
    p_Main->setPsaveData(3, XRCCTRL(*this, "ConversionTypeWav", wxChoice)->GetSelection());
    p_Main->setPsaveData(4, XRCCTRL(*this, "Channel", wxChoice)->GetSelection());
    p_Main->setPsaveData(7, XRCCTRL(*this, "ConversionTypeCassette", wxChoice)->GetSelection());
    p_Main->setPsaveData(5, XRCCTRL(*this, "Playback", wxCheckBox)->GetValue());
    p_Main->setPsaveData(6, XRCCTRL(*this, "ReversedPolarity", wxCheckBox)->GetValue());
    p_Main->setPsaveData(13, XRCCTRL(*this, "UseXml", wxCheckBox)->GetValue());

    wxString stringThreshold = XRCCTRL(*this, "FREDThreshold8", wxTextCtrl)->GetValue();
    if (stringThreshold == "")  stringThreshold = "10";
    long threshold;
    if (!stringThreshold.ToLong(&threshold, 10))
        threshold = 10;
    
    p_Main->setPsaveData(8, (int)threshold);

    stringThreshold = XRCCTRL(*this, "FREDThreshold16", wxTextCtrl)->GetValue();
    if (stringThreshold == "")  stringThreshold = "500";
    if (!stringThreshold.ToLong(&threshold, 10))
        threshold = 500;
    p_Main->setPsaveData(9, (int)threshold);

    stringThreshold = XRCCTRL(*this, "FREDThreshold24", wxTextCtrl)->GetValue();
    if (stringThreshold == "")  stringThreshold = "1000";
    if (!stringThreshold.ToLong(&threshold, 10))
        threshold = 1000;
    p_Main->setPsaveData(14, (int)threshold);

    stringThreshold = XRCCTRL(*this, "FREDFreq", wxTextCtrl)->GetValue();
    if (stringThreshold == "")  stringThreshold = "5.8";
    double freq;
    if (!p_Main->toDouble(stringThreshold, &freq))
        freq = 5.8;
    p_Main->setPsaveData(10, freq*10);
    
    wxString stringCvValue;
    stringCvValue = XRCCTRL(*this, "CVa", wxTextCtrl)->GetValue();
    if (stringCvValue == "")  stringCvValue = "18";
    long cvValue;
    if (!stringCvValue.ToLong(&cvValue, 10))
        cvValue = 18;
    p_Main->setPsaveData(11, (int)cvValue);

    stringCvValue = XRCCTRL(*this, "CVb", wxTextCtrl)->GetValue();
    if (stringCvValue == "")  stringCvValue = "0.18";
    double cvb;
    if (!p_Main->toDouble(stringCvValue, &cvb))
        cvb = 0.18;
    p_Main->setPsaveData(12, cvb*100);

    EndModal( wxID_OK );
}

