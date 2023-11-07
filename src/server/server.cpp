#include <server.h>

wxIMPLEMENT_APP(MyServerApp);

bool MyServerApp::OnInit()
{
    MyServerFrame* myServerWindow = new MyServerFrame(wxT("Server Window"), wxDefaultPosition, wxSize(400, 400));
    myServerWindow->Show(true);
    myServerWindow->Centre();
    return true;
}

wxBEGIN_EVENT_TABLE(MyServerFrame, wxFrame)
    EVT_SOCKET(wxID_SERVER, MyServerFrame::OnServerConnection)
    EVT_SOCKET(wxID_SOCKET, MyServerFrame::OnServerSocket)
wxEND_EVENT_TABLE()

MyServerFrame::MyServerFrame(const wxString &title, const wxPoint &pos, const wxSize &size, long style)
    : wxFrame(NULL, wxID_ANY, title, pos, size, style), capturedImage(1,1)
{
    panel = new wxPanel(this, wxID_ANY);

    allowButton =  new wxButton(panel, wxID_BUTTON, wxT("Allow Connection"), wxPoint(10, 10));
    allowButton->Bind(wxEVT_BUTTON, &MyServerFrame::OnClickAllowButton, this, wxID_BUTTON);

    logBox = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                            wxTE_MULTILINE | wxTE_BESTWRAP | wxTE_READONLY);

    capturingTimer = new wxTimer(this, wxID_TIMER);
    Bind(wxEVT_TIMER, &MyServerFrame::OnCapturingTimer, this, wxID_TIMER);
    capturingTimer->Start(16);

    LayoutServerScreen();

    Bind(wxEVT_CLOSE_WINDOW, &MyServerFrame::OnClose, this);
}

MyServerFrame::~MyServerFrame(){}

void MyServerFrame::LayoutServerScreen()
{
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(allowButton, 0, wxALL, 10);
    sizer->Add(logBox, 1, wxEXPAND | wxALL, 10);
    panel->SetSizer(sizer);
}

void MyServerFrame::OnInputThreadDestruction()
{
    wxCriticalSectionLocker lock(iTcs);
    inputThread = nullptr;
}

void MyServerFrame::OnClickAllowButton(wxCommandEvent &e)
{
    allowButton->Disable();

    wxIPV4address adr;
    adr.Service(3000);

    server = new wxSocketServer(adr);

    server->SetEventHandler(*this, wxID_SERVER);
    server->SetNotify(wxSOCKET_CONNECTION_FLAG);
    server->Notify(true);

    if (!server->Ok())
    {
        logBox->AppendText(wxT("Could not start server!\n"));
        allowButton->Enable();
    }
    else
    {
        logBox->AppendText(wxT("Server start!\n"));
    }
}

void MyServerFrame::OnServerConnection(wxSocketEvent &e)
{
    switch(e.GetSocketEvent())
    {
    case wxSOCKET_CONNECTION:
    {
        logBox->AppendText(wxT("Event: CONNECTION\n"));

        wxSocketBase* initialSocket = server->Accept(false);

        if (!initialSocket)
        {
            logBox->AppendText(wxT("Failed to accept incoming connection!\n"));
            return;
        }

        logBox->AppendText(wxT("Accepted Connection!\n"));
        initialSocket->SetEventHandler(*this, wxID_SOCKET);
        initialSocket->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
        initialSocket->Notify(true);

        break;
    }
    default:
    {
        logBox->AppendText(wxT("Event: unknown\n"));
    }
    }
}

void MyServerFrame::OnServerSocket(wxSocketEvent &e)
{
    socket = e.GetSocket();
    switch(e.GetSocketEvent())
    {
    case wxSOCKET_INPUT:
    {
        logBox->AppendText(wxT("Event: INPUT\n"));

        inputThread = new InputThread(this, capturedImage, cIcs, socket);

        if (inputThread->Run() !=  wxTHREAD_NO_ERROR)
        {
            logBox->AppendText(wxT("Failed to create InputThread!\n"));
            delete inputThread;
        }
        else
        {
            logBox->AppendText(wxT("Created InputThread!\n"));
        }

        break;
    }
    case wxSOCKET_LOST:
    {
        logBox->AppendText(wxT("Event: LOST\n"));
        socket->Destroy();
        break;
    }
    default:
    {
        logBox->AppendText(wxT("Event: unknown\n"));
    }
    }
}

void MyServerFrame::OnCapturingTimer(wxTimerEvent &e)
{
    wxScreenDC screenDC;
    wxMemoryDC memDC;

    int screenWidth = screenDC.GetSize().GetWidth();
    int screenHeight = screenDC.GetSize().GetHeight();

    wxBitmap bitmap(screenWidth, screenHeight);
    memDC.SelectObject(bitmap);
    memDC.Blit(0, 0, screenWidth, screenHeight, &screenDC, 0, 0);

    wxCriticalSectionLocker lock(cIcs);
    capturedImage = bitmap.ConvertToImage();
}

void MyServerFrame::OnClose(wxCloseEvent &e)
{
    capturingTimer->Stop();
    Destroy();
}