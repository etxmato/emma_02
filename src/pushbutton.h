#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

DECLARE_EVENT_TYPE(wxEVT_ButtonDownEvent, -1) 
DECLARE_EVENT_TYPE(wxEVT_ButtonUpEvent, -1) 
 
class PushBitmapButton : public wxBitmapButton
{
public:
    PushBitmapButton(wxWindow* parent, wxWindowID id, const wxBitmap& bitmap, const wxPoint& pos, const wxSize& size, long style);
    ~PushBitmapButton();

private:
    void onLeftMouseDown(wxMouseEvent& pushEvent);
    void onLeftMouseUp(wxMouseEvent& pushEvent);

    DECLARE_EVENT_TABLE()
};

class PushButton : public wxButton 
{ 
public: 
    PushButton(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos, const wxSize& size, long style); 
    ~PushButton(); 

private: 
    void onLeftMouseDown(wxMouseEvent& pushEvent); 
    void onLeftMouseUp(wxMouseEvent& pushEvent); 

    DECLARE_EVENT_TABLE() 
}; 
#endif  // PUSHBUTTON_H
