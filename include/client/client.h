#ifndef CLIENT_H
#define CLIENT_H

#include <wx/wx.h>
#include <clienteventsocketthread.h>
#include <clientscreensocketthread.h>
#include <displayscreen.h>

class MyClientApp : public wxApp
{
    public:
        bool OnInit() override;
};

enum
{
    wxID_CONNECT_BUTTON,
    wxID_DISPLAY_BUTTON,
};

class MyClientFrame : public wxFrame, public ScreenSocketThreadCallback, public EventSocketThreadCallback
{
    public:
        MyClientFrame(const wxString &title, const wxPoint &pos, const wxSize &size, long style = wxDEFAULT_FRAME_STYLE);
        virtual ~MyClientFrame();

    private:
        wxPanel *panel;
        wxButton *connectButton;
        wxButton *displayButton;
        wxButton *stopButton;
        wxTextCtrl *logBox;

        wxImage screenImage;
        wxCriticalSection sIcs;

        std::queue<msg> msgQueue;
        wxCriticalSection mQcs;

        WSADATA wsaData;

        ScreenSocketThread *screenSocketThread;
        EventSocketThread *eventSocketThread;

        DisplayScreenFrame *displayScreenWindow;

        void OnScreenSocketThreadDestruction() override;
        void OnEventSocketThreadDestruction() override;

        void LayoutClientScreen();
        void OnConnectButton(wxCommandEvent &e);
        void OnDisplayButton(wxCommandEvent &e);
        void OnClose(wxCloseEvent &e);
};

#endif