#ifndef BREAKPOINTSDLG_H
#define BREAKPOINTSDLG_H

class BreakPointsDialog : public wxDialog
{
public:

    BreakPointsDialog(wxWindow* parent);
    ~BreakPointsDialog(){};
    DECLARE_EVENT_TABLE()

private:
    void onClose( wxCommandEvent &event );


};

#endif  // BREAKPOINTSDLG_H
