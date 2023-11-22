#ifndef SERVERSOCKETTHREAH_H
#define SERVERSOCKETTHREAD_H

#include <wx/wx.h>
#include <wx/socket.h>
#include <wx/mstream.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <fstream>
#include <wx/mstream.h>
#include <wx/image.h>

class InputThreadCallback
{
public:
    virtual void OnInputThreadDestruction() = 0;
};

class InputThread : public wxThread
{
public:
    InputThread(InputThreadCallback *callback);
    virtual ~InputThread();

protected:
    virtual ExitCode Entry();

private:
    InputThreadCallback *callback;
};

#endif