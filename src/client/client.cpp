#include <client/displayscreen.h>
#include <client/discoverserver.h>
#include <client/screenrecv.h>
#include <client/eventsend.h>
#include <message.h>
#include <wx/wx.h>
#include <wx/splitter.h>
#include <winsock.h>
#include <vector>
#include <queue>

class MyClientApp : public wxApp
{
    bool OnInit() override;
};

class MyClientFrame : public wxFrame, public DiscoverServerCallback, public ScreenRecvCallback, public EventSendCallback
{
    public:
    MyClientFrame(const wxString &title, const wxPoint &pos, const wxSize &size);
    virtual ~MyClientFrame(){};

    private:
    wxSplitterWindow *splitter;
    wxScrolled<wxPanel> *choosePanel;
    wxSizer* chooseSizer;
    wxPanel *inputPanel;
    wxTextCtrl *inputBox;
    wxButton *discoverButton;
    wxButton *showScreenButton;
    wxTextCtrl *logBox;
    wxPanel *displayPanel;
    DisplayWindow *displayWindow;
    std::vector<wxString> ipList;

    WSAData wsaData;
    DiscoverServerThread *discoverServerThread = nullptr;
    wxCriticalSection discoverServercs;
    ScreenRecvThread *screenRecvThread = nullptr;
    wxCriticalSection screenRecvcs;
    wxBitmap bitmap;
    wxCriticalSection bcs;
    EventSendThread *eventSendThread = nullptr;
    wxCriticalSection eventSendcs;
    std::queue<msg> msgQueue;
    wxCriticalSection mQcs;
    bool isConnect = false;

    void OnTextEnter(wxCommandEvent &e);
    void OnAddButton(wxCommandEvent &e);
    void OnDiscoverButton(wxCommandEvent &e);
    void OnShowScreenButton(wxCommandEvent &e);
    void OnShowLogButton(wxCommandEvent &e);
    void OnConnectButton(wxCommandEvent &e);
    void OnDeleteButton(wxCommandEvent &e);
    void OnDisconnectButton(wxCommandEvent &e);
    void OnClose(wxCloseEvent &e);

    void OnDiscoverServerThreadDestruction() override;
    void OnDiscoverServerThreadUpdate(wxThreadEvent &e);
    void OnDiscoverServerComplete(wxThreadEvent &e);
    void OnScreenRecvThreadDestruction() override;
    void OnScreenRecvUpdate(wxThreadEvent &e);
    void OnScreenRecvComplete(wxThreadEvent &e);
    void OnEventSendThreadDestruction() override;
    void OnEventSendUpdate(wxThreadEvent &e);
    void OnEventSendComplete(wxThreadEvent &e);
};

wxIMPLEMENT_APP(MyClientApp);

bool MyClientApp::OnInit()
{
    MyClientFrame *myClientFrame = new MyClientFrame("Client App", wxDefaultPosition, wxSize(900, 500));
    myClientFrame->Show(true);
    myClientFrame->Centre();
    return true;
}

MyClientFrame::MyClientFrame(const wxString &title, const wxPoint &pos, const wxSize &size)
                            : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);

    choosePanel= new wxScrolled<wxPanel>(splitter);
    choosePanel->SetBackgroundColour(wxColor(100, 100, 100));
    choosePanel->SetScrollRate(5, 5);

    chooseSizer = new wxBoxSizer(wxVERTICAL);
    choosePanel->SetSizer(chooseSizer);

    inputPanel = new wxPanel(splitter);
    inputPanel->SetBackgroundColour(wxColor(100, 100, 100));

    wxStaticText *inputText = new wxStaticText(inputPanel, wxID_ANY, "Enter remote server's IP address:");
    wxFont textFont = inputText->GetFont();
    textFont.SetFamily(wxFONTFAMILY_ROMAN);
    textFont.MakeItalic();
    textFont.Scale(1.2);
    inputText->SetFont(textFont);

    inputBox = new wxTextCtrl(inputPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(150, 25), wxTE_PROCESS_ENTER);
    wxFont inputFont = inputBox->GetFont();
    inputFont.SetFamily(wxFONTFAMILY_ROMAN);
    inputFont.MakeBold();
    inputFont.Scale(1.2);
    inputBox->SetFont(inputFont);
    inputBox->Bind(wxEVT_TEXT_ENTER, &MyClientFrame::OnTextEnter, this);

    wxButton *addButton = new wxButton(inputPanel, wxID_ANY, "Add", wxDefaultPosition, wxSize(100, 26));
    addButton->Bind(wxEVT_BUTTON, &MyClientFrame::OnAddButton, this);
    
    wxStaticText *orText = new wxStaticText(inputPanel, wxID_ANY, "Or");
    wxFont orFont = orText->GetFont();
    orFont.SetFamily(wxFONTFAMILY_ROMAN);
    orFont.MakeItalic();
    orText->SetFont(orFont);

    discoverButton = new wxButton(inputPanel, wxID_ANY, "Discover", wxDefaultPosition, wxSize(100, 26));
    discoverButton->Bind(wxEVT_BUTTON, &MyClientFrame::OnDiscoverButton, this);

    wxSizer *inputChildSizer = new wxBoxSizer(wxHORIZONTAL);
    inputChildSizer->Add(inputBox, 1, wxLEFT | wxTOP, 5);
    inputChildSizer->Add(addButton, 0, wxLEFT | wxTOP, 5);
    inputChildSizer->Add(orText, 0, wxALIGN_CENTER | wxLEFT | wxTOP, 5);
    inputChildSizer->Add(discoverButton, 0, wxLEFT | wxTOP | wxRIGHT, 5);

    wxStaticText *inputNote = new wxStaticText(inputPanel, wxID_ANY, "Press Enter or click Add button to add a new remote server");
    wxFont noteFont = inputNote->GetFont();
    noteFont.SetFamily(wxFONTFAMILY_ROMAN);
    noteFont.MakeItalic();
    inputNote->SetFont(noteFont);

    showScreenButton = new wxButton(inputPanel, wxID_ANY, "Show screen", wxDefaultPosition, wxSize(100, 25));
    showScreenButton->Disable();
    showScreenButton->Bind(wxEVT_BUTTON, &MyClientFrame::OnShowScreenButton, this);

    logBox = new wxTextCtrl(inputPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_BESTWRAP);

    wxSizer *inputSizer = new wxBoxSizer(wxVERTICAL);
    inputSizer->Add(inputText, 0, wxLEFT | wxTOP | wxRIGHT, 5);
    inputSizer->Add(inputChildSizer, 0);
    inputSizer->Add(inputNote, 0, wxLEFT | wxTOP | wxRIGHT, 5);
    inputSizer->Add(showScreenButton, 0, wxALIGN_RIGHT | wxLEFT | wxTOP | wxRIGHT, 5);
    inputSizer->Add(logBox, 1, wxEXPAND | wxALL, 5);
    inputPanel->SetSizer(inputSizer);

    displayPanel = new wxPanel(splitter);
    displayPanel->SetBackgroundColour(wxColor(100, 100, 100));

    wxButton *showLogButton = new wxButton(displayPanel, wxID_ANY, "Show message log", wxDefaultPosition, wxSize(150, 25));
    showLogButton->Bind(wxEVT_BUTTON, &MyClientFrame::OnShowLogButton, this);

    displayWindow = new DisplayWindow(displayPanel, msgQueue, mQcs);
    
    wxSizer *displaySizer = new wxBoxSizer(wxVERTICAL);
    displaySizer->Add(showLogButton, 0, wxALIGN_RIGHT | wxLEFT | wxTOP | wxRIGHT, 5);
    displaySizer->Add(displayWindow, 1, wxEXPAND | wxALL, 5);
    displayPanel->SetSizer(displaySizer);
    displayPanel->Show(false);

    splitter->SetMinimumPaneSize(300);
    splitter->SplitVertically(choosePanel, inputPanel);
    splitter->SetSashPosition(300);
    splitter->UpdateSize();
    
    Bind(wxEVT_CLOSE_WINDOW, &MyClientFrame::OnClose, this);

    logBox->AppendText(wxT("Welcome to Client App\n"));

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        logBox->AppendText(wxT("Error: Failed to initialize Winsock\n"));
    }

    Bind(wxEVT_DISCOVERSERVERTHREAD_UPDATE, &MyClientFrame::OnDiscoverServerThreadUpdate, this);
    Bind(wxEVT_DISCOVERSERVERTHREAD_COMPLETE, &MyClientFrame::OnDiscoverServerComplete, this);
    Bind(wxEVT_SCREENRECVTHREAD_UPDATE, &MyClientFrame::OnScreenRecvUpdate, this);
    Bind(wxEVT_SCREENRECVTHREAD_COMPLETE, &MyClientFrame::OnScreenRecvComplete, this);
    Bind(wxEVT_EVENTSENDTHREAD_UPDATE, &MyClientFrame::OnEventSendUpdate, this);
    Bind(wxEVT_EVENTSENDTHREAD_COMPLETE, &MyClientFrame::OnEventSendComplete, this);
}

void MyClientFrame::OnTextEnter(wxCommandEvent &e)
{
    wxString ip = e.GetString();
    inputBox->SetValue(wxEmptyString);
    if (ip.IsEmpty()) 
    {
        return;
    }
    for (auto s : ipList)
        if (ip.IsSameAs(s)) 
        {
            return;
        }
    ipList.push_back(ip);  
    
    wxPanel *panel = new wxPanel(choosePanel, ipList.size() - 1);
    panel->SetBackgroundColour(wxColor(150, 150, 150));

    wxStaticText *serverip = new wxStaticText(panel, wxID_ANY, wxT("Server's IP: ") + ip);
    wxFont font = serverip->GetFont();
    font.SetFamily(wxFONTFAMILY_ROMAN);
    font.Scale(1.2);
    serverip->SetFont(font);

    wxButton *connectButton = new wxButton(panel, ipList.size() - 1, "Connect", wxDefaultPosition, wxSize(100, 25));
    connectButton->Bind(wxEVT_BUTTON, &MyClientFrame::OnConnectButton, this);

    wxButton *deleteButton = new wxButton(panel, ipList.size() - 1, "Delete", wxDefaultPosition, wxSize(100, 25));
    deleteButton->Bind(wxEVT_BUTTON, &MyClientFrame::OnDeleteButton, this);

    wxButton *disconnectButton = new wxButton(panel, ipList.size() - 1, "Disconnect", wxDefaultPosition, wxSize(100, 25));
    disconnectButton->Bind(wxEVT_BUTTON, &MyClientFrame::OnDisconnectButton, this);

    if (isConnect)
    {
        connectButton->Disable();
        deleteButton->Disable();
    }

    wxSizer *childSizer = new wxBoxSizer(wxVERTICAL);
    childSizer->Add(connectButton, 0, wxALIGN_RIGHT | wxLEFT | wxTOP | wxRIGHT, 10);
    childSizer->Add(deleteButton, 0, wxALIGN_RIGHT | wxALL, 10);
    childSizer->Add(disconnectButton, 0, wxALIGN_RIGHT | wxALL, 10);
    childSizer->Hide(disconnectButton);
    
    wxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(serverip, 1, wxALIGN_CENTER_VERTICAL | wxBOTTOM | wxLEFT | wxTOP, 10);
    sizer->Add(childSizer, 0, wxALIGN_CENTER_VERTICAL);
    
    panel->SetSizerAndFit(sizer);

    chooseSizer->Prepend(panel, 0, wxEXPAND | wxLEFT | wxTOP | wxRIGHT, 10);
    chooseSizer->Layout();

    choosePanel->FitInside();
    choosePanel->Update();
}

void MyClientFrame::OnAddButton(wxCommandEvent &e)
{
    wxString ip = inputBox->GetValue();
    inputBox->SetValue(wxEmptyString);
    if (ip.IsEmpty()) 
    {
        return;
    }
    for (auto s : ipList)
        if (ip.IsSameAs(s)) 
        {
            return;
        }
    ipList.push_back(ip);  
    
    wxPanel *panel = new wxPanel(choosePanel, ipList.size() - 1);
    panel->SetBackgroundColour(wxColor(150, 150, 150));

    wxStaticText *serverIp = new wxStaticText(panel, wxID_ANY, wxT("Server's IP: ") + ip);

    wxButton *connectButton = new wxButton(panel, ipList.size() - 1, "Connect", wxDefaultPosition, wxSize(100, 25));
    connectButton->Bind(wxEVT_BUTTON, &MyClientFrame::OnConnectButton, this);

    wxButton *deleteButton = new wxButton(panel, ipList.size() - 1, "Delete", wxDefaultPosition, wxSize(100, 25));
    deleteButton->Bind(wxEVT_BUTTON, &MyClientFrame::OnDeleteButton, this);

    wxButton *disconnectButton = new wxButton(panel, ipList.size() - 1, "Disconnect", wxDefaultPosition, wxSize(100, 25));
    disconnectButton->Bind(wxEVT_BUTTON, &MyClientFrame::OnDisconnectButton, this);

    if (isConnect)
    {
        connectButton->Disable();
        deleteButton->Disable();
    }

    wxSizer *childSizer = new wxBoxSizer(wxVERTICAL);
    childSizer->Add(connectButton, 0, wxALIGN_RIGHT | wxLEFT | wxTOP | wxRIGHT, 10);
    childSizer->Add(deleteButton, 0, wxALIGN_RIGHT | wxALL, 10);
    childSizer->Add(disconnectButton, 0, wxALIGN_RIGHT | wxALL, 10);
    childSizer->Hide(disconnectButton);
    
    wxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(serverIp, 1, wxALIGN_CENTER_VERTICAL | wxBOTTOM | wxLEFT | wxTOP, 10);
    sizer->Add(childSizer, 0, wxALIGN_CENTER_VERTICAL);
    
    panel->SetSizerAndFit(sizer);

    chooseSizer->Prepend(panel, 0, wxEXPAND | wxLEFT | wxTOP | wxRIGHT, 10);
    chooseSizer->Layout();

    choosePanel->FitInside();
    choosePanel->Update();
}

void MyClientFrame::OnDiscoverButton(wxCommandEvent &e)
{
    logBox->AppendText(wxT("Start discovering servers...\n"));
    
    discoverServerThread = new DiscoverServerThread(this, this);
    if (discoverServerThread->Run() != wxTHREAD_NO_ERROR)
    {
        logBox->AppendText(wxT("Error: Failed to run discovering servers thread\n"));
        delete discoverServerThread;
        discoverServerThread = nullptr;
        return;
    }

    discoverButton->Disable();
}

void MyClientFrame::OnShowScreenButton(wxCommandEvent &e)
{
    if(!IsMaximized())
    {
        SetSize(1610, 799);
        Centre();
    }

    splitter->Unsplit();
    splitter->SplitVertically(choosePanel, displayPanel);
    splitter->SetSashPosition(300);
    splitter->UpdateSize();
}

void MyClientFrame::OnShowLogButton(wxCommandEvent &e)
{
    if(!IsMaximized())
    {
        SetSize(900, 500);
        Centre();
    }

    splitter->Unsplit();
    splitter->SplitVertically(choosePanel, inputPanel);
    splitter->SetSashPosition(300);
    splitter->UpdateSize();
}

void MyClientFrame::OnConnectButton(wxCommandEvent &e)
{
    int id = e.GetId();

    logBox->AppendText(wxT("Connecting to server...\n"));
    isConnect = true;

    screenRecvThread = new ScreenRecvThread(this, this, id, ipList[id], bitmap, bcs);
    if (screenRecvThread->Run() != wxTHREAD_NO_ERROR)
    {
        logBox->AppendText(wxT("Error: Failed to run receiving screen image thread\n"));
        screenRecvThread->Delete();
        screenRecvThread = nullptr;
        return;
    }

    eventSendThread = new EventSendThread(this, this, id, ipList[id], msgQueue, mQcs);
    if (eventSendThread->Run() != wxTHREAD_NO_ERROR)
    {
        logBox->AppendText(wxT("Error: Failed to run sending event thread\n"));
        eventSendThread->Delete();
        eventSendThread = nullptr;
        return;
    }

    for (auto window : choosePanel->GetChildren())
    {
        for (auto subWindow : window->GetChildren())
        {
            wxString label = subWindow->GetLabel();
            if (label.IsSameAs("Connect") || label.IsSameAs("Delete"))
                subWindow->Disable();
        }
    }
}

void MyClientFrame::OnDeleteButton(wxCommandEvent &e)
{
    int id = e.GetId();
    wxButton *currentButton = dynamic_cast<wxButton *>(e.GetEventObject());
    
    ipList[id] = wxEmptyString;

    wxPanel *panel = dynamic_cast<wxPanel *>(currentButton->GetParent());
    panel->Destroy();

    choosePanel->FitInside();
    choosePanel->Update();
}

void MyClientFrame::OnDisconnectButton(wxCommandEvent &e)
{
    int id = e.GetId();

    if (screenRecvThread)
    {
        wxCriticalSectionLocker lock(screenRecvcs);
        screenRecvThread->Delete();
        screenRecvThread = nullptr;
    }

    if (eventSendThread)
    {
        wxCriticalSectionLocker lock(eventSendcs);
        eventSendThread->Delete();
        eventSendThread = nullptr;
    }

    logBox->AppendText(wxT("Disconnect successfully\n"));

    wxButton *currentButton = dynamic_cast<wxButton *>(e.GetEventObject());

    wxSizer *sizer = currentButton->GetContainingSizer();
    sizer->Show((size_t)0, true);
    sizer->Show(1, true);
    sizer->Hide(currentButton);
    sizer-Layout();

    if(!IsMaximized())
    {
        SetSize(900, 500);
        Centre();
    }

    showScreenButton->Disable();

    splitter->Unsplit();
    splitter->SplitVertically(choosePanel, inputPanel);
    splitter->SetSashPosition(300);
    splitter->UpdateSize();

    for (auto window : choosePanel->GetChildren())
    {
        for (auto subWindow : window->GetChildren())
        {
            subWindow->Enable();
        }
    }
}

void MyClientFrame::OnClose(wxCloseEvent &e)
{
    WSACleanup();
    Destroy();
}

void MyClientFrame::OnDiscoverServerThreadDestruction()
{
    wxCriticalSectionLocker lock(discoverServercs);
    discoverServerThread = nullptr;
}

void MyClientFrame::OnDiscoverServerThreadUpdate(wxThreadEvent &e)
{
    wxString ip = e.GetString();
    if (ip.IsEmpty()) 
    {
        return;
    }
    for (auto s : ipList)
        if (ip.IsSameAs(s)) 
        {
            return;
        }
    ipList.push_back(ip);  
    
    wxPanel *panel = new wxPanel(choosePanel, ipList.size() - 1);
    panel->SetBackgroundColour(wxColor(150, 150, 150));

    wxStaticText *serverIp = new wxStaticText(panel, wxID_ANY, wxT("Server's IP: ") + ip);

    wxButton *connectButton = new wxButton(panel, ipList.size() - 1, "Connect", wxDefaultPosition, wxSize(100, 25));
    connectButton->Bind(wxEVT_BUTTON, &MyClientFrame::OnConnectButton, this);

    wxButton *deleteButton = new wxButton(panel, ipList.size() - 1, "Delete", wxDefaultPosition, wxSize(100, 25));
    deleteButton->Bind(wxEVT_BUTTON, &MyClientFrame::OnDeleteButton, this);

    wxButton *disconnectButton = new wxButton(panel, ipList.size() - 1, "Disconnect", wxDefaultPosition, wxSize(100, 25));
    disconnectButton->Bind(wxEVT_BUTTON, &MyClientFrame::OnDisconnectButton, this);

    if (isConnect)
    {
        connectButton->Disable();
        deleteButton->Disable();
    }

    wxSizer *childSizer = new wxBoxSizer(wxVERTICAL);
    childSizer->Add(connectButton, 0, wxALIGN_RIGHT | wxLEFT | wxTOP | wxRIGHT, 10);
    childSizer->Add(deleteButton, 0, wxALIGN_RIGHT | wxALL, 10);
    childSizer->Add(disconnectButton, 0, wxALIGN_RIGHT | wxALL, 10);
    childSizer->Hide(disconnectButton);
    
    wxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(serverIp, 1, wxALIGN_CENTER_VERTICAL | wxBOTTOM | wxLEFT | wxTOP, 10);
    sizer->Add(childSizer, 0, wxALIGN_CENTER_VERTICAL);
    
    panel->SetSizerAndFit(sizer);

    chooseSizer->Prepend(panel, 0, wxEXPAND | wxLEFT | wxTOP | wxRIGHT, 10);
    chooseSizer->Layout();

    choosePanel->FitInside();
    choosePanel->Update();
}

void MyClientFrame::OnDiscoverServerComplete(wxThreadEvent &e)
{
    wxString message = e.GetString();
    logBox->AppendText(message);

    discoverButton->Enable();
}

void MyClientFrame::OnScreenRecvThreadDestruction()
{
    wxCriticalSectionLocker lock(screenRecvcs);
    screenRecvThread = nullptr;
}

void MyClientFrame::OnScreenRecvUpdate(wxThreadEvent &e)
{
    wxString message = e.GetString();
    if (message.IsSameAs(wxT("Connect successfully\n")))
    {
        logBox->AppendText(message);
        
        int id = e.GetId();

        wxSizer *sizer;
        for (auto window : choosePanel->GetChildren())
        {
            for (auto subWindow : window->GetChildren())
            {
                wxString label = subWindow->GetLabel();
                if (label.IsSameAs("Connect"))
                    sizer = subWindow->GetContainingSizer();
            }
        }
    
        sizer->Hide((size_t)0);
        sizer->Hide(1);
        sizer->Show(2, true);
        sizer-Layout();

        if(!IsMaximized())
        {
            SetSize(1610, 799);
            Centre();
        }

        showScreenButton->Enable();

        splitter->Unsplit();
        splitter->SplitVertically(choosePanel, displayPanel);
        splitter->SetSashPosition(300);
        splitter->UpdateSize();

        displayWindow->SetFocus();

        return;
    }
        
    wxSize size = displayWindow->GetClientSize();
    std::cout << size.GetWidth() << " " << size.GetHeight() << "\n";

    wxCriticalSectionLocker lock(bcs);
    displayWindow->SetBitmap(bitmap);
}

void MyClientFrame::OnScreenRecvComplete(wxThreadEvent &e)
{
    wxString message = e.GetString();
    logBox->AppendText(message);
    int id = e.GetInt();

    wxSizer* sizer;

    for (auto window : choosePanel->GetChildren())
    {
        for (auto subWindow : window->GetChildren())
        {
            subWindow->Enable();
        }
        if (window->GetId() == id)
            for (auto subWindow : window->GetChildren())
            {
                wxString label = subWindow->GetLabel();
                if (label.IsSameAs("Connect"))
                {
                    sizer = subWindow->GetContainingSizer();
                    break;
                }
            }
    }

    sizer->Show((size_t)0, true);
    sizer->Show(1, true);
    sizer->Hide(2);
    sizer-Layout();

    if(!IsMaximized())
    {
        SetSize(900, 500);
        Centre();
    }

    showScreenButton->Disable();

    splitter->Unsplit();
    splitter->SplitVertically(choosePanel, inputPanel);
    splitter->SetSashPosition(300);
    splitter->UpdateSize();
    
    isConnect = false;
}

void MyClientFrame::OnEventSendThreadDestruction()
{
    wxCriticalSectionLocker lock(eventSendcs);
    eventSendThread = nullptr;
}

void MyClientFrame::OnEventSendUpdate(wxThreadEvent &e)
{
    wxString message = e.GetString();

    logBox->AppendText(message);
    
    int id = e.GetId();

    wxSizer *sizer;
    for (auto window : choosePanel->GetChildren())
    {
        for (auto subWindow : window->GetChildren())
        {
            wxString label = subWindow->GetLabel();
            if (label.IsSameAs("Connect"))
                sizer = subWindow->GetContainingSizer();
        }
    }

    sizer->Hide((size_t)0);
    sizer->Hide(1);
    sizer->Show(2, true);
    sizer-Layout();

    if(!IsMaximized())
    {
        SetSize(1610, 799);
        Centre();
    }

    showScreenButton->Enable();

    splitter->Unsplit();
    splitter->SplitVertically(choosePanel, displayPanel);
    splitter->SetSashPosition(300);
    splitter->UpdateSize();

    displayWindow->SetFocus();
}

void MyClientFrame::OnEventSendComplete(wxThreadEvent &e)
{
    wxString message = e.GetString();
    logBox->AppendText(message);
    int id = e.GetInt();

    wxSizer* sizer;

    for (auto window : choosePanel->GetChildren())
    {
        for (auto subWindow : window->GetChildren())
        {
            subWindow->Enable();
        }
        if (window->GetId() == id)
            for (auto subWindow : window->GetChildren())
            {
                wxString label = subWindow->GetLabel();
                if (label.IsSameAs("Connect"))
                {
                    sizer = subWindow->GetContainingSizer();
                    break;
                }
            }
    }

    sizer->Show((size_t)0, true);
    sizer->Show(1, true);
    sizer->Hide(2);
    sizer-Layout();

    if(!IsMaximized())
    {
        SetSize(900, 500);
        Centre();
    }

    showScreenButton->Disable();

    splitter->Unsplit();
    splitter->SplitVertically(choosePanel, inputPanel);
    splitter->SetSashPosition(300);
    splitter->UpdateSize();

    isConnect = false;
}