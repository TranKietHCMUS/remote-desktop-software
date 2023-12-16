#ifndef ALLOWDISCOVER_H
#define ALLOWDISCOVERSERVER_H

#include <wx/wx.h>
#include <winsock2.h>

class AllowDiscoverCallback
{
    public:
    virtual void OnAllowDiscoverThreadDestruction() = 0;
};

wxDECLARE_EVENT(wxEVT_ALLOWDISCOVERTHREAD_COMPLETE, wxThreadEvent);

class AllowDiscoverThread : public wxThread
{
    public:
    AllowDiscoverThread(AllowDiscoverCallback *callback, wxEvtHandler *evtHandler)
                    : callback(callback), evtHandler(evtHandler){}
    virtual ~AllowDiscoverThread();

    protected:
    ExitCode Entry() override;

    private:
    AllowDiscoverCallback *callback;
    wxEvtHandler *evtHandler;
};

#endif