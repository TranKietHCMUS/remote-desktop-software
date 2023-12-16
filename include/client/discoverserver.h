#ifndef DISCOVERSERVER_H
#define DISCOVERSERVER_H

#include <wx/wx.h>
#include <wx/socket.h>
#include <winsock2.h>

class DiscoverServerCallback
{
    public:
    virtual void OnDiscoverServerThreadDestruction() = 0;
};

wxDECLARE_EVENT(wxEVT_DISCOVERSERVERTHREAD_UPDATE, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_DISCOVERSERVERTHREAD_COMPLETE, wxThreadEvent);

class DiscoverServerThread : public wxThread
{
    public:
    DiscoverServerThread(DiscoverServerCallback *callback, wxEvtHandler *evtHandler)
                    : callback(callback), evtHandler(evtHandler){}
    virtual ~DiscoverServerThread();

    protected:
    ExitCode Entry() override;

    private:
    DiscoverServerCallback *callback;
    wxEvtHandler *evtHandler;
};

#endif