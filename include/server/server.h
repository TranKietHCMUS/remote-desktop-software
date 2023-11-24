#ifndef SERVER_H
#define SERVER_H

#include <wx/wx.h>
#include <serversocketthread.h>

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

    void OnInputThreadDestruction() override;

    void OnClickAllowButton(wxCommandEvent &e);
    void OnCapturingTimer(wxTimerEvent &e);
    void LayoutServerScreen();
    void OnClose(wxCloseEvent &e);
};

#endif