#ifndef CLIENTSCREENSOCKETTHREAD_H
#define CLIENTSCREENSOCKETTHREAD_H

#include <wx/wx.h>
#include <winsock2.h>

#define SIZE 2764800

class ScreenSocketThreadCallback
{
    public:
        virtual void OnScreenSocketThreadDestruction() = 0;
};

class ScreenSocketThread : public wxThread
{
    public:
        ScreenSocketThread(ScreenSocketThreadCallback *callback, wxString &ip, wxImage &screenImage, wxCriticalSection &sIcs);
        virtual ~ScreenSocketThread();

    protected:
        virtual ExitCode Entry();

    private:
        ScreenSocketThreadCallback *callback;
        wxString &ip;
        wxImage &screenImage;
        wxCriticalSection &sIcs;
};

#endif