#ifndef CLIENTSOCKETTHREAD_H
#define CLIENTSOCKETTHREAD_H

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
        InputThread(InputThreadCallback *callback, wxImage &screenImage, wxCriticalSection &sIcs, wxSocketBase *socket);
        virtual ~InputThread();
    protected:
        virtual ExitCode Entry();
    private:
        InputThreadCallback *callback;
        wxImage &screenImage;
        wxCriticalSection &sIcs;
        wxSocketBase *socket;
};

#endif