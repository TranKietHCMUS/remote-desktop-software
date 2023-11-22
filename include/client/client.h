#ifndef CLIENT_H
#define CLIENT_H

#include <wx/wx.h>
#include <wx/socket.h>
#include <displayscreen.h>
#include <clientsocketthread.h>

#pragma comment(lib, "Ws2_32.lib")

class MyClientApp : public wxApp
{
public:
    bool OnInit() override;
};

enum
{
    wxID_CONNECT_BUTTON,
    wxID_DISPLAY_BUTTON,
    wxID_STOP_BUTTON,
    wxID_TIMER = wxID_HIGHEST + 1,
    wxID_SOCKET,
};

class MyClientFrame : public wxFrame, public InputThreadCallback
{
public:
    MyClientFrame(const wxString &title, const wxPoint &pos, const wxSize &size, long style = wxDEFAULT_FRAME_STYLE);
    virtual ~MyClientFrame();

private:
    bool stop;
    wxPanel *panel;
    wxButton *connectButton;
    wxButton *displayButton;
    wxButton *stopButton;
    wxTextCtrl *logBox;

    wxImage dataScreenImage;
    wxTimer *updatingScreenTimer;

    wxImage screenImage;
    wxCriticalSection sIcs;

    InputThread *inputThread;
    wxCriticalSection iTcs;
    void OnInputThreadDestruction() override;

    void LayoutClientScreen();
    void OnUpdatingScreenTimer(wxTimerEvent &e);
    void OnConnectButton(wxCommandEvent &e);
    void StopButton(wxCommandEvent &e);
    void OnDisplayButton(wxCommandEvent &e);
    // void OnClientSocket(wxSocketEvent &e);
    // void OnClose(wxCloseEvent &e);

    wxDECLARE_EVENT_TABLE();
};

#endif