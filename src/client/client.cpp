#include <client.h>

wxIMPLEMENT_APP(MyClientApp);

bool MyClientApp::OnInit()
{
    MyClientFrame* myClientWindow = new MyClientFrame(wxT("Client Window"), wxDefaultPosition, wxSize(400, 400));
    myClientWindow->Show(true);
    myClientWindow->Centre();
    return true;
}

wxBEGIN_EVENT_TABLE(MyClientFrame, wxFrame)
    EVT_SOCKET(wxID_SOCKET, MyClientFrame::OnClientSocket)
wxEND_EVENT_TABLE();

MyClientFrame::MyClientFrame(const wxString &title, const wxPoint &pos, const wxSize &size, long style)
    : wxFrame(NULL, wxID_ANY, title, pos, size, style), dataScreenImage(1, 1), screenImage(1, 1)
{
    panel = new wxPanel(this, wxID_ANY);
    
    connectButton =  new wxButton(panel, wxID_CONNECT_BUTTON, wxT("Connect"), wxPoint(10, 10));
    connectButton->Bind(wxEVT_BUTTON, &MyClientFrame::OnConnectButton, this, wxID_CONNECT_BUTTON);
    
    displayButton = new wxButton(panel, wxID_DISPLAY_BUTTON, wxT("Display Screen"));
    displayButton->Bind(wxEVT_BUTTON, &MyClientFrame::OnDisplayButton, this, wxID_DISPLAY_BUTTON);

    logBox = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                            wxTE_MULTILINE | wxTE_BESTWRAP | wxTE_READONLY);
    LayoutClientScreen();

    updatingScreenTimer = new wxTimer(this, wxID_TIMER);
    Bind(wxEVT_TIMER, &MyClientFrame::OnUpdatingScreenTimer, this, wxID_TIMER);
    updatingScreenTimer->Start(16);

    Bind(wxEVT_CLOSE_WINDOW, &MyClientFrame::OnClose, this);
}

MyClientFrame::~MyClientFrame(){}

void MyClientFrame::LayoutClientScreen()
{
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(connectButton, 0, wxALL, 10);
    sizer->Add(displayButton, 0, wxALL, 10);
    sizer->Add(logBox, 1, wxEXPAND | wxALL, 10);
    panel->SetSizer(sizer);
}

void MyClientFrame::OnInputThreadDestruction()
{
    wxCriticalSectionLocker lock(iTcs);
    inputThread = nullptr;
}

void MyClientFrame::OnUpdatingScreenTimer(wxTimerEvent &e)
{
    wxCriticalSectionLocker lock(sIcs);
    dataScreenImage = screenImage;
}

void MyClientFrame::OnConnectButton(wxCommandEvent &e)
{
    connectButton->Disable();

    wxIPV4address adr;
    adr.Hostname(wxT("localhost"));
    adr.Service(3000);

    client = new wxSocketClient();

    client->SetEventHandler(*this, wxID_SOCKET);
    client->SetNotify(wxSOCKET_CONNECTION_FLAG | wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
    client->Notify(true);

    client->Connect(adr, false);

    logBox->AppendText(wxT("Connecting to the server...\n"));
}

void MyClientFrame::OnClientSocket(wxSocketEvent &e)
{
    socket = e.GetSocket();
    switch (e.GetSocketEvent())
    {
    case wxSOCKET_CONNECTION:
    {
        logBox->AppendText(wxT("Event: CONNECTION\n"));
        const char reponse = '.';
        socket->Write(&reponse, sizeof(char));
        break;
    }
    case wxSOCKET_INPUT:
    {
        logBox->AppendText(wxT("Event: INPUT\n"));

        inputThread = new InputThread(this, screenImage, sIcs, socket); 

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
        connectButton->Enable();
        break;
    }

    default:
    {
        logBox->AppendText(wxT("Event : unknown\n"));
    }
    }
}

void MyClientFrame::OnDisplayButton(wxCommandEvent &e)
{
    DisplayScreenFrame* displayScrenWindow = new DisplayScreenFrame(wxT("Display Screen Window"), wxDefaultPosition, wxSize(800, 450), dataScreenImage);
    displayScrenWindow->Show(true);
    displayScrenWindow->Centre();
}


void MyClientFrame::OnClose(wxCloseEvent &e)
{
    Destroy();
}