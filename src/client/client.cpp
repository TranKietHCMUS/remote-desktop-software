// #include <client.h>

// IMPLEMENT_APP(MyApp)

// bool MyApp::OnInit()
// {
//     MyFrame *MainWindow = new MyFrame(_T("SocketDemo: Client"), wxDefaultPosition, wxSize(1000, 1000));
//     MainWindow->Centre();
//     MainWindow->Show(TRUE);
//     return TRUE;
// }

// BEGIN_EVENT_TABLE(MyFrame, wxFrame)
// EVT_BUTTON(wxID_BUTCONN, MyFrame::FunConnect)
// EVT_SOCKET(wxID_SOCKET, MyFrame::OnSocketEvent)
// END_EVENT_TABLE()

// MyFrame::MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size,
//                long style) : wxFrame(NULL, -1, title, pos, size, style)
// {
//     m_panel = new wxPanel(this, wxID_ANY);
//     wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

//     m_connect_button = new wxButton(m_panel, wxID_BUTCONN, _T("Connect"), wxPoint(10, 20));
//     sizer->Add(m_connect_button, 0, wxALL, 10);

//     m_log_box = new wxTextCtrl(m_panel, wxID_DESC, wxEmptyString, wxPoint(10, 200), wxSize(300, 300), wxTE_MULTILINE | wxTE_BESTWRAP | wxTE_READONLY);
//     sizer->Add(m_log_box, 1, wxEXPAND | wxALL, 10);
    
//     m_panel->SetSizer(sizer);

//     m_log_box->SetValue("Welcome in my SocketDemo: Client\nClient Ready!\n\n");
// }

// void MyFrame::OnTimer(wxTimerEvent& event)
// {
//     wxScreenDC screenDC;
//     wxMemoryDC memDC;

//     int screenWidth = screenDC.GetSize().GetWidth();
//     int screenHeight = screenDC.GetSize().GetHeight();

//     wxBitmap bitmap(screenWidth, screenHeight);
//     memDC.SelectObject(bitmap);
//     memDC.Blit(0, 0, screenWidth, screenHeight, &screenDC, 0, 0);

//     capturedImage = bitmap.ConvertToImage();
// }

// void MyFrame::FunConnect(wxCommandEvent &evt)
// {
//     m_log_box->AppendText(_T("Connecting to the server...\n"));
//     m_connect_button->Enable(FALSE);

//     wxIPV4address adr;
//     adr.Hostname(_T("localhost"));
//     adr.Service(3000);

//     wxSocketClient *Socket = new wxSocketClient();

//     Socket->SetEventHandler(*this, wxID_SOCKET);
//     Socket->SetNotify(wxSOCKET_CONNECTION_FLAG | wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
//     Socket->Notify(TRUE);

//     Socket->Connect(adr, false);
//     m_log_box->AppendText(_T("Connecting...\n"));

//     return;
// }

// void MyFrame::OnSocketEvent(wxSocketEvent &evt)
// {
//     wxSocketBase *Sock = evt.GetSocket();

//     switch (evt.GetSocketEvent())
//     {
//     case wxSOCKET_CONNECTION:
//     {
//         m_log_box->AppendText(_T("OnSocketEvent(wxSOCKET_CONNECTION) Connection successful\n"));
        
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