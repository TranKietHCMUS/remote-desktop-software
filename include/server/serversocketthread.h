#ifndef SERVERSOCKETTHREAH_H
#define SERVERSOCKETTHREAD_H

#include <wx/wx.h>
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
    InputThread(InputThreadCallback *callback, wxImage& capturedImage, wxCriticalSection& cIcs);
    virtual ~InputThread();

protected:
    virtual ExitCode Entry();

private:
    InputThreadCallback *callback;
    wxImage& capturedImage;
    wxCriticalSection& cIcs;
};

#endif