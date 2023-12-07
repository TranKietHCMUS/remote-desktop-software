#ifndef SERVERSCREENSOCKETTHREAH_H
#define SERVERSCREENSOCKETTHREAD_H

#include <wx/wx.h>
#include <winsock2.h>
#include <windows.h>
#include <chrono>

#define SIZE 2764800

class ScreenSocketThreadCallback
{
public:
    virtual void OnScreenSocketThreadDestruction() = 0;
};

class ScreenSocketThread : public wxThread
{
    public:
        ScreenSocketThread(ScreenSocketThreadCallback *callback, wxImage& capturedImage, wxCriticalSection& cIcs);
        virtual ~ScreenSocketThread();

    protected:
        virtual ExitCode Entry();

    private:
        ScreenSocketThreadCallback *callback;
        wxImage& capturedImage;
        wxCriticalSection& cIcs;
};

#endif