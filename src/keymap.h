#ifndef KEYMAPDLG_H
#define KEYMAPDLG_H

#define KEY_UP 0
#define KEY_LEFT 1
#define KEY_RIGHT 2
#define KEY_DOWN 3
#define KEY_FIRE 4
#define KEY_COIN 5

class KeyMapDialog : public wxDialog
{
public:

    KeyMapDialog(wxWindow* parent);
    ~KeyMapDialog();

private:
    void onSaveButton( wxCommandEvent &event );
    void onInButton( wxCommandEvent &event );
    void onHexKey( wxCommandEvent &event );
    void onGameKey( wxCommandEvent &event );
    void onGameHex( wxCommandEvent &event );
    void onSwitchPad( wxCommandEvent &event );
    void onSwitchPlayer( wxCommandEvent &event );
    void onHexLocation( wxCommandEvent &event );
    void onTinyBASIC( wxCommandEvent &event );
    void onHexChar( wxCommandEvent &event );
    void onSwitchStudio( wxCommandEvent &event );
    void onSwitchSet( wxCommandEvent &event );
    void updatePadAndSet();
    void onHexSwitchSet1( wxCommandEvent &event );
    void onHexSwitchSet2( wxCommandEvent &event );
    void onStudioLocation( wxCommandEvent &event );
    void onStudioChar( wxCommandEvent &event );
    void onKeyDown(wxKeyEvent& event);
    void connectKeyDownEvent(wxWindow* pclComponent); 
    void compareButtons(int toBeCheckedHex, int toBeCheckedValue, int type);
    void updateButtons();
    wxString getKeyStr(int key);
    void setLabel(wxString printStr, long button, int key);
    void onAuto(wxCommandEvent&event);
    void enableAuto(bool status);
    void onStudioEnableDiagonal(wxCommandEvent&event);

    void onFocusTimer(wxTimerEvent& event);
    
    void updateButtonsCoinArcade();
    void onArcadeADirKey(wxCommandEvent &event);
    void onArcadeBDirKey(wxCommandEvent &event);
    void onArcadeCoinKey(wxCommandEvent &event);
    void onSaveArcadeCoin(wxCommandEvent&event);
    void onDefaultArcadeCoin(wxCommandEvent&event);

    wxTimer *focusTimer;
    
    int inButton1_;
    int inButton2_;

    long arcadeADirKey_;
    long arcadeBDirKey_;
    long arcadeCoinKey_;
    long hexKey_;
    long inKey_;
    int hexKeyDefA1_[16];
    int hexKeyDefA2_[16];
    int keyDefGameHexA_[5];
    int keyDefGameValueA_[5];
    int keyDefCoin_;
    
    int hexKeyDefB1_[16];
    int hexKeyDefB2_[18];
    int keyDefGameHexB_[5];

    bool hexPadA_;
    bool hexPadSet1_;
    bool gamePlayer1_;
    bool hexPadBdefined_;
    bool player2defined_;
    wxString computerTypeStr_;
    int numberOfKeys_;
    bool autoGame_;

    bool simDefA2_;
    bool simDefB2_;
    
    DECLARE_EVENT_TABLE()

};

#endif  // KEYMAPDLG_H
