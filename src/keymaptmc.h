#ifndef KEYMAPTMCDLG_H
#define KEYMAPTMCDLG_H

class KeyMapTmcDialog : public wxDialog
{
public:

    KeyMapTmcDialog(wxWindow* parent);
	~KeyMapTmcDialog();

private:
    void onSaveButton( wxCommandEvent &event );
    void onKey( wxCommandEvent &event );
    void onDefault( wxCommandEvent &event );
	void onKeyDown(wxKeyEvent& event);
	void connectKeyDownEvent(wxWindow* pclComponent); 
	void compareButtons(int toBeCheckedKey, int toBeCheckedValue, int type);
	void updateButtons();
	void setLabel(wxString printStr, long button, int key);

    void onFocusTimer(wxTimerEvent& event);
    
    wxTimer *focusTimer;
    
   	int keyDefNormal_[21];
   	int keyDefShift_[21];
    
    long normalKey_;
    long shiftKey_;
    
    DECLARE_EVENT_TABLE()

};

#endif  // KEYMAPTMCDLG_H
