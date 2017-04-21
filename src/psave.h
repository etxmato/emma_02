#ifndef PSAVEDLG_H
#define PSAVEDLG_H

#include "wx/spinctrl.h"

class PsaveDialog : public wxDialog
{
public:

    PsaveDialog(wxWindow* parent);
    ~PsaveDialog(){};

private:
    void onSaveButton( wxCommandEvent &event );

    DECLARE_EVENT_TABLE()

};

#endif  // PSAVEDLG_H
