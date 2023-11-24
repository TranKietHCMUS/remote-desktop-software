#ifndef CLIENTSOCKETTHREAD_H
#define CLIENTSOCKETTHREAD_H

#include <wx/wx.h>
#include <iostream>
#include <winsock2.h>

#define SIZE 2764800

class InputThreadCallback
{
public:
    virtual void OnInputThreadDestruction() = 0;
};

class InputThread : public wxThread
{
public:
    InputThread(InputThreadCallback *callback, wxImage &screenImage, wxCriticalSection &sIcs);
    virtual ~InputThread();

protected:
    virtual ExitCode Entry();

private:
    InputThreadCallback *callback;
    wxImage &screenImage;
    wxCriticalSection &sIcs;

    int i = 1;
};

#endif