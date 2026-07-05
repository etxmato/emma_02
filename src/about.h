

class MyAboutDialog : public wxDialog
{
public:
    MyAboutDialog(wxWindow* parent);
    ~MyAboutDialog() {};

private:
    void onOkButton(wxCommandEvent &event);

    DECLARE_EVENT_TABLE()

};
