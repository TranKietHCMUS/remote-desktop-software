#ifndef CLIENTSOCKETTHREAD_H
#define CLIENTSOCKETTHREAD_H

#include <wx/wx.h>
#include <iostream>
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
        SocketThread(SocketThreadCallback *callback, wxImage &screenImage, wxCriticalSection &sIcs);
        virtual ~SocketThread();

    protected:
        virtual ExitCode Entry();

    private:
        SocketThreadCallback *callback;
        wxImage &screenImage;
        wxCriticalSection &sIcs;

        int i = 1;
};

#endif