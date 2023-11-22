#ifndef SERVER_H
#define SERVER_H

#include <wx/wx.h>
#include <wx/mstream.h>
#include <wx/image.h>
#include <serversocketthread.h>
#pragma comment(lib, "ws2_32.lib")

class MyServerApp : public wxApp
{
public:
    virtual bool OnInit();
};

enum
{
    wxID_BUTTON,
    wxID_TIMER = wxID_HIGHEST,
    wxID_SERVER,
    wxID_SOCKET,
};

class MyServerFrame : public wxFrame, public InputThreadCallback
{
public:
    MyServerFrame(const wxString &title, const wxPoint &pos, const wxSize &size, long style = wxDEFAULT_FRAME_STYLE);
    virtual ~MyServerFrame();

private:
    wxPanel *panel;
    wxButton *allowButton;
    wxTextCtrl *logBox;

    wxTimer *capturingTimer;

    wxImage capturedImage;
    wxCriticalSection cIcs;

    InputThread *inputThread;
    wxCriticalSection iTcs;
    void OnInputThreadDestruction() override;

    void OnClickAllowButton(wxCommandEvent &e);
    // void OnServerConnection(wxSocketEvent &e);
    // void OnServerSocket(wxSocketEvent &e);
    void OnCapturingTimer(wxTimerEvent &e);
    void LayoutServerScreen();
    void OnClose(wxCloseEvent &e);

    wxDECLARE_EVENT_TABLE();
};

#endif