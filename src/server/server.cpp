// #include <server.h>

// IMPLEMENT_APP(MyApp)

// bool MyApp::OnInit()
// {
//     MyFrame *MainWindow = new MyFrame(_T("SocketDemo: Server"), wxDefaultPosition, wxSize(300, 300));
//     MainWindow->Center();
//     MainWindow->Show(TRUE);
//     return TRUE;
// }

// BEGIN_EVENT_TABLE(MyFrame, wxFrame)
// EVT_BUTTON(wxID_BUTSTART, MyFrame::ServerStart)
// EVT_BUTTON(wxID_BUTDISP, MyFrame::DisplayScreen)
// EVT_SOCKET(wxID_SERVER, MyFrame::OnServerEvent)
// EVT_SOCKET(wxID_SOCKET, MyFrame::OnSocketEvent)
// END_EVENT_TABLE()

// MyFrame::MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size,
//                long style) : wxFrame(NULL, -1, title, pos, size, style)
// {
//     m_panel = new wxPanel(this, wxID_ANY);

//     wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

//     m_start_button = new wxButton(m_panel, wxID_BUTSTART, _T("Start"), wxPoint(10, 10));
//     sizer->Add(m_start_button, 0, wxALL, 10);
    
//     m_displayscreen_button = new wxButton(m_panel, wxID_BUTDISP, _T("Open Secondary Window"), wxPoint(10, 30));
//     sizer->Add(m_displayscreen_button, 0, wxALL, 10);

//     m_log_box = new wxTextCtrl(m_panel, wxID_DESC, wxEmptyString, wxPoint(10, 50), wxSize(300, 300),
//                                wxTE_MULTILINE | wxTE_BESTWRAP | wxTE_READONLY);
//     sizer->Add(m_log_box, 1, wxEXPAND | wxALL, 10);
        
//     m_panel->SetSizer(sizer);

//     m_log_box->SetValue("Welcome in my SocketDemo: Server\nServer Ready!\n\n");
// }

// void MyFrame::ServerStart(wxCommandEvent &evt)
// {
//     m_start_button->Disable();

//     wxIPV4address adr;
//     adr.Service(3000);

//     m_server = new wxSocketServer(adr);

//     m_server->SetEventHandler(*this, wxID_SERVER);
//     m_server->SetNotify(wxSOCKET_CONNECTION_FLAG);
//     m_server->Notify(true);

//     if (!m_server->Ok())
//     {
//         m_log_box->AppendText(wxT("Could not start server :(\n"));
//         m_start_button->Enable();
//         return;
//     }

//     m_log_box->AppendText(wxT("Server started\n"));

//     return;
// }

// void MyFrame::OnServerEvent(wxSocketEvent &evt)
// {
//     switch (evt.GetSocketEvent())
//     {
//     case wxSOCKET_CONNECTION:
//     {
//         m_log_box->AppendText("OnServerEvent : wxSOCKET_CONNECTION\n");

//         wxSocketBase *Sock = m_server->Accept(true);

//         if (Sock == NULL)
//         {
//             m_log_box->AppendText("Failed to accept incoming connection\n");
//             return;
//         }

//         m_log_box->AppendText("    Accepted Connection\n");
//         Sock->SetEventHandler(*this, wxID_SOCKET);
//         Sock->SetNotify(wxSOCKET_CONNECTION_FLAG | wxSOCKET_INPUT_FLAG | wxSOCKET_OUTPUT_FLAG | wxSOCKET_LOST_FLAG);
//         Sock->Notify(true);
//         break;
//     }
//     default:
//     {
//         m_log_box->AppendText("OnServerEvent : unknown event\n");
//         break;
//     }
//     }
// }

// void MyFrame::OnSocketEvent(wxSocketEvent &evt)
// {
//     wxSocketBase *Sock = evt.GetSocket();

//     switch (evt.GetSocketEvent())
//     {
//     case wxSOCKET_INPUT:
//     {
//         m_log_box->AppendText("OnSocketEvent : wxSOCKET_INPUT\n");

//         wxSocketBase *Sock = evt.GetSocket();
//         char buffer[10];
//         Sock->Read(buffer, sizeof(buffer));

//         char cstring[256];
//         sprintf(cstring, "%c%c%c%c%c%c%c%c%c%c", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5],
//                 buffer[6], buffer[7], buffer[8], buffer[9]);
//         m_log_box->AppendText(wxString("    received ") + cstring + "\n");

//         for (int n = 0; n < 10; n++)
//         {
//             buffer[n] = ('a' + n);
//         }
//         sprintf(cstring, "%c%c%c%c%c%c%c%c%c%c", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5],
//                 buffer[6], buffer[7], buffer[8], buffer[9]);
//         m_log_box->AppendText(wxString("    sending ") + cstring + "\n");

//         Sock->Write(buffer, sizeof(buffer));

//         m_log_box->AppendText("    closing socket\n");
//         Sock->Destroy();
//         break;
//     }
//     case wxSOCKET_LOST:
//     {
//         wxSocketBase *Sock = evt.GetSocket();
//         m_log_box->AppendText("OnSocketEvent : wxSOCKET_LOST\n");
//         Sock->Destroy();
//         break;
//     }
//     case wxSOCKET_OUTPUT:
//     {
//         m_log_box->AppendText("OnSocketEvent : wxSOCKET_OUTPUT\n");
//         break;
//     }
//     default:
//     {
//         m_log_box->AppendText("OnSocketEvent : unknown event\n");
//         break;
//     }
//     }
// }

// void MyFrame::DisplayScreen(wxCommandEvent &evt)
// {
//     MyDisplayScreenFrame *frame = new MyDisplayScreenFrame("Screen Capture", wxPoint(100, 100), wxSize(800, 600), receivedImage);
//     frame->Centre();
//     frame->Show(true);
// }

// BEGIN_EVENT_TABLE(MyDisplayScreenFrame, wxFrame)
// EVT_PAINT(MyDisplayScreenFrame::OnPaint)
// END_EVENT_TABLE()

// MyDisplayScreenFrame::MyDisplayScreenFrame(const wxString& title, const wxPoint& pos, const wxSize& size, wxImage image)
//     : wxFrame(NULL, wxID_ANY, title, pos, size, wxDEFAULT_FRAME_STYLE | wxCLOSE_BOX | wxFULL_REPAINT_ON_RESIZE) 
// { 
//     capturedImage = image;
//     SetBackgroundStyle(wxBG_STYLE_PAINT);

//     timer = new wxTimer(this, wxID_ANY);
//     Bind(wxEVT_TIMER, &MyDisplayScreenFrame::OnTimer, this, wxID_ANY);
//     timer->Start(16);

//     Bind(wxEVT_CLOSE_WINDOW, &MyDisplayScreenFrame::OnClose, this);
// }

// void MyDisplayScreenFrame::OnTimer(wxTimerEvent& event)
// {
//     Refresh();
// }

// void MyDisplayScreenFrame::OnPaint(wxPaintEvent& event)
// {
//     wxPaintDC dc(this); 
//     dc.DrawBitmap(wxBitmap(capturedImage), 0, 0, false);
// }

// void MyDisplayScreenFrame::OnClose(wxCloseEvent& event) 
// {
//     timer->Stop();
//     Destroy();
// }

