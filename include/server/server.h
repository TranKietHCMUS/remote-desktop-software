#ifndef SERVER_H
#define SERVER_H

#include <wx/wx.h>
#include <servereventsocketthread.h>
#include <serverscreensocketthread.h>
#include <winsock2.h>

class MyServerApp : public wxApp
{
public:
    bool OnInit() override;
};

enum
{
    wxID_BUTTON,
    wxID_TIMER = wxID_HIGHEST,
    wxID_TIMER_EVENT,
};

class MyServerFrame : public wxFrame, public ScreenSocketThreadCallback, public EventSocketThreadCallback
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

        std::queue<msg> msgQueue;
        wxCriticalSection mQcs;

        wxTimer *eventTimer;

        WSADATA wsaData;

        ScreenSocketThread *screenSocketThread;
        EventSocketThread *eventSocketThread;

        void OnScreenSocketThreadDestruction() override;
        void OnEventSocketThreadDestruction() override;
        
        void OnClickAllowButton(wxCommandEvent &e);
        void OnCapturingTimer(wxTimerEvent &e);
        void OnEventTimer(wxTimerEvent& e);
        void LayoutServerScreen();
        void OnClose(wxCloseEvent &e);
};

#endif