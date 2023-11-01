#ifndef CLIENT_H
#define CLIENT_H

#include <wx/wx.h>
#include <wx/socket.h>

enum
{
    wxID_BUTCONN = 0,
    wxID_DESC,
    wxID_SOCKET
};

class Api : public wxApp
{
public:
    virtual bool OnInit();
};

class Window : public wxFrame
{
protected: // Window Controls
    wxPanel *m_panel;
    wxButton *m_connect_button;
    wxTextCtrl *m_log_box;

public:
    Window(const wxString &title, const wxPoint &pos, const wxSize &size,
           long style = wxDEFAULT_FRAME_STYLE);
    void FunConnect(wxCommandEvent &evt);
    void OnSocketEvent(wxSocketEvent &evt);

private:
    DECLARE_EVENT_TABLE()
};

#endif