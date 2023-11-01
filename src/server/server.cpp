#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/socket.h"

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

// Event Table
BEGIN_EVENT_TABLE(Window, wxFrame)
EVT_BUTTON(wxID_BUTSTART, Window::ServerStart)
EVT_SOCKET(wxID_SERVER, Window::OnServerEvent)
EVT_SOCKET(wxID_SOCKET, Window::OnSocketEvent)
END_EVENT_TABLE()

Window::Window(const wxString &title, const wxPoint &pos, const wxSize &size,
               long style) : wxFrame(NULL, -1, title, pos, size, style)
{
    m_panel = new wxPanel(this, wxID_ANY);

    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

    m_start_button = new wxButton(m_panel, wxID_BUTSTART, _T("Start"), wxPoint(10, 10));
    sizer->Add(m_start_button, 0, wxALL, 10);
    m_log_box = new wxTextCtrl(m_panel, wxID_DESC, wxEmptyString, wxPoint(10, 50), wxSize(300, 300),
                               wxTE_MULTILINE | wxTE_BESTWRAP | wxTE_READONLY);
    sizer->Add(m_log_box, 1, wxEXPAND | wxALL, 10);
    m_panel->SetSizer(sizer);

    m_log_box->SetValue("Welcome in my SocketDemo: Server\nServer Ready!\n\n");
}

void Window::ServerStart(wxCommandEvent &evt)
{
    m_start_button->Disable();

    wxIPV4address adr;
    adr.Service(3000);

    m_server = new wxSocketServer(adr);

    m_server->SetEventHandler(*this, wxID_SERVER);
    m_server->SetNotify(wxSOCKET_CONNECTION_FLAG);
    m_server->Notify(true);

    if (!m_server->Ok())
    {
        m_log_box->AppendText(wxT("Could not start server :(\n"));
        m_start_button->Enable();
        return;
    }

    m_log_box->AppendText(wxT("Server started\n"));

    return;
}

void Window::OnServerEvent(wxSocketEvent &evt)
{
    switch (evt.GetSocketEvent())
    {
    case wxSOCKET_CONNECTION:
    {
        m_log_box->AppendText("OnServerEvent : wxSOCKET_CONNECTION\n");

        wxSocketBase *Sock = m_server->Accept(true);

        if (Sock == NULL)
        {
            m_log_box->AppendText("Failed to accept incoming connection\n");
            return;
        }

        m_log_box->AppendText("    Accepted Connection\n");
        Sock->SetEventHandler(*this, wxID_SOCKET);
        Sock->SetNotify(wxSOCKET_CONNECTION_FLAG | wxSOCKET_INPUT_FLAG | wxSOCKET_OUTPUT_FLAG | wxSOCKET_LOST_FLAG);
        Sock->Notify(true);
        break;
    }
    default:
    {
        m_log_box->AppendText("OnServerEvent : unknown event\n");
        break;
    }
    }
}

void Window::OnSocketEvent(wxSocketEvent &evt)
{
    wxSocketBase *Sock = evt.GetSocket();

    switch (evt.GetSocketEvent())
    {
    case wxSOCKET_INPUT:
    {
        m_log_box->AppendText("OnSocketEvent : wxSOCKET_INPUT\n");

        wxSocketBase *Sock = evt.GetSocket();
        char buffer[10];
        Sock->Read(buffer, sizeof(buffer));

        char cstring[256];
        sprintf(cstring, "%c%c%c%c%c%c%c%c%c%c", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5],
                buffer[6], buffer[7], buffer[8], buffer[9]);
        m_log_box->AppendText(wxString("    received ") + cstring + "\n");

        for (int n = 0; n < 10; n++)
        {
            buffer[n] = ('a' + n);
        }
        sprintf(cstring, "%c%c%c%c%c%c%c%c%c%c", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5],
                buffer[6], buffer[7], buffer[8], buffer[9]);
        m_log_box->AppendText(wxString("    sending ") + cstring + "\n");

        Sock->Write(buffer, sizeof(buffer));

        m_log_box->AppendText("    closing socket\n");
        Sock->Destroy();
        break;
    }
    case wxSOCKET_LOST:
    {
        wxSocketBase *Sock = evt.GetSocket();
        m_log_box->AppendText("OnSocketEvent : wxSOCKET_LOST\n");
        Sock->Destroy();
        break;
    }
    case wxSOCKET_OUTPUT:
    {
        m_log_box->AppendText("OnSocketEvent : wxSOCKET_OUTPUT\n");
        break;
    }
    default:
    {
        m_log_box->AppendText("OnSocketEvent : unknown event\n");
        break;
    }
    }
}

bool Api::OnInit()
{
    Window *MainWindow = new Window(_T("SocketDemo: Server"), wxDefaultPosition, wxSize(300, 300));
    MainWindow->Center();
    MainWindow->Show(TRUE);
    return TRUE;
}

IMPLEMENT_APP(Api)