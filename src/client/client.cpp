#include <client.h>

wxIMPLEMENT_APP(MyClientApp);

bool MyClientApp::OnInit()
{
    MyClientFrame *myClientWindow = new MyClientFrame(wxT("Client Window"), wxDefaultPosition, wxSize(400, 400));
    myClientWindow->Show(true);
    myClientWindow->Centre();
    return true;
}

MyClientFrame::MyClientFrame(const wxString &title, const wxPoint &pos, const wxSize &size, long style)
    : wxFrame(NULL, wxID_ANY, title, pos, size, style), screenImage(1280, 720)
{
    stop = false;
    panel = new wxPanel(this, wxID_ANY);

    connectButton = new wxButton(panel, wxID_CONNECT_BUTTON, wxT("Connect"), wxPoint(10, 10));
    connectButton->Bind(wxEVT_BUTTON, &MyClientFrame::OnConnectButton, this, wxID_CONNECT_BUTTON);

    displayButton = new wxButton(panel, wxID_DISPLAY_BUTTON, wxT("Display Screen"));
    displayButton->Bind(wxEVT_BUTTON, &MyClientFrame::OnDisplayButton, this, wxID_DISPLAY_BUTTON);

    logBox = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                            wxTE_MULTILINE | wxTE_BESTWRAP | wxTE_READONLY);
    
    LayoutClientScreen();

    displayScreenWindow = new DisplayScreenFrame(wxT("Display Screen Window"), wxDefaultPosition, wxSize(1297, 760), screenImage, sIcs);

    Bind(wxEVT_CLOSE_WINDOW, &MyClientFrame::OnClose, this);
}

MyClientFrame::~MyClientFrame() {}

void MyClientFrame::LayoutClientScreen()
{
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(connectButton, 0, wxALL, 10);
    sizer->Add(displayButton, 0, wxALL, 10);
    sizer->Add(logBox, 1, wxEXPAND | wxALL, 10);
    panel->SetSizer(sizer);
}

void MyClientFrame::OnSocketThreadDestruction()
{
    connectButton->Enable();
    socketThread = nullptr;
}

void MyClientFrame::OnConnectButton(wxCommandEvent &e)
{
    connectButton->Disable();
    socketThread = new SocketThread(this, screenImage, sIcs);
    if (socketThread->Run() != wxTHREAD_NO_ERROR)
    {
        logBox->AppendText(wxT("Failed to create InputThread!\n"));
        delete socketThread;
    }
    else
    {
        logBox->AppendText(wxT("Created InputThread!\n"));
    }
}

void MyClientFrame::OnDisplayButton(wxCommandEvent &e)
{
    
    displayScreenWindow->Show(true);
    displayScreenWindow->Centre();
}

void MyClientFrame::OnClose(wxCloseEvent& e)
{
    Destroy();
}
