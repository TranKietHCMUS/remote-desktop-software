#ifndef SERVER_H
#define SERVER_H

#include <wx/wx.h>
#include <wx/socket.h>

enum
{
    wxID_SOCKET = wxID_HIGHEST,
    wxID_SERVER,
    wxID_BUTSTART,
    wxID_DESC
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
    wxButton *m_start_button;
    wxTextCtrl *m_log_box;
    wxSocketServer *m_server;

public:
    Window(const wxString &title, const wxPoint &pos, const wxSize &size,
           long style = wxDEFAULT_FRAME_STYLE);
    void ServerStart(wxCommandEvent &evt);
    void OnServerEvent(wxSocketEvent &evt);
    void OnSocketEvent(wxSocketEvent &evt);

private:
    DECLARE_EVENT_TABLE()
};

#endif