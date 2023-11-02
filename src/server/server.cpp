#include <server.h>

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    MyFrame *MainWindow = new MyFrame(_T("SocketDemo: Server"), wxDefaultPosition, wxSize(300, 300));
    MainWindow->Center();
    MainWindow->Show(TRUE);
    return TRUE;
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_BUTTON(wxID_BUTSTART, MyFrame::ServerStart)
EVT_SOCKET(wxID_SERVER, MyFrame::OnServerEvent)
EVT_SOCKET(wxID_SOCKET, MyFrame::OnSocketEvent)
END_EVENT_TABLE()

MyFrame::MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size,
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

void MyFrame::ServerStart(wxCommandEvent &evt)
{
    m_start_button->Disable();

    wxIPV4address adr;
    adr.Service(3000);

    m_server = new wxSocketServer(adr, wxSOCKET_NOWAIT);

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

    DisplayScreen();

    return;
}

void MyFrame::OnServerEvent(wxSocketEvent &evt)
{
    switch (evt.GetSocketEvent())
    {
    case wxSOCKET_CONNECTION:
    {
        m_log_box->AppendText("OnServerEvent : wxSOCKET_CONNECTION\n");

        wxSocketBase* Sock = m_server->Accept(true);

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

void MyFrame::OnSocketEvent(wxSocketEvent &evt)
{
    wxSocketBase *Sock = evt.GetSocket();
    char buff[3] = "ok";
    switch (evt.GetSocketEvent())
    {
    case wxSOCKET_INPUT:
    {
        m_log_box->AppendText("OnSocketEvent : wxSOCKET_INPUT\n");

        wxPNGHandler *handler = new wxPNGHandler;
        wxImage::AddHandler(handler);
        wxUint32 imageSize = 0;
        Sock->Read(&imageSize, sizeof(wxUint32));
        std::vector<char> buf(imageSize);
        Sock->Read(buf.data(), imageSize);
        wxMemoryInputStream stream(buf.data(), buf.size());
        receivedImage.LoadFile(stream, wxBITMAP_TYPE_PNG);
        data = receivedImage;
        Sock->Write(buff, sizeof(buff));

        break;
    }
    case wxSOCKET_OUTPUT:
    {
         m_log_box->AppendText("OnSocketEvent : wxSOCKET_OUTPUT\n");
         break;
    }
    case wxSOCKET_LOST:
    {
        m_log_box->AppendText(_T("OnSocketEvent(wxSOCKET_LOST)\n"));
        Sock->Destroy();
        m_start_button->Enable(TRUE);
        break;
    }
    default:
    {
        m_log_box->AppendText("OnSocketEvent : unknown event\n");
        break;
    }
    }
}

void MyFrame::DisplayScreen()
{
    MyDisplayScreenFrame *frame = new MyDisplayScreenFrame("Screen Capture", wxPoint(100, 100), wxSize(800, 600));
    frame->Centre();
    frame->Show(true);
}

BEGIN_EVENT_TABLE(MyDisplayScreenFrame, wxFrame)
EVT_PAINT(MyDisplayScreenFrame::OnPaint)
END_EVENT_TABLE()

MyDisplayScreenFrame::MyDisplayScreenFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size, wxDEFAULT_FRAME_STYLE | wxCLOSE_BOX | wxFULL_REPAINT_ON_RESIZE),
    capturedImage(1,1) 
{ 
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    timer = new wxTimer(this, wxID_ANY);
    Bind(wxEVT_TIMER, &MyDisplayScreenFrame::OnTimer, this, wxID_ANY);
    timer->Start(30);

    Bind(wxEVT_CLOSE_WINDOW, &MyDisplayScreenFrame::OnClose, this);
}

void MyDisplayScreenFrame::OnTimer(wxTimerEvent& event)
{
    if (data.IsOk()) capturedImage = data;
    Refresh();
}

void MyDisplayScreenFrame::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this); 
    dc.DrawBitmap(wxBitmap(capturedImage), 0, 0, false);
}

void MyDisplayScreenFrame::OnClose(wxCloseEvent& event) 
{
    timer->Stop();
    Destroy();
}