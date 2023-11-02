#include <client.h>

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    MyFrame *MainWindow = new MyFrame(_T("SocketDemo: Client"), wxDefaultPosition, wxSize(1000, 1000));
    MainWindow->Centre();
    MainWindow->Show(TRUE);
    return TRUE;
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_BUTTON(wxID_BUTCONN, MyFrame::FunConnect)
EVT_SOCKET(wxID_SOCKET, MyFrame::OnSocketEvent)
END_EVENT_TABLE()

MyFrame::MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size,
               long style) : wxFrame(NULL, -1, title, pos, size, style), capturedImage(1,1)
{
    m_panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

    m_connect_button = new wxButton(m_panel, wxID_BUTCONN, _T("Connect"), wxPoint(10, 20));
    sizer->Add(m_connect_button, 0, wxALL, 10);

    m_log_box = new wxTextCtrl(m_panel, wxID_DESC, wxEmptyString, wxPoint(10, 200), wxSize(300, 300), wxTE_MULTILINE | wxTE_BESTWRAP | wxTE_READONLY);
    sizer->Add(m_log_box, 1, wxEXPAND | wxALL, 10);
    
    m_panel->SetSizer(sizer);

    m_log_box->SetValue("Welcome in my SocketDemo: Client\nClient Ready!\n\n");

    timer = new wxTimer(this, wxID_ANY);
    Bind(wxEVT_TIMER, &MyFrame::OnTimer, this, wxID_ANY);
    timer->Start(30);

    Bind(wxEVT_CLOSE_WINDOW, &MyFrame::OnClose, this);
}

void MyFrame::OnTimer(wxTimerEvent& event)
{
    wxScreenDC screenDC;
    wxMemoryDC memDC;

    int screenWidth = screenDC.GetSize().GetWidth();
    int screenHeight = screenDC.GetSize().GetHeight();

    wxBitmap bitmap(screenWidth, screenHeight);
    memDC.SelectObject(bitmap);
    memDC.Blit(0, 0, screenWidth, screenHeight, &screenDC, 0, 0);

    capturedImage = bitmap.ConvertToImage();
}

void MyFrame::FunConnect(wxCommandEvent &evt)
{
    m_log_box->AppendText(_T("Connecting to the server...\n"));
    m_connect_button->Enable(FALSE);

    wxIPV4address adr;
    adr.Hostname(_T("localhost"));
    adr.Service(3000);

    wxSocketClient *Socket = new wxSocketClient();

    Socket->SetEventHandler(*this, wxID_SOCKET);
    Socket->SetNotify(wxSOCKET_CONNECTION_FLAG | wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
    Socket->Notify(TRUE);

    Socket->Connect(adr, false);
    m_log_box->AppendText(_T("Connecting...\n"));

    return;
}

void MyFrame::OnSocketEvent(wxSocketEvent &evt)
{
    wxSocketBase *Sock = evt.GetSocket();
    wxPNGHandler *handler = new wxPNGHandler;
    wxImage::AddHandler(handler);
    char buff[3];

    switch (evt.GetSocketEvent())
    {
    case wxSOCKET_CONNECTION:
    {
        m_log_box->AppendText(_T("OnSocketEvent(wxSOCKET_CONNECTION) Connection successful\n"));
        wxMemoryOutputStream stream;
        capturedImage.SaveFile(stream, wxBITMAP_TYPE_PNG);
        wxUint32 imageSize = stream.GetSize();
        Sock->Write(&imageSize, sizeof(wxUint32));
        Sock->Write(stream.GetOutputStreamBuffer()->GetBufferStart(), imageSize);
        break;
    }
    case wxSOCKET_INPUT:
    {
        m_log_box->AppendText(_T("OnSocketEvent(wxSOCKET_INPUT):\n"));
        Sock->Read(buff, 3);
        wxMemoryOutputStream stream;
        capturedImage.SaveFile(stream, wxBITMAP_TYPE_PNG);
        wxUint32 imageSize = stream.GetSize();
        Sock->Write(&imageSize, sizeof(wxUint32));
        Sock->Write(stream.GetOutputStreamBuffer()->GetBufferStart(), imageSize);
        break;
    }
    case wxSOCKET_OUTPUT:
    {
        m_log_box->AppendText(_T("OnSocketEvent(wxSOCKET_OUTPUT):\n"));
        break;
    }
    case wxSOCKET_LOST:
    {
        m_log_box->AppendText(_T("OnSocketEvent(wxSOCKET_LOST)\n"));
        Sock->Destroy();
        m_connect_button->Enable(TRUE);
        break;
    }

    default:
    {
        m_log_box->AppendText(_T("OnSocketEvent : unknown socket event\n"));
        break;
    }
    }
}

void MyFrame::OnClose(wxCloseEvent &evt)
{
    timer->Stop();
    Destroy();
}

// #include "wx/wxprec.h"
// #ifndef WX_PRECOMP
// #include "wx/wx.h"
// #endif

// #include "wx/socket.h"

// enum
// {
//     wxID_BUTCONN = 0,
//     wxID_DESC,
//     wxID_SOCKET
// };

// class Api : public wxApp
// {
// public:
//     virtual bool OnInit();
// };

// class Window : public wxFrame
// {
// protected: // Window Controls
//     wxPanel *m_panel;
//     wxButton *m_connect_button;
//     wxTextCtrl *m_log_box;

// public:
//     Window(const wxString &title, const wxPoint &pos, const wxSize &size,
//            long style = wxDEFAULT_FRAME_STYLE);
//     void FunConnect(wxCommandEvent &evt);
//     void OnSocketEvent(wxSocketEvent &evt);

// private:
//     DECLARE_EVENT_TABLE()
// };

// // Event Table
// BEGIN_EVENT_TABLE(Window, wxFrame)
// EVT_BUTTON(wxID_BUTCONN, Window::FunConnect)
// EVT_SOCKET(wxID_SOCKET, Window::OnSocketEvent)
// END_EVENT_TABLE()

// void Window::FunConnect(wxCommandEvent &evt)
// {
//     m_log_box->AppendText(_T("Connecting to the server...\n"));
//     m_connect_button->Enable(FALSE);

//     // Connecting to the server
//     wxIPV4address adr;
//     adr.Hostname(_T("localhost"));
//     adr.Service(3000);

//     // Create the socket
//     wxSocketClient *Socket = new wxSocketClient();

//     Socket->SetEventHandler(*this, wxID_SOCKET);
//     Socket->SetNotify(wxSOCKET_CONNECTION_FLAG | wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
//     Socket->Notify(TRUE);

//     Socket->Connect(adr, false);
//     m_log_box->AppendText(_T("Connecting...\n"));

//     return;
// }

// void Window::OnSocketEvent(wxSocketEvent &evt)
// {
//     wxSocketBase *Sock = evt.GetSocket();

//     char buffer[10];

//     switch (evt.GetSocketEvent())
//     {
//     case wxSOCKET_CONNECTION:
//     {
//         m_log_box->AppendText(_T("OnSocketEvent(wxSOCKET_CONNECTION) Connection successful\n"));

//         char mychar = '0';

//         for (int i = 0; i < 10; ++i)
//         {
//             buffer[i] = mychar++;
//         }

//         Sock->Write(buffer, sizeof(buffer));

//         char cstring[256];
//         sprintf(cstring, "%c%c%c%c%c%c%c%c%c%c\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5],
//                 buffer[6], buffer[7], buffer[8], buffer[9]);
//         m_log_box->AppendText(wxString("    Sent ") + cstring + "\n");

//         break;
//     }

//     case wxSOCKET_INPUT:
//     {
//         m_log_box->AppendText(_T("OnSocketEvent(wxSOCKET_INPUT)\n"));
//         Sock->Read(buffer, sizeof(buffer));

//         char cstring[256];
//         sprintf(cstring, "%c%c%c%c%c%c%c%c%c%c\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5],
//                 buffer[6], buffer[7], buffer[8], buffer[9]);
//         m_log_box->AppendText(wxString("    received ") + cstring + "\n");

//         break;
//     }

//     case wxSOCKET_LOST:
//     {
//         m_log_box->AppendText(_T("OnSocketEvent(wxSOCKET_LOST)\n"));
//         Sock->Destroy();
//         m_connect_button->Enable(TRUE);
//         break;
//     }

//     default:
//     {
//         m_log_box->AppendText(_T("OnSocketEvent : unknown socket event\n"));
//         break;
//     }
//     }
// }

// Window::Window(const wxString &title, const wxPoint &pos, const wxSize &size,
//                long style) : wxFrame(NULL, -1, title, pos, size, style)
// {
//     m_panel = new wxPanel(this, wxID_ANY);
//     wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

//     m_connect_button = new wxButton(m_panel, wxID_BUTCONN, _T("Connect"), wxPoint(10, 10));
//     sizer->Add(m_connect_button, 0, wxALL, 10);

//     m_log_box = new wxTextCtrl(m_panel, wxID_DESC, wxEmptyString, wxPoint(10, 50), wxSize(300, 300), wxTE_MULTILINE | wxTE_BESTWRAP | wxTE_READONLY);
//     sizer->Add(m_log_box, 1, wxEXPAND | wxALL, 10);
//     m_panel->SetSizer(sizer);

//     m_log_box->SetValue("Welcome in my SocketDemo: Client\nClient Ready!\n\n");
// }

// bool Api::OnInit()
// {
//     Window *MainWindow = new Window(_T("SocketDemo: Client"), wxDefaultPosition, wxSize(500, 500));
//     MainWindow->Centre();
//     MainWindow->Show(TRUE);
//     return TRUE;
// }

// IMPLEMENT_APP(Api)