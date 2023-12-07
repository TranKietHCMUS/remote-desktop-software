#ifndef CLIENTSCREENSOCKETTHREAD_H
#define CLIENTSCREENSOCKETTHREAD_H

#include <wx/wx.h>
#include <winsock2.h>
#include <chrono>

#define SIZE 2764800

wxDECLARE_EVENT(wxEVT_SCREENSOCKETTHREAD_COMPLETED, wxThreadEvent);

class ScreenSocketThreadCallback
{
    public:
        virtual void OnScreenSocketThreadDestruction() = 0;
};

class ScreenSocketThread : public wxThread
{
    public:
        ScreenSocketThread(ScreenSocketThreadCallback *callback, wxEvtHandler *evtHandler, wxString &ip, wxBitmap &bitmap, wxCriticalSection &bcs);
        virtual ~ScreenSocketThread();

    protected:
        virtual ExitCode Entry();

    private:
        ScreenSocketThreadCallback *callback;
        wxEvtHandler *evtHandler;
        wxString &ip;
        wxBitmap &bitmap;
        wxCriticalSection &bcs;
};

#endif