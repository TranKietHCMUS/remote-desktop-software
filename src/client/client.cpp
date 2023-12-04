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
    panel = new wxPanel(this, wxID_ANY);

    connectButton = new wxButton(panel, wxID_CONNECT_BUTTON, wxT("Connect"), wxPoint(10, 10));
    connectButton->Bind(wxEVT_BUTTON, &MyClientFrame::OnConnectButton, this, wxID_CONNECT_BUTTON);

    displayButton = new wxButton(panel, wxID_DISPLAY_BUTTON, wxT("Display Screen"));
    displayButton->Bind(wxEVT_BUTTON, &MyClientFrame::OnDisplayButton, this, wxID_DISPLAY_BUTTON);

    inputBox = new wxTextCtrl(panel, 1, wxEmptyString, wxDefaultPosition, wxSize(200, -1),
                            wxTE_PROCESS_ENTER | wxTE_BESTWRAP);
    Bind(wxEVT_TEXT_ENTER, &MyClientFrame::OnTextEnter, this, 1);

    logBox = new wxTextCtrl(panel, 2, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                            wxTE_MULTILINE | wxTE_BESTWRAP | wxTE_READONLY);
    
    LayoutClientScreen();

    ip = wxEmptyString;
    
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        std::cerr << "Failed to initialize Winsock." << "\n";

    quitScreen = false;
    quitEvent = false;

    Bind(wxEVT_CLOSE_WINDOW, &MyClientFrame::OnClose, this);
}

MyClientFrame::~MyClientFrame() {}

void MyClientFrame::LayoutClientScreen()
{
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(connectButton, 0, wxALL, 10);
    sizer->Add(displayButton, 0, wxALL, 10);
    sizer->Add(inputBox, 0, wxALL, 10);
    sizer->Add(logBox, 1, wxEXPAND | wxALL, 10);
    panel->SetSizer(sizer);
}

void MyClientFrame::OnScreenSocketThreadDestruction()
{
    quitScreen = true;
    if (quitScreen && quitEvent)
        connectButton->Enable();
    screenSocketThread = nullptr;
}

void MyClientFrame::OnEventSocketThreadDestruction()
{
    quitEvent = true;
    if (quitScreen && quitEvent)
        connectButton->Enable();
    eventSocketThread = nullptr;
}

void MyClientFrame::OnConnectButton(wxCommandEvent &e)
{
    connectButton->Disable();
    
    screenSocketThread = new ScreenSocketThread(this, ip, screenImage, sIcs);
    if (screenSocketThread->Run() != wxTHREAD_NO_ERROR)
        delete screenSocketThread;

    eventSocketThread = new EventSocketThread(this, ip, msgQueue, mQcs);
    if (eventSocketThread->Run() != wxTHREAD_NO_ERROR)
        delete eventSocketThread;
}

void MyClientFrame::OnDisplayButton(wxCommandEvent &e)
{
    displayButton->Disable();
    
    displayScreenWindow = new DisplayScreenFrame(wxT("Display Screen Window"), wxPoint(100, 100), wxSize(1296, 759), screenImage, sIcs, msgQueue, mQcs);
    displayScreenWindow->Show(true);

    SetPosition(wxPoint(1496, 100));
    SetSize(wxSize(400, 759));

    displayScreenWindow->Bind(wxEVT_CLOSE_WINDOW, &MyClientFrame::OnScreenClose, this);
}

void MyClientFrame::OnTextEnter(wxCommandEvent& e)
{
    ip = e.GetString();
    inputBox->SetValue(wxEmptyString);
    std::cout << ip.mb_str();
}

void MyClientFrame::OnScreenClose(wxCloseEvent& e)
{
    displayScreenWindow->Unbind(wxEVT_CLOSE_WINDOW, &MyClientFrame::OnScreenClose, this);
    
    displayButton->Enable();
    SetSize(wxSize(400, 400));
    Centre();

    e.Skip();
}

void MyClientFrame::OnClose(wxCloseEvent& e)
{   
    WSACleanup();
    Destroy();
}
