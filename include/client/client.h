#ifndef CLIENT_H
#define CLIENT_H

#include <wx/wx.h>
#include <clienteventsocketthread.h>
#include <clientscreensocketthread.h>
#include <displayscreen.h>
#include <winsock2.h>

class MyClientApp : public wxApp
{
    public:
        bool OnInit() override;
};

enum
{
    wxID_TEXT_OPEN,
    wxID_TEXT_NOTE,
    wxID_DISCONNECT_BUTTON,
    wxID_DISPLAY_BUTTON,
    wxID_TEXT_INPUT,
    wxID_TEXT_LOG,
};

class MyClientFrame : public wxFrame, public ScreenSocketThreadCallback, public EventSocketThreadCallback
{
    public:
        MyClientFrame(const wxString &title, const wxPoint &pos, const wxSize &size, long style = wxDEFAULT_FRAME_STYLE);
        virtual ~MyClientFrame();

    private:
        wxPanel *panel;
        wxStaticText *openText;
        wxStaticText *note;
        wxButton *disconnectButton;
        wxButton *displayButton;
        wxTextCtrl *inputBox;
        wxTextCtrl *logBox;
        wxBoxSizer *sizer;

        wxString ip;

        wxBitmap bitmap;
        wxCriticalSection bcs;

        std::queue<msg> msgQueue;
        wxCriticalSection mQcs;

        WSADATA wsaData;

        ScreenSocketThread *screenSocketThread;
        EventSocketThread *eventSocketThread;

        wxCriticalSection threadCs;

        DisplayScreenFrame *displayScreenWindow;

        void OnScreenSocketThreadDestruction() override;
        void OnEventSocketThreadDestruction() override;

        void OnTextEnter(wxCommandEvent &e);
        void OnScreenThreadCompletion(wxThreadEvent &e);
        void OnDisplayButton(wxCommandEvent &e);
        void OnDisconnectButton(wxCommandEvent &e);
        //void OnScreenClose(wxCloseEvent &e);
        void OnClose(wxCloseEvent &e);
};

#endif