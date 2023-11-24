#ifndef CLIENT_H
#define CLIENT_H

#include <wx/wx.h>
#include <clientsocketthread.h>
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

class MyClientFrame : public wxFrame, public SocketThreadCallback
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

        wxImage screenImage;
        wxCriticalSection sIcs;

        SocketThread *socketThread;

        DisplayScreenFrame *displayScreenWindow;

        void OnSocketThreadDestruction() override;

        void LayoutClientScreen();
        void OnConnectButton(wxCommandEvent &e);
        void OnDisplayButton(wxCommandEvent &e);
        void OnClose(wxCloseEvent &e);
};

#endif