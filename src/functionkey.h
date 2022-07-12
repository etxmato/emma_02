#ifndef FUNCTIONKEYDLG_H
#define FUNCTIONKEYDLG_H

class FunctionKeyMapDialog : public wxDialog
{
public:

    FunctionKeyMapDialog(wxWindow* parent);
    ~FunctionKeyMapDialog();

private:
    void onSaveButton( wxCommandEvent &event );
    void onFunctionKey( wxCommandEvent &event );
    void onInputFunctionKey( wxCommandEvent &event );
    void onDefault( wxCommandEvent &event );
    void onFocusTimer(wxTimerEvent& event);
    void onKeyDown(wxKeyEvent &event);
    void connectKeyDownEvent(wxWindow* pclComponent);
    void LoadAndCompare(int computerType, wxString computer);
    void LoadAndCompareStudio(int computerType, wxString computer);
    void compareButtons(int toBeCheckedButton, int toBeCheckedValue);
    void updateButtons();
    void setLabel(wxString printStr, long button, int key);
    void onEnableEscape(wxCommandEvent&event);
    void onEnableCtrlv(wxCommandEvent&event);

    int functionKey_[14];
    long fKey_;

    bool useCtrlvKey_;
    bool useExitKey_;
    wxTimer *focusTimer;

    DECLARE_EVENT_TABLE()

};

#endif  // FUNCTIONKEYDLG_H
