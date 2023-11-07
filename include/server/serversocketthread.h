#ifndef SERVERSOCKETTHREAH_H
#define SERVERSOCKETTHREAD_H

#include <wx/wx.h>
#include <wx/socket.h>
#include <wx/mstream.h>

class InputThreadCallback
{
    public:
        virtual void OnInputThreadDestruction() = 0;
};

class InputThread : public wxThread
{
    public:
        InputThread(InputThreadCallback *callback, wxImage &capturedImage, wxCriticalSection &cIcs, wxSocketBase *socket);
        virtual ~InputThread();
    protected:
        virtual ExitCode Entry();
    private:
        InputThreadCallback *callback;
        wxImage &capturedImage;
        wxCriticalSection &cIcs;
        wxSocketBase *socket;
};

#endif