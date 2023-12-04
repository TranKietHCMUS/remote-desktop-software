#include <server.h>

wxIMPLEMENT_APP(MyServerApp);

bool MyServerApp::OnInit()
{
    MyServerFrame *myServerWindow = new MyServerFrame(wxT("Server Window"), wxDefaultPosition, wxSize(400, 400));
    myServerWindow->Show(true);
    myServerWindow->Centre();
    return true;
}

MyServerFrame::MyServerFrame(const wxString &title, const wxPoint &pos, const wxSize &size, long style)
    : wxFrame(NULL, wxID_ANY, title, pos, size, style), capturedImage(1600, 900)
{
    panel = new wxPanel(this, wxID_ANY);

    allowButton = new wxButton(panel, wxID_BUTTON, wxT("Allow Connection"), wxPoint(10, 10));
    allowButton->Bind(wxEVT_BUTTON, &MyServerFrame::OnClickAllowButton, this, wxID_BUTTON);

    logBox = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                            wxTE_MULTILINE | wxTE_BESTWRAP | wxTE_READONLY);

    LayoutServerScreen();

    capturingTimer = new wxTimer(this, wxID_TIMER);
    Bind(wxEVT_TIMER, &MyServerFrame::OnCapturingTimer, this, wxID_TIMER);
    capturingTimer->Start(16);

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        std::cerr << "Failed to initialize Winsock." << "\n";

    Bind(wxEVT_CLOSE_WINDOW, &MyServerFrame::OnClose, this);
}

MyServerFrame::~MyServerFrame() {}

void MyServerFrame::LayoutServerScreen()
{
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(allowButton, 0, wxALL, 10);
    sizer->Add(logBox, 1, wxEXPAND | wxALL, 10);
    panel->SetSizer(sizer);
}

void MyServerFrame::OnScreenSocketThreadDestruction()
{
    screenSocketThread = nullptr;
}

void MyServerFrame::OnEventSocketThreadDestruction()
{
    eventSocketThread = nullptr;
}

void MyServerFrame::OnEventThreadDestruction()
{
    eventThread = nullptr;
}

void MyServerFrame::OnClickAllowButton(wxCommandEvent &e)
{
    allowButton->Disable();

    screenSocketThread = new ScreenSocketThread(this, capturedImage, cIcs);
    if (screenSocketThread->Run() != wxTHREAD_NO_ERROR)
    {
        logBox->AppendText(wxT("Failed to create ScreenThread!\n"));
        delete screenSocketThread;
    }
    else
    {
        logBox->AppendText(wxT("Created ScreenThread!\n"));
    }

    eventSocketThread = new EventSocketThread(this, msgQueue, mQcs);
    if (eventSocketThread->Run() != wxTHREAD_NO_ERROR)
    {
        logBox->AppendText(wxT("Failed to create EventThread!\n"));
        delete eventSocketThread;
    }
    else
    {
        logBox->AppendText(wxT("Created EventThread!\n"));
    }

    eventThread = new EventThread(this, msgQueue, mQcs);
    if (eventThread->Run() != wxTHREAD_NO_ERROR)
    {
        logBox->AppendText(wxT("Failed to create EThread!\n"));
        delete eventThread;
    }
    else
    {
        logBox->AppendText(wxT("Created EThread!\n"));
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
    //capturedImage = capturedImage.Rescale(1280, 720, wxIMAGE_QUALITY_HIGH);
}

void MyServerFrame::OnClose(wxCloseEvent &e)
{
    capturingTimer->Stop();
    WSACleanup();
    Destroy();
}