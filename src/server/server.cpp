#include <server/allowdiscover.h>
#include <server/screensend.h>
#include <server/eventrecv.h>
#include <server/eventsim.h>
#include <message.h>
#include <wx/wx.h>
#include <wx/socket.h>
#include <winsock.h>
#include <queue>

class MyServerApp : public wxApp
{
    bool OnInit() override;
};

class MyServerFrame : public wxFrame, public AllowDiscoverCallback, public ScreenSendCallback, public EventRecvCallback, public EventSimCallback
{
    public:
    MyServerFrame(const wxString &title, const wxPoint &pos, const wxSize &size);
    virtual ~MyServerFrame(){};

    private:
    wxTextCtrl *logBox;
    wxButton *sessionButton;
    wxTimer *capturingTimer;

    WSAData wsaData;
    AllowDiscoverThread *allowDiscoverThread = nullptr;
    wxCriticalSection allowDiscovercs;
    ScreenSendThread *screenSendThread = nullptr;
    wxCriticalSection screenSendcs;
    wxImage image;
    wxCriticalSection ics;
    EventRecvThread *eventRecvThread = nullptr;
    wxCriticalSection eventRecvcs;
    std::queue<msg> msgQueue;
    wxCriticalSection mQcs;
    EventSimThread *eventSimThread = nullptr;
    wxCriticalSection eventSimcs;
    bool isScreenConnect = false;
    bool isEventConnect = false;

    void OnPowerSessionButton(wxCommandEvent &e);
    void OnCapturingTimer(wxTimerEvent &e);
    void OnClose(wxCloseEvent &e);

    void OnAllowDiscoverThreadDestruction() override;
    void OnAllowDiscoverComplete(wxThreadEvent &e);
    void OnScreenSendThreadDestruction() override;
    void OnScreenSendUpdate(wxThreadEvent &e);
    void OnScreenSendComplete(wxThreadEvent &e);
    void OnEventRecvThreadDestruction() override;
    void OnEventRecvUpdate(wxThreadEvent &e);
    void OnEventRecvComplete(wxThreadEvent &e);
    void OnEventSimThreadDestruction() override;
};

wxIMPLEMENT_APP(MyServerApp);

bool MyServerApp::OnInit()
{
    MyServerFrame *myServerFrame = new MyServerFrame("Server App", wxDefaultPosition, wxSize(300, 400));
    myServerFrame->Show(true);
    myServerFrame->Centre();
    return true;
}

MyServerFrame::MyServerFrame(const wxString &title, const wxPoint &pos, const wxSize &size)
                            : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    
    wxIPV4address addr;
    addr.Hostname(wxGetHostName());
    wxString ipAddr = addr.IPAddress();
    
    wxPanel *panel = new wxPanel(this);
    panel->SetBackgroundColour(wxColor(100, 100, 100));
    
    wxStaticText *serverip = new wxStaticText(panel, wxID_ANY, "Your IP address: " + ipAddr);
    wxFont font = serverip->GetFont();
    font.SetFamily(wxFONTFAMILY_ROMAN);
    font.MakeItalic();
    font.Scale(1.2);
    serverip->SetFont(font);

    sessionButton = new wxButton(panel, wxID_ANY, "Begin session", wxDefaultPosition, wxSize(100, 25));
    sessionButton->Bind(wxEVT_BUTTON, &MyServerFrame::OnPowerSessionButton, this);

    logBox = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_BESTWRAP);

    wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(serverip, 0, wxLEFT | wxTOP | wxRIGHT, 5);
    sizer->Add(sessionButton, 0, wxALIGN_RIGHT | wxLEFT | wxTOP | wxRIGHT, 5);
    sizer->Add(logBox, 1, wxEXPAND | wxALL, 5);
    panel->SetSizer(sizer);

    logBox->AppendText(wxT("Welcome to Server App\nClick Begin session button to allow other computers to connect to your computer\n"));

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        logBox->AppendText(wxT("Error: Failed to initialize Winsock\n"));
    }

    allowDiscoverThread = new AllowDiscoverThread(this, this);
    if (allowDiscoverThread->Run() != wxTHREAD_NO_ERROR)
    {
        logBox->AppendText(wxT("Error: Failed to run allowing discover thread\n"));
        delete allowDiscoverThread;
        allowDiscoverThread = nullptr;
        return;
    }

    capturingTimer = new wxTimer(this, wxID_ANY);
    Bind(wxEVT_TIMER, &MyServerFrame::OnCapturingTimer, this);

    Bind(wxEVT_CLOSE_WINDOW, MyServerFrame::OnClose, this);

    Bind(wxEVT_ALLOWDISCOVERTHREAD_COMPLETE, &MyServerFrame::OnAllowDiscoverComplete, this);
    Bind(wxEVT_SCREENSENDTHREAD_UPDATE, &MyServerFrame::OnScreenSendUpdate, this);
    Bind(wxEVT_SCREENSENDTHREAD_COMPLETE, &MyServerFrame::OnScreenSendComplete, this);
    Bind(wxEVT_EVENTRECVTHREAD_UPDATE, &MyServerFrame::OnEventRecvUpdate, this);
    Bind(wxEVT_EVENTRECVTHREAD_COMPLETE, &MyServerFrame::OnEventRecvComplete, this);
}

void MyServerFrame::OnPowerSessionButton(wxCommandEvent &e)
{
    wxString label = sessionButton->GetLabel();
    if (label.IsSameAs("Begin session"))
    {
        logBox->AppendText(wxT("Waiting for connection...\n"));
        
        capturingTimer->Start();

        screenSendThread = new ScreenSendThread(this, this, image, ics);
        if (screenSendThread->Run() != wxTHREAD_NO_ERROR)
        {
            logBox->AppendText(wxT("Error: Failed to run sending screen image thread\n"));
            delete screenSendThread;
            screenSendThread = nullptr;
            return;
        }

        eventRecvThread = new EventRecvThread(this, this, msgQueue, mQcs);
        if (eventRecvThread->Run() != wxTHREAD_NO_ERROR)
        {
            logBox->AppendText(wxT("Error: Failed to run receiving event thread\n"));
            delete eventRecvThread;
            eventRecvThread = nullptr;
            return;
        }

        eventSimThread = new EventSimThread(this, msgQueue, mQcs);
        if (eventSimThread->Run() != wxTHREAD_NO_ERROR)
        {
            logBox->AppendText(wxT("Error: Failed to run simulating event thread\n"));
            delete eventSimThread;
            eventSimThread = nullptr;
            return;
        }

        sessionButton->Disable();
    }
    if (label.IsSameAs("End session"))
    {
        capturingTimer->Stop();
        
        if (screenSendThread)
        {
            wxCriticalSectionLocker lock(screenSendcs);
            screenSendThread->Delete();
            screenSendThread = nullptr;
        }

        if (eventRecvThread)
        {
            wxCriticalSectionLocker lock(eventRecvcs);
            eventRecvThread->Delete();
            eventRecvThread = nullptr;
        }

        if (eventSimThread)
        {
            wxCriticalSectionLocker lock(eventSimcs);
            eventSimThread->Delete();
            eventSimThread = nullptr;
        }

        logBox->AppendText(wxT("Disconnect successfully"));

        sessionButton->SetLabel("Begin session");
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

    wxCriticalSectionLocker lock(ics);
    image = bitmap.ConvertToImage();
}

void MyServerFrame::OnClose(wxCloseEvent &e)
{
   capturingTimer->Stop();
   WSACleanup();
   Destroy();
}

void MyServerFrame::OnAllowDiscoverThreadDestruction()
{
    wxCriticalSectionLocker lock(allowDiscovercs);
    allowDiscoverThread = nullptr;
}

void MyServerFrame::OnAllowDiscoverComplete(wxThreadEvent &e)
{
    wxString message = e.GetString();
    logBox->AppendText(message);
}

void MyServerFrame::OnScreenSendThreadDestruction()
{
    wxCriticalSectionLocker lock(screenSendcs);
    screenSendThread = nullptr;
}

void MyServerFrame::OnScreenSendUpdate(wxThreadEvent &e)
{
    isScreenConnect = true;

    logBox->AppendText(wxT("Connect successfully\n"));

    if (isEventConnect)
    {
        sessionButton->Enable();
        sessionButton->SetLabel("End session");
    }
}

void MyServerFrame::OnScreenSendComplete(wxThreadEvent &e)
{
    wxString message = e.GetString();
    logBox->AppendText(message);

    sessionButton->SetLabel("Begin session");

    capturingTimer->Stop();

    if (eventSimThread)
    {
        wxCriticalSectionLocker lock(eventSimcs);
        eventSimThread->Delete();
        eventSimThread = nullptr;
    }
}

void MyServerFrame::OnEventRecvThreadDestruction()
{
    wxCriticalSectionLocker lock(eventRecvcs);
    eventRecvThread = nullptr;
}

void MyServerFrame::OnEventRecvComplete(wxThreadEvent &e)
{
    wxString message = e.GetString();
    logBox->AppendText(message);

    sessionButton->SetLabel("Begin session");

    capturingTimer->Stop();

    if (eventSimThread)
    {
        wxCriticalSectionLocker lock(eventSimcs);
        eventSimThread->Delete();
        eventSimThread = nullptr;
    }
}

void MyServerFrame::OnEventRecvUpdate(wxThreadEvent &e)
{
    logBox->AppendText(wxT("Connect successfully\n"));
    
    isEventConnect = true;

    if (isScreenConnect) 
    {
        sessionButton->Enable();
        sessionButton->SetLabel("End session");
    }
}

void MyServerFrame::OnEventSimThreadDestruction()
{
    wxCriticalSectionLocker lock(eventSimcs);
    eventSimThread = nullptr;
}