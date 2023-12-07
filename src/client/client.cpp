#include <client.h>

wxIMPLEMENT_APP(MyClientApp);

bool MyClientApp::OnInit()
{
    MyClientFrame *myClientWindow = new MyClientFrame(wxT("Client Window"), wxDefaultPosition, wxSize(300, 125));
    myClientWindow->Show(true);
    myClientWindow->Centre();
    return true;
}

MyClientFrame::MyClientFrame(const wxString &title, const wxPoint &pos, const wxSize &size, long style)
    : wxFrame(NULL, wxID_ANY, title, pos, size, style), bitmap(1280, 720)
{
    panel = new wxPanel(this, wxID_ANY);

    openText = new wxStaticText(panel, wxID_TEXT_OPEN, wxT("Enter IP of remote computer:"));
    wxFont fontOpen = openText->GetFont();
    fontOpen.Scale(1.2);
    fontOpen.SetFamily(wxFONTFAMILY_ROMAN);
    openText->SetFont(fontOpen);

    inputBox = new wxTextCtrl(panel, wxID_TEXT_INPUT, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                            wxTE_LEFT | wxTE_PROCESS_ENTER | wxTE_BESTWRAP);
    wxFont fontInput = inputBox->GetFont();
    fontInput.SetFamily(wxFONTFAMILY_ROMAN);
    fontInput.Scale(1.4);
    fontInput.MakeBold();
    inputBox->SetFont(fontInput);
    
    note = new wxStaticText(panel, wxID_TEXT_NOTE, wxT("Press 'Enter' to connect!"));
    wxFont fontNote = note->GetFont();
    fontNote.SetFamily(wxFONTFAMILY_ROMAN);
    fontNote.MakeItalic();
    note->SetFont(fontNote);

    Bind(wxEVT_TEXT_ENTER, &MyClientFrame::OnTextEnter, this, wxID_TEXT_INPUT);
    
    sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(openText, 0, wxLEFT | wxTOP | wxRIGHT, 10);
    sizer->Add(inputBox, 1, wxEXPAND | wxLEFT | wxTOP | wxRIGHT, 10);
    sizer->Add(note, 0, wxLEFT | wxBOTTOM | wxRIGHT, 10);
    panel->SetSizer(sizer);

    ip = wxEmptyString;
    
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        wxMessageBox(wxT("Failed to initialize Winsock!"));

    screenSocketThread = nullptr;
    eventSocketThread = nullptr;

    Bind(wxEVT_SCREENSOCKETTHREAD_COMPLETED, &MyClientFrame::OnScreenThreadCompletion, this);

    Bind(wxEVT_CLOSE_WINDOW, &MyClientFrame::OnClose, this);
}

MyClientFrame::~MyClientFrame() {}

void MyClientFrame::OnScreenSocketThreadDestruction()
{
    wxCriticalSectionLocker lock(threadCs);
    screenSocketThread = nullptr;
}

void MyClientFrame::OnEventSocketThreadDestruction()
{
    wxCriticalSectionLocker lock(threadCs);
    eventSocketThread = nullptr;
}

void MyClientFrame::OnTextEnter(wxCommandEvent& e)
{
    ip = e.GetString();
    inputBox->SetValue(wxEmptyString);

    screenSocketThread = new ScreenSocketThread(this, this, ip, bitmap, bcs);
    if (screenSocketThread->Run() != wxTHREAD_NO_ERROR)
        delete screenSocketThread;

    eventSocketThread = new EventSocketThread(this, ip, msgQueue, mQcs);
    if (eventSocketThread->Run() != wxTHREAD_NO_ERROR)
        delete eventSocketThread;

    disconnectButton = new wxButton(panel, wxID_DISCONNECT_BUTTON, wxT("Disconnect"), wxPoint(10, 10));
    disconnectButton->Bind(wxEVT_BUTTON, &MyClientFrame::OnDisconnectButton, this, wxID_DISCONNECT_BUTTON);

    displayButton = new wxButton(panel, wxID_DISPLAY_BUTTON, wxT("Display Screen"));
    displayButton->Bind(wxEVT_BUTTON, &MyClientFrame::OnDisplayButton, this, wxID_DISPLAY_BUTTON);
    
    logBox = new wxTextCtrl(panel, wxID_TEXT_LOG, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                            wxTE_MULTILINE | wxTE_BESTWRAP | wxTE_READONLY);

    inputBox->Disable();
    sizer->Hide(openText);
    sizer->Hide(inputBox);
    sizer->Hide(note);
    sizer->Add(disconnectButton, 0, wxLEFT | wxTOP | wxRIGHT, 10);
    sizer->Add(displayButton, 0, wxLEFT | wxTOP | wxRIGHT, 10);
    sizer->Add(logBox, 1, wxEXPAND| wxALL, 10);
    sizer->Layout();

    displayScreenWindow = new DisplayScreenFrame(wxT("Display Screen Window"), wxPoint(100, 100), wxSize(1286, 749), bitmap, bcs, msgQueue, mQcs);
    displayScreenWindow->Show(true);

    SetPosition(wxPoint(1496, 100));
    SetSize(400, 749);

    //displayScreenWindow->Bind(wxEVT_CLOSE_WINDOW, &MyClientFrame::OnScreenClose, this);
}

void MyClientFrame::OnScreenThreadCompletion(wxThreadEvent &e)
{
    wxString announcement = e.GetString();
    logBox->AppendText(announcement);
}

void MyClientFrame::OnDisplayButton(wxCommandEvent &e)
{
    displayScreenWindow->Show(!displayScreenWindow->IsShown());
}

void MyClientFrame::OnDisconnectButton(wxCommandEvent &e)
{
    disconnectButton->Disable();
    displayButton->Disable();
    
    //displayScreenWindow->Unbind(wxEVT_CLOSE_WINDOW, &MyClientFrame::OnScreenClose, this);
    
    // inputBox->Enable();
    // sizer->Remove(3);
    // sizer->Remove(3);
    // sizer->Remove(3);
    // sizer->Show(openText);
    // sizer->Show(inputBox);
    // sizer->Show(note);
    // sizer->Layout();
    // disconnectButton->Destroy();
    // displayButton->Destroy();
    // logBox->Destroy();

    {
        wxCriticalSectionLocker lock(threadCs);
        if (screenSocketThread) 
        {
            screenSocketThread->Delete();
            screenSocketThread = nullptr;
        }

        if (eventSocketThread)
        {
            eventSocketThread->Delete();
            eventSocketThread = nullptr;
        }
    }  

    // SetSize(wxSize(300, 125));
    // Centre();

    displayScreenWindow->OnClose();
    displayScreenWindow = nullptr;
}

// void MyClientFrame::OnScreenClose(wxCloseEvent& e)
// {
//     displayScreenWindow->Unbind(wxEVT_CLOSE_WINDOW, &MyClientFrame::OnScreenClose, this);
    
//     // inputBox->Enable();
//     // sizer->Remove(3);
//     // sizer->Remove(3);
//     // sizer->Remove(3);
//     // sizer->Show(openText);
//     // sizer->Show(inputBox);
//     // sizer->Show(note);
//     // sizer->Layout();
//     // disconnectButton->Destroy();
//     // displayButton->Destroy();
//     // logBox->Destroy();

//     {
//         wxCriticalSectionLocker lock(threadCs);
//         if (screenSocketThread) 
//         {
//             screenSocketThread->Delete();
//             screenSocketThread = nullptr;
//         }

//         if (eventSocketThread)
//         {
//             eventSocketThread->Delete();
//             eventSocketThread = nullptr;
//         }
//     } 

//     // SetSize(wxSize(300, 125));
//     // Centre();

//     displayScreenWindow->OnClose();
//     displayScreenWindow = nullptr;
// }

void MyClientFrame::OnClose(wxCloseEvent& e)
{   
    if (displayScreenWindow)
        displayScreenWindow->OnClose();
    WSACleanup();
    Destroy();
}
