#ifndef SERVERSOCKETTHREAH_H
#define SERVERSOCKETTHREAD_H

#include <wx/wx.h>
#include <winsock2.h>

#define SIZE 2764800

class SocketThreadCallback
{
public:
    virtual void OnSocketThreadDestruction() = 0;
};

class SocketThread : public wxThread
{
    public:
        SocketThread(SocketThreadCallback *callback, wxImage& capturedImage, wxCriticalSection& cIcs);
        virtual ~SocketThread();

    protected:
        virtual ExitCode Entry();

    private:
        SocketThreadCallback *callback;
        wxImage& capturedImage;
        wxCriticalSection& cIcs;
};

#endif