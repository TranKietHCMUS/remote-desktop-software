#ifndef CLIENT_H
#define CLIENT_H

#include <wx/wx.h>
#include <wx/socket.h>
#include <wx/mstream.h>

enum
{
    wxID_BUTCONN = 0,
    wxID_DESC,
    wxID_SOCKET
};

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame
{
protected:
    wxTimer* timer;
    wxImage capturedImage;
    wxPanel *m_panel;
    wxButton *m_connect_button;
    wxTextCtrl *m_log_box;

public:
    MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size, long style = wxDEFAULT_FRAME_STYLE);
    void OnTimer(wxTimerEvent& event);
    void FunConnect(wxCommandEvent &evt);
    void OnSocketEvent(wxSocketEvent &evt);
    void OnClose(wxCloseEvent &evt);
    
private:
    DECLARE_EVENT_TABLE()
};

#endif